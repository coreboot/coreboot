/*
 * This file is part of the coreboot project.
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

#include <commonlib/helpers.h>
#include <console/console.h>
#include <arch/stages.h>
#include <program_loading.h>
#include <ip_checksum.h>
#include <string.h>
#include <symbols.h>

/* When the ramstage is relocatable the elf loading ensures an elf image cannot
 * be loaded over the ramstage code. */
static void jmp_payload_no_bounce_buffer(void *entry)
{
	/* Jump to kernel */
	__asm__ __volatile__(
		"	cld\n\t"
		/* Now jump to the loaded image */
		"	call	*%0\n\t"

		/* The loaded image returned? */
		"	cli\n\t"
		"	cld\n\t"

		::
		"r" (entry)
		);
}

static void jmp_payload(void *entry, unsigned long buffer, unsigned long size)
{
	unsigned long lb_start, lb_size;

	lb_start = (unsigned long)&_program;
	lb_size = _program_size;

	printk(BIOS_SPEW, "entry    = 0x%08lx\n", (unsigned long)entry);
	printk(BIOS_SPEW, "lb_start = 0x%08lx\n", lb_start);
	printk(BIOS_SPEW, "lb_size  = 0x%08lx\n", lb_size);
	printk(BIOS_SPEW, "buffer   = 0x%08lx\n", buffer);

	/* Jump to kernel */
	__asm__ __volatile__(
		"	cld\n\t"
#ifdef __x86_64__
		/* switch back to 32-bit mode */
		"       push    %4\n\t"
		"       push    %3\n\t"
		"       push    %2\n\t"
		"       push    %1\n\t"
		"       push    %0\n\t"

		/* use iret to switch to 32-bit code segment */
		"       xor     %%rax,%%rax\n\t"
		"       mov     %%ss, %%ax\n\t"
		"       push    %%rax\n\t"
		"       mov     %%rsp, %%rax\n\t"
		"       add     $8, %%rax\n\t"
		"       push    %%rax\n\t"
		"       pushfq\n\t"
		"       push    $0x10\n\t"
		"       lea     3(%%rip), %%rax\n\t"
		"       push    %%rax\n\t"
		"       iretq\n\t"
		".code32\n\t"
		/* disable paging */
		"       mov     %%cr0, %%eax\n\t"
		"       btc     $31, %%eax\n\t"
		"       mov     %%eax, %%cr0\n\t"
		/* disable long mode */
		"       mov     $0xC0000080, %%ecx\n\t"
		"       rdmsr\n\t"
		"       btc     $8, %%eax\n\t"
		"       wrmsr\n\t"

		"       pop     %%eax\n\t"
		"       add     $4, %%esp\n\t"
		"       pop     %%ebx\n\t"
		"       add     $4, %%esp\n\t"
		"       pop     %%ecx\n\t"

		"       add     $4, %%esp\n\t"
		"       pop     %%edx\n\t"
		"       add     $4, %%esp\n\t"
		"       pop     %%esi\n\t"
		"       add     $4, %%esp\n\t"
#endif

		/* Save the callee save registers... */
		"	pushl	%%esi\n\t"
		"	pushl	%%edi\n\t"
		"	pushl	%%ebx\n\t"
		/* Save the parameters I was passed */
#ifdef __x86_64__
		"	pushl	$0\n\t"    /* 20 adjust */
		"	pushl	%%eax\n\t" /* 16 lb_start */
		"	pushl	%%ebx\n\t" /* 12 buffer */
		"	pushl	%%ecx\n\t" /*  8 lb_size */
		"	pushl	%%edx\n\t" /*  4 entry */
		"	pushl	%%esi\n\t" /*  0 elf_boot_notes */
#else
		"	pushl	$0\n\t" /* 20 adjust */
		"	pushl	%0\n\t" /* 16 lb_start */
		"	pushl	%1\n\t" /* 12 buffer */
		"	pushl	%2\n\t" /*  8 lb_size */
		"	pushl	%3\n\t" /*  4 entry */
		"	pushl	%4\n\t" /*  0 elf_boot_notes */

#endif
		/* Compute the adjustment */
		"	xorl	%%eax, %%eax\n\t"
		"	subl	16(%%esp), %%eax\n\t"
		"	addl	12(%%esp), %%eax\n\t"
		"	addl	 8(%%esp), %%eax\n\t"
		"	movl	%%eax, 20(%%esp)\n\t"
		/* Place a copy of coreboot in its new location */
		/* Move ``longs'' the coreboot size is 4 byte aligned */
		"	movl	12(%%esp), %%edi\n\t"
		"	addl	 8(%%esp), %%edi\n\t"
		"	movl	16(%%esp), %%esi\n\t"
		"	movl	 8(%%esp), %%ecx\n\n"
		"	shrl	$2, %%ecx\n\t"
		"	rep	movsl\n\t"

		/* Adjust the stack pointer to point into the new coreboot
		 * image
		 */
		"	addl	20(%%esp), %%esp\n\t"
		/* Adjust the instruction pointer to point into the new coreboot
		 * image
		 */
		"	movl	$1f, %%eax\n\t"
		"	addl	20(%%esp), %%eax\n\t"
		"	jmp	*%%eax\n\t"
		"1:\n\t"

		/* Copy the coreboot bounce buffer over coreboot */
		/* Move ``longs'' the coreboot size is 4 byte aligned */
		"	movl	16(%%esp), %%edi\n\t"
		"	movl	12(%%esp), %%esi\n\t"
		"	movl	 8(%%esp), %%ecx\n\t"
		"	shrl	$2, %%ecx\n\t"
		"	rep	movsl\n\t"

		/* Now jump to the loaded image */
		"	movl	%5, %%eax\n\t"
		"	movl	 0(%%esp), %%ebx\n\t"
		"	call	*4(%%esp)\n\t"

		/* The loaded image returned? */
		"	cli\n\t"
		"	cld\n\t"

		/* Copy the saved copy of coreboot where coreboot runs */
		/* Move ``longs'' the coreboot size is 4 byte aligned */
		"	movl	16(%%esp), %%edi\n\t"
		"	movl	12(%%esp), %%esi\n\t"
		"	addl	 8(%%esp), %%esi\n\t"
		"	movl	 8(%%esp), %%ecx\n\t"
		"	shrl	$2, %%ecx\n\t"
		"	rep	movsl\n\t"

		/* Adjust the stack pointer to point into the old coreboot
		 * image
		 */
		"	subl	20(%%esp), %%esp\n\t"

		/* Adjust the instruction pointer to point into the old coreboot
		 * image
		 */
		"	movl	$1f, %%eax\n\t"
		"	subl	20(%%esp), %%eax\n\t"
		"	jmp	*%%eax\n\t"
		"1:\n\t"

		/* Drop the parameters I was passed */
		"	addl	$24, %%esp\n\t"

		/* Restore the callee save registers */
		"	popl	%%ebx\n\t"
		"	popl	%%edi\n\t"
		"	popl	%%esi\n\t"
#ifdef __x86_64__
		".code64\n\t"
#endif
		::
		"ri" (lb_start), "ri" (buffer), "ri" (lb_size),
		"ri" (entry),
		"ri"(0), "ri" (0)
		);
}

int arch_supports_bounce_buffer(void)
{
	return 1;
}

int payload_arch_usable_ram_quirk(uint64_t start, uint64_t size)
{
	if (start < 1 * MiB && (start + size) <= 1 * MiB) {
		printk(BIOS_DEBUG,
			"Payload being loaded at below 1MiB without region being marked as RAM usable.\n");
		return 1;
	}

	return 0;
}

static void try_payload(struct prog *prog)
{
	if (prog_type(prog) == PROG_PAYLOAD) {
		if (IS_ENABLED(CONFIG_RELOCATABLE_RAMSTAGE))
			jmp_payload_no_bounce_buffer(prog_entry(prog));
		else
			jmp_payload(prog_entry(prog),
					(uintptr_t)prog_start(prog),
					prog_size(prog));
	}
}

void arch_prog_run(struct prog *prog)
{
	if (ENV_RAMSTAGE)
		try_payload(prog);
	__asm__ volatile (
#ifdef __x86_64__
		"jmp  *%%rdi\n"
#else
		"jmp  *%%edi\n"
#endif

		:: "D"(prog_entry(prog))
	);
}
