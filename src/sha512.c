#include "hash.h"
#include "sha2_64.h"

uint8_t *sha512(const uint8_t *message, size_t size)
{
    static const uint64_t iv[8] = {
        0x6a09e667f3bcc908u, 0xbb67ae8584caa73bu,
        0x3c6ef372fe94f82bu, 0xa54ff53a5f1d36f1u,
        0x510e527fade682d1u, 0x9b05688c2b3e6c1fu,
        0x1f83d9abfb41bd6bu, 0x5be0cd19137e2179u
    };

    return sha2_64_digest(message, size, iv, 8);
}
