#include "hash.h"
#include "hash_util.h"

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

static inline uint32_t md5_f(uint32_t b, uint32_t c, uint32_t d) { return (b & c) | ((~b) & d); }

static inline uint32_t md5_g(uint32_t b, uint32_t c, uint32_t d) { return (d & b) | ((~d) & c); }

static inline uint32_t md5_h(uint32_t b, uint32_t c, uint32_t d) { return b ^ c ^ d; }

static inline uint32_t md5_i(uint32_t b, uint32_t c, uint32_t d) { return c ^ (b | (~d)); }

static inline void md5_round(uint32_t *a, uint32_t *b, uint32_t *c, uint32_t *d, uint32_t f,
                             uint32_t k, uint32_t m, unsigned s) {
    uint32_t temp = *d;

    *d = *c;
    *c = *b;
    *b = *b + rotl32(*a + f + k + m, s);
    *a = temp;
}

/* Process one 64-byte block; `state` is updated in place. */
static void md5_process_block(uint32_t state[4], const uint8_t block[64]) {
    static const unsigned s[64] = {7, 12, 17, 22, 7, 12, 17, 22, 7, 12, 17, 22, 7, 12, 17, 22,
                                   5, 9,  14, 20, 5, 9,  14, 20, 5, 9,  14, 20, 5, 9,  14, 20,
                                   4, 11, 16, 23, 4, 11, 16, 23, 4, 11, 16, 23, 4, 11, 16, 23,
                                   6, 10, 15, 21, 6, 10, 15, 21, 6, 10, 15, 21, 6, 10, 15, 21};

    // K[i] = floor(2**32 × abs(sin(i + 1)))
    static const uint32_t K[64] = {
        0xd76aa478, 0xe8c7b756, 0x242070db, 0xc1bdceee, 0xf57c0faf, 0x4787c62a, 0xa8304613,
        0xfd469501, 0x698098d8, 0x8b44f7af, 0xffff5bb1, 0x895cd7be, 0x6b901122, 0xfd987193,
        0xa679438e, 0x49b40821, 0xf61e2562, 0xc040b340, 0x265e5a51, 0xe9b6c7aa, 0xd62f105d,
        0x02441453, 0xd8a1e681, 0xe7d3fbc8, 0x21e1cde6, 0xc33707d6, 0xf4d50d87, 0x455a14ed,
        0xa9e3e905, 0xfcefa3f8, 0x676f02d9, 0x8d2a4c8a, 0xfffa3942, 0x8771f681, 0x6d9d6122,
        0xfde5380c, 0xa4beea44, 0x4bdecfa9, 0xf6bb4b60, 0xbebfbc70, 0x289b7ec6, 0xeaa127fa,
        0xd4ef3085, 0x04881d05, 0xd9d4d039, 0xe6db99e5, 0x1fa27cf8, 0xc4ac5665, 0xf4292244,
        0x432aff97, 0xab9423a7, 0xfc93a039, 0x655b59c3, 0x8f0ccc92, 0xffeff47d, 0x85845dd1,
        0x6fa87e4f, 0xfe2ce6e0, 0xa3014314, 0x4e0811a1, 0xf7537e82, 0xbd3af235, 0x2ad7d2bb,
        0xeb86d391};

    uint32_t M[16];
    for (unsigned j = 0; j < 16; ++j)
        M[j] = load_le32(block + 4 * j);

    uint32_t A = state[0];
    uint32_t B = state[1];
    uint32_t C = state[2];
    uint32_t D = state[3];

    for (unsigned i = 0; i < 16; ++i) {
        uint32_t f = md5_f(B, C, D);
        md5_round(&A, &B, &C, &D, f, K[i], M[i], s[i]);
    }

    for (unsigned i = 16; i < 32; ++i) {
        uint32_t f = md5_g(B, C, D);
        unsigned g = (5 * i + 1) % 16;
        md5_round(&A, &B, &C, &D, f, K[i], M[g], s[i]);
    }

    for (unsigned i = 32; i < 48; ++i) {
        uint32_t f = md5_h(B, C, D);
        unsigned g = (3 * i + 5) % 16;
        md5_round(&A, &B, &C, &D, f, K[i], M[g], s[i]);
    }

    for (unsigned i = 48; i < 64; ++i) {
        uint32_t f = md5_i(B, C, D);
        unsigned g = (7 * i) % 16;
        md5_round(&A, &B, &C, &D, f, K[i], M[g], s[i]);
    }

    state[0] += A;
    state[1] += B;
    state[2] += C;
    state[3] += D;
}

/*
 * Finish the message: process the final `tail_len` bytes (0..63) plus
 * MD5 padding. `state` is updated in place.
 */
static void md5_finish(uint32_t state[4], const uint8_t *tail, size_t tail_len, uint64_t bit_len) {
    uint8_t block[128] = {0};
    size_t final_len = tail_len < 56 ? 64 : 128;

    if (tail_len != 0)
        memcpy(block, tail, tail_len);

    block[tail_len] = 0x80;
    store_le32(block + final_len - 8, (uint32_t)bit_len);
    store_le32(block + final_len - 4, (uint32_t)(bit_len >> 32));

    for (size_t offset = 0; offset < final_len; offset += 64)
        md5_process_block(state, block + offset);
}

uint8_t *md5(const uint8_t *message, size_t size) {
    uint32_t state[4] = {0x67452301u, 0xefcdab89u, 0x98badcfeu, 0x10325476u};

    if ((message == NULL && size != 0) || size > UINT64_MAX / 8 || size > SIZE_MAX - 72)
        return NULL;

    uint64_t bit_len = (uint64_t)size << 3;
    size_t tail_len = size % 64;
    size_t full_len = size - tail_len;

    for (size_t offset = 0; offset < full_len; offset += 64)
        md5_process_block(state, message + offset);

    const uint8_t *tail = tail_len == 0 ? NULL : message + full_len;
    md5_finish(state, tail, tail_len, bit_len);

    uint8_t *v = (uint8_t *)malloc(16 * sizeof(uint8_t));

    if (v == NULL)
        return NULL;

    for (unsigned i = 0; i < 4; ++i)
        store_le32(v + 4 * i, state[i]);

    return &v[0];
}
