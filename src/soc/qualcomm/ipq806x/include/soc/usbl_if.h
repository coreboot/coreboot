/* SPDX-License-Identifier: BSD-3-Clause */

#ifndef __SOC_QUALCOMM_IPQ806X_INCLUDE_SOC_USBL_IF_H__
#define __SOC_QUALCOMM_IPQ806X_INCLUDE_SOC_USBL_IF_H__

#include <types.h>

typedef struct {
	u32	time_stamp;
	char	msg[27];
	u8	type;

} boot_log_entry;

typedef struct {
	u32		num_log_entries;
	boot_log_entry	*log;
} sbl_ro_info;

typedef struct {
	u32		start_magic;
	u32		num;
	char		*version;
	sbl_ro_info	info[2];
	/*
	 * The two addresses below can be used for communicating with the RPM
	 * (passing it the starting address of the program to execute and
	 * triggering the jump to the program). Presently these addresses are
	 * hardcodeded in firmware source code.
	 */
	u32		*rpm_jump_trigger;
	u32		*rpm_entry_addr_ptr;
	u32		end_magic;
} uber_sbl_shared_info;

#define UBER_SBL_SHARED_INFO_START_MAGIC	0x5552504d // URPM
#define UBER_SBL_SHARED_INFO_END_MAGIC		0x554b5254 // UKRT

extern uber_sbl_shared_info *maskrom_param;

#endif
