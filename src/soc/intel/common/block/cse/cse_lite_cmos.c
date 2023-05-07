/* SPDX-License-Identifier: GPL-2.0-only */

#include <console/console.h>
#include <ip_checksum.h>
#include <pc80/mc146818rtc.h>

#include "cse_lite_cmos.h"

/*
 * We need a region in CMOS to store the firmware versions.
 *
 * This can either be declared as part of the option
 * table or statically defined in the board config.
 */
#if CONFIG(USE_OPTION_TABLE)
# include "option_table.h"

#ifndef CMOS_VSTART_partition_fw
#error "The `ramtop` CMOS entry is missing, please add it to your cmos.layout."
#endif

#if CMOS_VSTART_partition_fw % 8 != 0
#error "The `partition firmware` CMOS entry needs to be byte aligned, check your cmos.layout."
#endif	// CMOS_VSTART_partition_fw % 8 != 0

#if CMOS_VLEN_partition_fw != (32 * 8)
#error "The partition firmware entry needs to be 32 bytes long, check your cmos.layout."
#endif

# define PARTITION_FW_CMOS_OFFSET (CMOS_VSTART_partition_fw >> 3)

#else
# if (CONFIG_SOC_INTEL_CSE_FW_PARTITION_CMOS_OFFSET != 0)
#  define PARTITION_FW_CMOS_OFFSET CONFIG_SOC_INTEL_CSE_FW_PARTITION_CMOS_OFFSET
# else
#  error "Must configure CONFIG_SOC_INTEL_CSE_FW_PARTITION_CMOS_OFFSET"
# endif
#endif

/* Helper function to read CSE fpt information from cmos memory. */
void cmos_read_fw_partition_info(struct cse_specific_info *info)
{
	for (uint8_t *p = (uint8_t *)info, i = 0; i < sizeof(*info); i++, p++)
		*p = cmos_read(PARTITION_FW_CMOS_OFFSET + i);
}

/* Helper function to write CSE fpt information to cmos memory. */
void cmos_write_fw_partition_info(const struct cse_specific_info *info)
{
	for (uint8_t *p = (uint8_t *)info, i = 0; i < sizeof(*info); i++, p++)
		cmos_write(*p, PARTITION_FW_CMOS_OFFSET + i);
}
