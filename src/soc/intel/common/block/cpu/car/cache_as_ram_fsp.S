/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <device/pci_def.h>
#include <cpu/x86/cr.h>
#include <cpu/x86/post_code.h>
#include <intelblocks/post_codes.h>

#define CBFS_FILE_MAGIC 0
#define CBFS_FILE_LEN (CBFS_FILE_MAGIC + 8)
#define CBFS_FILE_TYPE (CBFS_FILE_LEN + 4)
#define CBFS_FILE_CHECKSUM (CBFS_FILE_TYPE + 4)
#define CBFS_FILE_OFFSET (CBFS_FILE_CHECKSUM + 4)

.section .init, "ax", @progbits

.extern	temp_ram_init_params

.global	bootblock_pre_c_entry
bootblock_pre_c_entry:

.global	cache_as_ram
cache_as_ram:
	post_code(POST_BOOTBLOCK_CAR)

	movl	$(CONFIG_FSP_T_LOCATION), %ebx
	add	$0x94, %ebx

	/*
	 * save mm2 into ebp, because TempRamInit API doesn't preserve
	 * mm2 register
	 */
	movd	%mm2, %ebp

	/*
	 * ebx = FSP INFO HEADER
	 * Calculate entry into FSP
	 */
	movl	0x30(%ebx), %eax	/* Load TempRamInitEntryOffset */
	addl	0x1c(%ebx), %eax	/* add the FSP ImageBase */

	/*
	 * Pass early init variables on a fake stack (no memory yet)
	 * as well as the return location
	 */
	leal	CAR_init_stack, %esp

	/* call FSP binary to setup temporary stack */
	jmp	*%eax

/*
 * If the TempRamInit API is successful, then when returning, the ECX and
 * EDX registers will point to the temporary but writeable memory range
 * available to the bootloader where ECX is the start and EDX is the end of
 * the range i.e. [ECX,EDX). See FSP Integration Guide for more information.
 *
 * Return Values:
 * EAX | Return Status
 * ECX | Temporary Memory Start
 * EDX | Temporary Memory End
 * EBX, EDI, ESI, EBP, MM0, MM1 | Preserved Through API Call
 */

CAR_init_done:
	cmp	$0, %eax
	jnz	.halt_forever

	/* Setup bootblock stack */
	movl	$_ecar_stack, %esp
	/*
	 * temp_memory_start/end reside in the .bss section, which gets cleared
	 * below. Save the FSP return value to the stack before writing those
	 * variables.
	 */
	push	%ecx
	push	%edx

	/* clear .bss section as it is not shared */
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

	/* Restore the timestamp from bootblock_crt0.S (ebp:mm1) */
	push	%ebp
	movd	%mm1, %eax
	push	%eax

	/* We can call into C functions now */
	call	bootblock_c_entry

	/* Never reached */

.halt_forever:
	post_code(POST_DEAD_CODE)
	hlt
	jmp	.halt_forever

CAR_init_stack:
	.long	CAR_init_done
	.long	temp_ram_init_params

fsp_name:
	.string	"fspt.bin"
