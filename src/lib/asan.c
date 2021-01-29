/* SPDX-License-Identifier: GPL-2.0-only */

/*
 * Address sanitizer support.
 *
 * Parts of this file are based on mm/kasan
 * from the Linux kernel 4.19.137.
 *
 */

#include <console/console.h>
#include <symbols.h>
#include <assert.h>
#include <arch/symbols.h>
#include <asan.h>

static inline void *asan_mem_to_shadow(const void *addr)
{
#if ENV_ROMSTAGE
	return (void *)((uintptr_t)&_asan_shadow + (((uintptr_t)addr -
		(uintptr_t)&_car_region_start) >> ASAN_SHADOW_SCALE_SHIFT));
#elif ENV_RAMSTAGE
	return (void *)((uintptr_t)&_asan_shadow + (((uintptr_t)addr -
		(uintptr_t)&_data) >> ASAN_SHADOW_SCALE_SHIFT));
#endif
}

static inline const void *asan_shadow_to_mem(const void *shadow_addr)
{
#if ENV_ROMSTAGE
	return (void *)((uintptr_t)&_car_region_start + (((uintptr_t)shadow_addr -
		(uintptr_t)&_asan_shadow) << ASAN_SHADOW_SCALE_SHIFT));
#elif ENV_RAMSTAGE
	return (void *)((uintptr_t)&_data + (((uintptr_t)shadow_addr -
		(uintptr_t)&_asan_shadow) << ASAN_SHADOW_SCALE_SHIFT));
#endif
}

static void asan_poison_shadow(const void *address, size_t size, u8 value)
{
	void *shadow_start, *shadow_end;

	shadow_start = asan_mem_to_shadow(address);
	shadow_end = asan_mem_to_shadow(address + size);

	__builtin_memset(shadow_start, value, shadow_end - shadow_start);
}

void asan_unpoison_shadow(const void *address, size_t size)
{
	asan_poison_shadow(address, size, 0);

	if (size & ASAN_SHADOW_MASK) {
		u8 *shadow = (u8 *)asan_mem_to_shadow(address + size);
		*shadow = size & ASAN_SHADOW_MASK;
	}
}

static __always_inline bool memory_is_poisoned_1(unsigned long addr)
{
	s8 shadow_value = *(s8 *)asan_mem_to_shadow((void *)addr);

	if (unlikely(shadow_value)) {
		s8 last_accessible_byte = addr & ASAN_SHADOW_MASK;
		return unlikely(last_accessible_byte >= shadow_value);
	}

	return false;
}

static __always_inline bool memory_is_poisoned_2_4_8(unsigned long addr,
						unsigned long size)
{
	u8 *shadow_addr = (u8 *)asan_mem_to_shadow((void *)addr);

	if (unlikely(((addr + size - 1) & ASAN_SHADOW_MASK) < size - 1))
		return *shadow_addr || memory_is_poisoned_1(addr + size - 1);

	return memory_is_poisoned_1(addr + size - 1);
}

static __always_inline bool memory_is_poisoned_16(unsigned long addr)
{
	u16 *shadow_addr = (u16 *)asan_mem_to_shadow((void *)addr);

	if (unlikely(!IS_ALIGNED(addr, ASAN_SHADOW_SCALE_SIZE)))
		return *shadow_addr || memory_is_poisoned_1(addr + 15);

	return *shadow_addr;
}

static __always_inline unsigned long bytes_is_nonzero(const u8 *start,
					size_t size)
{
	while (size) {
		if (unlikely(*start))
			return (unsigned long)start;
		start++;
		size--;
	}

	return 0;
}

static __always_inline unsigned long memory_is_nonzero(const void *start,
						const void *end)
{
	unsigned int words;
	unsigned long ret;
	unsigned int prefix = (unsigned long)start % 8;

	if (end - start <= 16)
		return bytes_is_nonzero(start, end - start);

	if (prefix) {
		prefix = 8 - prefix;
		ret = bytes_is_nonzero(start, prefix);
		if (unlikely(ret))
			return ret;
		start += prefix;
	}

	words = (end - start) / 8;
	while (words) {
		if (unlikely(*(u64 *)start))
			return bytes_is_nonzero(start, 8);
		start += 8;
		words--;
	}

	return bytes_is_nonzero(start, (end - start) % 8);
}

static __always_inline bool memory_is_poisoned_n(unsigned long addr,
						size_t size)
{
	unsigned long ret;

	ret = memory_is_nonzero(asan_mem_to_shadow((void *)addr),
			asan_mem_to_shadow((void *)addr + size - 1) + 1);

	if (unlikely(ret)) {
		unsigned long last_byte = addr + size - 1;
		s8 *last_shadow = (s8 *)asan_mem_to_shadow((void *)last_byte);

		if (unlikely(ret != (unsigned long)last_shadow ||
			((long)(last_byte & ASAN_SHADOW_MASK) >= *last_shadow)))
			return true;
	}
	return false;
}

static __always_inline bool memory_is_poisoned(unsigned long addr, size_t size)
{
	if (__builtin_constant_p(size)) {
		switch (size) {
		case 1:
			return memory_is_poisoned_1(addr);
		case 2:
		case 4:
		case 8:
			return memory_is_poisoned_2_4_8(addr, size);
		case 16:
			return memory_is_poisoned_16(addr);
		default:
			assert(0);
		}
	}

	return memory_is_poisoned_n(addr, size);
}

static const void *find_first_bad_addr(const void *addr, size_t size)
{
	u8 shadow_val = *(u8 *)asan_mem_to_shadow(addr);
	const void *first_bad_addr = addr;

	while (!shadow_val && first_bad_addr < addr + size) {
		first_bad_addr += ASAN_SHADOW_SCALE_SIZE;
		shadow_val = *(u8 *)asan_mem_to_shadow(first_bad_addr);
	}
	return first_bad_addr;
}

static const char *get_shadow_bug_type(const void *addr, size_t size)
{
	const char *bug_type = "unknown-crash";
	u8 *shadow_addr;
	const void *first_bad_addr;

	if (addr < asan_shadow_to_mem((void *) &_asan_shadow))
		return bug_type;

	first_bad_addr = find_first_bad_addr(addr, size);

	shadow_addr = (u8 *)asan_mem_to_shadow(first_bad_addr);

	if (*shadow_addr > 0 && *shadow_addr <= ASAN_SHADOW_SCALE_SIZE - 1)
		shadow_addr++;

	switch (*shadow_addr) {
	case 0 ... ASAN_SHADOW_SCALE_SIZE - 1:
		bug_type = "out-of-bounds";
		break;
	case ASAN_GLOBAL_REDZONE:
		bug_type = "global-out-of-bounds";
		break;
	case ASAN_STACK_LEFT:
	case ASAN_STACK_MID:
	case ASAN_STACK_RIGHT:
	case ASAN_STACK_PARTIAL:
		bug_type = "stack-out-of-bounds";
		break;
	case ASAN_USE_AFTER_SCOPE:
		bug_type = "use-after-scope";
		break;
	default:
		bug_type = "unknown-crash";
	}

	return bug_type;
}

void asan_report(unsigned long addr, size_t size, bool is_write,
	unsigned long ip)
{
	const char *bug_type = get_shadow_bug_type((void *) addr, size);
	printk(BIOS_ERR, "\n");
	printk(BIOS_ERR, "ASan: %s in %p\n", bug_type, (void *) ip);
	printk(BIOS_ERR, "%s of %zu byte%s at addr %p\n",
		is_write ? "Write" : "Read", size, (size > 1 ? "s" : ""),
		(void *) addr);
	printk(BIOS_ERR, "\n");
}

static __always_inline void check_memory_region_inline(unsigned long addr,
						size_t size, bool write,
						unsigned long ret_ip)
{
#if ENV_ROMSTAGE
	if (((uintptr_t)addr < (uintptr_t)&_car_region_start) ||
		((uintptr_t)addr > (uintptr_t)&_ebss))
		return;
#elif ENV_RAMSTAGE
	if (((uintptr_t)addr < (uintptr_t)&_data) ||
		((uintptr_t)addr > (uintptr_t)&_eheap))
		return;
#endif
	if (unlikely(size == 0))
		return;

	if (unlikely((void *)addr <
		asan_shadow_to_mem((void *) &_asan_shadow))) {
		asan_report(addr, size, write, ret_ip);
		return;
	}

	if (likely(!memory_is_poisoned(addr, size)))
		return;

	asan_report(addr, size, write, ret_ip);
}

void check_memory_region(unsigned long addr, size_t size, bool write,
				unsigned long ret_ip)
{
	check_memory_region_inline(addr, size, write, ret_ip);
}

uintptr_t __asan_shadow_offset(uintptr_t addr)
{
#if ENV_ROMSTAGE
	return (uintptr_t)&_asan_shadow - (((uintptr_t)&_car_region_start) >>
		ASAN_SHADOW_SCALE_SHIFT);
#elif ENV_RAMSTAGE
	return (uintptr_t)&_asan_shadow - (((uintptr_t)&_data) >>
		ASAN_SHADOW_SCALE_SHIFT);
#endif
}

static void register_global(struct asan_global *global)
{
	size_t aligned_size = ALIGN_UP(global->size, ASAN_SHADOW_SCALE_SIZE);

	asan_unpoison_shadow(global->beg, global->size);

	asan_poison_shadow(global->beg + aligned_size,
		global->size_with_redzone - aligned_size,
		ASAN_GLOBAL_REDZONE);
}

void __asan_register_globals(struct asan_global *globals, size_t size)
{
	int i;

	for (i = 0; i < size; i++)
		register_global(&globals[i]);
}

void __asan_unregister_globals(struct asan_global *globals, size_t size)
{
}

/*
 * GCC adds constructors invoking __asan_register_globals() and passes
 * information about global variable (address, size, size with redzone ...)
 * to it so we could poison variable's redzone.
 * This function calls those constructors.
 */
#if ENV_RAMSTAGE
static void asan_ctors(void)
{
	extern long __CTOR_LIST__;
	typedef void (*func_ptr)(void);
	func_ptr *ctor = (func_ptr *) &__CTOR_LIST__;
	if (ctor == NULL)
		return;

	for (; *ctor != (func_ptr) 0; ctor++)
		(*ctor)();
}
#endif

void asan_init(void)
{
#if ENV_ROMSTAGE
	size_t size = (size_t)&_ebss - (size_t)&_car_region_start;
	asan_unpoison_shadow((void *)&_car_region_start, size);
#elif ENV_RAMSTAGE
	size_t size = (size_t)&_eheap - (size_t)&_data;
	asan_unpoison_shadow((void *)&_data, size);
	asan_ctors();
#endif
}

void __asan_poison_stack_memory(const void *addr, size_t size)
{
	asan_poison_shadow(addr, ALIGN_UP(size, ASAN_SHADOW_SCALE_SIZE),
			    ASAN_USE_AFTER_SCOPE);
}

void __asan_unpoison_stack_memory(const void *addr, size_t size)
{
	asan_unpoison_shadow(addr, size);
}

#define DEFINE_ASAN_LOAD_STORE(size)	\
	void __asan_load##size(unsigned long addr)	\
	{	\
		check_memory_region_inline(addr, size, false, _RET_IP_);\
	}	\
	void __asan_load##size##_noabort(unsigned long addr)	\
	{	\
		check_memory_region_inline(addr, size, false, _RET_IP_);\
	}	\
	void __asan_store##size(unsigned long addr)	\
	{	\
		check_memory_region_inline(addr, size, true, _RET_IP_);	\
	}	\
	void __asan_store##size##_noabort(unsigned long addr)	\
	{	\
		check_memory_region_inline(addr, size, true, _RET_IP_);	\
	}

DEFINE_ASAN_LOAD_STORE(1);
DEFINE_ASAN_LOAD_STORE(2);
DEFINE_ASAN_LOAD_STORE(4);
DEFINE_ASAN_LOAD_STORE(8);
DEFINE_ASAN_LOAD_STORE(16);

void __asan_loadN(unsigned long addr, size_t size)
{
	check_memory_region(addr, size, false, _RET_IP_);
}

void __asan_storeN(unsigned long addr, size_t size)
{
	check_memory_region(addr, size, true, _RET_IP_);
}

void __asan_loadN_noabort(unsigned long addr, size_t size)
{
	check_memory_region(addr, size, false, _RET_IP_);
}

void __asan_storeN_noabort(unsigned long addr, size_t size)
{
	check_memory_region(addr, size, true, _RET_IP_);
}

void __asan_handle_no_return(void)
{
}

#define DEFINE_ASAN_SET_SHADOW(byte)	\
	void __asan_set_shadow_##byte(const void *addr, size_t size)	\
	{	\
		__builtin_memset((void *)addr, 0x##byte, size);	\
	}

DEFINE_ASAN_SET_SHADOW(00);
DEFINE_ASAN_SET_SHADOW(f1);
DEFINE_ASAN_SET_SHADOW(f2);
DEFINE_ASAN_SET_SHADOW(f3);
DEFINE_ASAN_SET_SHADOW(f5);
DEFINE_ASAN_SET_SHADOW(f8);

#define DEFINE_ASAN_REPORT_LOAD(size)	\
void __asan_report_load##size##_noabort(unsigned long addr)	\
{	\
	asan_report(addr, size, false, _RET_IP_);	\
}

#define DEFINE_ASAN_REPORT_STORE(size)	\
void __asan_report_store##size##_noabort(unsigned long addr)	\
{	\
	asan_report(addr, size, true, _RET_IP_);	\
}

DEFINE_ASAN_REPORT_LOAD(1);
DEFINE_ASAN_REPORT_LOAD(2);
DEFINE_ASAN_REPORT_LOAD(4);
DEFINE_ASAN_REPORT_LOAD(8);
DEFINE_ASAN_REPORT_LOAD(16);
DEFINE_ASAN_REPORT_STORE(1);
DEFINE_ASAN_REPORT_STORE(2);
DEFINE_ASAN_REPORT_STORE(4);
DEFINE_ASAN_REPORT_STORE(8);
DEFINE_ASAN_REPORT_STORE(16);

void __asan_report_load_n_noabort(unsigned long addr, size_t size)
{
	asan_report(addr, size, false, _RET_IP_);
}

void __asan_report_store_n_noabort(unsigned long addr, size_t size)
{
	asan_report(addr, size, true, _RET_IP_);
}
