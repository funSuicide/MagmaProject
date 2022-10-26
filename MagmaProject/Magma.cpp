#include "Magma.h"
#include <iostream>
#include <cmath>
#include <algorithm>
#include <fstream>


halfVector Magma::xOR(halfVector& src1, halfVector& src2) { //good
	halfVector result;
	for (int i = 0; i < 4; i++) {
		result.bytes[i] = src1.bytes[i] ^ src2.bytes[i];
	}
	return result;
}

halfVector Magma::mod32(halfVector& src, halfVector& key) { //good
	halfVector result;
	unsigned int internal = 0;
	for (int i = 3; i >= 0; i--)
	{
		internal = src.bytes[i] + key.bytes[i] + (internal >> 8);
		result.bytes[i] = internal & 0xff;
	}
	return result;
}

halfVector Magma::transformationT(halfVector& src) { //good
	halfVector result;
	uint8_t leftHalfByte, rightHalfByte;
	for (int i = 0; i < 4; i++) {
		leftHalfByte = (src.bytes[i] & 0xf0) >> 4;
		rightHalfByte = src.bytes[i] & 0x0f;
		leftHalfByte = tTable[i * 2][leftHalfByte];
		rightHalfByte = tTable[i * 2 + 1][rightHalfByte];
		result.bytes[i] = (leftHalfByte << 4) | rightHalfByte;
	}
	return result;
}

halfVector* Magma::expandKeys() { //need testing
	halfVector* roundKeys = new halfVector[32];
	int q = 0;
	for (int i = 0; i < 3; i++) {
		int step = 0;
		for (int j = 0; j < 8; j++) {
			memcpy(roundKeys[j+q].bytes, key + step, 4);
			step += 4;
		}
		q += 8;
	}
	int step = 28;
	for (int j = 24; j < 32; j++) {
		memcpy(roundKeys[j].bytes, key + step, 4);
		step -= 4;
	}
	return roundKeys;
}

halfVector Magma::gTransformation(halfVector& key, halfVector& half) { //need testing
	halfVector result;
	uint32_t byteVector;
	halfVector tmp = mod32(half, key);
	tmp = transformationT(tmp);
	byteVector = tmp.bytes[0];
	for (int i = 1; i < 4; i++) {
		byteVector = (byteVector << 8) + tmp.bytes[i];
	}
	byteVector = (byteVector << 11) | (byteVector >> 21);
	result.bytes[3] = byteVector;
	result.bytes[2] = byteVector >> 8;
	result.bytes[1] = byteVector >> 16;
	result.bytes[0] = byteVector >> 24;
	return result;
}

byteVector Magma::transformationG(byteVector& src, halfVector& key) { //need testing
	byteVector result;
	halfVector gResult = gTransformation(key, src.right);
	halfVector tmp = xOR(gResult, src.left);
	result.left = src.right;
	result.right = tmp;
	return result;
}

byteVector Magma::encryptBlock(byteVector& src) { //need testing
	halfVector* roundKeys = expandKeys();
	byteVector tmp = src;
	byteVector result;
	for (int i = 0; i < 31; i++) {
		tmp = transformationG(tmp, roundKeys[i]);
	}
	halfVector gResult = gTransformation(roundKeys[31], tmp.right);
	halfVector tmp2 = xOR(gResult, tmp.left);
	result.left = tmp2;
	result.right = tmp.right;
	return result;
}

byteVector Magma::decryptBlock(byteVector& src) { //need testing
	halfVector* roundKeys = expandKeys();
	byteVector tmp = src;
	byteVector result;
	for (int i = 31; i > 0; i--) {
		tmp = transformationG(tmp, roundKeys[i]);
	}
	halfVector gResult = gTransformation(roundKeys[0], tmp.right);
	halfVector tmp2 = xOR(gResult, tmp.left);
	result.left = tmp2;
	result.right = tmp.right;
	return result;
}


void Magma::encryptText() {
	std::ifstream in(path1, std::ios::binary);
	std::ofstream out(path2, std::ios::binary);
	char* tmp = new char[8];
	while (in.read(tmp, 8)) {
		halfVector left;
		halfVector right;
		for (int i = 0; i < 4; i++) {
			left.bytes[i] = tmp[i];
		}
		for (int j = 4; j < 8; j++) {
			right.bytes[j-4] = tmp[j];
		}
		byteVector block;
		block.left = left;
		block.right = right;
		byteVector chiperBlock = encryptBlock(block);
		out.write((const char*)chiperBlock.left.bytes, 4);
		out.write((const char*)chiperBlock.right.bytes, 4);
	}
	out.close();
	in.close();
}

void Magma::decryptText() {
	std::ifstream in(path1, std::ios::binary);
	std::ofstream out(path2, std::ios::binary);
	char* tmp = new char[8];
	while (in.read(tmp, 8)) {
		halfVector left;
		halfVector right;
		for (int i = 0; i < 4; i++) {
			left.bytes[i] = tmp[i];
		}
		for (int j = 4; j < 8; j++) {
			right.bytes[j - 4] = tmp[j];
		}
		byteVector block;
		block.left = left;
		block.right = right;
		byteVector chiperBlock = decryptBlock(block);
		out.write((const char*)chiperBlock.left.bytes, 4);
		out.write((const char*)chiperBlock.right.bytes, 4);
	}
	out.close();
	in.close();
}
