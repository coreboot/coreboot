/* SPDX-License-Identifier: GPL-2.0-or-later */

#if defined __GNUC__

#include <arch/asm.h>

#ifdef __ARMEB__
#define al r1
#define ah r0
#else
#define al r0
#define ah r1
#endif

ENTRY(__ashldi3)
.global __aeabi_llsl
__aeabi_llsl:

	subs	r3, r2, #32
	rsb	ip, r2, #32
	movmi	ah, ah, lsl r2
	movpl	ah, al, lsl r3
 ARM(	orrmi	ah, ah, al, lsr ip	)
 THUMB(	lsrmi	r3, al, ip		)
 THUMB(	orrmi	ah, ah, r3		)
	mov	al, al, lsl r2
	mov	pc, lr

.type __aeabi_llsl, %function
.size __aeabi_llsl, .-__aeabi_llsl
ENDPROC(__ashldi3)

#endif
