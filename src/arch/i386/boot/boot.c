#include <ip_checksum.h>
#include <boot/elf.h>
#include <boot/elf_boot.h>
#include <string.h>

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

void jmp_to_elf_entry(void *entry)
{
	unsigned long type = 0x0E1FB007;
	elf_boot_notes.hdr.b_checksum = 
		compute_ip_checksum(&elf_boot_notes, sizeof(elf_boot_notes));

	/* Jump to kernel */
	__asm__ __volatile__(
		"pushl %0\n\t"
		"pushl %1\n\t"
		"pushl %2\n\t"
		"popl  %%ebx\n\t"
		"popl  %%eax\n\t"
		"ret\n\t"
		:: "g" (entry), "g"(type), "g"(&elf_boot_notes));
}


