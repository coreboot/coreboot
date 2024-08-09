// SPDX-License-Identifier: GPL-2.0+
#define _GNU_SOURCE
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include <endian.h>
#include <errno.h>
#include <limits.h>

#define NOSIZE ((size_t)-1)

extern uint32_t crc32(uint32_t iv, uint32_t sv, const void *data, size_t n);
extern uint32_t crc32_final(uint32_t iv);

/* all uint32_t ends up little endian in output header */
struct __attribute__((__packed__)) ubootsplhdr {
	uint32_t sofs;		/* offset of spl header: 64+256+256 = 0x240 */
	uint32_t bofs;		/* SBL_BAK_OFFSET: Offset of backup SBL from Flash info start (from input_sbl_normal.cfg) */
	uint8_t  zro2[636];
	uint32_t vers;		/* version: shall be 0x01010101
				 * (from https://doc-en.rvspace.org/VisionFive2/SWTRM/VisionFive2_SW_TRM/create_spl.html) */
	uint32_t fsiz;		/* u-boot-spl.bin size in bytes */
	uint32_t res1;		/* Offset from HDR to SPL_IMAGE, 0x400 (00 04 00 00) currently */
	uint32_t crcs;		/* CRC32 of u-boot-spl.bin */
	uint8_t  zro3[364];
};

struct hdr_conf_t {
	const char name[PATH_MAX];
	uint32_t vers;
	uint32_t bofs;
	bool creat_hdr_flag;
	bool fixed_img_hdr_flag;
};

static struct ubootsplhdr ubsplhdr;
static struct ubootsplhdr imghdr;
static struct hdr_conf_t g_hdr_conf;

static char ubootspl[181072-sizeof(struct ubootsplhdr)+1];
static char outpath[PATH_MAX];

#define DEFVERSID 0x01010101
#define DEFBACKUP 0x200000U
#define CRCFAILED 0x5A5A5A5A

static void xerror(int errnoval, const char *s)
{
	if (errnoval) perror(s);
	else fprintf(stderr, "%s\n", s);
	exit(2);
}

static void usage(void)
{
	const char help[] = {
		"\n            StarFive spl tool\n\n"
		"usage:\n"
		"-c, --creat-splhdr	creat spl hdr\n"
		"-i, --fix-imghdr	fixed img hdr for emmc boot.\n"
		"-a, --spl-bak-addr	set backup SPL addr(default: 0x200000)\n"
		"-v, --version		set version (default: 0x01010101)\n"
		"-f, --file		input file name(spl/img)\n"
		"-h, --help		show this information\n"
	};
	puts(help);
}

static int parse_args(int argc, char **argv)
{
	uint32_t v;

	enum {
		OPTION_CREAD_HDR = 1,
		OPTION_FIXED_HDR,
		OPTION_SBL_BAK_OFFSET,
		OPTION_VERSION,
		OPTION_FILENAME,
		OPTION_HELP,
	};

	static struct option long_options[] =
	{
		{"creat-splhdr" , no_argument, NULL, OPTION_CREAD_HDR},
		{"fix-imghdr" , no_argument, NULL, OPTION_FIXED_HDR},
		{"spl-bak-addr" , required_argument, NULL, OPTION_SBL_BAK_OFFSET},
		{"version", required_argument, NULL, OPTION_VERSION},
		{"file", required_argument, NULL, OPTION_FILENAME},
		{"help", no_argument, NULL, OPTION_HELP},
		{0, 0, 0, 0}
	};

	while (1)
	{
		/* getopt_long stores the option index here. */
		int option_index = 0;

		int c = getopt_long(argc, argv, "cio:v:f:h", long_options, &option_index);

		/* Detect the end of the options. */
		if (c == -1)
		    break;

		switch (c) {
		case 0:
			/* If this option set a flag, do nothing else now. */
			if (long_options[option_index].flag != 0)
			break;

		case 'c':
		case OPTION_CREAD_HDR:
			g_hdr_conf.creat_hdr_flag = true;
			g_hdr_conf.fixed_img_hdr_flag = false;
			break;

		case 'i':
		case OPTION_FIXED_HDR:
			g_hdr_conf.fixed_img_hdr_flag = true;
			g_hdr_conf.creat_hdr_flag = false;
			break;

		case 'a':
		case OPTION_SBL_BAK_OFFSET:
			v = (uint32_t)strtoul(optarg, NULL, 16);
			v = htole32(v);
			g_hdr_conf.bofs = v;
			break;

		case 'v':
		case OPTION_VERSION:
			v = (uint32_t)strtoul(optarg, NULL, 16);
			v = htole32(v);
			g_hdr_conf.vers = v;
			break;

		case 'f':
		case OPTION_FILENAME:
			strcpy((char*)g_hdr_conf.name, optarg);
			break;

		case 'h':
		case OPTION_HELP:
			usage();
			break;

		default:
			usage();
			break;
		}
	}
	return 0;
}

int spl_creat_hdr(struct hdr_conf_t *conf)
{
	int fd;
	uint32_t v;
	size_t sz;

	if (!conf->creat_hdr_flag)
		return 0;

	ubsplhdr.sofs = htole32(0x240U);
	ubsplhdr.res1 = htole32(0x400U);
	ubsplhdr.bofs = conf->bofs ? conf->bofs : htole32(DEFBACKUP);
	ubsplhdr.vers = conf->vers ? conf->vers : htole32(DEFVERSID);

	printf("ubsplhdr.sofs:%#x, ubsplhdr.bofs:%#x, ubsplhdr.vers:%#x name:%s\n",
		ubsplhdr.sofs, ubsplhdr.bofs, ubsplhdr.vers, conf->name);

	fd = open(conf->name, O_RDONLY);
	if (fd == -1) xerror(errno, conf->name);

	sz = (size_t)read(fd, ubootspl, sizeof(ubootspl));
	if (sz == NOSIZE) xerror(errno, conf->name);
	if (sz >= (sizeof(ubootspl)))
		xerror(0, "File too large! Please rebuild your SPL with -Os. Maximum allowed size is 180048 bytes.");
	v = htole32((uint32_t)sz);
	ubsplhdr.fsiz = v;

	close(fd);
	snprintf(outpath, sizeof(outpath), "%s.normal.out", conf->name);
	fd = creat(outpath, 0666);
	if (fd == -1) xerror(errno, outpath);

	v = crc32(~0U, 0x04c11db7U, ubootspl, sz);
	v = crc32_final(v);
	v = htole32(v);
	ubsplhdr.crcs = v;

	write(fd, &ubsplhdr, sizeof(struct ubootsplhdr));
	write(fd, ubootspl, sz);

	close(fd);

	printf("SPL written to %s successfully.\n", outpath);

	return 0;
}

int img_fixed_hdr(struct hdr_conf_t *conf)
{
	int fd;
	size_t sz;

	if (!conf->fixed_img_hdr_flag)
		return 0;

	fd = open(conf->name, O_RDWR);
	if (fd == -1) xerror(errno, conf->name);

	sz = (size_t)read(fd, &imghdr, sizeof(imghdr));
	if (sz == NOSIZE) xerror(errno, conf->name);

	/* When starting with emmc, bootrom will read 0x0 instead of partition 0. (Known issues).
	Read GPT PMBR+Header, then write the backup address at 0x4, and write the wrong CRC
	check value at 0x290, so that bootrom CRC check fails and jump to the backup address
	to load the real spl. */

	imghdr.bofs = conf->bofs ? conf->bofs : htole32(DEFBACKUP);
	imghdr.crcs = htole32(CRCFAILED);

	lseek(fd, 0x0, SEEK_SET);
	write(fd, &imghdr, sizeof(imghdr));
	close(fd);

	printf("IMG  %s fixed hdr successfully.\n", conf->name);

	return 0;
}

int main(int argc, char **argv)
{
	parse_args(argc, argv);
	spl_creat_hdr(&g_hdr_conf);
	img_fixed_hdr(&g_hdr_conf);
}
