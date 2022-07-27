/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef __ASAN_H
#define __ASAN_H

#include <types.h>

#define ASAN_SHADOW_SCALE_SHIFT 3

#define ASAN_SHADOW_SCALE_SIZE	(1UL << ASAN_SHADOW_SCALE_SHIFT)
#define ASAN_SHADOW_MASK	(ASAN_SHADOW_SCALE_SIZE - 1)

#define ASAN_GLOBAL_REDZONE	0xFA
#define ASAN_STACK_LEFT		0xF1
#define ASAN_STACK_MID		0xF2
#define ASAN_STACK_RIGHT	0xF3
#define ASAN_STACK_PARTIAL	0xF4
#define ASAN_USE_AFTER_SCOPE	0xF8

#define _RET_IP_	((unsigned long)__builtin_return_address(0))
#define likely(x)	__builtin_expect(!!(x), 1)
#define unlikely(x)	__builtin_expect(!!(x), 0)

#define WARN_ON(condition) ({	\
	int __ret_warn_on = !!(condition);	\
	unlikely(__ret_warn_on);	\
})

#ifndef ASAN_ABI_VERSION
#define ASAN_ABI_VERSION 5
#endif

/* The layout of struct dictated by compiler */
struct asan_source_location {
	const char *filename;
	int line_no;
	int column_no;
};

/* The layout of struct dictated by compiler */
struct asan_global {
	const void *beg;		/* Address of the beginning of the global variable. */
	size_t size;			/* Size of the global variable. */
	size_t size_with_redzone;	/* Size of the variable + size of the red zone
					   32 bytes aligned. */
	const void *name;
	const void *module_name;	/* Name of the module where the global variable
					   is declared. */
	unsigned long has_dynamic_init;	/* This needed for C++. */
#if ASAN_ABI_VERSION >= 4
	struct asan_source_location *location;
#endif
#if ASAN_ABI_VERSION >= 5
	char *odr_indicator;
#endif
};

void asan_unpoison_shadow(const void *address, size_t size);
void asan_report(unsigned long addr, size_t size, bool is_write,
	unsigned long ip);
void asan_init(void);
void check_memory_region(unsigned long addr, size_t size, bool write,
				unsigned long ret_ip);

uintptr_t __asan_shadow_offset(uintptr_t addr);
void __asan_register_globals(struct asan_global *globals, size_t size);
void __asan_unregister_globals(struct asan_global *globals, size_t size);
void __asan_poison_stack_memory(const void *addr, size_t size);
void __asan_unpoison_stack_memory(const void *addr, size_t size);

void __asan_load1(unsigned long addr);
void __asan_store1(unsigned long addr);
void __asan_load2(unsigned long addr);
void __asan_store2(unsigned long addr);
void __asan_load4(unsigned long addr);
void __asan_store4(unsigned long addr);
void __asan_load8(unsigned long addr);
void __asan_store8(unsigned long addr);
void __asan_load16(unsigned long addr);
void __asan_store16(unsigned long addr);
void __asan_loadN(unsigned long addr, size_t size);
void __asan_storeN(unsigned long addr, size_t size);

void __asan_load1_noabort(unsigned long addr);
void __asan_store1_noabort(unsigned long addr);
void __asan_load2_noabort(unsigned long addr);
void __asan_store2_noabort(unsigned long addr);
void __asan_load4_noabort(unsigned long addr);
void __asan_store4_noabort(unsigned long addr);
void __asan_load8_noabort(unsigned long addr);
void __asan_store8_noabort(unsigned long addr);
void __asan_load16_noabort(unsigned long addr);
void __asan_store16_noabort(unsigned long addr);
void __asan_loadN_noabort(unsigned long addr, size_t size);
void __asan_storeN_noabort(unsigned long addr, size_t size);
void __asan_handle_no_return(void);

void __asan_set_shadow_00(const void *addr, size_t size);
void __asan_set_shadow_f1(const void *addr, size_t size);
void __asan_set_shadow_f2(const void *addr, size_t size);
void __asan_set_shadow_f3(const void *addr, size_t size);
void __asan_set_shadow_f5(const void *addr, size_t size);
void __asan_set_shadow_f8(const void *addr, size_t size);

void __asan_report_load1_noabort(unsigned long addr);
void __asan_report_store1_noabort(unsigned long addr);
void __asan_report_load2_noabort(unsigned long addr);
void __asan_report_store2_noabort(unsigned long addr);
void __asan_report_load4_noabort(unsigned long addr);
void __asan_report_store4_noabort(unsigned long addr);
void __asan_report_load8_noabort(unsigned long addr);
void __asan_report_store8_noabort(unsigned long addr);
void __asan_report_load16_noabort(unsigned long addr);
void __asan_report_store16_noabort(unsigned long addr);
void __asan_report_load_n_noabort(unsigned long addr, size_t size);
void __asan_report_store_n_noabort(unsigned long addr, size_t size);
#endif
