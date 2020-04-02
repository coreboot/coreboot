/* SPDX-License-Identifier: GPL-2.0-only */
/* This file is part of the coreboot project. */

#ifndef _FSP1_1_API_H_
#define _FSP1_1_API_H_

/* All the FSP headers need to have UEFI types provided before inclusion. */
#include <fsp/uefi_binding.h>

/*
 * Intel's code does not have a handle on changing global packing state.
 * Therefore, one needs to protect against packing policies that are set
 * globally for a compilation unit just by including a header file.
 */
#pragma pack(push)

#include <vendorcode/intel/fsp/fsp1_1/IntelFspPkg/Include/FspApi.h>
#include <vendorcode/intel/fsp/fsp1_1/IntelFspPkg/Include/FspInfoHeader.h>

/* Restore original packing policy. */
#pragma pack(pop)

#endif
