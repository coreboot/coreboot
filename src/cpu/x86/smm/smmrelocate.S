/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2008-2010 coresystems GmbH
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; version 2 of
 * the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston,
 * MA 02110-1301 USA
 */

// Make sure no stage 2 code is included:
#define __PRE_RAM__

/* On AMD's platforms we can set SMBASE by writing an MSR */
#if !CONFIG_NORTHBRIDGE_AMD_AMDK8 && !CONFIG_NORTHBRIDGE_AMD_AMDFAM10

// FIXME: Is this piece of code southbridge specific, or
// can it be cleaned up so this include is not required?
// It's needed right now because we get our DEFAULT_PMBASE from
// here.
#if CONFIG_SOUTHBRIDGE_INTEL_I82801GX
#include "../../../southbridge/intel/i82801gx/i82801gx.h"
#elif CONFIG_SOUTHBRIDGE_INTEL_I82801DX
#include "../../../southbridge/intel/i82801dx/i82801dx.h"
#elif CONFIG_SOUTHBRIDGE_INTEL_SCH
#include "../../../southbridge/intel/sch/sch.h"
#elif CONFIG_SOUTHBRIDGE_INTEL_BD82X6X || CONFIG_SOUTHBRIDGE_INTEL_C216
#include "../../../southbridge/intel/bd82x6x/pch.h"
#elif CONFIG_SOUTHBRIDGE_INTEL_I82801IX
#include "../../../southbridge/intel/i82801ix/i82801ix.h"
#else
#error "Southbridge needs SMM handler support."
#endif

#if CONFIG_SMM_TSEG

#if CONFIG_NORTHBRIDGE_INTEL_SANDYBRIDGE || CONFIG_NORTHBRIDGE_INTEL_IVYBRIDGE
#include <northbridge/intel/sandybridge/sandybridge.h>
#define TSEG_BAR (DEFAULT_PCIEXBAR | TSEG)
#else
#error "Northbridge must define TSEG_BAR."
#endif
#include <cpu/x86/mtrr.h>

#endif /* CONFIG_SMM_TSEG */

#define LAPIC_ID 0xfee00020

.global smm_relocation_start
.global smm_relocation_end

/* initially SMM is some sort of real mode. */
.code16

/**
 * When starting up, x86 CPUs have their SMBASE set to 0x30000. However,
 * this is not a good place for the SMM handler to live, so it needs to
 * be relocated.
 * Traditionally SMM handlers used to live in the A segment (0xa0000).
 * With growing SMM handlers, more CPU cores, etc. CPU vendors started
 * allowing to relocate the handler to the end of physical memory, which
 * they refer to as TSEG.
 * This trampoline code relocates SMBASE to base address - ( lapicid * 0x400 )
 *
 * Why 0x400? It is a safe value to cover the save state area per CPU. On
 * current AMD CPUs this area is _documented_ to be 0x200 bytes. On Intel
 * Core 2 CPUs the _documented_ parts of the save state area is 48 bytes
 * bigger, effectively sizing our data structures 0x300 bytes.
 *
 * Example (with SMM handler living at 0xa0000):
 *
 * LAPICID	SMBASE		SMM Entry 	SAVE STATE
 *    0		0xa0000		0xa8000		0xafd00
 *    1		0x9fc00		0xa7c00		0xaf900
 *    2		0x9f800		0xa7800		0xaf500
 *    3		0x9f400		0xa7400		0xaf100
 *    4		0x9f000		0xa7000		0xaed00
 *    5		0x9ec00		0xa6c00		0xae900
 *    6		0x9e800		0xa6800		0xae500
 *    7		0x9e400		0xa6400		0xae100
 *    8		0x9e000		0xa6000		0xadd00
 *    9		0x9dc00		0xa5c00		0xad900
 *   10		0x9d800		0xa5800		0xad500
 *   11		0x9d400		0xa5400		0xad100
 *   12		0x9d000		0xa5000		0xacd00
 *   13		0x9cc00		0xa4c00		0xac900
 *   14		0x9c800		0xa4800		0xac500
 *   15		0x9c400		0xa4400		0xac100
 *    .		   .		   .		   .
 *    .		   .		   .		   .
 *    .		   .		   .		   .
 *   31		0x98400		0xa0400		0xa8100
 *
 * With 32 cores, the SMM handler would need to fit between
 * 0xa0000-0xa0400 and the stub plus stack would need to go
 * at 0xa8000-0xa8100 (example for core 0). That is not enough.
 *
 * This means we're basically limited to 16 cpu cores before
 * we need to move the SMM handler to TSEG.
 *
 * Note: Some versions of Pentium M need their SMBASE aligned to 32k.
 * On those the above only works for up to 2 cores. But for now we only
 * care fore Core (2) Duo/Solo
 *
 */

smm_relocation_start:
	/* Check revision to see if AMD64 style SMM_BASE
	 *   Intel Core Solo/Duo:  0x30007
	 *   Intel Core2 Solo/Duo: 0x30100
	 *   Intel SandyBridge:    0x30101
	 *   AMD64:                0x3XX64
	 * This check does not make much sense, unless someone ports
	 * SMI handling to AMD64 CPUs.
	 */

	mov $0x38000 + 0x7efc, %ebx
	addr32 mov (%ebx), %al
	cmp $0x64, %al
	je 1f

	mov $0x38000 + 0x7ef8, %ebx
	jmp smm_relocate
1:
	mov $0x38000 + 0x7f00, %ebx

smm_relocate:
	/* Get this CPU's LAPIC ID */
	movl $LAPIC_ID, %esi
	addr32 movl (%esi), %ecx
	shr  $24, %ecx

	/* calculate offset by multiplying the
	 * apic ID by 1024 (0x400)
	 */
	movl %ecx, %edx
	shl $10, %edx

#if CONFIG_SMM_TSEG
	movl	$(TSEG_BAR), %ecx	/* Get TSEG base from PCIE */
	addr32	movl (%ecx), %eax	/* Save TSEG_BAR in %eax */
	andl	$~1, %eax		/* Remove lock bit */
#else
	movl $0xa0000, %eax
#endif
	subl %edx, %eax	/* subtract offset, see above */

	addr32 movl %eax, (%ebx)

#if CONFIG_SMM_TSEG
	/* Check for SMRR capability in MTRRCAP[11] */
	movl	$MTRRcap_MSR, %ecx
	rdmsr
	bt	$11, %eax
	jnc	skip_smrr

	/* TSEG base */
	movl	$(TSEG_BAR), %ecx	/* Get TSEG base from PCIE */
	addr32	movl (%ecx), %eax	/* Save TSEG_BAR in %eax */
	andl	$~1, %eax		/* Remove lock bit */
	movl	%eax, %ebx

	/* Set SMRR base address. */
	movl	$SMRRphysBase_MSR, %ecx
	orl	$MTRR_TYPE_WRBACK, %eax
	xorl	%edx, %edx
	wrmsr

	/* Set SMRR mask. */
	movl	$SMRRphysMask_MSR, %ecx
	movl	$(~(CONFIG_SMM_TSEG_SIZE - 1) | MTRRphysMaskValid), %eax
	xorl	%edx, %edx
	wrmsr

#if CONFIG_NORTHBRIDGE_INTEL_SANDYBRIDGE || CONFIG_NORTHBRIDGE_INTEL_IVYBRIDGE
	/*
	 * IED base is top 4M of TSEG
	 */
	addl	$(CONFIG_SMM_TSEG_SIZE - IED_SIZE), %ebx
	movl	$(0x30000 + 0x8000 + 0x7eec), %eax
	addr32	movl %ebx, (%eax)
#endif

skip_smrr:
#endif

	/* The next section of code is potentially southbridge specific */

	/* Clear SMI status */
	movw $(DEFAULT_PMBASE + 0x34), %dx
	inw %dx, %ax
	outw %ax, %dx

	/* Clear PM1 status */
	movw $(DEFAULT_PMBASE + 0x00), %dx
	inw %dx, %ax
	outw %ax, %dx

	/* Set EOS bit so other SMIs can occur */
	movw $(DEFAULT_PMBASE + 0x30), %dx
	inl %dx, %eax
	orl $(1 << 1), %eax
	outl %eax, %dx

	/* End of southbridge specific section. */

#if CONFIG_DEBUG_SMM_RELOCATION
	/* print [SMM-x] so we can determine if CPUx went to SMM */
	movw $CONFIG_TTYS0_BASE, %dx
	mov $'[', %al
	outb %al, %dx
	mov $'S', %al
	outb %al, %dx
	mov $'M', %al
	outb %al, %dx
	outb %al, %dx
	movb $'-', %al
	outb %al, %dx
	/* calculate ascii of cpu number. More than 9 cores? -> FIXME */
	movb %cl, %al
	addb $'0', %al
	outb %al, %dx
	mov $']', %al
	outb %al, %dx
	mov $'\r', %al
	outb %al, %dx
	mov $'\n', %al
	outb %al, %dx
#endif

	/* That's it. return */
	rsm
smm_relocation_end:
#endif
