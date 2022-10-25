#include "Magma.h"
#include <iostream>
#include <cmath>
#include <algorithm>
#include <fstream>


std::vector<bool> Magma::decToBin(int value, size_t size) { //working
	std::vector<bool> result(size);
	for (int i = 0; value > 0; i++) {
		result[size - i - 1] = value % 2;
		value = value / 2;
	}
	return result;
}

std::vector<bool> Magma::strToBin(std::string& src, size_t size) { //working
	std::vector<bool> result(size);
	int start = 0;
	for (auto i = src.begin(); i != src.end(); i++) {
		int c = int(*i);
		std::vector<bool> tmp = decToBin(c, 8);
		for (auto j = tmp.begin(); j != tmp.end(); j++) {
			result[start] = *j;
			start++;
		}
	}
	return result;
}

void Magma::xOR(std::vector<bool>& src1, std::vector<bool>& src2) { //working
	for (int i = 0; i < src1.size(); i++) {
		if (src1[i] != src2[i]) {
			src1[i] = 1;
		}
		else src1[i] = 0;
	}
}

int Magma::binToDec(std::vector<bool>& src) { //working
	int result = 0;
	for (int i = 0; i < src.size(); i++) {
		result += pow(2, (src.size() - i - 1)) * src[i];
	}
	return result;
}



std::vector<bool> Magma::mod32(std::vector<bool>& src, std::vector<bool>& key) { //working
	int a = binToDec(src);
	int b = binToDec(key);
	return decToBin((a + b) % 32, 32);
}

std::vector<std::string> Magma::expandKeys(std::string& key) { //working
	std::vector<std::string> result;
	for (int i = 0; i < 32; i += 4) {
		result.push_back(key.substr(i, 4));
	}
	for (int i = 0; i < 32; i += 4) {
		result.push_back(key.substr(i, 4));
	}
	for (int i = 0; i < 32; i += 4) {
		result.push_back(key.substr(i, 4));
	}
	for (int i = 3; i < 32; i += 4) {
		result.push_back(key.substr(31 - i, 4));
	}
	return result;
}

std::vector<bool> Magma::transformationT(std::vector<bool>& src) { //need test
	std::vector<bool> result(32);
	int start = 0;
	int r = 0;
	int end = 4;
	int target = 0;
	for (int i = 0; i < 8; i++) {
		std::vector<bool> tmp(4);
		for (int j = 0; j < 4; j++) {
			tmp[j] = src[start];
			start++;
		}
		int val = binToDec(tmp);
		for (int q = 0; q < 16; q++) {
			if (val == t_table[i][q]) {
				target = q;
				break;
			}
		}
		std::vector<bool> prev = decToBin(target, 8);
		int z = 0;
		for (r; r < end; r++) {
			result[r] = prev[4 + z];
			z++;
		}
		end += 4;
	}
	return result;
}

std::pair<std::vector<bool>, std::vector<bool>> Magma::transformationG(std::pair<std::vector<bool>, std::vector<bool>>& src, std::vector<bool>& key) {
	std::pair<std::vector<bool>, std::vector<bool>> result;
	std::vector<bool> tmp = mod32(src.second, key);
	tmp = transformationT(tmp);
	std::rotate(tmp.begin(), tmp.begin() + 11, tmp.end());
	xOR(tmp, src.first);
	return std::pair<std::vector<bool>, std::vector<bool>>(src.second, tmp);
}


std::vector<bool> Magma::encryptBlock(std::vector<bool>& text) {
	std::vector<bool> l(text.begin(), text.begin() + 32);
	std::vector<bool> r(text.begin() + 32, text.end());
	std::vector<std::string> keys = expandKeys(key);
	std::pair<std::vector<bool>, std::vector<bool>> block(l, r);
	for (int i = 0; i < 32; i++) {
		std::vector<bool> tmp = strToBin(keys[i], 32);
		block = transformationG(block, tmp);
	}
	std::pair<std::vector<bool>, std::vector<bool>> final_block(block.second, block.first);
	std::vector<bool> result(final_block.first);
	for (auto i = final_block.second.begin(); i != final_block.second.end(); i++) {
		result.push_back(*i);
	}
	return result;
}

std::string Magma::binToStr(std::vector<bool>& src) {
	std::string result;
	for (int i = 8; i < 72; i += 8) {
		std::vector<bool> tmp(src.begin(), src.begin() + i);
		result.push_back(char(binToDec(tmp)));
	}
	return result;
}

std::vector<bool> Magma::decryptBlock(std::vector<bool>& text) {
	std::vector<bool> l(text.begin(), text.begin() + 32);
	std::vector<bool> r(text.begin() + 32, text.end());
	std::vector<std::string> keys = expandKeys(key);
	std::pair<std::vector<bool>, std::vector<bool>> block(l, r);
	for (int i = 31; i >= 0; i--) {
		std::vector<bool> tmp = strToBin(keys[31 - i], 32);
		block = transformationG(block, tmp);
	}
	std::pair<std::vector<bool>, std::vector<bool>> final_block(block.second, block.first);
	std::vector<bool> result(final_block.first);
	for (auto i = final_block.second.begin(); i != final_block.second.end(); i++) {
		result.push_back(*i);

	}
	return result;
}

void Magma::encryptText() {
	std::ifstream in(path1);
	std::ofstream out(path2, std::ios::app);
	while (!(in.eof())) {
		char tmp[8];
		in.read(tmp, 8);
		std::string block(tmp, sizeof(tmp));
		std::vector<bool> bitBlock = strToBin(block, 64);
		std::vector<bool> bitChiperBlock = encryptBlock(bitBlock);
		out << binToStr(bitChiperBlock);

	}
	out.close();
	in.close();
}

void Magma::decryptText() {
	std::ifstream in(path1);
	std::ofstream out(path2, std::ios::app);
	while (!(in.eof())) {
		char tmp[8];
		in.read(tmp, 8);
		std::string block(tmp, sizeof(tmp));
		std::vector<bool> bitBlock = strToBin(block, 64);
		std::vector<bool> bitChiperBlock = decryptBlock(bitBlock);
		out << binToStr(bitChiperBlock);
	}
	in.close();
}