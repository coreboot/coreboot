/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef _XEON_SP_ME_H_
#define _XEON_SP_ME_H_

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
		u32 reserved1: 1;
		u32 bist_test_state: 1;
		u32 bist_reset_request: 1;
		u32 current_power_source: 2;
		u32 d3_support_valid: 1;
		u32 d0i3_support_valid: 1;
	} __packed fields;
};

union me_hfsts2 {
	u32 data;
	struct {
	u32 reserved1: 3;
	u32 invoke_mebx: 1;
	u32 cpu_replaced_sts: 1;
	u32 reserved2: 1;
	u32 mfs_failure: 1;
	u32 warm_reset_request: 1;
	u32 cpu_replaced_valid: 1;
	u32 low_power_state: 1;
	u32 power_gating_ind: 1;
	u32 reserved3: 1;
	u32 fw_upd_forced_sb: 1;
	u32 reserved4: 3;
	u32 current_state: 8;
	u32 current_pmevent: 4;
	u32 progress_code: 4;
	} __packed fields;
};

union me_hfsts3 {
	u32 data;
	struct {
	u32 reserved1: 4;
	u32 fw_sku: 3;
	u32 encrypt_key_check: 1;
	u32 pch_config_change: 1;
	u32 reserved2: 21;
	u32 encrypt_key_override: 1;
	u32 power_down_mitigation: 1;
	} __packed fields;
};

#endif
