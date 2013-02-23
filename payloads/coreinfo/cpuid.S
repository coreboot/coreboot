/*
 * This file is part of the coreinfo project.
 *
 * It is derived from the x86info project, which is GPLv2-licensed.
 *
 * Copyright (C) 2001-2007 Dave Jones <davej@codemonkey.org.uk>
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

/* calling syntax:  docpuid(idx,eax,ebx,ecx,edx) */

.align 4
.text

.global docpuid
	.type docpuid,@function

docpuid:
	pushl %ebp
	movl %esp, %ebp
	pushl %edi
	pushl %ebx
	pushl %ecx
	pushl %edx
	movl 8(%ebp),%eax
	cpuid
	movl 12(%ebp),%edi
	test %edi,%edi
	jz L1
	movl %eax, (%edi)

L1:	movl 16(%ebp),%edi
	test %edi,%edi
	jz L2
	movl %ebx,(%edi)

L2:	movl 20(%ebp),%edi
	test %edi,%edi
	jz L3
	movl %ecx,(%edi)

L3:	movl 24(%ebp), %edi
	test %edi,%edi
	jz L4
	movl %edx,(%edi)

L4:	popl %edx
	popl %ecx
	popl %ebx
	popl %edi
	movl %ebp,%esp
	popl %ebp
	ret

.section .note.GNU-stack,"", @progbits
