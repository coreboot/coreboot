/*
 * This file is part of the libpayload project.
 *
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

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
