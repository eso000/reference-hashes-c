#!/bin/sh

# Golden-output and error-path tests for the command-line front end.
#
# The expected digests are constants, so the test does not need an external
# hashing tool at run time. The script only reads the built CLI plus /dev/zero.

set -u

cd "$(dirname "$0")/.." || exit 1

CLI=${CLI:-build/cli_demo}

if [ ! -x "$CLI" ]; then
  echo "$0: missing $CLI (run make first)" >&2
  exit 1
fi

TMP=$(mktemp -d "${TMPDIR:-/tmp}/hash-cli.XXXXXX") || exit 1
trap 'rm -rf "$TMP"' EXIT HUP INT TERM

total=0
failed=0
OUT=
STATUS=0
IN_FILE=

check_equal() { # label expected actual
  total=$((total + 1))
  if [ "$2" = "$3" ]; then
    return 0
  fi
  failed=$((failed + 1))
  printf 'FAIL %s\n  expected: %s\n  actual:   %s\n' "$1" "$2" "$3"
  return 1
}

check_status() { # label expected actual
  total=$((total + 1))
  if [ "$2" -eq "$3" ]; then
    return 0
  fi
  failed=$((failed + 1))
  printf 'FAIL %s: expected exit %s, got %s\n' "$1" "$2" "$3"
  return 1
}

check_stderr_contains() { # label needle
  total=$((total + 1))
  case $(cat "$TMP/stderr") in
    *"$2"*) return 0 ;;
  esac
  failed=$((failed + 1))
  printf 'FAIL %s: stderr does not contain: %s\n' "$1" "$2"
  sed 's/^/  /' "$TMP/stderr"
  return 1
}

run() {
  OUT=$("$@" 2>"$TMP/stderr")
  STATUS=$?
}

run_stdin() {
  IN_FILE=$1
  shift
  OUT=$("$@" < "$IN_FILE" 2>"$TMP/stderr")
  STATUS=$?
}

check_digest() { # label expected command...
  label=$1
  expected=$2
  shift 2

  run "$@"

  if [ "$STATUS" -ne 0 ]; then
    check_status "$label: exit status" 0 "$STATUS"
    sed 's/^/  /' "$TMP/stderr"
    return
  fi

  check_equal "$label" "$expected" "$OUT"
}

check_stdin_digest() { # label expected input_file command...
  label=$1
  expected=$2
  file=$3
  shift 3

  run_stdin "$file" "$@"

  if [ "$STATUS" -ne 0 ]; then
    check_status "$label: exit status" 0 "$STATUS"
    sed 's/^/  /' "$TMP/stderr"
    return
  fi

  check_equal "$label" "$expected" "$OUT"
}

check_error() { # label expected_status stderr_needle command...
  label=$1
  expected_status=$2
  needle=$3
  shift 3

  run "$@"
  check_status "$label: exit status" "$expected_status" "$STATUS"
  check_stderr_contains "$label: stderr" "$needle"
}

printf 'abc' > "$TMP/abc"
printf '\000\001\377\177\200' > "$TMP/binary"
: > "$TMP/empty"
dd if=/dev/zero of="$TMP/zeros" bs=65536 count=2 2>/dev/null

# Known-answer digests for the literal "abc".
check_digest "md5/abc" \
  900150983cd24fb0d6963f7d28e17f72 "$CLI" -c md5 -s abc
check_digest "sha1/abc" \
  a9993e364706816aba3e25717850c26c9cd0d89d "$CLI" -c sha1 -s abc
check_digest "sha224/abc" \
  23097d223405d8228642a477bda255b32aadbce4bda0b3f7e36c9da7 \
  "$CLI" -c sha224 -s abc
check_digest "sha256/abc" \
  ba7816bf8f01cfea414140de5dae2223b00361a396177a9cb410ff61f20015ad \
  "$CLI" -c sha256 -s abc
check_digest "sha384/abc" \
  cb00753f45a35e8bb5a03d699ac65007272c32ab0eded1631a8b605a43ff5bed8086072ba1e7cc2358baeca134c825a7 \
  "$CLI" -c sha384 -s abc
check_digest "sha512/abc" \
  ddaf35a193617abacc417349ae20413112e6fa4e89a97ea20a9eeee64b55d39a2192992a274fc1a836ba3c23a3feebbd454d4423643ce80e2a9ac94fa54ca49f \
  "$CLI" -c sha512 -s abc

# Empty input is valid, whether it arrives as a literal or a stream.
check_digest "md5/empty-string" \
  d41d8cd98f00b204e9800998ecf8427e "$CLI" -c md5 -s ''
check_digest "sha256/empty-file" \
  e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855 \
  "$CLI" -c sha256 -f "$TMP/empty"
check_stdin_digest "sha512/empty-stdin" \
  cf83e1357eefb8bdf1542850d66d8007d620e4050b5715dc83f4a921d36ce9ce47d0d13c5d85f2b0ff8318d2877eec2f63b931bd47417a81a538327af927da3e \
  /dev/null "$CLI" -c sha512

# Arguments with spaces must not be split, and stdin is the default source.
check_digest "sha256/spaces" \
  c8687a08aa5d6ed2044328fa6a697ab8e96dc34291e8c2034ae8c38e6fcc6d65 \
  "$CLI" -c sha256 -s 'a b'
check_stdin_digest "sha1/stdin" \
  a9993e364706816aba3e25717850c26c9cd0d89d "$TMP/abc" "$CLI" -c sha1

# -f handles regular files and the explicit stdin spelling.
check_digest "sha256/file" \
  ba7816bf8f01cfea414140de5dae2223b00361a396177a9cb410ff61f20015ad \
  "$CLI" -c sha256 -f "$TMP/abc"
check_stdin_digest "sha384/file-dash" \
  cb00753f45a35e8bb5a03d699ac65007272c32ab0eded1631a8b605a43ff5bed8086072ba1e7cc2358baeca134c825a7 \
  "$TMP/abc" "$CLI" -c sha384 -f -

# Binary input, including NUL and high-bit bytes, goes through unchanged.
check_digest "sha256/binary-file" \
  bc558ad83ac68b37609890638a13b7e3f9146e5e70603e02656f1134e0f3e6d5 \
  "$CLI" -c sha256 -f "$TMP/binary"

# Cross the 64 KiB read buffer so the growth path is exercised.
check_digest "sha256/growth" \
  fa43239bcee7b97ca62f007cc68487560a39e19f74f3dde7486db3f98df8e471 \
  "$CLI" -c sha256 -f "$TMP/zeros"

expected_help="Usage: $CLI -c ALG [-f FILE | -s TEXT] [-h]

  -c ALG    hash algorithm: md5, sha1, sha224, sha256, sha384, sha512
  -f FILE   hash FILE ('-' means stdin; stdin is the default)
  -s TEXT   hash the literal TEXT instead of reading input
  -h        show this help and exit"

run "$CLI" -h
check_status "help: exit status" 0 "$STATUS"
check_equal "help: output" "$expected_help" "$OUT"

run "$CLI" -c md5 -h
check_status "help with -c: exit status" 0 "$STATUS"
check_equal "help with -c: output" "$expected_help" "$OUT"

# Usage and I/O errors must fail with the documented status and message.
check_error "unknown algorithm" 2 "unknown algorithm 'nope'" \
  "$CLI" -c nope -s abc
check_error "missing algorithm" 2 "-c ALG is required" \
  "$CLI" -s abc
check_error "conflicting options" 2 "-f and -s are mutually exclusive" \
  "$CLI" -c md5 -f "$TMP/abc" -s abc
check_error "unexpected argument" 2 "unexpected argument 'extra'" \
  "$CLI" -c md5 extra
check_error "missing file" 1 "$TMP/does-not-exist" \
  "$CLI" -c md5 -f "$TMP/does-not-exist"
check_error "unknown option" 2 "Usage:" \
  "$CLI" -z

if [ "$failed" -ne 0 ]; then
  printf 'CLI tests: %s of %s checks failed\n' "$failed" "$total"
  exit 1
fi

printf 'CLI tests: %s checks passed\n' "$total"
