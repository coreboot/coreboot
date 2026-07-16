/* SPDX-License-Identifier: GPL-2.0-only */
#include <getopt.h>
#include <stddef.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "amdfwtool.h"

#include <vendorcode/amd/psp/psp_fw_parser.h>

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

struct ro_list_state {
	bool psp_l2_seen;
	bool stop_after_psp_l2;
};

/* Size of the FW in bytes */
static uint32_t rom_size;

static bool walk_efs_psp_softfuse_callback(const struct psp_dir_region *dir_rdev,
					   const size_t level,
					   const size_t index,
					   const union psp_dir_entry *entry,
					   void *user_data)
{
	(void) user_data;
	(void) index;
	(void) level;

	if (dir_rdev->type != PSP_DIR_PSP)
		return true;
	if (entry->psp.type == AMD_FW_PSP_FUSE_CHAIN) {
		uint64_t fuse = (uint64_t)entry->psp.address_mode << 62 | entry->psp.addr;
		printf("Soft-fuse:0x%lx\n", fuse);
		return false;	/* Abort walk */
	}
	return true;
}


#define MAX_NUM_LEVELS 10
#define MAX_INDENT_PER_LEVEL 4
#define MAX_INDENTATION_LEN (MAX_NUM_LEVELS * MAX_INDENT_PER_LEVEL + 1)

static void dump_directory_header(const struct psp_dir_region *dir_rdev, uint8_t level)
{
	char indent[MAX_INDENTATION_LEN] = {0};

	const struct bios_directory_hdr *header = rdev_mmap_full(psp_dir_get_rdev(dir_rdev));

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

	rdev_munmap(psp_dir_get_rdev(dir_rdev), (void *)header);
}

static void do_indentation_string(char *dest, uint8_t level)
{
	dest[0]	= '\0';
	for (uint8_t i = 0; i < level && i < MAX_NUM_LEVELS; i++)
		strcat(dest, "    ");
	strcat(dest, "+-->");
}

/*
 * Returns the size of the PSP directory.
 *
 * @param dir_rdev	PSP directory region
 * @param size		Where to write the size to
 *
 * @return 0 on success, or < 0 on error.
 */
static int amdfw_psp_dir_size(const struct psp_dir_region *dir_rdev, uint32_t *size)
{
	if (!dir_rdev || dir_rdev->type != PSP_DIR_PSP)
		return -1;

	const struct psp_directory_header *hdr = rdev_mmap_full(psp_dir_get_rdev(dir_rdev));

	if (hdr->additional_info_fields.version == 1)
		*size = hdr->additional_info_fields_v1.dir_size;
	else
		*size = hdr->additional_info_fields.dir_size;

	rdev_munmap(psp_dir_get_rdev(dir_rdev), (void *)hdr);

	*size *= TABLE_GRANULARITY;
	return 0;
}

/*
 * Returns the size of the BIOS directory.
 *
 * @param dir_rdev	PSP directory region
 * @param size		Where to write the size to
 *
 * @return 0 on success, or < 0 on error.
 */
static int amdfw_bios_dir_size(const struct psp_dir_region *dir_rdev, uint32_t *size)
{
	if (!dir_rdev || dir_rdev->type != PSP_DIR_BIOS)
		return -1;

	const struct bios_directory_hdr *hdr = rdev_mmap_full(psp_dir_get_rdev(dir_rdev));

	if (hdr->additional_info_fields.version == 1)
		*size = hdr->additional_info_fields_v1.dir_size;
	else
		*size = hdr->additional_info_fields.dir_size;

	rdev_munmap(psp_dir_get_rdev(dir_rdev), (void *)hdr);

	*size *= TABLE_GRANULARITY;
	return 0;
}

static bool walk_efs_list_hdr_callback(const struct psp_dir_region *dir_rdev,
				       const size_t level,
				       void *user_data)
{
	struct ro_list_state *state = (struct ro_list_state *)user_data;
	char indent[MAX_INDENTATION_LEN] = {0};
	uint32_t size;

	do_indentation_string(indent, level);

	if (dir_rdev->type == PSP_DIR_BIOS) {
		if (amdfw_bios_dir_size(dir_rdev, &size) == 0) {
			printf("%sBIOSL%zd: Dir  [0x%08zx-0x%08zx)\n", indent, dir_rdev->level,
				psp_dir_relative_offset(dir_rdev), psp_dir_relative_offset(dir_rdev) + size);
		} else {
			printf("%sBIOSL%zd: Dir  @0x%08zx\n", indent, dir_rdev->level,
				psp_dir_relative_offset(dir_rdev));
		}
		dump_directory_header(dir_rdev, level);
	} else if (dir_rdev->type == PSP_DIR_PSP) {
		if (dir_rdev->level == 2 && state->stop_after_psp_l2) {
			if (state->psp_l2_seen)
				return false;
			state->psp_l2_seen = true;
		}
		if (amdfw_psp_dir_size(dir_rdev, &size) == 0) {
			printf("%sPSPL%zd: Dir  [0x%08zx-0x%08zx)\n", indent, dir_rdev->level,
				psp_dir_relative_offset(dir_rdev), psp_dir_relative_offset(dir_rdev) + size);
		} else {
			printf("%sPSPL%zd: Dir  @0x%08zx\n", indent, dir_rdev->level,
				psp_dir_relative_offset(dir_rdev));
		}
		dump_directory_header(dir_rdev, level);
	}

	return true;
}

static bool walk_efs_ro_list_callback(const struct psp_dir_region *dir_rdev,
				      const size_t level,
				      const size_t index,
				      const union psp_dir_entry *entry,
				      void *user_data)
{
	(void) index;
	(void) user_data;
	char indent[MAX_INDENTATION_LEN] = {0};

	do_indentation_string(indent, level);
	uint32_t rel_offset = psp_dir_entry_offset(dir_rdev, entry);

	if (dir_rdev->type == PSP_DIR_BIOS) {
		switch (entry->bios.type) {
		case AMD_BIOS_APOB:
		case AMD_BIOS_PSP_SHARED_MEM:
			printf("%sBIOS%s: 0x%02x 0x%lx(DRAM-Address)\n",
				indent, dir_rdev->level == 1 ? "L1" : "L2",
				entry->bios.type, entry->bios.dest);
			break;
		default:
			printf("%sBIOS%s: 0x%02x 0x%08x 0x%08x\n",
				indent, dir_rdev->level == 1 ? "L1" : "L2",
				entry->bios.type, rel_offset, entry->bios.size);
			break;
		}
	} else if (dir_rdev->type == PSP_DIR_PSP) {
		switch (entry->psp.type) {
		case AMD_FW_PSP_FUSE_CHAIN:
			printf("%sPSP%s: 0x%02x 0x%lx(Soft-fuse)\n",
				indent, dir_rdev->level == 1 ? "L1" : "L2",
				entry->psp.type, (uint64_t)entry->psp.address_mode << 62 | entry->psp.addr);
			break;
		default:
			printf("%sPSP%s: 0x%02x 0x%08x 0x%08x\n",
				indent, dir_rdev->level == 1 ? "L1" : "L2",
				entry->psp.type, rel_offset, entry->psp.size);
			break;
		}
	} else if (dir_rdev->type == PSP_DIR_ISH) {
		printf("%sISH%s: PSPID 0x%08x\n", indent,
		      psp_fw_partition(dir_rdev) == 'A' ? "A" : "B", entry->ish.psp_id);
	}

	return true;
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

static int dump_efw(const struct embedded_firmware *fw_header)
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
	uint8_t *raw_buf = malloc(rom_size);
	if (!raw_buf) {
		ERR("Failed to allocate memory for FW file\n");
		fclose(fw);
		return -1;
	}
	if (fread(raw_buf, 1, rom_size, fw) != rom_size) {
		ERR("Failed to read FW file\n");
		fclose(fw);
		return -1;
	}

	struct mem_region_device mem_rdev;
	mem_region_device_ro_init(&mem_rdev, raw_buf, rom_size);
	struct psp_dir_region rdev_efs;

	/* Find the FW header by checking each possible location */
	int found_header = 0;
	if (rom_size <= 4 * MiB) {
		for (size_t i = 0; i < ARRAY_SIZE(fw_header_offsets_4MiB); i++) {
			if (psp_fw_read_and_validate_table(&mem_rdev.rdev, fw_header_offsets_4MiB[i],
							   PSP_DIR_EFS, &rdev_efs) == CB_SUCCESS) {
				found_header = 1;
				break;
			}
		}
	} else if (rom_size <= 8 * MiB) {
		for (size_t i = 0; i < ARRAY_SIZE(fw_header_offsets_8MiB); i++) {
			if (psp_fw_read_and_validate_table(&mem_rdev.rdev, fw_header_offsets_8MiB[i],
							   PSP_DIR_EFS, &rdev_efs) == CB_SUCCESS) {
				found_header = 1;
				break;
			}
		}
	} else {
		for (size_t i = 0; i < ARRAY_SIZE(fw_header_offsets); i++) {
			if (psp_fw_read_and_validate_table(&mem_rdev.rdev, fw_header_offsets[i],
							   PSP_DIR_EFS, &rdev_efs) == CB_SUCCESS) {
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
		dump_efw(rdev_mmap_full(psp_dir_get_rdev(&rdev_efs)));

	if (selected_functions & AMDFW_OPT_SOFT_FUSE)
		psp_dir_walk_recursive(&rdev_efs, 0, 0, NULL, walk_efs_psp_softfuse_callback, NULL);

	struct ro_list_state state = { .stop_after_psp_l2 = true, .psp_l2_seen = false };

	if (selected_functions & AMDFW_OPT_RO_LIST)
		psp_dir_walk_recursive(&rdev_efs, 0, 0, walk_efs_list_hdr_callback, walk_efs_ro_list_callback, &state);

	fclose(fw);
	return 0;
}
