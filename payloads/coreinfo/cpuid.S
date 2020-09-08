/* SPDX-License-Identifier: GPL-2.0-only */

/* It is derived from the x86info project, which is GPLv2-licensed. */

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
