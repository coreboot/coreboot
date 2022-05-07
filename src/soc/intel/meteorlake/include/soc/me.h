/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef _METEORLAKE_ME_H_
#define _METEORLAKE_ME_H_

#include <stdint.h>

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
		u32 invoke_enhance_dbg_mode: 1;
		u32 bist_test_state: 1;
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
#endif
