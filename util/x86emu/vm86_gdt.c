/*
 *  Erik Arjan Hendriks <hendriks@lanl.gov>
 *  Copyright (C) 2000 Scyld.
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
 */


/* Declare a temporary global descriptor table - 
 * necessary because the core part of the bios 
 * no longer sets up any 16 bit segments 
 */

__asm__ (
	/* pointer to original gdt */
	"	.globl gdtarg\n"
	"gdtarg:			\n"
	"	.word	gdt_limit	\n"
	"	.long	gdtptr	       	\n"		

	/* compute the table limit */
	"__mygdt_limit = __mygdt_end - __mygdt - 1	\n"
	"	.globl __mygdtaddr\n"
	"__mygdtaddr:			\n"
	"	.word	__mygdt_limit  	\n"
	"	.long	__mygdt	       	\n"

	"	.globl __mygdt\n"
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

	/* FIXME: This does probably not belong here */
	"	.globl idtarg\n"
	"idtarg:\n"
	"	.word   _idt_end - _idt - 1\n"     /* limit */
	"	.long   _idt\n"
	"	.word   0\n"
	"_idt:\n"
	"	.fill   20, 8, 0\n" //       # idt is unitiailzed
	"_idt_end:\n"

	/* Declare a pointer to where our idt is going to be i.e. at mem zero */
	"	.globl __myidt\n"
	 "__myidt:		\n"
	 /* 16-bit limit */
	 "	.word 1023	\n"
	 /* 24-bit base */
	 "	.long 0		\n"
	 "	.word 0		\n"
);
