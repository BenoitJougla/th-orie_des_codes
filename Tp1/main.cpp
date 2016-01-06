#include <vector>
#include <queue>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <cstdlib>
#include <queue>
#include <map>
#include <cmath>

using namespace std;

class Symbol {

    public:
		string name;
		double freq;
		string code;
		bool leaf;
		Symbol *left, *right;

		Symbol()
			: name(""),freq(0),code(""),leaf(true), left(NULL), right(NULL) {}
			
		Symbol(string newName, double newFreq, string newCode="", bool newLeaf=true, Symbol* newLeft=NULL, Symbol* newRight=NULL)
			:name(newName), freq(newFreq),code(newCode), leaf(newLeaf), left(newLeft), right(newRight) {}
 
		string toDot() 
		{
			string result = "";
			
			ostringstream strs;
			strs << freq;
			string freqStr = strs.str();

			
			if(!leaf)
			{
				result += name + " [shape=ellipse,label=" + freqStr + "];\n";
			}
			else
			{
				result += name + " [shape=none, margin=0, label=<\n" +
					"<TABLE BORDER=\"0\" CELLBORDER=\"1\" CELLSPACING=\"0\" CELLPADDING=\"3\"\n>" +
					"\t<TR>\n" +
					"\t\t<TD>" + name + "</TD>\n" +
					"\t\t<TD COLSPAN=\"2\">" + freqStr + "</TD>\n" +
					"\t</TR>\n" +
					"\t<TR>\n" +
					"\t\t<TD COLSPAN=\"3\">" + code + "</TD>\n" +
					"\t</TR>\n" +
					"</TABLE>>];\n";
			}
			
			return result;
		}
 	// To Do: Code Here
  	// Complete the class
};

class SymbolCmp {
	public:
		// fonction de comparaison  
		bool operator()(const Symbol *s1, const Symbol *s2)
		{
			return s1->freq > s2->freq;
		}
};

void updateCode(Symbol* symbol, string code)
{
	symbol->code = code + symbol->code;
	
	if( symbol->left != NULL )
	{
		updateCode(symbol->left, code);
	}
	
	if( symbol->right != NULL )
	{
		updateCode(symbol->right, code);
	}
}

Symbol* CreateHuffmanCode(vector<Symbol*>& alphabet)
{
	// To Do: Code Here
	// Build the Huffman code tree
	priority_queue<Symbol*, vector<Symbol*>, SymbolCmp> binaryTree;
	
	for(size_t i = 0; i < alphabet.size(); ++i)
	{
		binaryTree.push(alphabet[i]);
	}
	
	while(binaryTree.size() > 1)
	{
		Symbol* s1 = binaryTree.top();
		binaryTree.pop();
		updateCode(s1, "0");
		
		Symbol* s2 = binaryTree.top();
		binaryTree.pop();
		updateCode(s2, "1");
		
		Symbol* node = new Symbol(s1->name + s2->name, s1->freq + s2->freq, "", false, s1, s2);
		
		binaryTree.push(node);
	}

	return binaryTree.top();
}

void CreateAlphabet(vector<Symbol*>& alphabet, bool Proba=true)
{
	if(Proba)
	{
		// Probability of french letters
		alphabet.push_back(new Symbol("a",8.11));
		alphabet.push_back(new Symbol("b",0.81));
		alphabet.push_back(new Symbol("c",3.38));
		alphabet.push_back(new Symbol("d",4.28));
		alphabet.push_back(new Symbol("e",17.69));
		alphabet.push_back(new Symbol("f",1.13));
		alphabet.push_back(new Symbol("g",1.19));
		alphabet.push_back(new Symbol("h",0.74));
		alphabet.push_back(new Symbol("i",7.24));
		alphabet.push_back(new Symbol("j",0.18));
		alphabet.push_back(new Symbol("k",0.02));
		alphabet.push_back(new Symbol("l",5.99));
		alphabet.push_back(new Symbol("m",2.29));
		alphabet.push_back(new Symbol("n",7.68));
		alphabet.push_back(new Symbol("o",5.2));
		alphabet.push_back(new Symbol("p",2.92));
		alphabet.push_back(new Symbol("q",0.83));
		alphabet.push_back(new Symbol("r",6.43));
		alphabet.push_back(new Symbol("s",8.87));
		alphabet.push_back(new Symbol("t",7.44));
		alphabet.push_back(new Symbol("u",5.23));
		alphabet.push_back(new Symbol("v",1.28));
		alphabet.push_back(new Symbol("w",0.06));
		alphabet.push_back(new Symbol("x",0.53));
		alphabet.push_back(new Symbol("y",0.26));
		alphabet.push_back(new Symbol("z",0.12));
	}
	else
	{
		// To Do: Code Here
		// 1/ Take the probabilty of letters in french langage (default code)
		// 2/ Compute the probabilty by the text to encode (text.txt)
		ifstream file("text.txt", ios::in);
		
		if(file)
		{
			char word;
			double nbChar = 0.0;
			map<char,int> charCounter;
			
			while( file.get(word) )
			{
				if( word != '\n' ) 
				{
					++nbChar;
					charCounter[word] += 1;
				}
			}
			
			file.close();
			
			for( map<char,int>::iterator it=charCounter.begin(); it!=charCounter.end(); ++it )
			{
				stringstream ss;
				string name;
				ss << it->first;
				ss >> name;
				
				alphabet.push_back(new Symbol(name, (it->second / nbChar) * 100.0));
			}			
		}
		else
		{
			cout << "Erreur lors de lecture du fichier text.txt" << endl;
		}
	}
}

void DeleteTree(Symbol* root)
{
	if( root->left != NULL )
	{
		DeleteTree(root->left);
	}
	
	if( root->right != NULL )
	{
		DeleteTree(root->right);
	}
	
	delete root;
}

void DeleteMemory(vector<Symbol*>& alphabet, Symbol* root)
{
	DeleteTree(root);
	
	alphabet.erase( alphabet.begin(),alphabet.end() );
	
	// To Do: Code Here
	// Clear the memory	
}

string ExportTree(Symbol* root)
{
	string result = root->toDot();
	
	if( root->left != NULL )
	{
		result += root->name + "->" + root->left->name + ";\n";
		result += ExportTree(root->left);
	}
	
	if( root->right != NULL )
	{
		result += root->name + "->" + root->right->name + ";\n";
		result += ExportTree(root->right);
	}
	
	return result;
}

int ExportTreeToDot(Symbol* root, string fileName)
{
	ofstream file(fileName.c_str(), ios::out | ios::trunc);
	
	if(file)
	{
		file << "digraph Huffman {" << endl;
		
		file << ExportTree(root) << endl;
		
		file << "}" << endl;
		
		file.close();
		return 0;
	}
	else
	{
		return -1;
	}
}

void Entropy(vector<Symbol*>& alphabet, double& entropy, double& averageLength) 
{
	for(size_t i = 0; i < alphabet.size(); ++i)
	{
		entropy += alphabet[i]->freq * log2(alphabet[i]->freq) / 100.0;
		averageLength += (alphabet[i]->freq / 100.0) * alphabet[i]->code.length();
	}
}

string EncodingFuncs(vector<Symbol*>& alphabet, string stringToEncode) 
{
	string result = "";
	
	map<string, Symbol*> map;
	vector<Symbol*>::iterator it;
	
	for(it = alphabet.begin(); it != alphabet.end(); ++it)
	{
		map[(*it)->name] = (*it);
	}
	
	for(size_t i = 0; i < stringToEncode.size(); ++i) 
	{
		stringstream ss;
		string s;
		ss << stringToEncode[i];
		ss >> s;
		
		result += map[s]->code;
	}
	
	return result;
}

/**
 *	Encode le fichier avec l'alphabet passé en paramétre
 * 	Retourn le taux de compression 
 */
double FileEncoding(string fileName, vector<Symbol*>& alphabet, string& fileEncoded) 
{
	ifstream file(fileName.c_str(), ios::in);
	double sizefile = 0.0;
	
	if(file)
	{
		string line;
		while( getline(file, line) )
		{
			sizefile += line.size(); 
			fileEncoded += EncodingFuncs(alphabet, line);
		}
	}
	else
	{
		cout << "Erreur lors de lecture du fichier text.txt" << endl;
	}
	
	file.close();
	
	double sizeFileEncoded = ( (fileEncoded.size() / 8.0) + alphabet.size() );
	
	return 1 - (sizeFileEncoded / sizefile);
}

string DecodingFuncs(vector<Symbol*>& alphabet, string stringToDecode) 
{
	string result = "";
	
	map<string, string> map;
	vector<Symbol*>::iterator it;
	
	for(it = alphabet.begin(); it != alphabet.end(); ++it)
	{
		map[(*it)->code] = (*it)->name;
	}
	
	string code = "";

	for(size_t i = 0; i < stringToDecode.size(); ++i) 
	{
		stringstream ss;
		string s;
		ss << stringToDecode[i];
		ss >> s;
		
		code += s;
		
		if(map.find(code) != map.end())
		{
			result += map[code];
			code = "";
		}
	}
	
	return result;
}

int main()
{
	vector<Symbol*> alphabet;

	// Compute the frequencies of the symbol
	CreateAlphabet(alphabet);

	// Build the Huffman code tree 
	Symbol* root = CreateHuffmanCode(alphabet);
	
	double entropy1 = 0.0;
	double averageLength1 = 0.0;
	Entropy(alphabet, entropy1, averageLength1);

	// Display the result
	for(size_t i=0; i < alphabet.size(); ++i)
		cout << "Name : " << alphabet[i]->name << " ; Code : " << alphabet[i]->code << endl;

	// Export tree
	if( ExportTreeToDot(root, "Solutions/binaryTree.dot") == -1 )
	{
		cout << "Erreur lors de l'exportation de l'arbre en format dot" << endl;
	}
	
	string fileEncoded = "";
	cout << "Encodage du fichier text.txt avec l'alphabet ASCII. Taux de compression : " <<  FileEncoding("text.txt", alphabet, fileEncoded) * 100.0 << "%" <<endl;
	
	// Clear the memory
	DeleteMemory(alphabet, root);
	
	/*******************************/
	/**	Frequencies from text.txt **/
	/*******************************/
	// Compute the frequencies from text.txt
	CreateAlphabet(alphabet, false);
	
	// Build the Huffman code tree 
	root = CreateHuffmanCode(alphabet);
	
	// Display the result
	cout << "\nFrequencies from test.txt :\n" << endl;
	for(size_t i=0; i < alphabet.size(); ++i)
	{
		cout << "Name : " << alphabet[i]->name << " ; Code : " << alphabet[i]->code << endl;
	}

	// Export tree
	if( ExportTreeToDot(root, "Solutions/binaryTree2.dot") == -1 )
	{
		cout << "Erreur lors de l'exportation de l'arbre en format dot" << endl;
	}
	
	fileEncoded = "";
	cout << "Encodage du fichier text.txt avec l'alphabet ASCII. Taux de compression : " <<  FileEncoding("text.txt", alphabet, fileEncoded) * 100.0 << "%" <<endl;
	cout << "\nEncodage du message :\n" << fileEncoded << endl;
	cout << "\nDecodage du message :\n" << DecodingFuncs(alphabet, fileEncoded) << endl;
	
	double entropy2 = 0.0;
	double averageLength2 = 0.0;
	Entropy(alphabet, entropy2, averageLength2);
	
	cout << "\nEntropy : " << entropy2 << " longueur moyenne ASCII  : " << averageLength1 << " longueur moyenne de l'alphabet de text.txt : " << averageLength2 << endl;
	
	// Clear the memory
	DeleteMemory(alphabet, root);
	
	return 0;
}

