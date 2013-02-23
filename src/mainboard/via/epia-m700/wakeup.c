/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2008 Rudolf Marek <r.marek@assembler.cz>
 * Copyright (C) 2009 One Laptop per Child, Association, Inc.
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
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */

/* FIXME This code should be dropped and instead the generic resume code
 * should be used.
 */

/* Parts of this code is taken from reboot.c from Linux. */

/*
 * This file mostly copied from Rudolf's S3 patch, some changes in
 * acpi_jump_wake().
 */

#include <stdint.h>
#include <string.h>
#include <arch/io.h>
#include <console/console.h>
#include <delay.h>
#include "wakeup.h"

int enable_a20(void);

/*
 * The following code and data reboots the machine by switching to real
 * mode and jumping to the BIOS reset entry point, as if the CPU has
 * really been reset. The previous version asked the keyboard
 * controller to pulse the CPU reset line, which is more thorough, but
 * doesn't work with at least one type of 486 motherboard. It is easy
 * to stop this code working; hence the copious comments.
 */

static unsigned long long real_mode_gdt_entries[3] = {
	0x0000000000000000ULL,	/* Null descriptor */
	0x00009a000000ffffULL,	/* 16-bit real-mode 64k code at 0x00000000 */
	0x000092000100ffffULL	/* 16-bit real-mode 64k data at 0x00000100 */
};

struct Xgt_desc_struct {
	unsigned short size;
	unsigned long address __attribute__ ((packed));
	unsigned short pad;
} __attribute__ ((packed));

static struct Xgt_desc_struct real_mode_gdt = {
	sizeof(real_mode_gdt_entries) - 1,
	(long)real_mode_gdt_entries
},
real_mode_idt = {0x3ff, 0},
no_idt = { 0, 0 };

/*
 * This is 16-bit protected mode code to disable paging and the cache,
 * switch to real mode and jump to the BIOS reset code.
 *
 * The instruction that switches to real mode by writing to CR0 must be
 * followed immediately by a far jump instruction, which set CS to a
 * valid value for real mode, and flushes the prefetch queue to avoid
 * running instructions that have already been decoded in protected
 * mode.
 *
 * Clears all the flags except ET, especially PG (paging), PE
 * (protected-mode enable) and TS (task switch for coprocessor state
 * save). Flushes the TLB after paging has been disabled. Sets CD and
 * NW, to disable the cache on a 486, and invalidates the cache. This
 * is more like the state of a 486 after reset. I don't know if
 * something else should be done for other chips.
 *
 * More could be done here to set up the registers as if a CPU reset had
 * occurred; hopefully real BIOSs don't assume much.
 */

//      0x66, 0x0d, 0x00, 0x00, 0x00, 0x60,     /* orl $0x60000000, %eax */

static unsigned char real_mode_switch[] = {
	0x66, 0x0f, 0x20, 0xc0,			/* movl %cr0,%eax */
	0x24, 0xfe,				/* andb $0xfe,al */
	0x66, 0x0f, 0x22, 0xc0			/* movl %eax,%cr0 */
};

static unsigned char jump_to_wakeup[] = {
	0xea, 0x00, 0x00, 0x00, 0xe0		/* ljmp $0xffff, $0x0000 */
};

void acpi_jump_wake(u32 vector)
{
	u32 dwEip;
	struct Xgt_desc_struct *wake_thunk16_Xgt_desc;

	printk(BIOS_DEBUG, "IN ACPI JUMP WAKE TO %x\n", vector);
	if (enable_a20())
		die("failed to enable A20\n");
	printk(BIOS_DEBUG, "IN ACPI JUMP WAKE TO 3 %x\n", vector);

	*((u16 *) (jump_to_wakeup + 3)) = (u16) (vector >> 4);
	printk(BIOS_DEBUG, "%x %x %x %x %x\n", jump_to_wakeup[0], jump_to_wakeup[1],
		     jump_to_wakeup[2], jump_to_wakeup[3], jump_to_wakeup[4]);

	memcpy((void *)(WAKE_THUNK16_ADDR - sizeof(real_mode_switch) - 100),
	       real_mode_switch, sizeof(real_mode_switch));
	memcpy((void *)(WAKE_THUNK16_ADDR - 100), jump_to_wakeup,
	       sizeof(jump_to_wakeup));

	//jason_tsc_count();
	printk(BIOS_EMERG, "file '%s', line %d\n\n", __FILE__, __LINE__);
	//jason_tsc_count_end();

	unsigned long long *real_mode_gdt_entries_at_eseg;
	real_mode_gdt_entries_at_eseg = (void *)WAKE_THUNK16_GDT;	/* Copy from real_mode_gdt_entries and change limition to 1M and data base to 0; */
	real_mode_gdt_entries_at_eseg[0] = 0x0000000000000000ULL;	/* Null descriptor */
	real_mode_gdt_entries_at_eseg[1] = 0x000f9a000000ffffULL;	/* 16-bit real-mode 1M code at 0x00000000 */
	real_mode_gdt_entries_at_eseg[2] = 0x000f93000000ffffULL;	/* 16-bit real-mode 1M data at 0x00000000 */

	wake_thunk16_Xgt_desc = (void *)WAKE_THUNK16_XDTR;
	wake_thunk16_Xgt_desc[0].size = sizeof(real_mode_gdt_entries) - 1;
	wake_thunk16_Xgt_desc[0].address = (long)real_mode_gdt_entries_at_eseg;
	wake_thunk16_Xgt_desc[1].size = 0x3ff;
	wake_thunk16_Xgt_desc[1].address = 0;
	wake_thunk16_Xgt_desc[2].size = 0;
	wake_thunk16_Xgt_desc[2].address = 0;

	/* Added this code to get current value of EIP. */
	__asm__ volatile (
		"calll geip\n\t"
		"geip: \n\t"
		"popl %0\n\t"
		: "=a" (dwEip)
	);

	unsigned char *dest, *src;
	src = (unsigned char *)dwEip;
	dest = (void *)WAKE_RECOVER1M_CODE;
	u32 i;
	for (i = 0; i < 0x200; i++)
		dest[i] = src[i];

	__asm__ __volatile__("ljmp $0x0010,%0"	/* 08 error */
			     ::"i"((void *)(WAKE_RECOVER1M_CODE + 0x20)));

	/* Added 0x20 "nop" to make sure the ljmp will not jump then halt. */
	asm volatile ("nop");
	asm volatile ("nop");
	asm volatile ("nop");
	asm volatile ("nop");
	asm volatile ("nop");
	asm volatile ("nop");
	asm volatile ("nop");
	asm volatile ("nop");
	asm volatile ("nop");
	asm volatile ("nop");

	asm volatile ("nop");
	asm volatile ("nop");
	asm volatile ("nop");
	asm volatile ("nop");
	asm volatile ("nop");
	asm volatile ("nop");
	asm volatile ("nop");
	asm volatile ("nop");
	asm volatile ("nop");
	asm volatile ("nop");

	asm volatile ("nop");
	asm volatile ("nop");
	asm volatile ("nop");
	asm volatile ("nop");
	asm volatile ("nop");
	asm volatile ("nop");
	asm volatile ("nop");
	asm volatile ("nop");
	asm volatile ("nop");
	asm volatile ("nop");

	__asm__ volatile (
		/*
		 * Set new esp, maybe ebp should not equal to esp?, due to the
		 * variable in acpi_jump_wake?, anyway, this may be not a big
		 * problem. and I didn't clear the area (ef000+-0x200) to zero.
		 */
		"movl %0, %%ebp\n\t"
		"movl %0, %%esp\n\t"::"a" (WAKE_THUNK16_STACK)
	);

	/*
	 * Only "src" and "dest" use the new stack, and the esp maybe also
	 * used in resumevector.
	 */
#if PAYLOAD_IS_SEABIOS == 1
	/* WAKE_MEM_INFO inited in get_set_top_available_mem in tables.c. */
	src =
	    (unsigned char *)((*(u32 *) WAKE_MEM_INFO) - 64 * 1024 - 0x100000);
	dest = 0;

	/*
	 * If recovered 0-e0000, then when resume, before WinXP turn on the
	 * desktop screen, there is gray background which last 1sec.
	 */
	for (i = 0; i < 0xa0000; i++)
		dest[i] = src[i];

#if 0
	__asm__ volatile (
		"movl %0, %%esi\n\t"
		"movl $0, %%edi\n\t"
		"movl $0xa0000, %%ecx\n\t"
		"shrl $2, %%ecx\n\t"
		"rep movsd\n\t"
		::"a"(src)
	);
#endif
	src = (unsigned char *)((*(u32 *) WAKE_MEM_INFO) - 64 * 1024
			- 0x100000 + 0xc0000);

#if 0
	dest = 0xc0000;
	for (i = 0; i < 0x20000; i++)
	      dest[i] = src[i];

	__asm__ volatile (
		"movl %0, %%esi\n\t"
		"movl $0xc0000, %%edi\n\t"
		"movl $0x20000, %%ecx\n\t"
		"shrl $2, %%ecx\n\t"
		"rep movsd\n\t"
		::"a"(src)
	);
#endif

	src = (unsigned char *)((*(u32 *) WAKE_MEM_INFO) - 64 * 1024
			- 0x100000 + 0xe0000 + WAKE_SPECIAL_SIZE);

	/* dest = 0xf0000; */
	/* for (i = 0; i < 0x10000; i++) */
	/* 	dest[i] = src[i]; */
	__asm__ volatile (
		"movl %0, %%esi\n\t"
		"movl %1, %%edi\n\t"
		"movl %2, %%ecx\n\t"
		"shrl $2, %%ecx\n\t"
		"rep movsd\n\t"::"r" (src),
		"r"(0xe0000 + WAKE_SPECIAL_SIZE),
		"r"(0x10000 - WAKE_SPECIAL_SIZE)
	);

	src = (unsigned char *)((*(u32 *) WAKE_MEM_INFO) - 64 * 1024
			- 0x100000 + 0xf0000);
	/* dest = 0xf0000; */
	/* for (i = 0; i < 0x10000; i++) */
	/* 	dest[i] = src[i]; */
	__asm__ volatile (
		"movl %0, %%esi\n\t"
		"movl $0xf0000, %%edi\n\t"
		"movl $0x10000, %%ecx\n\t"
		"shrl $2, %%ecx\n\t" "rep movsd\n\t"::"a" (src)
	);

	asm volatile ("wbinvd");
#endif
	/* Set up the IDT for real mode. */
	asm volatile ("lidt %0"::"m" (wake_thunk16_Xgt_desc[1]));

	/*
	 * Set up a GDT from which we can load segment descriptors for real
	 * mode. The GDT is not used in real mode; it is just needed here to
	 * prepare the descriptors.
	 */
	asm volatile ("lgdt %0"::"m" (wake_thunk16_Xgt_desc[0]));

	/*
	 * Load the data segment registers, and thus the descriptors ready for
	 * real mode.  The base address of each segment is 0x100, 16 times the
	 * selector value being loaded here.  This is so that the segment
	 * registers don't have to be reloaded after switching to real mode:
	 * the values are consistent for real mode operation already.
	 */
	__asm__ __volatile__(
		"movl $0x0010,%%eax\n"
		"\tmovl %%eax,%%ds\n"
		"\tmovl %%eax,%%es\n"
		"\tmovl %%eax,%%fs\n"
		"\tmovl %%eax,%%gs\n"
		"\tmovl %%eax,%%ss":::"eax"
	);

	/*
	 * Jump to the 16-bit code that we copied earlier. It disables paging
	 * and the cache, switches to real mode, and jumps to the BIOS reset
	 * entry point.
	 */

	__asm__ __volatile__(
		"ljmp $0x0008,%0"::"i"
		((void *)(WAKE_THUNK16_ADDR - sizeof(real_mode_switch) - 100))
	);
}

/* -*- linux-c -*- ------------------------------------------------------- *
 *
 *   Copyright (C) 1991, 1992 Linus Torvalds
 *   Copyright 2007 rPath, Inc. - All Rights Reserved
 *
 *   This file is part of the Linux kernel, and is made available under
 *   the terms of the GNU General Public License version 2.
 *
 * ----------------------------------------------------------------------- */

/*
 * arch/x86/boot/a20.c
 *
 * Enable A20 gate (return -1 on failure)
 */

#define MAX_8042_LOOPS	100000

static int empty_8042(void)
{
	u8 status;
	int loops = MAX_8042_LOOPS;

	while (loops--) {
		udelay(1);

		status = inb(0x64);
		if (status & 1) {
			/* Read and discard input data */
			udelay(1);
			(void)inb(0x60);
		} else if (!(status & 2)) {
			/* Buffers empty, finished! */
			return 0;
		}
	}

	return -1;
}

/* Returns nonzero if the A20 line is enabled.  The memory address
   used as a test is the int $0x80 vector, which should be safe. */

#define A20_TEST_ADDR	(4*0x80)
#define A20_TEST_SHORT  32
#define A20_TEST_LONG	2097152	/* 2^21 */

static int a20_test(int loops)
{
	int ok = 0;
	int saved, ctr;

	saved = ctr = *((u32 *) A20_TEST_ADDR);

	while (loops--) {

		*((u32 *) A20_TEST_ADDR) = ++ctr;

		udelay(1);	/* Serialize and make delay constant */

		ok = *((u32 *) A20_TEST_ADDR + 0xffff0 + 0x10) ^ ctr;
		if (ok)
			break;
	}

	*((u32 *) A20_TEST_ADDR) = saved;
	return ok;
}

/* Quick test to see if A20 is already enabled */
static int a20_test_short(void)
{
	return a20_test(A20_TEST_SHORT);
}

/* Longer test that actually waits for A20 to come on line; this
   is useful when dealing with the KBC or other slow external circuitry. */
static int a20_test_long(void)
{
	return a20_test(A20_TEST_LONG);
}

static void enable_a20_kbc(void)
{
	empty_8042();

	outb(0xd1, 0x64);	/* Command write */
	empty_8042();

	outb(0xdf, 0x60);	/* A20 on */
	empty_8042();
}

static void enable_a20_fast(void)
{
	u8 port_a;

	port_a = inb(0x92);	/* Configuration port A */
	port_a |= 0x02;		/* Enable A20 */
	port_a &= ~0x01;	/* Do not reset machine */
	outb(port_a, 0x92);
}

/*
 * Actual routine to enable A20; return 0 on ok, -1 on failure
 */

#define A20_ENABLE_LOOPS 255	/* Number of times to try */

int enable_a20(void)
{
	int loops = A20_ENABLE_LOOPS;

	while (loops--) {
		/* First, check to see if A20 is already enabled
		   (legacy free, etc.) */
		if (a20_test_short())
			return 0;

		/* Try enabling A20 through the keyboard controller */
		empty_8042();

		// if (a20_test_short())
		// 	return 0; /* BIOS worked, but with delayed reaction */

		enable_a20_kbc();
		if (a20_test_long())
			return 0;

		/* Finally, try enabling the "fast A20 gate" */
		enable_a20_fast();
		if (a20_test_long())
			return 0;
	}

	return -1;
}
