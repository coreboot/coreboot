/* SPDX-License-Identifier: GPL-2.0-only */

#include <arch/cpu.h>

/* GCC's libgcc handling is quite broken. While the libgcc functions
 * are always regparm(0) the code that calls them uses whatever the
 * compiler call specifies. Therefore we need a wrapper around those
 * functions. See gcc bug PR41055 for more information.
 */

/* TODO: maybe this code should move to arch/x86 as architecture
 * specific implementations may vary
 */
#define WRAP_LIBGCC_CALL(type, name) \
	asmlinkage type __real_##name(type a, type b); \
	type __wrap_##name(type a, type b); \
	type __wrap_##name(type a, type b) { return __real_##name(a, b); }

WRAP_LIBGCC_CALL(long long, __divdi3)
WRAP_LIBGCC_CALL(unsigned long long, __udivdi3)
WRAP_LIBGCC_CALL(long long, __moddi3)
WRAP_LIBGCC_CALL(unsigned long long, __umoddi3)
