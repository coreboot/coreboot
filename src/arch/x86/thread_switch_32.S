/* SPDX-License-Identifier: GPL-2.0-only */

#if ENV_X86_64
#error COOP_MULTITASKING does not currently support x86_64
#endif

.code32
.text

/*
 * stack layout after pushad:
 * +------------+
 * | save stack | <-- esp + 0x28
 * +------------+
 * |  new stack | <-- esp + 0x24
 * +------------+
 * |  ret addr  | <-- esp + 0x20
 * +------------+
 * |    eax     | <-- esp + 0x1c
 * +------------+
 * |    ecx     | <-- esp + 0x18
 * +------------+
 * |    edx     | <-- esp + 0x14
 * +------------+
 * |    ebx     | <-- esp + 0x10
 * +------------+
 * |  orig esp  | <-- esp + 0x0c
 * +------------+
 * |    ebp     | <-- esp + 0x08
 * +------------+
 * |    esi     | <-- esp + 0x04
 * +------------+
 * |    edi     | <-- esp + 0x00
 * +------------+
 */
.globl switch_to_thread
switch_to_thread:
	pusha
	/* Save the current stack */
	movl	0x28(%esp), %ebx
	movl	%esp, (%ebx)
	/* Switch to the new stack. */
	movl	0x24(%esp), %eax
	movl	%eax, %esp
	popa
	ret
