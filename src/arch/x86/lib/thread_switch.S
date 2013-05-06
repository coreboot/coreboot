/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2013 Google, Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */
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
