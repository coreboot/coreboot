/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2015 Intel Corp.
 * (Written by Alexandru Gagniuc <alexandrux.gagniuc@intel.com> for Intel Corp.)
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */

#ifndef _FSP2_0_API_H_
#define _FSP2_0_API_H_

#include <stddef.h>
#include <memrange.h>
#include <fsp/info_header.h>
#include <soc/fsp/FspmUpd.h>
#include <soc/fsp/FspsUpd.h>

enum fsp_status {
	FSP_SUCCESS = 0x00000000,
	FSP_INVALID_PARAMETER = 0x80000002,
	FSP_UNSUPPORTED = 0x80000003,
	FSP_NOT_READY = 0x80000006,
	FSP_DEVICE_ERROR = 0x80000007,
	FSP_OUT_OF_RESOURCES = 0x80000009,
	FSP_VOLUME_CORRUPTED = 0x8000000a,
	FSP_NOT_FOUND = 0x8000000a,
	FSP_TIMEOUT = 0x80000012,
	FSP_ABORTED = 0x80000015,
	FSP_INCOMPATIBLE_VERSION = 0x80000010,
	FSP_SECURITY_VIOLATION = 0x8000001a,
	FSP_CRC_ERROR = 0x8000001b,
};

enum fsp_boot_mode {
	FSP_BOOT_WITH_FULL_CONFIGURATION = 0x00,
	FSP_BOOT_WITH_MINIMAL_CONFIGURATION = 0x01,
	FSP_BOOT_ASSUMING_NO_CONFIGURATION_CHANGES = 0x02,
	FSP_BOOT_ON_S4_RESUME = 0x05,
	FSP_BOOT_ON_S3_RESUME = 0x11,
	FSP_BOOT_ON_FLASH_UPDATE = 0x12,
	FSP_BOOT_IN_RECOVERY_MODE = 0x20
};

enum fsp_notify_phase {
	AFTER_PCI_ENUM = 0x20,
	READY_TO_BOOT = 0x40
};


/* Main FSP stages */
enum fsp_status fsp_memory_init(void **hob_list, struct range_entry *r);
enum fsp_status fsp_silicon_init(struct range_entry *r);
enum fsp_status fsp_notify(enum fsp_notify_phase phase);

/* Callbacks for updating stage-specific parameters */
void platform_fsp_memory_init_params_cb(struct FSPM_UPD *mupd);
void platform_fsp_silicon_init_params_cb(struct FSPS_UPD *supd);

/*
 * # DOCUMENTATION:
 *
 * This file defines the interface between coreboot and the FSP 2.0 wrapper
 * fsp_memory_init(), fsp_silicon_init(), and fsp_notify() are the main entry
 * points and map 1:1 to the FSP entry points of the same name.
 *
 * ### fsp_memory_init():
 *     - hob_list: retuns a pointer to the HOB storage area created by FSP
 *     - r: memory range that the binary is allowed to be loaded into
 *
 * This function is responsible for loading and executing the memory
 * initialization code from the FSP-M binary. It expects this binary to reside
 * in cbfs as FSP_M_FILE.
 *
 * The function takes one parameter, which is described below, but does not
 * take in memory parameters as an argument. The memory parameters can be filled
 * in with platform_fsp_memory_init_params_cb(). This is a callback symbol
 * that fsp_memory_init() will call. The platform must provide this symbol.
 *
 * FSP returns information about the memory layout in a series of structures
 * called hand-off-blocks (HOB). The "hob_list" output parameter will point to
 * the start of the HOB list. The fsp reserved region will also be described by
 * one of the HOBs. For more information on parsing these structures, see
 * fsp/util.h
 *
 *
 * ### fsp_silicon_init():
 *     - r: memory range that the binary is allowed to be loaded into
 *
 * This function is responsible for loading and executing the silicon
 * initialization code from the FSP-S binary. It expects this binary to reside
 * in cbfs as FSP_S_FILE.
 *
 * Like fsp_memory_init(), it provides a callback to fill in FSP-specific
 * parameters, via platform_fsp_silicon_init_params_cb(). The platform must
 * also provide this symbol.
 *
 *
 * ### fsp_notify():
 *     - phase: Which FSP notification phase
 *
 * This function is responsible for loading and executing the notify code from
 * the FSP-S binary. It expects that fsp_silicon_init() has already been called
 * succesfully, and that the FSP-S binary is still loaded into memory.
 */

#endif /* _FSP2_0_API_H_ */
