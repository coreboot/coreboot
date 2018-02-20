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

/*
  Reserved for future.
  TODO: PSP2 is for Combo BIOS, which is the idea that one image supports 2
  kinds of APU.
*/
#define PSP2 1
#if PSP2
/* Use PSP combo directory or not.
 * Currently we dont have to squeeze 3 PSP directories into 1 image. So
 * we skip the combo directory.
 */
   #define PSP_COMBO 0
#endif

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
static uint32_t fletcher32(const uint16_t *pptr, int length)
{
	uint32_t c0;
	uint32_t c1;
	uint32_t checksum;
	int index;

	c0 = 0xFFFF;
	c1 = 0xFFFF;

	for (index = 0; index < length; index++) {
		/*
		* Ignore the contents of the checksum field.
		*/
		c0 += *(pptr++);
		c1 += c0;
		if ((index % 360) == 0) {
			c0 = (c0 & 0xFFFF) + (c0 >> 16);	// Sum0 modulo 65535 + the overflow
			c1 = (c1 & 0xFFFF) + (c1 >> 16);	// Sum1 modulo 65535 + the overflow
		}
	}

	c0 = (c0 & 0xFFFF) + (c0 >> 16);	// Sum0 modulo 65535 + the overflow
	c1 = (c1 & 0xFFFF) + (c1 >> 16);	// Sum1 modulo 65535 + the overflow
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
	printf("-p | --pubkey <FILE>           Add pubkey\n");
	printf("-b | --bootloader <FILE>       Add bootloader\n");
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

#if PSP2
	printf("\nPSP2 options:\n");
	printf("-P | --pubkey2 <FILE>          Add pubkey\n");
	printf("-B | --bootloader2 <FILE>      Add bootloader\n");
	printf("-S | --smufirmware_2 <FILE>    Add smufirmware\n");
	printf("-L | --smufnfirmware_2 <FILE>  Add fanless smufirmware\n");
	printf("-R | --recovery2 <FILE>        Add recovery\n");
	printf("-K | --rtmpubkey2 <FILE>       Add rtmpubkey\n");
	printf("-C | --secureos2 <FILE>        Add secureos\n");
	printf("-N | --nvram2 <FILE>           Add nvram\n");
	printf("-D | --securedebug2 <FILE>     Add securedebug\n");
	printf("-T | --trustlets2 <FILE>       Add trustlets\n");
	printf("-U | --trustletkey2 <FILE>     Add trustletkey\n");
	printf("-W | --smufirmware2_2 <FILE>   Add smufirmware2\n");
	printf("-E | --smufnfirmware2_2 <FILE> Add fanless smufirmware2\n");
	printf("-M | --smuscs2 <FILE>          Add smuscs\n");
#endif

	printf("\n-o | --output <filename>     output filename\n");
	printf("-f | --flashsize <HEX_VAL>     ROM size in bytes\n");
	printf("                               size must be larger than %dKB\n",
		MIN_ROM_KB);
	printf("                               and must a multiple of 1024\n");
	printf("-l | --location                Location of Directory\n");
	printf("-h | --help                    show this help\n");

}

#define FANLESS_FW 0x100 /* type[15:8]: 0=non-fanless OPNs, 1=fanless */

typedef enum _amd_fw_type {
	AMD_FW_PSP_PUBKEY = 0,
	AMD_FW_PSP_BOOTLOADER = 1,
	AMD_FW_PSP_SMU_FIRMWARE = 8,
	AMD_FW_PSP_SMU_FN_FIRMWARE = FANLESS_FW + AMD_FW_PSP_SMU_FIRMWARE,
	AMD_FW_PSP_RECOVERY = 3,
	AMD_FW_PSP_RTM_PUBKEY = 5,
	AMD_FW_PSP_SECURED_OS = 2,
	AMD_FW_PSP_NVRAM = 4,
	AMD_FW_PSP_SECURED_DEBUG = 9,
	AMD_FW_PSP_TRUSTLETS = 12,
	AMD_FW_PSP_TRUSTLETKEY = 13,
	AMD_FW_PSP_SMU_FIRMWARE2 = 18,
	AMD_FW_PSP_SMU_FN_FIRMWARE2 = FANLESS_FW + AMD_FW_PSP_SMU_FIRMWARE2,
	AMD_PSP_FUSE_CHAIN = 11,
	AMD_FW_PSP_SMUSCS = 95,

	AMD_FW_IMC,
	AMD_FW_GEC,
	AMD_FW_XHCI,
	AMD_FW_INVALID,
} amd_fw_type;

typedef struct _amd_fw_entry {
	amd_fw_type type;
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
	{ .type = AMD_FW_PSP_SMU_FN_FIRMWARE },
	{ .type = AMD_FW_PSP_SMU_FN_FIRMWARE2 },
	{ .type = AMD_FW_PSP_SMU_FIRMWARE2 },
	{ .type = AMD_FW_PSP_SMUSCS },
	{ .type = AMD_PSP_FUSE_CHAIN },
	{ .type = AMD_FW_INVALID },
};

#if PSP2
static amd_fw_entry amd_psp2_fw_table[] = {
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
	{ .type = AMD_FW_PSP_SMU_FN_FIRMWARE },
	{ .type = AMD_FW_PSP_SMU_FN_FIRMWARE2 },
	{ .type = AMD_FW_PSP_SMU_FIRMWARE2 },
	{ .type = AMD_FW_PSP_SMUSCS },
	{ .type = AMD_PSP_FUSE_CHAIN },
	{ .type = AMD_FW_INVALID },
};
#endif

static amd_fw_entry amd_fw_table[] = {
	{ .type = AMD_FW_XHCI },
	{ .type = AMD_FW_IMC },
	{ .type = AMD_FW_GEC },
	{ .type = AMD_FW_INVALID },
};

static void fill_psp_head(uint32_t *pspdir, uint32_t count)
{
	pspdir[0] = 0x50535024;		/* 'PSP$' */
	pspdir[2] = count;		/* size */
	pspdir[3] = 0;
	pspdir[1] = fletcher32((uint16_t *)&pspdir[1],
			(count * 16 + 16) / 2 - 2);
}

static uint32_t integrate_firmwares(char *base, uint32_t pos, uint32_t *romsig,
				    amd_fw_entry *fw_table, uint32_t rom_size)
{
	int fd;
	ssize_t bytes;
	struct stat fd_stat;
	int i;
	uint32_t rom_base_address = 0xFFFFFFFF - rom_size + 1;

	for (i = 0; fw_table[i].type != AMD_FW_INVALID; i++) {
		if (fw_table[i].filename != NULL) {
			fd = open(fw_table[i].filename, O_RDONLY);
			if (fd < 0) {
				printf("Error: %s\n", strerror(errno));
				free(base);
				exit(1);
			}
			if (fstat(fd, &fd_stat)) {
				printf("fstat error: %s\n", strerror(errno));
				free(base);
				exit(1);
			}

			switch (fw_table[i].type) {
			case AMD_FW_IMC:
				pos = ALIGN(pos, 0x10000U);
				romsig[1] = pos + rom_base_address;
				break;
			case AMD_FW_GEC:
				romsig[2] = pos + rom_base_address;
				break;
			case AMD_FW_XHCI:
				romsig[3] = pos + rom_base_address;
				break;
			default:
				/* Error */
				break;
			}

			if (pos + fd_stat.st_size > rom_size) {
				printf("Error: Specified ROM size of %d"
					" will not fit %s.  Exiting.\n",
					rom_size, fw_table[i].filename);
				free(base);
				exit(1);
			}

			bytes = read(fd, (void *)(base + pos),
					(size_t)fd_stat.st_size);
			if (bytes == (ssize_t)fd_stat.st_size)
				pos += fd_stat.st_size;
			else {
				printf("Error while reading %s\n",
					fw_table[i].filename);
				free(base);
				exit(1);
			}

			close(fd);
			pos = ALIGN(pos, 0x100U);
		}
	}

	return pos;
}

static uint32_t integrate_psp_firmwares(char *base, uint32_t pos,
					uint32_t *pspdir,
					amd_fw_entry *fw_table,
					uint32_t rom_size)
{
	int fd;
	ssize_t bytes;
	struct stat fd_stat;
	unsigned int i;
	uint32_t rom_base_address = 0xFFFFFFFF - rom_size + 1;

	for (i = 0; fw_table[i].type != AMD_FW_INVALID; i++) {
		if (fw_table[i].type == AMD_PSP_FUSE_CHAIN) {
			pspdir[4+4*i+0] = fw_table[i].type;
			pspdir[4+4*i+1] = 0xFFFFFFFF;
			pspdir[4+4*i+2] = 1;
			pspdir[4+4*i+3] = 0;
		} else if (fw_table[i].filename != NULL) {
			pspdir[4+4*i+0] = fw_table[i].type;

			fd = open(fw_table[i].filename, O_RDONLY);
			if (fd < 0) {
				printf("Error: %s\n", strerror(errno));
				free(base);
				exit(1);
			}
			if (fstat(fd, &fd_stat)) {
				printf("fstat error: %s\n", strerror(errno));
				free(base);
				exit(1);
			}
			pspdir[4+4*i+1] = (uint32_t)fd_stat.st_size;

			pspdir[4+4*i+2] = pos + rom_base_address;
			pspdir[4+4*i+3] = 0;

			if (pos + fd_stat.st_size > rom_size) {
				printf("Error: Specified ROM size of %d"
					" will not fit %s.  Exiting.\n",
					rom_size, fw_table[i].filename);
				free(base);
				exit(1);
			}

			bytes = read(fd, (void *)(base + pos),
					(size_t)fd_stat.st_size);
			if (bytes == (ssize_t)fd_stat.st_size)
				pos += fd_stat.st_size;
			else {
				printf("Error while reading %s\n",
					fw_table[i].filename);
				free(base);
				exit(1);
			}

			close(fd);
			pos = ALIGN(pos, 0x100U);
		} else {
			/* This APU doesn't have this firmware. */
		}
	}
	fill_psp_head(pspdir, i);
	return pos;
}

#if PSP2
static const char *optstring  =
	"x:i:g:p:b:s:r:k:c:n:d:t:u:w:m:P:B:S:L:R:K:C:N:D:T:U:W:E:M:o:f:l:h";
#else
static const char *optstring  = "x:i:g:p:b:s:r:k:c:n:d:t:u:w:m:o:f:l:h";
#endif

static struct option long_options[] = {
	{"xhci",             required_argument, 0, 'x' },
	{"imc",              required_argument, 0, 'i' },
	{"gec",              required_argument, 0, 'g' },
	/* PSP */
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

	/* TODO: PSP2 */
#if PSP2
	{"pubkey2",          required_argument, 0, 'P' },
	{"bootloader2",      required_argument, 0, 'B' },
	{"smufirmware_2",    required_argument, 0, 'S' },
	{"smufnfirmware_2",  required_argument, 0, 'L' },
	{"recovery2",        required_argument, 0, 'R' },
	{"rtmpubkey2",       required_argument, 0, 'K' },
	{"secureos2",        required_argument, 0, 'C' },
	{"nvram2",           required_argument, 0, 'N' },
	{"securedebug2",     required_argument, 0, 'D' },
	{"trustlets2",       required_argument, 0, 'T' },
	{"trustletkey2",     required_argument, 0, 'U' },
	{"smufirmware2_2",   required_argument, 0, 'W' },
	{"smufnfirmware2_2", required_argument, 0, 'E' },
	{"smuscs2",          required_argument, 0, 'M' },
#endif

	{"output",           required_argument, 0, 'o' },
	{"flashsize",        required_argument, 0, 'f' },
	{"location",         required_argument, 0, 'l' },
	{"help",             no_argument,       0, 'h' },

	{NULL,               0,                 0,  0  }
};

static void register_fw_filename(amd_fw_type type, char filename[], int pspflag)
{
	unsigned int i;

	for (i = 0; i < sizeof(amd_fw_table) / sizeof(amd_fw_entry); i++) {
		if (amd_fw_table[i].type == type) {
			amd_fw_table[i].filename = filename;
			return;
		}
	}

	if (pspflag == 1) {
		for (i = 0; i < sizeof(amd_psp_fw_table) /
				sizeof(amd_fw_entry); i++) {
			if (amd_psp_fw_table[i].type == type) {
				amd_psp_fw_table[i].filename = filename;
				return;
			}
		}
	}

#if PSP2
	if (pspflag == 2) {
		for (i = 0; i < sizeof(amd_psp2_fw_table) /
				sizeof(amd_fw_entry); i++) {
			if (amd_psp2_fw_table[i].type == type) {
				amd_psp2_fw_table[i].filename = filename;
				return;
			}
		}
	}
#endif
}

int main(int argc, char **argv)
{
	int c, pspflag = 0;
	int retval = 0;
#if PSP2
	int psp2flag = 0;
	uint32_t *psp2dir;
	char *tmp;
#endif
#if PSP_COMBO
	int psp2count;
#endif

	char *rom = NULL;
	uint32_t current;
	uint32_t *amd_romsig, *pspdir;

	int targetfd;
	char *output = NULL;
	uint32_t rom_size = CONFIG_ROM_SIZE;
	uint32_t dir_location = 0;
	uint32_t romsig_offset;
	uint32_t rom_base_address;

	while (1) {
		int optindex = 0;

		c = getopt_long(argc, argv, optstring, long_options, &optindex);

		if (c == -1)
			break;

		switch (c) {
		case 'x':
			register_fw_filename(AMD_FW_XHCI, optarg, 0);
			break;
		case 'i':
			register_fw_filename(AMD_FW_IMC, optarg, 0);
			break;
		case 'g':
			register_fw_filename(AMD_FW_GEC, optarg, 0);
			break;
		case 'p':
			register_fw_filename(AMD_FW_PSP_PUBKEY, optarg, 1);
			pspflag = 1;
			break;
		case 'b':
			register_fw_filename(AMD_FW_PSP_BOOTLOADER, optarg, 1);
			pspflag = 1;
			break;
		case 's':
			register_fw_filename(AMD_FW_PSP_SMU_FIRMWARE,
					optarg, 1);
			pspflag = 1;
			break;
		case 'r':
			register_fw_filename(AMD_FW_PSP_RECOVERY, optarg, 1);
			pspflag = 1;
			break;
		case 'k':
			register_fw_filename(AMD_FW_PSP_RTM_PUBKEY, optarg, 1);
			pspflag = 1;
			break;
		case 'c':
			register_fw_filename(AMD_FW_PSP_SECURED_OS, optarg, 1);
			pspflag = 1;
			break;
		case 'n':
			register_fw_filename(AMD_FW_PSP_NVRAM, optarg, 1);
			pspflag = 1;
			break;
		case 'd':
			register_fw_filename(AMD_FW_PSP_SECURED_DEBUG,
					optarg, 1);
			pspflag = 1;
			break;
		case 't':
			register_fw_filename(AMD_FW_PSP_TRUSTLETS, optarg, 1);
			pspflag = 1;
			break;
		case 'u':
			register_fw_filename(AMD_FW_PSP_TRUSTLETKEY, optarg, 1);
			pspflag = 1;
			break;
		case 'w':
			register_fw_filename(AMD_FW_PSP_SMU_FIRMWARE2,
					optarg, 1);
			pspflag = 1;
			break;
		case 'm':
			register_fw_filename(AMD_FW_PSP_SMUSCS, optarg, 1);
			pspflag = 1;
			break;
#if PSP2
		case 'P':
			register_fw_filename(AMD_FW_PSP_PUBKEY, optarg, 2);
			psp2flag = 1;
			break;
		case 'B':
			register_fw_filename(AMD_FW_PSP_BOOTLOADER, optarg, 2);
			psp2flag = 1;
			break;
		case 'S':
			register_fw_filename(AMD_FW_PSP_SMU_FIRMWARE,
					optarg, 2);
			psp2flag = 1;
			break;
		case 'L':
			register_fw_filename(AMD_FW_PSP_SMU_FN_FIRMWARE,
					optarg, 2);
			psp2flag = 1;
			break;
		case 'R':
			register_fw_filename(AMD_FW_PSP_RECOVERY, optarg, 2);
			psp2flag = 1;
			break;
		case 'K':
			register_fw_filename(AMD_FW_PSP_RTM_PUBKEY, optarg, 2);
			psp2flag = 1;
			break;
		case 'C':
			register_fw_filename(AMD_FW_PSP_SECURED_OS, optarg, 2);
			psp2flag = 1;
			break;
		case 'N':
			register_fw_filename(AMD_FW_PSP_NVRAM, optarg, 2);
			psp2flag = 1;
			break;
		case 'D':
			register_fw_filename(AMD_FW_PSP_SECURED_DEBUG,
					optarg, 2);
			psp2flag = 1;
			break;
		case 'T':
			register_fw_filename(AMD_FW_PSP_TRUSTLETS, optarg, 2);
			psp2flag = 1;
			break;
		case 'U':
			register_fw_filename(AMD_FW_PSP_TRUSTLETKEY, optarg, 2);
			psp2flag = 1;
			break;
		case 'W':
			register_fw_filename(AMD_FW_PSP_SMU_FIRMWARE2,
					optarg, 2);
			psp2flag = 1;
			break;
		case 'E':
			register_fw_filename(AMD_FW_PSP_SMU_FN_FIRMWARE2,
					optarg, 2);
			psp2flag = 1;
			break;
		case 'M':
			register_fw_filename(AMD_FW_PSP_SMUSCS, optarg, 2);
			psp2flag = 1;
			break;
#endif
		case 'o':
			output = optarg;
			break;
		case 'f':
			rom_size = (uint32_t)strtoul(optarg, &tmp, 16);
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

	if (!rom_size) {
		printf("Error: ROM Size is not specified.\n\n");
		retval = 1;
	}

	if (rom_size % 1024 != 0) {
		printf("Error: ROM Size (%d bytes) should be a multiple of"
			" 1024 bytes.\n\n", rom_size);
		retval = 1;
	}

	if (rom_size < MIN_ROM_KB * 1024) {
		printf("Error: ROM Size (%dKB) must be at least %dKB.\n\n",
			rom_size / 1024, MIN_ROM_KB);
		retval = 1;
	}

	if (retval) {
		usage();
		return retval;
	}

	printf("    AMDFWTOOL  Using ROM size of %dKB\n", rom_size / 1024);

	rom_base_address = 0xFFFFFFFF - rom_size + 1;
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

	rom = malloc(rom_size);
	if (!rom)
		return 1;
	memset(rom, 0xFF, rom_size);

	if (dir_location)
		romsig_offset = current = dir_location - rom_base_address;
	else
		romsig_offset = current = AMD_ROMSIG_OFFSET;
	printf("    AMDFWTOOL  Using firmware directory location of %08lx\n",
			(unsigned long)rom_base_address + current);

	amd_romsig = (void *)(rom + romsig_offset);
	amd_romsig[0] = 0x55AA55AA; /* romsig */
	amd_romsig[1] = 0;
	amd_romsig[2] = 0;
	amd_romsig[3] = 0;

	current += 0x20;	    /* size of ROMSIG */
	current = ALIGN(current, 0x1000U);
	current = integrate_firmwares(rom, current, amd_romsig,
			amd_fw_table, rom_size);

	if (pspflag == 1) {
		current = ALIGN(current, 0x10000U);
		pspdir = (void *)(rom + current);
		amd_romsig[4] = current + rom_base_address;

		current += 0x200;	/* Conservative size of pspdir */
		current = integrate_psp_firmwares(rom, current, pspdir,
				amd_psp_fw_table, rom_size);
	}

#if PSP2
	if (psp2flag == 1) {
		current = ALIGN(current, 0x10000U); /* PSP2 dir */
		psp2dir = (void *)(rom + current);
		amd_romsig[5] = current + rom_base_address;
		current += 0x200;	/* Add conservative size of psp2dir. */

#if PSP_COMBO
		/* TODO: remove the hardcode. */
		psp2count = 1;		/* Start from 1. */
		/* for (; psp2count <= PSP2COUNT; psp2count++, current=ALIGN(current, 0x100)) { */
		/* Now the psp2dir is psp combo dir. */
		psp2dir[psp2count*4 + 0 + 4] = 0; /* 0 -Compare PSP ID, 1 -Compare chip family ID */
		psp2dir[psp2count*4 + 1 + 4] = 0x10220B00; /* TODO: PSP ID. Documentation is needed. */
		psp2dir[psp2count*4 + 2 + 4] = current + rom_base_address;
		pspdir = rom + current;
		psp2dir[psp2count*4 + 3 + 4] = 0;

		current += 0x200;	/* Add conservative size of pspdir. Start of PSP entries. */
		current = integrate_psp_firmwares(rom, current, pspdir,
				amd_psp2_fw_table, rom_size);
		/* } */ /* End of loop */

		/* fill the PSP combo head */
		psp2dir[0] = 0x50535032;  /* 'PSP2' */
		psp2dir[2] = psp2count;		  /* Count */
		psp2dir[3] = 1;		/* 0-Dynamic look up through all entries, 1-PSP/chip ID match */
		psp2dir[4] = 0;		/* reserved 4 dwords. */
		psp2dir[5] = 0;
		psp2dir[6] = 0;
		psp2dir[7] = 0;
		psp2dir[1] = fletcher32((uint16_t *)&psp2dir[1],
				(psp2count * 16 + 32) / 2 - 2);
#else
		current = integrate_psp_firmwares(rom, current, psp2dir,
				amd_psp2_fw_table, rom_size);
#endif
	}
#endif

	targetfd = open(output, O_RDWR | O_CREAT | O_TRUNC, 0666);
	if (targetfd >= 0) {
		write(targetfd, amd_romsig, current - romsig_offset);
		close(targetfd);
	} else {
		printf("Error: could not open file: %s\n", output);
		retval = 1;
	}

	free(rom);
	return retval;
}
