/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef _ELKHARTLAKE_ME_H_
#define _ELKHARTLAKE_ME_H_

/* ME Host Firmware Status register 1 */
union me_hfsts1 {
	uint32_t data;
	struct {
		uint32_t working_state: 4;
		uint32_t mfg_mode: 1;
		uint32_t fpt_bad: 1;
		uint32_t operation_state: 3;
		uint32_t fw_init_complete: 1;
		uint32_t ft_bup_ld_flr: 1;
		uint32_t update_in_progress: 1;
		uint32_t error_code: 4;
		uint32_t operation_mode: 4;
		uint32_t reset_count: 4;
		uint32_t boot_options_present: 1;
		uint32_t reserved1: 1;
		uint32_t bist_test_state: 1;
		uint32_t bist_reset_request: 1;
		uint32_t current_power_source: 2;
		uint32_t reserved: 1;
		uint32_t d0i3_support_valid: 1;
	} __packed fields;
};

/* Host Firmware Status Register 2 */
union me_hfsts2 {
	uint32_t data;
	struct {
		uint32_t nftp_load_failure	: 1;
		uint32_t icc_prog_status	: 2;
		uint32_t invoke_mebx		: 1;
		uint32_t cpu_replaced		: 1;
		uint32_t rsvd0			: 1;
		uint32_t mfs_failure		: 1;
		uint32_t warm_reset_rqst	: 1;
		uint32_t cpu_replaced_valid	: 1;
		uint32_t low_power_state	: 1;
		uint32_t me_power_gate		: 1;
		uint32_t ipu_needed		: 1;
		uint32_t forced_safe_boot	: 1;
		uint32_t rsvd1			: 2;
		uint32_t listener_change	: 1;
		uint32_t status_data		: 8;
		uint32_t current_pmevent	: 4;
		uint32_t phase			: 4;
	} __packed fields;
};

/* ME Host Firmware Status Register 3 */
union me_hfsts3 {
	uint32_t data;
	struct {
		uint32_t reserved_0: 4;
		uint32_t fw_sku: 3;
		uint32_t reserved: 25;
	} __packed fields;
};

/* Host Firmware Status Register 4 */
union me_hfsts4 {
	uint32_t data;
	struct {
		uint32_t rsvd0			: 9;
		uint32_t enforcement_flow	: 1;
		uint32_t sx_resume_type		: 1;
		uint32_t rsvd1			: 1;
		uint32_t tpms_disconnected	: 1;
		uint32_t rvsd2			: 1;
		uint32_t fwsts_valid		: 1;
		uint32_t boot_guard_self_test	: 1;
		uint32_t rsvd3			: 16;
	} __packed fields;
};

/* Host Firmware Status Register 5 */
union me_hfsts5 {
	uint32_t data;
	struct {
		uint32_t acm_active		: 1;
		uint32_t valid			: 1;
		uint32_t result_code_source	: 1;
		uint32_t error_status_code	: 5;
		uint32_t acm_done_sts		: 1;
		uint32_t timeout_count		: 7;
		uint32_t scrtm_indicator	: 1;
		uint32_t inc_boot_guard_acm	: 4;
		uint32_t inc_key_manifest	: 4;
		uint32_t inc_boot_policy	: 4;
		uint32_t rsvd0			: 2;
		uint32_t start_enforcement	: 1;
	} __packed fields;
};

/* Host Firmware Status Register 6 */
union me_hfsts6 {
	uint32_t data;
	struct {
		uint32_t force_boot_guard_acm	: 1;
		uint32_t cpu_debug_disable	: 1;
		uint32_t bsp_init_disable	: 1;
		uint32_t protect_bios_env	: 1;
		uint32_t rsvd0			: 2;
		uint32_t error_enforce_policy	: 2;
		uint32_t measured_boot		: 1;
		uint32_t verified_boot		: 1;
		uint32_t boot_guard_acmsvn	: 4;
		uint32_t kmsvn			: 4;
		uint32_t bpmsvn			: 4;
		uint32_t key_manifest_id	: 4;
		uint32_t boot_policy_status	: 1;
		uint32_t error			: 1;
		uint32_t boot_guard_disable	: 1;
		uint32_t fpf_disable		: 1;
		uint32_t fpf_soc_lock		: 1;
		uint32_t txt_support		: 1;
	} __packed fields;
};
#endif /* _ELKHARTLAKE_ME_H_ */
