/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef __AMD_MISC_DATA_HOB_H__
#define __AMD_MISC_DATA_HOB_H__

#define AMD_MISC_DATA_VERSION	2

struct amd_misc_data {
	uint8_t		version;
	uint8_t		unused[3];
	uint32_t	smu_power_and_thm_limit;
} __packed;

#endif /* __AMD_MISC_DATA_HOB_H__ */
