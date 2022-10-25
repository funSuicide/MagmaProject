#include <string>
#include <vector>
#include <stdint.h>
#include <inttypes.h>

union halfVector { //half 4 byte vector
	uint8_t bytes[sizeof(uint32_t)]; //bytes
};

union byteVector { //8 byte vector
	halfVector left, right; //right and left halfs of 8 byte vector
};

class Magma {
	unsigned int tTable[8][16] = { //table for T-transformation
		{ 1,7,14,13,0,5,8,3,4,15,10,6,9,12,11,2 },
		{ 8,14,2,5,6,9,1,12,15,4,11,0,13,10,3,7 },
		{ 5,13,15,6,9,2,12,10,11,7,8,1,4,3,14,0 },
		{ 7,15,5,10,8,1,6,13,0,9,3,14,11,4,2,12 },
		{ 12,8,2,1,13,4,15,6,7,0,10,5,3,14,9,11 },
		{ 11,3,5,8,2,15,10,13,14,1,7,4,12,9,6,0 },
		{ 6,8,2,3,9,10,5,12,1,14,4,7,11,13,0,15 },
		{ 12,4,6,2,10,5,11,9,14,8,13,7,0,3,15,1 }
	};


	std::string key; 
	std::string path1; 
	std::string path2; 
public:
	Magma(std::string& key, std::string& path1, std::string& path2) { 
		this->key = key;
		this->path1 = path1;
		this->path2 = path2;
	};

	halfVector xOR(halfVector& src1, halfVector& src2); //function for xor 
	halfVector mod32(halfVector& src, halfVector& key); //function for mod32
	halfVector transformationT(halfVector& src); //function for T-transformation


	std::vector<std::string> expandKeys(std::string& key); //need remake
	std::pair<std::vector<bool>, std::vector<bool>> transformationG(std::pair<std::vector<bool>, std::vector<bool>>& src, std::vector<bool>& key); //need remake



	std::vector<bool> decToBin(int value, size_t size); //need remake
	std::vector<bool> strToBin(std::string& src, size_t size); //need remake
	int binToDec(std::vector<bool>& src); //need remake
	std::vector<bool> encryptBlock(std::vector<bool>& block); //need remake
	std::string binToStr(std::vector<bool>& src);  //need remake
	std::vector<bool> decryptBlock(std::vector<bool>& src); //need remake
	void encryptText(); //need remake
	void decryptText(); //need remake
};
