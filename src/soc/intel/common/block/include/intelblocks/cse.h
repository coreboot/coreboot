/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2017 Intel Corp.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#ifndef SOC_INTEL_COMMON_CSE_H
#define SOC_INTEL_COMMON_CSE_H

#include <stdint.h>

/* MKHI Command groups */
#define MKHI_GROUP_ID_CBM	0x0
#define MKHI_GROUP_ID_HMRFPO	0x5

/* Global Reset Command ID */
#define MKHI_CBM_GLOBAL_RESET_REQ	0xb

/* Origin of Global Reset command */
#define GR_ORIGIN_BIOS_POST	0x2

/* HMRFPO Command Ids */
#define MKHI_HMRFPO_ENABLE	0x1
#define MKHI_HMRFPO_GET_STATUS	0x3

/* ME Current Working States */
#define ME_HFS1_CWS_NORMAL	0x5

/* ME Current Operation Modes */
#define ME_HFS1_COM_NORMAL	0x0
#define ME_HFS1_COM_SOFT_TEMP_DISABLE	0x3
#define ME_HFS1_COM_SECOVER_MEI_MSG	0x5

/* HFSTS register offsets in PCI config space */
enum {
	PCI_ME_HFSTS1 = 0x40,
	PCI_ME_HFSTS2 = 0x48,
	PCI_ME_HFSTS3 = 0x60,
	PCI_ME_HFSTS4 = 0x64,
	PCI_ME_HFSTS5 = 0x68,
	PCI_ME_HFSTS6 = 0x6C,
};

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

/* HECI Message Header */
struct mkhi_hdr {
	uint8_t group_id;
	uint8_t command:7;
	uint8_t is_resp:1;
	uint8_t rsvd;
	uint8_t result;
} __packed;

/* set up device for use in early boot enviroument with temp bar */
void heci_init(uintptr_t bar);
/*
 * Receive message into buff not exceeding maxlen. Message is considered
 * successfully received if a 'complete' indication is read from ME side
 * and there was enough space in the buffer to fit that message. maxlen
 * is updated with size of message that was received. Returns 0 on failure
 * and 1 on success.
 * In case of error heci_reset() may be requiered.
 */
int heci_receive(void *buff, size_t *maxlen);
/*
 * Send message msg of size len to host from host_addr to cse_addr.
 * Returns 1 on success and 0 otherwise.
 * In case of error heci_reset() may be requiered.
 */
int
heci_send(const void *msg, size_t len, uint8_t host_addr, uint8_t cse_addr);

/*
 * Sends snd_msg of size snd_sz, and reads message into buffer pointed by
 * rcv_msg of size rcv_sz
 * Returns 0 on failure and 1 on success.
 */
int heci_send_receive(const void *snd_msg, size_t snd_sz, void *rcv_msg, size_t *rcv_sz);

/*
 * Attempt device reset. This is useful and perhaps only thing left to do when
 * CPU and CSE are out of sync or CSE fails to respond.
 * Returns 0 on failure and 1 on success.
 */
int heci_reset(void);

/* Reads config value from a specified offset in the CSE PCI Config space. */
uint32_t me_read_config32(int offset);

/*
 * Check if the CSE device is enabled in device tree. Also check if the device
 * is visible on the PCI bus by reading config space.
 * Return true if device present and config space enabled, else return false.
 */
bool is_cse_enabled(void);

/* Makes the host ready to communicate with CSE*/
void set_host_ready(void);

/*
 * Polls for ME state 'HECI_OP_MODE_SEC_OVERRIDE' for 15 seconds.
 * Returns 0 on failure and 1 on success.
 */
uint8_t wait_cse_sec_override_mode(void);

/*
 * Sends GLOBAL_RESET_REQ cmd to CSE.The reset type can be
 * GLOBAL_RESET/HOST_RESET_ONLY/CSE_RESET_ONLY.
 * Returns -1 on failure and 0 on success.
 */
int send_heci_reset_req_message(uint8_t rst_type);

/*
 * Send HMRFPO_ENABLE command.
 * returns 0 on failure and 1 on success.
 */
int send_hmrfpo_enable_msg(void);

/*
 * Send HMRFPO_GET_STATUS command.
 * returns -1 on failure and 0 (DISABLED)/ 1 (LOCKED)/ 2 (ENABLED)
 * on success.
 */
int send_hmrfpo_get_status_msg(void);

/* Fixed Address MEI Header's Host Address field value */
#define BIOS_HOST_ADDR	0x00

/* Fixed Address MEI Header's ME Address field value */
#define HECI_MKHI_ADDR	0x07

/* Command GLOBAL_RESET_REQ Reset Types */
#define GLOBAL_RESET	1
#define HOST_RESET_ONLY	2
#define CSE_RESET_ONLY	3

/* HMRFPO Status types */
#define MKHI_HMRFPO_DISABLED	0
#define MKHI_HMRFPO_LOCKED	1
#define MKHI_HMRFPO_ENABLED	2

#endif // SOC_INTEL_COMMON_CSE_H
