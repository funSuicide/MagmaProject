#include "Magma.h"
#include <iostream>
#include <omp.h>

Magma::Magma(key& key) {
	expandKeys(key);
}

halfVector Magma::xOR(halfVector& src1, halfVector& src2) const { 
	uint32_t tmp = src1.vector ^ src2.vector;
	return halfVector(tmp);
}

halfVector Magma::mod32(halfVector& src, halfVector& key) const { 
	uint32_t tmp = src.vector + key.vector;
	return halfVector(tmp);
}

halfVector Magma::transformationT(halfVector& src) const { 
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

void Magma::expandKeys(key& key){ 
	int q = 0;
	for (int i = 0; i < 3; i++) {
		int step = 0;
		for (int j = 7; j >= 0; j--) {
			std::copy(key.bytes + step, key.bytes+step+4, roundKeys[j + q].bytes);
			step += 4;
		}
		q += 8;
	}
	int step = 28;
	for (int j = 31; j >= 24; j--) {
		std::copy(key.bytes + step, key.bytes + step + 4, roundKeys[j].bytes);
		step -= 4;
	}
}

halfVector Magma::gTransformation(halfVector& key, halfVector& half) const { 
	halfVector tmp = mod32(half, key);
	tmp = transformationT(tmp);
	tmp.vector = (tmp.vector << 11) | (tmp.vector >> 21);
	return tmp;
}

byteVector Magma::transformationG(byteVector& src, halfVector& key) const {
	halfVector gResult = gTransformation(key, src.left);
	halfVector tmp = xOR(gResult, src.right);
	return byteVector(tmp, src.left);
}

byteVector Magma::encryptBlock(byteVector& src) { 
	byteVector tmp = src;
	for (int i = 0; i < 31; i++) {
		tmp = transformationG(tmp, roundKeys[i]);
	}
	halfVector gResult = gTransformation(roundKeys[31], tmp.left);
	halfVector tmp2 = xOR(gResult, tmp.right);
	return byteVector(tmp.left, tmp2);
}

byteVector Magma::decryptBlock(byteVector& src) { //-
	byteVector tmp = src;
	for (int i = 31; i > 0; i--) {
		tmp = transformationG(tmp, roundKeys[i]);
	}
	halfVector gResult = gTransformation(roundKeys[0], tmp.left);
	halfVector tmp2 = xOR(gResult, tmp.right);
	return byteVector(tmp.left, tmp2);
}

void Magma::encryptText(uint8_t* data, char* dest, int size){
	#pragma omp parallel for num_threads(8)
	for (int i = 0; i < size; i+=8){
		halfVector left = (uint32_t)i;
		halfVector right = (uint32_t)i;
		byteVector block(left, right);
		byteVector chiperBlock = encryptBlock(block);
		memcpy((void*)&block, data + i, 8);
		block.left.vector ^= chiperBlock.left.vector;
		block.right.vector ^= chiperBlock.right.vector;
		memcpy(dest + i, (void*)&block, 8);
	}
}

/*
void Magma::decryptText(uint8_t* data, char* dest, int size) {
	#pragma omp parallel for num_threads(8)
	for (int i = 0; i < size; i +=8) {
		halfVector left = 
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
*/