/* SPDX-License-Identifier: GPL-2.0-only */

#include <cpu/x86/mtrr.h>
#include <cpu/x86/cache.h>
#include <cpu/x86/post_code.h>

/*
 * Replacement for cache_as_ram.inc when using the FSP binary.  This code
 * locates the FSP binary, initializes the cache as RAM and performs the
 * first stage of initialization.  Next this code switches the stack from
 * the cache to RAM and then disables the cache as RAM.  Finally this code
 * performs the final stage of initialization.
 */

#define LHLT_DELAY	0x50000	/* I/O delay between post codes on failure */

.section .init, "ax", @progbits

.global	bootblock_pre_c_entry
bootblock_pre_c_entry:
	/*
	 * Per FSP1.1 specs, following registers are preserved:
	 * EBX, EDI, ESI, EBP, MM0, MM1
	 *
	 * Shift values to release MM2.
	 * mm0 -> ebx:  BIST value
	 * mm1 -> mm0:  low 32-bits of TSC value
	 * mm2 -> mm1:  high 32-bits of TSC value
	 */
	movd	%mm0, %ebx
	movd	%mm1, %eax
	movd	%eax, %mm0
	movd	%mm2, %eax
	movd	%eax, %mm1

cache_as_ram:
	post_code(POST_BOOTBLOCK_CAR)

	/* Cache the rom and update the microcode */
cache_rom:
	/* Disable cache */
	movl	%cr0, %eax
	orl	$CR0_CacheDisable, %eax
	movl	%eax, %cr0

	movl	$MTRR_PHYS_BASE(1), %ecx
	xorl	%edx, %edx
	movl	$(CACHE_ROM_BASE | MTRR_TYPE_WRPROT), %eax
	wrmsr

	movl	$MTRR_PHYS_MASK(1), %ecx
	rdmsr
	movl	$(~(CACHE_ROM_SIZE - 1) | MTRR_PHYS_MASK_VALID), %eax
	wrmsr

	/* Enable cache */
	movl	%cr0, %eax
	andl	$(~(CR0_CacheDisable | CR0_NoWriteThrough)), %eax
	invd
	movl	%eax, %cr0

	/* Enable MTRR. */
	movl	$MTRR_DEF_TYPE_MSR, %ecx
	rdmsr
	orl	$MTRR_DEF_TYPE_EN, %eax
	wrmsr

	/* The Google FSP release for Braswell has broken microcode update
	   code and FSP needs the installed microcode revision to be non zero.
	   It is better to have coreboot do it instead of relying on a fragile
	   blob. */
update_microcode:
	/* put the return address in %esp */
	movl	$end_microcode_update, %esp
	jmp	update_bsp_microcode
end_microcode_update:

	/*
	 * Find the FSP binary in cbfs.
	 * Make a fake stack that has the return value back to this code.
	 */
	lea	fake_fsp_stack, %esp
	jmp	find_fsp
find_fsp_ret:
	/* Save the FSP location */
	mov	%eax, %ebp

	/*
	 * Only when a valid FSP binary is found at CONFIG_FSP_LOC is
	 * the returned FSP_INFO_HEADER structure address above the base
	 * address of FSP binary specified by the CONFIG_FSP_LOC value.
	 * All of the error values are in the 0x8xxxxxxx range which are
	 * below the CONFIG_FSP_LOC value.
	 */
	cmp	$CONFIG_FSP_LOC, %eax
	jbe	halt1

	post_code(POST_FSP_TEMP_RAM_INIT)

	/* Calculate entry into FSP */
	mov	0x30(%ebp), %eax	/* Load TempRamInitEntry */
	add	0x1c(%ebp), %eax	/* add in the offset for FSP */

	/*
	 * Pass early init variables on a fake stack (no memory yet)
	 * as well as the return location
	 */
	lea	CAR_init_stack, %esp

	/*
	 *       BIST value is zero
	 * eax:  TempRamInitApi address
	 * ebx:  BIST value
	 * ebp:  FSP_INFO_HEADER address
	 * esi:  Not used
	 * mm0:  low 32-bits of TSC value
	 * mm1:  high 32-bits of TSC value
	 */

	/* call FSP binary to setup temporary stack */
	jmp	*%eax

CAR_init_done:

	/*
	 * ebp:  FSP_INFO_HEADER address
	 * ebx:  BIST value
	 * ecx:  Temp RAM base
	 * edx:  Temp RAM top
	 * mm0:  low 32-bits of TSC value
	 * mm1:  high 32-bits of TSC value
	 */

	cmp	$0, %eax
	jne	halt2

	/* Setup bootblock stack */
	movl	$_ecar_stack, %esp

	/*
	 * ebp:  FSP_INFO_HEADER address
	 * ebx:  BIST value
	 * ecx:  Temp RAM base
	 * edx:  Temp RAM top
	 * esp:  Top of stack in temp RAM
	 * mm0:  low 32-bits of TSC value
	 * mm1:  high 32-bits of TSC value
	 */

	/*
	 * temp_memory_start/end reside in the .bss section, which gets cleared
	 * below. Save the FSP return value to the stack before writing those
	 * variables.
	 */
	push	%ecx
	push	%edx

	/* clear .bss section */
	cld
	xor	%eax, %eax
	movl	$(_ebss), %ecx
	movl	$(_bss), %edi
	sub	%edi, %ecx
	shrl	$2, %ecx
	rep	stosl

	pop	%edx
	movl	%edx, temp_memory_end
	pop	%ecx
	movl	%ecx, temp_memory_start

	/* Need to align stack to 16 bytes at call instruction. Account for
	the pushes below. */
	andl	$0xfffffff0, %esp
	subl	$8, %esp

	/* Push initial timestamp on the stack */
	movd	%mm1, %eax
	pushl	%eax	/* tsc[63:32] */
	movd	%mm0, %eax
	pushl	%eax	/* tsc[31:0] */

before_romstage:
	/* Call bootblock_c_entry(uint64_t base_timestamp) */
	call	bootblock_c_entry

	/* Never reached */

halt1:
	/*
	 * Failures for postcode 0xBA - failed in fsp_fih_early_find()
	 *
	 * Values are:
	 * 0x01 - FV signature, "_FVH" not present
	 * 0x02 - FFS GUID not present
	 * 0x03 - FSP INFO Header not found
	 * 0x04 - ImageBase does not equal CONFIG_FSP_LOC - Is the FSP rebased
	 *	  to a different location, or does it need to be?
	 * 0x05 - FSP INFO Header signature "FSPH" not found
	 * 0x06 - FSP Image ID is not the expected ID.
	 */
	movb	$0xBA, %ah
	jmp	.Lhlt

halt2:
	/*
	 * Failures for postcode 0xBB - failed in the FSP:
	 *
	 * 0x00 - FSP_SUCCESS: Temp RAM was initialized successfully.
	 * 0x02 - FSP_INVALID_PARAMETER: Input parameters are invalid.
	 * 0x03 - FSP_UNSUPPORTED: The FSP calling conditions were not met.
	 * 0x07 - FSP_DEVICE_ERROR: Temp RAM initialization failed
	 * 0x0E - FSP_NOT_FOUND: No valid microcode was found in the microcode
	 *	  region.
	 * 0x14 - FSP_ALREADY_STARTED: Temp RAM initialization has been invoked
	 */
	movb	$0xBB, %ah
	jmp	.Lhlt

.Lhlt:
	xchg	%al, %ah
#if CONFIG(POST_IO)
	outb	%al, $CONFIG_POST_IO_PORT
#else
	post_code(POST_DEAD_CODE)
#endif
	movl	$LHLT_DELAY, %ecx
.Lhlt_Delay:
	outb	%al, $0xED
	loop	.Lhlt_Delay
	jmp	.Lhlt

/*
 * esp is set to this location so that the call into and return from the FSP
 * in find_fsp will work.
 */
	.align 4
fake_fsp_stack:
	.long	find_fsp_ret
	.long	CONFIG_FSP_LOC		/* FSP base address */

CAR_init_params:
	.long	fake_microcode				/* Microcode Location */
	.long	fake_microcode_end - fake_microcode	/* Microcode Length */
	.long	0xFFFFFFFF - CONFIG_ROM_SIZE + 1	/* Firmware Location */
	.long	CONFIG_ROM_SIZE				/* Firmware Length */

CAR_init_stack:
	.long	CAR_init_done
	.long	CAR_init_params

	/* coreboot updates microcode itself. FSP still needs a pointer
	   to something that looks like microcode, so provide it with fake
	   microcode. */
fake_microcode:
fake_microcode_header_start:
	.long	1		/* Header Version */
	.long	1		/* Microcode revision */
	.long	0x10232019	/* Date: Time of writing 23-10-2019 */
	.long	0x00010ff0	/* Sig: (non existing) Family: 0xf, Model: 0x1f, stepping: 0 */
	.long	0		/* Checksum: not checked by FSP, so won't care */
	.long	1		/* Loader Revision */
	.long	1		/* Processor Flags */
	.long	fake_microcode_end - fake_microcode_header_end	/* Data Size */
	.long	fake_microcode_end - fake_microcode	/* Total Size */
	.space	12		/* Reserved */
fake_microcode_header_end:
	.space 0x10	/* 16 bytes of empty data */
fake_microcode_end:
