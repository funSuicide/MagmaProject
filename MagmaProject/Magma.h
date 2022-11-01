#include <stdint.h>
#include <inttypes.h>

union halfVector { //half 4 byte vector
	uint8_t bytes[sizeof(uint32_t)]; //bytes
	uint32_t vector;
	halfVector() {

	}
	halfVector(uint32_t& src) {
		vector = src;
	}
};

struct byteVector { //8 byte vector
	halfVector left, right; //right and left halfs of 8 byte vector
	byteVector() {

	}
	byteVector(halfVector& left, halfVector& right) {
		this->left = left;
		this->right = right;
	}
};

class Magma {
	static constexpr unsigned int tTable[8][16] = { //table for T-transformation
			{ 12,4,6,2,10,5,11,9,14,8,13,7,0,3,15,1 },
			{ 6,8,2,3,9,10,5,12,1,14,4,7,11,13,0,15 },
			{ 11,3,5,8,2,15,10,13,14,1,7,4,12,9,6,0 },
			{ 12,8,2,1,13,4,15,6,7,0,10,5,3,14,9,11 },
			{ 7,15,5,10,8,1,6,13,0,9,3,14,11,4,2,12 },
			{ 5,13,15,6,9,2,12,10,11,7,8,1,4,3,14,0 },
			{ 8,14,2,5,6,9,1,12,15,4,11,0,13,10,3,7 },
			{ 1,7,14,13,0,5,8,3,4,15,10,6,9,12,11,2 }
	};
	uint8_t key[32];
	halfVector roundKeys[32];
public:
	Magma(uint8_t* key);
	void expandKeys(halfVector* dest); //function for expanding keys
	halfVector xOR(halfVector& src1, halfVector& src2); //function for xor 
	halfVector mod32(halfVector& src, halfVector& key); //function for mod32
	halfVector transformationT(halfVector& src); //function for T-transformation
	
	halfVector gTransformation(halfVector& key, halfVector& half); //function for g-transformation
	byteVector transformationG(byteVector& src, halfVector& key); //function for G-transformation

	
	byteVector encryptBlock(byteVector&src); //function for encrypt one block
	byteVector decryptBlock(byteVector& src); //function for decrypt one block

	void encryptText(uint8_t* data, char* dest, int size); //need remake
	void decryptText(uint8_t* data, char* dest, int size); //need remake
};
