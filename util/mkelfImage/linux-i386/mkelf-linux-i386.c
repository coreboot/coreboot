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
#include "x86-linux.h"
#include "mkelfImage.h"

static unsigned char payload[] = {
#include "convert.bin.c"
};

struct kernel_info;
static void (*parse_kernel_type)(struct kernel_info *info, char *kernel_buf, size_t kernel_size);
static void parse_bzImage_kernel(struct kernel_info *info, char *kernel_buf, size_t kernel_size);
static void parse_elf32_kernel(struct kernel_info *info, char *kernel_buf, size_t kernel_size);
static void parse_elf64_kernel(struct kernel_info *info, char *kernel_buf, size_t kernel_size);

char *vmlinux_x86_64_probe(char *kernel_buf, off_t kernel_size);

char *vmlinux_i386_probe(char *kernel_buf, off_t kernel_size)
{
	Elf32_Ehdr *ehdr;
	Elf32_Phdr *phdr;
	int i;
	int phdrs;
	ehdr = (Elf32_Ehdr *)kernel_buf;
	if (
		(ehdr->e_ident[EI_MAG0] != ELFMAG0) ||
		(ehdr->e_ident[EI_MAG1] != ELFMAG1) ||
		(ehdr->e_ident[EI_MAG2] != ELFMAG2) ||
		(ehdr->e_ident[EI_MAG3] != ELFMAG3)) {
		return "No ELF signature found on kernel\n";
	}
	if (ehdr->e_ident[EI_CLASS] != ELFCLASS32) {
		return vmlinux_x86_64_probe(kernel_buf, kernel_size);
//		return "Not a 32bit ELF kernel\n";
	}
	if (ehdr->e_ident[EI_DATA]  != ELFDATA2LSB) {
		return "Not a little endian ELF kernel\n";
	}
	if (le16_to_cpu(ehdr->e_type) != ET_EXEC) {
		return "Not an executable kernel\n";
	}
	if (le16_to_cpu(ehdr->e_machine) != EM_386) {
		return "Not an i386 kernel\n";
	}
	if (	(ehdr->e_ident[EI_VERSION] != EV_CURRENT) ||
		(le32_to_cpu(ehdr->e_version) != EV_CURRENT)) {
		return "Kernel not using ELF version 1.\n";
	}
	if (le16_to_cpu(ehdr->e_phentsize) != sizeof(*phdr)) {
		return "Kernel uses bad program header size.\n";
	}
	phdr = (Elf32_Phdr *)(kernel_buf + le32_to_cpu(ehdr->e_phoff));
	phdrs = 0;
	for(i = 0; i < le16_to_cpu(ehdr->e_phnum); i++) {
		if (le32_to_cpu(phdr[i].p_type) != PT_LOAD)
			continue;
		phdrs++;
	}
	if (phdrs == 0) {
		return "No PT_LOAD segments!\n";
	}
	parse_kernel_type = parse_elf32_kernel;
	return 0;
}
char *vmlinux_x86_64_probe(char *kernel_buf, off_t kernel_size)
{
        Elf64_Ehdr *ehdr;
        Elf64_Phdr *phdr;
        int i;
	int phdrs = 0;
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
        if (le16_to_cpu(ehdr->e_machine) != EM_X86_64) {
                return "Not an x86_64 kernel\n";
        }
        if (    (ehdr->e_ident[EI_VERSION] != EV_CURRENT) ||
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
        parse_kernel_type = parse_elf64_kernel;
        return 0;
}

char *bzImage_i386_probe(char *kernel_buf, off_t kernel_size)
{
	struct x86_linux_header *hdr;
	unsigned long offset;
	int setup_sects;
	hdr = (struct x86_linux_header *)kernel_buf;

	if (le16_to_cpu(hdr->boot_sector_magic) != 0xaa55) {
		return "No bootsector magic";
	}
	if (memcmp(hdr->header_magic, "HdrS", 4) != 0) {
		return "Not a linux kernel";
	}

	if (le16_to_cpu(hdr->protocol_version) < 0x202) {
		return "Kernel protcols version before 2.02 not supported";
	}

	setup_sects = hdr->setup_sects;
	if (setup_sects == 0) {
		setup_sects = 4;
	}
	offset = 512 + (512 *setup_sects);
	if (offset > kernel_size) {
		return "Not enough bytes";
	}
	parse_kernel_type = parse_bzImage_kernel;
	return 0;
}

char *linux_i386_probe(char *kernel_buf, off_t kernel_size)
{
	char *result;
	result = "";
	if (result) result = bzImage_i386_probe(kernel_buf, kernel_size);
	if (result) result = vmlinux_i386_probe(kernel_buf, kernel_size);
	if (result) result = bzImage_i386_probe(kernel_buf, kernel_size);
	return result;
}

#define NR_SECTIONS 16

struct kernel_info
{
	int phdrs;
	void *kernel[NR_SECTIONS];
	size_t filesz[NR_SECTIONS];
	size_t memsz[NR_SECTIONS];
	size_t paddr[NR_SECTIONS];
	size_t vaddr[NR_SECTIONS];
	size_t entry;
	size_t switch_64;
	char *version;
};

static void parse_elf32_kernel(struct kernel_info *info, char *kernel_buf, size_t kernel_size)
{
	Elf32_Ehdr *ehdr;
	Elf32_Phdr *phdr;
	int i;
	int phdrs;
	ehdr = (Elf32_Ehdr *)kernel_buf;
	phdr = (Elf32_Phdr *)(kernel_buf + ehdr->e_phoff);
	phdrs = 0;
	for(i = 0; i < le16_to_cpu(ehdr->e_phnum); i++) {
		if (le32_to_cpu(phdr[i].p_type) != PT_LOAD)
			continue;
		if(phdrs == NR_SECTIONS)
			die("NR_SECTIONS is too small\n");
		info->kernel[phdrs]  = kernel_buf + le32_to_cpu(phdr[i].p_offset);
		info->filesz[phdrs]  = le32_to_cpu(phdr[i].p_filesz);
		info->memsz[phdrs]   = le32_to_cpu(phdr[i].p_memsz);
		info->paddr[phdrs]   = le32_to_cpu(phdr[i].p_paddr) & 0xfffffff;
		info->vaddr[phdrs]   = le32_to_cpu(phdr[i].p_vaddr);
		phdrs++;
	}

	if(!phdrs)
		die("We need at least one phdr\n");

	info->phdrs = phdrs;
        info->entry   = le32_to_cpu(ehdr->e_entry);
	info->switch_64   = 0; //not convert from elf64
	info->version = "unknown";
}

static void parse_elf64_kernel(struct kernel_info *info, char *kernel_buf, size_t kernel_size)
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
		if(phdrs == NR_SECTIONS)
			die("NR_SECTIONS is too small\n");
		info->kernel[phdrs]  = kernel_buf + le64_to_cpu(phdr[i].p_offset);
		info->filesz[phdrs]  = le64_to_cpu(phdr[i].p_filesz);
		info->memsz[phdrs]   = le64_to_cpu(phdr[i].p_memsz);
		info->paddr[phdrs]   = le64_to_cpu(phdr[i].p_paddr) & 0xfffffff;
		info->vaddr[phdrs]   = le64_to_cpu(phdr[i].p_vaddr);
		phdrs++;
	}

	if(!phdrs)
		die("We need at least one phdr\n");

	info->phdrs = phdrs;
        info->entry   = le64_to_cpu(ehdr->e_entry);
#if  0
	if (info->entry != info->paddr[0]) {
		info->entry = info->paddr[0]; // we still have startup_32 there
		info->switch_64   = 0; //not convert from elf64
	} else
#endif
		info->switch_64   = 1; //convert from elf64

        info->version = "unknown";
}


static void parse_bzImage_kernel(struct kernel_info *info, char *kernel_buf, size_t kernel_size)
{
	struct x86_linux_header *hdr;
	unsigned long offset;
	int setup_sects;
	hdr = (struct x86_linux_header *)kernel_buf;
	setup_sects = hdr->setup_sects;
	if (setup_sects == 0) {
		setup_sects = 4;
	}
	offset = 512 + (512 *setup_sects);

	info->kernel[0]  = kernel_buf + offset;
	info->filesz[0]  = kernel_size - offset;
	info->memsz[0]   = 0x700000;
	info->paddr[0]   = 0x100000;
	info->vaddr[0]   = 0x100000;
	info->phdrs = 1;
	info->entry   = info->paddr[0];
	info->switch_64   = 0; //not convert from elf64, even later bzImage become elf64, it still includes startup_32
	info->version = kernel_buf + 512 + le16_to_cpu(hdr->kver_addr);
}

static void parse_kernel(struct kernel_info *info, char *kernel_buf, size_t kernel_size)
{
	memset(info, 0, sizeof(*info));
	if (parse_kernel_type) {
		parse_kernel_type(info, kernel_buf, kernel_size);
	}
	else {
		die("Unknown kernel format");
	}
}

void linux_i386_usage(void)
{
	printf(
		"      --command-line=<string> Set the command line to <string>\n"
		"      --append=<string>       Set the command line to <string>\n"
		"      --initrd=<filename>     Set the initrd to <filename>\n"
		"      --ramdisk=<filename>    Set the initrd to <filename>\n"
		"      --ramdisk-base=<addr>   Set the initrd load address to <addr>\n"
		);
	return;
}


#define OPT_CMDLINE        OPT_MAX+0
#define OPT_RAMDISK        OPT_MAX+1
#define OPT_RAMDISK_BASE   OPT_MAX+2

#define DEFAULT_RAMDISK_BASE (8*1024*1024)

int linux_i386_mkelf(int argc, char **argv,
	struct memelfheader *ehdr, char *kernel_buf, off_t kernel_size)
{
	const char *ramdisk, *cmdline;
	unsigned long ramdisk_base;
	char *payload_buf, *ramdisk_buf;
	off_t payload_size, ramdisk_size;
	struct memelfphdr *phdr;
	struct memelfnote *note;
	struct kernel_info kinfo;
	struct image_parameters *params;
	int index;
	int i;

	int opt;
	static const struct option options[] = {
		MKELF_OPTIONS
		{ "command-line",    1, 0, OPT_CMDLINE },
		{ "append",          1, 0, OPT_CMDLINE },
		{ "initrd",          1, 0, OPT_RAMDISK },
		{ "ramdisk",         1, 0, OPT_RAMDISK },
		{ "ramdisk-base",    1, 0, OPT_RAMDISK_BASE },
		{ 0 , 0, 0, 0 },
	};
	static const char short_options[] = MKELF_OPT_STR;

	ramdisk_base = DEFAULT_RAMDISK_BASE;
	ramdisk = 0;
	cmdline="";

	while((opt = getopt_long(argc, argv, short_options, options, 0)) != -1) {
		switch(opt) {
		case '?':
			error("Unknown option %s\n", argv[optind]);
			break;
		case OPT_RAMDISK_BASE:
		{
			char *end;
			unsigned long base;
			base = strtoul(optarg, &end, 0);
			if ((end == optarg) || (*end != '\0')) {
				error("Invalid ramdisk base\n");
			}
			ramdisk_base = base;
		}
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
	ehdr->ei_class  = ELFCLASS32;
	ehdr->ei_data   = ELFDATA2LSB;
	ehdr->e_type    = ET_EXEC;
	ehdr->e_machine = EM_386;

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
		die("Internal error convert_magic %08x != %08x\n",
			params->convert_magic, CONVERT_MAGIC);
	}

	/* Copy the command line */
	strncpy(params->cmdline, cmdline, sizeof(params->cmdline));
	params->cmdline[sizeof(params->cmdline)-1]= '\0';


	/* Add a program header for the note section */
	index = 4;
	index += (kinfo.phdrs - 1);
	index += ramdisk_size ? 1:0;
	phdr = add_program_headers(ehdr, index);

	/* Fill in the program headers*/
	phdr[0].p_type = PT_NOTE;

	/* Fill in the converter program headers */
	phdr[1].p_paddr  = CONVERTLOC;
	phdr[1].p_vaddr  = CONVERTLOC;
	phdr[1].p_filesz = payload_size;
	phdr[1].p_memsz  = payload_size + params->bss_size;
	phdr[1].p_data   = payload;

	/* Reserve space for the REAL MODE DATA segment AND the GDT segment */
	phdr[2].p_paddr  = REAL_MODE_DATA_LOC;
	phdr[2].p_vaddr  = REAL_MODE_DATA_LOC;
	phdr[2].p_filesz = 0;
	if(!kinfo.switch_64)
		phdr[2].p_memsz = (GDTLOC - REAL_MODE_DATA_LOC) + params->gdt_size;
	else
		phdr[2].p_memsz = (PGTLOC - REAL_MODE_DATA_LOC) + params->pgt_size;
	phdr[2].p_data   = 0;

	if( (phdr[1].p_paddr + phdr[1].p_memsz) > phdr[2].p_paddr) {
		die("Internal error: need to increase REAL_MODE_DATA_LOC !\n");
	}

	index = 3;
	/* Put the second kernel frament if present */
	for(i=0;i<kinfo.phdrs;i++) {
		phdr[index].p_paddr  = kinfo.paddr[i];
		phdr[index].p_vaddr  = kinfo.vaddr[i];
		phdr[index].p_filesz = kinfo.filesz[i];
		phdr[index].p_memsz  = kinfo.memsz[i];
		phdr[index].p_data   = kinfo.kernel[i];
		index++;
	}

	/* Put the ramdisk at ramdisk base.
	 */
	params->initrd_start = params->initrd_size = 0;
	if (ramdisk_size) {
		if( (phdr[index-1].p_paddr + phdr[index-1].p_memsz) > ramdisk_base) {
			die("need to increase increase ramdisk_base !\n");
		}

		phdr[index].p_paddr  = ramdisk_base;
		phdr[index].p_vaddr  = ramdisk_base;
		phdr[index].p_filesz = ramdisk_size;
		phdr[index].p_memsz  = ramdisk_size;
		phdr[index].p_data   = ramdisk_buf;
		params->initrd_start = phdr[index].p_paddr;
		params->initrd_size  = phdr[index].p_filesz;
		index++;
	}

	/* Set the start location */
	params->entry = kinfo.entry;
	params->switch_64 = kinfo.switch_64;
	ehdr->e_entry = phdr[1].p_paddr;

	/* Setup the elf notes */
	note = add_notes(ehdr, 3);
	note[0].n_type = EIN_PROGRAM_NAME;
	note[0].n_name = "ELFBoot";
	note[0].n_desc = "Linux";
	note[0].n_descsz = strlen(note[0].n_desc)+1;

	note[1].n_type = EIN_PROGRAM_VERSION;
	note[1].n_name = "ELFBoot";
	note[1].n_desc = kinfo.version;
	note[1].n_descsz = strlen(note[1].n_desc);

	note[2].n_type = EIN_PROGRAM_CHECKSUM;
	note[2].n_name = "ELFBoot";
	note[2].n_desc = 0;
	note[2].n_descsz = 2;

	return 0;
}

