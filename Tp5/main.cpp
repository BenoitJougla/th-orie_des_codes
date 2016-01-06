#include "vigenere.hpp"
#include "vigenereCryptanalysis.hpp"

#include <string>
#include <iostream>
#include <array>
#include <fstream>

using namespace std;

//////////////////////////////////////////////////////////////////
//                             MAIN                             //
//////////////////////////////////////////////////////////////////
int main()
{
	// Vigenere
	cout << "\nVigenere ************************************************************************\n\n" << endl;
	Vigenere cipher("MYKEY");

	string original_en  = "Kerckhoffs's principle - A cryptosystem should be secure even if everything about the system, except the key, is public knowledge.";
	string encrypted_en = cipher.encrypt(original_en);
	string decrypted_en = cipher.decrypt(encrypted_en);
	
	cout << "English version : " << endl;
	cout << original_en << endl;
	cout << "Encrypted: " << encrypted_en << endl;
	cout << "Decrypted: " << decrypted_en << endl;

	string original_fr  = "Principe de Kerckhoffs - Toute la securite d'un systeme cryptographique doit reposer sur la clef et pas sur le systeme lui meme.";

	string encrypted_fr = cipher.encrypt(original_fr);
	string decrypted_fr = cipher.decrypt(encrypted_fr);

	cout << "\nFrench version : " << endl;
	cout << original_fr << endl;
	cout << "Encrypted: " << encrypted_fr << endl;
	cout << "Decrypted: " << decrypted_fr << endl;
	
	// Cryptanalysis part
	cout << "\n\nCryptanalysis ************************************************************************\n\n" << endl;
	
	cout << "Choose the text to decrypt : " << endl;
	cout << "1) ciphertext_1.txt" << endl;
	cout << "2) ciphertext_1.txt" << endl;
	cout << "3) ciphertext_1.txt" << endl;
	cout << "4) ciphertext_1.txt" << endl;
	
	string cmd;
	cin >> cmd;
	
	int fileIndex;
	try
	{
		fileIndex = std::stoi(cmd);
	}
	catch(const std::invalid_argument& ia)
	{
		cout << "Erreur : fichier non reconnu. Par défaut fichier 1" << endl;
		fileIndex = 1;
	}
	
	string fileName;
	
	switch(fileIndex)
	{
		case 1:
			fileName = "ciphertext_1.txt";
			break;
		case 2:
			fileName = "ciphertext_2.txt";
			break;
		case 3:
			fileName = "ciphertext_3.txt";
			break;
		case 4:
			fileName = "ciphertext_4.txt";
			break;
	}
	
	ifstream file(fileName, ios::in);
	
	string input = "";
	if(file)
	{
		string line;
		while( getline(file, line) )
		{
			input += line;
		}
	}	
	
	file.close();
	
	cout << "Text encrypted : " << input << "\n\n" << endl;
	
	/*string input = "YOU HAVE TO COPY THE CIPHERTEXT FROM THE ATTACHED FILES OR FROM YOUR CIPHER ALGORITHM";
	string input = "vptnvffuntshtarptymjwzirappljmhhqvsubwlzzygvtyitarptyiougxiuydtgzhhvvmumshwkzgstfmekvmpkswdgbilvjljmglmjfqwioiivknulvvfemioiemojtywdsajtwmtcgluysdsumfbieugmvalvxkjduetukatymvkqzhvqvgvptytjwwldyeevquhlulwpkt";*/

	array<double, 26> english = { {
		0.08167, 0.01492, 0.02782, 0.04253, 0.12702, 0.02228,
		0.02015, 0.06094, 0.06966, 0.00153, 0.00772, 0.04025,
		0.02406, 0.06749, 0.07507, 0.01929, 0.00095, 0.05987,
		0.06327, 0.09056, 0.02758, 0.00978, 0.02360, 0.00150,
		0.01974, 0.00074} };

	array<double, 26> french = { {
		0.0811,  0.0081,  0.0338,  0.0428,  0.1769,  0.0113,
		0.0119,  0.0074,  0.0724,  0.0018,  0.0002,  0.0599, 
		0.0229,  0.0768,  0.0520,  0.0292,  0.0083,  0.0643,
		0.0887,  0.0744,  0.0523,  0.0128,  0.0006,  0.0053,
		0.0026,  0.0012} };
 
	cout << "English frequencies" << endl;
	VigenereCryptanalysis vc_en(english);
	pair<string, string> output_en = vc_en.analyze(input);

	cout << "Key: "  << output_en.second << endl;
	cout << "Text: " << output_en.first << endl;

	cout << "\nFrench frequencies" << endl;
	VigenereCryptanalysis vc_fr(french);
	pair<string, string> output_fr = vc_fr.analyze(input);

	cout << "Key: "  << output_fr.second << endl;
	cout << "Text: " << output_fr.first << endl;
}
