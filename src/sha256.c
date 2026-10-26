#include "hash.h"
#include "hash_util.h"

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

static inline uint32_t sha256_ch(uint32_t x, uint32_t y, uint32_t z)
{
    return (x & y) ^ ((~x) & z);
}

static inline uint32_t sha256_maj(uint32_t x, uint32_t y, uint32_t z)
{
    return (x & y) ^ (x & z) ^ (y & z);
}

static inline uint32_t sha256_big_sigma0(uint32_t x)
{
    return rotr32(x, 2) ^ rotr32(x, 13) ^ rotr32(x, 22);
}

static inline uint32_t sha256_big_sigma1(uint32_t x)
{
    return rotr32(x, 6) ^ rotr32(x, 11) ^ rotr32(x, 25);
}

static inline uint32_t sha256_small_sigma0(uint32_t x)
{
    return rotr32(x, 7) ^ rotr32(x, 18) ^ (x >> 3);
}

static inline uint32_t sha256_small_sigma1(uint32_t x)
{
    return rotr32(x, 17) ^ rotr32(x, 19) ^ (x >> 10);
}

static inline void sha256_round(uint32_t *a, uint32_t *b, uint32_t *c,
                                uint32_t *d, uint32_t *e, uint32_t *f,
                                uint32_t *g, uint32_t *h,
                                uint32_t k, uint32_t w)
{
    uint32_t temp1 = *h + sha256_big_sigma1(*e) +
                     sha256_ch(*e, *f, *g) + k + w;
    uint32_t temp2 = sha256_big_sigma0(*a) + sha256_maj(*a, *b, *c);

    *h = *g;
    *g = *f;
    *f = *e;
    *e = *d + temp1;
    *d = *c;
    *c = *b;
    *b = *a;
    *a = temp1 + temp2;
}

/* Process one 64-byte block; `state` is updated in place. */
static void sha256_process_block(uint32_t state[8], const uint8_t block[64])
{
    static const uint32_t k[64] = {
        0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5,
        0x3956c25b, 0x59f111f1, 0x923f82a4, 0xab1c5ed5,
        0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3,
        0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174,
        0xe49b69c1, 0xefbe4786, 0x0fc19dc6, 0x240ca1cc,
        0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da,
        0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7,
        0xc6e00bf3, 0xd5a79147, 0x06ca6351, 0x14292967,
        0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13,
        0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85,
        0xa2bfe8a1, 0xa81a664b, 0xc24b8b70, 0xc76c51a3,
        0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070,
        0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5,
        0x391c0cb3, 0x4ed8aa4a, 0x5b9cca4f, 0x682e6ff3,
        0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208,
        0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2};

    uint32_t w[64];

    for (unsigned i = 0; i < 16; ++i)
        w[i] = load_be32(block + 4 * i);

    for (unsigned i = 16; i < 64; ++i)
        w[i] = w[i - 16] + sha256_small_sigma0(w[i - 15]) + w[i - 7] +
               sha256_small_sigma1(w[i - 2]);

    uint32_t a = state[0];
    uint32_t b = state[1];
    uint32_t c = state[2];
    uint32_t d = state[3];
    uint32_t e = state[4];
    uint32_t f = state[5];
    uint32_t g = state[6];
    uint32_t h = state[7];

    for (unsigned i = 0; i < 64; ++i)
        sha256_round(&a, &b, &c, &d, &e, &f, &g, &h, k[i], w[i]);

    state[0] += a;
    state[1] += b;
    state[2] += c;
    state[3] += d;
    state[4] += e;
    state[5] += f;
    state[6] += g;
    state[7] += h;
}

/*
 * Finish the message: process the final `tail_len` bytes (0..63) plus
 * SHA-256 padding. `state` is updated in place.
 */
static void sha256_finish(uint32_t state[8], const uint8_t *tail,
                          size_t tail_len, uint64_t bit_len)
{
    uint8_t block[128] = {0};
    size_t final_len = tail_len < 56 ? 64 : 128;

    if (tail_len != 0)
        memcpy(block, tail, tail_len);

    block[tail_len] = 0x80;
    store_be32(block + final_len - 8, (uint32_t)(bit_len >> 32));
    store_be32(block + final_len - 4, (uint32_t)bit_len);

    for (size_t offset = 0; offset < final_len; offset += 64)
        sha256_process_block(state, block + offset);
}

uint8_t* sha256(const uint8_t* message, size_t size)
{
    uint32_t state[8] = {
        0x6a09e667u, 0xbb67ae85u, 0x3c6ef372u, 0xa54ff53au,
        0x510e527fu, 0x9b05688cu, 0x1f83d9abu, 0x5be0cd19u
    };

    if ((message == NULL && size != 0) || size > UINT64_MAX / 8 ||
        size > SIZE_MAX - 72)
        return NULL;

    uint64_t bit_len = (uint64_t)size << 3;
    size_t tail_len = size % 64;
    size_t full_len = size - tail_len;

    for (size_t offset = 0; offset < full_len; offset += 64)
        sha256_process_block(state, message + offset);

    const uint8_t *tail = tail_len == 0 ? NULL : message + full_len;
    sha256_finish(state, tail, tail_len, bit_len);

    uint8_t *v = (uint8_t*)malloc(32 * sizeof(uint8_t));

    if (v == NULL)
        return NULL;

    for (unsigned i = 0; i < 8; ++i)
        store_be32(v + 4 * i, state[i]);

    return v;
}
