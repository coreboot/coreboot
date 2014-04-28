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

#include <console/console.h>

int raise (int signum) __attribute__((used));
int raise (int signum)
{
	return 0;
}

void __aeabi_unwind_cpp_pr0(void) __attribute__((used));
void __aeabi_unwind_cpp_pr0(void)
{
};

void __aeabi_unwind_cpp_pr1(void) __attribute__((used));
void __aeabi_unwind_cpp_pr1(void)
{
};
