/* SPDX-License-Identifier: GPL-2.0-only */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <getopt.h>
#include <errno.h>
#include <stdarg.h>

/* Place the following struct somewhere in the ROM: */
struct SMC_BIOS_Update {
	uint8_t		magic0[4];	// always 0xaa00b1ed
	char		magic1[4];	// always $FID
	uint8_t		magic2[2];	// always 0x7804
	uint8_t		space0;		// always zero
	// SMCinfotool doesn't care for the first letter
	// The BMC webinterface does.
	char		boardid[9];	// "100000000"
	uint8_t		space1[15];	// unknown data
	uint8_t		space2;		// always 0x1f
	char		ukn_majorVer[2];// unknown
	uint8_t		space3;		// always zero
	char		ukn_minorVer[2];// unknown
	uint8_t		space4;		// always zero
	char		majorVer[3];	// BIOS major version
	char		minorVer[2];	// BIOS minor version
	uint8_t		space5;		// always zero
	uint16_t	year;		// year
	uint8_t		month;		// month
	uint8_t		day;		// day
	uint32_t	space6;		// unknown data
	uint8_t		space7;		// all ones
	char		str[15];	// "SUPERMSMCI--MB1"
	uint8_t		space8[3];	// always zero
	uint64_t	space9[6];	// all ones
} __packed;

static const char *optstring  = "b:i:o:h";

static struct option long_options[] = {
	{"boardid",            required_argument, 0, 'b' },
	{"input",              required_argument, 0, 'i' },
	{"output",             required_argument, 0, 'o' },
	{"help",                     no_argument, 0, 'h' },
};

static void usage(void)
{
	printf("smcbiosinfo: Create BIOSInfo for BMC BIOS updates\n");
	printf("Usage: smcbiosinfo [options] -i build.h -b <boardid> -o <filename>\n");
	printf("-b | --boardid <ID>              The board ID assigned by SMC\n");
	printf("-i | --input <FILE>              The build.h file to parse\n");
	printf("-o | --output <FILE>             The file to generate\n");
	printf("-h | --help                      Print this help\n");
}

static int bcd2int(int hex)
{
	if (hex > 0xff)
		return -1;
	return ((hex & 0xF0) >> 4) * 10 + (hex & 0x0F);
}

static char *get_line(char *fn, char *match)
{
	ssize_t read;
	char *line = NULL;
	char *ret = NULL;
	size_t len = 0;

	FILE *fp = fopen(fn, "r");
	if (fp == NULL) {
		fprintf(stderr, "E: Couldn't open file '%s'\n", fn);
		return NULL;
	}

	while ((read = getline(&line, &len, fp)) != -1) {
		if (strstr(line, match) != NULL) {
			ret = strdup(strstr(line, match) + strlen(match));
			break;
		}
	}

	if (!ret)
		fprintf(stderr, "E: %s not found in %s\n", match, fn);

	fclose(fp);
	return ret;
}

static int get_line_as_int(char *fn, char *match, int bcd)
{
	int ret = -1;
	char *s = get_line(fn, match);
	if (s && strlen(s) > 0) {
		char *endptr;
		ret = strtol(s, &endptr, 0);
		if (*endptr != '\0' && *endptr != '\n') {
			fprintf(stderr, "E: Couldn't parse number for key '%s'\n", match);
			free(s);
			return -1;
		}
		if (bcd)
			ret = bcd2int(ret);
	} else {
		fprintf(stderr, "E: Got invalid line for key '%s'\n", match);
	}

	free(s);
	return ret;
}

int main(int argc, char **argv)
{
	int c;
	int ret = 1;
	char *filename = NULL;
	char *inputfilename = NULL;
	char *boardid = NULL;
	int num;

	while (1) {
		int optindex = 0;

		c = getopt_long(argc, argv, optstring, long_options, &optindex);

		if (c == -1)
			break;

		switch (c) {
		case 'b':
			boardid = strdup(optarg);
			break;
		case 'i':
			inputfilename = strdup(optarg);
			break;
		case 'o':
			filename = strdup(optarg);
			break;
		case 'h':
			ret = 0; /* fallthrough */
		case '?':
			usage();
			goto out;
		default:
			break;
		}
	}

	if (!inputfilename) {
		fprintf(stderr, "E: Must specify build.h filename\n");
		goto out;
	}
	if (!filename) {
		fprintf(stderr, "E: Must specify a destination filename\n");
		goto out;
	}

	if (!boardid || strlen(boardid) == 0) {
		fprintf(stderr, "E: Board ID must be set\n");
		goto out;
	}
	if (strlen(boardid) > 8) {
		fprintf(stderr, "E: Board ID must be less than 8 characters\n");
		goto out;
	}

	// generate the table

	struct SMC_BIOS_Update sbu = {
		{0xed, 0xb1, 0x00, 0xaa},
		"$FID",
		{0x04, 0x78},
		0, // space
		"100000000", // boardid
		{}, // unknown data
		0x1f, // space
		"05", // unknown data
		0, // zero
		"06", // unknown data
		0, // zero
		"000", // major
		"00", // minor
		0, // zero
		0, // year
		0, // month
		0, //day
		0, // unknown data
		0xff, // space
		"SUPERMSMCI--MB1",
		{0, 0, 0}, // all zero
		{~0, ~0, ~0, ~0, ~0, ~0}, // all ones
	};

	num = get_line_as_int(inputfilename, "COREBOOT_MAJOR_VERSION", 0);
	if (num < 0)
		goto out;

	if (num < 999) {
		char tmp[4];
		snprintf(tmp, sizeof(tmp), "%03d", num);
		memcpy(&sbu.majorVer, &tmp, sizeof(sbu.majorVer));
	} else {
		fprintf(stderr, "E: Unsupported coreboot major version\n");
		goto out;
	}

	num = get_line_as_int(inputfilename, "COREBOOT_MINOR_VERSION", 0);
	if (num < 0)
		goto out;

	if (num < 99) {
		char tmp[3];
		snprintf(tmp, sizeof(tmp), "%02d", num);
		memcpy(&sbu.minorVer, &tmp, sizeof(sbu.minorVer));
	} else {
		fprintf(stderr, "E: Unsupported coreboot minor version\n");
		goto out;
	}

	num = get_line_as_int(inputfilename, "COREBOOT_BUILD_YEAR_BCD", 1);
	if (num < 0)
		goto out;
	sbu.year = 2000 + num;

	num = get_line_as_int(inputfilename, "COREBOOT_BUILD_MONTH_BCD", 1);
	if (num < 0)
		goto out;
	sbu.month = num;

	num = get_line_as_int(inputfilename, "COREBOOT_BUILD_DAY_BCD", 1);
	if (num < 0)
		goto out;
	sbu.day = num;

	memcpy(&sbu.boardid[1], boardid, strlen(boardid));

	// write the table
	FILE *fd = fopen(filename, "wb");
	if (!fd) {
		fprintf(stderr, "E: %s open failed: %s\n", filename, strerror(errno));
		goto out;
	}

	if (fwrite(&sbu, 1, sizeof(sbu), fd) != sizeof(sbu)) {
		fprintf(stderr, "E: %s write failed: %s\n", filename, strerror(errno));
		fclose(fd);
		goto out;
	}

	if (fclose(fd)) {
		fprintf(stderr, "E: %s close failed: %s\n", filename, strerror(errno));
		goto out;
	}

	ret = 0;
out:
	if (ret > 0)
		fprintf(stderr, "E: Error creating '%s'\n", filename);

	free(filename);

	exit(ret);

	return 0;
}
