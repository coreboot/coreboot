/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef _APOLLOLAKE_CSE_H_
#define _APOLLOLAKE_CSE_H_

/* CSE Host Firmware Status register 1 */
union cse_fwsts1 {
	uint32_t data;
	struct {
		uint32_t	working_state:		4;
		uint32_t	mfg_mode:		1;
		uint32_t	fpt_bad:		1;
		uint32_t	operation_state:	3;
		uint32_t	fw_init_complete:	1;
		uint32_t	ft_bup_ld_flr:		1;
		uint32_t	update_in_progress:	1;
		uint32_t	error_code:		4;
		uint32_t	operation_mode:		4;
		uint32_t	reserved:		4;
		uint32_t	boot_options_present:	1;
		uint32_t	ack_data:		3;
		uint32_t	bios_message_ack:	4;
	} __packed fields;
};


/* Miscellaneous Shadow Register 1 */
union cse_fwsts2 {
	uint32_t data;
	struct {
		uint32_t	cse_uma_size:		6;
		uint32_t	reserved1:		8;
		uint32_t	reserved2:		2;
		uint32_t	cse_uma_size_valid:	1;
		uint32_t	reserved3:		8;
		uint32_t	reserved4:		6;
		uint32_t	misc_shadow_valid:	1;
	} __packed fields;
};

/* General Status Shadow Register */
union cse_fwsts3 {
	uint32_t data;
	struct {
		uint32_t	bist_in_progress:	1;
		uint32_t	icc_prog_sts:		2;
		uint32_t	invoke_mebx:		1;
		uint32_t	cpu_replaced_sts:	1;
		uint32_t	mbp_ready:		1;
		uint32_t	mfs_failure:		1;
		uint32_t	warm_rst_req_for_df:	1;
		uint32_t	cpu_replaced_valid:	1;
		uint32_t	reserved1:		2;
		uint32_t	fw_upd_ipu:		1;
		uint32_t	reserved2:		1;
		uint32_t	mbp_cleared:		1;
		uint32_t	reserved3:		2;
		uint32_t	ext_stat_code1:		8;
		uint32_t	ext_stat_code2:		4;
		uint32_t	infra_phase_code:	4;
	} __packed fields;
};

#endif /* _APOLLOLAKE_CSE_H_ */
