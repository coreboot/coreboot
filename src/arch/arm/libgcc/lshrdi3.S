/* SPDX-License-Identifier: GPL-2.0-or-later */
/* This file is part of the coreboot project. */

#if defined __GNUC__

#include <arch/asm.h>

#ifdef __ARMEB__
#define al r1
#define ah r0
#else
#define al r0
#define ah r1
#endif

ENTRY(__lshrdi3)
.global __aeabi_llsr
__aeabi_llsr:

	subs	r3, r2, #32
	rsb	ip, r2, #32
	movmi	al, al, lsr r2
	movpl	al, ah, lsr r3
 ARM(	orrmi	al, al, ah, lsl ip	)
 THUMB(	lslmi	r3, ah, ip		)
 THUMB(	orrmi	al, al, r3		)
	mov	ah, ah, lsr r2
	mov	pc, lr

.type __aeabi_llsr, %function
.size __aeabi_llsr, .-__aeabi_llsr
ENDPROC(__lshrdi3)

#endif
