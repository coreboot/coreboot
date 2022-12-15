/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef __ASSERT_H__
#define __ASSERT_H__

#include <arch/hlt.h>
#include <console/console.h>
#include <stdint.h>

/* TODO: Fix vendorcode headers to not define macros coreboot uses or to be more
   properly isolated. */
#ifdef ASSERT
#undef ASSERT
#endif

/* Do not use filenames nor line numbers on timeless builds, to preserve reproducibility */
#if ENV_TIMELESS
#define __ASSERT_FILE__ "(filenames not available on timeless builds)"
#define __ASSERT_LINE__ 404
#else
#define __ASSERT_FILE__ __FILE__
#define __ASSERT_LINE__ __LINE__
#endif

#ifndef _PORTING_H_	/* TODO: Isolate AGESA properly. */
#define __build_time_assert(x) \
	(__builtin_constant_p(x) ? ((x) ? 1 : dead_code_t(int)) : 0)
#else
#define __build_time_assert(x) 0
#endif

/* CMocka function redefinition. */
void mock_assert(const int result, const char *const expression,
		const char *const file, const int line);

#if ENV_TEST
#define MOCK_ASSERT(result, expression) \
	mock_assert((result), (expression), __ASSERT_FILE__, __ASSERT_LINE__)
#else
#define MOCK_ASSERT(result, expression)
#endif

/*
 * assert() should be used to test stuff that the programmer *knows* to be true.
 * It should not be used to test something that may actually change at runtime
 * (e.g. anything involving hardware accesses). For example, testing whether
 * function parameters match the documented requirements is a good use of
 * assert() (where it is still the responsibility of the caller to ensure it
 * passes valid values, and the callee is just double-checking).
 *
 * Depending on CONFIG(FATAL_ASSERTS), assert() will either halt execution or
 * just print an error message and continue. For more guidelines on error
 * handling, see Documentation/contributing/coding_style.md.
 */
#define ASSERT(x) {							\
	if (!__build_time_assert(x) && !(x)) {				\
		printk(BIOS_EMERG,					\
			"ASSERTION ERROR: file '%s', line %d\n",	\
			__ASSERT_FILE__, __ASSERT_LINE__);		\
		MOCK_ASSERT(!!(x), #x);					\
		if (CONFIG(FATAL_ASSERTS))				\
			hlt();						\
	}								\
}
#define ASSERT_MSG(x, msg) {						\
	if (!__build_time_assert(x) && !(x)) {				\
		printk(BIOS_EMERG,					\
			"ASSERTION ERROR: file '%s', line %d\n",	\
			__ASSERT_FILE__, __ASSERT_LINE__);		\
		printk(BIOS_EMERG, "%s", msg);				\
		MOCK_ASSERT(!!(x), (msg));				\
		if (CONFIG(FATAL_ASSERTS))				\
			hlt();						\
	}								\
}
#define BUG() {								\
	printk(BIOS_EMERG,						\
		"ERROR: BUG ENCOUNTERED at file '%s', line %d\n",	\
		__ASSERT_FILE__, __ASSERT_LINE__);			\
	MOCK_ASSERT(0, "BUG ENCOUNTERED");				\
	if (CONFIG(FATAL_ASSERTS))					\
		hlt();							\
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
 * bootmode.c:42: undefined reference to `_dead_code_assertion_failed'
 */
extern void _dead_code_assertion_failed(void) __attribute__((noreturn));
#define dead_code() _dead_code_assertion_failed()

/* This can be used in the context of an expression of type 'type'. */
#define dead_code_t(type) ({ \
	dead_code(); \
	*(type *)(uintptr_t)0; \
})

#if ENV_X86_64
#define pointer_to_uint32_safe(x) ({ \
	if ((uintptr_t)(x) > 0xffffffffUL) \
		die("Cast from pointer to uint32_t overflows"); \
	(uint32_t)(uintptr_t)(x); \
})
#else
#define pointer_to_uint32_safe(x) ({ \
	(uint32_t)(uintptr_t)(x); \
})
#endif
#endif // __ASSERT_H__
