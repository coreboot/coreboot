/*
 *  Copyright (C) 2000 Erik Arjan Hendriks
 *  Copyright (C) 2000 Scyld Computing Corporation
 *  Copyright (C) 2001 University of California.  LA-CC Number 01-67.
 *  Copyright (C) 2005 Nick.Barker9@btinternet.com
 *  Copyright (C) 2007 coresystems GmbH
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 *
 * LA-CC is the Los Alamos Control and Compliance Number, see also:
 * http://supply.lanl.gov/property/customs/eximguide/default.shtml
 */
#include <types.h>
#include <console.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <msr.h>
#include <amd_geodelx.h>
#include <lar.h>

#define VSA2_BUFFER			0x60000
#define VSA2_ENTRY_POINT	0x60020


/* The address arguments to this function are PHYSICAL ADDRESSES */
static void real_mode_switch_call_vsm(unsigned long smm, unsigned long sysm)
{
	u16 entryHi = (VSA2_ENTRY_POINT & 0xffff0000) >> 4;
	u16 entryLo = (VSA2_ENTRY_POINT & 0xffff);

	__asm__ __volatile__(
		// paranoia -- does ecx get saved? not sure. This is
		// the easiest safe thing to do.
		"	pushal			\n"
		/* save the stack */
		"	mov 	%%esp, __stack	\n"
		"	jmp 	1f		\n"
		"__stack: .long 0		\n" 
		"1:\n"
		/* get devfn into %%ecx */
		"	movl    %%esp, %%ebp	\n"
		/* Get the smm and sysm args into ecx and edx */
		"	movl    %0, %%ecx	\n"
		"	movl    %1, %%edx	\n"
		/* load 'our' gdt */
		"	lgdt	%%cs:__mygdtaddr	\n"
		/*  This configures CS properly for real mode. */
		"	ljmp	$0x28, $__rms_16bit\n"
		"__rms_16bit:		  	\n"
		"	.code16			\n"
		/* 16 bit code from here on... */
		/* Load the segment registers w/ properly configured segment
		 * descriptors.  They will retain these configurations (limits,
		 * writability, etc.) once protected mode is turned off. */
		"	mov	$0x30, %%ax	\n"
		"	mov	%%ax, %%ds       	\n"
		"	mov	%%ax, %%es       	\n"
		"	mov	%%ax, %%fs       	\n"
		"	mov	%%ax, %%gs       	\n"
		"	mov	%%ax, %%ss       	\n"
		/* Turn off protection (bit 0 in CR0) */
		"	movl	%%cr0, %%eax	\n"
		"	andl	$0xFFFFFFFE, %%eax \n"
		"	movl	%%eax, %%cr0	\n"
		/* Now really going into real mode */
		"	ljmp	$0,  $__rms_real\n"
		"__rms_real:			\n"

		/* put the stack at the end of page zero.
		 * that way we can easily share it between real and protected,
		 * since the 16-bit ESP at segment 0 will work for any case. 
		 */

		/* Setup a stack */
		"	mov	$0x0, %%ax	\n"
		"	mov	%%ax, %%ss	\n"
		"	movl	$0x1000, %%eax	\n"
		"	movl	%%eax, %%esp	\n"
		/* Load our 16 it idt */
		"	xor	%%ax, %%ax	\n"
		"	mov	%%ax, %%ds	\n"
		"	lidt	__myidt		\n"
		/* Dump zeros in the other segregs */
		"	mov	%%ax, %%es     	\n"
		/* FixMe: Big real mode for gs, fs? */
		"	mov	%%ax, %%fs     	\n"
		"	mov	%%ax, %%gs     	\n"
		"	mov	$0x40, %%ax	\n"
		"	mov	%%ax, %%ds	\n"
		//"     mov     %%cx, %%ax      \n"
		"	movl    %0, %%ecx	\n"
		"	movl    %1, %%edx	\n"
		/* call the VSA2 entry point address */
		"	lcall	%2, %3\n"
		/* if we got here, just about done.
		 * Need to get back to protected mode */
		"	movl	%%cr0, %%eax	\n" 
		"	orl	$0x0000001, %%eax\n"	/* PE = 1 */
		"	movl	%%eax, %%cr0	\n"
		/* Now that we are in protected mode jump to a 32 bit code segment. */
		"	data32	ljmp	$0x10, $vsmrestart\n"
		"vsmrestart:\n"
		"	.code32\n"
		"	movw	$0x18, %%ax    	\n"
		"	mov	%%ax, %%ds     	\n"
		"	mov	%%ax, %%es	\n"
		"	mov	%%ax, %%fs	\n"
		"	mov	%%ax, %%gs	\n"
		"	mov	%%ax, %%ss	\n"
		/* restore proper gdt and idt */
		"	lgdt	%%cs:gdtarg	\n"
		"	lidt	idtarg		\n"
		".globl vsm_exit		\n"
		"vsm_exit:			\n"
		"	mov	__stack, %%esp	\n"
		"	popal			\n"::
		"g" (smm), "g"(sysm), "g"(entryHi), "g"(entryLo)
	);
}

__asm__(".text\n" "real_mode_switch_end:\n");
extern char real_mode_switch_end[];

// andrei: some VSA virtual register helpers: raw read and MSR read

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
	struct mem_file archive;
	struct mem_file file;

	printk(BIOS_ERR, "do_vsmbios\n");
	/* clear vsm bios data area */
	for (i = 0x400; i < 0x500; i++) {
		*(volatile unsigned char *)i = 0;
	}
	init_archive(&archive);

	if (find_file(&archive, "blob/vsa", &file)){
		printk(BIOS_ERR, "NO VSA found!\n");
		return;
	}

	if (process_file(&file, (void *)VSA2_BUFFER)) {
		printk(BIOS_ERR, "Processing /blob/vsa failed\n");
		return;
	}

	buf = (unsigned char *)VSA2_BUFFER;
	printk(BIOS_DEBUG, "buf ilen %d real len %uld\n", file.len, file.reallen);
	printk(BIOS_DEBUG, "buf %p *buf %d buf[256k] %d\n",
		     buf, buf[0], buf[SMM_SIZE * 1024]);
	printk(BIOS_DEBUG, "buf[0x20] signature is %x:%x:%x:%x\n",
		     buf[0x20], buf[0x21], buf[0x22], buf[0x23]);
	/* check for post code at start of vsainit.bin. If you don't see it,
	   don't bother. */
	if ((buf[0x20] != 0xb0) || (buf[0x21] != 0x10) ||
	    (buf[0x22] != 0xe6) || (buf[0x23] != 0x80)) {
		printk(BIOS_ERR, "do_vsmbios: no vsainit.bin signature, skipping!\n");
		return;
	}

	/* ecx gets smm, edx gets sysm */
	printk(BIOS_ERR, "Call real_mode_switch_call_vsm\n");
	real_mode_switch_call_vsm(MSR_GLIU0_SMM, MSR_GLIU0_SYSMEM);

	/* restart timer 1 */
	outb(0x56, 0x43);
	outb(0x12, 0x41);

	// check that VSA is running OK
	if (VSA_vrRead(SIGNATURE) == VSA2_SIGNATURE)
		printk(BIOS_DEBUG, "do_vsmbios: VSA2 VR signature verified\n");
	else
		printk(BIOS_ERR, 
		       "do_vsmbios: VSA2 VR signature not valid, install failed!\n");
}

enum {
	PCIBIOS = 0x1a,
	MEMSIZE = 0x12
};

int pcibios(unsigned long *pedi, unsigned long *pesi, unsigned long *pebp,
	    unsigned long *pesp, unsigned long *pebx, unsigned long *pedx,
	    unsigned long *pecx, unsigned long *peax, unsigned long *pflags);

int handleint21(unsigned long *pedi, unsigned long *pesi, unsigned long *pebp,
		unsigned long *pesp, unsigned long *pebx, unsigned long *pedx,
		unsigned long *pecx, unsigned long *peax,
		unsigned long *pflags);

/* see the vga_exit() call. until we clean this up this function will remain here */
static int biosint(unsigned long intnumber,
	    unsigned long gsfs, unsigned long dses,
	    unsigned long edi, unsigned long esi,
	    unsigned long ebp, unsigned long esp,
	    unsigned long ebx, unsigned long edx,
	    unsigned long ecx, unsigned long eax,
	    unsigned long cs_ip, unsigned short stackflags)
{
	unsigned long ip;
	unsigned long cs;
	unsigned long flags;
	int ret = -1;

	ip = cs_ip & 0xffff;
	cs = cs_ip >> 16;
	flags = stackflags;

	printk(BIOS_DEBUG, "biosint: INT# 0x%lx\n", intnumber);
	printk(BIOS_DEBUG, "biosint: eax 0x%lx ebx 0x%lx ecx 0x%lx edx 0x%lx\n",
		     eax, ebx, ecx, edx);
	printk(BIOS_DEBUG, "biosint: ebp 0x%lx esp 0x%lx edi 0x%lx esi 0x%lx\n",
		     ebp, esp, edi, esi);
	printk(BIOS_DEBUG, "biosint:  ip 0x%lx   cs 0x%lx  flags 0x%lx\n",
		     ip, cs, flags);
	printk(BIOS_DEBUG, "biosint: gs 0x%lx fs 0x%lx ds 0x%lx es 0x%lx\n",
		     gsfs >> 16, gsfs & 0xffff, dses >> 16, dses & 0xffff);

	// cases in a good compiler are just as good as your own tables.
	switch (intnumber) {
	case 0 ... 15:
		// These are not BIOS service, but the CPU-generated exceptions
		printk(BIOS_INFO, "biosint: Oops, exception %lu\n", intnumber);
		if (esp < 0x1000) {
			printk(BIOS_DEBUG, "Stack contents: ");
			while (esp < 0x1000) {
				printk(BIOS_DEBUG, "0x%04x ", *(unsigned short *)esp);
				esp += 2;
			}
			printk(BIOS_DEBUG, "\n");
		}
		printk(BIOS_DEBUG, "biosint: Bailing out ... not now\n");
		// "longjmp"
		//vga_exit();
		break;

	case PCIBIOS:
		ret = pcibios(&edi, &esi, &ebp, &esp,
			      &ebx, &edx, &ecx, &eax, &flags);
		break;
	case MEMSIZE:
		// who cares.
		eax = 128 * 1024;
		ret = 0;
		break;
	case 0x15:
		ret = handleint21(&edi, &esi, &ebp, &esp,
				  &ebx, &edx, &ecx, &eax, &flags);
		break;
	default:
	  printk(BIOS_INFO, "BIOSINT: Unsupport int #0x%lx\n", intnumber);
		break;
	}
	if (ret)
		flags |= 1;	// carry flags
	else
		flags &= ~1;
	stackflags = flags;
	return ret;
}

