/* Miscellaneous BPABI functions.

   Copyright (C) 2003, 2004, 2007, 2008  Free Software Foundation, Inc.
   Contributed by CodeSourcery, LLC.

   This file is free software; you can redistribute it and/or modify it
   under the terms of the GNU General Public License as published by the
   Free Software Foundation; either version 2, or (at your option) any
   later version.

   In addition to the permissions in the GNU General Public License, the
   Free Software Foundation gives you unlimited permission to link the
   compiled version of this file into combinations with other programs,
   and to distribute those combinations without any restriction coming
   from the use of this file.  (The General Public License restrictions
   do apply in other respects; for example, they cover modification of
   the file, and distribution when not linked into a combine
   executable.)

   This file is distributed in the hope that it will be useful, but
   WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; see the file COPYING.  If not, write to
   the Free Software Foundation, 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.  */


#include <arch/asm.h>

#ifdef __ARMEB__
#define xxh r0
#define xxl r1
#define yyh r2
#define yyl r3
#else
#define xxh r1
#define xxl r0
#define yyh r3
#define yyl r2
#endif

#if defined __thumb2__

.macro do_it cond
	it	\cond
.endm
#define do_push push
#define do_pop pop

#else

.macro do_it cond
.endm
#define do_push stmfd sp!,
#define do_pop ldmfd sp!,

#endif

ENTRY(__aeabi_lcmp)

	cmp	xxh, yyh
	do_it	lt
	movlt	r0, #-1
	do_it	gt
	movgt	r0, #1
	do_it	ne
	movne	pc, lr
	subs	r0, xxl, yyl
	do_it	lo
	movlo	r0, #-1
	do_it	hi
	movhi	r0, #1
	mov	pc, lr

ENDPROC(__aeabi_lcmp)

ENTRY(__aeabi_ulcmp)

	cmp	xxh, yyh
	do_it	lo
	movlo	r0, #-1
	do_it	hi
	movhi	r0, #1
	do_it	ne
	movne	pc, lr
	cmp	xxl, yyl
	do_it	lo
	movlo	r0, #-1
	do_it	hi
	movhi	r0, #1
	do_it	eq
	moveq	r0, #0
	mov	pc, lr

ENDPROC(__aeabi_ulcmp)

ENTRY(__aeabi_ldivmod)
	sub sp, sp, #8
#if defined(__thumb2__)
	mov ip, sp
	push {ip, lr}
#else
	do_push {sp, lr}
#endif
	bl __gnu_ldivmod_helper
	ldr lr, [sp, #4]
	add sp, sp, #8
	do_pop {r2, r3}
	mov	pc, lr

ENDPROC(__aeabi_ldivmod)

ENTRY(__aeabi_uldivmod)
	sub sp, sp, #8
#if defined(__thumb2__)
	mov ip, sp
	push {ip, lr}
#else
	do_push {sp, lr}
#endif
	bl __gnu_uldivmod_helper
	ldr lr, [sp, #4]
	add sp, sp, #8
	do_pop {r2, r3}
	mov	pc, lr

ENDPROC(__aeabi_uldivmod)
