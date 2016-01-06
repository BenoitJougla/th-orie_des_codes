#include "vigenere.hpp"
#include <iostream>


using namespace std;

Vigenere::Vigenere(string key)
{
	// Modifying all characters other than uppercase : lowercase -> uppercase, other -> delete
	for(unsigned int i = 0; i < key.size(); ++i)
	{
		if(key[i] >= 'A' && key[i] <= 'Z')
			this->key += key[i];
		else if(key[i] >= 'a' && key[i] <= 'z')
			this->key += key[i] + 'A' - 'a';
	}
}

string Vigenere::encrypt(string text)
{
	string out;

	// Modifying all characters other than uppercase : lowercase -> uppercase, other -> delete
	for(unsigned int i = 0; i < text.length(); ++i)
	{
		if(text[i] >= 'A' && text[i] <= 'Z')
			out += text[i];
		else if(text[i] >= 'a' && text[i] <= 'z')
			out += text[i] + 'A' - 'a';
	}

	// ADD THE VIGENERE CRYPTION
	for(unsigned int i = 0; i < out.length(); ++i)
	{
		out[i] = 'A' + 	(out[i] + key[(i % key.length())]) % 26;
	}

	return out;
}

string Vigenere::decrypt(string text)
{
	string out;
	string in;

	// Modifying all characters other than uppercase : lowercase -> uppercase, other -> delete
	for(unsigned int i = 0; i < text.length(); ++i)
	{
		if(text[i] >= 'A' && text[i] <= 'Z')
			in += text[i];
		else if(text[i] >= 'a' && text[i] <= 'z')
			in += text[i] + 'A' - 'a';
	}

	// THE VIGENERE DECRYPTION
	for(unsigned int i = 0; i < in.length(); ++i)
	{
		int dec = in[i] - key[(i % key.length())];
		
		if(dec < 0)
		{
			dec = 26 + dec;
		}
		
		out += 'A' + dec;
	}

	return out;
}
