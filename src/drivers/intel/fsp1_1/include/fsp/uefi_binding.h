/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef _FSP1_1_UEFI_BINDING_H_
#define _FSP1_1_UEFI_BINDING_H_

/*
 * Intel's code does not have a handle on changing global packing state.
 * Therefore, one needs to protect against packing policies that are set
 * globally for a compilation unit just by including a header file.
 */
#pragma pack(push)

/*
 * Pull in the UEFI types from 2.4. Smarter decisions can be made on what
 * version to bind to, but for now 2.4 is standard for FSP 1.1.
 */
#include <vendorcode/intel/edk2/uefi_2.4/uefi_types.h>

/* Restore original packing policy. */
#pragma pack(pop)

#endif
