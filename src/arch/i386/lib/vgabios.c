#ifndef lint
static char rcsid[] = "$Id$";
#endif

#include <pci.h>
#include <pci_ids.h>
#undef __KERNEL__
#include <arch/io.h>
#include <printk.h>
#include <string.h>

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
#if (CONFIG_VGABIOS == 1)


/* The address arguments to this function are PHYSICAL ADDRESSES */ 
static void real_mode_switch_call_vga(void)
{
  __asm__ __volatile__
      (
	/* save the stack */
	"mov %esp, __stack\n"
	"jmp 1f\n"
	"__stack: .long 0\n"
	"1:\n"
	/*  This configures CS properly for real mode. */
       "    ljmp $0x28, $__rms_16bit\n"
       "__rms_16bit:                 \n"
       ".code16                      \n" /* 16 bit code from here on... */

       /* Load the segment registers w/ properly configured segment
	* descriptors.  They will retain these configurations (limits,
	* writability, etc.) once protected mode is turned off. */
       "    mov  $0x30, %ax         \n"
       "    mov  %ax, %ds          \n"
       "    mov  %ax, %es          \n"
       "    mov  %ax, %fs          \n"
       "    mov  %ax, %gs          \n"
       "    mov  %ax, %ss          \n"

       /* Turn off protection (bit 0 in CR0) */
       "    movl %cr0, %eax        \n"
       "    andl $0xFFFFFFFE, %eax  \n"
       "    movl %eax, %cr0        \n"

       /* Now really going into real mode */
       "    ljmp $0,  $__rms_real \n"
       "__rms_real:                  \n"

	// put the stack at the end of page zero. 
	// that way we can easily share it between real and protected, 
	// since the 16-bit ESP at segment 0 will work for any case. 
       /* Setup a stack */
       "    mov  $0x0, %ax       \n"
       "    mov  %ax, %ss          \n"
       "    movl  $0x1000, %eax       \n"
       "    movl  %eax, %esp          \n"
 	/* ebugging for RGM */
       "    mov $0x11, %al	\n"
	" outb	%al, $0x80\n"

       /* Dump zeros in the other segregs */
       "    xor  %ax, %ax          \n"
       "    mov  %ax, %ds          \n"
       "    mov  %ax, %es          \n"
       "    mov  %ax, %fs          \n"
       "    mov  %ax, %gs          \n"
	" .byte 0x9a, 0x03, 0, 0, 0xc0  \n"
	" movb $0x55, %al\noutb %al, $0x80\n"
       /* if we got here, just about done. 
	* Need to get back to protected mode */
       "movl	%cr0, %eax\n"
//       "andl	$0x7FFAFFD1, %eax\n" /* PG,AM,WP,NE,TS,EM,MP = 0 */
//	"orl	$0x60000001, %eax\n" /* CD, NW, PE = 1 */
	"orl	$0x0000001, %eax\n" /* PE = 1 */
       "movl	%eax, %cr0\n"
	/* Now that we are in protected mode jump to a 32 bit code segment. */
       "data32	ljmp	$0x10, $vgarestart\n"
       "vgarestart:\n"
       ".code32\n"
       "    movw $0x18, %ax          \n"
       "    mov  %ax, %ds          \n"
       "    mov  %ax, %es          \n"
       "    mov  %ax, %fs          \n"
       "    mov  %ax, %gs          \n"
       "    mov  %ax, %ss          \n"
       "    mov  __stack, %esp\n"
       );
}
__asm__ (".text\n""real_mode_switch_end:\n");
extern char real_mode_switch_end[];

void
do_vgabios(void)
{
  struct pci_dev *dev;
  unsigned int rom = 0;
  unsigned char *buf;
  unsigned int size = 64*1024;
  int i;

  dev = pci_find_class(PCI_CLASS_DISPLAY_VGA <<8, NULL);

  if (! dev) {
    printk_debug("NO VGA FOUND\n");
    return;
  }
  printk_debug("found VGA: vid=%ux, did=%ux\n", dev->vendor, dev->device);
  pci_read_config_dword(dev, PCI_ROM_ADDRESS, &rom);
  // paranoia
  rom = 0xf0000000;
  pci_write_config_dword(dev, PCI_ROM_ADDRESS, rom|1);
  printk_debug("rom base, size: %x\n", rom);
  buf = (unsigned char *) rom;
  if ((buf[0] == 0x55) && (buf[1] = 0xaa)) {
  	memcpy((void *) 0xc0000, buf, size);

  	for(i = 0; i < 16; i++)
    		printk_debug("0x%x ", buf[i]);
  	// check signature here later!
  	real_mode_switch_call_vga();
  } else 
	printk_debug("BAD SIGNATURE 0x%x 0x%x\n", buf[0], buf[1]);
  pci_write_config_dword(dev, PCI_ROM_ADDRESS, 0);
}

#endif // (CONFIG_VGABIOS == 1)
