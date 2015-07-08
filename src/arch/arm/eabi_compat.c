/*
 * Utility functions needed for (some) EABI conformant tool chains.
 *
 * (C) Copyright 2009 Wolfgang Denk <wd@denx.de>
 *
 * This program is Free Software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of the
 * License, or (at your option) any later version.
 */

#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <console/console.h>

/* FIXME(dhendrix): prototypes added for assembler */
int raise (int signum) __attribute__((used));
int raise (int signum)
{
	printk(BIOS_CRIT, "raise: Signal # %d caught\n", signum);
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

void __aeabi_memcpy(void *dest, const void *src, size_t n);
void __aeabi_memcpy(void *dest, const void *src, size_t n)
{
	(void) memcpy(dest, src, n);
}

void __aeabi_memset(void *dest, size_t n, int c);
void __aeabi_memset(void *dest, size_t n, int c)
{
	(void) memset(dest, c, n);
}
