/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef ARCH_CPUID_H
#define ARCH_CPUID_H

#include <types.h>

struct cpuid_result {
	uint32_t eax;
	uint32_t ebx;
	uint32_t ecx;
	uint32_t edx;
};

/*
 * Generic CPUID function
 */
static inline struct cpuid_result cpuid(const uint32_t eax)
{
	struct cpuid_result result;
	asm volatile(
		"cpuid;"
		: "=a" (result.eax),
		  "=b" (result.ebx),
		  "=c" (result.ecx),
		  "=d" (result.edx)
		: "a" (eax));
	return result;
}

/*
 * Generic Extended CPUID function
 */
static inline struct cpuid_result cpuid_ext(const uint32_t eax, const uint32_t ecx)
{
	struct cpuid_result result;
	asm volatile(
		"cpuid;"
		: "=a" (result.eax),
		  "=b" (result.ebx),
		  "=c" (result.ecx),
		  "=d" (result.edx)
		: "a" (eax), "c" (ecx));
	return result;
}

/*
 * CPUID functions returning a single datum
 */
static inline uint32_t cpuid_eax(uint32_t eax)
{
	asm volatile(
		"cpuid;"
		: "+a" (eax)
		:: "ebx", "ecx", "edx");
	return eax;
}

static inline uint32_t cpuid_ebx(const uint32_t eax)
{
	uint32_t ebx;

	asm volatile(
		"cpuid;"
		: "=b" (ebx)
		: "a" (eax)
		: "ecx", "edx");
	return ebx;
}

static inline uint32_t cpuid_ecx(const uint32_t eax)
{
	uint32_t ecx;

	asm volatile(
		"cpuid;"
		: "=c" (ecx)
		: "a" (eax)
		: "ebx", "edx");
	return ecx;
}

static inline uint32_t cpuid_edx(const uint32_t eax)
{
	uint32_t edx;

	asm volatile(
		"cpuid;"
		: "=d" (edx)
		: "a" (eax)
		: "ebx", "ecx");
	return edx;
}

#endif /* ARCH_CPUID_H */
