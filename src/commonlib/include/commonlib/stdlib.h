/* SPDX-License-Identifier: BSD-3-Clause */

#ifndef __COMMONLIB_STDLIB_H__
#define __COMMONLIB_STDLIB_H__

#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#if CONFIG(COREBOOT_BUILD)
#include <console/console.h>
#include <halt.h>
#define HALT(x)			halt()
#else
#include <stdio.h>
#define printk(level, ...)	printf(__VA_ARGS__)
#define HALT(x)			abort()
#endif

static inline void *xmalloc_work(size_t size, const char *file,
				 const char *func, int line)
{
	void *ret = malloc(size);
	if (!ret && size) {
		printk(BIOS_ERR, "%s/%s/line %d: Failed to malloc %zu bytes\n",
			file, func, line, size);
		while (1)
			HALT(1);
	}
	return ret;
}
#define xmalloc(size) xmalloc_work((size), __FILE__, __FUNCTION__, __LINE__)

static inline void *xzalloc_work(size_t size, const char *file,
				 const char *func, int line)
{
	void *ret = xmalloc_work(size, file, func, line);
	memset(ret, 0, size);
	return ret;
}
#define xzalloc(size) xzalloc_work((size), __FILE__, __FUNCTION__, __LINE__)

void *dma_malloc(size_t size);
int dma_coherent(void *ptr);

#endif /* __COMMONLIB_STDLIB_H__ */
