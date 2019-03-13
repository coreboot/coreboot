/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2015 - 2016 Advanced Micro Devices, Inc.
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

/*
 *  ROMSIG At ROMBASE + 0x20000:
 *  0            4               8                C
 *  +------------+---------------+----------------+------------+
 *  | 0x55AA55AA |EC ROM Address |GEC ROM Address |USB3 ROM    |
 *  +------------+---------------+----------------+------------+
 *  | PSPDIR ADDR|PSPDIR ADDR    |<-- Field 0x14 could be either
 *  +------------+---------------+   2nd PSP directory or PSP COMBO directory
 *  EC ROM should be 64K aligned.
 *
 *  PSP directory (Where "PSPDIR ADDR" points)
 *  +------------+---------------+----------------+------------+
 *  | 'PSP$'     | Fletcher      |    Count       | Reserved   |
 *  +------------+---------------+----------------+------------+
 *  |  0         | size          | Base address   | Reserved   | Pubkey
 *  +------------+---------------+----------------+------------+
 *  |  1         | size          | Base address   | Reserved   | Bootloader
 *  +------------+---------------+----------------+------------+
 *  |  8         | size          | Base address   | Reserved   | Smu Firmware
 *  +------------+---------------+----------------+------------+
 *  |  3         | size          | Base address   | Reserved   | Recovery Firmware
 *  +------------+---------------+----------------+------------+
 *  |                                                          |
 *  |                                                          |
 *  |             Other PSP Firmware                           |
 *  |                                                          |
 *  |                                                          |
 *  +------------+---------------+----------------+------------+
 *
 *  PSP Combo directory
 *  +------------+---------------+----------------+------------+
 *  | 'PSP2'     | Fletcher      |    Count       |Look up mode|
 *  +------------+---------------+----------------+------------+
 *  |            R e s e r v e d                               |
 *  +------------+---------------+----------------+------------+
 *  | ID-Sel     | PSP ID        |   PSPDIR ADDR  |            | 2nd PSP directory
 *  +------------+---------------+----------------+------------+
 *  | ID-Sel     | PSP ID        |   PSPDIR ADDR  |            | 3rd PSP directory
 *  +------------+---------------+----------------+------------+
 *  |                                                          |
 *  |        Other PSP                                         |
 *  |                                                          |
 *  +------------+---------------+----------------+------------+
 *
 */

#include <fcntl.h>
#include <errno.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <getopt.h>

#ifndef CONFIG_ROM_SIZE
#define CONFIG_ROM_SIZE 0x400000
#endif

#define AMD_ROMSIG_OFFSET	0x20000
#define MIN_ROM_KB		256

#define ALIGN(val, by) (((val) + (by) - 1) & ~((by) - 1))
#define TABLE_ALIGNMENT 0x1000U
#define BLOB_ALIGNMENT 0x100U

#define EMBEDDED_FW_SIGNATURE 0x55aa55aa
#define PSP_COOKIE 0x50535024	/* 'PSP$' */
#define PSP2_COOKIE 0x50535032	/* 'PSP2' */

/*
 * Beginning with Family 15h Models 70h-7F, a.k.a Stoney Ridge, the PSP
 * can support an optional "combo" implementation.  If the PSP sees the
 * PSP2 cookie, it interprets the table as a roadmap to additional PSP
 * tables.  Using this, support for multiple product generations may be
 * built into one image.  If the PSP$ cookie is found, the table is a
 * normal directory table.
 *
 * Modern generations supporting the combo directories require the
 * pointer to be at offset 0x14 of the Embedded Firmware Structure,
 * regardless of the type of directory used.  The --combo-capable
 * argument enforces this placement.
 *
 * TODO: Future work may require fully implementing the PSP_COMBO feature.
 */
#define PSP_COMBO 0

typedef unsigned long long int uint64_t;
typedef unsigned int uint32_t;
typedef unsigned char uint8_t;
typedef unsigned short uint16_t;

/*
 * Creates the OSI Fletcher checksum. See 8473-1, Appendix C, section C.3.
 * The checksum field of the passed PDU does not need to be reset to zero.
 *
 * The "Fletcher Checksum" was proposed in a paper by John G. Fletcher of
 * Lawrence Livermore Labs.  The Fletcher Checksum was proposed as an
 * alternative to cyclical redundancy checks because it provides error-
 * detection properties similar to cyclical redundancy checks but at the
 * cost of a simple summation technique.  Its characteristics were first
 * published in IEEE Transactions on Communications in January 1982.  One
 * version has been adopted by ISO for use in the class-4 transport layer
 * of the network protocol.
 *
 * This program expects:
 *    stdin:    The input file to compute a checksum for.  The input file
 *              not be longer than 256 bytes.
 *    stdout:   Copied from the input file with the Fletcher's Checksum
 *              inserted 8 bytes after the beginning of the file.
 *    stderr:   Used to print out error messages.
 */
static uint32_t fletcher32(const void *data, int length)
{
	uint32_t c0;
	uint32_t c1;
	uint32_t checksum;
	int index;
	const uint16_t *pptr = data;

	length /= 2;

	c0 = 0xFFFF;
	c1 = 0xFFFF;

	for (index = 0; index < length; index++) {
		/*
		* Ignore the contents of the checksum field.
		*/
		c0 += *(pptr++);
		c1 += c0;
		if ((index % 360) == 0) {
			/* Sums[0,1] mod 64K + overflow */
			c0 = (c0 & 0xFFFF) + (c0 >> 16);
			c1 = (c1 & 0xFFFF) + (c1 >> 16);
		}
	}

	/* Sums[0,1] mod 64K + overflow */
	c0 = (c0 & 0xFFFF) + (c0 >> 16);
	c1 = (c1 & 0xFFFF) + (c1 >> 16);
	checksum = (c1 << 16) | c0;

	return checksum;
}

static void usage(void)
{
	printf("amdfwtool: Create AMD Firmware combination\n");
	printf("Usage: amdfwtool [options] -f <size> -o <filename>\n");
	printf("-x | --xhci <FILE>             Add XHCI blob\n");
	printf("-i | --imc <FILE>              Add IMC blob\n");
	printf("-g | --gec <FILE>              Add GEC blob\n");

	printf("\nPSP options:\n");
	printf("-A | --combo-capable           Place PSP directory pointer at Embedded Firmware\n");
	printf("                               offset able to support combo directory\n");
	printf("-p | --pubkey <FILE>           Add pubkey\n");
	printf("-b | --bootloader <FILE>       Add bootloader\n");
	printf("-S | --subprogram <number>     Sets subprogram field for the next firmware\n");
	printf("-s | --smufirmware <FILE>      Add smufirmware\n");
	printf("-r | --recovery <FILE>         Add recovery\n");
	printf("-k | --rtmpubkey <FILE>        Add rtmpubkey\n");
	printf("-c | --secureos <FILE>         Add secureos\n");
	printf("-n | --nvram <FILE>            Add nvram\n");
	printf("-d | --securedebug <FILE>      Add securedebug\n");
	printf("-t | --trustlets <FILE>        Add trustlets\n");
	printf("-u | --trustletkey <FILE>      Add trustletkey\n");
	printf("-w | --smufirmware2 <FILE>     Add smufirmware2\n");
	printf("-m | --smuscs <FILE>           Add smuscs\n");
	printf("\n-o | --output <filename>     output filename\n");
	printf("-f | --flashsize <HEX_VAL>     ROM size in bytes\n");
	printf("                               size must be larger than %dKB\n",
		MIN_ROM_KB);
	printf("                               and must a multiple of 1024\n");
	printf("-l | --location                Location of Directory\n");
	printf("-h | --help                    show this help\n");
}

typedef enum _amd_fw_type {
	AMD_FW_PSP_PUBKEY = 0,
	AMD_FW_PSP_BOOTLOADER = 1,
	AMD_FW_PSP_SMU_FIRMWARE = 8,
	AMD_FW_PSP_RECOVERY = 3,
	AMD_FW_PSP_RTM_PUBKEY = 5,
	AMD_FW_PSP_SECURED_OS = 2,
	AMD_FW_PSP_NVRAM = 4,
	AMD_FW_PSP_SECURED_DEBUG = 9,
	AMD_FW_PSP_TRUSTLETS = 12,
	AMD_FW_PSP_TRUSTLETKEY = 13,
	AMD_FW_PSP_SMU_FIRMWARE2 = 18,
	AMD_PSP_FUSE_CHAIN = 11,
	AMD_FW_PSP_SMUSCS = 95,

	AMD_FW_IMC,
	AMD_FW_GEC,
	AMD_FW_XHCI,
	AMD_FW_INVALID,
} amd_fw_type;

typedef struct _amd_fw_entry {
	amd_fw_type type;
	uint8_t subprog;
	char *filename;
} amd_fw_entry;

static amd_fw_entry amd_psp_fw_table[] = {
	{ .type = AMD_FW_PSP_PUBKEY },
	{ .type = AMD_FW_PSP_BOOTLOADER },
	{ .type = AMD_FW_PSP_SMU_FIRMWARE },
	{ .type = AMD_FW_PSP_RECOVERY },
	{ .type = AMD_FW_PSP_RTM_PUBKEY },
	{ .type = AMD_FW_PSP_SECURED_OS },
	{ .type = AMD_FW_PSP_NVRAM },
	{ .type = AMD_FW_PSP_SECURED_DEBUG },
	{ .type = AMD_FW_PSP_TRUSTLETS },
	{ .type = AMD_FW_PSP_TRUSTLETKEY },
	{ .type = AMD_FW_PSP_SMU_FIRMWARE, .subprog = 1 },
	{ .type = AMD_FW_PSP_SMU_FIRMWARE2, .subprog = 1 },
	{ .type = AMD_FW_PSP_SMU_FIRMWARE2 },
	{ .type = AMD_FW_PSP_SMUSCS },
	{ .type = AMD_PSP_FUSE_CHAIN },
	{ .type = AMD_FW_INVALID },
};

static amd_fw_entry amd_fw_table[] = {
	{ .type = AMD_FW_XHCI },
	{ .type = AMD_FW_IMC },
	{ .type = AMD_FW_GEC },
	{ .type = AMD_FW_INVALID },
};

typedef struct _embedded_firmware {
	uint32_t signature; /* 0x55aa55aa */
	uint32_t imc_entry;
	uint32_t gec_entry;
	uint32_t xhci_entry;
	uint32_t psp_entry;
	uint32_t comboable;
} __attribute__((packed, aligned(16))) embedded_firmware;

typedef struct _psp_directory_header {
	uint32_t cookie;
	uint32_t checksum;
	uint32_t num_entries;
	uint32_t reserved;
} __attribute__((packed, aligned(16))) psp_directory_header;

typedef struct _psp_directory_entry {
	uint8_t type;
	uint8_t subprog;
	uint16_t rsvd;
	uint32_t size;
	uint64_t addr; /* or a value in some cases */
} __attribute__((packed)) psp_directory_entry;

typedef struct _psp_directory_table {
	psp_directory_header header;
	psp_directory_entry entries[];
} __attribute__((packed)) psp_directory_table;

#define MAX_PSP_ENTRIES 0x1f

typedef struct _psp_combo_header {
	uint32_t cookie;
	uint32_t checksum;
	uint32_t num_entries;
	uint32_t lookup;
	uint64_t reserved[2];
} __attribute__((packed, aligned(16))) psp_combo_header;

typedef struct _psp_combo_entry {
	uint32_t id_sel;
	uint32_t id;
	uint64_t lvl2_addr;
} __attribute__((packed)) psp_combo_entry;

typedef struct _psp_combo_directory {
	psp_combo_header header;
	psp_combo_entry entries[];
} __attribute__((packed)) psp_combo_directory;

#define MAX_COMBO_ENTRIES 1

typedef struct _context {
	char *rom;		/* target buffer, size of flash device */
	uint32_t rom_size;	/* size of flash device */
	uint32_t current;	/* pointer within flash & proxy buffer */
} context;

#define RUN_BASE(ctx) (0xFFFFFFFF - (ctx).rom_size + 1)
#define RUN_OFFSET(ctx, offset) (RUN_BASE(ctx) + (offset))
#define RUN_CURRENT(ctx) RUN_OFFSET((ctx), (ctx).current)
#define BUFF_OFFSET(ctx, offset) ((void *)((ctx).rom + (offset)))
#define BUFF_CURRENT(ctx) BUFF_OFFSET((ctx), (ctx).current)
#define BUFF_TO_RUN(ctx, ptr) RUN_OFFSET((ctx), ((char *)(ptr) - (ctx).rom))
#define BUFF_ROOM(ctx) ((ctx).rom_size - (ctx).current)

static void *new_psp_dir(context *ctx)
{
	void *ptr;

	ctx->current = ALIGN(ctx->current, TABLE_ALIGNMENT);
	ptr = BUFF_CURRENT(*ctx);
	ctx->current += sizeof(psp_directory_header)
			+ MAX_PSP_ENTRIES * sizeof(psp_directory_entry);
	return ptr;
}

static void *new_combo_dir(context *ctx)
{
	void *ptr;

	ctx->current = ALIGN(ctx->current, TABLE_ALIGNMENT);
	ptr = BUFF_CURRENT(*ctx);
	ctx->current += sizeof(psp_combo_header)
			+ MAX_COMBO_ENTRIES * sizeof(psp_combo_entry);
	return ptr;
}

static void fill_dir_header(void *directory, uint32_t count, uint32_t cookie)
{
	if (cookie == PSP2_COOKIE) {
		/* caller is responsible for lookup mode */
		psp_combo_directory *cdir = directory;
		cdir->header.cookie = cookie;
		cdir->header.num_entries = count;
		cdir->header.reserved[0] = 0;
		cdir->header.reserved[1] = 0;
		/* checksum everything that comes after the Checksum field */
		cdir->header.checksum = fletcher32(&cdir->header.num_entries,
					count * sizeof(psp_combo_entry)
					+ sizeof(cdir->header.num_entries)
					+ sizeof(cdir->header.lookup)
					+ 2 * sizeof(cdir->header.reserved[0]));
	} else {
		psp_directory_table *dir = directory;
		dir->header.cookie = cookie;
		dir->header.num_entries = count;
		dir->header.reserved = 0;
		/* checksum everything that comes after the Checksum field */
		dir->header.checksum = fletcher32(&dir->header.num_entries,
					count * sizeof(psp_directory_entry)
					+ sizeof(dir->header.num_entries)
					+ sizeof(dir->header.reserved));
	}
}

static ssize_t copy_blob(void *dest, const char *src_file, size_t room)
{
	int fd;
	struct stat fd_stat;
	ssize_t bytes;

	fd = open(src_file, O_RDONLY);
	if (fd < 0) {
		printf("Error: %s\n", strerror(errno));
		return -1;
	}

	if (fstat(fd, &fd_stat)) {
		printf("fstat error: %s\n", strerror(errno));
		return -2;
	}

	if (fd_stat.st_size > room) {
		printf("Error: %s will not fit.  Exiting.\n", src_file);
		return -3;
	}

	bytes = read(fd, dest, (size_t)fd_stat.st_size);
	close(fd);
	if (bytes != (ssize_t)fd_stat.st_size) {
		printf("Error while reading %s\n", src_file);
		return -4;
	}

	return bytes;
}

static void integrate_firmwares(context *ctx,
				embedded_firmware *romsig,
				amd_fw_entry *fw_table)
{
	ssize_t bytes;
	int i;

	ctx->current += sizeof(embedded_firmware);
	ctx->current = ALIGN(ctx->current, BLOB_ALIGNMENT);

	for (i = 0; fw_table[i].type != AMD_FW_INVALID; i++) {
		if (fw_table[i].filename != NULL) {
			switch (fw_table[i].type) {
			case AMD_FW_IMC:
				ctx->current = ALIGN(ctx->current, 0x10000U);
				romsig->imc_entry = RUN_CURRENT(*ctx);
				break;
			case AMD_FW_GEC:
				romsig->gec_entry = RUN_CURRENT(*ctx);
				break;
			case AMD_FW_XHCI:
				romsig->xhci_entry = RUN_CURRENT(*ctx);
				break;
			default:
				/* Error */
				break;
			}

			bytes = copy_blob(BUFF_CURRENT(*ctx),
					fw_table[i].filename, BUFF_ROOM(*ctx));
			if (bytes < 0) {
				free(ctx->rom);
				exit(1);
			}

			ctx->current = ALIGN(ctx->current + bytes,
							BLOB_ALIGNMENT);
		}
	}
}

static void integrate_psp_firmwares(context *ctx,
					psp_directory_table *pspdir,
					amd_fw_entry *fw_table)
{
	ssize_t bytes;
	unsigned int i, count;

	ctx->current = ALIGN(ctx->current, BLOB_ALIGNMENT);

	for (i = 0, count = 0; fw_table[i].type != AMD_FW_INVALID; i++) {
		if (fw_table[i].type == AMD_PSP_FUSE_CHAIN) {
			pspdir->entries[count].type = fw_table[i].type;
			pspdir->entries[count].subprog = fw_table[i].subprog;
			pspdir->entries[count].rsvd = 0;
			pspdir->entries[count].size = 0xFFFFFFFF;
			pspdir->entries[count].addr = 1;
			count++;
		} else if (fw_table[i].filename != NULL) {
			bytes = copy_blob(BUFF_CURRENT(*ctx),
					fw_table[i].filename, BUFF_ROOM(*ctx));
			if (bytes < 0) {
				free(ctx->rom);
				exit(1);
			}

			pspdir->entries[count].type = fw_table[i].type;
			pspdir->entries[count].subprog = fw_table[i].subprog;
			pspdir->entries[count].rsvd = 0;
			pspdir->entries[count].size = (uint32_t)bytes;
			pspdir->entries[count].addr = RUN_CURRENT(*ctx);

			ctx->current = ALIGN(ctx->current + bytes,
							BLOB_ALIGNMENT);
			count++;
		} else {
			/* This APU doesn't have this firmware. */
		}
	}

	if (count > MAX_PSP_ENTRIES) {
		printf("Error: PSP entries exceed max allowed items\n");
		free(ctx->rom);
		exit(1);
	}

	fill_dir_header(pspdir, count, PSP_COOKIE);
}

static const char *optstring  = "x:i:g:AS:p:b:s:r:k:c:n:d:t:u:w:m:o:f:l:h";

static struct option long_options[] = {
	{"xhci",             required_argument, 0, 'x' },
	{"imc",              required_argument, 0, 'i' },
	{"gec",              required_argument, 0, 'g' },
	/* PSP */
	{"combo-capable",          no_argument, 0, 'A' },
	{"subprogram",       required_argument, 0, 'S' },
	{"pubkey",           required_argument, 0, 'p' },
	{"bootloader",       required_argument, 0, 'b' },
	{"smufirmware",      required_argument, 0, 's' },
	{"recovery",         required_argument, 0, 'r' },
	{"rtmpubkey",        required_argument, 0, 'k' },
	{"secureos",         required_argument, 0, 'c' },
	{"nvram",            required_argument, 0, 'n' },
	{"securedebug",      required_argument, 0, 'd' },
	{"trustlets",        required_argument, 0, 't' },
	{"trustletkey",      required_argument, 0, 'u' },
	{"smufirmware2",     required_argument, 0, 'w' },
	{"smuscs",           required_argument, 0, 'm' },
	{"output",           required_argument, 0, 'o' },
	{"flashsize",        required_argument, 0, 'f' },
	{"location",         required_argument, 0, 'l' },
	{"help",             no_argument,       0, 'h' },

	{NULL,               0,                 0,  0  }
};

static void register_fw_filename(amd_fw_type type, uint8_t sub, char filename[])
{
	unsigned int i;

	for (i = 0; i < sizeof(amd_fw_table) / sizeof(amd_fw_entry); i++) {
		if (amd_fw_table[i].type == type) {
			amd_fw_table[i].filename = filename;
			return;
		}
	}

	for (i = 0; i < sizeof(amd_psp_fw_table) / sizeof(amd_fw_entry); i++) {
		if (amd_psp_fw_table[i].type != type)
			continue;

		if (amd_psp_fw_table[i].subprog == sub) {
			amd_psp_fw_table[i].filename = filename;
			return;
		}
	}
}

int main(int argc, char **argv)
{
	int c;
	int retval = 0;
	char *tmp;
	char *rom = NULL;
	embedded_firmware *amd_romsig;
	psp_directory_table *pspdir;
	int comboable = 0;
	int targetfd;
	char *output = NULL;
	context ctx = {
		.rom_size = CONFIG_ROM_SIZE,
	};
	uint32_t dir_location = 0;
	uint32_t romsig_offset;
	uint32_t rom_base_address;
	uint8_t sub = 0;

	while (1) {
		int optindex = 0;

		c = getopt_long(argc, argv, optstring, long_options, &optindex);

		if (c == -1)
			break;

		switch (c) {
		case 'x':
			register_fw_filename(AMD_FW_XHCI, sub, optarg);
			sub = 0; /* subprogram is N/A but clear anyway */
			break;
		case 'i':
			register_fw_filename(AMD_FW_IMC, sub, optarg);
			sub = 0; /* subprogram is N/A but clear anyway */
			break;
		case 'g':
			register_fw_filename(AMD_FW_GEC, sub, optarg);
			sub = 0; /* subprogram is N/A but clear anyway */
			break;
		case 'A':
			comboable = 1;
			break;
		case 'S':
			sub = (uint8_t)strtoul(optarg, &tmp, 16);
			break;
		case 'p':
			register_fw_filename(AMD_FW_PSP_PUBKEY, sub, optarg);
			sub = 0;
			break;
		case 'b':
			register_fw_filename(AMD_FW_PSP_BOOTLOADER,
								sub, optarg);
			sub = 0;
			break;
		case 's':
			register_fw_filename(AMD_FW_PSP_SMU_FIRMWARE,
								sub, optarg);
			sub = 0;
			break;
		case 'r':
			register_fw_filename(AMD_FW_PSP_RECOVERY, sub, optarg);
			sub = 0;
			break;
		case 'k':
			register_fw_filename(AMD_FW_PSP_RTM_PUBKEY,
								sub, optarg);
			sub = 0;
			break;
		case 'c':
			register_fw_filename(AMD_FW_PSP_SECURED_OS,
								sub, optarg);
			sub = 0;
			break;
		case 'n':
			register_fw_filename(AMD_FW_PSP_NVRAM, sub, optarg);
			sub = 0;
			break;
		case 'd':
			register_fw_filename(AMD_FW_PSP_SECURED_DEBUG,
								sub, optarg);
			sub = 0;
			break;
		case 't':
			register_fw_filename(AMD_FW_PSP_TRUSTLETS, sub, optarg);
			sub = 0;
			break;
		case 'u':
			register_fw_filename(AMD_FW_PSP_TRUSTLETKEY,
								sub, optarg);
			sub = 0;
			break;
		case 'w':
			register_fw_filename(AMD_FW_PSP_SMU_FIRMWARE2,
								sub, optarg);
			sub = 0;
			break;
		case 'm':
			register_fw_filename(AMD_FW_PSP_SMUSCS, sub, optarg);
			sub = 0;
			break;
		case 'o':
			output = optarg;
			break;
		case 'f':
			ctx.rom_size = (uint32_t)strtoul(optarg, &tmp, 16);
			if (*tmp != '\0') {
				printf("Error: ROM size specified"
					" incorrectly (%s)\n\n", optarg);
				retval = 1;
			}
			break;
		case 'l':
			dir_location = (uint32_t)strtoul(optarg, &tmp, 16);
			if (*tmp != '\0') {
				printf("Error: Directory Location specified"
					" incorrectly (%s)\n\n", optarg);
				retval = 1;
			}
			break;

		case 'h':
			usage();
			return 0;
		default:
			break;
		}
	}

	if (!output) {
		printf("Error: Output value is not specified.\n\n");
		retval = 1;
	}

	if (ctx.rom_size % 1024 != 0) {
		printf("Error: ROM Size (%d bytes) should be a multiple of"
			" 1024 bytes.\n\n", ctx.rom_size);
		retval = 1;
	}

	if (ctx.rom_size < MIN_ROM_KB * 1024) {
		printf("Error: ROM Size (%dKB) must be at least %dKB.\n\n",
			ctx.rom_size / 1024, MIN_ROM_KB);
		retval = 1;
	}

	if (retval) {
		usage();
		return retval;
	}

	printf("    AMDFWTOOL  Using ROM size of %dKB\n", ctx.rom_size / 1024);

	rom_base_address = 0xFFFFFFFF - ctx.rom_size + 1;
	if (dir_location && (dir_location < rom_base_address)) {
		printf("Error: Directory location outside of ROM.\n\n");
		return 1;
	}

	switch (dir_location) {
	case 0:          /* Fall through */
	case 0xFFFA0000: /* Fall through */
	case 0xFFF20000: /* Fall through */
	case 0xFFE20000: /* Fall through */
	case 0xFFC20000: /* Fall through */
	case 0xFF820000: /* Fall through */
	case 0xFF020000: /* Fall through */
		break;
	default:
		printf("Error: Invalid Directory location.\n");
		printf("  Valid locations are 0xFFFA0000, 0xFFF20000,\n");
		printf("  0xFFE20000, 0xFFC20000, 0xFF820000, 0xFF020000\n");
		return 1;
	}

	ctx.rom = malloc(ctx.rom_size);
	if (!ctx.rom) {
		printf("Error: Failed to allocate memory\n");
		return 1;
	}
	memset(ctx.rom, 0xFF, ctx.rom_size);

	if (dir_location)
		romsig_offset = ctx.current = dir_location - rom_base_address;
	else
		romsig_offset = ctx.current = AMD_ROMSIG_OFFSET;
	printf("    AMDFWTOOL  Using firmware directory location of 0x%08x\n",
			RUN_CURRENT(ctx));

	amd_romsig = BUFF_OFFSET(ctx, romsig_offset);
	amd_romsig->signature = EMBEDDED_FW_SIGNATURE;
	amd_romsig->imc_entry = 0;
	amd_romsig->gec_entry = 0;
	amd_romsig->xhci_entry = 0;

	integrate_firmwares(&ctx, amd_romsig, amd_fw_table);

	ctx.current = ALIGN(ctx.current, 0x10000U); /* todo: is necessary? */

	pspdir = new_psp_dir(&ctx);
	integrate_psp_firmwares(&ctx, pspdir, amd_psp_fw_table);

	if (comboable)
		amd_romsig->comboable = BUFF_TO_RUN(ctx, pspdir);
	else
		amd_romsig->psp_entry = BUFF_TO_RUN(ctx, pspdir);

#if PSP_COMBO
	psp_combo_directory *combo_dir = new_combo_dir(&ctx);
	amd_romsig->comboable = BUFF_TO_RUN(ctx, combo_dir);
	/* 0 -Compare PSP ID, 1 -Compare chip family ID */
	combo_dir->entries[0].id_sel = 0;
	/* TODO: PSP ID. Documentation is needed. */
	combo_dir->entries[0].id = 0x10220B00;
	combo_dir->entries[0].lvl2_addr = BUFF_TO_RUN(ctx, pspdir);

	combo_dir->header.lookup = 1;
	fill_dir_header(combo_dir, 1, PSP2_COOKIE);
#endif

	targetfd = open(output, O_RDWR | O_CREAT | O_TRUNC, 0666);
	if (targetfd >= 0) {
		write(targetfd, amd_romsig, ctx.current - romsig_offset);
		close(targetfd);
	} else {
		printf("Error: could not open file: %s\n", output);
		retval = 1;
	}

	free(rom);
	return retval;
}
