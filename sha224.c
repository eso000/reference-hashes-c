#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>


#define rotr32(xr,times) (((xr >> times)|(xr << (32-times))))

#define round(i) {\
            uint32_t S1 = rotr32(e, 6) ^ rotr32(e, 11) ^ rotr32(e, 25);\
            uint32_t ch = (e & f) ^ ((~ e) & g);\
            uint32_t temp1 = h + S1 + ch + k[i] + w[i];\
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


uint8_t* sha224(uint8_t* message, int size)
{   
    uint32_t h0 = 0xc1059ed8;
    uint32_t h1 = 0x367cd507;
    uint32_t h2 = 0x3070dd17;
    uint32_t h3 = 0xf70e5939;
    uint32_t h4 = 0xffc00b31;
    uint32_t h5 = 0x68581511;
    uint32_t h6 = 0x64f98fa7;
    uint32_t h7 = 0xbefa4fa4;
    
    uint32_t k[] ={
        0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5, 0x3956c25b, 0x59f111f1, 0x923f82a4, 0xab1c5ed5,
        0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3, 0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174,
        0xe49b69c1, 0xefbe4786, 0x0fc19dc6, 0x240ca1cc, 0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da,
        0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7, 0xc6e00bf3, 0xd5a79147, 0x06ca6351, 0x14292967,
        0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13, 0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85,
        0xa2bfe8a1, 0xa81a664b, 0xc24b8b70, 0xc76c51a3, 0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070,
        0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5, 0x391c0cb3, 0x4ed8aa4a, 0x5b9cca4f, 0x682e6ff3,
        0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208, 0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2};


    
    uint64_t ml = (uint64_t)(size*8);
    
    int left = (size+8)%64;
    if (left)
        left = 64 -left;

    uint8_t *_temp = (uint8_t*)realloc(message, (size+8+left) * sizeof(uint8_t));
    if (_temp == NULL)
        return NULL;
    message = _temp;

    if (left)
    {
		message[size] = 0x80;
        
        memset(message+size+1,0,left-1);
    }
    message[size+left+0] = (ml>>56);
    message[size+left+1] = (uint8_t)(ml>>48);
    message[size+left+2] = (uint8_t)(ml>>40);
    message[size+left+3] = (uint8_t)(ml>>32);
    message[size+left+4] = (uint8_t)(ml>>24);
    message[size+left+5] = (uint8_t)(ml>>16);
    message[size+left+6] = (uint8_t)(ml>>8);
    message[size+left+7] = (uint8_t)ml;

    uint32_t  w[80];

    for (uint8_t *p = &message[0]; p< &message[0]+size+8; p=p+64)
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

    uint8_t *v= (uint8_t*)malloc(28 * sizeof(uint8_t));
    
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

    return &v[0];       
}

int main(int argc, char const *argv[])
{   
    //616263 => 23097d223405d8228642a477bda255b32aadbce4bda0b3f7e36c9da7
    uint8_t* v = (uint8_t *)malloc(3 * sizeof(uint8_t));
    int size  = 3;
    v[0] = (uint8_t)0x61;
    v[1] = 0x62;
    v[2] = 0x63;
     
    v = sha224(v, 3);

    for (uint8_t *i = &v[0]; i < &v[0]+28; ++i)
        printf("%.2x", *i);
    printf("\n");


    return 0;
}
