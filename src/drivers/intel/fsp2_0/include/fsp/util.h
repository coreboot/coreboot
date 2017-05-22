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

struct hob_resource {
	uint8_t owner_guid[16];
	uint32_t type;
	uint32_t attribute_type;
	uint64_t addr;
	uint64_t length;
} __attribute__((packed));

enum resource_type {
	EFI_RESOURCE_SYSTEM_MEMORY		= 0,
	EFI_RESOURCE_MEMORY_MAPPED_IO		= 1,
	EFI_RESOURCE_IO				= 2,
	EFI_RESOURCE_FIRMWARE_DEVICE		= 3,
	EFI_RESOURCE_MEMORY_MAPPED_IO_PORT	= 4,
	EFI_RESOURCE_MEMORY_RESERVED		= 5,
	EFI_RESOURCE_IO_RESERVED		= 6,
	EFI_RESOURCE_MAX_MEMORY_TYPE		= 7,
};

enum hob_type {
	HOB_TYPE_HANDOFF			= 0x0001,
	HOB_TYPE_MEMORY_ALLOCATION		= 0x0002,
	HOB_TYPE_RESOURCE_DESCRIPTOR		= 0x0003,
	HOB_TYPE_GUID_EXTENSION			= 0x0004,
	HOB_TYPE_FV				= 0x0005,
	HOB_TYPE_CPU				= 0x0006,
	HOB_TYPE_MEMORY_POOL			= 0x0007,
	HOB_TYPE_FV2				= 0x0009,
	HOB_TYPE_LOAD_PEIM_UNUSED		= 0x000A,
	HOB_TYPE_UCAPSULE			= 0x000B,
	HOB_TYPE_UNUSED				= 0xFFFE,
	HOB_TYPE_END_OF_HOB_LIST		= 0xFFFF,
};

extern const uint8_t fsp_bootloader_tolum_guid[16];
extern const uint8_t fsp_nv_storage_guid[16];
extern const uint8_t fsp_reserved_memory_guid[16];

const void *fsp_get_hob_list(void);
void *fsp_get_hob_list_ptr(void);
const void *fsp_find_extension_hob_by_guid(const uint8_t *guid, size_t *size);
const void *fsp_find_nv_storage_data(size_t *size);
const void *fsp_find_smbios_memory_info(size_t *size);
enum cb_err fsp_fill_lb_framebuffer(struct lb_framebuffer *framebuffer);
int fsp_find_range_hob(struct range_entry *re, const uint8_t guid[16]);
int fsp_find_reserved_memory(struct range_entry *re);
const struct hob_resource *fsp_hob_header_to_resource(
	const struct hob_header *hob);
const struct hob_header *fsp_next_hob(const struct hob_header *parent);
bool fsp_guid_compare(const uint8_t guid1[16], const uint8_t guid2[16]);

/* Fill in header and validate sanity of component within region device. */
enum cb_err fsp_validate_component(struct fsp_header *hdr,
					const struct region_device *rdev);

/* Load a vbt.bin file for graphics. Returns 0 if a valid VBT is not found. */
#if IS_ENABLED(CONFIG_RUN_FSP_GOP)
uintptr_t fsp_load_vbt(void);
#else
static inline uintptr_t fsp_load_vbt(void) { return 0; }
#endif

/* Get igd framebuffer bar from SoC */
uintptr_t fsp_soc_get_igd_bar(void);

/*
 * Handle FSP reboot request status. Chipset/soc is expected to provide
 * chipset_handle_reset() that deals with reset type codes specific to given
 * SoC. If the requested status is not a reboot status or unhandled, this
 * function does nothing.
 */
void fsp_handle_reset(uint32_t status);

/* SoC/chipset must provide this to handle platform-specific reset codes */
void chipset_handle_reset(uint32_t status);

typedef asmlinkage uint32_t (*temp_ram_exit_fn)(void *param);
typedef asmlinkage uint32_t (*fsp_memory_init_fn)
				   (void *raminit_upd, void **hob_list);
typedef asmlinkage uint32_t (*fsp_silicon_init_fn)(void *silicon_upd);
typedef asmlinkage uint32_t (*fsp_notify_fn)(struct fsp_notify_params *);
#include <fsp/debug.h>

#endif /* _FSP2_0_UTIL_H_ */
