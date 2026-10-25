#include "hash.h"

#include <stdint.h>
#include <stdlib.h>
#include <string.h>



#define rotr64(xr,times) (((xr >> times)|(xr << (64-times))))

#define round(i) {\
            uint64_t S1 = rotr64(e, 14) ^ rotr64(e, 18) ^ rotr64(e, 41);\
            uint64_t ch = (e & f) ^ ((~ e) & g);\
            uint64_t temp1 = h + S1 + ch + k[i] + w[i];\
            uint64_t S0 = rotr64(a, 28) ^ rotr64(a, 34) ^ rotr64(a, 39);\
            uint64_t maj = (a & b) ^ (a & c) ^ (b & c);\
            uint64_t temp2 = S0 + maj;\
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

#define process1024block {\
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
        round(64)\
        round(65)\
        round(66)\
        round(67)\
        round(68)\
        round(69)\
        round(70)\
        round(71)\
        round(72)\
        round(73)\
        round(74)\
        round(75)\
        round(76)\
        round(77)\
        round(78)\
        round(79)\
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


uint8_t* sha384(const uint8_t* message, size_t size)
{   
    uint64_t h0 = 0xcbbb9d5dc1059ed8;
    uint64_t h1 = 0x629a292a367cd507;
    uint64_t h2 = 0x9159015a3070dd17;
    uint64_t h3 = 0x152fecd8f70e5939;
    uint64_t h4 = 0x67332667ffc00b31;
    uint64_t h5 = 0x8eb44a8768581511;
    uint64_t h6 = 0xdb0c2e0d64f98fa7;
    uint64_t h7 = 0x47b5481dbefa4fa4;
    
    static const uint64_t k[] ={
        0x428a2f98d728ae22, 0x7137449123ef65cd, 0xb5c0fbcfec4d3b2f, 0xe9b5dba58189dbbc, 0x3956c25bf348b538, 
        0x59f111f1b605d019, 0x923f82a4af194f9b, 0xab1c5ed5da6d8118, 0xd807aa98a3030242, 0x12835b0145706fbe, 
        0x243185be4ee4b28c, 0x550c7dc3d5ffb4e2, 0x72be5d74f27b896f, 0x80deb1fe3b1696b1, 0x9bdc06a725c71235, 
        0xc19bf174cf692694, 0xe49b69c19ef14ad2, 0xefbe4786384f25e3, 0x0fc19dc68b8cd5b5, 0x240ca1cc77ac9c65, 
        0x2de92c6f592b0275, 0x4a7484aa6ea6e483, 0x5cb0a9dcbd41fbd4, 0x76f988da831153b5, 0x983e5152ee66dfab, 
        0xa831c66d2db43210, 0xb00327c898fb213f, 0xbf597fc7beef0ee4, 0xc6e00bf33da88fc2, 0xd5a79147930aa725, 
        0x06ca6351e003826f, 0x142929670a0e6e70, 0x27b70a8546d22ffc, 0x2e1b21385c26c926, 0x4d2c6dfc5ac42aed, 
        0x53380d139d95b3df, 0x650a73548baf63de, 0x766a0abb3c77b2a8, 0x81c2c92e47edaee6, 0x92722c851482353b, 
        0xa2bfe8a14cf10364, 0xa81a664bbc423001, 0xc24b8b70d0f89791, 0xc76c51a30654be30, 0xd192e819d6ef5218, 
        0xd69906245565a910, 0xf40e35855771202a, 0x106aa07032bbd1b8, 0x19a4c116b8d2d0c8, 0x1e376c085141ab53, 
        0x2748774cdf8eeb99, 0x34b0bcb5e19b48a8, 0x391c0cb3c5c95a63, 0x4ed8aa4ae3418acb, 0x5b9cca4f7763e373, 
        0x682e6ff3d6b2b8a3, 0x748f82ee5defb2fc, 0x78a5636f43172f60, 0x84c87814a1f0ab72, 0x8cc702081a6439ec, 
        0x90befffa23631e28, 0xa4506cebde82bde9, 0xbef9a3f7b2c67915, 0xc67178f2e372532b, 0xca273eceea26619c, 
        0xd186b8c721c0c207, 0xeada7dd6cde0eb1e, 0xf57d4f7fee6ed178, 0x06f067aa72176fba, 0x0a637dc5a2c898a6, 
        0x113f9804bef90dae, 0x1b710b35131c471b, 0x28db77f523047d84, 0x32caab7b40c72493, 0x3c9ebe0a15c9bebc, 
        0x431d67c49c100d4c, 0x4cc5d4becb3e42b6, 0x597f299cfc657e2a, 0x5fcb6fab3ad6faec, 0x6c44198c4a475817};

    
    if ((message == NULL && size != 0) || size > SIZE_MAX - 144)
        return NULL;

    uint64_t bit_len_high = (uint64_t)size >> 61;
    uint64_t bit_len_low = (uint64_t)size << 3;

    size_t left = 128 - ((size + 16) % 128);

    uint8_t* padded = (uint8_t*)malloc(size + 16 + left);

    if (padded == NULL)
        return NULL;

    if (size != 0)
        memcpy(padded, message, size);

    padded[size] = 0x80;
    memset(padded + size + 1, 0, left - 1);

    for (int i = 0; i < 8; ++i) {
        padded[size + left + i] = (uint8_t)(bit_len_high >> (56 - 8 * i));
        padded[size + left + 8 + i] = (uint8_t)(bit_len_low >> (56 - 8 * i));
    }

    uint64_t  w[80];

    for (uint8_t *p = padded; p < padded + size + left + 16; p = p + 128)
    {               
        w[ 0]= ((uint64_t)*(p    )<<56)|((uint64_t)*(p+  1)<<48)|((uint64_t)*(p+  2)<<40)|((uint64_t)*(p+  3)<<32)|((uint64_t)*(p+  4)<<24)|((uint64_t)*(p+  5)<<16)|((uint64_t)*(p+  6)<<8)|(uint64_t)*(p+  7) ;
        w[ 1]= ((uint64_t)*(p+  8)<<56)|((uint64_t)*(p+  9)<<48)|((uint64_t)*(p+ 10)<<40)|((uint64_t)*(p+ 11)<<32)|((uint64_t)*(p+ 12)<<24)|((uint64_t)*(p+ 13)<<16)|((uint64_t)*(p+ 14)<<8)|(uint64_t)*(p+ 15) ;
        w[ 2]= ((uint64_t)*(p+ 16)<<56)|((uint64_t)*(p+ 17)<<48)|((uint64_t)*(p+ 18)<<40)|((uint64_t)*(p+ 19)<<32)|((uint64_t)*(p+ 20)<<24)|((uint64_t)*(p+ 21)<<16)|((uint64_t)*(p+ 22)<<8)|(uint64_t)*(p+ 23) ;
        w[ 3]= ((uint64_t)*(p+ 24)<<56)|((uint64_t)*(p+ 25)<<48)|((uint64_t)*(p+ 26)<<40)|((uint64_t)*(p+ 27)<<32)|((uint64_t)*(p+ 28)<<24)|((uint64_t)*(p+ 29)<<16)|((uint64_t)*(p+ 30)<<8)|(uint64_t)*(p+ 31) ;
        w[ 4]= ((uint64_t)*(p+ 32)<<56)|((uint64_t)*(p+ 33)<<48)|((uint64_t)*(p+ 34)<<40)|((uint64_t)*(p+ 35)<<32)|((uint64_t)*(p+ 36)<<24)|((uint64_t)*(p+ 37)<<16)|((uint64_t)*(p+ 38)<<8)|(uint64_t)*(p+ 39) ;
        w[ 5]= ((uint64_t)*(p+ 40)<<56)|((uint64_t)*(p+ 41)<<48)|((uint64_t)*(p+ 42)<<40)|((uint64_t)*(p+ 43)<<32)|((uint64_t)*(p+ 44)<<24)|((uint64_t)*(p+ 45)<<16)|((uint64_t)*(p+ 46)<<8)|(uint64_t)*(p+ 47) ;
        w[ 6]= ((uint64_t)*(p+ 48)<<56)|((uint64_t)*(p+ 49)<<48)|((uint64_t)*(p+ 50)<<40)|((uint64_t)*(p+ 51)<<32)|((uint64_t)*(p+ 52)<<24)|((uint64_t)*(p+ 53)<<16)|((uint64_t)*(p+ 54)<<8)|(uint64_t)*(p+ 55) ;
        w[ 7]= ((uint64_t)*(p+ 56)<<56)|((uint64_t)*(p+ 57)<<48)|((uint64_t)*(p+ 58)<<40)|((uint64_t)*(p+ 59)<<32)|((uint64_t)*(p+ 60)<<24)|((uint64_t)*(p+ 61)<<16)|((uint64_t)*(p+ 62)<<8)|(uint64_t)*(p+ 63) ;
        w[ 8]= ((uint64_t)*(p+ 64)<<56)|((uint64_t)*(p+ 65)<<48)|((uint64_t)*(p+ 66)<<40)|((uint64_t)*(p+ 67)<<32)|((uint64_t)*(p+ 68)<<24)|((uint64_t)*(p+ 69)<<16)|((uint64_t)*(p+ 70)<<8)|(uint64_t)*(p+ 71) ;
        w[ 9]= ((uint64_t)*(p+ 72)<<56)|((uint64_t)*(p+ 73)<<48)|((uint64_t)*(p+ 74)<<40)|((uint64_t)*(p+ 75)<<32)|((uint64_t)*(p+ 76)<<24)|((uint64_t)*(p+ 77)<<16)|((uint64_t)*(p+ 78)<<8)|(uint64_t)*(p+ 79) ;
        w[10]= ((uint64_t)*(p+ 80)<<56)|((uint64_t)*(p+ 81)<<48)|((uint64_t)*(p+ 82)<<40)|((uint64_t)*(p+ 83)<<32)|((uint64_t)*(p+ 84)<<24)|((uint64_t)*(p+ 85)<<16)|((uint64_t)*(p+ 86)<<8)|(uint64_t)*(p+ 87) ;
        w[11]= ((uint64_t)*(p+ 88)<<56)|((uint64_t)*(p+ 89)<<48)|((uint64_t)*(p+ 90)<<40)|((uint64_t)*(p+ 91)<<32)|((uint64_t)*(p+ 92)<<24)|((uint64_t)*(p+ 93)<<16)|((uint64_t)*(p+ 94)<<8)|(uint64_t)*(p+ 95) ;
        w[12]= ((uint64_t)*(p+ 96)<<56)|((uint64_t)*(p+ 97)<<48)|((uint64_t)*(p+ 98)<<40)|((uint64_t)*(p+ 99)<<32)|((uint64_t)*(p+100)<<24)|((uint64_t)*(p+101)<<16)|((uint64_t)*(p+102)<<8)|(uint64_t)*(p+103) ;
        w[13]= ((uint64_t)*(p+104)<<56)|((uint64_t)*(p+105)<<48)|((uint64_t)*(p+106)<<40)|((uint64_t)*(p+107)<<32)|((uint64_t)*(p+108)<<24)|((uint64_t)*(p+109)<<16)|((uint64_t)*(p+110)<<8)|(uint64_t)*(p+111) ;
        w[14]= ((uint64_t)*(p+112)<<56)|((uint64_t)*(p+113)<<48)|((uint64_t)*(p+114)<<40)|((uint64_t)*(p+115)<<32)|((uint64_t)*(p+116)<<24)|((uint64_t)*(p+117)<<16)|((uint64_t)*(p+118)<<8)|(uint64_t)*(p+119) ;
        w[15]= ((uint64_t)*(p+120)<<56)|((uint64_t)*(p+121)<<48)|((uint64_t)*(p+122)<<40)|((uint64_t)*(p+123)<<32)|((uint64_t)*(p+124)<<24)|((uint64_t)*(p+125)<<16)|((uint64_t)*(p+126)<<8)|(uint64_t)*(p+127) ;
        
        for (int i = 16; i< 80; ++i)
        {
            uint64_t s0 = (rotr64(w[i-15], 1)) ^ (rotr64(w[i-15], 8)) ^ (w[i-15] >> 7);
            uint64_t s1 = rotr64(w[i-2], 19) ^ rotr64(w[i-2], 61) ^ (w[i-2] >> 6);
            w[i] = w[i-16] + s0 + w[i-7] + s1;
        }

        uint64_t a = h0;
        uint64_t b = h1;
        uint64_t c = h2;
        uint64_t d = h3;
        uint64_t e = h4;
        uint64_t f = h5;
        uint64_t g = h6;
        uint64_t h = h7;
    
                
        process1024block

    }

    uint8_t *v= (uint8_t*)malloc(48 * sizeof(uint8_t));

    if (v == NULL) {
        free(padded);
        return NULL;
    }

    v[ 0] = (uint8_t)(h0>>56);
    v[ 1] = (uint8_t)(h0>>48);
    v[ 2] = (uint8_t)(h0>>40);
    v[ 3] = (uint8_t)(h0>>32);
    v[ 4] = (uint8_t)(h0>>24);
    v[ 5] = (uint8_t)(h0>>16);
    v[ 6] = (uint8_t)(h0>> 8);
    v[ 7] = (uint8_t)(h0    );

    v[ 8] = (uint8_t)(h1>>56);
    v[ 9] = (uint8_t)(h1>>48);
    v[10] = (uint8_t)(h1>>40);
    v[11] = (uint8_t)(h1>>32);
    v[12] = (uint8_t)(h1>>24);
    v[13] = (uint8_t)(h1>>16);
    v[14] = (uint8_t)(h1>> 8);
    v[15] = (uint8_t)(h1    );
    
    v[16] = (uint8_t)(h2>>56);
    v[17] = (uint8_t)(h2>>48);
    v[18] = (uint8_t)(h2>>40);
    v[19] = (uint8_t)(h2>>32);
    v[20] = (uint8_t)(h2>>24);
    v[21] = (uint8_t)(h2>>16);
    v[22] = (uint8_t)(h2>> 8);
    v[23] = (uint8_t)(h2    );

    v[24] = (uint8_t)(h3>>56);
    v[25] = (uint8_t)(h3>>48);
    v[26] = (uint8_t)(h3>>40);
    v[27] = (uint8_t)(h3>>32);
    v[28] = (uint8_t)(h3>>24);
    v[29] = (uint8_t)(h3>>16);
    v[30] = (uint8_t)(h3>> 8);
    v[31] = (uint8_t)(h3    );

    v[32] = (uint8_t)(h4>>56);
    v[33] = (uint8_t)(h4>>48);
    v[34] = (uint8_t)(h4>>40);
    v[35] = (uint8_t)(h4>>32);
    v[36] = (uint8_t)(h4>>24);
    v[37] = (uint8_t)(h4>>16);
    v[38] = (uint8_t)(h4>> 8);
    v[39] = (uint8_t)(h4    );

    v[40] = (uint8_t)(h5>>56);
    v[41] = (uint8_t)(h5>>48);
    v[42] = (uint8_t)(h5>>40);
    v[43] = (uint8_t)(h5>>32);
    v[44] = (uint8_t)(h5>>24);
    v[45] = (uint8_t)(h5>>16);
    v[46] = (uint8_t)(h5>> 8);
    v[47] = (uint8_t)(h5    );

    free(padded);
    return v;
}
