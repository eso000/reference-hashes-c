#include "hash.h"

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#include <string.h>


#define rotl32(xr,times) (((xr << times)|(xr >> (32-times))))

#define f1 f = (b & c) | ((~b) & d);

#define f2 f = b ^ c ^ d;

#define f3 f = (b & c) | (b & d) | (c & d);

#define end(i) temp = rotl32(a, 5) + f + e + k + w[i];\
        e = d;\
        d = c;\
        c = rotl32(b,30);\
        b = a;\
        a = temp;

#define process512block {\
    uint32_t k = 0x5A827999;\
    f1 end( 0)\
    f1 end( 1)\
    f1 end( 2)\
    f1 end( 3)\
    f1 end( 4)\
    f1 end( 5)\
    f1 end( 6)\
    f1 end( 7)\
    f1 end( 8)\
    f1 end( 9)\
    f1 end(10)\
    f1 end(11)\
    f1 end(12)\
    f1 end(13)\
    f1 end(14)\
    f1 end(15)\
    f1 end(16)\
    f1 end(17)\
    f1 end(18)\
    f1 end(19)\
    k = 0x6ED9EBA1;\
    f2 end(20)\
    f2 end(21)\
    f2 end(22)\
    f2 end(23)\
    f2 end(24)\
    f2 end(25)\
    f2 end(26)\
    f2 end(27)\
    f2 end(28)\
    f2 end(29)\
    f2 end(30)\
    f2 end(31)\
    f2 end(32)\
    f2 end(33)\
    f2 end(34)\
    f2 end(35)\
    f2 end(36)\
    f2 end(37)\
    f2 end(38)\
    f2 end(39)\
    k = 0x8F1BBCDC;\
    f3 end(40)\
    f3 end(41)\
    f3 end(42)\
    f3 end(43)\
    f3 end(44)\
    f3 end(45)\
    f3 end(46)\
    f3 end(47)\
    f3 end(48)\
    f3 end(49)\
    f3 end(50)\
    f3 end(51)\
    f3 end(52)\
    f3 end(53)\
    f3 end(54)\
    f3 end(55)\
    f3 end(56)\
    f3 end(57)\
    f3 end(58)\
    f3 end(59)\
    k = 0xCA62C1D6;\
    f2 end(60)\
    f2 end(61)\
    f2 end(62)\
    f2 end(63)\
    f2 end(64)\
    f2 end(65)\
    f2 end(66)\
    f2 end(67)\
    f2 end(68)\
    f2 end(69)\
    f2 end(70)\
    f2 end(71)\
    f2 end(72)\
    f2 end(73)\
    f2 end(74)\
    f2 end(75)\
    f2 end(76)\
    f2 end(77)\
    f2 end(78)\
    f2 end(79)\
    h0 = h0 + a;\
    h1 = h1 + b;\
    h2 = h2 + c;\
    h3 = h3 + d;\
    h4 = h4 + e;\
    }

uint8_t* sha1(uint8_t * message, int size)
{
uint32_t h0 = 0x67452301;
uint32_t h1 = 0xEFCDAB89;
uint32_t h2 = 0x98BADCFE;
uint32_t h3 = 0x10325476;
uint32_t h4 = 0xC3D2E1F0;


    uint32_t temp;
    uint32_t f;

    uint64_t ml = (uint64_t)size * 8;

    int left = 64 - (size+8)%64;

    uint8_t *_temp = (uint8_t*)malloc((size+8+left) * sizeof(uint8_t));

    if (_temp == NULL)
        return NULL;

    memcpy(_temp, message, size);
    message = (uint8_t *)_temp;

    message[size] = 0x80;
    memset(message+size +1 , 0, left-1);
    message[size+left+0] = (ml>>56);
    message[size+left+1] = (uint8_t)(ml>>48);
    message[size+left+2] = (uint8_t)(ml>>40);
    message[size+left+3] = (uint8_t)(ml>>32);
    message[size+left+4] = (uint8_t)(ml>>24);
    message[size+left+5] = (uint8_t)(ml>>16);
    message[size+left+6] = (uint8_t)(ml>>8);
    message[size+left+7] = (uint8_t)ml;

    uint32_t  w[80];

for (uint8_t *p = &message[0]; p<&message[0]+size+left+8; p=p+64)
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

        for (int i = 16; i< 80; ++i)
    {
        w[i] = w[i-3] ^ w[i-8] ^ w[i-14] ^ w[i-16];
        w[i] = rotl32(w[i],1);
    }

        uint32_t a = h0;
        uint32_t b = h1;
        uint32_t c = h2;
        uint32_t d = h3;
        uint32_t e = h4;


        process512block

}
    uint8_t *v= (uint8_t*)malloc(20 * sizeof(uint8_t));

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

    free(message);
    return &v[0];
}

int main(void)
{
    //616263 => a9993e364706816aba3e25717850c26c9cd0d89d
    uint8_t v1[] = {0x61, 0x62, 0x63};
    uint8_t *v;
    v = (uint8_t *)malloc(3 * sizeof(uint8_t));
    int size  = 3;
    memcpy(v,v1,3);

    //std::vector<uint8_t> v({0x61, 0x62, 0x63});
    uint8_t* digest = sha1(v, size);
    for (uint8_t *i = &digest[0]; i < &digest[0]+20; ++i)
        printf("%.2x", *i);
    printf("\n");
    free(digest);
    free(v);
    return 0;
}
