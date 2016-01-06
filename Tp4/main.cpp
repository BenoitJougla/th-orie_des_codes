#include <iostream>       
#include <fstream>
#include <string>        
#include <vector>
#include <bitset>        
#include <cstdlib>
#include <ctime>

const int N=2;
const int K=1;
const int R=4;

#define DEBUG

using namespace std; 

////////////////////////////////////////////////////////////
//      template<int bits> bitset<bits> randBitset()      //
//                                                        //
//               Generate random bitset                   //
////////////////////////////////////////////////////////////

template<int bits> bitset<bits> randBitset()
{ 
	bitset<bits> r(rand());
	for(int i = 0; i < bits/16 - 1; i++)
	{
		r <<= 16;
		r |= bitset<bits>(rand()); 
	}
	return r;
} 

////////////////////////////////////////////////////////////
// vector< bitset<N> > GSM_code(vector< bitset<K> > mess) //
//                                                        //
//     Convolutional coding of a message (GSM norm)       //
////////////////////////////////////////////////////////////

bitset<N> ConvolutionalCoding(const bitset<R+1>& G0, const bitset<R+1>& G1, bitset<R+1>& reg, const bitset<K>& msg)
{
	int g0;
	int g1;
	bitset<N> cod_out;
	
	reg = reg << 1;
	reg.set(0, msg.count());

	g0 = (reg & G0).count() % 2;
	g1 = (reg & G1).count() % 2;

	cod_out.set(0, g0);
	cod_out.set(1, g1);
	
	return cod_out;
}

vector< bitset<N> > GSM_code(vector< bitset<K> > mess)
{
	int i=0;
	vector< bitset<N> > mess_out;

	bitset<R+1> G0(25);
	bitset<R+1> G1(27);
	bitset<R+1> reg;
	reg.reset();

	#ifdef DEBUG
	cout << "-------------------- Debug Informations (Coding) --------------------" << endl << endl;
	cout << "Initial register ( u(i-4)  u(i-3)  u(i-2)  u(i-1)  u(i)  ): " << reg << endl;
	cout << "Polynom G0       ( g0(i-4) g0(i-3) g0(i-2) g0(i-1) g0(i) ): " << G0 << endl;
	cout << "Polynom G1       ( g1(i-4) g1(i-3) g1(i-2) g1(i-1) g1(i) ): " << G1 << endl << endl;
	#endif
	
	vector< bitset<K> > messCpy;
	
	for (unsigned int i = 0; i < mess.size(); ++i)
	{
		messCpy.push_back(mess[i]);
		
		// On rajoute des "0" pour rénitialiser les registres à "0"
		if ( i != 0 && (i % (R +1)) == 0 )
		{
			for (int j = 0; j < R; ++j )
			{
				messCpy.push_back(0);
			}
		}
	}

	for (vector<bitset<K> >::iterator it = messCpy.begin(); it != messCpy.end(); ++it)
	{
		bitset<N> cod_out = ConvolutionalCoding(G0, G1, reg, (*it));
		mess_out.push_back(cod_out);

		#ifdef DEBUG
		cout << "Block number: " << ++i << " - In frame: "<< *it << endl; 
		cout << "\t Current status of registers: "<< reg << endl;
		cout << "\t Out : " << cod_out << endl;
		#endif
	}
	#ifdef DEBUG
	cout << "------------------------------------------------------------------" << endl << endl;
	#endif

	return mess_out;
}

/////////////////////////////////////////////////////////////////////////
// vector< bitset<N> >  GSM_transmission(vector< bitset<N> > mess_cod) //
//                                                                     //
//         Simulation of a transmission channel => adding errors       //
/////////////////////////////////////////////////////////////////////////

vector< bitset<N> >  GSM_transmission(vector< bitset<N> > mess_cod, int nbErrors)
{
	vector< bitset<N> > mess_tra = mess_cod;
	
	for(int i = 0; i < nbErrors; ++i)
	{
		int index = rand() % mess_cod.size();
		cout << "Index Error : " << index << endl;
		mess_tra[index] = randBitset<N>();
	}	

	return mess_tra;
}

//////////////////////////////////////////////////////////////////
// vector< bitset<K> > GSM_decode(vector< bitset<N> > mess_tra) //
//                                                              //
//     Convolutional decoding of a message (GSM norm)           //
//////////////////////////////////////////////////////////////////

struct structPath {
	bitset<R+1> reg;
	int error;
	vector< bitset<K> > code;
};

typedef struct structPath structPath;


vector< bitset<K> > bestPath(const vector<structPath>& paths)
{
	vector< bitset<K> > path;
	
	if( paths.size() > 0 )
	{
		int index = 0;
		structPath minPath = paths[0];
		int min = paths[0].error;
		
		for (vector<structPath>::const_iterator it = paths.begin(); it != paths.end(); ++it)
		{
			if( (*it).error < min )
			{
				min = (*it).error;
				minPath = (*it);
			}
			
			++index;
		}
				
		for (vector<bitset<K> >::iterator it = minPath.code.begin(); it != minPath.code.end(); ++it)
		{
			path.push_back((*it));
		}		
	}
	
	return path;
}

vector< bitset<K> > GSM_decode(vector< bitset<N> > mess_tra)
{
	vector< bitset<K> > mess_dec;
	
	structPath path;
	path.reg.reset();
	path.error = 0;
	
	bitset<R+1> G0(25);
	bitset<R+1> G1(27);
	
	vector<structPath> tmp;
	vector<structPath> paths;
	
	// Etat initial reg = 0000
	paths.push_back(path);
	
	int i = 0;
	
	for (vector<bitset<N> >::iterator msg_it = mess_tra.begin() ; msg_it != mess_tra.end(); )
	{	
		tmp = paths;
		paths.clear();
		
		if ( i != 0 && (i % (R +1)) == 0 )
		{			
			for (int j = 0; j < R; ++j )
			{				
				for (vector<structPath>::iterator it = tmp.begin(); it != tmp.end(); ++it) 
				{
					// Chhemin avec le code 0
					bitset<N> encodedMsg = ConvolutionalCoding(G0, G1, (*it).reg, 0);
					
					bitset<N> codeError(0);
					codeError.set(0, (*msg_it)[0] - encodedMsg[0]);
					codeError.set(1, (*msg_it)[1] - encodedMsg[1]);
					
					(*it).error += codeError.count();
					
					paths.push_back((*it));
				}
				
				++msg_it;
			}
			
			// On prend le meilleur chemin
			vector<bitset<K> > bestpath = bestPath(paths);
			
			for (vector<bitset<K> >::iterator it = bestpath.begin(); it != bestpath.end(); ++it)
			{
				mess_dec.push_back((*it));
			}
			
			tmp.clear();
			paths.clear();
			
			structPath path;
			path.reg.reset();
			path.error = 0;
			
			// Etat initial reg = 0000
			paths.push_back(path);
			
			i = 0;
		}
		else
		{
			for (vector<structPath>::iterator it = tmp.begin(); it != tmp.end(); ++it) 
			{
				structPath path;
				path.error = (*it).error;
				path.reg = (*it).reg;
				path.code = (*it).code;
				
				// Chhemin avec le code 0
				bitset<N> encodedMsg = ConvolutionalCoding(G0, G1, path.reg, 0);
				
				bitset<N> codeError(0);
				codeError.set(0, (*msg_it)[0] - encodedMsg[0]);
				codeError.set(1, (*msg_it)[1] - encodedMsg[1]);
		
				path.error += codeError.count();
				path.code.push_back( bitset<K> (0) );
				
				// Chemin avec le code 1
				encodedMsg.reset();			
				encodedMsg = ConvolutionalCoding(G0, G1, (*it).reg, 1);
				
				codeError.reset();
				codeError.set(0, (*msg_it)[0] - encodedMsg[0]);
				codeError.set(1, (*msg_it)[1] - encodedMsg[1]);
				
				(*it).error += codeError.count();
				(*it).code.push_back( bitset<K> (1) );
				
				paths.push_back((*it));
				paths.push_back(path);
			}
			
			++msg_it;
			++i;
		}
	}
	
	// On prend le meilleur chemin
	vector<bitset<K> > bestpath = bestPath(paths);
	
	for (vector<bitset<K> >::iterator it = bestpath.begin(); it != bestpath.end(); ++it)
	{
		mess_dec.push_back((*it));
	}	
	
	return mess_dec;
}

//////////////////////////////////////////////////////////////////
//                             MAIN                             //
//////////////////////////////////////////////////////////////////

int main()
{
	int NbMot = 12;

	vector< bitset<K> > mess;
	vector< bitset<N> > mess_cod;
	vector< bitset<N> > mess_tra;
	vector< bitset<K> > mess_dec;

	// Random initialization message
	srand( (unsigned)time( NULL ) );
	
	for(int i=0;i<NbMot;++i)
		mess.push_back(randBitset<K>());
		
	for(int i=0;i<R;++i)
		mess.push_back(bitset<K>(0));

	// Coding of the message => mess_cod
	mess_cod = GSM_code(mess);

	// Simulation of a transmission (errors) => mess_tra
	mess_tra = GSM_transmission(mess_cod, 2);

	// Decoding of the transmitted message => mess_dec
	mess_dec = GSM_decode(mess_tra);

	cout << "Source Message   : ";
	for (vector<bitset<K> >::iterator it = mess.begin() ; it != mess.end(); ++it)
		cout << ' ' << *it;
	cout << '\n';

	cout << "Coded Message    : ";
	for (vector<bitset<N> >::iterator it = mess_cod.begin() ; it != mess_cod.end(); ++it)
		cout << ' ' << *it;
	cout << '\n';

	cout << "Received Message : ";
	for (vector<bitset<N> >::iterator it = mess_tra.begin() ; it != mess_tra.end(); ++it)
		cout << ' ' << *it;
	cout << '\n';

	cout << "Decoded Message  : ";
	for (vector<bitset<K> >::iterator it = mess_dec.begin() ; it != mess_dec.end(); ++it)
		cout << ' ' << *it;
	cout << '\n';
}

