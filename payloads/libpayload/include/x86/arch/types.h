/*
 *
 * Copyright (C) 2007 Uwe Hermann <uwe@hermann-uwe.de>
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

#ifndef _ARCH_TYPES_H
#define _ARCH_TYPES_H

typedef unsigned char uint8_t;
typedef unsigned char uint_fast8_t;
typedef unsigned char u8;
typedef signed char int8_t;
typedef signed char int_fast8_t;
typedef signed char s8;

typedef unsigned short uint16_t;
typedef unsigned short u16;
typedef unsigned short uint_fast16_t;
typedef signed short int16_t;
typedef signed short int_fast16_t;
typedef signed short s16;

typedef unsigned int uint32_t;
typedef unsigned int uint_fast32_t;
typedef unsigned int u32;
typedef signed int int32_t;
typedef signed int int_fast32_t;
typedef signed int s32;

typedef unsigned long long uint64_t;
typedef unsigned long long uint_fast64_t;
typedef unsigned long long u64;
typedef signed long long int64_t;
typedef signed long long int_fast64_t;
typedef signed long long s64;

typedef long time_t;
typedef long suseconds_t;

#ifndef NULL
#define NULL ((void *)0)
#endif

#endif
