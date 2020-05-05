/* intelmetool */
/* SPDX-License-Identifier: GPL-2.0-or-later */

#include "mmap.h"
#include <errno.h>
#include <string.h>

#ifndef __DARWIN__
int fd_mem;

void *map_physical_exact(off_t phys_addr, void *mapto, size_t len)
{
	void *virt_addr;
	int err;

	virt_addr = mmap(mapto, len, PROT_WRITE | PROT_READ,
	                 MAP_SHARED | MAP_FIXED, fd_mem, phys_addr);

	if (virt_addr == MAP_FAILED) {
		err = errno;
		printf("Error mapping physical memory 0x%016jd [0x%zx] ERRNO=%d %s\n",
		       (intmax_t)phys_addr, len, err, strerror(err));
		return NULL;
	}

	return virt_addr;
}

void *map_physical(off_t phys_addr, size_t len)
{
	void *virt_addr;
	int err;

	virt_addr = mmap(NULL, len, PROT_WRITE | PROT_READ, MAP_SHARED, fd_mem, phys_addr);

	if (virt_addr == MAP_FAILED) {
		err = errno;
		printf("Error mapping physical memory 0x%016jd [0x%zx] ERRNO=%d %s\n",
		       (intmax_t)phys_addr, len, err, strerror(err));
		return NULL;
	}

	return virt_addr;
}

void unmap_physical(void *virt_addr, size_t len)
{
	munmap(virt_addr, len);
}
#endif
