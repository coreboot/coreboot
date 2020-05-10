/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef _FSP1_1_SOC_BINDING_H_
#define _FSP1_1_SOC_BINDING_H_

/* All the FSP headers need to have UEFI types provided before inclusion. */
#include <fsp/uefi_binding.h>

/*
 * Intel's code does not have a handle on changing global packing state.
 * Therefore, one needs to protect against packing policies that are set
 * globally for a compilation unit just by including a header file.
 */
#pragma pack(push)

/*
 * This file is found in the soc / chipset directory. It is
 * a per implementation specific header. i.e. different FSP implementations
 * for different chipsets.
 */
#include <FspUpdVpd.h>

/* Restore original packing policy. */
#pragma pack(pop)

#endif
