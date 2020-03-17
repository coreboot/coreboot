/*
 * This file is part of the coreboot project.
 *
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 */

#ifndef _APOLLOLAKE_ME_H_
#define _APOLLOLAKE_ME_H_

/* ME Host Firmware Status register 1 */
union me_hfsts1 {
	u32 data;
	struct {
		u32 working_state: 4;
		u32 mfg_mode: 1;
		u32 fpt_bad: 1;
		u32 operation_state: 3;
		u32 fw_init_complete: 1;
		u32 ft_bup_ld_flr: 1;
		u32 update_in_progress: 1;
		u32 error_code: 4;
		u32 operation_mode: 4;
		u32 reset_count: 4;
		u32 boot_options_present: 1;
		u32 bist_finished: 1;
		u32 hw_bist_passed: 1;
		u32 bist_reset_request: 1;
		u32 current_power_source: 2;
		u32 reserved: 1;
		u32 d0i3_support_valid: 1;
	} __packed fields;
};

/* ME Host Firmware Status Register 3 */
union me_hfsts3 {
	u32 data;
	struct {
		u32 reserved_0: 4;
		u32 fw_sku: 3;
		u32 reserved_7: 2;
		u32 reserved_9: 2;
		u32 resered_11: 3;
		u32 resered_14: 16;
		u32 reserved_30: 2;
	} __packed fields;
};

#endif /* _APOLLOLAKE_ME_H_ */
