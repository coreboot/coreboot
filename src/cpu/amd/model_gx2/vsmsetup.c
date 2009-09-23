/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2000 Erik Arjan Hendriks
 * Copyright (C) 2000 Scyld Computing Corporation
 * Copyright (C) 2001 University of California.  LA-CC Number 01-67.
 * Copyright (C) 2005 Nick.Barker9@btinternet.com
 * Copyright (C) 2007 coresystems GmbH
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 * LA-CC is the Los Alamos Control and Compliance Number, see also:
 * http://supply.lanl.gov/property/customs/eximguide/default.shtml
 */

#include <console/console.h>
#include <device/device.h>
#include <device/pci.h>
#include <string.h>
#include <cpu/cpu.h>
#include <cpu/x86/lapic.h>
#include <cpu/x86/cache.h>
#include <arch/io.h>

u32 VSA_vrRead(u16 classIndex);
void do_vsmbios(void);

#define VSA2_BUFFER		0x60000
#define VSA2_ENTRY_POINT	0x60020
#define VSA2_SIGNATURE	0x56534132	// 'VSA2' returned in EAX 
#define SIGNATURE			0x03
/**
 * TODO.
 *
 * The address arguments to this function are PHYSICAL ADDRESSES!
 *
 * @param smm TODO.
 * @param sysm TODO.
 */
static void real_mode_switch_call_vsm(unsigned long smm, unsigned long sysm)
{
	u16 entryHi = (VSA2_ENTRY_POINT & 0xffff0000) >> 4;
	u16 entryLo = (VSA2_ENTRY_POINT & 0xffff);

	__asm__ __volatile__(
		/* Paranoia -- does ecx get saved? not sure. This is
		 * the easiest safe thing to do.
		 */
		"	pushal			\n"
		/* Save the stack. */
		"	mov 	%%esp, __stack	\n"
		"	jmp 	1f		\n"
		"__stack: .long 0		\n" 
		"1:\n"
		/* Get devfn into %%ecx. */
		"	movl    %%esp, %%ebp	\n"
		/* Get the smm and sysm args into ecx and edx. */
		"	movl    %0, %%ecx	\n"
		"	movl    %1, %%edx	\n"
		/* Load 'our' gdt. */
		"	lgdt	%%cs:__mygdtaddr	\n"
		/* This configures CS properly for real mode. */
		"	ljmp	$0x28, $__rms_16bit\n"
		"__rms_16bit:		  	\n"
		"	.code16			\n"
		/* 16 bit code from here on... */
		/* Load the segment registers with properly configured segment
		 * descriptors. They will retain these configurations (limits,
		 * writability, etc.) once protected mode is turned off.
		 */
		"	mov	$0x30, %%ax	\n"
		"	mov	%%ax, %%ds       	\n"
		"	mov	%%ax, %%es       	\n"
		"	mov	%%ax, %%fs       	\n"
		"	mov	%%ax, %%gs       	\n"
		"	mov	%%ax, %%ss       	\n"
		/* Turn off protection (bit 0 in CR0). */
		"	movl	%%cr0, %%eax	\n"
		"	andl	$0xFFFFFFFE, %%eax \n"
		"	movl	%%eax, %%cr0	\n"
		/* Now really going into real mode. */
		"	ljmp	$0,  $__rms_real\n"
		"__rms_real:			\n"

		/* Put the stack at the end of page zero.
		 * That way we can easily share it between real and protected,
		 * since the 16-bit ESP at segment 0 will work for any case.
		 */

		/* Setup a stack. */
		"	mov	$0x0, %%ax	\n"
		"	mov	%%ax, %%ss	\n"
		"	movl	$0x1000, %%eax	\n"
		"	movl	%%eax, %%esp	\n"
		/* Dump zeros in the other segregs. */
		"	mov	%%ax, %%es     	\n"
		/* FIXME: Big real mode for gs, fs? */
		"	mov	%%ax, %%fs     	\n"
		"	mov	%%ax, %%gs     	\n"
		"	mov	$0x40, %%ax	\n"
		"	mov	%%ax, %%ds	\n"
		/* "     mov     %%cx, %%ax      \n" */
		"	movl    %0, %%ecx	\n"
		"	movl    %1, %%edx	\n"
		/* Call the VSA2 entry point address. */
		"	lcall	%2, %3\n"
		/* If we got here, just about done.
		 * Need to get back to protected mode.
		 */
		"	movl	%%cr0, %%eax	\n" 
		"	orl	$0x0000001, %%eax\n"	/* PE = 1 */
		"	movl	%%eax, %%cr0	\n"
		/* Now that we are in protected mode,
		 * jump to a 32 bit code segment.
		 */
		"	data32	ljmp	$0x10, $vsmrestart\n"
		"vsmrestart:\n"
		"	.code32\n"
		"	movw	$0x18, %%ax    	\n"
		"	mov	%%ax, %%ds     	\n"
		"	mov	%%ax, %%es	\n"
		"	mov	%%ax, %%fs	\n"
		"	mov	%%ax, %%gs	\n"
		"	mov	%%ax, %%ss	\n"
		/* Restore proper gdt. */
		"	lgdt	%%cs:gdtarg	\n"
		".globl vsm_exit		\n"
		"vsm_exit:			\n"
		"	mov	__stack, %%esp	\n"
		"	popal			\n"::
		"g" (smm), "g"(sysm), "g"(entryHi), "g"(entryLo)
	);
}

__asm__(".text\n" "real_mode_switch_end:\n");
extern char real_mode_switch_end[];

/* andrei: Some VSA virtual register helpers: raw read and MSR read. */

u32 VSA_vrRead(u16 classIndex)
{
	unsigned eax, ebx, ecx, edx;
	asm volatile (
		"movw	$0x0AC1C, %%dx		\n"
		"orl	$0x0FC530000, %%eax	\n"
		"outl	%%eax, %%dx		\n"
		"addb	$2, %%dl		\n"
		"inw	%%dx, %%ax		\n"
		: "=a" (eax), "=b"(ebx), "=c"(ecx), "=d"(edx) 
		: "a"(classIndex)
	);

	return eax;
}

u32 VSA_msrRead(u32 msrAddr)
{
	unsigned eax, ebx, ecx, edx;
	asm volatile (
		"movw	$0x0AC1C, %%dx			\n"
		"movl	$0x0FC530007, %%eax		\n"
		"outl	%%eax, %%dx				\n"
		"addb	$2, %%dl				\n"
		"inw	%%dx, %%ax				\n"
		: "=a" (eax), "=b"(ebx), "=c"(ecx), "=d"(edx)
		: "c"(msrAddr)
	);

	return eax;
}

void do_vsmbios(void)
{
	unsigned char *buf;
	int i;

	printk_err( "do_vsmbios\n");
	/* Clear VSM BIOS data area. */
	for (i = 0x400; i < 0x500; i++)
		*(volatile unsigned char *)i = 0;
	/* set up cbfs and find the vsa file -- later */
/*
	init_archive(&archive);

	if (find_file(&archive, "blob/vsa", &file))
		die("FATAL: NO VSA found!\n");

	if (process_file(&file, (void *)VSA2_BUFFER))
		die("FATAL: Processing /blob/vsa failed\n");
 */

	buf = (unsigned char *)VSA2_BUFFER;
	printk_debug("buf[0x20] signature is %x:%x:%x:%x\n",
	       buf[0x20], buf[0x21], buf[0x22], buf[0x23]);
	/* Check for POST code at start of vsainit.bin. If you don't see it,
	 * don't bother.
	 */
	if ((buf[0x20] != 0xb0) || (buf[0x21] != 0x10) ||
	    (buf[0x22] != 0xe6) || (buf[0x23] != 0x80)) {
		die("FATAL: no vsainit.bin signature, skipping!\n");
	}

	/* ecx gets smm, edx gets sysm. */
	printk_err("Call real_mode_switch_call_vsm\n");
//	real_mode_switch_call_vsm(MSR_GLIU0_SMM, MSR_GLIU0_SYSMEM);

	/* Restart Timer 1. */
	outb(0x56, 0x43);
	outb(0x12, 0x41);

	/* Check that VSA is running OK. */
	if (VSA_vrRead(SIGNATURE) == VSA2_SIGNATURE)
		printk_debug("do_vsmbios: VSA2 VR signature verified\n");
	else
		die("FATAL: VSA2 VR signature not valid, install failed!\n");
}

