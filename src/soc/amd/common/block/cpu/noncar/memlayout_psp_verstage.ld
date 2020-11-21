/* SPDX-License-Identifier: GPL-2.0-only */

#include <memlayout.h>
#include <soc/psp_transfer.h>
#include <fmap_config.h>

/* TODO: Move defines to SoC-specific header file to allow SoC specific values if needed. */

/*
 * Start of available space is 0x15000 and this is where the
 * header for the user app (verstage) must be mapped.
 * Size is 0x28000 bytes
 */
#define PSP_SRAM_START			0x15000
#define PSP_SRAM_SIZE			160K

#define VERSTAGE_START			0x15000

/*
 * The temp stack can be made much smaller if needed - even 256 bytes
 * should be sufficient.  This is just for the function mapping the
 * actual stack.
 */
#define PSP_VERSTAGE_TEMP_STACK_START	0x32000
#define PSP_VERSTAGE_TEMP_STACK_SIZE	4K

/*
 * The top of the stack must be 4k aligned, so set the bottom as 4k aligned
 * and make the size a multiple of 4k
 */
#define PSP_VERSTAGE_STACK_START	0x33000
#define PSP_VERSTAGE_STACK_SIZE		40K

ENTRY(_psp_vs_start)
SECTIONS
{
	SRAM_START(PSP_SRAM_START)
	_verstage = .;

		.text : { *(PSP_HEADER_DATA) }
		.text : { *(.text._psp_vs_start) }
		.text : { *(.text.Main) }
		.text : { *(.text*) }
		.rodata : { *(.rodata*) }

		.data : { *(.data*) }
		.data : { *(PSP_FOOTER_DATA) }

		_bss_start = .;
		.bss : { *(.bss*) }
		_bss_end = .;

	ALIGN_COUNTER(64)
	_everstage = .;

	#include "memlayout_transfer_buffer.inc"

	PSP_VERSTAGE_TEMP_STACK_END = (PSP_VERSTAGE_TEMP_STACK_START + PSP_VERSTAGE_TEMP_STACK_SIZE );

	REGION(stack, PSP_VERSTAGE_STACK_START, PSP_VERSTAGE_STACK_SIZE, 64)
	PSP_VERSTAGE_STACK_BASE = _stack;
	PSP_VERSTAGE_STACK_END = _estack;
	SRAM_END(PSP_SRAM_START + PSP_SRAM_SIZE)
}
