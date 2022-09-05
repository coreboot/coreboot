/* SPDX-License-Identifier: GPL-2.0-only */

/*
 * This path is for stages that are post bootblock. The gdt is reloaded
 * to accommodate platforms that are executing out of CAR. In order to
 * continue with C code execution one needs to set stack pointer and
 * clear .bss variables that are stage specific.
 */

#if CONFIG(RESET_VECTOR_IN_RAM)
	#define _STACK_TOP _eearlyram_stack
#else
	#define _STACK_TOP _ecar_stack
#endif

#if ENV_X86_64
.code64
#else
.code32
#endif

.section ".text._start", "ax", @progbits
.global _start
_start:

	/* Migrate GDT to this text segment */
#if ENV_X86_64
	call	gdt_init64
#else
	call	gdt_init
#endif

	/* reset stack pointer to CAR/EARLYRAM stack */
	mov	$_STACK_TOP, %esp

	/* clear .bss section as it is not shared */
	cld
	xor	%eax, %eax
	movl	$(_ebss), %ecx
	movl	$(_bss), %edi
	sub	%edi, %ecx
	shrl	$2, %ecx
	rep	stosl

#if ((ENV_SEPARATE_VERSTAGE && CONFIG(VERSTAGE_DEBUG_SPINLOOP)) \
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

#if CONFIG(ASAN_IN_ROMSTAGE)
	call	asan_init
#endif
	call	car_stage_entry

	/* Expect to never return. */
1:
	jmp	1b
