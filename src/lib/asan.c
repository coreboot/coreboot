#include <stddef.h>

#pragma GCC diagnostic ignored "-Wmissing-prototypes"

#define DEFINE_ASAN_LOAD_STORE(size)                        \
	void __asan_load##size(unsigned long addr)              \
	{}                                                      \
	void __asan_load##size##_noabort(unsigned long addr)    \
	{}                                                      \
	void __asan_store##size(unsigned long addr)             \
	{}                                                      \
	void __asan_store##size##_noabort(unsigned long addr)   \
	{}

DEFINE_ASAN_LOAD_STORE(1);
DEFINE_ASAN_LOAD_STORE(2);
DEFINE_ASAN_LOAD_STORE(4);
DEFINE_ASAN_LOAD_STORE(8);
DEFINE_ASAN_LOAD_STORE(16);

void __asan_loadN(unsigned long addr, size_t size)
{}

void __asan_loadN_noabort(unsigned long addr, size_t size)
{}

void __asan_storeN(unsigned long addr, size_t size)
{}

void __asan_storeN_noabort(unsigned long addr, size_t size)
{}

void __asan_handle_no_return(void)
{}
