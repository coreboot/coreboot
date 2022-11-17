/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <stddef.h>
#include <string.h>
#include <console/console.h>

/* FIXME(dhendrix): prototypes added for assembler */
int raise(int signum) __attribute__((used));
int raise(int signum)
{
	printk(BIOS_CRIT, "%s: Signal # %d caught\n", __func__, signum);
	return 0;
}

/* Dummy function to avoid linker complaints */
void __aeabi_unwind_cpp_pr0(void) __attribute__((used));
void __aeabi_unwind_cpp_pr0(void)
{
}

void __aeabi_unwind_cpp_pr1(void) __attribute__((used));
void __aeabi_unwind_cpp_pr1(void)
{
}

/* Support the alias for the __aeabi_memcpy which may
   assume memory alignment.  */
void __aeabi_memcpy4(void *dest, const void *src, size_t n)
	__attribute((alias("__aeabi_memcpy")));

void __aeabi_memcpy8(void *dest, const void *src, size_t n)
	__attribute((alias("__aeabi_memcpy")));

void __aeabi_memcpy(void *dest, const void *src, size_t n);
void __aeabi_memcpy(void *dest, const void *src, size_t n)
{
	(void)memcpy(dest, src, n);
}

void __aeabi_memset(void *dest, size_t n, int c);
void __aeabi_memset(void *dest, size_t n, int c)
{
	(void)memset(dest, c, n);
}

/* Support the alias for the __aeabi_memclr which may
   assume memory alignment.  */
void __aeabi_memclr4(void *dest, size_t n)
	__attribute((alias("__aeabi_memclr")));

void __aeabi_memclr8(void *dest, size_t n)
	__attribute((alias("__aeabi_memclr")));

/* Support the routine __aeabi_memclr.  */
void __aeabi_memclr(void *dest, size_t n);
void __aeabi_memclr(void *dest, size_t n)
{
	__aeabi_memset(dest, n, 0);
}
