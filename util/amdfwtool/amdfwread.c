/* SPDX-License-Identifier: GPL-2.0-only */
#include <getopt.h>
#include <stddef.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "amdfwtool.h"

#define ERR(...) fprintf(stderr, __VA_ARGS__)

enum spi_frequency {
	SPI_FREQUENCY_66_66MHZ = 0,
	SPI_FREQUENCY_33_33MHZ = 1,
	SPI_FREQUENCY_22_22MHZ = 2,
	SPI_FREQUENCY_16_66MHZ = 3,
	SPI_FREQUENCY_100MHZ = 4,
	SPI_FREQUENCY_800KHZ = 5,
};

enum spi_read_mode {
	SPI_READ_MODE_NORMAL_33 = 0,
	SPI_READ_MODE_RESERVED = 1,
	SPI_READ_MODE_DUAL_1_1_2 = 2,
	SPI_READ_MODE_QUAD_1_1_4 = 3,
	SPI_READ_MODE_DUAL_1_2_2 = 4,
	SPI_READ_MODE_QUAD_1_4_4 = 5,
	SPI_READ_MODE_NORMAL_66 = 6,
	SPI_READ_MODE_FAST = 7,
};

/* Possible locations for the header */
const uint32_t fw_header_offsets[] = {
	0xfa0000,
	0xf20000,
	0xe20000,
	0xc20000,
	0x820000,
	0x020000,
};

/* Possible locations for the header on 8 MiB ROM */
const uint32_t fw_header_offsets_8MiB[] = {
	0x7A0000,
	0x720000,
	0x620000,
	0x420000,
	0x020000,
};

/* Possible locations for the header on 4 MiB ROM */
const uint32_t fw_header_offsets_4MiB[] = {
	0x3A0000,
	0x320000,
	0x220000,
	0x020000,
};

/* Size of the FW in bytes */
static uint32_t rom_size;

static uint32_t file_rel_mask(uint32_t addr)
{
	return addr & (rom_size - 1);
}

/* Converts addresses to be relative to the start of the file */
static uint64_t relative_offset(uint32_t header_offset, uint64_t addr, uint64_t mode)
{
	switch (mode) {
	/* Since this utility operates on the BIOS file, physical address is converted
	   relative to the start of the BIOS file. */
	case AMD_ADDR_PHYSICAL:
		if (addr < MAX(SPI_ROM_BASE, 0xffffffff - rom_size + 1) ||
		    addr > 0xffffffff) {
			ERR("Invalid address(%lx) or mode(%lx)\n", addr, mode);
			exit(1);
		}
		return file_rel_mask(addr);

	case AMD_ADDR_REL_BIOS:
		if (addr >= rom_size) {
			ERR("Invalid address(%lx) or mode(%lx)\n", addr, mode);
			exit(1);
		}
		return file_rel_mask(addr);

	case AMD_ADDR_REL_TAB:
		return addr + header_offset;

	default:
		ERR("Unsupported mode %lu\n", mode);
		exit(1);
	}
}

static int read_header(FILE *fw, uint32_t offset, void *header, size_t header_size)
{
	if (fseek(fw, offset, SEEK_SET) != 0) {
		ERR("Failed to seek to file offset 0x%x\n", offset);
		return 1;
	}

	if (fread(header, header_size, 1, fw) != 1) {
		ERR("Failed to read header at 0x%x\n", offset);
		return 1;
	}

	return 0;
}

static int read_fw_header(FILE *fw, uint32_t offset, embedded_firmware *fw_header)
{
	if (read_header(fw, offset, fw_header, sizeof(embedded_firmware))) {
		ERR("Failed to read fw header at 0x%x\n", offset);
		return 1;
	}

	return fw_header->signature != EMBEDDED_FW_SIGNATURE;
}

/* Returns true if there's a PSP directory with the expected cookie */
static bool test_if_psp_directory(FILE *fw, uint32_t offset, uint32_t expected_cookie)
{
	psp_directory_header header;
	offset = file_rel_mask(offset);

	if (read_header(fw, offset, &header, sizeof(psp_directory_header))) {
		ERR("Failed to read PSP header\n");
		return false;
	}
	return header.cookie == expected_cookie;
}

static int read_psp_directory(FILE *fw, uint32_t offset, uint32_t expected_cookie,
			psp_directory_header *header, psp_directory_entry **entries,
			size_t *num_entries)
{
	offset = file_rel_mask(offset);

	if (read_header(fw, offset, header, sizeof(psp_directory_header))) {
		ERR("Failed to read PSP header\n");
		return 1;
	}

	/* Ensure that we have a PSP directory */
	if (header->cookie != expected_cookie) {
		ERR("Invalid PSP header cookie value found: 0x%x, expected: 0x%x\n",
		    header->cookie, expected_cookie);
		return 1;
	}

	if (!entries || !num_entries)
		return 0;

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

static int read_ish_directory(FILE *fw, uint32_t offset, ish_directory_table *table)
{
	return read_header(fw, file_rel_mask(offset), table, sizeof(*table));
}

static int read_bios_directory(FILE *fw, uint32_t offset, uint32_t expected_cookie,
			bios_directory_hdr *header, bios_directory_entry **entries,
			size_t *num_entries)
{
	offset = file_rel_mask(offset);

	if (read_header(fw, offset, header, sizeof(bios_directory_hdr))) {
		ERR("Failed to read BIOS header\n");
		return 1;
	}

	/* Ensure that we have a BIOS directory */
	if (header->cookie != expected_cookie) {
		ERR("Invalid BIOS header cookie value found: 0x%x, expected: 0x%x\n",
			header->cookie, expected_cookie);
		return 1;
	}

	if (!entries || !num_entries)
		return 0;

	/* Read the entries */
	*num_entries = header->num_entries;
	*entries = malloc(sizeof(bios_directory_entry) * header->num_entries);
	if (fread(*entries, sizeof(bios_directory_entry), header->num_entries, fw)
		!= header->num_entries) {
		ERR("Failed to read %d BIOS entries\n", header->num_entries);
		return 1;
	}

	return 0;
}

static int read_soft_fuse(FILE *fw, const embedded_firmware *fw_header)
{
	psp_directory_entry *current_entries = NULL;
	size_t num_current_entries = 0;

	uint32_t psp_offset = 0;
	/* 0xffffffff or 0x00000000 indicates that the offset is in new_psp_directory */
	if (fw_header->psp_directory != 0xffffffff && fw_header->psp_directory != 0x00000000)
		psp_offset = fw_header->psp_directory;
	else
		psp_offset = fw_header->new_psp_directory;

	psp_directory_header header;
	if (read_psp_directory(fw, psp_offset, PSP_COOKIE, &header,
		       &current_entries, &num_current_entries) != 0)
		return 1;

	while (1) {
		uint32_t l2_dir_offset = 0;
		uint32_t ish_dir_offset;
		ish_directory_table ish_dir;

		for (size_t i = 0; i < num_current_entries; i++) {
			uint32_t type = current_entries[i].type;
			uint64_t mode = current_entries[i].address_mode;
			uint64_t addr = current_entries[i].addr;
			uint64_t fuse;

			switch (type) {
			case AMD_FW_PSP_FUSE_CHAIN:
				fuse = mode << 62 | addr;

				printf("Soft-fuse:0x%lx\n", fuse);
				free(current_entries);
				return 0;

			case AMD_FW_PSP_L2_PTR:
				/* There's a second level PSP directory to read */
				if (l2_dir_offset != 0) {
					ERR("Duplicate PSP L2 Entry, prior offset: %08x\n",
										l2_dir_offset);
					free(current_entries);
					return 1;
				}

				l2_dir_offset = relative_offset(psp_offset, addr, mode);
				break;

			case AMD_FW_PSP_RECOVERYAB_A:
				if (l2_dir_offset != 0) {
					ERR("Duplicate PSP L2 Entry, prior offset: %08x\n",
										l2_dir_offset);
					free(current_entries);
					return 1;
				}

				ish_dir_offset = relative_offset(psp_offset, addr, AMD_ADDR_REL_BIOS);
				if (read_ish_directory(fw, ish_dir_offset, &ish_dir) != 0) {
					ERR("Error reading ISH directory\n");
					free(current_entries);
					return 1;
				}

				l2_dir_offset = ish_dir.pl2_location;
				break;

			default:
				/* No-op, continue to the next entry. */
				break;
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

#define MAX_NUM_LEVELS 10
#define MAX_INDENT_PER_LEVEL 4
#define MAX_INDENTATION_LEN (MAX_NUM_LEVELS * MAX_INDENT_PER_LEVEL + 1)

static void dump_directory_header(psp_directory_header *header, uint8_t level)
{
	char indent[MAX_INDENTATION_LEN] = {0};

	for (uint8_t i = 0; i < level && i < MAX_NUM_LEVELS; i++)
		strcat(indent, "    ");

	printf("%sHeader:", indent);
	printf("\n%s  Checksum:              %08X",
	       indent, header->checksum);
	printf("\n%s  Number of entries:     %u",
	       indent, header->num_entries);
	printf("\n%s  Additional Info:       %08x",
	       indent, header->additional_info);
	printf("\n%s    Version:             %u",
	       indent, header->additional_info_fields.version);
	if (header->additional_info_fields.version == 1) {
		printf("\n%s    Directory size:      %x",
		       indent, header->additional_info_fields_v1.dir_size * 4 * KiB);
		printf("\n%s    SPI block size:      %x",
		       indent,
		       4 * KiB * (1 << header->additional_info_fields_v1.spi_block_size));
		printf("\n%s    Dir header size:     %x",
		       indent, header->additional_info_fields_v1.dir_hdr_size * KiB);
		printf("\n%s    Address mode:        %u",
		       indent, header->additional_info_fields_v1.address_mode);
	} else {
		printf("\n%s    Directory size:      %x",
		       indent, header->additional_info_fields.dir_size * 4 * KiB);
		printf("\n%s    SPI block size:      %x",
		       indent,
		       4 * KiB * (1 << header->additional_info_fields.spi_block_size));
		printf("\n%s    Base address:        %x",
		       indent, header->additional_info_fields.base_addr << 12);
		printf("\n%s    Address mode:        %u",
		       indent, header->additional_info_fields.address_mode);
	}
	printf("\n\n");
}

static void do_indentation_string(char *dest, uint8_t level)
{
	dest[0]	= '\0';
	for (uint8_t i = 0; i < level && i < MAX_NUM_LEVELS; i++)
		strcat(dest, "    ");
	strcat(dest, "+-->");
}

static int amdfw_bios_dir_walk(FILE *fw, uint32_t bios_offset, uint32_t cookie, uint8_t level)
{
	bios_directory_entry *current_entries = NULL;
	size_t num_current_entries = 0;
	bios_directory_hdr header;
	uint32_t l2_dir_offset = 0;
	uint64_t dir_mode = AMD_ADDR_PHYSICAL;
	char indent[MAX_INDENTATION_LEN] = {0};

	if (read_bios_directory(fw, bios_offset, cookie, &header,
		       &current_entries, &num_current_entries) != 0)
		return 1;

	if (header.additional_info_fields.version == 1)
		dir_mode = header.additional_info_fields_v1.address_mode;
	else
		dir_mode = header.additional_info_fields.address_mode;

	dump_directory_header((psp_directory_header *)&header, level);

	do_indentation_string(indent, level);
	for (size_t i = 0; i < num_current_entries; i++) {
		uint32_t type = current_entries[i].type;
		uint64_t mode = current_entries[i].address_mode;
		uint64_t addr = current_entries[i].source;

		if (dir_mode < AMD_ADDR_REL_TAB)
			mode = dir_mode;

		if (type == AMD_BIOS_APOB || type == AMD_BIOS_PSP_SHARED_MEM)
			printf("%sBIOS%s: 0x%02x 0x%lx(DRAM-Address)\n",
				indent, cookie == BHD_COOKIE ? "L1" : "L2",
				type, current_entries[i].dest);
		else
			printf("%sBIOS%s: 0x%02x 0x%08lx 0x%08x\n",
				indent, cookie == BHD_COOKIE ? "L1" : "L2",
				type, relative_offset(bios_offset, addr, mode),
				current_entries[i].size);

		if (type == AMD_BIOS_L2_PTR) {
			/* There's a second level BIOS directory to read */
			if (l2_dir_offset != 0) {
				ERR("Duplicate BIOS L2 Entry, prior offset: %08x\n",
									l2_dir_offset);
				free(current_entries);
				return 1;
			}

			l2_dir_offset = relative_offset(bios_offset, addr, mode);
			printf("    %sBIOSL2: Dir  0x%08x\n", indent, l2_dir_offset);
			amdfw_bios_dir_walk(fw, l2_dir_offset, BHDL2_COOKIE, level + 2);
		}
	}

	free(current_entries);
	return 0;
}

/*
 * Returns the size of the PSP directory.
 *
 * @param fw			File to operate on
 * @param psp_offset		Relative offset to PSP directory
 * @param cookie		Expected table cookie
 * @param size			Where to write the size to
 *
 * @return 0 on success, or < 0 on error.
 */
static int amdfw_psp_dir_size(FILE *fw, uint32_t psp_offset, uint32_t cookie, uint32_t *size)
{
	psp_directory_header header;

	if (!fw || !size)
		return -1;

	if (read_psp_directory(fw, psp_offset, cookie, &header, NULL, NULL) != 0)
		return -1;

	if (header.additional_info_fields.version == 1)
		*size = header.additional_info_fields_v1.dir_size;
	else
		*size = header.additional_info_fields.dir_size;

	*size *= TABLE_GRANULARITY;
	return 0;
}

/*
 * Returns the size of the BIOS directory.
 *
 * @param fw			File to operate on
 * @param bios_offset		Relative offset to BIOS directory
 * @param cookie		Expected table cookie
 * @param size			Where to write the size to
 *
 * @return 0 on success, or < 0 on error.
 */
static int amdfw_bios_dir_size(FILE *fw, uint32_t bios_offset, uint32_t cookie, uint32_t *size)
{
	bios_directory_hdr header;

	if (!fw || !size)
		return -1;

	if (read_bios_directory(fw, bios_offset, cookie, &header, NULL, NULL) != 0)
		return -1;

	if (header.additional_info_fields.version == 1)
		*size = header.additional_info_fields_v1.dir_size;
	else
		*size = header.additional_info_fields.dir_size;

	*size *= TABLE_GRANULARITY;
	return 0;
}

static int amdfw_psp_dir_walk(FILE *fw, uint32_t psp_offset, uint32_t cookie, uint8_t level)
{
	psp_directory_entry *current_entries = NULL;
	size_t num_current_entries = 0;
	psp_directory_header header;
	uint32_t l2_dir_offset = 0, l2b_dir_offset = 0;
	uint32_t bios_dir_offset = 0;
	uint32_t ish_dir_offset = 0;
	ish_directory_table ish_dir;
	uint64_t dir_mode = AMD_ADDR_PHYSICAL;
	char indent[MAX_INDENTATION_LEN] = {0};

	if (read_psp_directory(fw, psp_offset, cookie, &header,
		       &current_entries, &num_current_entries) != 0)
		return 1;

	if (header.additional_info_fields.version == 1)
		dir_mode = header.additional_info_fields_v1.address_mode;
	else
		dir_mode = header.additional_info_fields.address_mode;

	dump_directory_header(&header, level);

	do_indentation_string(indent, level);
	for (size_t i = 0; i < num_current_entries; i++) {
		uint32_t type = current_entries[i].type;
		uint64_t mode = current_entries[i].address_mode;
		uint64_t addr = current_entries[i].addr;
		uint32_t dir_size = 0;

		if (dir_mode < AMD_ADDR_REL_TAB)
			mode = dir_mode;

		/* RECOVERY_AB is always relative to BIOS */
		if (type == AMD_FW_PSP_RECOVERYAB_B || type == AMD_FW_PSP_RECOVERYAB_A)
			mode = AMD_ADDR_REL_BIOS;

		if (type == AMD_FW_PSP_FUSE_CHAIN)
			printf("%sPSP%s: 0x%02x 0x%lx(Soft-fuse)\n",
				indent, cookie == PSP_COOKIE ? "L1" : "L2",
				type, (uint64_t)current_entries[i].address_mode << 62 | addr);
		else
			printf("%sPSP%s: 0x%02x 0x%08lx 0x%08x\n",
				indent, cookie == PSP_COOKIE ? "L1" : "L2",
				type, relative_offset(psp_offset, addr, mode),
				current_entries[i].size);

		switch (type) {
		case AMD_FW_PSP_L2_PTR:
			/* There's a second level PSP directory to read */
			if (l2_dir_offset != 0) {
				ERR("Duplicate PSP L2 Entry, prior offset: %08x\n",
									l2_dir_offset);
				free(current_entries);
				return 1;
			}

			l2_dir_offset = relative_offset(psp_offset, addr, mode);
			if (amdfw_psp_dir_size(fw, l2_dir_offset, PSPL2_COOKIE, &dir_size) == 0)
				printf("    %sPSPL2: Dir  [0x%08x-0x%08x)\n", indent, l2_dir_offset, l2_dir_offset + dir_size);
			else
				printf("    %sPSPL2: Dir  @0x%08x\n", indent, l2_dir_offset);
			amdfw_psp_dir_walk(fw, l2_dir_offset, PSPL2_COOKIE, level + 2);
			break;

		case AMD_FW_PSP_RECOVERYAB_B:
			if (l2b_dir_offset != 0) {
				ERR("Duplicate PSP L2B Entry, prior offset: %08x\n",
					l2b_dir_offset);
				free(current_entries);
				return 1;
			}

			ish_dir_offset = relative_offset(psp_offset, addr, AMD_ADDR_REL_BIOS);
			/* Test if it points to PSP L2 */
			if (test_if_psp_directory(fw, ish_dir_offset, PSPL2_COOKIE)) {
				/* Legacy A/B recovery has no ISH */
				l2b_dir_offset = ish_dir_offset;
			} else {
				/* Newer platforms use ISH for A/B recovery */
				if (read_ish_directory(fw, ish_dir_offset, &ish_dir) != 0) {
					ERR("Error reading ISH directory\n");
					free(current_entries);
					return 1;
				}
				do_indentation_string(indent, level);
				printf("    %sISHB: PSPID 0x%08x\n", indent, ish_dir.psp_id);
				do_indentation_string(indent, level+1);

				l2b_dir_offset = ish_dir.pl2_location;
			}

			if (amdfw_psp_dir_size(fw, l2b_dir_offset, PSPL2_COOKIE, &dir_size) == 0)
				printf("    %sPSPL2(B): Dir  [0x%08x-0x%08x)\n", indent, l2b_dir_offset, l2b_dir_offset + dir_size);
			else
				printf("    %sPSPL2(B): Dir  @0x%08x\n", indent, l2b_dir_offset);
			amdfw_psp_dir_walk(fw, l2b_dir_offset, PSPL2_COOKIE, level + 3);

			do_indentation_string(indent, level);

			break;

		case AMD_FW_PSP_RECOVERYAB_A:
			if (l2_dir_offset != 0) {
				ERR("Duplicate PSP L2 Entry, prior offset: %08x\n",
									l2_dir_offset);
				free(current_entries);
				return 1;
			}

			ish_dir_offset = relative_offset(psp_offset, addr, AMD_ADDR_REL_BIOS);
			/* Test if it points to PSP L2 */
			if (test_if_psp_directory(fw, ish_dir_offset, PSPL2_COOKIE)) {
				/* Legacy A/B recovery has no ISH */
				l2_dir_offset = ish_dir_offset;
			} else {
				/* Newer platforms use ISH for A/B recovery */
				if (read_ish_directory(fw, ish_dir_offset, &ish_dir) != 0) {
					ERR("Error reading ISH directory\n");
					free(current_entries);
					return 1;
				}
				do_indentation_string(indent, level);
				printf("    %sISHA: PSPID 0x%08x\n", indent, ish_dir.psp_id);
				do_indentation_string(indent, level+1);

				l2_dir_offset = ish_dir.pl2_location;
			}

			if (amdfw_psp_dir_size(fw, l2_dir_offset, PSPL2_COOKIE, &dir_size) == 0)
				printf("    %sPSPL2: Dir  [0x%08x-0x%08x)\n", indent, l2_dir_offset, l2_dir_offset + dir_size);
			else
				printf("    %sPSPL2: Dir  @0x%08x\n", indent, l2_dir_offset);
			amdfw_psp_dir_walk(fw, l2_dir_offset, PSPL2_COOKIE, level + 3);
			do_indentation_string(indent, level);
			break;

		case AMD_FW_PSP_BIOS_TABLE:
			bios_dir_offset = relative_offset(psp_offset, addr, mode);
			if (amdfw_bios_dir_size(fw, bios_dir_offset, BHDL2_COOKIE, &dir_size) == 0)
				printf("    %sBIOSL2: Dir  [0x%08x-0x%08x)\n", indent, bios_dir_offset, bios_dir_offset + dir_size);
			else
				printf("    %sBIOSL2: Dir  @0x%08x\n", indent, bios_dir_offset);
			amdfw_bios_dir_walk(fw, bios_dir_offset, BHDL2_COOKIE, level + 2);
			break;

		default:
			/* No additional processing required, continue to the next entry. */
			break;
		}
	}

	free(current_entries);
	return 0;
}

static int list_amdfw_psp_dir(FILE *fw, const embedded_firmware *fw_header)
{
	uint32_t psp_offset = 0, dir_size = 0;

	/* 0xffffffff or 0x00000000 indicates that the offset is in new_psp_directory */
	if (fw_header->psp_directory != 0xffffffff && fw_header->psp_directory != 0x00000000)
		psp_offset = fw_header->psp_directory;
	else
		psp_offset = fw_header->new_psp_directory;

	if (amdfw_psp_dir_size(fw, psp_offset, PSP_COOKIE, &dir_size) == 0)
		printf("PSPL1: Dir  [0x%08x-0x%08x)\n", psp_offset, psp_offset + dir_size);
	else
		printf("PSPL1: Dir  @0x%08x\n", psp_offset);

	amdfw_psp_dir_walk(fw, psp_offset, PSP_COOKIE, 0);
	return 0;
}

static int list_amdfw_bios_dir(FILE *fw, const embedded_firmware *fw_header)
{
	uint32_t dir_size = 0;

	/* 0xffffffff or 0x00000000 implies that the SoC uses recovery A/B
	   layout. Only BIOS L2 directory is present and that too as part of
	   PSP L2 directory. */
	if (fw_header->bios3_entry != 0xffffffff && fw_header->bios3_entry != 0x00000000) {
		if (amdfw_psp_dir_size(fw, fw_header->bios3_entry, BHD_COOKIE, &dir_size) == 0)
			printf("BIOSL1: Dir  [0x%08x-0x%08x)\n", fw_header->bios3_entry, fw_header->bios3_entry + dir_size);
		else
			printf("BIOSL1: Dir  @0x%08x\n", fw_header->bios3_entry);

		amdfw_bios_dir_walk(fw, fw_header->bios3_entry, BHD_COOKIE, 0);
	}
	return 0;
}


static int list_amdfw_ro(FILE *fw, const embedded_firmware *fw_header)
{
	printf("Table: FW   Offset     Size\n");
	list_amdfw_psp_dir(fw, fw_header);
	list_amdfw_bios_dir(fw, fw_header);
	return 0;
}

static void decode_spi_frequency(unsigned int freq)
{
	switch (freq) {
	case SPI_FREQUENCY_66_66MHZ:
		printf("66.66MHz");
		break;
	case SPI_FREQUENCY_33_33MHZ:
		printf("33.33MHz");
		break;
	case SPI_FREQUENCY_22_22MHZ:
		printf("22.22MHz");
		break;
	case SPI_FREQUENCY_16_66MHZ:
		printf("16.66MHz");
		break;
	case SPI_FREQUENCY_100MHZ:
		printf("100MHz");
		break;
	case SPI_FREQUENCY_800KHZ:
		printf("800kHz");
		break;
	default:
		printf("unknown<%x>MHz", freq);
	}
}

static void decode_spi_read_mode(unsigned int mode)
{
	switch (mode) {
	case SPI_READ_MODE_NORMAL_33:
		printf("Normal read (up to 33M)");
		break;
	case SPI_READ_MODE_RESERVED:
		printf("Reserved");
		break;
	case SPI_READ_MODE_DUAL_1_1_2:
		printf("Dual IO (1-1-2)");
		break;
	case SPI_READ_MODE_QUAD_1_1_4:
		printf("Quad IO (1-1-4)");
		break;
	case SPI_READ_MODE_DUAL_1_2_2:
		printf("Dual IO (1-2-2)");
		break;
	case SPI_READ_MODE_QUAD_1_4_4:
		printf("Quad IO (1-4-4)");
		break;
	case SPI_READ_MODE_NORMAL_66:
		printf("Normal read (up to 66M)");
		break;
	case SPI_READ_MODE_FAST:
		printf("Fast Read");
		break;
	default:
		printf("unknown<%x>mode", mode);
	}
}

static void print_bios_size(uint8_t size)
{
	printf("\n  BIOS size:             ");

	switch (size) {
	case 0:
		printf("16MB");
		break;
	case 1:
		printf("32MB");
		break;
	case 2:
		printf("48MB");
		break;
	case 3:
		printf("64MB");
		break;
	case 0xff:
		printf("not implemented");
		break;
	default:
		printf("unknown<%x>size", size);
	}
}

static int dump_efw(const embedded_firmware *fw_header)
{
	printf("EFS Generation:            %s\n", fw_header->efs_gen.gen ? "first" : "second");

	printf("\nFamily 15h Models 60h-6Fh");
	printf("\n  SPI Read Mode          ");
	decode_spi_read_mode(fw_header->spi_readmode_f15_mod_60_6f);
	printf("\n  SPI Frequency:         ");
	decode_spi_frequency(fw_header->fast_speed_new_f15_mod_60_6f);

	printf("\n\nFamily 17h Models 00h-0Fh");
	printf("\n  PSP Dir:               %08x", fw_header->new_psp_directory);
	printf("\n  BIOS Dir:              %08x", fw_header->bios0_entry);
	printf("\n  SPI Read Mode:         ");
	decode_spi_read_mode(fw_header->spi_readmode_f17_mod_00_2f);
	printf("\n  Fast Speed New:        ");
	decode_spi_frequency(fw_header->spi_fastspeed_f17_mod_00_2f);
	printf("\n  QPR_Dummy Cycle configure:    0x%02x\n", fw_header->qpr_dummy_cycle_f17_mod_00_2f);

	printf("\n\nFamily 17h Models 10h-1Fh");
	printf("\n  BIOS Dir:              %08x", fw_header->bios1_entry);

	printf("\nFamily 17h Models 30h-3Fh and later Families");
	printf("\n  BIOS Dir:              %08x", fw_header->bios2_entry);
	printf("\n  SPI Read Mode:         ");
	decode_spi_read_mode(fw_header->spi_readmode_f17_mod_30_3f);
	printf("\n  SPI Fast Speed:        ");
	decode_spi_frequency(fw_header->spi_fastspeed_f17_mod_30_3f);
	printf("\n  Micron Detect Flag:    0x%02x\n", fw_header->micron_detect_f17_mod_30_3f);

	printf("\nFamily 19h Models 00h-0Fh and later Families");
	printf("\n  Multi Gen EFS:         %08x", fw_header->multi_gen_efs);
	printf("\n  BIOS Dir:              %08x\n", fw_header->bios3_entry);

	printf("\nFamily 1Ah Models 50h-5Fh and later Families");
	printf("\n  PSPL1 backup:          %08x", fw_header->psp_bak_directory);
	printf("\n  BIOS Dir:              %08x\n", fw_header->bios3_entry);

	printf("\nMisc info");
	printf("\n  Promontory FW:         %08x", fw_header->promontory_fw_ptr);
	printf("\n  LP Promontory FW:      %08x", fw_header->lp_promontory_fw_ptr);
	printf("\n  Promontory19 FW:       %08x", fw_header->promontory19_fw_ptr);
	printf("\n  Vendor ID:             %04x", fw_header->vendor_id);
	printf("\n  Board ID:              %04x", fw_header->board_id);
	printf("\n  ESPI0 Config:          %02x", fw_header->espi0_config0);
	printf("\n  ESPI0 Config1:         %02x", fw_header->espi0_config1);
	printf("\n  ESPI1 Config:          %02x", fw_header->espi1_config0);
	printf("\n  ESPI1 Config1:         %02x", fw_header->espi1_config1);
	printf("\n  UBU Table:             %08x", fw_header->ubu_table);
	print_bios_size(fw_header->bios_size);
	printf("\n\n");

	return 0;
}

enum {
	AMDFW_OPT_HELP = 'h',
	AMDFW_OPT_DUMP = 'd',
	AMDFW_OPT_SOFT_FUSE = 1UL << 0, /* Print Softfuse */
	AMDFW_OPT_RO_LIST = 1UL << 1, /* List entries in AMDFW RO */
};

static const char optstring[] = {AMDFW_OPT_HELP, AMDFW_OPT_DUMP};

static struct option long_options[] = {
	{"help", no_argument, 0, AMDFW_OPT_HELP},
	{"dump", no_argument, 0, AMDFW_OPT_DUMP},
	{"soft-fuse", no_argument, 0, AMDFW_OPT_SOFT_FUSE},
	{"ro-list", no_argument, 0, AMDFW_OPT_RO_LIST},
};

static void print_usage(void)
{
	printf("amdfwread: Examine AMD firmware images\n");
	printf("Usage: amdfwread [options] <file>\n");
	printf("-d | --dump                Dump Embedded Firmware Structure\n");
	printf("--soft-fuse                Print soft fuse value\n");
	printf("--ro-list                  List the programs under AMDFW in RO region\n");
}

int main(int argc, char **argv)
{
	char *fw_file = NULL;
	int mode_dump = 0;
	ssize_t fw_size;

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
		case AMDFW_OPT_DUMP:
			mode_dump = 1;
			break;

		case AMDFW_OPT_SOFT_FUSE:
		case AMDFW_OPT_RO_LIST:
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

	/* Get file size */
	fseek(fw, 0, SEEK_END);
	if ((fw_size = ftell(fw)) == -1) {
		ERR("Failed to get FW file size\n");
		fclose(fw);
		return -1;
	}
	if (fw_size > 128 * MiB) {
		ERR("FW ROM size 0x%zx not supported\n", fw_size);
		fclose(fw);
		return -1;
	}
	fseek(fw, 0, SEEK_SET);
	rom_size = fw_size;

	/* Find the FW header by checking each possible location */
	embedded_firmware fw_header;
	int found_header = 0;
	if (rom_size <= 4 * MiB) {
		for (size_t i = 0; i < ARRAY_SIZE(fw_header_offsets_4MiB); i++) {
			if (read_fw_header(fw, fw_header_offsets_4MiB[i], &fw_header) == 0) {
				found_header = 1;
				break;
			}
		}
	} else if (rom_size <= 8 * MiB) {
		for (size_t i = 0; i < ARRAY_SIZE(fw_header_offsets_8MiB); i++) {
			if (read_fw_header(fw, fw_header_offsets_8MiB[i], &fw_header) == 0) {
				found_header = 1;
				break;
			}
		}
	} else {
		for (size_t i = 0; i < ARRAY_SIZE(fw_header_offsets); i++) {
			if (read_fw_header(fw, fw_header_offsets[i], &fw_header) == 0) {
				found_header = 1;
				break;
			}
		}
	}

	if (!found_header) {
		ERR("Failed to find FW header\n");
		fclose(fw);
		return 1;
	}

	if (mode_dump)
		dump_efw(&fw_header);

	if (selected_functions & AMDFW_OPT_SOFT_FUSE) {
		if (read_soft_fuse(fw, &fw_header) != 0) {
			fclose(fw);
			return 1;
		}
	}

	if (selected_functions & AMDFW_OPT_RO_LIST) {
		if (list_amdfw_ro(fw, &fw_header) != 0) {
			fclose(fw);
			return 1;
		}
	}

	fclose(fw);
	return 0;
}
