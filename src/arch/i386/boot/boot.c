#include <boot/uniform_boot.h>
#include <boot/elf.h>

#ifndef CMD_LINE
#define CMD_LINE ""
#endif

/* FIXME: the current placement of ube_all could lead to problems... 
 * It should be in a location normally reserved for the bios.
 */

static struct {
	struct uniform_boot_header header;
	struct linuxbios_header lb_header;
	struct {
		struct {
			struct ube_memory memory;
			struct ube_memory_range range[2];
		} mem;
	}env;
	unsigned char command_line[1024];
} ube_all = {
	.header = {
		.header_bytes = sizeof(ube_all.header),
		.header_checksum = 0,
		.arg = (unsigned long)&ube_all.command_line,
		.arg_bytes = sizeof(ube_all.command_line),
		.env = (unsigned long)&ube_all.env,
		.env_bytes = sizeof(ube_all.env),
	},
	.lb_header = {
		.signature = { 'L', 'B', 'I', 'O' },
		.header_bytes = sizeof(ube_all.lb_header),
		.header_checksum = 0,
		.env_bytes = sizeof(ube_all.env),
		.env_checksum = 0,
		.env_entries = 0,
	},
	.env = {
		.mem = {
			.memory = {
				.tag = UBE_TAG_MEMORY,
				.size = sizeof(ube_all.env.mem),
			},
			.range = {
#if 0
				{
					.start = 0,
					.size = 0xa0000, /* 640k */
					.type = UBE_MEM_RAM,
				},
#else
				{
					.start = 4096,	/* skip the first page */
					.size = 0x9f000, /* 640k */
					.type = UBE_MEM_RAM,
				},
#endif
				{
					.start = 0x00100000, /* 1M */
					.size = 0, /* Fill in the size */
					.type = UBE_MEM_RAM,
				},
			},
		},
		
	},
	.command_line = CMD_LINE,
};

void *get_ube_pointer(unsigned long totalram)
{
	ube_all.env.mem.range[1].size = ((totalram - 1024) << 10);
	ube_all.header.header_checksum = 0;
	ube_all.header.header_checksum = 
		uniform_boot_compute_header_checksum(&ube_all.header);
	ube_all.lb_header.env_entries = 1; /* FIXME remove this hardcode.. */
	ube_all.lb_header.env_checksum = 
		compute_checksum(&ube_all.env, sizeof(ube_all.env));
	ube_all.lb_header.header_checksum = 
		compute_checksum(&ube_all.lb_header, sizeof(ube_all.lb_header));
	return &ube_all.header;
}

int elf_check_arch(Elf_ehdr *ehdr)
{
	return (
		((ehdr->e_machine == EM_386) ||	(ehdr->e_machine == EM_486)) &&
		(ehdr->e_ident[EI_CLASS] == ELFCLASS32) &&
		(ehdr->e_ident[EI_DATA] == ELFDATA2LSB) 
		);
	
}

void jmp_to_elf_entry(void *entry, void *ube)
{
	unsigned long type = 0x0A11B007;

	/* Jump to kernel */
	__asm__ __volatile__(
		"pushl %0\n\t"
		"pushl %1\n\t"
		"pushl %2\n\t"
		"popl  %%ebx\n\t"
		"popl  %%eax\n\t"
		"ret\n\t"
		:: "g" (entry), "g"(type), "g"(ube));
}


