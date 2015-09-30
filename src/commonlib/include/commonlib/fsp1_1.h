/*
 * This file is part of the coreboot project.
 *
 * Copyright 2015 Google Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc.
 */

#ifndef _COMMONLIB_FSP1_1_H_
#define _COMMONLIB_FSP1_1_H_

#include <stddef.h>
#include <stdint.h>

/*
 * Intel's code does not have a handle on changing global packing state.
 * Therefore, one needs to protect against packing policies that are set
 * globally for a compliation unit just by including a header file.
 */
#pragma pack(push)

/* Default bind FSP 1.1 API to edk2 UEFI 2.4 types. */
#include <vendorcode/intel/edk2/uefi_2.4/uefi_types.h>

#include <vendorcode/intel/fsp/fsp1_1/IntelFspPkg/Include/FspApi.h>
#include <vendorcode/intel/fsp/fsp1_1/IntelFspPkg/Include/FspInfoHeader.h>

/* Restore original packing policy. */
#pragma pack(pop)

/*
 * Relocate FSP held within buffer defined by size to new_addr. Returns < 0
 * on error, offset to FSP_INFO_HEADER on success.
 */
ssize_t fsp1_1_relocate(uintptr_t new_addr, void *fsp, size_t size);

#endif
