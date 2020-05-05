/* intelmetool */
/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <inttypes.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <stdio.h>

#ifndef __DARWIN__
extern int fd_mem;
extern void *map_physical(off_t phys_addr, size_t len);
extern void unmap_physical(void *virt_addr, size_t len);
extern void *map_physical_exact(off_t phys_addr, void *mapto, size_t len);
#endif
