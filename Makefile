# ?= would not work here. Make gives CC a built-in default of "cc", so CC is
# always defined and a conditional assignment never fires. Default to gcc only
# while CC is still that built-in default or unset, which leaves the
# environment and the command line free to pick something else.
CC := $(if $(filter default undefined,$(origin CC)),gcc,$(CC))
CFLAGS ?= -std=c11 -O2

BUILD_DIR := build
SAN_BUILD_DIR := build-san
UBSAN_BUILD_DIR := build-ubsan
SRC_DIR := src
INC_DIR := include
TEST_DIR := tests
ALGS := md5 sha1 sha224 sha256 sha384 sha512
DEMO := $(BUILD_DIR)/sha1_demo
CLI := $(BUILD_DIR)/cli_demo
EXAMPLES := $(DEMO) $(CLI)
OBJS := $(addprefix $(BUILD_DIR)/,$(addsuffix .o,$(ALGS)))
TEST := $(BUILD_DIR)/test_hashes
TEST_OOM := $(BUILD_DIR)/test_oom
TEST_CLI := $(TEST_DIR)/cli_tests.sh

SAN_FLAGS := -g -O1 -fno-omit-frame-pointer -fsanitize=address,undefined -fno-sanitize-recover=all
UBSAN_FLAGS := -g -O1 -fsanitize=undefined -fsanitize-trap=undefined
WRAP_ALLOC := -Wl,--wrap=malloc -Wl,--wrap=calloc -Wl,--wrap=realloc -Wl,--wrap=free

override CFLAGS += -Wall -Wextra -Wpedantic -I$(INC_DIR)

.PHONY: all examples check test vectors test-san test-ubsan test-valgrind test-oom test-cli clean

all: $(EXAMPLES) $(TEST)

examples: $(EXAMPLES)

# The main suite: known-answer tests, allocation failures, undefined-behaviour
# checks and the CLI contract. test-san and test-valgrind stay separate because
# they need host toolchain support (an ASan runtime and Valgrind respectively).
check: test test-oom test-ubsan test-cli

test: $(TEST)
	./$(TEST)

test-san:
	$(MAKE) BUILD_DIR=$(SAN_BUILD_DIR) CFLAGS="$(CFLAGS) $(SAN_FLAGS)" $(SAN_BUILD_DIR)/test_hashes
	ASAN_OPTIONS=detect_leaks=1:abort_on_error=1 \
	UBSAN_OPTIONS=halt_on_error=1:print_stacktrace=1 \
	./$(SAN_BUILD_DIR)/test_hashes

test-ubsan:
	$(MAKE) BUILD_DIR=$(UBSAN_BUILD_DIR) CFLAGS="$(CFLAGS) $(UBSAN_FLAGS)" $(UBSAN_BUILD_DIR)/test_hashes
	./$(UBSAN_BUILD_DIR)/test_hashes

test-valgrind: $(TEST)
	valgrind --error-exitcode=1 --leak-check=full --show-leak-kinds=all \
	  --errors-for-leak-kinds=definite,indirect --track-origins=yes ./$(TEST)

vectors:
	./tests/gen_vectors.sh

$(BUILD_DIR):
	mkdir -p $@

# The implementations no longer carry demo main() functions, so the test
# objects can be linked into one binary without renaming anything.
$(OBJS): $(BUILD_DIR)/%.o: $(SRC_DIR)/%.c $(INC_DIR)/hash.h | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(DEMO): examples/sha1_demo.c $(BUILD_DIR)/sha1.o $(INC_DIR)/hash.h | $(BUILD_DIR)
	$(CC) $(CFLAGS) $< $(BUILD_DIR)/sha1.o -o $@

$(CLI): examples/cli_demo.c $(OBJS) $(INC_DIR)/hash.h | $(BUILD_DIR)
	$(CC) $(CFLAGS) $< $(OBJS) -o $@

$(TEST): $(TEST_DIR)/test_hashes.c $(TEST_DIR)/testutil.h $(TEST_DIR)/boundary_vectors.h $(INC_DIR)/hash.h $(OBJS) | $(BUILD_DIR)
	$(CC) $(CFLAGS) $(TEST_DIR)/test_hashes.c $(OBJS) -o $@

$(TEST_OOM): $(TEST_DIR)/test_oom.c $(TEST_DIR)/testutil.h $(TEST_DIR)/fail_alloc.c $(TEST_DIR)/fail_alloc.h $(OBJS) $(INC_DIR)/hash.h | $(BUILD_DIR)
	$(CC) $(CFLAGS) $(TEST_DIR)/test_oom.c $(TEST_DIR)/fail_alloc.c $(OBJS) $(WRAP_ALLOC) -o $@

test-oom: $(TEST_OOM)
	./$(TEST_OOM)

test-cli: $(CLI) $(TEST_CLI)
	CLI="$(CLI)" ./$(TEST_CLI)

clean:
	rm -rf $(BUILD_DIR) $(SAN_BUILD_DIR) $(UBSAN_BUILD_DIR)
