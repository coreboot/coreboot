/* SPDX-License-Identifier: GPL-2.0-only */

/* This file applies to AMD64 products.
 * The definitions come from the AMD64 Programmers Manual vol2
 * Revision 3.30 and/or the device's BKDG.
 */

#ifndef CPU_AMD_CPUID_H
#define CPU_AMD_CPUID_H

#define CPUID_EXT_PM			0x80000007
#define CPUID_MODEL			1
#define CPUID_EBX_CORE_ID		0x8000001E
#define  CPUID_EBX_THREADS_SHIFT	8
#define  CPUID_EBX_THREADS_MASK		(0xFF << CPUID_EBX_THREADS_SHIFT)

#endif /* CPU_AMD_CPUID_H */
