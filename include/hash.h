#ifndef HASH_H
#define HASH_H

#include <stdint.h>

/*
 * One-shot hash functions.
 *
 * Each function reads `size` bytes from `message` and returns a newly
 * allocated digest buffer, or NULL when allocation fails. The caller owns the
 * returned buffer and must release it with free().
 *
 * Digest lengths:
 *   md5     16 bytes
 *   sha1    20 bytes
 *   sha224  28 bytes
 *   sha256  32 bytes
 *   sha384  48 bytes
 *   sha512  64 bytes
 *
 * The input buffer is not modified.
 */

uint8_t* md5(uint8_t* message, int size);
uint8_t* sha1(uint8_t* message, int size);
uint8_t* sha224(uint8_t* message, int size);
uint8_t* sha256(uint8_t* message, int size);
uint8_t* sha384(uint8_t* message, int size);
uint8_t* sha512(uint8_t* message, int size);

#endif
