/*
    freebios/src/northbridge/nsc/gx1/gx_setup.inc

    Copyright (c) 2002 Christer Weinigel <wingel@hack.org>

    Setup the GX_BASE registers on a National Semiconductor Geode CPU
*/

#include <cpu/amd/gx1def.h>

	movl %eax, %ebp /* Preserve bist */

gx_setup_start:
	leal	gx_setup_table, %esi
	movl	$gx_setup_len, %ecx
	movl	$GX_BASE, %edi

gx_setup_loop:
	movw	(%esi), %di		/* Only read the low word of address */
	addl	$4, %esi
	movl	(%esi), %eax		/* Data */
	addl	$4, %esi
	movl	%eax, (%edi)
	loop	gx_setup_loop

	jmp	gx_setup_end

gx_setup_table:
	/* Allow writes to config registers */
	.long	DC_UNLOCK, DC_UNLOCK_MAGIC
	.long	DC_GENERAL_CFG, 0
	.long	DC_UNLOCK, 0

	.long	BC_DRAM_TOP, 0x3fffffff
	.long	BC_XMAP_1, 0x60
	.long	BC_XMAP_2, 0
	.long	BC_XMAP_3, 0

	.long	MC_BANK_CFG, 0x00700070	/* No DIMMS installed */
	.long	MC_MEM_CNTRL1, XBUSARB
	.long	MC_GBASE_ADD, 0x7ff	/* Almost 1GB */
gx_setup_len = (.-gx_setup_table)/8

gx_setup_end:
	nop

	movl %ebp, %eax	/* Restore bist */
