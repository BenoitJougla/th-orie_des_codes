//
//  TP6_RSA
//  

#include <stdio.h>
#include <iostream>
#include <gmp.h>

/*
 * test de primalité de Rabin-Miller
 */ 
void nextprime(mpz_t rop, const mpz_t op)
{
	
}

/*
 *  algorithme d’Euclide étendu
 */
void euclide(mpz_t rop, const mpz_t a, const mpz_t b)
{
	if( mpz_cmp_ui(b, 0) == 0 )
	{
		mpz_set(rop, a);
		return;
	}
	
	mpz_t r;
	mpz_init(r);
	
	mpz_mod(r, a, b);
	
	 char c_str[1000];
    mpz_get_str(c_str,10,r);
    std::cout << "mod = " << c_str << std::endl << std::endl;
	
	euclide(rop, b, r);
	
	mpz_clear(r);
}

int invert(mpz_t rop, const mpz_t op1, const mpz_t op2)
{
	euclide(rop, op1, op2);
	
	 char c_str[1000];
    mpz_get_str(c_str,10,rop);
    std::cout << "rop = " << c_str << std::endl << std::endl;
	
	mpz_t abs;
	mpz_init(abs);
	
	mpz_abs(abs, op2);
	
	if( ( mpz_cmp_ui(rop, 0) > 0 || (mpz_cmp_ui(rop, 0) == 0 && mpz_cmp_ui(abs, 1) == 0) ) && mpz_cmp(rop, abs) < 0 )
		return 1;
		
	return 0;
}

/*
 * exponentiation rapide 
 */
void powm(mpz_t msg, mpz_t base, mpz_t exp, mpz_t mod)
{
	int result =  mpz_cmp_ui(exp, 0);
	
	mpz_t g, k, y;
	
	mpz_init(y);
	mpz_init(g);
	mpz_init(k);
	
	mpz_set_ui(y, 1);
	mpz_set(g, base);

	// exp < 0
	if( result < 0 ) 
	{		
		// base = 1 / base
		mpz_fdiv_q(g, y, base);
		mpz_mul_ui(k, k, -1);
	}
	// exp = 0
	else if( result == 0 )
	{
		mpz_set_ui(msg, 1);
		
		mpz_clear(y);
		mpz_clear(g);
		mpz_clear(k);
		
		return;
	}
	
	mpz_set(k, exp);
	
	// tant que exp > 1
	while( mpz_cmp_ui(k, 1) > 0 )
	{ 		
		if( mpz_even_p(k) != 0 )
		{			
			mpz_mul(g, g, g);
			mpz_mod(g, g, mod);
			
			mpz_fdiv_q_ui(k, k, 2);
		}
		else
		{
			mpz_mul(y, g, y);
			mpz_mul(g, g, g);
			
			mpz_sub_ui(k, k, 1);
			mpz_fdiv_q_ui(k, k, 2);
		}
	}
	
	mpz_mul(msg, g, y);
	mpz_mod(msg, msg, mod);
	
	mpz_clear(g);
	mpz_clear(k);
	mpz_clear(y);
}

/*function which make the rsa encryption
 * This function creates the keys.
 *
 *  1. Generate two large distinct primes p and q randomly
 *  2. Calculate n = pq and x = (p-1)(q-1)
 *  3. Select a random integer e (1<e<x) such that gcd(e,x) = 1
 *  4. Calculate the unique d such that ed = 1(mod x)
 *  5. Public key pair : (e,n), Private key pair : (d,n)
 * and return an array of mpz_t with
 * [0]: Public Keys
 * [1]: Private Keys
 * [2]: The encrypted message
 */
mpz_t* rsa_encrypt(mpz_t msg, mpz_t seed, int primes_size, mpz_t tab_res[3])
{
    /* Declare global variables */
    mpz_t d, e, n, M, c;

    /* Initialize the GMP integers */
    mpz_init(d);
    mpz_init(e);
    mpz_init(n);
    mpz_init(M);
    mpz_init(c);
    
    /*
     *  Step 1 : Get two large primes.
     */
    mpz_t p,q;
    mpz_init(p);
    mpz_init(q);
    
    gmp_randstate_t rand;
    gmp_randinit_mt(rand);
    gmp_randseed(rand, seed);
    
    // get rand numbers
    mpz_urandomb(p, rand, primes_size);
    mpz_urandomb(q, rand, primes_size);
    
    // get the next prime number
    mpz_nextprime(p, p);
    mpz_nextprime(q, q);

    char p_str[1000];
    char q_str[1000];
    mpz_get_str(p_str,10,p);
    mpz_get_str(q_str,10,q);
    
    std::cout << "Random Prime 'p' = " << p_str <<  std::endl;
    std::cout << "Random Prime 'q' = " << q_str <<  std::endl;
    
    /*
     *  Step 2 : Calculate n (=pq) ie the 1024 bit modulus
     *  and x = ((p-1)(q-1)).
     */
    char n_str[1000];
    mpz_t x;
    mpz_init(x);

    /* Calculate n... */
    mpz_mul(n,p,q);
    mpz_get_str(n_str,10,n);
    std::cout << "\t n = " << n_str << std::endl;

    /* fill the returned table*/
    mpz_set(tab_res[0], n);
    
    /* Calculate x... */
    mpz_t p_minus_1,q_minus_1;
    mpz_init(p_minus_1);
    mpz_init(q_minus_1);

    mpz_sub_ui(p_minus_1,p,(unsigned long int)1);
    mpz_sub_ui(q_minus_1,q,(unsigned long int)1);

    mpz_mul(x,p_minus_1,q_minus_1);
    char phi_str[1000];
    mpz_get_str(phi_str,10,x);
    std::cout << "\t phi(n) = " << phi_str << std::endl;

    /*
     *  Step 3 : Get small odd integer e such that gcd(e,x) = 1.
     */
    mpz_t gcd;
    mpz_init(gcd);
    
    mpz_set_ui(e, (unsigned long int)1);
   
    do
    {
        mpz_add_ui(e, e, (unsigned long int)2);
        mpz_gcd(gcd, x, e);
    } while( mpz_cmp_ui(gcd, (unsigned long int)1) != 0 );
     
    char e_str[1000];
    mpz_get_str(e_str,10,e);
    std::cout << "\t e = " << e_str << std::endl;

    /* fill the returned table*/
    mpz_set(tab_res[1], e);

    /*
     *  Step 4 : Calculate unique d such that ed = 1(mod x)
     */
    //if(mpz_invert (d, e, x) == 0)
    if(invert (d, e, x) == 0)
    {
        std::cerr << "T_T invert error" << std::endl;
        exit(-1);
    }
    
    char d_str[1000];
    mpz_get_str(d_str,10,d);
    std::cout << "\t d = " << d_str << std::endl << std::endl;

    /* fill the returned table*/
    mpz_set(tab_res[2], d);

    /*
     *  Step 5 : Print the public and private key pairs...
     */
    std::cout << "Public Keys  (e,n): ( " << e_str <<" , " << n_str << " )" << std::endl;
    std::cout << "Private Keys (d,n): ( " << d_str <<" , " << n_str << " )" << std::endl;

    /*
     *  Encrypt
     */
    mpz_t c_msg;
    mpz_init(c_msg);
    
    powm(c_msg, msg, e, n);
    
    char c_str[1000];
    mpz_get_str(c_str,10,c_msg);
    std::cout << "\t encrypted = " << c_str << std::endl << std::endl;

    /* Fill the returned table */
    mpz_set(tab_res[3], c_msg);
    
    /* Clean up the GMP integers */
    mpz_clear(p_minus_1);
    mpz_clear(q_minus_1);
    mpz_clear(x);
    mpz_clear(p);
    mpz_clear(q);
    
    mpz_clear(d);
    mpz_clear(e);
    mpz_clear(n);
    
    mpz_clear(M);
    mpz_clear(c);

    mpz_set(tab_res[3], c_msg);

    return tab_res;
}

void rsa_decrypt(mpz_t& d_msg, mpz_t* tab)
{
    powm(d_msg, tab[3], tab[2], tab[0]);
}

void rsa_random_test (int iter)
{
    mpz_t tab_res[4];

    /* init the mpz number*/
    for(int i = 0; i<4; ++i)
    {
        mpz_init(tab_res[i]);
    }

    mpz_t msg, d_msg, seed;

    mpz_init(d_msg);
    mpz_init(seed);
    mpz_init(msg);

    unsigned long int ui_seed = time(NULL);

    mpz_init_set_ui (seed, ui_seed);
    
    gmp_randstate_t rand;
    gmp_randinit_mt(rand);
    gmp_randseed(rand, seed);

    for (int i = iter; i != 0; --i)
    {
        // get rand numbers, it's the message to encrypt
        mpz_urandomb(msg, rand, 10);

        // get rand numbers, it's the seed for the two prims
        mpz_urandomb(seed, rand, 5);

        rsa_encrypt(msg, seed, 30, tab_res);

        rsa_decrypt(d_msg, tab_res);

        char msg_str[1000];
        mpz_get_str(msg_str,10, msg);

        char d_str[1000];
        mpz_get_str (d_str,10, d_msg);

        char e_str[1000];
        mpz_get_str( e_str,10, tab_res[3]);

        std::cout << "-------------------------------------------------" << std::endl;
        std::cout << "Message sent      : " << msg_str << std::endl;
        std::cout << "Message encrypted : " << e_str << std::endl;
        std::cout << "Message decrypted : " << d_str << std::endl;
        std::cout << "-------------------------------------------------" << std::endl;
    }
}

/* Main subroutine */
int main()
{
    rsa_random_test (5);

    return 0;
}