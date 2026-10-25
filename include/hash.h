#ifndef HASH_H
#define HASH_H

#include <stddef.h>
#include <stdint.h>

/*
 * One-shot hash functions.
 *
 * Each function reads `size` bytes from `message` and returns a newly
 * allocated digest buffer, or NULL on failure. The caller owns the returned
 * buffer and must release it with free().
 *
 * Contract:
 *   - `message` may be NULL only when `size` is 0. A NULL message with a
 *     non-zero size is rejected.
 *   - `size` is a byte count. It is a size_t, so a negative length is not
 *     representable; an argument that converts to an oversized size_t is
 *     rejected.
 *   - The input buffer is not modified.
 *   - Inputs that would overflow the algorithm's length field or the internal
 *     padding calculation are rejected. On 64-bit platforms this means
 *     md5/sha1/sha224/sha256 reject sizes above 2^61 - 1 bytes; sha384/sha512
 *     accept any size that can be padded without overflowing size_t, subject
 *     to available memory.
 *   - NULL is returned on allocation failure or invalid arguments. The API
 *     does not distinguish between those cases.
 *   - The functions are reentrant and use no global state.
 *
 * Digest lengths:
 *   md5     16 bytes
 *   sha1    20 bytes
 *   sha224  28 bytes
 *   sha256  32 bytes
 *   sha384  48 bytes
 *   sha512  64 bytes
 */

uint8_t* md5(const uint8_t* message, size_t size);
uint8_t* sha1(const uint8_t* message, size_t size);
uint8_t* sha224(const uint8_t* message, size_t size);
uint8_t* sha256(const uint8_t* message, size_t size);
uint8_t* sha384(const uint8_t* message, size_t size);
uint8_t* sha512(const uint8_t* message, size_t size);

#endif
