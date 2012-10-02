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

#ifndef _STDIO_H
#define _STDIO_H

#include <stddef.h>

#define EOF (-1)

typedef struct _FILE FILE;

extern FILE *stdout, *stdin, *stderr;

/**
 * @defgroup printf Print functions
 * @{
 */
int snprintf(char *str, size_t size, const char *fmt, ...)
	__attribute__ ((format (printf, 3, 4)));
int sprintf(char *str, const char *fmt, ...)
	__attribute__ ((format (printf, 2, 3)));
int printf(const char *fmt, ...)
	__attribute__ ((format (printf, 1, 2)));
int fprintf(FILE *file, const char *fmt, ...)
	__attribute__ ((format (printf, 2, 3)));
/** @} */

void perror(const char *s);

#define SEEK_SET 0 /**< The seek offset is absolute. */
#define SEEK_CUR 1 /**< The seek offset is against the current position. */
#define SEEK_END 2 /**< The seek offset is against the end of the file. */

#endif
