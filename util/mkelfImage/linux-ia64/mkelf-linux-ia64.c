#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#define _GNU_SOURCE
#include <getopt.h>
#include "elf.h"
#include "elf_boot.h"
#include "convert.h"
#include "mkelfImage.h"

static unsigned char payload[] = {
#include "convert.bin.c"
};

char *linux_ia64_probe(char *kernel_buf, off_t kernel_size)
{
	Elf64_Ehdr *ehdr;
	Elf64_Phdr *phdr;
	int i;
	int phdrs;
	ehdr = (Elf64_Ehdr *)kernel_buf;
	if (
		(ehdr->e_ident[EI_MAG0] != ELFMAG0) ||
		(ehdr->e_ident[EI_MAG1] != ELFMAG1) ||
		(ehdr->e_ident[EI_MAG2] != ELFMAG2) ||
		(ehdr->e_ident[EI_MAG3] != ELFMAG3)) {
		return "No ELF signature found on kernel\n";
	}
	if (ehdr->e_ident[EI_CLASS] != ELFCLASS64) {
		return "Not a 64bit ELF kernel\n";
	}
	if (ehdr->e_ident[EI_DATA]  != ELFDATA2LSB) {
		return "Not a little endian ELF kernel\n";
	}
	if (le16_to_cpu(ehdr->e_type) != ET_EXEC) {
		return "Not an executable kernel\n";
	}
	if (le16_to_cpu(ehdr->e_machine) != EM_IA_64) {
		return "Not an ia64 kernel\n";
	}
	if (	(ehdr->e_ident[EI_VERSION] != EV_CURRENT) ||
		(le32_to_cpu(ehdr->e_version) != EV_CURRENT)) {
		return "Kernel not using ELF version 1.\n";
	}
	if (le16_to_cpu(ehdr->e_phentsize) != sizeof(*phdr)) {
		return "Kernel uses bad program header size.\n";
	}
	phdr = (Elf64_Phdr *)(kernel_buf + le64_to_cpu(ehdr->e_phoff));
	phdrs = 0;
	for(i = 0; i < le16_to_cpu(ehdr->e_phnum); i++) {
		if (le32_to_cpu(phdr[i].p_type) != PT_LOAD)
			continue;
		phdrs++;
	}
	if (phdrs == 0) {
		return "No PT_LOAD segments!\n";
	}
	return 0;
}

struct kernel_info
{
	int phdrs;
	char *kernel_buf;
	Elf64_Ehdr *ehdr;
	Elf64_Phdr *phdr;
	uint64_t entry;
	char *version;
};

static void parse_kernel(struct kernel_info *info,
	char *kernel_buf, size_t kernel_size)
{
	Elf64_Ehdr *ehdr;
	Elf64_Phdr *phdr;
	int i;
	int phdrs;
	ehdr = (Elf64_Ehdr *)kernel_buf;
	phdr = (Elf64_Phdr *)(kernel_buf + le64_to_cpu(ehdr->e_phoff));
	phdrs = 0;
	for(i = 0; i < le16_to_cpu(ehdr->e_phnum); i++) {
		if (le32_to_cpu(phdr[i].p_type) != PT_LOAD)
			continue;
		phdrs++;
	}
	if (phdrs == 0) {
		die("No PT_LOAD segments!\n");
	}
	info->kernel_buf = kernel_buf;
	info->ehdr    = ehdr;
	info->phdrs   = phdrs;
	info->phdr    = phdr;
	info->entry   = le64_to_cpu(ehdr->e_entry);
	info->version = "unknown";
}

static int populate_kernel_phdrs(struct kernel_info *info, struct memelfphdr *phdr)
{
	uint64_t paddr;
	int i;
	paddr = 0;
	for(i = 0; i < info->phdrs; i++) {
		Elf64_Phdr *hdr;
		int j;
		hdr = 0;
		for(j = 0; j < le16_to_cpu(info->ehdr->e_phnum); j++) {
			if (le16_to_cpu(info->phdr[j].p_type != PT_LOAD)) {
				continue;
			}
			if (paddr > le64_to_cpu(info->phdr[j].p_paddr)) {
				continue;
			}
			if (hdr &&
				le64_to_cpu(hdr->p_paddr) <
				le64_to_cpu(info->phdr[j].p_paddr)) {
				continue;
			}
			hdr = info->phdr + j;
		}
		if (!hdr) {
			die("Expected %d phdrs found %d!", info->phdrs, i);
		}
		phdr[i].p_paddr  = le64_to_cpu(hdr->p_paddr);
		phdr[i].p_vaddr  = le64_to_cpu(hdr->p_vaddr);
		phdr[i].p_filesz = le64_to_cpu(hdr->p_filesz);
		phdr[i].p_memsz  = le64_to_cpu(hdr->p_memsz);
		phdr[i].p_data   = info->kernel_buf + le64_to_cpu(hdr->p_offset);
		paddr = phdr[i].p_paddr + phdr[i].p_memsz;
	}
	return i;
}


void linux_ia64_usage(void)
{
	printf(
		"      --command-line=<string> Set the command line to <string>\n"
		"      --append=<string>       Set the command line to <string>\n"
		"      --initrd=<filename>     Set the initrd to <filename>\n"
		"      --ramdisk=<filename>    Set the initrd to <filename>\n"
		);
	return;
}

#define OPT_CMDLINE        OPT_MAX+0
#define OPT_RAMDISK        OPT_MAX+1

int linux_ia64_mkelf(int argc, char **argv,
	struct memelfheader *ehdr, char *kernel_buf, off_t kernel_size)
{
	const char *ramdisk, *cmdline;
	char *payload_buf, *ramdisk_buf;
	off_t payload_size, ramdisk_size;
	struct memelfphdr *phdr;
	struct memelfnote *note;
	struct kernel_info kinfo;
	struct image_parameters *params;
	int index;

	int opt;
	static const struct option options[] = {
		MKELF_OPTIONS
		{ "command-line",    1, 0, OPT_CMDLINE },
		{ "append",          1, 0, OPT_CMDLINE },
		{ "initrd",          1, 0, OPT_RAMDISK },
		{ "ramdisk",         1, 0, OPT_RAMDISK },
		{ 0 , 0, 0, 0 },
	};
	static const char short_options[] = "HV";

	ramdisk = 0;
	cmdline="";

	while((opt = getopt_long(argc, argv, short_options, options, 0)) != -1) {
		switch(opt) {
		case '?':
			error("Unknown option %s\n", argv[optind]);
			break;
		case OPT_RAMDISK:
			ramdisk = optarg;
			break;
		case OPT_CMDLINE:
			cmdline = optarg;
			break;
		default:
			break;
		}
	}
	ehdr->ei_class  = ELFCLASS64;
	ehdr->ei_data   = ELFDATA2LSB;
	ehdr->e_type    = ET_EXEC;
	ehdr->e_machine = EM_IA_64;

	/* locate the payload buffer */
	payload_buf = payload;
	payload_size = sizeof(payload);

	/* slurp the input files */
	ramdisk_buf = slurp_file(ramdisk, &ramdisk_size);

	/* parse the kernel */
	parse_kernel(&kinfo, kernel_buf, kernel_size);

	/* Find the parameters */
	params = (void *)(payload_buf + (payload_size - sizeof(*params)));

	/* A sanity check against bad versions of binutils */
	if (params->convert_magic != CONVERT_MAGIC) {
		die("Internal error convert_magic %16llx != %16llx\n",
			(unsigned long long)(params->convert_magic), CONVERT_MAGIC);
	}

	/* Copy the command line */
	strncpy(params->cmdline, cmdline, sizeof(params->cmdline));
	params->cmdline[sizeof(params->cmdline)-1]= '\0';

	/* Add a program header for the note section */
	phdr = add_program_headers(ehdr, 2 + kinfo.phdrs + (ramdisk_size?1:0));

	/* Fill in the program headers*/
	phdr[0].p_type = PT_NOTE;

	/* Fill in the kernel program headers */
	index = 1 + populate_kernel_phdrs(&kinfo, phdr + 1);

	/* Fill in the converter program header */
	phdr[index].p_paddr  = roundup(phdr[index -1].p_paddr + phdr[index -1].p_memsz, 16);
	phdr[index].p_vaddr  = phdr[index].p_paddr;
	phdr[index].p_filesz = payload_size;
	phdr[index].p_memsz  = payload_size;
	phdr[index].p_data   = payload_buf;
	index++;

	/* Set the start location */
	params->entry = kinfo.entry;
	ehdr->e_entry = phdr[index -1].p_paddr;


	/* Fill in the ramdisk program header */
	params->initrd_start = params->initrd_size = 0;
	if (ramdisk_size) {
		phdr[index].p_paddr  = roundup(phdr[index -1].p_paddr + phdr[index -1].p_memsz, 16);
		phdr[index].p_vaddr  = phdr[index].p_paddr;
		phdr[index].p_filesz = ramdisk_size;
		phdr[index].p_memsz  = ramdisk_size;
		phdr[index].p_data   = ramdisk_buf;
		params->initrd_start = phdr[index].p_paddr;
		params->initrd_size  = phdr[index].p_filesz;
		index++;
	}

	/* Compute the elf notes */
	note = add_notes(ehdr, 3);
	note[0].n_type = EIN_PROGRAM_NAME;
	note[0].n_name = "ELFBoot";
	note[0].n_desc = "Linux";
	note[0].n_descsz = strlen(note[0].n_desc)+1;

	note[1].n_type = EIN_PROGRAM_VERSION;
	note[1].n_name = "ELFBoot";
	note[1].n_desc = kinfo.version;
	note[1].n_descsz = strlen(note[1].n_desc)+1;

	note[2].n_type = EIN_PROGRAM_CHECKSUM;
	note[2].n_name = "ELFBoot";
	note[2].n_desc = 0;
	note[2].n_descsz = 2;

	return 0;
}
