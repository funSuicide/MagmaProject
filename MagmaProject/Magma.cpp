#include "Magma.h"
#include <iostream>
#include <cmath>
#include <algorithm>
#include <fstream>
#include <omp.h>

Magma::Magma(uint8_t* key) {
	memcpy(this->key, key, 32);
	expandKeys(this->roundKeys);
}

halfVector Magma::xOR(halfVector& src1, halfVector& src2) { 
	uint32_t tmp = src1.vector ^ src2.vector;
	return halfVector(tmp);
}

halfVector Magma::mod32(halfVector& src, halfVector& key) { 
	uint32_t tmp = src.vector + key.vector;
	return halfVector(tmp);
}

halfVector Magma::transformationT(halfVector& src) { 
	uint32_t tmp;
	uint8_t leftHalfByte, rightHalfByte;
	for (int i = 0; i < 4; i++) {
		leftHalfByte = src.bytes[i] & 0x0f;
		rightHalfByte = (src.bytes[i] & 0xf0) >> 4;
		leftHalfByte = tTable[i * 2][leftHalfByte];
		rightHalfByte = tTable[i * 2 + 1][rightHalfByte];
		tmp = (rightHalfByte << 4) | leftHalfByte;
	}
	return halfVector(tmp);
}

void Magma::expandKeys(halfVector* dest) { 
	int q = 0;
	for (int i = 0; i < 3; i++) {
		int step = 0;
		for (int j = 7; j >= 0; j--) {
			memcpy(dest[j+q].bytes, key + step, 4);
			step += 4;
		}
		q += 8;
	}
	int step = 28;
	for (int j = 31; j >= 24; j--) {
		memcpy(dest[j].bytes, key + step, 4);
		step -= 4;
	}
}

halfVector Magma::gTransformation(halfVector& key, halfVector& half) { 
	uint32_t byteVector;
	uint32_t result;
	halfVector tmp = mod32(half, key);
	tmp = transformationT(tmp);
	byteVector = tmp.bytes[3];
	for (int i = 2; i >= 0; i--) {
		byteVector = (byteVector << 8) + tmp.bytes[i];
	}
	byteVector = (byteVector << 11) | (byteVector >> 21);
	result = byteVector;
	result = byteVector >> 8;
	result = byteVector >> 16;
	result = byteVector >> 24;
	return halfVector(result);
}

byteVector Magma::transformationG(byteVector& src, halfVector& key) {
	halfVector l = src.left;
	halfVector r = src.right;
	halfVector gResult = gTransformation(key, l);
	halfVector tmp = xOR(gResult, r);
	return byteVector(tmp, l);
}

byteVector Magma::encryptBlock(byteVector& src) { 
	byteVector tmp = src;
	for (int i = 0; i < 31; i++) {
		tmp = transformationG(tmp, roundKeys[i]);
	}
	halfVector l = tmp.left;
	halfVector r = tmp.right;
	halfVector gResult = gTransformation(roundKeys[31], l);
	halfVector tmp2 = xOR(gResult, r);
	return byteVector(l, tmp2);
}

byteVector Magma::decryptBlock(byteVector& src) { 
	byteVector tmp = src;
	for (int i = 31; i > 0; i--) {
		tmp = transformationG(tmp, roundKeys[i]);
	}
	halfVector l = tmp.left;
	halfVector r = tmp.right;
	halfVector gResult = gTransformation(roundKeys[0], l);
	halfVector tmp2 = xOR(gResult, r);
	return byteVector(l, tmp2);
}

void Magma::encryptText(uint8_t* data, char* dest) {
	#pragma omp parallel for num_threads(8)
	for (int i = 0; i < 1048576-8; i+=8){
		halfVector left;
		halfVector right;
		memcpy(left.bytes, data + i, 4);
		memcpy(right.bytes, data + 4 + i, 4);
		byteVector block;
		block.left = left;
		block.right = right;
		byteVector chiperBlock = encryptBlock(block);
		memcpy(dest + i, chiperBlock.left.bytes, 4);
		memcpy(dest + 4  + i, chiperBlock.right.bytes, 4);
	}
}

void Magma::decryptText(uint8_t* data, char* dest) {
	#pragma omp parallel for num_threads(8)
	for (int i = 0; i < 1048576-8; i +=8) {
		halfVector left;
		halfVector right;
		memcpy(left.bytes, data + i, 4);
		memcpy(right.bytes, data + 4 + i, 4);
		byteVector block;
		block.left = left;
		block.right = right;
		byteVector dechiperBlock = decryptBlock(block);
		memcpy(dest + i, dechiperBlock.left.bytes, 4);
		memcpy(dest + 4 + i, dechiperBlock.right.bytes, 4);
	}
}
