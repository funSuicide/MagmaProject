#include "Encryptor.h"
#include "Magma.h"
#include <fstream>
#define MEGABYTE  1048576;

Encryptor::Encryptor(std::string& pathOne, std::string& pathTwo, std::string& pathKey) {
	this->pathOne = pathOne;
	this->pathTwo = pathTwo;
	this->pathKey = pathKey;
}

void Encryptor::encrypt() const {
	char tmpKey[32];
	std::ifstream key(pathKey, std::ios::binary);
	key.read(tmpKey, 32);
	key.close();
	uint8_t* t = (uint8_t*)tmpKey;
	Magma C (t);
	std::ifstream inEnd(pathOne, std::ios::binary);
	inEnd.seekg(0, std::ios::end);
	size_t sizeFile = inEnd.tellg();
	inEnd.close();
	size_t countMegabytes = sizeFile / MEGABYTE;
	std::ifstream in(pathOne, std::ios::binary);
	std::ofstream out(pathTwo, std::ios::binary);
	for (size_t i = 0; i < countMegabytes; i++) {
		char* tmp = new char[1048576];
		in.read(tmp, 1048576);
		uint8_t* tmp2 = (uint8_t*)tmp;
		uint8_t* result = C.encryptText(tmp2);
		out.write((const char*)result, 1048576);
		delete[] tmp;
	}
	in.close();
	out.close();
}

void Encryptor::decrypt() const {
	char tmpKey[32];
	std::ifstream key(pathKey, std::ios::binary);
	key.read(tmpKey, 32);
	key.close();

	uint8_t* t = (uint8_t*)tmpKey;
	Magma D(t);
	std::ifstream inEnd(pathOne, std::ios::binary);
	inEnd.seekg(0, std::ios::end);
	size_t sizeFile = inEnd.tellg();
	inEnd.close();
	size_t countMegabytes = sizeFile / MEGABYTE;
	std::ifstream in(pathOne, std::ios::binary);
	std::ofstream out(pathTwo, std::ios::binary);
	for (size_t i = 0; i < countMegabytes; i++) {
		char* tmp = new char[1048576];
		in.read(tmp, 1048576);
		uint8_t* tmp2 = (uint8_t*)tmp;
		uint8_t* result = D.decryptText(tmp2);
		out.write((const char*)result, 1048576);
		delete[] tmp;
	}
	in.close();
	out.close();
}