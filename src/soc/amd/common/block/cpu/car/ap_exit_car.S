/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2018 Advanced Micro Devices, Inc.
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

.code32

#include <cpu/x86/cr.h>

.globl ap_teardown_car
ap_teardown_car:
	pop	%esi		/* return address, don't care */
	pop	%esi		/* flags */

	/* chipset_teardown_car() is expected to disable cache-as-ram. */
	call	chipset_teardown_car

	/*
	 * Check flags requirements (0 = FALSE, 1 = TRUE) :
	 * bit 0 = ExecWbinvd
	 * bit 1 = CacheEn
	 */

	/*
	 * TODO: Either use or remove this code - we'll node if the code
	 * is needed when 3 conditions happens:
	 * 1) This code is in place
	 * 2) AGESA code that calls HALT_THIS_AP is in place
	 * 3) We boot to OS, go to S3 and resume.
	 * If S3 resume fails, this code might be needed, if S3 resume
	 * is successful then the code can be removed.
	 */

	/*
	 * Commented out until defined if needed or not.
	 * test	%esi, 1
	 * jz	1f
	 * wbinvd
	 * 1:
	 */

	test	%esi, 2
	jz	2f
	/* Enable cache */
	mov	%cr0, %eax
	and	$(~(CR0_CD | CR0_NW)), %eax
	mov	%eax, %cr0
2:
	cli
	hlt
	jmp	2b
