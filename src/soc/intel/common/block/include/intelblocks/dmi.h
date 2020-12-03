/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef SOC_INTEL_COMMON_BLOCK_DMI_H
#define SOC_INTEL_COMMON_BLOCK_DMI_H

#include <types.h>

#define PCR_DMI_DMICTL		0x2234
#define  PCR_DMI_DMICTL_SRLOCK	(1 << 31)

#define PCR_DMI_GCS		0x274C
#define  PCR_DMI_GCS_BILD	(1 << 0)

/*
 * Takes base, size and destination ID and configures the GPMR
 * for accessing the region.
 */
enum cb_err dmi_enable_gpmr(uint32_t base, uint32_t size, uint32_t dest_id);

#endif /* SOC_INTEL_COMMON_BLOCK_DMI_H */
