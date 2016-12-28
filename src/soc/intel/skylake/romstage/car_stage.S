/*
 * This file is part of the coreboot project.
 *
 * Copyright 2015 Google Inc
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

/* I/O delay between post codes on failure */
#define LHLT_DELAY	0x50000

.text
.global car_stage_entry
car_stage_entry:
	call	romstage_c_entry
	#include "src/drivers/intel/fsp1_1/after_raminit.S"


	movb	$0x69, %ah
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
