/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2010 coresystems GmbH
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

/* GCC and CAR versions */
#define ASSERT(x) {						\
	if (!(x)) {						\
		printk(BIOS_EMERG, "ASSERTION ERROR: file '%s'"	\
			", line %d\n", __FILE__, __LINE__);	\
		if (IS_ENABLED(CONFIG_FATAL_ASSERTS))		\
			hlt();					\
	}							\
}
#define BUG() {							\
	printk(BIOS_EMERG, "ERROR: BUG ENCOUNTERED at file '%s'"\
		", line %d\n", __FILE__, __LINE__);		\
	if (IS_ENABLED(CONFIG_FATAL_ASSERTS))			\
		hlt();						\
}

#define assert(statement)	ASSERT(statement)

/*
 * These macros can be used to assert that a certain branch of code is dead and
 * will be compile-time eliminated. This differs from _Static_assert(), which
 * will generate a compiler error even if the scope it was called from is dead
 * code. This may be useful to double-check things like constants that are only
 * valid if a certain Kconfig option is set.
 */
#define __dead_code(message, line) do { \
	__attribute__((error(#message " in " __FILE__ ":" #line))) \
	extern void dead_code_assertion_failed_##line(void); \
	dead_code_assertion_failed_##line(); \
} while (0)
#define _dead_code(message, line) __dead_code(message, line)
#define dead_code(message) _dead_code(message, __LINE__)

/* This can be used in the context of an expression of type 'type'. */
#define dead_code_t(type, message) ({ \
	dead_code(message); \
	*(type *)(uintptr_t)0; \
})

#endif // __ASSERT_H__
