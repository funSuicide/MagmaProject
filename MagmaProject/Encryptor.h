#include <iostream>
#include <fstream>

class Encryptor {
	std::string pathOne;
	std::string pathTwo;
	std::string pathKey;
public:
	Encryptor(std::string& pathOne, std::string& pathTwo, std::string& pathKey);
	void encrypt() const;
	void decrypt() const;
};

