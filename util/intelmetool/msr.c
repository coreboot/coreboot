/* intelmetool
 *
 * Copyright (C) 2013-2016 Philipp Deppenwiese <zaolin@das-labor.org>,
 * Copyright (C) 2013-2016 Alexander Couzens <lynxis@fe80.eu>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 */

#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "msr.h"

#ifndef __DARWIN__
static int fd_msr = 0;

static uint64_t rdmsr(int addr)
{
	uint32_t buf[2];
	uint64_t msr = 0;

	if (lseek(fd_msr, (off_t) addr, SEEK_SET) == -1) {
		perror("Could not lseek() to MSR");
		close(fd_msr);
		return -1;
	}

	if (read(fd_msr, buf, 8) == 8) {
		msr = buf[1];
		msr <<= 32;
		msr |= buf[0];
		close(fd_msr);
		return msr;
	}

	if (errno == EIO) {
		perror("IO error couldn't read MSR.");
		close(fd_msr);
		return -2;
	}

	perror("Couldn't read() MSR");
	close(fd_msr);
	return -1;
}
#endif

int msr_bootguard(uint64_t *msr, int debug)
{

#ifndef __DARWIN__
	fd_msr = open("/dev/cpu/0/msr", O_RDONLY);
	if (fd_msr < 0) {
		perror("Error while opening /dev/cpu/0/msr");
		printf("Did you run 'modprobe msr'?\n");
		return -1;
	}

	*msr = rdmsr(MSR_BOOTGUARD);
#endif

	if (!debug)
		*msr &= ~0xff;

	return 0;
}
