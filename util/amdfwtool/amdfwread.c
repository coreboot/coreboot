/* SPDX-License-Identifier: GPL-2.0-only */
#include <getopt.h>
#include <stddef.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>
#include "amdfwtool.h"

/* An address can be relative to the image/file start but it can also be the address when
 * the image is mapped at 0xff000000. Used to ensure that we only attempt to read within
 * the limits of the file. */
#define SPI_ROM_BASE 0xff000000
#define FILE_REL_MASK 0xffffff

#define ERR(...) fprintf(stderr, __VA_ARGS__)

/* Possible locations for the header */
const uint32_t fw_header_offsets[] = {
	0xfa0000,
	0xe20000,
	0xc20000,
	0x820000,
	0x020000,
};

/* Converts addresses to be relative to the start of the file */
static uint64_t relative_offset(uint32_t header_offset, uint64_t addr, uint64_t mode)
{
	switch (mode) {
	/* Since this utility operates on the BIOS file, physical address is converted
	   relative to the start of the BIOS file. */
	case AMD_ADDR_PHYSICAL:
		if (addr < SPI_ROM_BASE || addr > (SPI_ROM_BASE + FILE_REL_MASK)) {
			ERR("Invalid address(%lx) or mode(%lx)\n", addr, mode);
			/* TODO: fix amdfwtool to program the right address/mode. In guybrush,
			 * lots of addresses are marked as physical, but they are relative to
			 * BIOS. Until that is fixed, just leave an error message. */
			// exit(1);
		}
		return addr & FILE_REL_MASK;

	case AMD_ADDR_REL_BIOS:
		if (addr > FILE_REL_MASK) {
			ERR("Invalid address(%lx) or mode(%lx)\n", addr, mode);
			exit(1);
		}
		return addr & FILE_REL_MASK;

	case AMD_ADDR_REL_TAB:
		return addr + header_offset;

	default:
		ERR("Unsupported mode %lu\n", mode);
		exit(1);
	}
}

static int read_fw_header(FILE *fw, uint32_t offset, embedded_firmware *fw_header)
{
	if (fseek(fw, offset, SEEK_SET) != 0) {
		ERR("Failed to seek to fw header offset 0x%x\n", offset);
		return 1;
	}

	if (fread(fw_header, sizeof(embedded_firmware), 1, fw) != 1) {
		ERR("Failed to read fw header at 0x%x\n", offset);
		return 1;
	}

	return fw_header->signature != EMBEDDED_FW_SIGNATURE;
}

static int read_psp_directory(FILE *fw, uint32_t offset, uint32_t expected_cookie,
			psp_directory_header *header, psp_directory_entry **entries,
			size_t *num_entries)
{
	offset &= FILE_REL_MASK;

	if (fseek(fw, offset, SEEK_SET) != 0) {
		ERR("Failed to seek to PSP header at 0x%x\n", offset);
		return 1;
	}

	if (fread(header, sizeof(psp_directory_header), 1, fw) != 1) {
		ERR("Failed to read PSP header cookie\n");
		return 1;
	}

	/* Ensure that we have a PSP directory */
	if (header->cookie != expected_cookie) {
		ERR("Invalid PSP header cookie value found: 0x%x, expected: 0x%x\n",
		    header->cookie, expected_cookie);
		return 1;
	}

	/* Read the entries */
	*num_entries = header->num_entries;
	*entries = malloc(sizeof(psp_directory_entry) * header->num_entries);
	if (fread(*entries, sizeof(psp_directory_entry), header->num_entries, fw)
		!= header->num_entries) {
		ERR("Failed to read %d PSP entries\n", header->num_entries);
		return 1;
	}

	return 0;
}

static int read_soft_fuse(FILE *fw, const embedded_firmware *fw_header)
{
	psp_directory_entry *current_entries = NULL;
	size_t num_current_entries = 0;

	uint32_t psp_offset = 0;
	/* 0xffffffff indicates that the offset is in new_psp_directory */
	if (fw_header->psp_directory != 0xffffffff)
		psp_offset = fw_header->psp_directory;
	else
		psp_offset = fw_header->new_psp_directory;

	psp_directory_header header;
	if (read_psp_directory(fw, psp_offset, PSP_COOKIE, &header,
		       &current_entries, &num_current_entries) != 0)
		return 1;

	while (1) {
		uint32_t l2_dir_offset = 0;

		for (size_t i = 0; i < num_current_entries; i++) {
			uint32_t type = current_entries[i].type;
			uint64_t mode = current_entries[i].address_mode;
			uint64_t addr = current_entries[i].addr;

			if (type == AMD_PSP_FUSE_CHAIN) {
				uint64_t fuse = mode << 62 | addr;

				printf("Soft-fuse:0x%lx\n", fuse);
				return 0;
			} else if (type == AMD_FW_L2_PTR) {
				/* There's a second level PSP directory to read */
				if (l2_dir_offset != 0)
					return 1;

				l2_dir_offset = relative_offset(psp_offset, addr, mode);
			}
		}

		free(current_entries);

		/* Didn't find an L2 PSP directory so we can stop */
		if (l2_dir_offset == 0)
			break;

		/* Read the L2 PSP directory */
		if (read_psp_directory(fw, l2_dir_offset, PSPL2_COOKIE, &header,
			       &current_entries, &num_current_entries) != 0)
			break;
	}

	return 1;
}

enum {
	AMDFW_OPT_HELP = 'h',
	AMDFW_OPT_SOFT_FUSE = 1UL << 0, /* Print Softfuse */
};

static char const optstring[] = {AMDFW_OPT_HELP};

static struct option long_options[] = {
	{"help", no_argument, 0, AMDFW_OPT_HELP},
	{"soft-fuse", no_argument, 0, AMDFW_OPT_SOFT_FUSE},
};

static void print_usage(void)
{
	printf("amdfwread: Examine AMD firmware images\n");
	printf("Usage: amdfwread [options] <file>\n");
	printf("--soft-fuse                Print soft fuse value\n");
}

int main(int argc, char **argv)
{
	char *fw_file = NULL;

	int selected_functions = 0;
	while (1) {
		int opt = getopt_long(argc, argv, optstring, long_options, NULL);

		if (opt == -1) {
			if (optind != (argc - 1)) {
				/* Print usage if one and only one option i.e. filename is
				   not found. */
				print_usage();
				return 0;
			}

			fw_file = argv[optind];
			break;
		}

		switch (opt) {
		case AMDFW_OPT_HELP:
			print_usage();
			return 0;

		case AMDFW_OPT_SOFT_FUSE:
			selected_functions |= opt;
			break;

		default:
			break;
		}
	}

	FILE *fw = fopen(fw_file, "rb");
	if (!fw) {
		ERR("Failed to open FW file %s\n", fw_file);
		return 1;
	}

	/* Find the FW header by checking each possible location */
	embedded_firmware fw_header;
	int found_header = 0;
	for (size_t i = 0; i < ARRAY_SIZE(fw_header_offsets); i++) {
		if (read_fw_header(fw, fw_header_offsets[i], &fw_header) == 0) {
			found_header = 1;
			break;
		}
	}

	if (!found_header) {
		ERR("Failed to find FW header\n");
		fclose(fw);
		return 1;
	}

	if (selected_functions & AMDFW_OPT_SOFT_FUSE) {
		if (read_soft_fuse(fw, &fw_header) != 0) {
			fclose(fw);
			return 1;
		}
	}

	fclose(fw);
	return 0;
}
