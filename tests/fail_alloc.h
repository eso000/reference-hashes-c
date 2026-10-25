#ifndef HASH_FAIL_ALLOC_H
#define HASH_FAIL_ALLOC_H

#include <stddef.h>

/*
 * Allocation-failure harness used by test_oom. Programming is done through
 * the linker's --wrap option in the test-oom Makefile target.
 */

/* Fail the allocation with the given zero-based index while armed. Only one
 * allocation is failed per arm; allocations after it succeed. */
void fail_alloc_arm(long index);
void fail_alloc_disarm(void);

/* Number of allocation attempts since the last arm. */
long fail_alloc_calls(void);

/* Whether the armed failure actually fired. */
int fail_alloc_triggered(void);

/* Number of successful allocations that have not been freed yet. */
long fail_alloc_live(void);

#endif
