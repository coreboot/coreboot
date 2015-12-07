/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2015 Advanced Micro Devices, Inc.
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
 *  +------------+---------------+----------------+------------+
 *  | 0x55AA55AA |EC ROM Address |GEC ROM Address |USB3 ROM    |
 *  +------------+---------------+----------------+------------+
 *  | PSPDIR ADDR|PSP2DIR ADDR   |
 *  +------------+---------------+
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
 *  PSP2 directory
 *  +------------+---------------+----------------+------------+
 *  | 'PSP2'     | Fletcher      |    Count       | Reserved   |
 *  +------------+---------------+----------------+------------+
 *  |     1      | PSP ID        |   PSPDIR ADDR  |            | 2nd PSP directory
 *  +------------+---------------+----------------+------------+
 *  |     2      | PSP ID        |   PSPDIR ADDR  |            | 3rd PSP directory
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

#define ROM_BASE_ADDRESS  (0xFFFFFFFF - CONFIG_ROM_SIZE + 1)
#define AMD_ROMSIG_OFFSET 0x20000

#define ALIGN(val, by) (((val) + (by)-1)&~((by)-1))

/*
  Reserved for future.
  TODO: PSP2 is for Combo BIOS, which is the idea that one image supports 2
  kinds of APU.
*/
#define PSP2 1

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
uint32_t fletcher32 (const uint16_t *pptr, int length)
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

void usage()
{
	printf("Create AMD Firmware combination\n");
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
} amd_fw_type;

typedef struct _amd_fw_entry {
	amd_fw_type type;
	char *filename;
} amd_fw_entry;

amd_fw_entry amd_psp_fw_table[] = {
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
	{ .type = AMD_FW_PSP_SMU_FIRMWARE2 },
	{ .type = AMD_FW_PSP_SMUSCS },
	{ .type = AMD_PSP_FUSE_CHAIN },
};

#if PSP2
amd_fw_entry amd_psp2_fw_table[] = {
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
	{ .type = AMD_FW_PSP_SMU_FIRMWARE2 },
	{ .type = AMD_FW_PSP_SMUSCS },
	{ .type = AMD_PSP_FUSE_CHAIN },
};
#endif

amd_fw_entry amd_fw_table[] = {
	{ .type = AMD_FW_XHCI },
	{ .type = AMD_FW_IMC },
	{ .type = AMD_FW_GEC },
};

void fill_psp_head(uint32_t *pspdir, int count)
{
	pspdir[0] = 0x50535024;	/* 'PSP$' */
	pspdir[2] = count;		/* size */
	pspdir[3] = 0;
	pspdir[1] = fletcher32((uint16_t *)&pspdir[1], (count *16 + 16)/2 - 2);
}

uint32_t integerate_one_fw(void *base, uint32_t pos, uint32_t *romsig, int i)
{
	int fd;
	struct stat fd_stat;

	if (amd_fw_table[i].filename != NULL) {
		fd = open (amd_fw_table[i].filename, O_RDONLY);
		fstat(fd, &fd_stat);

		switch (amd_fw_table[i].type) {
		case AMD_FW_IMC:
			pos = ALIGN(pos, 0x10000);
			romsig[1] = pos + ROM_BASE_ADDRESS;
			break;
		case AMD_FW_GEC:
			romsig[2] = pos + ROM_BASE_ADDRESS;
			break;
		case AMD_FW_XHCI:
			romsig[3] = pos + ROM_BASE_ADDRESS;
			break;
		default:
			/* Error */
			break;
		}

		read (fd, base+pos, fd_stat.st_size);

		pos += fd_stat.st_size;
		pos = ALIGN(pos, 0x100);
		close (fd);
	}

	return pos;
}

uint32_t integerate_one_psp(void *base, uint32_t pos, uint32_t *pspdir, int i)
{
	int fd;
	struct stat fd_stat;

	if (amd_psp_fw_table[i].type == AMD_PSP_FUSE_CHAIN) {
		pspdir[4+4*i+0] = amd_psp_fw_table[i].type;
		pspdir[4+4*i+1] = 0xFFFFFFFF;
		pspdir[4+4*i+2] = 1;
		pspdir[4+4*i+3] = 0;
	} else if (amd_psp_fw_table[i].filename != NULL) {
		pspdir[4+4*i+0] = amd_psp_fw_table[i].type;

		fd = open (amd_psp_fw_table[i].filename, O_RDONLY);
		fstat(fd, &fd_stat);
		pspdir[4+4*i+1] = fd_stat.st_size;

		pspdir[4+4*i+2] = pos + ROM_BASE_ADDRESS;
		pspdir[4+4*i+3] = 0;

		read (fd, base+pos, fd_stat.st_size);

		pos += fd_stat.st_size;
		pos = ALIGN(pos, 0x100);
		close (fd);
	} else {
		/* This APU doesn't have this firmware. */
	}

	return pos;
}

static const char *optstring  = "x:i:g:p:b:s:r:k:o:n:d:t:u:w:m:h";
static struct option long_options[] = {
	{"xhci",         required_argument, 0, 'x' },
	{"imc",          required_argument, 0, 'i' },
	{"gec",          required_argument, 0, 'g' },
	/* PSP */
	{"pubkey",       required_argument, 0, 'p' },
	{"bootloader",   required_argument, 0, 'b' },
	{"smufirmware",  required_argument, 0, 's' },
	{"recovery",     required_argument, 0, 'r' },
	{"rtmpubkey",    required_argument, 0, 'k' },
	{"secureos",     required_argument, 0, 'c' },
	{"nvram",        required_argument, 0, 'n' },
	{"securedebug",  required_argument, 0, 'd' },
	{"trustlets",    required_argument, 0, 't' },
	{"trustletkey",  required_argument, 0, 'u' },
	{"smufirmware2", required_argument, 0, 'w' },
	{"smuscs",       required_argument, 0, 'm' },

	/* TODO: PSP2 */
#if PSP2
	{"pubkey2",       required_argument, 0, 'P' },
	{"bootloader2",   required_argument, 0, 'B' },
	{"smufirmware2",  required_argument, 0, 'S' },
	{"recovery2",     required_argument, 0, 'R' },
	{"rtmpubkey2",    required_argument, 0, 'K' },
	{"secureos2",     required_argument, 0, 'C' },
	{"nvram2",        required_argument, 0, 'N' },
	{"securedebug2",  required_argument, 0, 'D' },
	{"trustlets2",    required_argument, 0, 'T' },
	{"trustletkey2",  required_argument, 0, 'U' },
	{"smufirmware2_2",required_argument, 0, 'W' },
	{"smuscs2",       required_argument, 0, 'M' },
#endif

	{"output",       required_argument, 0, 'o' },
	{"help",         no_argument,       0, 'h' },

	{NULL,           0,                 0,  0  }
};

void register_fw_filename(amd_fw_type type, char filename[], int pspflag)
{
	int i;

	for (i = 0; i < sizeof(amd_fw_table)/sizeof(amd_fw_entry); i++) {
		if (amd_fw_table[i].type == type) {
			amd_fw_table[i].filename = filename;
			return;
		}
	}

	if (pspflag == 1) {
		for (i = 0; i < sizeof(amd_psp_fw_table)/sizeof(amd_fw_entry); i++) {
			if (amd_psp_fw_table[i].type == type) {
				amd_psp_fw_table[i].filename = filename;
				return;
			}
		}
	}

#if PSP2
	if (pspflag == 2) {
		for (i = 0; i < sizeof(amd_psp2_fw_table)/sizeof(amd_fw_entry); i++) {
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
	int c, count, pspflag = 0;
#if PSP2
	int psp2flag = 0;
	int psp2count;
	uint32_t *psp2dir;
#endif
	void *rom = NULL;
	uint32_t current;
	uint32_t *amd_romsig, *pspdir;

	int targetfd;
	char *output;

	rom = malloc(CONFIG_ROM_SIZE);
	memset (rom, 0xFF, CONFIG_ROM_SIZE);
	if (!rom) {
		return 1;
	}

	current = AMD_ROMSIG_OFFSET;
	amd_romsig = rom + AMD_ROMSIG_OFFSET;
	amd_romsig[0] = 0x55AA55AA; /* romsig */
	amd_romsig[1] = 0;
	amd_romsig[2] = 0;
	amd_romsig[3] = 0;

	current += 0x20;	    /* size of ROMSIG */

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
			register_fw_filename(AMD_FW_PSP_SMU_FIRMWARE, optarg, 1);
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
			register_fw_filename(AMD_FW_PSP_SECURED_DEBUG, optarg, 1);
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
			register_fw_filename(AMD_FW_PSP_SMU_FIRMWARE2, optarg, 1);
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
			register_fw_filename(AMD_FW_PSP_SMU_FIRMWARE, optarg, 2);
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
			register_fw_filename(AMD_FW_PSP_SECURED_DEBUG, optarg, 2);
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
			register_fw_filename(AMD_FW_PSP_SMU_FIRMWARE2, optarg, 2);
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
		case 'h':
			usage();
			return 1;
		default:
			break;
		}
	}

	current = ALIGN(current, 0x100);
	for (count = 0; count < sizeof(amd_fw_table) / sizeof(amd_fw_entry); count ++) {
		current = integerate_one_fw(rom, current, amd_romsig, count);
	}

	if (pspflag == 1) {
		current = ALIGN(current, 0x10000);
		pspdir = rom + current;
		amd_romsig[4] = current + ROM_BASE_ADDRESS;

		current += 0x200;	/* Conservative size of pspdir */
		for (count = 0; count < sizeof(amd_psp_fw_table) / sizeof(amd_fw_entry); count ++) {
			current = integerate_one_psp(rom, current, pspdir, count);
		}

		fill_psp_head(pspdir, count);

#if PSP2
		if (psp2flag == 1) {
			current = ALIGN(current, 0x10000); /* PSP2 dir */
			psp2dir = rom + current;
			amd_romsig[5] = current + ROM_BASE_ADDRESS;
			current += 0x100;	/* Add conservative size of psp2dir. */

			/* TODO: remove the hardcode. */
			psp2count = 1;		/* Start from 1. */
			/* for (; psp2count <= PSP2COUNT; psp2count++, current=ALIGN(current, 0x100)) { */
			psp2dir[psp2count*4 + 0] = psp2count;		/* PSP Number */
			psp2dir[psp2count*4 + 1] = 0x10220B00; /* TODO: PSP ID. Documentation is needed. */
			psp2dir[psp2count*4 + 2] = current + ROM_BASE_ADDRESS;
			pspdir = rom + current;
			psp2dir[psp2count*4 + 3] = 0;

			current += 0x200;	/* Add conservative size of pspdir. Start of PSP entries. */
			for (count = 0; count < sizeof(amd_psp2_fw_table) / sizeof(amd_fw_entry); count ++) {
				current = integerate_one_psp(rom, current, pspdir, count);
			}
			fill_psp_head(pspdir, count);
			/* } */ /* End of loop */

			/* fill the PSP2 head */
			psp2dir[0] = 0x50535032;  /* 'PSP2' */
			psp2dir[2] = psp2count;		  /* Count */
			psp2dir[3] = 0;
			psp2dir[1] = fletcher32((uint16_t *)&psp2dir[1], (psp2count*16 + 16)/2 - 2);
		}
#endif
	}

	targetfd = open(output, O_RDWR | O_CREAT | O_TRUNC, 0666);
	write(targetfd, amd_romsig, current - AMD_ROMSIG_OFFSET);
	close(targetfd);
	free(rom);

	return 0;
}
