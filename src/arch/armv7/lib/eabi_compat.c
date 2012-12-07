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

#include <common.h>

/* FIXME(dhendrix): prototypes added for assembler */
int raise (int signum);
int raise (int signum)
{
#if !defined(CONFIG_SPL_BUILD) || defined(CONFIG_SPL_LIBCOMMON_SUPPORT)
	printf("raise: Signal # %d caught\n", signum);
#endif
	return 0;
}

/* Dummy function to avoid linker complaints */
void __aeabi_unwind_cpp_pr0(void);
void __aeabi_unwind_cpp_pr0(void)
{
};

void __aeabi_unwind_cpp_pr1(void);
void __aeabi_unwind_cpp_pr1(void)
{
};
