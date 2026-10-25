#include "hash.h"

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>


#define rotr32(xr,times) (((xr >> times)|(xr << (32-times))))

#define k0 0x428a2f98
#define k1 0x71374491
#define k2 0xb5c0fbcf
#define k3 0xe9b5dba5
#define k4 0x3956c25b
#define k5 0x59f111f1
#define k6 0x923f82a4
#define k7 0xab1c5ed5
#define k8 0xd807aa98
#define k9 0x12835b01
#define k10 0x243185be
#define k11 0x550c7dc3
#define k12 0x72be5d74
#define k13 0x80deb1fe
#define k14 0x9bdc06a7
#define k15 0xc19bf174
#define k16 0xe49b69c1
#define k17 0xefbe4786
#define k18 0xfc19dc6
#define k19 0x240ca1cc
#define k20 0x2de92c6f
#define k21 0x4a7484aa
#define k22 0x5cb0a9dc
#define k23 0x76f988da
#define k24 0x983e5152
#define k25 0xa831c66d
#define k26 0xb00327c8
#define k27 0xbf597fc7
#define k28 0xc6e00bf3
#define k29 0xd5a79147
#define k30 0x6ca6351
#define k31 0x14292967
#define k32 0x27b70a85
#define k33 0x2e1b2138
#define k34 0x4d2c6dfc
#define k35 0x53380d13
#define k36 0x650a7354
#define k37 0x766a0abb
#define k38 0x81c2c92e
#define k39 0x92722c85
#define k40 0xa2bfe8a1
#define k41 0xa81a664b
#define k42 0xc24b8b70
#define k43 0xc76c51a3
#define k44 0xd192e819
#define k45 0xd6990624
#define k46 0xf40e3585
#define k47 0x106aa070
#define k48 0x19a4c116
#define k49 0x1e376c08
#define k50 0x2748774c
#define k51 0x34b0bcb5
#define k52 0x391c0cb3
#define k53 0x4ed8aa4a
#define k54 0x5b9cca4f
#define k55 0x682e6ff3
#define k56 0x748f82ee
#define k57 0x78a5636f
#define k58 0x84c87814
#define k59 0x8cc70208
#define k60 0x90befffa
#define k61 0xa4506ceb
#define k62 0xbef9a3f7
#define k63 0xc67178f2


#define round(i) {\
            uint32_t S1 = rotr32(e, 6) ^ rotr32(e, 11) ^ rotr32(e, 25);\
            uint32_t ch = (e & f) ^ ((~ e) & g);\
            uint32_t temp1 = h + S1 + ch + k##i + w[i];\
            uint32_t S0 = rotr32(a, 2) ^ rotr32(a, 13) ^ rotr32(a, 22);\
            uint32_t maj = (a & b) ^ (a & c) ^ (b & c);\
            uint32_t temp2 = S0 + maj;\
            \
            h = g;\
            g = f;\
            f = e;\
            e = d + temp1;\
            d = c;\
            c = b;\
            b = a;\
            a = temp1 + temp2;\
        }

#define process512block {\
        round( 0)\
        round( 1)\
        round( 2)\
        round( 3)\
        round( 4)\
        round( 5)\
        round( 6)\
        round( 7)\
        round( 8)\
        round( 9)\
        round(10)\
        round(11)\
        round(12)\
        round(13)\
        round(14)\
        round(15)\
        round(16)\
        round(17)\
        round(18)\
        round(19)\
        round(20)\
        round(21)\
        round(22)\
        round(23)\
        round(24)\
        round(25)\
        round(26)\
        round(27)\
        round(28)\
        round(29)\
        round(30)\
        round(31)\
        round(32)\
        round(33)\
        round(34)\
        round(35)\
        round(36)\
        round(37)\
        round(38)\
        round(39)\
        round(40)\
        round(41)\
        round(42)\
        round(43)\
        round(44)\
        round(45)\
        round(46)\
        round(47)\
        round(48)\
        round(49)\
        round(50)\
        round(51)\
        round(52)\
        round(53)\
        round(54)\
        round(55)\
        round(56)\
        round(57)\
        round(58)\
        round(59)\
        round(60)\
        round(61)\
        round(62)\
        round(63)\
        \
        h0 += a;\
        h1 += b;\
        h2 += c;\
        h3 += d;\
        h4 += e;\
        h5 += f;\
        h6 += g;\
        h7 += h;\
        }


uint8_t* sha256(uint8_t* message, int size)
{	
	uint32_t h0 = 0x6a09e667;
    uint32_t h1 = 0xbb67ae85;
    uint32_t h2 = 0x3c6ef372;
    uint32_t h3 = 0xa54ff53a;
    uint32_t h4 = 0x510e527f;
    uint32_t h5 = 0x9b05688c;
    uint32_t h6 = 0x1f83d9ab;
    uint32_t h7 = 0x5be0cd19;
	
    uint64_t ml = (uint64_t)size * 8;
    
    int left = 64 - (size+8)%64;

    uint8_t *_temp = (uint8_t*)malloc((size+8+left) * sizeof(uint8_t));

    if (_temp == NULL)
        return NULL;

    memcpy(_temp, message, size);
    message = _temp;

    message[size] = 0x80;
    memset(message+size+1,0,left-1);
    message[size+left+0] = (ml>>56);
    message[size+left+1] = (uint8_t)(ml>>48);
    message[size+left+2] = (uint8_t)(ml>>40);
    message[size+left+3] = (uint8_t)(ml>>32);
    message[size+left+4] = (uint8_t)(ml>>24);
    message[size+left+5] = (uint8_t)(ml>>16);
    message[size+left+6] = (uint8_t)(ml>>8);
    message[size+left+7] = (uint8_t)ml;

    uint32_t  w[80];

	for (uint8_t *p = &message[0]; p< &message[0]+size+left+8; p=p+64)
	{   	        
        

        w[ 0] = (*(p   )<<24)|(*(p+ 1)<<16)|(*(p+ 2)<<8)|(*(p+ 3)); 
        w[ 1] = (*(p+ 4)<<24)|(*(p+ 5)<<16)|(*(p+ 6)<<8)|(*(p+ 7)); 
        w[ 2] = (*(p+ 8)<<24)|(*(p+ 9)<<16)|(*(p+10)<<8)|(*(p+11)); 
		w[ 3] = (*(p+12)<<24)|(*(p+13)<<16)|(*(p+14)<<8)|(*(p+15)); 
	    w[ 4] = (*(p+16)<<24)|(*(p+17)<<16)|(*(p+18)<<8)|(*(p+19)); 
        w[ 5] = (*(p+20)<<24)|(*(p+21)<<16)|(*(p+22)<<8)|(*(p+23)); 
        w[ 6] = (*(p+24)<<24)|(*(p+25)<<16)|(*(p+26)<<8)|(*(p+27)); 
        w[ 7] = (*(p+28)<<24)|(*(p+29)<<16)|(*(p+30)<<8)|(*(p+31)); 
        w[ 8] = (*(p+32)<<24)|(*(p+33)<<16)|(*(p+34)<<8)|(*(p+35)); 
        w[ 9] = (*(p+36)<<24)|(*(p+37)<<16)|(*(p+38)<<8)|(*(p+39)); 
        w[10] = (*(p+40)<<24)|(*(p+41)<<16)|(*(p+42)<<8)|(*(p+43)); 
        w[11] = (*(p+44)<<24)|(*(p+45)<<16)|(*(p+46)<<8)|(*(p+47)); 
        w[12] = (*(p+48)<<24)|(*(p+49)<<16)|(*(p+50)<<8)|(*(p+51)); 
        w[13] = (*(p+52)<<24)|(*(p+53)<<16)|(*(p+54)<<8)|(*(p+55)); 
        w[14] = (*(p+56)<<24)|(*(p+57)<<16)|(*(p+58)<<8)|(*(p+59)); 
        w[15] = (*(p+60)<<24)|(*(p+61)<<16)|(*(p+62)<<8)|(*(p+63)); 
        

        for (int i = 16; i< 64; ++i)
	    {
	        uint32_t s0 = (rotr32(w[i-15], 7)) ^ (rotr32(w[i-15], 18)) ^ (w[i-15] >> 3);
            uint32_t s1 = rotr32(w[i-2], 17) ^ rotr32(w[i-2], 19) ^ (w[i-2] >> 10);
            w[i] = w[i-16] + s0 + w[i-7] + s1;
	    }

        uint32_t a = h0;
        uint32_t b = h1;
        uint32_t c = h2;
        uint32_t d = h3;
        uint32_t e = h4;
        uint32_t f = h5;
        uint32_t g = h6;
        uint32_t h = h7;
    
                
        process512block

	}

    uint8_t *v= (uint8_t*)malloc(32 * sizeof(uint8_t));
    
    v[ 0] = (uint8_t)(h0>>24);
    v[ 1] = (uint8_t)(h0>>16);
    v[ 2] = (uint8_t)(h0>> 8);
    v[ 3] = (uint8_t)(h0    );

    v[ 4] = (uint8_t)(h1>>24);
    v[ 5] = (uint8_t)(h1>>16);
    v[ 6] = (uint8_t)(h1>> 8);
    v[ 7] = (uint8_t)(h1    );

    v[ 8] = (uint8_t)(h2>>24);
    v[ 9] = (uint8_t)(h2>>16);
    v[10] = (uint8_t)(h2>> 8);
    v[11] = (uint8_t)(h2    );

    v[12] = (uint8_t)(h3>>24);
    v[13] = (uint8_t)(h3>>16);
    v[14] = (uint8_t)(h3>> 8);
    v[15] = (uint8_t)(h3    );

    v[16] = (uint8_t)(h4>>24);
    v[17] = (uint8_t)(h4>>16);
    v[18] = (uint8_t)(h4>> 8);
    v[19] = (uint8_t)(h4    );

    v[20] = (uint8_t)(h5>>24);
    v[21] = (uint8_t)(h5>>16);
    v[22] = (uint8_t)(h5>> 8);
    v[23] = (uint8_t)(h5    );

    v[24] = (uint8_t)(h6>>24);
    v[25] = (uint8_t)(h6>>16);
    v[26] = (uint8_t)(h6>> 8);
    v[27] = (uint8_t)(h6    );

    v[28] = (uint8_t)(h7>>24);
    v[29] = (uint8_t)(h7>>16);
    v[30] = (uint8_t)(h7>> 8);
    v[31] = (uint8_t)(h7    );

    free(message);
    return &v[0];       
}

int main(void)
{   
    //616263 => ba7816bf8f01cfea414140de5dae2223b00361a396177a9cb410ff61f20015ad
    uint8_t* v = (uint8_t *)malloc(3 * sizeof(uint8_t));
    v[0] = (uint8_t)0x61;
    v[1] = 0x62;
    v[2] = 0x63;
     
    uint8_t* digest = sha256(v, 3);

    for (uint8_t *i = &digest[0]; i < &digest[0]+32; ++i)
        printf("%.2x", *i);
    printf("\n");

    free(digest);
    free(v);

    return 0;
  
}
