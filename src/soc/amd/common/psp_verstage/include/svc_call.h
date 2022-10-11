/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef PSP_VERSTAGE_SVC_CALL_H
#define PSP_VERSTAGE_SVC_CALL_H

#define SVC_CALL4(SVC_ID, R0, R1, R2, R3, Ret) \
	__asm__ __volatile__ ( \
	"mov r0, %[reg0]\n\t" \
	"mov r1, %[reg1]\n\t" \
	"mov r2, %[reg2]\n\t" \
	"mov r3, %[reg3]\n\t" \
	"svc %[id]\n\t" \
	"mov %[result], r0\n\t" \
	: [result] "=r" (Ret) /* output */ \
	: [id] "i" (SVC_ID), [reg0] "r" (R0), [reg1] "r" (R1), [reg2] "r" (R2), \
		[reg3] "r" (R3) /* input(s) */ \
	: "r0", "r1", "r2", "r3", "memory", "cc" /* list of clobbered registers */)

#define SVC_CALL3(SVC_ID, R0,  R1, R2, Ret) \
	__asm__ __volatile__ ( \
	"mov r0, %[reg0]\n\t" \
	"mov r1, %[reg1]\n\t" \
	"mov r2, %[reg2]\n\t" \
	"svc %[id]\n\t" \
	"mov %[result], r0\n\t" \
	: [result] "=r" (Ret) /* output */ \
	: [id] "i" (SVC_ID), [reg0] "r" (R0), [reg1] "r" (R1), [reg2] "r" (R2) \
	: "r0", "r1", "r2", "memory", "cc" /* list of clobbered registers */)

#define SVC_CALL2(SVC_ID, R0, R1, Ret) \
	__asm__ __volatile__ ( \
	"mov r0, %[reg0]\n\t" \
	"mov r1, %[reg1]\n\t" \
	"svc %[id]\n\t" \
	"mov %[result], r0\n\t" \
	: [result] "=r" (Ret) /* output */ \
	: [id] "i" (SVC_ID), [reg0] "r" (R0), [reg1] "r" (R1)/* input(s) */ \
	: "r0", "r1", "memory", "cc" /* list of clobbered registers */)

#define SVC_CALL1(SVC_ID, R0, Ret) \
	__asm__ __volatile__ ( \
	"mov r0, %[reg0]\n\t" \
	"svc %[id]\n\t" \
	"mov %[result], r0\n\t" \
	: [result] "=r" (Ret) /* output */ \
	: [id] "i" (SVC_ID), [reg0] "r" (R0) /* input(s) */ \
	: "r0", "memory", "cc" /* list of clobbered registers */)

#define SVC_CALL0(SVC_ID, Ret) \
	__asm__ __volatile__ ( \
	"svc %[id]\n\t" \
	"mov %[result], r0\n\t" \
	: [result] "=r" (Ret) /* output */ \
	: [id] "I" (SVC_ID) /* input(s) */ \
	: "memory", "cc" /* list of clobbered registers */)

#endif /* PSP_VERSTAGE_SVC_CALL_H */
