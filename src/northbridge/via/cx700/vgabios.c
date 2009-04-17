/*
 * This file is part of the coreboot project.
 *
 * (C) 2007-2009 coresystems GmbH
 * (See further copyright notices below)
 *
 * NOTE: This file is supposed to go away once the generic vm86 handler
 * in util/x86emu is able to handle intXX hooks like yabel does.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
 */

#include <console/console.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <device/pci_ops.h>
#undef __KERNEL__
#include <arch/io.h>
#include <string.h>

void write_protect_vgabios(void);

/* vgabios.c. Derived from: */

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
 *--------------------------------------------------------------------*/

/* Modified to be a self sufficient plug in so that it can be used 
   without reliance on other parts of core Linuxbios 
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
	       "__mygdt_end:				\n");

/* Declare a pointer to where our idt is going to be i.e. at mem zero */
__asm__("__myidt:		\n"
	/* 16-bit limit */
	"	.word 1023	\n"
	/* 24-bit base */
	"	.long 0		\n" "	.word 0		\n");

/* The address arguments to this function are PHYSICAL ADDRESSES */
static void real_mode_switch_call_vga(unsigned long devfn)
{
	__asm__ __volatile__(
				    // paranoia -- does ecx get saved? not sure. This is 
				    // the easiest safe thing to do.
				    "	pushal			\n"
				    /* save the stack */
				    "	mov 	%esp, __stack	\n"
				    "	jmp 	1f		\n"
				    "__stack: .long 0		\n" "1:\n"
				    /* get devfn into %ecx */
				    "	movl    %esp, %ebp	\n"
				    "	movl    8(%ebp), %ecx	\n"
				    /* load 'our' gdt */
				    "	lgdt	%cs:__mygdtaddr	\n"
				    /*  This configures CS properly for real mode. */
				    "	ljmp	$0x28, $__rms_16bit\n"
				    "__rms_16bit:		  	\n"
				    "	.code16			\n"
				    /* 16 bit code from here on... */
				    /* Load the segment registers w/ properly configured segment
				     * descriptors.  They will retain these configurations (limits,
				     * writability, etc.) once protected mode is turned off. */
				    "	mov	$0x30, %ax	\n"
				    "	mov	%ax, %ds       	\n"
				    "	mov	%ax, %es       	\n"
				    "	mov	%ax, %fs       	\n"
				    "	mov	%ax, %gs       	\n"
				    "	mov	%ax, %ss       	\n"
				    /* Turn off protection (bit 0 in CR0) */
				    "	movl	%cr0, %eax	\n"
				    "	andl	$0xFFFFFFFE, %eax \n"
				    "	movl	%eax, %cr0	\n"
				    /* Now really going into real mode */
				    "	ljmp	$0,  $__rms_real\n"
				    "__rms_real:			\n"
				    /* put the stack at the end of page zero. 
				     * that way we can easily share it between real and protected, 
				     * since the 16-bit ESP at segment 0 will work for any case. 
				     */
				    /* Setup a stack */
				    "	mov	$0x0, %ax	\n"
				    "	mov	%ax, %ss	\n"
				    "	movl	$0x1000, %eax	\n"
				    "	movl	%eax, %esp	\n"
				    /* Load our 16 it idt */
				    "	xor	%ax, %ax	\n"
				    "	mov	%ax, %ds	\n"
				    "	lidt	__myidt		\n"
				    /* Dump zeros in the other segregs */
				    "	mov	%ax, %es       	\n"
				    "	mov	%ax, %fs       	\n"
				    "	mov	%ax, %gs       	\n"
				    "	mov	$0x40, %ax	\n"
				    "	mov	%ax, %ds	\n"
				    "	mov	%cx, %ax	\n"
				    /* run VGA BIOS at 0xc000:0003 */
				    "	lcall	$0xc000, $0x0003\n"
				    /* if we got here, just about done. 
				     * Need to get back to protected mode */
				    "	movl	%cr0, %eax	\n" "	orl	$0x0000001, %eax\n"	/* PE = 1 */
				    "	movl	%eax, %cr0	\n"
				    /* Now that we are in protected mode jump to a 32 bit code segment. */
				    "	data32	ljmp	$0x10, $vgarestart\n"
				    "vgarestart:\n"
				    "	.code32\n"
				    "	movw	$0x18, %ax     	\n"
				    "	mov	%ax, %ds       	\n"
				    "	mov	%ax, %es	\n"
				    "	mov	%ax, %fs	\n"
				    "	mov	%ax, %gs	\n"
				    "	mov	%ax, %ss	\n"
				    /* restore proper gdt and idt */
				    "	lgdt	%cs:gdtarg	\n"
				    "	lidt	idtarg		\n"
				    ".globl vga_exit		\n"
				    "vga_exit:			\n"
				    "	mov	__stack, %esp	\n"
				    "	popal			\n");
}

__asm__(".text\n" "real_mode_switch_end:\n");
extern char real_mode_switch_end[];

/* call vga bios int 10 function 0x4f14 to enable main console 
   epia-m does not always autosence the main console so forcing it on is good !! */
void vga_enable_console(void)
{
	__asm__ __volatile__(
				    /* paranoia -- does ecx get saved? not sure. This is 
				     * the easiest safe thing to do. */
				    "	pushal			\n"
				    /* save the stack */
				    "	mov	%esp, __stack	\n"
				    /* load 'our' gdt */
				    "	lgdt 	%cs:__mygdtaddr	\n"
				    /*  This configures CS properly for real mode. */
				    "	ljmp 	$0x28, $__vga_ec_16bit\n"
				    "__vga_ec_16bit:		\n"
				    "	.code16			\n"
				    /* 16 bit code from here on... */
				    /* Load the segment registers w/ properly configured segment
				     * descriptors.  They will retain these configurations (limits,
				     * writability, etc.) once protected mode is turned off. */
				    "	mov	$0x30, %ax     	\n"
				    "	mov	%ax, %ds       	\n"
				    "	mov	%ax, %es       	\n"
				    "	mov	%ax, %fs       	\n"
				    "	mov	%ax, %gs       	\n"
				    "	mov	%ax, %ss       	\n"
				    /* Turn off protection (bit 0 in CR0) */
				    "	movl	%cr0, %eax     	\n"
				    "	andl	$0xFFFFFFFE, %eax\n"
				    "	movl	%eax, %cr0     	\n"
				    /* Now really going into real mode */
				    "	ljmp	$0, $__vga_ec_real \n"
				    "__vga_ec_real:                  \n"
				    /* put the stack at the end of page zero. 
				     * that way we can easily share it between real and protected, 
				     * since the 16-bit ESP at segment 0 will work for any case. 
				     */
				    /* Setup a stack */
				    "	mov	$0x0, %ax	\n"
				    "	mov	%ax, %ss	\n"
				    "	movl	$0x1000, %eax	\n"
				    "	movl	%eax, %esp	\n"
				    /* debugging for RGM */
				    "	mov	$0x11, %al	\n"
				    "	outb	%al, $0x80	\n"
				    /* Load our 16 it idt */
				    "	xor	%ax, %ax       	\n"
				    "	mov	%ax, %ds	\n"
				    "	lidt	__myidt		\n"
				    /* Dump zeros in the other segregs */
				    "	mov	%ax, %ds	\n"
				    "	mov	%ax, %es	\n"
				    "	mov	%ax, %fs	\n"
				    "	mov	%ax, %gs	\n"
				    /* ask bios to enable main console */
				    /* set up for int 10 call - values found from X server
				     * bios call routines */
				    "	movw	$0x4f14,%ax	\n"
				    "	movw	$0x8003,%bx	\n"
				    "	movw	$1, %cx		\n"
				    "	movw	$0, %dx		\n"
				    "	movw	$0, %di		\n"
				    "	int	$0x10		\n"
				    "	movb	$0x55, %al	\n"
				    "	outb	%al, $0x80	\n"
				    /* if we got here, just about done. 
				     * Need to get back to protected mode */
				    "	movl	%cr0, %eax	\n" "	orl	$0x0000001, %eax\n"	/* PE = 1 */
				    "	movl	%eax, %cr0	\n"
				    /* Now that we are in protected mode jump to a 32 bit code segment. */
				    "	data32	ljmp	$0x10, $vga_ec_restart\n"
				    "vga_ec_restart:\n"
				    "	.code32\n"
				    "	movw	$0x18, %ax	\n"
				    "	mov	%ax, %ds	\n"
				    "	mov	%ax, %es	\n"
				    "	mov	%ax, %fs	\n"
				    "	mov	%ax, %gs	\n"
				    "	mov	%ax, %ss	\n"
				    /* restore proper gdt and idt */
				    "	lgdt	%cs:gdtarg 	\n"
				    "	lidt	idtarg		\n"
				    "	.globl	vga__ec_exit	\n"
				    "vga_ec_exit:\n"
				    "	mov	__stack, %esp	\n"
				    "	popal\n");
}

void do_vgabios(void)
{
	device_t dev;
	unsigned long busdevfn;
	unsigned int rom = 0;
	unsigned char *buf;
	unsigned int size = 64 * 1024;
	int i;

	/* clear vga bios data area */
	for (i = 0x400; i < 0x500; i++) {
		*(unsigned char *)i = 0;
	}

	dev = dev_find_class(PCI_CLASS_DISPLAY_VGA << 8, 0);

	if (!dev) {
		printk_debug("NO VGA FOUND\n");
		return;
	}
	printk_debug("found VGA: vid=%x, did=%x\n", dev->vendor, dev->device);

	/* declare rom address here - keep any config data out of the way
	 * of core LXB stuff */

#warning ROM address hardcoded to 512K
	rom = 0xfff80000;
	pci_write_config32(dev, PCI_ROM_ADDRESS, rom | 1);
	printk_debug("rom base, size: %x\n", rom);

	buf = (unsigned char *)rom;
	if ((buf[0] == 0x55) && (buf[1] == 0xaa)) {
		memcpy((void *)0xc0000, buf, size);

		write_protect_vgabios();	// in northbridge

		// check signature again
		buf = (unsigned char *)0xc0000;
		if (buf[0] == 0x55 && buf[1] == 0xAA) {
			busdevfn =
			    (dev->bus->secondary << 8) | dev->path.pci.devfn;
			printk_debug("bus/devfn = %#x\n", busdevfn);

			real_mode_switch_call_vga(busdevfn);
		} else
			printk_debug("Failed to copy VGA BIOS to 0xc0000\n");
	} else
		printk_debug("BAD SIGNATURE 0x%x 0x%x\n", buf[0], buf[1]);

	pci_write_config32(dev, PCI_ROM_ADDRESS, 0);
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
// second, since this only ever runs as part of linuxbios, 
// we know all the segment register values -- so we don't save any.
// keep the handler that calls things small. It can do a call to 
// more complex code in linuxbios itself. This helps a lot as we don't
// have to do address fixup in this little stub, and calls are absolute
// so the handler is relocatable.
void handler(void)
{
	__asm__ __volatile__("	.code16		\n"
			     "idthandle:		\n"
			     "	pushal		\n"
			     "	movb 	$0, %al	\n"
			     "	ljmp 	$0, $callbiosint16\n"
			     "end_idthandle:		\n"
			     "	.code32		\n");
}

void debughandler(void)
{
	__asm__ __volatile__("	.code16		\n"
			     "debughandle:		\n"
			     "	pushw	%cx	\n"
			     "	movw	$250, %cx \n"
			     "dbh1:			\n"
			     "	loop	dbh1	\n"
			     "	popw	%cx	\n"
			     "	iret		\n"
			     "end_debughandle:	\n" ".code32		\n");
}

// Calling conventions. The first C function is called with this stuff
// on the stack. They look like value parameters, but note that if you
// modify them they will go back to the INTx function modified. 
// the C function will call the biosint function with these as
// REFERENCE parameters. In this way, we can easily get 
// returns back to the INTx caller (i.e. vgabios)
void callbiosint(void)
{
	__asm__ __volatile__("	.code16		\n"
			     "callbiosint16:		\n"
			     "	push	%ds	\n"
			     "	push	%es	\n"
			     "	push	%fs	\n" "	push	%gs	\n"
			     // clean up the int #. To save space we put it in the lower
			     // byte. But the top 24 bits are junk. 
			     "	andl	$0xff, %eax\n"
			     // this push does two things:
			     // - put the INT # on the stack as a parameter
			     // - provides us with a temp for the %cr0 mods.
			     "	pushl	%eax	\n" "	movl    %cr0, %eax\n" "	orl	$0x00000001, %eax\n"	/* PE = 1 */
			     "	movl	%eax, %cr0\n"
			     /* Now that we are in protected mode jump to a 32 bit code segment. */
			     "	data32  ljmp    $0x10, $biosprotect\n"
			     "biosprotect:		\n"
			     "	.code32		\n"
			     "	movw	$0x18, %ax          \n"
			     "	mov	%ax, %ds          \n"
			     "	mov	%ax, %es          \n"
			     "	mov	%ax, %fs          \n"
			     "	mov	%ax, %gs          \n"
			     "	mov	%ax, %ss          \n"
			     "	lidt	idtarg         \n"
			     "	call	biosint	\n"
			     // back to real mode ...
			     "	ljmp	$0x28, $__rms_16bit2\n"
			     "__rms_16bit2:			\n"
			     "	.code16			\n"
			     /* 16 bit code from here on... */
			     /* Load the segment registers w/ properly configured segment
			      * descriptors.  They will retain these configurations (limits,
			      * writability, etc.) once protected mode is turned off. */
			     "	mov	$0x30, %ax	\n"
			     "	mov	%ax, %ds	\n"
			     "	mov	%ax, %es	\n"
			     "	mov	%ax, %fs	\n"
			     "	mov	%ax, %gs	\n"
			     "	mov	%ax, %ss	\n"
			     /* Turn off protection (bit 0 in CR0) */
			     "	movl	%cr0, %eax		\n"
			     "	andl	$0xFFFFFFFE, %eax	\n"
			     "	movl	%eax, %cr0		\n"
			     /* Now really going into real mode */
			     "	ljmp $0,  $__rms_real2	\n"
			     "__rms_real2:			\n"
			     /* Setup a stack
			      * FixME: where is esp? */
			     "	mov	$0x0, %ax       \n"
			     "	mov	%ax, %ss	\n"
			     /* ebugging for RGM */
			     "	mov	$0x11, %al	\n"
			     "	outb	%al, $0x80	\n"
			     /* Load our 16 it idt */
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
			     "	.code32			\n");
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

extern void vga_exit(void);

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
		printk_debug("biosint: Bailing out\n");
		// "longjmp"
		vga_exit();
		break;

	case PCIBIOS:
		ret = pcibios(&edi, &esi, &ebp, &esp,
			      &ebx, &edx, &ecx, &eax, &flags);
		break;
	case MEMSIZE:
		// who cares. 
		eax = 64 * 1024;
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

	// VIA's VBIOS will call f000:f859 instead of sending int15.
	codeptr = (unsigned char *)0xff859;
	memcpy(codeptr, &idthandle, codesize);
	intbyte = codeptr + 3;
	*intbyte = 0x15;

	/* debug handler - useful to set a programmable delay between instructions if the
	   TF bit is set upon call to real mode */
	idts[1].cs = 0;
	idts[1].offset = 16384;
	memcpy((void *)16384UL, &debughandle, &end_debughandle - &debughandle);
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
	unsigned short func = (unsigned short)(*peax);
	int retval = 0;
	unsigned short devid, vendorid, devfn;
	short devindex;		/* Use short to get rid of garbage in upper half of 32-bit register */
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
	}
	return res;
}
