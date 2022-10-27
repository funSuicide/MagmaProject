#include <iostream>
#include <stdint.h>
#include <bitset>
#include <vector>
#include <string>
#include "Magma.h"
#include <chrono>

int main()
{
	std::string key = "key.txt";
	std::string path1 = "test.txt";
	std::string path2 = "out_2.txt";
	std::string path3 = "out2_2.txt";
	
	
	Magma C(key, path1, path2);
	auto start = std::chrono::steady_clock::now();
	C.encryptText();
	auto end = std::chrono::steady_clock::now();
	std::chrono::duration<double, std::milli> elapsed = end - start;
	std::cout << "timeWork: " << elapsed.count() << std::endl;
	
	Magma D(key, path2, path3);
	auto start2 = std::chrono::steady_clock::now();
	D.decryptText();
	auto end2 = std::chrono::steady_clock::now();
	std::chrono::duration<double, std::milli> elapsed2 = end2 - start2;
	std::cout << "timeWork: " << elapsed2.count() << std::endl; 



	/*
	uint8_t text[8]= { 0x10, 0x32, 0x54, 0x76, 0x98, 0xba, 0xdc, 0xfe };
	for (int i = 0; i < 8; i++) {
		printf("%02x", text[i]);
	}
	std::cout << std::endl;
	uint8_t ex[8] = { 0x3d, 0xca, 0xd8, 0xc2, 0xe5, 0x01, 0xe9, 0x4e };
	std::cout << "MEGA KRUTOY PRIMER SUKA:" << std::endl;
	for (int i = 0; i < 8; i++) {
		printf("%u ", ex[i]);
	}
	std::cout << std::endl;

	Magma C(key, path1, path2);
	halfVector left;
	halfVector right;

	byteVector res;
	byteVector res2;
	for (int i = 0; i < 4; i++) {
		left.bytes[i] = text[i];
	}
	for (int i = 4; i < 8; i++) {
		right.bytes[i-4] = text[i];
	}
	res.left = left;
	res.right = right;
	res2 = C.encryptBlock(res);

	byteVector s3;
	s3 = C.decryptBlock(res2);
	int c = 0;
	for (int i = 0; i < 4; i++) {
		if (ex[i] == res2.left.bytes[i]) {
			c++;
		}
		printf("%u ", res2.left.bytes[i]);
	}
	for (int i = 4; i < 8; i++) {
		if (ex[i] == res2.right.bytes[i-4]) {
			c++;
		}
		printf("%u ", res2.right.bytes[i-4]);
	}

	std::cout << std::endl;
	int d = 0;
	for (int i = 0; i < 4; i++) {
		if (text[i] == s3.left.bytes[i]) {
			d++;
		}
		printf("%u ", s3.left.bytes[i]);
	}
	for (int i = 4; i < 8; i++) {
		if (text[i] == s3.right.bytes[i - 4]) {
			d++;
		}
		printf("%u ", s3.right.bytes[i-4]);
	}
	std::cout << std::endl;
	*/
}


