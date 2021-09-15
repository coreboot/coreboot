/* SPDX-License-Identifier: GPL-2.0-only */

#include <libpayload-config.h>
#include <arch/types.h>
#include <stddef.h>
#include <unistd.h>
#include <stdio.h>

/* Use libc version. calling exit() or abort() would cause infinite recursion */
__attribute__((noreturn))
void _exit(int);

__attribute__((noreturn))
void halt(void)
{
	_exit(0);
}

#define TEST_SYMBOL(symbol, value) asm(".set " #symbol ", " #value "\n\t.globl " #symbol)

#define TEST_REGION(region, size) uint8_t _##region[size]; \
	TEST_SYMBOL(_e##region, _##region + size); \
	TEST_SYMBOL(_##region##_size, size)

TEST_REGION(heap, CONFIG_LP_HEAP_SIZE);

uint64_t timer_raw_value(void)
{
	return 0;
}

uint64_t timer_hz(void)
{
	return 0;
}

/* Not present in libpayload. Can be used to write to real stdout. */
ssize_t write(int fildes, const void *buf, size_t nbyte);

void console_write(const void *buffer, size_t count)
{
	write(1, buffer, count);
}
