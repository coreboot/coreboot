#include <console/console.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <device/pci_ops.h>
#undef __KERNEL__
#include <arch/io.h>
#include <string.h>
#include <cpu/amd/lxdef.h>
#include <cpu/amd/vr.h>

// andrei: use the /lib copy of nrv2b
#include "../lib/nrv2b.c"

#define VSA2_BUFFER			0x60000
#define VSA2_ENTRY_POINT	0x60020

/* vsmsetup.c derived from vgabios.c. Derived from: */

/*------------------------------------------------------------ -*- C -*-
 *  2 Kernel Monte a.k.a. Linux loading Linux on x86
 *
 *  Erik Arjan Hendriks <hendriks@lanl.gov>
 *
 *  This version is a derivative of the original two kernel monte
 *  which is (C) 2000 Scyld.
 *
 *  Copyright (C) 2000 Scyld Computing Corporation
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
 * Portions related to the alpha architecture are:
 *
 *  Copyright(C) 2001 University of California.  LA-CC Number 01-67.
 *  This software has been authored by an employee or employees of the
 *  University of California, operator of the Los Alamos National
 *  Laboratory under Contract No.  W-7405-ENG-36 with the U.S.
 *  Department of Energy.  The U.S. Government has rights to use,
 *  reproduce, and distribute this software. If the software is
 *  modified to produce derivative works, such modified software should
 *  be clearly marked, so as not to confuse it with the version
 *  available from LANL.
 *
 *  This software may be used and distributed according to the terms
 *  of the GNU General Public License, incorporated herein by
 *  reference to http://www.gnu.org/licenses/gpl.html.
 *
 *  This software is provided by the author(s) "as is" and any express
 *  or implied warranties, including, but not limited to, the implied
 *  warranties of merchantability and fitness for a particular purpose
 *  are disclaimed.  In no event shall the author(s) be liable for any
 *  direct, indirect, incidental, special, exemplary, or consequential
 *  damages (including, but not limited to, procurement of substitute
 *  goods or services; loss of use, data, or profits; or business
 *  interruption) however caused and on any theory of liability,
 *  whether in contract, strict liability, or tort (including
 *  negligence or otherwise) arising in any way out of the use of this
 *  software, even if advised of the possibility of such damage.
 *
 *  $Id: vsmsetup.c,v 1.8 2006/09/08 12:47:57 andrei Exp $
 *
 * Copyright (C) 2007 Advanced Micro Devices, Inc.
 *
 *--------------------------------------------------------------------*/

/* Modified to be a self sufficient plug in so that it can be used
   without reliance on other parts of core coreboot
   (C) 2005 Nick.Barker9@btinternet.com

  Used initially for epia-m where there are problems getting the bios
  emulator to successfully run this bios.
*/

/* Declare a temporary global descriptor table - necessary because the
   Core part of the bios no longer sets up any 16 bit segments */
__asm__(
       /* pointer to original gdt */
       "gdtarg:			\n"
       "	.word	gdt_limit	\n"
       "	.long	gdt	       	\n"
       /* compute the table limit */
       "__mygdt_limit = __mygdt_end - __mygdt - 1	\n"
       "__mygdtaddr:			\n"
       "	.word	__mygdt_limit  	\n"
       "	.long	__mygdt	       	\n"
       "__mygdt: 		       	\n"
       /* selgdt 0, unused */
       "	.word	0x0000, 0x0000	\n"
       "	.byte	0x00, 0x00, 0x00, 0x00	\n"
       /* selgdt 8, unused */
       "	.word	0x0000, 0x0000	       	\n"
       "	.byte	0x00, 0x00, 0x00, 0x00	\n"
       /* selgdt 0x10, flat code segment */
       "	.word	0xffff, 0x0000	       	\n"
       "	.byte	0x00, 0x9b, 0xcf, 0x00	\n"
       /* selgdt 0x18, flat data segment */
       "	.word	0xffff, 0x0000	       	\n"
       "	.byte	0x00, 0x93, 0xcf, 0x00	\n"
       /* selgdt 0x20, unused */
       "	.word	0x0000, 0x0000	       	\n"
       "	.byte	0x00, 0x00, 0x00, 0x00	\n"
       /* selgdt 0x28 16-bit 64k code at 0x00000000 */
       "	.word	0xffff, 0x0000	       	\n"
       "	.byte	0, 0x9a, 0, 0	       	\n"
       /* selgdt 0x30 16-bit 64k data at 0x00000000 */
       "	.word	0xffff, 0x0000	       	\n"
       "	.byte	0, 0x92, 0, 0	       	\n"
       "__mygdt_end:				\n"
);

/* Declare a pointer to where our idt is going to be i.e. at mem zero */
__asm__(
	"__myidt:		\n"
	/* 16-bit limit */
	"	.word 1023	\n"
	/* 24-bit base */
	"	.long 0		\n" 
	"	.word 0		\n"
);

/* The address arguments to this function are PHYSICAL ADDRESSES */
static void real_mode_switch_call_vsm(unsigned long smm, unsigned long sysm)
{
	uint16_t entryHi = (VSA2_ENTRY_POINT & 0xffff0000) >> 4;
	uint16_t entryLo = (VSA2_ENTRY_POINT & 0xffff);

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
#if 0
		/* I'm not happy about that pushal followed by esp-relative
		 * references. Just do hard-codes for now
		 */
		"	movl    8(%%ebp), %%ecx	\n"
		"	movl    12(%%ebp), %%edx	\n"
#endif
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

uint32_t VSA_vrRead(uint16_t classIndex)
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

uint32_t VSA_msrRead(uint32_t msrAddr)
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
	device_t dev;
	unsigned long busdevfn;
	unsigned int rom = 0;
	unsigned char *buf;
	unsigned int size = SMM_SIZE * 1024;
	int i;
	unsigned long ilen, olen;

	printk_err("do_vsmbios\n");
	/* clear vsm bios data area */
	for (i = 0x400; i < 0x500; i++) {
		*(volatile unsigned char *)i = 0;
	}

	/* declare rom address here - keep any config data out of the way
	 * of core LXB stuff */

	/* this is the base of rom on the LX at present. At some point, this has to be
	 * much better parameterized
	 */

	//VSA is cat onto the end after LB builds
	rom = ((unsigned long)0) - (ROM_SIZE + 36 * 1024);
	buf = (unsigned char *)VSA2_BUFFER;
	olen = unrv2b((uint8_t *) rom, buf, &ilen);
	printk_debug("buf ilen %d olen%d\n", ilen, olen);
	printk_debug("buf %p *buf %d buf[256k] %d\n",
		     buf, buf[0], buf[SMM_SIZE * 1024]);
	printk_debug("buf[0x20] signature is %x:%x:%x:%x\n",
		     buf[0x20], buf[0x21], buf[0x22], buf[0x23]);
	/* check for post code at start of vsainit.bin. If you don't see it,
	   don't bother. */
	if ((buf[0x20] != 0xb0) || (buf[0x21] != 0x10) ||
	    (buf[0x22] != 0xe6) || (buf[0x23] != 0x80)) {
		printk_err("do_vsmbios: no vsainit.bin signature, skipping!\n");
		return;
	}

	/* ecx gets smm, edx gets sysm */
	printk_err("Call real_mode_switch_call_vsm\n");
	real_mode_switch_call_vsm(MSR_GLIU0_SMM, MSR_GLIU0_SYSMEM);

	/* restart timer 1 */
	outb(0x56, 0x43);
	outb(0x12, 0x41);

	// check that VSA is running OK
	if (VSA_vrRead(SIGNATURE) == VSA2_SIGNATURE)
		printk_debug("do_vsmbios: VSA2 VR signature verified\n");
	else
		printk_err
		    ("do_vsmbios: VSA2 VR signature not valid, install failed!\n");
}

// we had hoped to avoid this.
// this is a stub IDT only. It's main purpose is to ignore calls
// to the BIOS.
// no longer. Dammit. We have to respond to these.
struct realidt {
	unsigned short offset, cs;
};

// from a handy writeup that andrey found.

// handler.
// There are some assumptions we can make here.
// First, the Top Of Stack (TOS) is located on the top of page zero.
// we can share this stack between real and protected mode.
// that simplifies a lot of things ...
// we'll just push all the registers on the stack as longwords,
// and pop to protected mode.
// second, since this only ever runs as part of coreboot,
// we know all the segment register values -- so we don't save any.
// keep the handler that calls things small. It can do a call to
// more complex code in coreboot itself. This helps a lot as we don't
// have to do address fixup in this little stub, and calls are absolute
// so the handler is relocatable.
void handler(void)
{
	__asm__ __volatile__(
		"	.code16		\n"
		"idthandle:		\n"
		"	pushal		\n"
		"	movb 	$0, %al	\n"
		"	ljmp 	$0, $callbiosint16\n"
		"end_idthandle:		\n"
		"	.code32		\n"
	);
}

void debughandler(void)
{
	__asm__ __volatile__(
		"	.code16		\n"
		"debughandle:		\n"
		"	pushw	%cx	\n"
		"	movw	$250, %cx \n"
		"dbh1:			\n"
		"	loop	dbh1	\n"
		"	popw	%cx	\n"
		"	iret		\n"
		"end_debughandle:	\n" 
		".code32		\n"
	);
}

// Calling conventions. The first C function is called with this stuff
// on the stack. They look like value parameters, but note that if you
// modify them they will go back to the INTx function modified.
// the C function will call the biosint function with these as
// REFERENCE parameters. In this way, we can easily get
// returns back to the INTx caller (i.e. vgabios)
void callbiosint(void)
{
	__asm__ __volatile__(
		"	.code16		\n"
		"callbiosint16:		\n"
		"	push	%ds	\n"
		"	push	%es	\n"
		"	push	%fs	\n"
		"	push	%gs	\n"
		// clean up the int #. To save space we put it in the lower
		// byte. But the top 24 bits are junk.
		"	andl	$0xff, %eax\n"
		// this push does two things:
		// - put the INT # on the stack as a parameter
		// - provides us with a temp for the %cr0 mods.
		"	pushl	%eax	\n" 
		"	movb	$0xbb, %al\n" 
		"	outb	%al, $0x80\n" 
		"	movl    %cr0, %eax\n" 
		"	orl	$0x00000001, %eax\n"	/* PE = 1 */
		"	movl	%eax, %cr0\n"
		/* Now that we are in protected mode jump to a 32 bit code segment. */
		"	data32  ljmp    $0x10, $biosprotect\n"
		"biosprotect:		\n"
		"	.code32		\n"
		"	movw	$0x18, %ax\n"
		"	mov	%ax, %ds\n"
		"	mov	%ax, %es\n"
		"	mov	%ax, %fs\n"
		"	mov	%ax, %gs\n"
		"	mov	%ax, %ss\n"
		"	lidt	idtarg  \n"
		"	call	biosint	\n"
		// back to real mode ...
		"	ljmp	$0x28, $__rms_16bit2\n"
		"__rms_16bit2:		\n"
		"	.code16		\n"
		/* 16 bit code from here on... */
		/* Load the segment registers w/ properly configured segment
		 * descriptors.  They will retain these configurations (limits,
		 * writability, etc.) once protected mode is turned off. */
		"	mov	$0x30, %ax\n"
		"	mov	%ax, %ds\n"
		"	mov	%ax, %es\n"
		"	mov	%ax, %fs\n"
		"	mov	%ax, %gs\n"
		"	mov	%ax, %ss\n"
		/* Turn off protection (bit 0 in CR0) */
		"	movl	%cr0, %eax		\n"
		"	andl	$0xFFFFFFFE, %eax	\n"
		"	movl	%eax, %cr0		\n"
		/* Now really going into real mode */
		"	ljmp $0,  $__rms_real2	\n"
		"__rms_real2:			\n"
		/* Setup a stack
		 * FixME: where is esp? */
		/* no need for a fix here. The esp is shared from 32-bit and 16-bit mode.
		 * you have to hack on the ss, but the esp remains the same across
		 * modes.
		 */
		"	mov	$0x0, %ax       \n"
		"	mov	%ax, %ss	\n"
		/* debugging for RGM */
		"	mov	$0x11, %al	\n"
		"	outb	%al, $0x80	\n"
		/* Load our 16 bit idt */
		"	xor	%ax, %ax	\n"
		"	mov	%ax, %ds	\n"
		"	lidt	__myidt		\n"
		/* Dump zeros in the other segregs */
		"	mov	%ax, %es	\n"
		"	mov	%ax, %fs	\n"
		"	mov	%ax, %gs	\n"
		"	mov	$0x40, %ax	\n"
		"	mov	%ax, %ds	\n"
		/* pop the INT # that you pushed earlier */
		"	popl	%eax		\n"
		"	pop	%gs		\n"
		"	pop	%fs		\n"
		"	pop	%es		\n"
		"	pop	%ds		\n"
		"	popal			\n"
		"	iret			\n"
		"	.code32			\n"
	);
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

int biosint(unsigned long intnumber,
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

	printk_debug("biosint: INT# 0x%lx\n", intnumber);
	printk_debug("biosint: eax 0x%lx ebx 0x%lx ecx 0x%lx edx 0x%lx\n",
		     eax, ebx, ecx, edx);
	printk_debug("biosint: ebp 0x%lx esp 0x%lx edi 0x%lx esi 0x%lx\n",
		     ebp, esp, edi, esi);
	printk_debug("biosint:  ip 0x%x   cs 0x%x  flags 0x%x\n",
		     ip, cs, flags);
	printk_debug("biosint: gs 0x%x fs 0x%x ds 0x%x es 0x%x\n",
		     gsfs >> 16, gsfs & 0xffff, dses >> 16, dses & 0xffff);

	// cases in a good compiler are just as good as your own tables.
	switch (intnumber) {
	case 0 ... 15:
		// These are not BIOS service, but the CPU-generated exceptions
		printk_info("biosint: Oops, exception %u\n", intnumber);
		if (esp < 0x1000) {
			printk_debug("Stack contents: ");
			while (esp < 0x1000) {
				printk_debug("0x%04x ", *(unsigned short *)esp);
				esp += 2;
			}
			printk_debug("\n");
		}
		printk_debug("biosint: Bailing out ... not now\n");
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
		printk_info("BIOSINT: Unsupport int #0x%x\n", intnumber);
		break;
	}
	if (ret)
		flags |= 1;	// carry flags
	else
		flags &= ~1;
	stackflags = flags;
	return ret;
}

void setup_realmode_idt(void)
{
	extern unsigned char idthandle, end_idthandle;
	extern unsigned char debughandle, end_debughandle;

	int i;
	struct realidt *idts = (struct realidt *)0;
	int codesize = &end_idthandle - &idthandle;
	unsigned char *intbyte, *codeptr;

	// for each int, we create a customized little handler
	// that just pushes %ax, puts the int # in %al,
	// then calls the common interrupt handler.
	// this necessitated because intel didn't know much about
	// architecture when they did the 8086 (it shows)
	// (hmm do they know anymore even now :-)
	// obviously you can see I don't really care about memory
	// efficiency. If I did I would probe back through the stack
	// and get it that way. But that's really disgusting.
	for (i = 0; i < 256; i++) {
		idts[i].cs = 0;
		codeptr = (unsigned char *)4096 + i * codesize;
		idts[i].offset = (unsigned)codeptr;
		memcpy(codeptr, &idthandle, codesize);
		intbyte = codeptr + 3;
		*intbyte = i;
	}

	// fixed entry points

	// VGA BIOSes tend to hardcode f000:f065 as the previous handler of
	// int10.
	// calling convention here is the same as INTs, we can reuse
	// the int entry code.
	codeptr = (unsigned char *)0xff065;
	memcpy(codeptr, &idthandle, codesize);
	intbyte = codeptr + 3;
	*intbyte = 0x42;	/* int42 is the relocated int10 */

	/* debug handler - useful to set a programmable delay between instructions if the
	   TF bit is set upon call to real mode */
	idts[1].cs = 0;
	idts[1].offset = 16384;
	memcpy((void *)16384, &debughandle, &end_debughandle - &debughandle);
}

enum {
	CHECK = 0xb001,
	FINDDEV = 0xb102,
	READCONFBYTE = 0xb108,
	READCONFWORD = 0xb109,
	READCONFDWORD = 0xb10a,
	WRITECONFBYTE = 0xb10b,
	WRITECONFWORD = 0xb10c,
	WRITECONFDWORD = 0xb10d
};

// errors go in AH. Just set these up so that word assigns
// will work. KISS.
enum {
	PCIBIOS_NODEV = 0x8600,
	PCIBIOS_BADREG = 0x8700
};

int
pcibios(unsigned long *pedi, unsigned long *pesi, unsigned long *pebp,
	unsigned long *pesp, unsigned long *pebx, unsigned long *pedx,
	unsigned long *pecx, unsigned long *peax, unsigned long *pflags)
{
	unsigned long edi = *pedi;
	unsigned long esi = *pesi;
	unsigned long ebp = *pebp;
	unsigned long esp = *pesp;
	unsigned long ebx = *pebx;
	unsigned long edx = *pedx;
	unsigned long ecx = *pecx;
	unsigned long eax = *peax;
	unsigned long flags = *pflags;
	unsigned short func = (unsigned short)eax;
	int retval = 0;
	unsigned short devid, vendorid, devfn;
	short devindex;		/* Use short to get rid of gabage in upper half of 32-bit register */
	unsigned char bus;
	device_t dev;

	switch (func) {
	case CHECK:
		*pedx = 0x4350;
		*pecx = 0x2049;
		retval = 0;
		break;
	case FINDDEV:
		{
			devid = *pecx;
			vendorid = *pedx;
			devindex = *pesi;
			dev = 0;
			while ((dev = dev_find_device(vendorid, devid, dev))) {
				if (devindex <= 0)
					break;
				devindex--;
			}
			if (dev) {
				unsigned short busdevfn;
				*peax = 0;
				// busnum is an unsigned char;
				// devfn is an int, so we mask it off.
				busdevfn = (dev->bus->secondary << 8)
				    | (dev->path.pci.devfn & 0xff);
				printk_debug("0x%x: return 0x%x\n", func,
					     busdevfn);
				*pebx = busdevfn;
				retval = 0;
			} else {
				*peax = PCIBIOS_NODEV;
				retval = -1;
			}
		}
		break;
	case READCONFDWORD:
	case READCONFWORD:
	case READCONFBYTE:
	case WRITECONFDWORD:
	case WRITECONFWORD:
	case WRITECONFBYTE:
		{
			unsigned long dword;
			unsigned short word;
			unsigned char byte;
			unsigned char reg;

			devfn = *pebx & 0xff;
			bus = *pebx >> 8;
			reg = *pedi;
			dev = dev_find_slot(bus, devfn);
			if (!dev) {
				printk_debug
				    ("0x%x: BAD DEVICE bus %d devfn 0x%x\n",
				     func, bus, devfn);
				// idiots. the pcibios guys assumed you'd never pass a bad bus/devfn!
				*peax = PCIBIOS_BADREG;
				retval = -1;
			}
			switch (func) {
			case READCONFBYTE:
				byte = pci_read_config8(dev, reg);
				*pecx = byte;
				break;
			case READCONFWORD:
				word = pci_read_config16(dev, reg);
				*pecx = word;
				break;
			case READCONFDWORD:
				dword = pci_read_config32(dev, reg);
				*pecx = dword;
				break;
			case WRITECONFBYTE:
				byte = *pecx;
				pci_write_config8(dev, reg, byte);
				break;
			case WRITECONFWORD:
				word = *pecx;
				pci_write_config16(dev, reg, word);
				break;
			case WRITECONFDWORD:
				dword = *pecx;
				pci_write_config32(dev, reg, dword);
				break;
			}

			if (retval)
				retval = PCIBIOS_BADREG;
			printk_debug
			    ("0x%x: bus %d devfn 0x%x reg 0x%x val 0x%lx\n",
			     func, bus, devfn, reg, *pecx);
			*peax = 0;
			retval = 0;
		}
		break;
	default:
		printk_err("UNSUPPORTED PCIBIOS FUNCTION 0x%x\n", func);
		break;
	}

	return retval;
}

int handleint21(unsigned long *edi, unsigned long *esi, unsigned long *ebp,
		unsigned long *esp, unsigned long *ebx, unsigned long *edx,
		unsigned long *ecx, unsigned long *eax, unsigned long *flags)
{
	int res = -1;
	printk_debug("handleint21, eax 0x%x\n", *eax);
	switch (*eax & 0xffff) {
	case 0x5f19:
		break;
	case 0x5f18:
		*eax = 0x5f;
		*ebx = 0x545;	// MCLK = 133, 32M frame buffer, 256 M main memory
		*ecx = 0x060;
		res = 0;
		break;
	case 0x5f00:
		*eax = 0x8600;
		break;
	case 0x5f01:
		*eax = 0x5f;
		*ecx = (*ecx & 0xffffff00) | 2;	// panel type =  2 = 1024 * 768
		res = 0;
		break;
	case 0x5f02:
		*eax = 0x5f;
		*ebx = (*ebx & 0xffff0000) | 2;
		*ecx = (*ecx & 0xffff0000) | 0x401;	// PAL + crt only
		*edx = (*edx & 0xffff0000) | 0;	// TV Layout - default
		res = 0;
		break;
	case 0x5f0f:
		*eax = 0x860f;
		break;
	case 0xBEA7:
		*eax = 66;
		break;
	case 0xBEA4:
		*eax = 500;
		break;
	}
	return res;
}
