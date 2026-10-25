#define _POSIX_C_SOURCE 200809L

#include "hash.h"

#include <limits.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

/*
 * Command-line front end for the hash library.
 *
 * Input comes from -s TEXT, from -f FILE, or from stdin when neither is
 * given. The whole input is buffered because the library exposes a one-shot
 * API. The command prints the digest as lowercase hex.
 */

/*
 * The one-shot functions take an int length and add up to 144 padding bytes
 * in signed int arithmetic, so keep a margin below INT_MAX.
 */
#define MAX_MESSAGE ((size_t)(INT_MAX - 256))

typedef uint8_t* (*hash_fn)(uint8_t* message, int size);

struct algorithm {
  const char* name;
  hash_fn fn;
  size_t digest_len;
};

static const struct algorithm algorithms[] = {
    {"md5", md5, 16},       {"sha1", sha1, 20},
    {"sha224", sha224, 28}, {"sha256", sha256, 32},
    {"sha384", sha384, 48}, {"sha512", sha512, 64},
};

struct options {
  const char* algorithm;
  const char* file;
  const char* text;
  int help;
};

enum read_result { READ_OK, READ_OOM, READ_IO, READ_TOO_BIG };

static void print_algorithm_names(FILE* out) {
  for (size_t i = 0; i < sizeof algorithms / sizeof algorithms[0]; ++i)
    fprintf(out, "%s%s", i == 0 ? "" : ", ", algorithms[i].name);
}

static void usage(FILE* out, const char* program) {
  fprintf(out,
          "Usage: %s -c ALG [-f FILE | -s TEXT] [-h]\n"
          "\n"
          "  -c ALG    hash algorithm: ",
          program);
  print_algorithm_names(out);
  fprintf(out,
          "\n"
          "  -f FILE   hash FILE ('-' means stdin; stdin is the default)\n"
          "  -s TEXT   hash the literal TEXT instead of reading input\n"
          "  -h        show this help and exit\n");
}

/* Returns 0 on success, 2 on a usage error. */
static int parse_args(int argc, char** argv, struct options* opts) {
  int opt;

  opts->algorithm = NULL;
  opts->file = NULL;
  opts->text = NULL;
  opts->help = 0;

  while ((opt = getopt(argc, argv, "c:f:s:h")) != -1) {
    switch (opt) {
      case 'c':
        opts->algorithm = optarg;
        break;
      case 'f':
        opts->file = optarg;
        break;
      case 's':
        opts->text = optarg;
        break;
      case 'h':
        opts->help = 1;
        break;
      default:
        usage(stderr, argv[0]);
        return 2;
    }
  }

  if (opts->help) {
    usage(stdout, argv[0]);
    return 0;
  }

  if (optind < argc) {
    fprintf(stderr, "%s: unexpected argument '%s'\n", argv[0], argv[optind]);
    usage(stderr, argv[0]);
    return 2;
  }

  if (opts->algorithm == NULL) {
    fprintf(stderr, "%s: -c ALG is required\n", argv[0]);
    usage(stderr, argv[0]);
    return 2;
  }

  if (opts->file != NULL && opts->text != NULL) {
    fprintf(stderr, "%s: -f and -s are mutually exclusive\n", argv[0]);
    usage(stderr, argv[0]);
    return 2;
  }

  return 0;
}

static const struct algorithm* find_algorithm(const char* name) {
  for (size_t i = 0; i < sizeof algorithms / sizeof algorithms[0]; ++i) {
    if (strcmp(algorithms[i].name, name) == 0)
      return &algorithms[i];
  }
  return NULL;
}

/*
 * Read the whole stream into a growable buffer. Works for regular files and
 * pipes, and returns a non-NULL buffer for empty input because the hash
 * functions call memcpy() even when the length is zero.
 */
static enum read_result read_stream(FILE* in, uint8_t** out, size_t* out_len) {
  size_t cap = 64 * 1024;
  size_t len = 0;
  uint8_t* buf = (uint8_t*)malloc(cap);

  if (buf == NULL)
    return READ_OOM;

  for (;;) {
    if (len == cap) {
      size_t new_cap = cap > MAX_MESSAGE / 2 ? MAX_MESSAGE + 1 : cap * 2;
      uint8_t* grown = (uint8_t*)realloc(buf, new_cap);

      if (grown == NULL) {
        free(buf);
        return READ_OOM;
      }
      buf = grown;
      cap = new_cap;
    }

    size_t n = fread(buf + len, 1, cap - len, in);
    len += n;

    if (len > MAX_MESSAGE) {
      free(buf);
      return READ_TOO_BIG;
    }

    if (n == 0) {
      if (ferror(in)) {
        free(buf);
        return READ_IO;
      }
      break;
    }
  }

  *out = buf;
  *out_len = len;
  return READ_OK;
}

int main(int argc, char** argv) {
  struct options opts;
  int status = parse_args(argc, argv, &opts);

  if (status != 0)
    return status;

  if (opts.help)
    return 0;

  const struct algorithm* alg = find_algorithm(opts.algorithm);

  if (alg == NULL) {
    fprintf(stderr, "%s: unknown algorithm '%s'\n", argv[0], opts.algorithm);
    fprintf(stderr, "valid algorithms: ");
    print_algorithm_names(stderr);
    fprintf(stderr, "\n");
    return 2;
  }

  uint8_t* message = NULL;
  size_t len = 0;
  int owns_message = 0;
  const char* label = "stdin";

  if (opts.text != NULL) {
    /* hash.h documents that the input buffer is not modified. */
    message = (uint8_t*)opts.text;
    len = strlen(opts.text);
    label = "-s";
  } else {
    FILE* in = stdin;

    if (opts.file != NULL && strcmp(opts.file, "-") != 0) {
      label = opts.file;
      in = fopen(opts.file, "rb");
      if (in == NULL) {
        perror(opts.file);
        return 1;
      }
    }

    enum read_result result = read_stream(in, &message, &len);

    if (in != stdin)
      fclose(in);

    if (result != READ_OK) {
      if (result == READ_OOM)
        fprintf(stderr, "%s: %s: out of memory\n", argv[0], label);
      else if (result == READ_TOO_BIG)
        fprintf(stderr, "%s: %s: input too large\n", argv[0], label);
      else
        perror(label);
      return 1;
    }
    owns_message = 1;
  }

  if (len > MAX_MESSAGE) {
    fprintf(stderr, "%s: %s: input too large\n", argv[0], label);
    if (owns_message)
      free(message);
    return 1;
  }

  uint8_t* digest = alg->fn(message, (int)len);

  if (digest == NULL) {
    fprintf(stderr, "%s: %s: allocation failed\n", argv[0], alg->name);
    if (owns_message)
      free(message);
    return 1;
  }

  for (size_t i = 0; i < alg->digest_len; ++i)
    printf("%.2x", (unsigned)digest[i]);
  putchar('\n');

  free(digest);
  if (owns_message)
    free(message);

  if (fflush(stdout) != 0) {
    perror("stdout");
    return 1;
  }

  return 0;
}
