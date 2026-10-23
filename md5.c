#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define rotl32(xr, times) (((xr << times) | (xr >> (32 - times))))

#define step16                \
  {                           \
    F = (B & C) | ((~B) & D); \
    g = i;                    \
  }

#define step31                \
  {                           \
    F = (D & B) | ((~D) & C); \
    g = (5 * i + 1) % 16;     \
  }

#define step47            \
  {                       \
    F = B ^ C ^ D;        \
    g = (3 * i + 5) % 16; \
  }

#define step63          \
  {                     \
    F = C ^ (B | (~D)); \
    g = (7 * i) % 16;   \
  }

void append(uint8_t** p, int* size, uint8_t new) {
  uint8_t* _temp = (uint8_t*)realloc(*p, (*size + 1) * sizeof(uint8_t));

  if (_temp == NULL)
    return;
  *p = _temp;
  (*p)[*size] = new;
  *size += 1;
}

uint8_t* md5(uint8_t* message, int size) {
  int s[64] = {7, 12, 17, 22, 7, 12, 17, 22, 7, 12, 17, 22, 7, 12, 17, 22,
               5, 9,  14, 20, 5, 9,  14, 20, 5, 9,  14, 20, 5, 9,  14, 20,
               4, 11, 16, 23, 4, 11, 16, 23, 4, 11, 16, 23, 4, 11, 16, 23,
               6, 10, 15, 21, 6, 10, 15, 21, 6, 10, 15, 21, 6, 10, 15, 21};

  // K[i] = floor(2**32 × abs(sin(i + 1)))
  uint32_t K[64] = {
      0xd76aa478, 0xe8c7b756, 0x242070db, 0xc1bdceee, 0xf57c0faf, 0x4787c62a,
      0xa8304613, 0xfd469501, 0x698098d8, 0x8b44f7af, 0xffff5bb1, 0x895cd7be,
      0x6b901122, 0xfd987193, 0xa679438e, 0x49b40821, 0xf61e2562, 0xc040b340,
      0x265e5a51, 0xe9b6c7aa, 0xd62f105d, 0x02441453, 0xd8a1e681, 0xe7d3fbc8,
      0x21e1cde6, 0xc33707d6, 0xf4d50d87, 0x455a14ed, 0xa9e3e905, 0xfcefa3f8,
      0x676f02d9, 0x8d2a4c8a, 0xfffa3942, 0x8771f681, 0x6d9d6122, 0xfde5380c,
      0xa4beea44, 0x4bdecfa9, 0xf6bb4b60, 0xbebfbc70, 0x289b7ec6, 0xeaa127fa,
      0xd4ef3085, 0x04881d05, 0xd9d4d039, 0xe6db99e5, 0x1fa27cf8, 0xc4ac5665,
      0xf4292244, 0x432aff97, 0xab9423a7, 0xfc93a039, 0x655b59c3, 0x8f0ccc92,
      0xffeff47d, 0x85845dd1, 0x6fa87e4f, 0xfe2ce6e0, 0xa3014314, 0x4e0811a1,
      0xf7537e82, 0xbd3af235, 0x2ad7d2bb, 0xeb86d391};

  uint32_t a0 = 0x67452301;  // A
  uint32_t b0 = 0xefcdab89;  // B
  uint32_t c0 = 0x98badcfe;  // C
  uint32_t d0 = 0x10325476;  // D

  uint64_t ml = (uint64_t)(size * 8);

  append(&message, &size, 0x80);

  while (size % 64 != 56)
    append(&message, &size, 0);
  uint8_t* _temp = (uint8_t*)realloc(message, (size + 8) * sizeof(uint8_t));

  if (_temp == NULL)
    return NULL;
  message = _temp;

  message[size + 7] = (ml >> 56);
  message[size + 6] = (uint8_t)(ml >> 48);
  message[size + 5] = (uint8_t)(ml >> 40);
  message[size + 4] = (uint8_t)(ml >> 32);
  message[size + 3] = (uint8_t)(ml >> 24);
  message[size + 2] = (uint8_t)(ml >> 16);
  message[size + 1] = (uint8_t)(ml >> 8);
  message[size + 0] = (uint8_t)ml;
  size += 8;

  for (uint8_t* p = &message[0]; p < &message[0] + size; p++)
    printf("%.2x", *p);
  printf("\n");
  uint32_t M[16];
  for (uint8_t* p = &message[0]; p < &message[0] + size; p = p + 64) {
    for(int j = 0; j<16;++j)
      M[j] = (*(p + j*4 + 3) << 24) | (*(p + j*4 + 2) << 16) | (*(p + j*4 + 1) << 8) | (*(p + j*4 + 0));

    uint32_t A = a0;
    uint32_t B = b0;
    uint32_t C = c0;
    uint32_t D = d0;

    uint32_t F, g;
    for (int i = 0; i < 64; ++i) {
      if (i <= 15)
        step16
      else if (i <= 31)
         step31
      else if (i <= 47)
          step47
      else if (i <= 63)
        step63

      F = F + A + K[i] + M[g];
      A = D;
      D = C;
      C = B;
      B = B + rotl32(F, s[i]);
    }

    a0 = a0 + A;
    b0 = b0 + B;
    c0 = c0 + C;
    d0 = d0 + D;
  }

  uint8_t* v = (uint8_t*)malloc(16 * sizeof(uint8_t));

  v[3] = (uint8_t)(a0 >> 24);
  v[2] = (uint8_t)(a0 >> 16);
  v[1] = (uint8_t)(a0 >> 8);
  v[0] = (uint8_t)(a0);

  v[7] = (uint8_t)(b0 >> 24);
  v[6] = (uint8_t)(b0 >> 16);
  v[5] = (uint8_t)(b0 >> 8);
  v[4] = (uint8_t)(b0);

  v[11] = (uint8_t)(c0 >> 24);
  v[10] = (uint8_t)(c0 >> 16);
  v[9] = (uint8_t)(c0 >> 8);
  v[8] = (uint8_t)(c0);

  v[15] = (uint8_t)(d0 >> 24);
  v[14] = (uint8_t)(d0 >> 16);
  v[13] = (uint8_t)(d0 >> 8);
  v[12] = (uint8_t)(d0);

  return &v[0];
}

int main(int argc, char const* argv[]) {
  // 616263 => 900150983cd24fb0d6963f7d28e17f72
  uint8_t v1[] = {0x61, 0x62, 0x63};
  uint8_t* v = NULL;
  v = (uint8_t*)malloc(3 * sizeof(uint8_t));
  int size = 3;
  v[0] = 0x61;
  v[1] = 0x62;
  v[2] = 0x63;

  uint8_t* v2 = md5(v, size);
  for (uint8_t* i = &v2[0]; i < &v2[0] + 16; ++i)
    printf("%.2x", *i);
  printf("\n");
  free(v);
  return 0;
}
