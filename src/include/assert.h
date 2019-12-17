/*
 * This file is part of the coreboot project.
 *
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#ifndef __ASSERT_H__
#define __ASSERT_H__

#include <arch/hlt.h>
#include <console/console.h>

/* TODO: Fix vendorcode headers to not define macros coreboot uses or to be more
   properly isolated. */
#ifdef ASSERT
#undef ASSERT
#endif

/* GCC and CAR versions */
#define ASSERT(x) {						\
	if (!(x)) {						\
		printk(BIOS_EMERG, "ASSERTION ERROR: file '%s'"	\
			", line %d\n", __FILE__, __LINE__);	\
		if (CONFIG(FATAL_ASSERTS))		\
			hlt();					\
	}							\
}

#define ASSERT_MSG(x, msg) {					\
	if (!(x)) {						\
		printk(BIOS_EMERG, "ASSERTION ERROR: file '%s'"	\
			", line %d\n", __FILE__, __LINE__);	\
		printk(BIOS_EMERG, "%s", msg);                  \
		if (CONFIG(FATAL_ASSERTS))			\
			hlt();					\
	}							\
}

#define BUG() {							\
	printk(BIOS_EMERG, "ERROR: BUG ENCOUNTERED at file '%s'"\
		", line %d\n", __FILE__, __LINE__);		\
	if (CONFIG(FATAL_ASSERTS))			\
		hlt();						\
}

#define assert(statement)	ASSERT(statement)

/*
 * These macros can be used to assert that a certain branch of code is dead and
 * will be compile-time eliminated. This differs from _Static_assert(), which
 * will generate a compiler error even if the scope it was called from is dead
 * code. This may be useful to double-check things like constants that are only
 * valid if a certain Kconfig option is set.
 *
 * The error message when this hits will look like this:
 *
 * ramstage/lib/bootmode.o: In function `display_init_required':
 * bootmode.c:42: undefined reference to `dead_code_assertion_failed_at_line_42'
 */
#define __dead_code(line) do { \
	extern void dead_code_assertion_failed_at_line_##line(void) \
		__attribute__((noreturn)); \
	dead_code_assertion_failed_at_line_##line(); \
} while (0)
#define _dead_code(line) __dead_code(line)
#define dead_code() _dead_code(__LINE__)

/* This can be used in the context of an expression of type 'type'. */
#define dead_code_t(type) ({ \
	dead_code(); \
	*(type *)(uintptr_t)0; \
})

#endif // __ASSERT_H__
