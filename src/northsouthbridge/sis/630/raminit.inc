/*
 * raminit.inc:	Setting registers to their recommended values for SiS 630.
 *		Since we have inited the DRAM in IPL it is not neceressary
 *		to do "raminit" here any more.
 *
 *
 * Copyright 2000 Silicon Integrated Systems Corporation
 *
 *	This program is free software; you can redistribute it and/or modify
 *	it under the terms of the GNU General Public License as published by
 *	the Free Software Foundation; either version 2 of the License, or
 *	(at your option) any later version.
 *
 *	This program is distributed in the hope that it will be useful,
 *	but WITHOUT ANY WARRANTY; without even the implied warranty of
 *	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *	GNU General Public License for more details.
 *
 *	You should have received a copy of the GNU General Public License
 *	along with this program; if not, write to the Free Software
 *	Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 *
 * Reference:
 *	1. SiS 630 Specification
 *	2. SiS 630A1 Register Recommended Setting
 *	   Rev 0.97, Jan 7, 2000
 *
 * $Id$
 */

#define PCI_COMMAND_PORT	0xcf8
#define PCI_DATA_PORT		0xcfc

#define NORTH_BRIDGE_BASE_ADDR	0x80000000
#define LPC_BRIDGE_BASE_ADDR	0x80000800
#define PCI_PCI_BRIDGE_BASE_ADDR	0x80001000

register_setting_start:
	movl	$northbridge_init_table, %esi
	movl	$(northbridge_init_table_end - northbridge_init_table), %ecx
	shrl	$0x01, %ecx
	
init_northbridge:
	lodsw	(%esi), %ax			# load the register address/value to %ax
	xchgb	%al, %ah			# exchange address/values for write_common

write_northbridge_register:			
	/* Input: AH - register number. AL - register value. */
	movl	$NORTH_BRIDGE_BASE_ADDR, %edx

	movl	%eax, %ebx			# Save %eax to %ebx.

	movzbl	%bh, %eax			# add register address to
	addl	%edx, %eax			# PCI base address

	movw	$PCI_COMMAND_PORT, %dx
	outl	%eax, %dx

	movw	$PCI_DATA_PORT, %dx
	andb	$0x03, %al
	addb	%al, %dl
	movb	%bl, %al
	outb	%al, %dx
	loopnz	init_northbridge

#ifdef VIDEO_CONSOLE
// by S. Gehlbach < steve @ kesa . com >
// this turns on the vga io access
	movw	$0x3e08, %ax			# Writing PCI-PCI VGA I/O enable register
write_vga_register:
	/* Input: AH - register number. AL - register value. */
	movl	$PCI_PCI_BRIDGE_BASE_ADDR, %edx
	movl	%eax, %ebx			# Save %eax to %ebx.

	movzbl	%bh, %eax			# add register address to
	addl	%edx, %eax			# PCI base address

	movw	$PCI_COMMAND_PORT, %dx
	outl	%eax, %dx

	movw	$PCI_DATA_PORT, %dx
	andb	$0x03, %al
	addb	%al, %dl
	movb	%bl, %al
	outb	%al, %dx
#endif

#ifdef DISABLE_INTERNAL_DEVICES
	movw	$0x7c0e, %ax			# Writting undocumented LPC register
write_lpc_register:
	/* Input: AH - register number. AL - register value. */
	movl	$LPC_BRIDGE_BASE_ADDR, %edx
	movl	%eax, %ebx			# Save %eax to %ebx.

	movzbl	%bh, %eax			# add register address to
	addl	%edx, %eax			# PCI base address

	movw	$PCI_COMMAND_PORT, %dx
	outl	%eax, %dx

	movw	$PCI_DATA_PORT, %dx
	andb	$0x03, %al
	addb	%al, %dl
	movb	%bl, %al
	outb	%al, %dx
#endif /* DISABLE_INTERNAL_DEVICES */

	jmp	register_setting_end

#ifdef SIS630S
#include "630s_regs.inc"
#else /* !SIS630S */
#include "630_regs.inc"
#endif

register_setting_end:
