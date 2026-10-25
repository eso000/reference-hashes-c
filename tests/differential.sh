#!/bin/sh

# Compare the command-line front end against OpenSSL over generated inputs.
#
# The lengths cover the padding edges for both the 64-byte and 128-byte block
# sizes, plus a few larger messages. The fills match tests/gen_vectors.sh, so
# the oracle sees text, all-zero, all-ones and high-bit input.
set -eu

cd "$(dirname "$0")/.." || exit 1

CLI=${CLI:-build/cli_demo}

if [ ! -x "$CLI" ]; then
  echo "$0: missing $CLI (run make examples first)" >&2
  exit 1
fi

if ! command -v openssl >/dev/null 2>&1; then
  echo "$0: openssl not found" >&2
  exit 1
fi

TMP=$(mktemp -d "${TMPDIR:-/tmp}/hash-diff.XXXXXX") || exit 1
trap 'rm -rf "$TMP"' EXIT HUP INT TERM

LENGTHS="0 1 2 3 7 8 15 16 55 56 57 63 64 65 111 112 113 119 120 121"
LENGTHS="$LENGTHS 127 128 129 191 192 193 255 256 257 1000 4096 65536"
PATTERNS="61 00 ff 80"
ALGS="md5 sha1 sha224 sha256 sha384 sha512"

total=0
failed=0

fill_message() { # length hex-fill-byte path
  if [ "$2" = "00" ]; then
    head -c "$1" /dev/zero > "$3"
  else
    oct=$(printf '%03o' "0x$2")
    head -c "$1" /dev/zero | tr '\000' "\\$oct" > "$3"
  fi
}

for fill in $PATTERNS; do
  for len in $LENGTHS; do
    fill_message "$len" "$fill" "$TMP/message"

    for alg in $ALGS; do
      total=$((total + 1))
      expected=$(openssl dgst -"$alg" -r "$TMP/message" | cut -d ' ' -f 1)
      actual=$("$CLI" -c "$alg" -f "$TMP/message")

      if [ "$expected" != "$actual" ]; then
        failed=$((failed + 1))
        printf 'MISMATCH %s len=%s fill=0x%s\n  expected: %s\n  actual:   %s\n' \
          "$alg" "$len" "$fill" "$expected" "$actual"
      fi
    done
  done
done

if [ "$failed" -ne 0 ]; then
  printf 'differential: %s of %s inputs failed\n' "$failed" "$total"
  exit 1
fi

printf 'differential: %s inputs match OpenSSL %s\n' "$total" \
  "$(openssl version | cut -d ' ' -f 2)"
