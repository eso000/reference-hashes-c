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
DEMOS := $(addprefix $(BUILD_DIR)/,$(ALGS))
OBJS := $(addprefix $(BUILD_DIR)/,$(addsuffix .o,$(ALGS)))
TEST := $(BUILD_DIR)/test_hashes
TEST_OOM := $(BUILD_DIR)/test_oom

SAN_FLAGS := -g -O1 -fno-omit-frame-pointer -fsanitize=address,undefined -fno-sanitize-recover=all
UBSAN_FLAGS := -g -O1 -fsanitize=undefined -fsanitize-trap=undefined
WRAP_ALLOC := -Wl,--wrap=malloc -Wl,--wrap=calloc -Wl,--wrap=realloc -Wl,--wrap=free

override CFLAGS += -Wall -Wextra -Wpedantic -I$(INC_DIR)

.PHONY: all test vectors test-san test-ubsan test-valgrind test-oom clean

all: $(DEMOS) $(TEST)

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

# Each source file still carries its demo main(), so the test objects rename
# main to a per-algorithm name before linking them into one binary.
$(OBJS): $(BUILD_DIR)/%.o: $(SRC_DIR)/%.c $(INC_DIR)/hash.h | $(BUILD_DIR)
	$(CC) $(CFLAGS) -Dmain=$*_demo -c $< -o $@

$(DEMOS): $(BUILD_DIR)/%: $(SRC_DIR)/%.c $(INC_DIR)/hash.h | $(BUILD_DIR)
	$(CC) $(CFLAGS) $< -o $@

$(TEST): $(TEST_DIR)/test_hashes.c $(TEST_DIR)/testutil.h $(TEST_DIR)/boundary_vectors.h $(INC_DIR)/hash.h $(OBJS) | $(BUILD_DIR)
	$(CC) $(CFLAGS) $(TEST_DIR)/test_hashes.c $(OBJS) -o $@

$(TEST_OOM): $(TEST_DIR)/test_oom.c $(TEST_DIR)/fail_alloc.c $(TEST_DIR)/fail_alloc.h $(OBJS) $(INC_DIR)/hash.h | $(BUILD_DIR)
	$(CC) $(CFLAGS) $(TEST_DIR)/test_oom.c $(TEST_DIR)/fail_alloc.c $(OBJS) $(WRAP_ALLOC) -o $@

test-oom: $(TEST_OOM)
	./$(TEST_OOM)

clean:
	rm -rf $(BUILD_DIR) $(SAN_BUILD_DIR) $(UBSAN_BUILD_DIR)
