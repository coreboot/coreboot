/*
 * This file is part of msrtool.
 *
 * Copyright (c) 2009 coresystems GmbH
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

/* This Darwin support requires DirectHW, which is available at
 * http://www.coreboot.org/DirectHW
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
