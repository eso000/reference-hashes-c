#include "hash.h"
#include "sha2_64.h"

uint8_t *sha384(const uint8_t *message, size_t size)
{
    static const uint64_t iv[8] = {
        0xcbbb9d5dc1059ed8u, 0x629a292a367cd507u,
        0x9159015a3070dd17u, 0x152fecd8f70e5939u,
        0x67332667ffc00b31u, 0x8eb44a8768581511u,
        0xdb0c2e0d64f98fa7u, 0x47b5481dbefa4fa4u
    };

    return sha2_64_digest(message, size, iv, 6);
}
