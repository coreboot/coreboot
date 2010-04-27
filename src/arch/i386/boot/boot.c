#include <console/console.h>
#include <ip_checksum.h>
#include <boot/elf.h>
#include <boot/elf_boot.h>
#include <string.h>
#include <cpu/x86/multiboot.h>


#ifndef CMD_LINE
#define CMD_LINE ""
#endif



#define UPSZ(X) ((sizeof(X) + 3) &~3)

static struct {
	Elf_Bhdr hdr;
	Elf_Nhdr ft_hdr;
	unsigned char ft_desc[UPSZ(FIRMWARE_TYPE)];
	Elf_Nhdr bl_hdr;
	unsigned char bl_desc[UPSZ(BOOTLOADER)];
	Elf_Nhdr blv_hdr;
	unsigned char blv_desc[UPSZ(BOOTLOADER_VERSION)];
	Elf_Nhdr cmd_hdr;
	unsigned char cmd_desc[UPSZ(CMD_LINE)];
} elf_boot_notes = {
	.hdr = {
		.b_signature = 0x0E1FB007,
		.b_size = sizeof(elf_boot_notes),
		.b_checksum = 0,
		.b_records = 4,
	},
	.ft_hdr = {
		.n_namesz = 0,
		.n_descsz = sizeof(FIRMWARE_TYPE),
		.n_type = EBN_FIRMWARE_TYPE,
	},
	.ft_desc = FIRMWARE_TYPE,
	.bl_hdr = {
		.n_namesz = 0,
		.n_descsz = sizeof(BOOTLOADER),
		.n_type = EBN_BOOTLOADER_NAME,
	},
	.bl_desc = BOOTLOADER,
	.blv_hdr = {
		.n_namesz = 0,
		.n_descsz = sizeof(BOOTLOADER_VERSION),
		.n_type = EBN_BOOTLOADER_VERSION,
	},
	.blv_desc = BOOTLOADER_VERSION,
	.cmd_hdr = {
		.n_namesz = 0,
		.n_descsz = sizeof(CMD_LINE),
		.n_type = EBN_COMMAND_LINE,
	},
	.cmd_desc = CMD_LINE,
};


int elf_check_arch(Elf_ehdr *ehdr)
{
	return (
		((ehdr->e_machine == EM_386) ||	(ehdr->e_machine == EM_486)) &&
		(ehdr->e_ident[EI_CLASS] == ELFCLASS32) &&
		(ehdr->e_ident[EI_DATA] == ELFDATA2LSB)
		);

}

void jmp_to_elf_entry(void *entry, unsigned long buffer, unsigned long size)
{
	extern unsigned char _ram_seg, _eram_seg;
	unsigned long lb_start, lb_size;
	unsigned long adjust, adjusted_boot_notes;

	elf_boot_notes.hdr.b_checksum =
		compute_ip_checksum(&elf_boot_notes, sizeof(elf_boot_notes));

	lb_start = (unsigned long)&_ram_seg;
	lb_size = (unsigned long)(&_eram_seg - &_ram_seg);
	adjust = buffer +  size - lb_start;

	adjusted_boot_notes = (unsigned long)&elf_boot_notes;
	adjusted_boot_notes += adjust;

	printk(BIOS_SPEW, "entry    = 0x%08lx\n", (unsigned long)entry);
	printk(BIOS_SPEW, "lb_start = 0x%08lx\n", lb_start);
	printk(BIOS_SPEW, "lb_size  = 0x%08lx\n", lb_size);
	printk(BIOS_SPEW, "adjust   = 0x%08lx\n", adjust);
	printk(BIOS_SPEW, "buffer   = 0x%08lx\n", buffer);
	printk(BIOS_SPEW, "     elf_boot_notes = 0x%08lx\n", (unsigned long)&elf_boot_notes);
	printk(BIOS_SPEW, "adjusted_boot_notes = 0x%08lx\n", adjusted_boot_notes);

	/* Jump to kernel */
	__asm__ __volatile__(
		"	cld	\n\t"
		/* Save the callee save registers... */
		"	pushl	%%esi\n\t"
		"	pushl	%%edi\n\t"
		"	pushl	%%ebx\n\t"
		/* Save the parameters I was passed */
		"	pushl	$0\n\t" /* 20 adjust */
	        "	pushl	%0\n\t" /* 16 lb_start */
		"	pushl	%1\n\t" /* 12 buffer */
		"	pushl	%2\n\t" /*  8 lb_size */
		"	pushl	%3\n\t" /*  4 entry */
		"	pushl	%4\n\t" /*  0 elf_boot_notes */
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

		/* Adjust the stack pointer to point into the new coreboot image */
		"	addl	20(%%esp), %%esp\n\t"
		/* Adjust the instruction pointer to point into the new coreboot image */
		"	movl	$1f, %%eax\n\t"
		"	addl	20(%%esp), %%eax\n\t"
		"	jmp	*%%eax\n\t"
		"1:	\n\t"

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
		"	cli	\n\t"
		"	cld	\n\t"

		/* Copy the saved copy of coreboot where coreboot runs */
		/* Move ``longs'' the coreboot size is 4 byte aligned */
		"	movl	16(%%esp), %%edi\n\t"
		"	movl	12(%%esp), %%esi\n\t"
		"	addl	 8(%%esp), %%esi\n\t"
		"	movl	 8(%%esp), %%ecx\n\t"
		"	shrl	$2, %%ecx\n\t"
		"	rep	movsl\n\t"

		/* Adjust the stack pointer to point into the old coreboot image */
		"	subl	20(%%esp), %%esp\n\t"

		/* Adjust the instruction pointer to point into the old coreboot image */
		"	movl	$1f, %%eax\n\t"
		"	subl	20(%%esp), %%eax\n\t"
		"	jmp	*%%eax\n\t"
		"1:	\n\t"

		/* Drop the parameters I was passed */
		"	addl	$24, %%esp\n\t"

		/* Restore the callee save registers */
		"	popl	%%ebx\n\t"
		"	popl	%%edi\n\t"
		"	popl	%%esi\n\t"

		::
		"ri" (lb_start), "ri" (buffer), "ri" (lb_size),
		"ri" (entry),
#if CONFIG_MULTIBOOT
		"ri"(mbi), "ri" (MB_MAGIC2)
#else
		"ri"(adjusted_boot_notes), "ri" (0x0E1FB007)
#endif
		);
}


