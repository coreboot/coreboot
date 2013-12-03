/*
 * This file is part of the libpayload project.
 *
 * Copyright (C) 2008 Advanced Micro Devices, Inc.
 * Copyright (C) 2008 coresystems GmbH
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

#ifndef _ARCH_IO_H
#define _ARCH_IO_H

#include <stdint.h>
#include <arch/cache.h>

static inline uint8_t readb(volatile void *_a)
{
	dmb();
	return *(volatile uint8_t *)_a;
}

static inline uint16_t readw(volatile void *_a)
{
	dmb();
	return *(volatile uint16_t *)_a;
}

static inline uint32_t readl(volatile void *_a)
{
	dmb();
	return *(volatile uint32_t *)_a;
}

static inline void writeb(uint8_t _v, volatile void *_a)
{
	dmb();
	*(volatile uint8_t *)_a = _v;
	dmb();
}

static inline void writew(uint16_t _v, volatile void *_a)
{
	dmb();
	*(volatile uint16_t *)_a = _v;
	dmb();
}

static inline void writel(uint32_t _v, volatile void *_a)
{
	dmb();
	*(volatile uint32_t *)_a = _v;
	dmb();
}

#endif
