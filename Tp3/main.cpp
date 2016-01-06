#include <iostream>
#include <fstream>
#include <string>
#include <bitset>
#include <vector>
#include <math.h>

/**
 * Hamming (7,4)
 **/ 
#define N 4
#define HAMMING_7 7

/**
 * Debug macros
 **/ 
#define DEBUG_RF true // Debug Information: Read File
#define DEBUG_HE true // Debug Information: Hamming Encoding

using namespace std; 

/**
 * vector<bitset<N> > readFile(string filename)
 * Read a file in binary and create a vector of bitset wih a width of 4 for each bitset
 * Return a vector bitset
 **/ 
vector<bitset<N> > readFile(string filename)
{
	vector<bitset<N> > content;
	ifstream reader;
	char buffer;
	reader.open(filename.c_str(), ios::binary|ios::in);

	if(DEBUG_RF)
		cout << "Read : \t";

	if(reader != NULL && reader.is_open())
	{
		while(!reader.eof())
		{
			reader.read(&buffer, 1);
			bitset<N> bsBufferLSB(buffer);
			bitset<N> bsBufferMSB(buffer >> 4);
			
			content.push_back(bsBufferMSB);
			content.push_back(bsBufferLSB);
	
			if(DEBUG_RF)
			{
				cout << " |" << bsBufferMSB.to_string();
				cout << " |" << bsBufferLSB.to_string();
			}
		}
	}
	
	if(DEBUG_RF)
		cout << endl;

	reader.close();
	return content;
}

/**
 * vector<bitset<HAMMING_7> > HammingEncoding(vector<bitset<N> > bitsetVector)
 * Convert a vector of bitset<4> into a hamming vector of bitset<7>
 **/ 
vector<bitset<HAMMING_7> > HammingEncoding(vector<bitset<N> >& bitsetVector)
{
	vector<bitset<HAMMING_7> > encodedBitset;
	
	if(DEBUG_HE)
		std::cout << "Encode : \t";
		
	for(vector<bitset<N> >::iterator i = bitsetVector.begin(); i != bitsetVector.end();++i)
	{
		bitset<N> inBuffer = *i;
		bitset<HAMMING_7> outBuffer;

		outBuffer[0] = (inBuffer[0] + inBuffer[1] + inBuffer[3]) % 2;
		outBuffer[1] = (inBuffer[0] + inBuffer[2] + inBuffer[3]) % 2;
		outBuffer[2] = inBuffer[0];
		outBuffer[3] = (inBuffer[1] + inBuffer[2] + inBuffer[3]) % 2;
		outBuffer[4] = inBuffer[1];
		outBuffer[5] = inBuffer[2];
		outBuffer[6] = inBuffer[3];
		
		if(DEBUG_HE)
			cout << " | " << outBuffer.to_string();
		
		encodedBitset.push_back(outBuffer);
	}
	
	if(DEBUG_HE)
		cout << endl;
	
	return encodedBitset;
}

void CheckData(bitset<HAMMING_7>& encode_data)
{
	bitset<HAMMING_7> col1;
	col1[0] = 0;
	col1[1] = 0;
	col1[2] = 0;
	col1[3] = 1;
	col1[4] = 1;
	col1[5] = 1;
	col1[6] = 1;
	
	bitset<HAMMING_7> col2;
	col2[0] = 0;
	col2[1] = 1;
	col2[2] = 1;
	col2[3] = 0;
	col2[4] = 0;
	col2[5] = 1;
	col2[6] = 1;
	
	bitset<HAMMING_7> col3;
	col3[0] = 1;
	col3[1] = 0;
	col3[2] = 1;
	col3[3] = 0;
	col3[4] = 1;
	col3[5] = 0;
	col3[6] = 1;
	
	vector<bitset<HAMMING_7> > checkMatrix;
	checkMatrix.push_back(col1);
	checkMatrix.push_back(col2);
	checkMatrix.push_back(col3);
	
	bitset<3> errorControl;
	for( int i = 0; i < 3; ++i )
	{
		errorControl[i] = 0;
		int value = 0;
		
		for(int j = 0; j < HAMMING_7; ++j )
		{
			value += encode_data[j] * checkMatrix[i][j];
		}
		
		errorControl[i] = (value % 2);
	}
	
	int i = pow(2, 2) * errorControl[0] + pow(2, 1) * errorControl[1] + pow(2, 0) * errorControl[2];
	
	if( i != 0 && i < (HAMMING_7 + 1) )
	{
		cout << "Erreur " << i << endl;
		encode_data[i - 1] = ~encode_data[i - 1];
	}
}

vector<bitset<N> > HammingDecoding(vector<bitset<HAMMING_7> >& encode_data)
{
	vector<bitset<N> > decodeData;
	
	vector<bitset<HAMMING_7> >::iterator it = encode_data.begin();
	
	for( ; it != encode_data.end(); ++it)
	{
		CheckData(*it);
	
		bitset<N> decodeBitset;
		
		decodeBitset[0] = (*it)[2];
		decodeBitset[1] = (*it)[4];
		decodeBitset[2] = (*it)[5];
		decodeBitset[3] = (*it)[6];
		
		decodeData.push_back(decodeBitset);
	}
	
	return decodeData;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                     Main                                                       //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int main()
{
	vector< bitset<N> > input_data;
	vector< bitset<HAMMING_7> > encode_data; 
	vector< bitset<N> > decode_data;

	// Read data to encode
	input_data = readFile("test.txt");

	// Encode by Hamming (7,4) coding
	encode_data = HammingEncoding(input_data);

	// Inject error
	encode_data[0][1] = ~encode_data[0][1];
	encode_data[1][5] = ~encode_data[1][5];

	// Decode
	decode_data = HammingDecoding(encode_data);
	
	// Affichage des données décodées
	vector<bitset<N> >::iterator it = decode_data.begin();
	
	cout << "\nTexte decode : " << endl;
	for( ; it != decode_data.end(); ++it)
	{
		bitset<8> c;
		
		c[4] = (*it)[0];
		c[5] = (*it)[1];
		c[6] = (*it)[2];
		c[7] = (*it)[3];
		
		++it;
		
		c[0] = (*it)[0];
		c[1] = (*it)[1];
		c[2] = (*it)[2];
		c[3] = (*it)[3];
				
		cout << char(c.to_ulong());
	}
}
