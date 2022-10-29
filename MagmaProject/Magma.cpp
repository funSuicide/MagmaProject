#include "Magma.h"
#include <iostream>
#include <cmath>
#include <algorithm>
#include <fstream>
#include <omp.h>

Magma::Magma(uint8_t* key) {
	memcpy(this->key, key, 32);
	roundKeys = expandKeys();
}

halfVector Magma::xOR(halfVector& src1, halfVector& src2) { 
	halfVector result;
	result.vector = src1.vector ^ src2.vector;
	return result;
}

halfVector Magma::mod32(halfVector& src, halfVector& key) { 
	halfVector result;
	result.vector = src.vector + key.vector;
	return result;
}


halfVector Magma::transformationT(halfVector& src) { 
	halfVector result;
	uint8_t leftHalfByte, rightHalfByte;
	for (int i = 0; i < 4; i++) {
		leftHalfByte = src.bytes[i] & 0x0f;
		rightHalfByte = (src.bytes[i] & 0xf0) >> 4;
		leftHalfByte = tTable[i * 2][leftHalfByte];
		rightHalfByte = tTable[i * 2 + 1][rightHalfByte];
		result.bytes[i] = (rightHalfByte << 4) | leftHalfByte;
	}
	return result;
}

halfVector* Magma::expandKeys() { 
	halfVector* roundKeys = new halfVector[32];
	int q = 0;
	for (int i = 0; i < 3; i++) {
		int step = 0;
		for (int j = 7; j >= 0; j--) {
			memcpy(roundKeys[j+q].bytes, key + step, 4);
			step += 4;
		}
		q += 8;
	}
	int step = 28;
	for (int j = 31; j >= 24; j--) {
		memcpy(roundKeys[j].bytes, key + step, 4);
		step -= 4;
	}
	return roundKeys;
}

halfVector Magma::gTransformation(halfVector& key, halfVector& half) { 
	halfVector result;
	uint32_t byteVector;
	halfVector tmp = mod32(half, key);
	tmp = transformationT(tmp);
	byteVector = tmp.bytes[3];
	for (int i = 2; i >= 0; i--) {
		byteVector = (byteVector << 8) + tmp.bytes[i];
	}
	byteVector = (byteVector << 11) | (byteVector >> 21);
	result.bytes[0] = byteVector;
	result.bytes[1] = byteVector >> 8;
	result.bytes[2] = byteVector >> 16;
	result.bytes[3] = byteVector >> 24;
	return result;
}

byteVector Magma::transformationG(byteVector& src, halfVector& key) {
	halfVector l = src.left;
	halfVector r = src.right;
	byteVector result;
	halfVector gResult = gTransformation(key, l);
	halfVector tmp = xOR(gResult, r);
	result.left = tmp;
	result.right = l;
	return result;
}

byteVector Magma::encryptBlock(byteVector& src, halfVector* roundKeys) { 
	byteVector tmp = src;
	byteVector result;
	for (int i = 0; i < 31; i++) {
		tmp = transformationG(tmp, roundKeys[i]);
	}
	halfVector l = tmp.left;
	halfVector r = tmp.right;
	halfVector gResult = gTransformation(roundKeys[31], l);
	halfVector tmp2 = xOR(gResult, r);
	result.left = l;
	result.right = tmp2;
	return result;
}

byteVector Magma::decryptBlock(byteVector& src, halfVector* roundKeys) { 
	byteVector tmp = src;
	byteVector result;
	for (int i = 31; i > 0; i--) {
		tmp = transformationG(tmp, roundKeys[i]);
	}
	halfVector l = tmp.left;
	halfVector r = tmp.right;
	halfVector gResult = gTransformation(roundKeys[0], l);
	halfVector tmp2 = xOR(gResult, r);
	result.left = l;
	result.right = tmp2;
	return result;
}

uint8_t* Magma::encryptText(uint8_t* data) {
	uint8_t* result = new uint8_t[1048576];
	#pragma omp parallel for num_threads(8)
	for (int i = 0; i < 1048576-8; i+=8){
		halfVector left;
		halfVector right;
		memcpy(left.bytes, data + i, 4);
		memcpy(right.bytes, data + 4 + i, 4);
		byteVector block;
		block.left = left;
		block.right = right;
		byteVector chiperBlock = encryptBlock(block, roundKeys);
		memcpy(result + i, chiperBlock.left.bytes, 4);
		memcpy(result + 4  + i, chiperBlock.right.bytes, 4);
	}
	return result;
}

uint8_t* Magma::decryptText(uint8_t* data) {
	uint8_t* result = new uint8_t[1048576];
	#pragma omp parallel for num_threads(8)
	for (int i = 0; i < 1048576-8; i +=8) {
		halfVector left;
		halfVector right;
		memcpy(left.bytes, data + i, 4);
		memcpy(right.bytes, data + 4 + i, 4);
		byteVector block;
		block.left = left;
		block.right = right;
		byteVector dechiperBlock = decryptBlock(block, roundKeys);
		memcpy(result + i, dechiperBlock.left.bytes, 4);
		memcpy(result + 4 + i, dechiperBlock.right.bytes, 4);
	}
	return result;
}
