/* SPDX-License-Identifier: GPL-2.0-only */

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

#include "msrtool.h"

/* This Darwin support requires DirectHW, which is available at
 * https://www.coreboot.org/DirectHW
 */

int darwin_probe(const struct sysdef *system)
{
#ifdef __DARWIN__
	return iopl(3) == 0;
#else
	return 0;
#endif
}

int darwin_open(uint8_t cpu, enum SysModes mode)
{
#ifdef __DARWIN__
	if (cpu > 0) {
		fprintf(stderr, "%s: only core 0 is supported on Mac OS X right now.\n", __func__);
		return 0;
	}
	return 1;
#else
	return 0;
#endif
}

int darwin_close(uint8_t cpu)
{
	return 1;
}

int darwin_rdmsr(uint8_t cpu, uint32_t addr, struct msr *val)
{
#ifdef __DARWIN__
	msr_t msr;

	msr = rdmsr(addr);

	val->hi = msr.lo;
	val->lo = msr.hi;
	return 1;
#else
	return 0;
#endif
}
