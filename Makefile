# ?= would not work here. Make gives CC a built-in default of "cc", so CC is
# always defined and a conditional assignment never fires. Default to gcc only
# while CC is still that built-in default or unset, which leaves the
# environment and the command line free to pick something else.
CC := $(if $(filter default undefined,$(origin CC)),gcc,$(CC))
CFLAGS ?= -std=c11 -O2
CFLAGS += -Wall -Wextra -Wpedantic

BUILD_DIR := build
ALGS := md5 sha1 sha224 sha256 sha384 sha512
DEMOS := $(addprefix $(BUILD_DIR)/,$(ALGS))
OBJS := $(addprefix $(BUILD_DIR)/,$(addsuffix .o,$(ALGS)))
TEST := $(BUILD_DIR)/test_hashes

.PHONY: all test vectors clean

all: $(DEMOS) $(TEST)

test: $(TEST)
	./$(TEST)

vectors:
	./tests/gen_vectors.sh

$(BUILD_DIR):
	mkdir -p $@

# Each source file still carries its demo main(), so the test objects rename
# main to a per-algorithm name before linking them into one binary.
$(OBJS): $(BUILD_DIR)/%.o: %.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -Dmain=$*_demo -c $< -o $@

$(DEMOS): $(BUILD_DIR)/%: %.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) $< -o $@

$(TEST): tests/test_hashes.c tests/testutil.h tests/boundary_vectors.h $(OBJS) | $(BUILD_DIR)
	$(CC) $(CFLAGS) tests/test_hashes.c $(OBJS) -o $@

clean:
	rm -rf $(BUILD_DIR)
