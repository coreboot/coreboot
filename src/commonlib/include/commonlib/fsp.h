/* SPDX-License-Identifier: GPL-2.0-only */
/* This file is part of the coreboot project. */

#ifndef _COMMONLIB_FSP_H_
#define _COMMONLIB_FSP_H_

#include <stddef.h>
#include <stdint.h>
#include <sys/types.h>

/*
 * Relocate FSP held within buffer defined by size to new_addr. Returns < 0
 * on error, offset to FSP_INFO_HEADER on success.
 */
ssize_t fsp_component_relocate(uintptr_t new_addr, void *fsp, size_t size);

/* API to relocate fsp 1.1 component. */
ssize_t fsp1_1_relocate(uintptr_t new_addr, void *fsp, size_t size);

#endif
