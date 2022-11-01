#include <iostream>
#include <stdint.h>
#include <string>
#include "Magma.h"
#include <chrono>
#include "Encryptor.h"

int main()
{
	//std::string key = "key.txt";
	std::string path1 = "test.txt";
	std::string path2 = "out_2.txt";
	std::string path3 = "out2_2.txt";
	
	/*
	Encryptor C(path1, path2, key);
	auto start = std::chrono::steady_clock::now();
	C.encrypt();
	auto end = std::chrono::steady_clock::now();
	std::chrono::duration<double, std::milli> elapsed = end - start;
	std::cout << "timeWork: " << elapsed.count() << std::endl;
	
	Encryptor D(path2, path3, key);
	auto start2 = std::chrono::steady_clock::now();
	D.decrypt();
	auto end2 = std::chrono::steady_clock::now();
	std::chrono::duration<double, std::milli> elapsed2 = end2 - start2;
	std::cout << "timeWork: " << elapsed2.count() << std::endl; 

	*/
	

	uint8_t text[8]= { 0x10, 0x32, 0x54, 0x76, 0x98, 0xba, 0xdc, 0xfe };
	uint8_t k[32] = {0xff, 0xfe, 0xfd, 0xfc, 0xfb, 0xfa, 0xf9, 0xf8, 0xf7, 0xf6, 0xf5, 0xf4, 0xf3, 0xf2, 0xf1, 0xf0, 0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88, 0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff};
	std::cout << "Test-example:" << std:: endl;
	for (int i = 0; i < 8; i++) {
		printf("%02x", text[i]);
	}
	std::cout << std::endl;
	uint8_t ex[8] = { 0x3d, 0xca, 0xd8, 0xc2, 0xe5, 0x01, 0xe9, 0x4e };
	std::cout << "Example chiper-text:" << std::endl;
	for (int i = 0; i < 8; i++) {
		printf("%02x", ex[i]);
	}
	std::cout << std::endl;

	key a;
	for (int i = 0; i < 32; i++) {
		a.bytes[i] = k[i];
	}
	Magma C(a);
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
	std::cout << "Chiper-text:" << std::endl;
	for (int i = 0; i < 4; i++) {
		printf("%02x", res2.left.bytes[i]);
	}
	for (int i = 4; i < 8; i++) {
		printf("%02x", res2.right.bytes[i-4]);
	}

	std::cout << std::endl;
	std::cout << "Text:" << std::endl;
	for (int i = 0; i < 4; i++) {
		printf("%02x", s3.left.bytes[i]);
	}
	for (int i = 4; i < 8; i++) {
		printf("%02x", s3.right.bytes[i-4]);
	}
	std::cout << std::endl;

}


