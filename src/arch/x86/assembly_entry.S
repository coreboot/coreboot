/*
 * This file is part of the coreboot project.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <rules.h>

#if !CONFIG(ROMCC_BOOTBLOCK)

/*
 * This path is for stages that are post bootblock. The gdt is reloaded
 * to accommodate platforms that are executing out of CAR. In order to
 * continue with C code execution one needs to set stack pointer and
 * clear .bss variables that are stage specific.
 */
.section ".text._start", "ax", @progbits
.global _start
_start:

	/* Migrate GDT to this text segment */
	call	gdt_init

	/* reset stack pointer to CAR stack */
	mov	$_ecar_stack, %esp

	/* clear .bss section as it is not shared */
	cld
	xor	%eax, %eax
	movl	$(_ebss), %ecx
	movl	$(_bss), %edi
	sub	%edi, %ecx
	shrl	$2, %ecx
	rep	stosl

#if ((ENV_VERSTAGE && CONFIG(VERSTAGE_DEBUG_SPINLOOP)) \
	|| (ENV_ROMSTAGE && CONFIG(ROMSTAGE_DEBUG_SPINLOOP)))

	/* Wait for a JTAG debugger to break in and set EBX non-zero */
	xor	%ebx, %ebx

debug_spinloop:
	cmp	$0, %ebx
	jz	debug_spinloop
#endif

	andl	$0xfffffff0, %esp
#if CONFIG(IDT_IN_EVERY_STAGE)
	call	exception_init
#endif
	call	car_stage_entry

	/* Expect to never return. */
1:
	jmp	1b

#else

/* This file assembles the start of the romstage program by the order of the
 * includes. Thus, it's extremely important that one pays very careful
 * attention to the order of the includes. */

#include <arch/x86/prologue.inc>
#include <cpu/x86/32bit/entry32.inc>
#include <cpu/x86/fpu_enable.inc>
#if CONFIG(SSE)
#include <cpu/x86/sse_enable.inc>
#endif

/*
 * The assembly.inc is generated based on the requirements of the mainboard.
 * For example, for ROMCC boards the MAINBOARDDIR/romstage.c would be
 * processed by ROMCC and added. In non-ROMCC boards the chipsets'
 * cache-as-ram setup files would be here.
 */
#include <generated/assembly.inc>

#endif
