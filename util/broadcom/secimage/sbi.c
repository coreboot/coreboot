/*
 * Copyright (C) 2015 Broadcom Corporation
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation version 2.
 *
 * This program is distributed "as is" WITHOUT ANY WARRANTY of any
 * kind, whether express or implied; without even the implied warranty
 * of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <zlib.h>
#include "secimage.h"

#define MIN_SIZE	(1024*120)

/*----------------------------------------------------------------------
 * Name    : SBIUsage
 * Purpose :
 * Input   : none
 * Output  : none
 *---------------------------------------------------------------------*/
int SBIUsage(void)
{
	printf("\nTo create a Secure Boot Image:\n");
	printf("secimage: -out <output binary> [-hmac hmac_binary_key] <-config configfile>");
	printf("\n\t\t[-bl input binary]\n");
	return 0;
}

/*----------------------------------------------------------------------
 * Name    : AddImagePayload
 * Purpose :
 * Input   : none
 * Output  : none
 *---------------------------------------------------------------------*/
int AddImagePayload(char *h, char *filename, unsigned int filesize)
{
	uint32_t totalLen;
	int length = filesize;
	int padlen = 0;
	int status = 0;

	totalLen = 0x40;

	status = DataRead(filename, (uint8_t *)h + totalLen, &length);
	printf("\r\n Adding file %s ... \r\n", filename);
	if (!status) {
		if (length & 15) {
			padlen = 16 - (length & 15);
			memset((uint8_t *)h + totalLen + length, 0, padlen);
			length += padlen;
		}

		*(uint32_t *)&h[FIELD5_OFFSET] = length;
		*(uint32_t *)&h[FIELD6_OFFSET] += length;

	} else
		printf("Error reading image Payload from %s\n", filename);

	return status;
}

/*----------------------------------------------------------------------
 * Name    : CreateSecureBootImage
 * Purpose :
 * Input   : none
 * Output  : none
 *---------------------------------------------------------------------*/
int CreateSecureBootImage(int ac, char **av)
{
	char *outfile, *configfile, *arg, *privkey = NULL, *bl = NULL;
	int status = 0;
	uint32_t sbiLen;
	struct stat file_stat;
	uint32_t add_header = 1;
	outfile = *av;
	unsigned int filesize;
	char *buf;
	--ac; ++av;

	if (ac <= 0)
		return SBIUsage();

	while (ac) {
		arg = *av;
		if (!strcmp(arg, "-bl")) {
			--ac, ++av;
			bl = *av;
		} else if (!strcmp(arg, "-hmac")) {
			--ac, ++av;
			privkey = *av;
		} else if (!strcmp(arg, "-config")) {
			--ac, ++av;
			configfile = *av;
		} else if (!strcmp(arg, "-noheader")) {
			add_header = 0;
		} else {
			return SBIUsage();
		}
		--ac, ++av;
	}

	stat(bl, &file_stat);
	filesize = file_stat.st_size + MIN_SIZE;
	buf = calloc(sizeof(uint8_t), filesize);

	if (buf == NULL) {
		puts("Memory allocation error");
		status = -1;
		goto done;
	}

	*(uint32_t *)&buf[FIELD6_OFFSET] = 0x40;
	*(uint32_t *)&buf[FIELD9_OFFSET] = 0x45F2D99A;
	*(uint32_t *)&buf[FIELD3_OFFSET] = 0x900FFFFF;
	*(uint16_t *)&buf[FIELD1_OFFSET] = 0x40;
	*(uint32_t *)&buf[FIELD4_OFFSET] = 0x40;
	*(uint16_t *)&buf[FIELD2_OFFSET] = 0x10;
	*(uint16_t *)&buf[FIELD8_OFFSET] = 0x20;
	*(uint16_t *)&buf[FIELD7_OFFSET] = 0x10;

	if (status == 0) {

		if (configfile)
			FillHeaderFromConfigFile(buf, configfile);

		status = AddImagePayload(buf, bl, filesize);
		if (status) {
			status = -1;
			goto done;
		}

		sbiLen = *(uint32_t *)&buf[FIELD6_OFFSET];

		printf("HMAC signing %d bytes\n", sbiLen);
		status = AppendHMACSignature((uint8_t *)buf, sbiLen, privkey,
					     add_header ? 0x10 : 0x40);
		if (status > 0) {
			sbiLen += status;
			status = 0;
		}

		if (!status) {
			((HEADER *)buf)->Length = sbiLen;
			((HEADER *)buf)->crc = crc32(0xFFFFFFFF,
							  (uint8_t *)buf, 12);

			printf("Generating Image file %s: %d bytes\n",
				outfile, sbiLen);
			if (!add_header)
				status = DataWrite(outfile, &buf[0x40],
						   sbiLen - 0x40);
			else
				status = DataWrite(outfile, buf, sbiLen);
		}
	}
	if (status < 0)
		printf("Generation error %d\n", status);

done:
	free(buf);
	return status;
}

int main(int argc, char **argv)
{
	argc--;
	argv++;
	if (argc > 0) {
		if (!strcmp(*argv, "-out"))
			return CreateSecureBootImage(--argc, ++argv);
	}
	SBIUsage();
	return 0;
}
