/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Utility functions needed for (some) EABI conformant tool chains.
 */

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
