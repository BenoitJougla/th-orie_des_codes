#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <cstdlib>
#include <vector>
#include <cmath> 

#include "CImg-1.6.6_pre093015/CImg.h"

#define ROUND( a ) ( ( (a) < 0 ) ? (int) ( (a) - 0.5 ) : (int) ( (a) + 0.5 ) )

using namespace std;
using namespace cimg_library;

/**
 * Génération des cosinus utilisés dans le calcul de la DCT.
 **/ 
double** DctCos() 
{
	double** dctCos = new double*[8];
	
	for( int i = 0; i < 8; ++i )
	{
		dctCos[i] = new double[8];
	}
	
	for( int i = 0; i < 8; ++i )
	{
		for( int j = 0; j < 8; ++j )
		{
			dctCos[i][j] = cos( ((2 * i + 1) * j * M_PI) / 16.0 );
		}
	}
	
	return dctCos;
}

/**
 * Calcul des coefficients pour le DCT.
 * Si i = 0 retourne 1/srqt(2)
 * Sinon 1
 **/ 
double DctCoefficient(int i)
{
	return i == 0 ? (1.0 / sqrt(2.0)) : 1.0;
}

/**
 * Applique la DCT sur un bloc de 8 * 8
 **/
void DctBlock(CImg<double>& image, CImg<double>& sub_image, int i, int j, double** dctCos)
{
	for( int k = 0; k < 8; ++k )
	{
		for( int l = 0; l < 8; ++l )
		{
			double somme2 = 0.0;
			
			for( int m = 0; m < 8; ++m )
			{
				double somme1 = 0.0;
				
				for( int n = 0; n < 8; ++n )
				{
					somme1 += (sub_image(m, n) - 128) * dctCos[m][k] * dctCos[n][l];
				}
				
				somme2 += somme1;
			}
			
			double dct = (2.0 / 8.0) * DctCoefficient(k) * DctCoefficient(l) * somme2;
			
			image( i + k, j + l) = dct;
		}
	}
}

/**
 * Applique la DCT sur une image
 **/ 
void Dct(CImg<double>& image, float quality)
{
	double ** dctCos = DctCos();
	
	for( int i = 0; i < image.height(); i += 8 )
	{
		for( int j = 0; j < image.width(); j += 8 )
		{
			CImg<double> sub_image = image.get_crop(i, j, i + 7, j + 7);
			DctBlock(image, sub_image, i, j, dctCos);
		}
	}
	
	for( int i = 0; i < 8; ++i )
	{
		delete[] dctCos[i];
	}
	
	delete[] dctCos;
}

/**
 * Quantification de l'image
 **/ 
void Quantization(CImg<double>& image, float quality, vector<double>& dcCoef)
{
	// Quantization matrix
	CImg<double> Q(8,8);
	Q(0,0)=16;   Q(0,1)=11;   Q(0,2)=10;   Q(0,3)=16;   Q(0,4)=24;   Q(0,5)=40;   Q(0,6)=51;   Q(0,7)=61;
	Q(1,0)=12;   Q(1,1)=12;   Q(1,2)=14;   Q(1,3)=19;   Q(1,4)=26;   Q(1,5)=58;   Q(1,6)=60;   Q(1,7)=55;
	Q(2,0)=14;   Q(2,1)=13;   Q(2,2)=16;   Q(2,3)=24;   Q(2,4)=40;   Q(2,5)=57;   Q(2,6)=69;   Q(2,7)=56;
	Q(3,0)=14;   Q(3,1)=17;   Q(3,2)=22;   Q(3,3)=29;   Q(3,4)=51;   Q(3,5)=87;   Q(3,6)=80;   Q(3,7)=62;
	Q(4,0)=18;   Q(4,1)=22;   Q(4,2)=37;   Q(4,3)=56;   Q(4,4)=68;   Q(4,5)=109;  Q(4,6)=103;  Q(4,7)=77;
	Q(5,0)=24;   Q(5,1)=35;   Q(5,2)=55;   Q(5,3)=64;   Q(5,4)=81;   Q(5,5)=104;  Q(5,6)=113;  Q(5,7)=92;
	Q(6,0)=49;   Q(6,1)=64;   Q(6,2)=78;   Q(6,3)=87;   Q(6,4)=103;  Q(6,5)=121;  Q(6,6)=120;  Q(6,7)=101;
	Q(7,0)=72;   Q(7,1)=92;   Q(7,2)=95;   Q(7,3)=98;   Q(7,4)=112;  Q(7,5)=100;  Q(7,6)=103;  Q(7,7)=99;
	Q *= quality;
	
	for( int i = 0; i < image.height(); i += 8 )
	{
		for( int j = 0; j < image.width(); j += 8 )
		{
			for( int k = 0; k < 8; ++k )
			{
				for( int l = 0; l < 8; ++l )
				{
					double dct = image( i + k, j + l);
					dcCoef.push_back(dct);
					image( i + k, j + l) = ROUND( dct / Q(k, l) );
				}
			}
		}
	}
}

/**
 * Encodage de l'image dans la norme JPEG
 **/ 
CImg<double>* JPEGEncoder(CImg<double>& image, float quality)
{
	CImg<double>* comp = new CImg<double>(image.width(), image.height(), 1, 1, 0);
	*comp = image;
	
	Dct(*comp, quality);
	
	vector<double> dcCoef;
	
	Quantization(*comp, quality, dcCoef);

	return comp;
}

/**
 * Déquantification et DCT inverse sur un bloc 8 * 8 de l'image encodé
 **/ 
void InverseDctBlock(CImg<double>& image, CImg<double>& sub_image, int i, int j, double** dctCos, float quality)
{
	// Quantization matrix
	CImg<double> Q(8,8);
	Q(0,0)=16;   Q(0,1)=11;   Q(0,2)=10;   Q(0,3)=16;   Q(0,4)=24;   Q(0,5)=40;   Q(0,6)=51;   Q(0,7)=61;
	Q(1,0)=12;   Q(1,1)=12;   Q(1,2)=14;   Q(1,3)=19;   Q(1,4)=26;   Q(1,5)=58;   Q(1,6)=60;   Q(1,7)=55;
	Q(2,0)=14;   Q(2,1)=13;   Q(2,2)=16;   Q(2,3)=24;   Q(2,4)=40;   Q(2,5)=57;   Q(2,6)=69;   Q(2,7)=56;
	Q(3,0)=14;   Q(3,1)=17;   Q(3,2)=22;   Q(3,3)=29;   Q(3,4)=51;   Q(3,5)=87;   Q(3,6)=80;   Q(3,7)=62;
	Q(4,0)=18;   Q(4,1)=22;   Q(4,2)=37;   Q(4,3)=56;   Q(4,4)=68;   Q(4,5)=109;  Q(4,6)=103;  Q(4,7)=77;
	Q(5,0)=24;   Q(5,1)=35;   Q(5,2)=55;   Q(5,3)=64;   Q(5,4)=81;   Q(5,5)=104;  Q(5,6)=113;  Q(5,7)=92;
	Q(6,0)=49;   Q(6,1)=64;   Q(6,2)=78;   Q(6,3)=87;   Q(6,4)=103;  Q(6,5)=121;  Q(6,6)=120;  Q(6,7)=101;
	Q(7,0)=72;   Q(7,1)=92;   Q(7,2)=95;   Q(7,3)=98;   Q(7,4)=112;  Q(7,5)=100;  Q(7,6)=103;  Q(7,7)=99;
	Q *= quality;
	
	for( int k = 0; k < 8; ++k )
	{
		for( int l = 0; l < 8; ++l )
		{
			double somme2 = 0.0;
			
			for( int m = 0; m < 8; ++m )
			{
				double somme1 = 0.0;
				
				for( int n = 0; n < 8; ++n )
				{
					somme1 += DctCoefficient(m) * DctCoefficient(n) * ROUND( sub_image(m, n) * Q(m, n) ) * dctCos[k][m] * dctCos[l][n];
				}
				
				somme2 += somme1;
			}
			
			double dct = (2.0 / 8.0) * somme2;
			
			image( i + k, j + l) = dct;
		}
	}
}

/**
 * DCT inverse sur une image encodé
 **/ 
void InverseDct(CImg<double>& image, float quality)
{
	double ** dctCos = DctCos();
	
	for( int i = 0; i < image.height(); i += 8 )
	{
		for( int j = 0; j < image.width(); j += 8 )
		{
			CImg<double> sub_image = image.get_crop(i, j, i + 7, j + 7);
			InverseDctBlock(image, sub_image, i, j, dctCos, quality);
		}
	}
	
	for( int i = 0; i < 8; ++i )
	{
		delete[] dctCos[i];
	}
	
	delete[] dctCos;
}

/**
 * Décodage d'une image dans la norme JPEG
 **/ 
CImg<double>* JPEGDecoder(CImg<double>& image, float quality)
{
	CImg<double>* decomp = new CImg<double>(image.width(), image.height(), 1, 1, 0);
	*decomp = image;
	
	InverseDct(*decomp, quality);
	
	return decomp;
}

int main()
{
	// Read the image "lena.bmp"
	CImg<double> my_image("lena.bmp");

	// Take the luminance information 
	my_image.channel(0);


	cout << "Affichage de la DCT et de la DCT inverse" << endl;
	cout << "Facteur de qualité :" << endl;

	string qualityCmd;
	cin >> qualityCmd;
	
	float quality= atoi(qualityCmd.c_str());
	if( quality == 0 )
		quality = 1;
	
	CImg<double>* comp_image = JPEGEncoder(my_image, quality);
	CImg<double>* decomp_image = JPEGDecoder(*comp_image, quality);
	
	// Display the bmp file
	CImgDisplay main_disp(my_image,"Initial Image");

	// Display the compressed file (by dct)
	CImgDisplay comp_disp(*comp_image,"Compressed Image");
	
	// Display the decompressed file (by dct)
	CImgDisplay decomp_disp(*decomp_image,"Decompressed Image");

	while (!main_disp.is_closed());
	{
		main_disp.wait();
	}
	
	delete comp_image;
	delete decomp_image;
}

