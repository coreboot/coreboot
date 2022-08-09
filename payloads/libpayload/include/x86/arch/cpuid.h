/*
 *
 * Copyright 2018 Google Inc.
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

#ifndef _ARCH_CPUID_H
#define _ARCH_CPUID_H

#define cpuid(fn, eax, ebx, ecx, edx) \
	asm("cpuid" : "=a"(eax), "=b"(ebx), "=c"(ecx), "=d"(edx) : "0"(fn))

#define _declare_cpuid(reg)					\
	static inline unsigned int cpuid_##reg(unsigned int fn)	\
	{							\
		unsigned int eax, ebx, ecx, edx;		\
		cpuid(fn, eax, ebx, ecx, edx);			\
		return reg;					\
	}

_declare_cpuid(eax)
_declare_cpuid(ebx)
_declare_cpuid(ecx)
_declare_cpuid(edx)

#undef _declare_cpuid

#define cpuid_sub_leaf(fn, sub_leaf, eax, ebx, ecx, edx) \
	asm("cpuid" : "=a"(eax), "=b"(ebx), "=c"(ecx), "=d"(edx) : "0"(fn), "1"(sub_leaf))

#define _declare_cpuid_sub_leaf(reg)					\
	static inline unsigned int cpuid_sub_leaf_##reg( \
		unsigned int fn, unsigned int sub_leaf)	\
	{								\
		unsigned int eax, ebx, ecx, edx;			\
		cpuid_sub_leaf(fn, sub_leaf, eax, ebx, ecx, edx);	\
		return reg;						\
	}

_declare_cpuid_sub_leaf(eax)
_declare_cpuid_sub_leaf(ebx)
_declare_cpuid_sub_leaf(ecx)
_declare_cpuid_sub_leaf(edx)

#undef _declare_cpuid_sub_leaf

static inline unsigned int cpuid_max(void)
{
	return cpuid_eax(0);
}

static inline unsigned int cpuid_family(void)
{
	const unsigned int eax = cpuid_eax(1);
	return (eax & 0xff00000) >> (20 - 4) | (eax & 0xf00) >> 8;
}

static inline unsigned int cpuid_model(void)
{
	const unsigned int eax = cpuid_eax(1);
	return (eax & 0xf0000) >> (16 - 4) | (eax & 0xf0) >> 4;
}

enum intel_fam6_model {
	NEHALEM		= 0x25,
	SANDYBRIDGE	= 0x2a,
	IVYBRIDGE	= 0x3a,
	HASWELL		= 0x3c,
	BROADWELL_U	= 0x3d,
	HASWELL_U	= 0x45,
	HASWELL_GT3E	= 0x46,
	BROADWELL	= 0x47,
	SKYLAKE_U_Y	= 0x4e,
	APOLLOLAKE	= 0x5c,
	SKYLAKE_S_H	= 0x5e,
	KABYLAKE_U_Y	= 0x8e,
	KABYLAKE_S_H	= 0x9e,
};

#endif
