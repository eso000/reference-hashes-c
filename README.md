# reference-hashes-c

[![CI](https://github.com/eso000/reference-hashes-c/actions/workflows/ci.yml/badge.svg)](https://github.com/eso000/reference-hashes-c/actions/workflows/ci.yml)
![C11](https://img.shields.io/badge/C-C11-blue)
![License: MIT](https://img.shields.io/badge/license-MIT-green)
![Dependencies: none](https://img.shields.io/badge/dependencies-none-brightgreen)

From-scratch, dependency-free implementations of six classic hash functions in
C11 — MD5, SHA-1, SHA-224, SHA-256, SHA-384 and SHA-512. Every function is
checked against published known-answer vectors and independently generated
boundary vectors, and the whole set has been compared against OpenSSL.

This is part of the same reference-implementation series as
[`reference-ciphers-c`](https://github.com/eso000/reference-ciphers-c).

Written to be read, and written to be checked.

## Security notice

**This is an educational, from-scratch implementation. Do not use it to protect
real secrets.**

- **MD5 and SHA-1 are collision-broken.** Do not use them for signatures,
  certificates, integrity checks, or anything adversarial. They remain useful
  only for compatibility and non-security checksums.
- The SHA-2 family has no known practical collisions, but this implementation is
  **not constant-time**, makes **no side-channel claims**, and has **not been
  audited**.
- It is **not a password-hashing library**. Use Argon2id, scrypt, or PBKDF2 for
  passwords, and use a vetted, maintained library such as OpenSSL or libsodium
  for anything that matters.
- It provides **no HMAC, KDF, or authenticated encryption** layer.

## Algorithms

| Algorithm | Digest size | Standard / source | Status |
|-----------|-------------|-------------------|--------|
| MD5       | 16 bytes (128 b) | RFC 1321 | Broken — practical collisions |
| SHA-1     | 20 bytes (160 b) | RFC 3174 / FIPS 180-4 | Broken — practical collisions |
| SHA-224   | 28 bytes (224 b) | RFC 6234 / FIPS 180-4 | No known practical collisions |
| SHA-256   | 32 bytes (256 b) | RFC 6234 / FIPS 180-4 | No known practical collisions |
| SHA-384   | 48 bytes (384 b) | RFC 6234 / FIPS 180-4 | No known practical collisions |
| SHA-512   | 64 bytes (512 b) | RFC 6234 / FIPS 180-4 | No known practical collisions |

All six are one-shot functions: the whole message is hashed in a single call, and
the digest is returned in a freshly allocated buffer.

## Requirements

- A C11 compiler. The strict build has been tested with GCC; the library itself
  uses only standard C11.
- GNU `make`.
- No third-party libraries and no runtime dependencies.

The command-line example uses POSIX `getopt`, and `make test-oom` uses the GNU
linker's `--wrap` option. The library has neither requirement.

Regenerating or verifying the boundary vectors (`make vectors`,
`make vectors-check`) needs the GNU coreutils checksum tools (`md5sum`,
`sha1sum`, …). The OpenSSL differential (`make test-differential`) needs
`openssl`. The main test suite needs neither.

## Build and test

```bash
make            # build the examples and the test binary into build/
make check      # main suite: KATs, allocation failures, UBSan, CLI
make examples   # build just the example programs
make clean      # remove the build directories
```

`make check` is the entry point worth remembering. It runs:

| Target | What it runs |
|---|---|
| `make test` | 766 known-answer, boundary, million-`a` and API-contract checks |
| `make test-oom` | 13 forced allocation-failure checks |
| `make test-ubsan` | the 766-check suite under UndefinedBehaviorSanitizer |
| `make test-cli` | 31 golden-output and error-path checks for the CLI |

Two further targets compare against external tools:

| Target | What it does |
|---|---|
| `make test-differential` | 768 comparisons against OpenSSL |
| `make vectors-check` | regenerate the boundary vectors and diff the result |

The differential test is not part of `make check` because it needs OpenSSL. The
sanitizer and Valgrind targets are separate because they need host toolchain
support:

```bash
make test-san       # AddressSanitizer + UndefinedBehaviorSanitizer
make test-valgrind  # requires Valgrind
```

For a strict local build:

```bash
make CC=gcc CFLAGS="-std=c11 -O2 -Wall -Wextra -Wpedantic -Werror" check
```

The Makefile respects `CC` from the environment or command line and otherwise
defaults to `gcc`.

## Command-line example

The CLI reads from `-s TEXT`, from `-f FILE`, or from stdin when neither is
given. It prints the digest as lowercase hex.

```bash
$ make examples
$ ./build/cli_demo -c sha256 -s abc
ba7816bf8f01cfea414140de5dae2223b00361a396177a9cb410ff61f20015ad

$ printf 'abc' | ./build/cli_demo -c sha1
a9993e364706816aba3e25717850c26c9cd0d89d

$ ./build/cli_demo -c sha512 -f LICENSE
...

$ ./build/cli_demo -c sha384 -f -   # explicit stdin
...
```

`-f` and `-s` are mutually exclusive. `-h` prints usage. Exit status is `0` on
success, `1` for an I/O, allocation, or hashing failure, and `2` for a usage
error.

The CLI buffers the whole input because the library exposes a one-shot API, and
caps it at about 2 GiB (`INT_MAX - 256` bytes).

## API

The public header is [`include/hash.h`](include/hash.h). Each algorithm is one
translation unit, so a caller can compile only what it uses.

```c
#include "hash.h"

#include <stdio.h>
#include <stdlib.h>

int main(void) {
  const uint8_t message[] = {'a', 'b', 'c'};
  uint8_t *digest = sha256(message, sizeof message);

  if (digest == NULL)
    return 1; /* allocation failure or invalid argument */

  for (size_t i = 0; i < 32; ++i)
    printf("%02x", (unsigned)digest[i]);
  putchar('\n');

  free(digest);
  return 0;
}
```

```bash
cc -std=c11 -O2 -Iinclude app.c src/sha256.c -o app
./app
# ba7816bf8f01cfea414140de5dae2223b00361a396177a9cb410ff61f20015ad
```

### API contract

- **Ownership.** Each function returns a newly `malloc`'d digest buffer. The
  caller owns it and must release it with `free()`.
- **Input.** `message` is `const uint8_t *` and is never modified.
- **Length.** `size` is a `size_t`, so a negative length cannot be represented.
- **NULL input.** `message` may be `NULL` only when `size` is `0`. A `NULL`
  message with a non-zero size is rejected.
- **Limits.** MD5, SHA-1, SHA-224 and SHA-256 use a 64-bit bit-length field and
  reject inputs above `2^61 - 1` bytes. SHA-384 and SHA-512 encode a full 128-bit
  length field and are limited by `size_t` and available memory. Sizes that would
  overflow the padding arithmetic are rejected.
- **Failure.** `NULL` is returned on allocation failure or invalid arguments.
  The API does not distinguish the two cases.
- **Concurrency.** There is no global mutable state; the functions are reentrant
  and safe to call concurrently on different inputs.

The memory cost is the returned digest plus a small fixed work buffer for MD5,
SHA-1, SHA-224, SHA-256 and SHA-512; SHA-384 currently allocates a padded copy.
There is no streaming interface yet, so the whole message must fit in memory.

## Architecture

```text
include/hash.h   public API and contract
src/             one implementation per algorithm
tests/           KATs, boundary vectors, OOM injection, CLI and differential tests
examples/        minimal API demo and the command-line front end
.github/         CI workflow
build/           binaries and objects (make output; ignored)
```

Padding follows the standards: append `0x80`, zero-fill, then append the message
length in bits as a big-endian integer. MD5, SHA-1, SHA-224 and SHA-256 use
64-byte blocks and a 64-bit length field; SHA-384 and SHA-512 use 128-byte blocks
and a 128-bit length field.

Message words and digest words are serialized byte by byte, so the code does not
depend on host endianness. MD5 uses its little-endian word order; SHA-1 and the
SHA-2 family use big-endian order.

## Testing and verification

`make test` records **766 checks**:

| Group | Count | Source |
|---|---:|---|
| Published known-answer vectors | 22 | RFC 1321 appendix A.5; FIPS 180-4 / RFC 3174 / RFC 6234 |
| Boundary vectors | 720 | Generated with GNU coreutils; 30 lengths × 4 fills × 6 algorithms |
| Million-`a` vectors | 6 | The classic one-million-character test for every algorithm |
| API-contract checks | 18 | `NULL`/zero, `NULL` with non-zero size, and `SIZE_MAX` input |

The boundary lengths include the padding edges: 55/56/57, 63/64/65, 111/112/113,
119/120/121, 127/128/129, 191/192/193 and 255/256/257 bytes. Each length is
tested with fills `0x61`, `0x00`, `0xff` and `0x80`, so the vectors cover text,
all-zero, all-ones and high-bit input. The vectors themselves are generated by
`tests/gen_vectors.sh` from an independent implementation (the coreutils
checksum tools), not from this library.

`make test-oom` links the library against wrappers for `malloc`, `calloc`,
`realloc` and `free`, fails each allocation in turn, and requires every function
to return `NULL` without crashing or leaking.

`make test-cli` covers all six algorithms, empty input, stdin, `-f`, `-f -`,
binary input containing NUL bytes, a 128 KiB file that crosses the read-buffer
growth point, help output, and the usage and I/O error paths.

The KAT suite is also run under UBSan.

`make test-differential` compares the CLI against **OpenSSL** over **768
generated inputs**: six algorithms × four fill bytes × 32 lengths. The lengths
cover both padding boundaries and larger messages up to 64 KiB. This is the
checked-in, repeatable version of the differential check used during
development.

`make vectors-check` regenerates the boundary vectors into a temporary file and
diffs them against the checked-in header, so the 720 committed vectors cannot
silently drift from the generator.

CI runs the strict GCC and Clang builds, ASan/UBSan, Valgrind, the OpenSSL
differential, the vector reproducibility check, and a `gcovr` coverage report.

## Known limitations

- **One-shot only.** There is no `init`/`update`/`final` interface. The message
  must fit in memory, and the CLI caps input at about 2 GiB.
- **No HMAC, KDF, or authenticated encryption.** This is a hash library, not a
  protocol or password-storage library.
- **Not constant-time and not audited.** See the security notice above.
- **MD5 and SHA-1 are broken.** They are included for compatibility and study.
- **The SHA-2 cores are duplicated.** `sha224.c`/`sha256.c` and
  `sha384.c`/`sha512.c` currently carry separate copies of their compression
  functions. Sharing a core per family is a planned refactor.
- **No fuzzing target yet.** Line coverage is measured in CI with `gcovr`.
- **No install target or packaged library yet.** Consumers compile the source
  files or the objects under `build/` directly.
- **The CLI is a demonstration front end**, not a hardened utility.

## Provenance

The C implementations in this repository were written by the author. They were
developed from a mixture of the published algorithm specifications — RFC 1321
(MD5), RFC 3174 (SHA-1), and RFC 6234 / FIPS 180-4 (SHA-2) — and the algorithm
descriptions and pseudocode in the corresponding Wikipedia articles.

No implementation code was copied from another hash library. The Wikipedia
articles are licensed CC BY-SA 4.0 and are cited as references; the MIT license in
this repository applies to the original C code.

The checked-in boundary vectors were generated at development time with the GNU
coreutils checksum tools. Those tools are external programs and are not part of
the library.

## References

- R. Rivest, *The MD5 Message-Digest Algorithm*, RFC 1321 — <https://www.rfc-editor.org/rfc/rfc1321>
- D. Eastlake and P. Jones, *US Secure Hash Algorithm 1 (SHA1)*, RFC 3174 — <https://www.rfc-editor.org/rfc/rfc3174>
- D. Eastlake and T. Hansen, *US Secure Hash Algorithms (SHA and HMAC-SHA)*, RFC 6234 — <https://www.rfc-editor.org/rfc/rfc6234>
- NIST, *Secure Hash Standard (SHS)*, FIPS 180-4 — <https://csrc.nist.gov/pubs/fips/180-4/upd1/final>
- Wikipedia, *MD5* — <https://en.wikipedia.org/wiki/MD5>
- Wikipedia, *SHA-1* — <https://en.wikipedia.org/wiki/SHA-1>
- Wikipedia, *SHA-2* — <https://en.wikipedia.org/wiki/SHA-2>
- *SHAttered*: the first practical SHA-1 collision — <https://shattered.io/>

## License

MIT — see [LICENSE](LICENSE).
