/*
 * Copyright (c) 1998-2006 The TCPDUMP project
 *               2014      Sage Electronic Engineering, LLC
 * All Rights Reserved
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that: (1) source code
 * distributions retain the above copyright notice and this paragraph
 * in its entirety, and (2) distributions including binary code include
 * the above copyright notice and this paragraph in its entirety in
 * the documentation or other materials provided with the distribution.
 * THIS SOFTWARE IS PROVIDED ``AS IS'' AND
 * WITHOUT ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, WITHOUT
 * LIMITATION, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE.
 *
 * Original code by Hannes Gredler <hannes@juniper.net>
 * Rewritten for Fletcher32 by Bruce Griffith <Bruce.Griffith@se-eng.com>
 */

#include <errno.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>

#define MAX_PSP_DIRECTORY_SIZE 512

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

int main(int argc, char **argv)
{
	uint32_t checksum = 0xFFFFFFFF;
	struct stat filestat = {};
	int retcode = EINVAL;
	size_t filesize = 0;
	char debugoption[] = "--print";

	uint16_t buffer[MAX_PSP_DIRECTORY_SIZE / sizeof(uint16_t)];

	retcode = fstat(fileno(stdin), &filestat);
	filesize = filestat.st_size;
	if (retcode < 0) {
		perror("FLETCHER32");
		return errno;
	} else if (!((12 < filesize) && (filesize <= sizeof(buffer)))) {
		fprintf(stderr, "FLETCHER32: input file is not valid for this program.\n");
		return EINVAL;
	}
	retcode = read(fileno(stdin), (void *)buffer, filesize);
	if (retcode < 0) {
		perror("FLETCHER32");
		return errno;
	}

	checksum = fletcher32(&buffer[2], filesize/2 - 2);
	*((uint32_t *)& buffer[2]) = checksum;
#ifndef DEBUG
	if ((argc == 2) && (strcmp(argv[1], debugoption) == 0)) {
#endif
		fprintf(stderr, "Fletcher's Checksum: %x\n", checksum);
#ifndef DEBUG
	}
#endif

	retcode = write(fileno(stdout), buffer, filesize);
	if (retcode < 0) {
		perror("FLETCHER32");
		return errno;
	}

	return 0;
}
