/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2000,2007 Ronald G. Minnich <rminnich@gmail.com>
 * Copyright (C) 2007-2008 coresystems GmbH
 * Copyright (C) 2013-2014 Sage Electronic Engineering, LLC.
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

#include <cpu/x86/mtrr.h>
#include <cpu/x86/cache.h>
#include <cpu/x86/post_code.h>

	cmp   $0, %eax
	je    cache_as_ram
	mov   $0xa0, %eax
	jmp   .Lhlt

cache_as_ram:
	post_code(0x20)

	/*
	 * Find the FSP binary in cbfs.
	 * Make a fake stack that has the return value back to this code.
	 */
	lea   fake_fsp_stack, %esp
	jmp   find_fsp_bypass_prologue
find_fsp_ret:
	/* Save the FSP location */
	mov   %eax,   %ebp
	cmp   $CONFIG_FSP_LOC, %eax
	jae   find_fsp_ok
	mov   $0xb0, %eax
	jmp   .Lhlt

find_fsp_ok:
	post_code(POST_FSP_TEMP_RAM_INIT)

	/* Calculate entry into FSP */
	mov   0x30(%ebp), %eax	/* Load TempRamInitEntry */
	add   0x1c(%ebp), %eax	/* add in the offset for the FSP base address */

	/*
	 * Pass early init variables on a fake stack (no memory yet)
	 * as well as the return location
	 */
	lea   CAR_init_stack, %esp

	/* call FSP binary to setup temporary stack */
	jmp   *%eax

CAR_init_done:
	addl  $4, %esp
	cmp   $0, %eax
	je    car_init_ok
	add   $0xc0, %eax
	jmp   .Lhlt

car_init_ok:

	/* Save FSP_INFO_HEADER location in ebx */
	mov    %ebp, %ebx

	/*
	 * set up bootloader stack
	 * ecx:  stack base
	 * edx:  stack top
	 */
	mov   %edx, %esp
	movl  %esp, %ebp

	/* Clear the cbmem CAR memory region. */
	movl  %ecx, %edi
	movl  %edx, %ecx
	sub   %edi, %ecx
	shr   $2, %ecx
	xorl  %eax, %eax
	rep   stosl

before_romstage:
	post_code(0x23)

	/* Call romstage.c main function. */
	pushl %ebx /* main takes FSP_INFO_HEADER as its argument */
	call  main /* does not return */
	movb  $0xB8, %ah
	jmp   .Lhlt

.Lhlt:
#if IS_ENABLED(CONFIG_POST_IO)
	outb  %al, $CONFIG_POST_IO_PORT
#endif
	hlt
	jmp   .Lhlt

/*
 * esp is set to this location so that the call into and return from the FSP
 * in find_fsp will work.
 */
	.align 4
fake_fsp_stack:
	.long  find_fsp_ret

CAR_init_params:
	.long  dummy_microcode
	.long  0
	.long  CACHE_ROM_BASE	/* Firmware Location */
	.long  CACHE_ROM_SIZE	/* Total Firmware Length */

CAR_init_stack:
	.long  CAR_init_done
	.long  CAR_init_params

dummy_microcode:
	.long  0
