/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2015-2016 Intel Corp.
 * (Written by Alexandru Gagniuc <alexandrux.gagniuc@intel.com> for Intel Corp.)
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */

#ifndef _FSP2_0_UTIL_H_
#define _FSP2_0_UTIL_H_

#include <boot/coreboot_tables.h>
#include <commonlib/region.h>
#include <arch/cpu.h>
#include <fsp/api.h>
#include <fsp/info_header.h>
#include <memrange.h>

struct hob_header {
	uint16_t type;
	uint16_t length;
} __attribute__((packed));

struct fsp_notify_params {
	enum fsp_notify_phase phase;
};

/*
 * Hand-off-block handling functions that depend on CBMEM, and thus can only
 * be used after cbmem_initialize().
 */
void fsp_save_hob_list(void *hob_list_ptr);
const void *fsp_get_hob_list(void);
const void *fsp_find_extension_hob_by_uuid(const uint8_t *uuid, size_t *size);
const void *fsp_find_nv_storage_data(size_t *size);
enum cb_err fsp_fill_lb_framebuffer(struct lb_framebuffer *framebuffer);
/*
 * Hand-off-block utilities which do not depend on CBMEM, but need to be passed
 * the HOB list explicitly.
 */
void fsp_find_reserved_memory(struct range_entry *re, const void *hob_list);
void fsp_print_memory_resource_hobs(const void *hob_list);

/* Fill in header and validate sanity of component within region device. */
enum cb_err fsp_validate_component(struct fsp_header *hdr,
					const struct region_device *rdev);

/* Load a vbt.bin file for graphics. Returns 0 if a valid VBT is not found. */
uintptr_t fsp_load_vbt(void);

/*
 * Handle FSP reboot request status. Chipset/soc is expected to provide
 * chipset_handle_reset() that deals with reset type codes specific to given
 * SoC. If the requested status is not a reboot status or unhandled, this
 * function does nothing.
 */
void fsp_handle_reset(enum fsp_status status);

/* SoC/chipset must provide this to handle platform-specific reset codes */
void chipset_handle_reset(enum fsp_status status);

typedef asmlinkage enum fsp_status (*fsp_memory_init_fn)
				   (void *raminit_upd, void **hob_list);
typedef asmlinkage enum fsp_status (*fsp_silicon_init_fn)
				   (void *silicon_upd);
typedef asmlinkage enum fsp_status (*fsp_notify_fn)
				   (struct fsp_notify_params *);
#include <fsp/debug.h>

#endif /* _FSP2_0_UTIL_H_ */
