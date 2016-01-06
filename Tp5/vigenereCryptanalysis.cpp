#include <iostream>
#include <map>
#include <algorithm>
#include <math.h>

#include "vigenereCryptanalysis.hpp"
#include "vigenere.hpp"

using namespace std;
 

VigenereCryptanalysis::VigenereCryptanalysis(const array<double, 26>& targetFreqs) 
{
	targets = targetFreqs;
	sortedTargets = targets;
	sort(sortedTargets.begin(), sortedTargets.end());
}

pair<string, string> VigenereCryptanalysis::analyze(string input) 
{
	string key = "ISIMA PERHAPS";
	string result = "I CAN NOT DECRYPT THIS TEXT FOR NOW T_T";
	
	vector<int> periods = findPeriod(input);
	
	if( periods.size() > 0 )
	{
		vector<int>::iterator it;
		for(it = periods.begin(); it != periods.end(); ++it)
		{
			string possibleKey = findKey(input, (*it));
			cout << "Key possible : " <<  possibleKey << endl;
		}
		
		cout << "Choose the key : " << endl;
		cin >> key;
		
		Vigenere cipher(key);
		result = cipher.decrypt(input);
	}
	
	return make_pair(result, key);
}

/**************************Find period**************************/
vector<int> VigenereCryptanalysis::findPeriod(const string& input)
{
	vector<int> periods;
	
	for(int i = 1; i < 21; ++i)
	{
		double ic = indexOfXoincidence(input, i);
		
		if( ic > 0.07 )
		{ 
			periods.push_back(i);
		}
	}
	
	return periods;
}

/*
 * http://www.practicalcryptography.com/cryptanalysis/text-characterisation/index-coincidence/
 * http://www.practicalcryptography.com/cryptanalysis/stochastic-searching/cryptanalysis-vigenere-cipher/
 */
double VigenereCryptanalysis::indexOfXoincidence(const string& input, int keyLength)
{
	if( keyLength == 0 )
	{
		return 0;
	}
	
	map< int, map<char, int> > sequences;
	map< int, double > nbChars;
	
	for(unsigned int i = 0; i < input.length(); ++i)
	{
		int indexSequence = i % keyLength;
		
		sequences[indexSequence][input[i]] += 1;
		nbChars[indexSequence] += 1.0;
	}
	
	map< int, map<char, int> >::iterator seqIt;
	map<char, int>::iterator mapIt;
	
	double average = 0.0;
	
	for(seqIt = sequences.begin(); seqIt != sequences.end(); ++seqIt)
	{
		double seqSize = nbChars[(*seqIt).first];
		double ic = 0.0;
		
		for(mapIt = (*seqIt).second.begin(); mapIt != (*seqIt).second.end(); ++mapIt)
		{
			ic += ( (*mapIt).second * ((*mapIt).second - 1) );
		}
		
		average += (ic / ( seqSize * (seqSize - 1) ) );
	}
	
	return average /= sequences.size();
}

/**************************Find key**************************/
//http://www.practicalcryptography.com/cryptanalysis/text-characterisation/chi-squared-statistic/
double VigenereCryptanalysis::chiSquared(const string& input)
{
	double size = 0.0;
	double chi = 0.0;
	
	double charFrequencies[26];
	
	for( int i = 0; i < 26; ++i )
	{
		charFrequencies[i] = 0.0;
	}
	
	for(unsigned int i = 0; i < input.length(); ++i)
	{
		charFrequencies[input[i] - 'a'] += 1.0;
		++size;
	}
				
	for( int i = 0; i < 26; ++i )
	{
		double tmp = charFrequencies[i] - (size * targets[i]);				
		chi += ( pow( tmp, 2 ) / (size * targets[i]) );
	}
	
	return chi;
}

string VigenereCryptanalysis::findKey(const string& input, int keyLength)
{
	if( keyLength == 0 )
	{
		return "";
	}
	
	string key;
	
	map< int, string >  sequences;
	
	// division du text en sous keyLength sequences
	// 1er sequence => toutes les characteres codes par le 1er charactere de la clef
	for(unsigned int i = 0; i < input.length(); ++i)
	{
		int indexSequence = i % keyLength;
		sequences[indexSequence] += input[i];
	}
	
	map< int, string >::iterator it;
	
	for(it = sequences.begin(); it != sequences.end(); ++it)
	{
		double keyChi[26];
		
		
		keyChi[0] = chiSquared((*it).second);
		
		// Pour chaque sequence calcul du chiSquared pour les 26 possibilités que peut prendre le n
		// charactere de la clef
		for( int i = 1; i < 26; ++i )
		{
			for( unsigned int j = 0; j < (*it).second.length(); ++j )
			{
				int dec = (*it).second[j] - 'a';
				--dec;
				
				if( dec < 0 )
				{
					dec = 26 + dec;
				}

				(*it).second[j] = 'a' + dec;
			}
			
			keyChi[i] = chiSquared((*it).second);
		}
		
		// L'index du plus petit chiSquered correspond au caracteres de la clef
		double minChi = keyChi[0];
		int index = 0;
		
		for(int i = 1; i < 26; ++i)
		{
			if( minChi > keyChi[i] )
			{
				index = i;
				minChi = keyChi[i];
			}
		}
		
		key += 'a' + index;
	}
	
	return key;
}
