/*
 *
 * Copyright (C) 2010 coresystems GmbH
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

#ifndef __STDINT_H
#define __STDINT_H

#include <arch/types.h>

typedef long long int      intmax_t;
typedef unsigned long long uintmax_t;

typedef unsigned long uintptr_t;
typedef long intptr_t;

typedef long ptrdiff_t;

#define INT8_MAX	(127)
#define INT16_MAX	(32767)
#define INT32_MAX	(2147483647)
#define INT64_MAX	(9223372036854775807LL)

#define INT8_MIN	(-INT8_MAX - 1)
#define INT16_MIN	(-INT16_MAX - 1)
#define INT32_MIN	(-INT32_MAX - 1)
#define INT64_MIN	(-INT64_MAX - 1)

#define UINT8_MAX	(255)
#define UINT16_MAX	(65535)
#define UINT32_MAX	(4294967295U)
#define UINT64_MAX	(18446744073709551615ULL)

#ifndef SIZE_MAX
#ifdef __SIZE_MAX__
#define SIZE_MAX __SIZE_MAX__
#else
#define SIZE_MAX (~(size_t)0)
#endif /* __SIZE_MAX__ */
#endif /* SIZE_MAX */

#endif
