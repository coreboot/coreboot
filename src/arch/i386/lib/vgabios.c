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
 *  $Id$
 *--------------------------------------------------------------------*/

/* andrey -- ugly */
	memcpy(regions->addr, vgarom, 1024*64);
	printk("copied region...");

	//for(err = 0; err < 16; err++)
	//	printk("0x%x ", *(regions->addr+err));
	printk("\n");

/* end andrey -- ugly */


#if defined(__i386__)
/*-------------------------------------------------------------------------
 * Machine restart code - x86
 *-----------------------------------------------------------------------*/
static unsigned long long
real_mode_gdt_entries [] = {
        0x0000000000000000ULL,  /* 00h: Null descriptor */
	0x0000000000000000ULL,  /* 08h: Unused... */
	0x00cf9a000000ffffULL,	/* 10h: 32-bit 4GB code at 0x00000000 */
	0x00cf92000000ffffULL,	/* 18h: 32-bit 4GB data at 0x00000000 */
        0x00009a000000ffffULL,  /* 20h: 16-bit 64k code at 0x00000000 */
        0x000092000000ffffULL	/* 28h: 16-bit 64k data at 0x00000000 */
};

static struct {
        unsigned short       size __attribute__ ((packed));
        unsigned long long * base __attribute__ ((packed));
}
real_mode_gdt = { sizeof (real_mode_gdt_entries)-1, 0 },
real_mode_idt = { 0x3ff, 0 };

/*
  Registers:
  eax - scratch
  ebx - memory list pointer
  ecx - counter
  edx - entry point
  esi - scratch memcpy pointer
  edi - scratch memcpy pointer
  ebp - flags
 */
/* The address arguments to this function are PHYSICAL ADDRESSES */ 
static void real_mode_switch(struct monte_reloc_page_t *mem_list,
			     void *entry,
			     unsigned long flags) {
  __asm__ __volatile__
      (
       /* Now that our memcpy is done we can get to 16 bit code
	* segment.  This configures CS properly for real mode. */
       "    ljmp $0x20, $0x1000-(real_mode_switch_end - __rms_16bit) \n"
       "__rms_16bit:                 \n"
       ".code16                      \n" /* 16 bit code from here on... */

       /* Load the segment registers w/ properly configured segment
	* descriptors.  They will retain these configurations (limits,
	* writability, etc.) once protected mode is turned off. */
       "    mov  $0x28, %%ax         \n"
       "    mov  %%ax, %%ds          \n"
       "    mov  %%ax, %%es          \n"
       "    mov  %%ax, %%fs          \n"
       "    mov  %%ax, %%gs          \n"
       "    mov  %%ax, %%ss          \n"

       /* Turn off protection (bit 0 in CR0) */
       "    movl %%cr0, %%eax        \n"
       "    andl $0xFFFFFFFE, %%eax  \n"
       "    movl %%eax, %%cr0        \n"

       /* Now really going into real mode */
       "    ljmp $0, $0x1000-(real_mode_switch_end - __rms_real) \n"
       "__rms_real:                  \n"

       /* Setup a stack */
       "    mov  $0x9000, %%ax       \n"
       "    mov  %%ax, %%ss          \n"
       "    mov  $0xAFFE, %%ax       \n"
       "    mov  %%ax, %%sp          \n"
 	/* ebugging for RGM */
       "    mov $0x11, %%al	\n"
	" outb	%%al, $0x80\n"

       /* Dump zeros in the other segregs */
       "    xor  %%ax, %%ax          \n"
       "    mov  %%ax, %%ds          \n"
       "    mov  %%ax, %%es          \n"
       "    mov  %%ax, %%fs          \n"
       "    mov  %%ax, %%gs          \n"

       "    sti                      \n" /* Enable interrupts */

       /* Try and sanitize the video hardware state. */
       "    mov  $0x0003, %%ax       \n"	/* Ask for 80x25 */
       "    int  $0x10               \n"
	" .byte 0x9a, 0x03, 0, 0, 0xc0  \n"
	" movb $0x55, %%al\noutb %%al, $0x80\n"
       
#if 0
       "    push %%edx               \n" /* Kludge to do far jump */
       "    lret                     \n"
#endif

       /*"    jmp *%%edx               \n"*/
       /*"    ljmp $0x9020, $0x0000    \n"*/
#if 0
       /* Debugging tools... */
       "    .byte 0xcc               \n" /* XXX DEBUG triple fault XXXX */
       "    .byte 0xeb, 0xfe         \n" /* XXX DEBUG wedge XXXXX */
#endif
       ".code32                      \n" /* Restore mode for rest of file */
       : : "m" (mem_list), "m" (entry), "m" (flags));
}
__asm__ (".text\n""real_mode_switch_end:\n");
extern char real_mode_switch_end[];


//static
int monte_restart(unsigned long entry_addr, unsigned long flags) {
    void * ptr;
    struct page *pg;

    /*----- POINT OF NO RETURN IS HERE --------------------------------------*/

    /* Ok, now the real monkey business begins.... Please keep hands
     * and feet inside the memory space and remain seated until the
     * ride comes to a complete stop. */
    __asm__ __volatile__
	(/* Install the IDT and GDT we copied to page zero. */
	 "lidt  %0             \n"
	 "lgdt  %1             \n"

	 
	 /* Function call with args... sort of */
	 "pushl %4             \n"
	 "pushl %3             \n"
	 "pushl %2             \n" /* Push args on the stack */
	 "sub $4, %%esp        \n" /* bogo return address */
	 "ljmp $0x10, $0x1000-(real_mode_switch_end-real_mode_switch) \n" : :
	 "m" (real_mode_idt), "m" (real_mode_gdt),
	 "r" (m_pg_list), "r" (entry_addr), "r" (flags)
	 : "memory");
    /* NOT REACHED */
    while(1);			/* Shut up gcc. */
}
#endif /* defined(__i386__) */


	/* andrey stuff starts here */
	{
	struct pci_dev *dev;
	unsigned int rom = 0;
        unsigned char *buf;


	dev = pci_find_class(PCI_CLASS_DISPLAY_VGA <<8, NULL);
	printk("found VGA: vid=%ux, did=%ux\n", dev->vendor, dev->device);
	pci_read_config_dword(dev, PCI_ROM_ADDRESS, &rom);
	// paranoia
	rom &= ~1;
	pci_write_config_byte(dev, PCI_ROM_ADDRESS, 1);
	printk("rom base, size: %x\n", rom);
	buf = ioremap(rom, sizeof(vgarom));
	printk("buf is %p\n" ,buf);
	if (buf) {
		int i;
		memcpy(vgarom, buf, sizeof(vgarom));
		iounmap(buf);
		for(i = 0; i < 16; i++)
			printk("0x%x ", vgarom[i]);
		printk("\n");
	}
	pci_write_config_byte(dev, PCI_ROM_ADDRESS, 0);
	printk("done...");
	}

