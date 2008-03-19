/*
 * This file is part of the libpayload project.
 *
 * Copyright (C) 2008 Advanced Micro Devices, Inc.
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

#ifndef LIBPAYLOAD_H_
#define LIBPAYLOAD_H_

#include <autoconf.h>
#include <stddef.h>
#include <arch/types.h>
#include <arch/io.h>
#include <stdarg.h>

#define MIN(a,b) ((a) < (b) ? (a) : (b))
#define MAX(a,b) ((a) > (b) ? (a) : (b))
#define ARRAY_SIZE(a) (sizeof(a) / sizeof((a)[0]))

/* serial.c */
void serial_putchar(unsigned char c);

/* console.c */
int putchar(int c);
extern int last_putchar;

#define havechar havekey

/* ctype.c */
int isspace(int c);
int isdigit(int c);
int tolower(int c);

/* malloc.c */
void *malloc(size_t size);
void free(void *ptr);
void *calloc(size_t nmemb, size_t size);
void *realloc(void *ptr, size_t size);

/* memory.c */
int memcmp(const char *s1, const char *s2, size_t len);
void *memcpy(void *dst, const void *src, size_t n);
void *memset(void *s, int c, size_t n);
void *memmove(void *dst, const void *src, size_t n);

/* printf.c */
int sprintf(char *str, const char *fmt, ...);
int vsnprintf(char *str, size_t size, const char *fmt, va_list ap);
int vsprintf(char *str, const char *fmt, va_list ap);
int printf(const char *fmt, ...);
int vprintf(const char *fmt, va_list ap);

/* string.c */
size_t strnlen(const char *src, size_t max);
size_t strlen(const char *src);
int strcmp(const char *s1, const char *s2);
char *strdup(const char *s);
char *strchr(const char *s, int c);
char *strncpy(char *to, const char *from, int count);
char * strcpy (char *dest, const char *src);
char * strstr (const char *s1, const char *s2);

/* ipchchecksum.c */
unsigned short ipchksum(const unsigned short *ptr, unsigned long nbytes);

/* util.S */
#define abort() halt()
void halt(void) __attribute__ ((noreturn));

/* Timer functions - defined by each arcitecture */

void ndelay(unsigned int);
void mdelay(unsigned int);
void delay(unsigned int);

#endif
