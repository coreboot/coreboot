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

#ifndef _ARCH_MSR_H
#define _ARCH_MSR_H

static inline unsigned long long _rdmsr(unsigned int msr)
{
	unsigned long long val;
	asm volatile("rdmsr" : "=A" (val) : "c" (msr));
	return val;
}

static inline void _wrmsr(unsigned int msr, unsigned long long val)
{
	asm volatile("wrmsr" : : "c" (msr), "A"(val));
}

#define rdmsr(_m, _l, _h) \
	do { \
		unsigned long long _v = _rdmsr((_m)); \
		(_l) = (unsigned int) _v; \
		(_h) = (unsigned int) ((_v >> 32) & 0xFFFFFFFF); \
	} while(0)

static inline void wrmsr(unsigned int msr, unsigned int lo, unsigned int hi)
{
	unsigned long long val = (((unsigned long long) hi) << 32) | lo;
	_wrmsr(msr, val);
}

#endif
