/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2000,2007 Ronald G. Minnich <rminnich@gmail.com>
 * Copyright (C) 2007-2008 coresystems GmbH
 * Copyright (C) 2013-2014 Sage Electronic Engineering, LLC.
 * Copyright (C) 2015 Intel Corp.
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

/*
 * Replacement for cache_as_ram.inc when using the FSP binary.  This code
 * locates the FSP binary, initializes the cache as RAM and performs the
 * first stage of initialization.  Next this code switches the stack from
 * the cache to RAM and then disables the cache as RAM.  Finally this code
 * performs the final stage of initialization.
 */

#include <rules.h>

#define LHLT_DELAY	0x50000		/* I/O delay between post codes on failure */

	/*
	 * eax:  BIST value
	 * mm0:  low 32-bits of TSC value
	 * mm1:  high 32-bits of TSC value
	 */
	movl	%eax, %edi
cache_as_ram:
	post_code(0x20)

#if IS_ENABLED(CONFIG_SKIP_FSP_CAR)

	/*
	 * SOC specific setup
	 * NOTE: This has to preserve the registers
	 * mm0, mm1 and edi.
	 */
	#include <soc/car_setup.S>

	post_code(0x28)

#endif

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
	 * ebp:  FSP_INFO_HEADER address
	 * edi:  BIST value
	 * esi:  Not used
	 * mm0:  low 32-bits of TSC value
	 * mm1:  high 32-bits of TSC value
	 */

	/* call FSP binary to setup temporary stack */
	jmp	*%eax

CAR_init_done:
	addl	$4, %esp

	/*
	 * ebp:  FSP_INFO_HEADER address
	 * ecx:  Temp RAM base
	 * edx:  Temp RAM top
	 * edi:  BIST value
	 * mm0:  low 32-bits of TSC value
	 * mm1:  high 32-bits of TSC value
	 */

	cmp	$0, %eax
	jne	halt2

	/* Setup bootloader stack */
	movl	%edx, %esp

	/*
	 * ebp:  FSP_INFO_HEADER address
	 * ecx:  Temp RAM base
	 * edx:  Temp RAM top
	 * edi:  BIST value
	 * esp:  Top of stack in temp RAM
	 * mm0:  low 32-bits of TSC value
	 * mm1:  high 32-bits of TSC value
	 */

	/* Create cache_as_ram_params on stack */
	pushl	%edx	/* bootloader CAR end */
	pushl	%ecx	/* bootloader CAR begin */
	pushl	%ebp	/* FSP_INFO_HEADER */
	pushl	%edi	/* bist */
	movd	%mm1, %eax
	pushl	%eax	/* tsc[63:32] */
	movd	%mm0, %eax
	pushl	%eax	/* tsc[31:0] */
	pushl	%esp	/* pointer to cache_as_ram_params */

	/* Save FSP_INFO_HEADER location in ebx */
	mov	%ebp, %ebx

	/* coreboot assumes stack/heap region will be zero */
	cld
	movl	%ecx, %edi
	neg	%ecx
	/* Only clear up to current stack value. */
	add	%esp, %ecx
	shrl	$2, %ecx
	xorl	%eax, %eax
	rep	stosl

before_romstage:
	post_code(0x2A)

	/* Call cache_as_ram_main(struct cache_as_ram_params *) */
	call	cache_as_ram_main

/* One will never return from cache_as_ram_main() in verstage so there's
 * no such thing as after RAM init. */
#if !ENV_VERSTAGE
#include "after_raminit.S"
#endif

	movb	$0x69, %ah
	jmp	.Lhlt

halt1:
	/*
	 * Failures for postcode 0xBA - failed in fsp_fih_early_find()
	 *
	 * Values are:
	 * 0x01 - FV signature, "_FVH" not present
	 * 0x02 - FFS GUID not present
	 * 0x03 - FSP INFO Header not found
	 * 0x04 - ImageBase does not equal CONFIG_FSP_LOC - Is the FSP rebased to
	 *        a different location, or does it need to be?
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
	 * 0x0E - FSP_NOT_FOUND: No valid microcode was found in the microcode region.
	 * 0x14 - FSP_ALREADY_STARTED: Temp RAM initialization has been invoked
	 */
	movb	$0xBB, %ah
	jmp	.Lhlt

.Lhlt:
	xchg	%al, %ah
#if IS_ENABLED(CONFIG_POST_IO)
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
	.long	CONFIG_CPU_MICROCODE_CBFS_LOC		/* Microcode Location */
	.long	CONFIG_CPU_MICROCODE_CBFS_LEN		/* Microcode Length */
	.long	0xFFFFFFFF - CONFIG_ROM_SIZE + 1	/* Firmware Location */
	.long	CONFIG_ROM_SIZE				/* Total Firmware Length */

CAR_init_stack:
	.long	CAR_init_done
	.long	CAR_init_params
