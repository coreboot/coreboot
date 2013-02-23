/*
 * This file is part of msrtool.
 *
 * Copyright (c) 2009 Andriy Gapon <avg@icyb.net.ua>
 * Copyright (c) 2009 Peter Stuge <peter@stuge.se>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

#include "msrtool.h"

static int msr_fd[MAX_CORES] = {-1, -1, -1, -1, -1, -1, -1, -1};

int freebsd_probe(const struct sysdef *system)
{
#ifdef __FreeBSD__
	struct stat st;

	return stat("/dev/cpuctl0", &st) == 0;
#else
	return 0;
#endif
}

int freebsd_open(uint8_t cpu, enum SysModes mode)
{
#ifdef __FreeBSD__
	int flags;
	char devname[32];

	if (cpu >= MAX_CORES)
		return 0;

	if (SYS_RDWR == mode)
		flags = O_RDWR;
	else if (SYS_WRONLY == mode)
		flags = O_WRONLY;
	else
		flags = O_RDONLY;

	snprintf(devname, sizeof(devname), "/dev/cpuctl%u", cpu);
	msr_fd[cpu] = open(devname, flags);
	if (msr_fd[cpu] < 0) {
		perror(devname);
		return 0;
	}
	return 1;
#else
	return 0;
#endif
}

int freebsd_close(uint8_t cpu)
{
	if (cpu >= MAX_CORES)
		return 0;

	if (msr_fd[cpu] != -1)
		close(msr_fd[cpu]);
	msr_fd[cpu] = -1;
	return 1;
}

int freebsd_rdmsr(uint8_t cpu, uint32_t addr, struct msr *val)
{
#ifdef __FreeBSD__
	cpuctl_msr_args_t args;

	if (cpu >= MAX_CORES)
		return 0;

	if (msr_fd[cpu] < 0)
		return 0;

	args.msr = addr;
	if (ioctl(msr_fd[cpu], CPUCTL_RDMSR, &args) < 0) {
		perror("ioctl(CPUCTL_RDMSR)");
		return 0;
	}

	val->hi = args.data >> 32;
	val->lo = args.data & 0xffffffff;
	return 1;
#else
	return 0;
#endif
}
