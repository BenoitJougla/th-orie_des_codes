#ifndef _VIGENERE_HPP_
#define _VIGENERE_HPP_

#include <string>

class Vigenere
{
	private:
		std::string key;

	public:
		Vigenere(std::string key);
		
		std::string encrypt(std::string text);
		std::string decrypt(std::string text);
};

#endif
