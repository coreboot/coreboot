/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2015-2016 Intel Corp.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <device/pci_def.h>
#include <cpu/x86/mtrr.h>
#include <cpu/x86/cache.h>
#include <cpu/x86/cr.h>
#include <cpu/x86/post_code.h>
#include <soc/cpu.h>

#include <../../../arch/x86/walkcbfs.S>

#define FSP_HDR_OFFSET 0x94

.global	bootblock_pre_c_entry
bootblock_pre_c_entry:

.global	cache_as_ram
cache_as_ram:
	post_code(0x21)

	/* find fsp in cbfs */
	lea	fsp_name, %esi
	mov	$1f, %esp
	jmp	walkcbfs_asm
1:
	cmp	$0, %eax
	jz	.halt_forever
	mov	CBFS_FILE_OFFSET(%eax), %ebx
	bswap	%ebx
	add	%eax, %ebx
	add	FSP_HDR_OFFSET, %ebx

	/*
	 * ebx = FSP INFO HEADER
	 * Calculate entry into FSP
	 */
	mov	0x30(%ebx), %eax	/* Load TempRamInitEntryOffset */
	add	0x1c(%ebx), %eax	/* add the FSP ImageBase */

	/*
	 * Pass early init variables on a fake stack (no memory yet)
	 * as well as the return location
	 */
	lea	CAR_init_stack, %esp

	/* call FSP binary to setup temporary stack */
	jmp	*%eax

/*
 * If the TempRamInit API is successful, then when returning, the ECX and
 * EDX registers will point to the temporary but writeable memory range
 * available to the bootloader where ECX is the start and EDX is the end of
 * the range i.e. [ECX,EDX).  See Apollo Lake FSP Integration Guide for more
 * information.
 *
 * Return Values:
 * EAX | Return Status
 * ECX | Temporary Memory Start
 * EDX | Temporary Memory End
 * EBX, EDI, ESI, EBP, MM0, MM1 | Preserved Through API Call
 */

CAR_init_done:

	/* Setup bootblock stack */
	mov	%edx, %esp

	/* clear CAR_GLOBAL area as it is not shared */
	cld
	xor	%eax, %eax
	movl	$(_car_global_end), %ecx
	movl	$(_car_global_start), %edi
	sub	%edi, %ecx
	rep	stosl

	/* We can call into C functions now */
	call	bootblock_c_entry

	/* Never reached */

.halt_forever:
	post_code(POST_DEAD_CODE)
	hlt
	jmp	.halt_forever

CAR_init_params:
	.long	0                                 /* Microcode Location */
	.long	0                                 /* Microcode Length */
	.long	0xFFFFFFFF - CONFIG_ROM_SIZE + 1 /* Firmware Location */
	.long	CONFIG_ROM_SIZE                  /* Total Firmware Length */

CAR_init_stack:
	.long	CAR_init_done
	.long	CAR_init_params

fsp_name:
	.ascii	"blobs/fspt.bin\x00"
