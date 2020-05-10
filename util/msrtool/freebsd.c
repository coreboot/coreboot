/* SPDX-License-Identifier: GPL-2.0-only */

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
