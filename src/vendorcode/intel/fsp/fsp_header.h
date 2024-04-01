/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef _FSP_HEADER_H_
#define _FSP_HEADER_H_

/*
 * Intel's code does not have a handle on changing global packing state.
 * Therefore, one needs to protect against packing policies that are set
 * globally for a compilation unit just by including a header file.
 */
#pragma pack(push)
/* Default bind edk2 UEFI 2.4 types. */
#include <vendorcode/intel/edk2/uefi_2.4/uefi_types.h>

#if CONFIG_UDK_VERSION >= CONFIG_UDK_2017_VERSION
#include <vendorcode/intel/fsp/fsp2_0/IntelFspPkg/Include/FspInfoHeader.h>
#else
#include <vendorcode/intel/fsp/fsp1_1/IntelFspPkg/Include/FspInfoHeader.h>
#endif
#pragma pack(pop)

#endif /* _FSP_HEADER_H_ */
