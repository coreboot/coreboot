/* SPDX-License-Identifier: BSD-3-Clause */

#ifndef __COMMONLIB_STDLIB_H__
#define __COMMONLIB_STDLIB_H__

#include <stddef.h>
#include <string.h>

#ifdef __COREBOOT__
#include <console/console.h>
#else
#include <stdio.h>
#define printk(level, ...) printf(__VA_ARGS__)
#endif

void __noreturn abort(void);
void free(void *ptr);
void *malloc(size_t size);
void *calloc(size_t nitems, size_t size);
void *memalign(size_t align, size_t size);
int dma_coherent(const void *ptr);

static inline void *xmalloc_work(size_t size, const char *file, const char *func, int line)
{
	void *ret = malloc(size);
	if (!ret && size) {
		printk(BIOS_ERR, "%s:%d %s(): Failed to malloc %zu bytes\n",
			file, line, func, size);
		abort();
	}
	return ret;
}
#define xmalloc(size) xmalloc_work((size), __FILE__, __func__, __LINE__)

static inline void *xzalloc_work(size_t size, const char *file, const char *func, int line)
{
	void *ret = xmalloc_work(size, file, func, line);
	memset(ret, 0, size);
	return ret;
}
#define xzalloc(size) xzalloc_work((size), __FILE__, __func__, __LINE__)

static inline void *xmemalign_work(size_t align, size_t size, const char *file,
				  const char *func, int line)
{
	void *ret = memalign(align, size);
	if (!ret && size) {
		printk(BIOS_ERR, "%s:%d %s(): "
				"Failed to memalign %zu bytes with %zu alignment.\n",
			file, line, func, size, align);
		abort();
	}
	return ret;
}
#define xmemalign(align, size) xmemalign_work((align), (size), __FILE__, __func__, __LINE__)

#endif /* __COMMONLIB_STDLIB_H__ */
