#ifndef SHA2_32_H
#define SHA2_32_H

#include <stddef.h>
#include <stdint.h>

/*
 * Internal one-shot SHA-224/SHA-256 core.
 *
 * Returns a newly allocated digest containing `output_words` big-endian
 * 32-bit words, or NULL on invalid arguments or allocation failure. The
 * caller owns the returned buffer. This is not part of the public API.
 */
uint8_t *sha2_32_digest(const uint8_t *message, size_t size, const uint32_t iv[8],
                        unsigned output_words);

#endif
