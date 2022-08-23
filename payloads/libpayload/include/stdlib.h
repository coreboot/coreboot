/*
 *
 * Copyright (C) 2008 Advanced Micro Devices, Inc.
 * Copyright 2013 Google Inc.
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

#ifndef _STDLIB_H
#define _STDLIB_H

#include <die.h>
#include <stddef.h>
#include <string.h>

/**
 * @defgroup malloc Memory allocation functions
 * @{
 */
void free(void *ptr);
void *malloc(size_t size);
void *calloc(size_t nmemb, size_t size);
void *realloc(void *ptr, size_t size);
void *memalign(size_t align, size_t size);
void *dma_malloc(size_t size);
void *dma_memalign(size_t align, size_t size);

#if CONFIG(LP_DEBUG_MALLOC) && !defined(IN_MALLOC_C)
#include <stdio.h>
void print_malloc_map(void);
#define free(p)	({ \
	void *__p = p; \
	printf("free(%p) called from %s:%s:%d...\n", __p, __FILE__, __func__, \
	       __LINE__);\
	printf("PRE free()\n"); \
	print_malloc_map(); \
	free(__p); \
	printf("POST free()\n"); \
	print_malloc_map(); \
})
#define malloc(s) ({ \
	size_t __s = s; \
	void *ptr; \
	printf("malloc(%zu) called from %s:%s:%d...\n", __s, __FILE__, \
	       __func__, __LINE__);\
	printf("PRE malloc\n"); \
	print_malloc_map(); \
	ptr = malloc(__s); \
	printf("POST malloc (ptr = %p)\n", ptr); \
	print_malloc_map(); \
	ptr; \
})
#define calloc(n, s) ({ \
	size_t __n = n, __s = s; \
	void *ptr; \
	printf("calloc(%zu, %zu) called from %s:%s:%d...\n", __n, __s, \
	       __FILE__, __func__, __LINE__);\
	printf("PRE calloc\n"); \
	print_malloc_map(); \
	ptr = calloc(__n, __s); \
	printf("POST calloc (ptr = %p)\n", ptr); \
	print_malloc_map(); \
	ptr; \
})
#define realloc(p, s) ({ \
	void *__p = p; \
	size_t __s = s; \
	void *ptr; \
	printf("realloc(%p, %zu) called from %s:%s:%d...\n", __p, __s, \
	       __FILE__, __func__, __LINE__);\
	printf("PRE realloc\n"); \
	print_malloc_map(); \
	ptr = realloc(__p, __s); \
	printf("POST realloc (ptr = %p)\n", ptr); \
	print_malloc_map(); \
	ptr; \
})
#define memalign(a, s) ({ \
	size_t __a = a, __s = s; \
	void *ptr; \
	printf("memalign(%zu, %zu) called from %s:%s:%d...\n", __a, __s, \
		__FILE__, __func__, __LINE__);\
	printf("PRE memalign\n"); \
	print_malloc_map(); \
	ptr = memalign(__a, __s); \
	printf("POST memalign (ptr = %p)\n", ptr); \
	print_malloc_map(); \
	ptr; \
})
#define dma_malloc(s) ({ \
	size_t __s = s; \
	void *ptr; \
	printf("dma_malloc(%zu) called from %s:%s:%d...\n", __s, __FILE__, \
	       __func__, __LINE__);\
	printf("PRE dma_malloc\n"); \
	print_malloc_map(); \
	ptr = dma_malloc(__s); \
	printf("POST dma_malloc (ptr = %p)\n", ptr); \
	print_malloc_map(); \
	ptr; \
})
#define dma_memalign(a, s) ({ \
	size_t __a = a, __s = s; \
	void *ptr; \
	printf("dma_memalign(%zu, %zu) called from %s:%s:%d...\n", __a, __s, \
	       __FILE__, __func__, __LINE__);\
	printf("PRE dma_memalign\n"); \
	print_malloc_map(); \
	ptr = dma_memalign(__a, __s); \
	printf("POST dma_memalign (ptr = %p)\n", ptr); \
	print_malloc_map(); \
	ptr; \
})
#endif

void init_dma_memory(void *start, u32 size);
int dma_initialized(void);
int dma_coherent(const void *ptr);

static inline void *xmalloc_work(size_t size, const char *file,
				 const char *func, int line)
{
	void *ret = malloc(size);
	if (!ret && size) {
		die_work(file, func, line, "Failed to malloc %zu bytes.\n",
			 size);
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

static inline void *xmemalign_work(size_t align, size_t size, const char *file,
				  const char *func, int line)
{
	void *ret = memalign(align, size);
	if (!ret && size) {
		die_work(file, func, line,
			 "Failed to memalign %zu bytes with %zu alignment.\n",
			 size, align);
	}
	return ret;
}
#define xmemalign(align, size) \
	xmemalign_work((align), (size), __FILE__, __func__, __LINE__)
/** @} */

/**
 * @defgroup stdlib String conversion functions
 * @{
 */
long int strtol(const char *s, char **nptr, int base);
long long int strtoll(const char *s, char **nptr, int base);
unsigned long int strtoul(const char *s, char **nptr, int base);
unsigned long long int strtoull(const char *s, char **nptr, int base);
long atol(const char *nptr);

/** @} */

/**
 * @defgroup rand Random number generator functions
 * @{
 */
int rand_r(unsigned int *seed);
int rand(void);
void srand(unsigned int seed);
/** @} */

/**
 * @defgroup misc Misc functions
 * @{
 */
int abs(int j);
long int labs(long int j);
long long int llabs(long long int j);
/** @} */

/* Enter remote GDB mode. Will initialize connection if not already up. */
void gdb_enter(void);
/* Disconnect existing GDB connection if one exists. */
void gdb_exit(s8 exit_status);

/**
 * Stop execution and halt the processor (this function does not return).
 */
void halt(void) __attribute__((noreturn));
void exit(int status) __attribute__((noreturn));
#define abort() halt()    /**< Alias for the halt() function */
#if CONFIG(LP_REMOTEGDB)
/* Override abort()/halt() to trap into GDB if it is enabled. */
#define halt() do { gdb_enter(); halt(); } while (0)
#endif

void qsort(void *aa, size_t n, size_t es, int (*cmp)(const void *, const void *));
char *getenv(const char*);
uint64_t __umoddi3(uint64_t num, uint64_t den);
uint64_t  __udivdi3(uint64_t num, uint64_t den);
uint64_t __ashldi3(uint64_t num, unsigned shift);
uint64_t __lshrdi3(uint64_t num, unsigned shift);

#endif
