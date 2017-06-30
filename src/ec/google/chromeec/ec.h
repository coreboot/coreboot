/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2012 The Chromium OS Authors. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * Mailbox EC communication interface for Google Chrome Embedded Controller.
 */

#ifndef _EC_GOOGLE_CHROMEEC_EC_H
#define _EC_GOOGLE_CHROMEEC_EC_H
#include <stddef.h>
#include <stdint.h>
#include "ec_commands.h"

/* Fill in base and size of the IO port resources used. */
void google_chromeec_ioport_range(uint16_t *base, size_t *size);

int google_chromeec_i2c_xfer(uint8_t chip, uint8_t addr, int alen,
			     uint8_t *buffer, int len, int is_read);
u32 google_chromeec_get_wake_mask(void);
int google_chromeec_set_sci_mask(u32 mask);
int google_chromeec_set_smi_mask(u32 mask);
int google_chromeec_set_wake_mask(u32 mask);
u8 google_chromeec_get_event(void);
int google_ec_running_ro(void);
void google_chromeec_init(void);

/* Device events */
uint32_t google_chromeec_get_device_enabled_events(void);
int google_chromeec_set_device_enabled_events(uint32_t mask);
uint32_t google_chromeec_get_device_current_events(void);
void google_chromeec_log_device_events(uint32_t mask);

/* If recovery mode is enabled and EC is not running RO firmware reboot. */
void google_chromeec_early_init(void);
/* Reboot if EC firmware is not expected type. */
void google_chromeec_check_ec_image(int expected_type);
void google_chromeec_check_pd_image(int expected_type);

int google_chromeec_check_feature(int feature);
uint8_t google_chromeec_calc_checksum(const uint8_t *data, int size);
u16 google_chromeec_get_board_version(void);
u32 google_chromeec_get_events_b(void);
int google_chromeec_clear_events_b(u32 mask);
int google_chromeec_kbbacklight(int percent);
void google_chromeec_post(u8 postcode);
void google_chromeec_log_events(u32 mask);
int google_chromeec_vbnv_context(int is_read, uint8_t *data, int len);
uint8_t google_chromeec_get_switches(void);

/* Temporary secure storage commands */
int google_chromeec_vstore_supported(void);
int google_chromeec_vstore_info(uint32_t *locked);
int google_chromeec_vstore_read(int slot, uint8_t *data);
int google_chromeec_vstore_write(int slot, uint8_t *data, size_t size);

/* Issue reboot command to EC with specified type and flags. Returns 0 on
   success, < 0 otherwise. */
int google_chromeec_reboot(int dev_idx, enum ec_reboot_cmd type, uint8_t flags);

/* MEC uses 0x800/0x804 as register/index pair, thus an 8-byte resource. */
#define MEC_EMI_BASE		0x800
#define MEC_EMI_SIZE		8

/* For MEC, access ranges 0x800 thru 0x9ff using EMI interface instead of LPC */
#define MEC_EMI_RANGE_START EC_HOST_CMD_REGION0
#define MEC_EMI_RANGE_END   (EC_LPC_ADDR_MEMMAP + EC_MEMMAP_SIZE)

void mec_io_bytes(int write, u16 offset, unsigned int length,
		  u8 *buf, u8 *csum);

enum usb_charge_mode {
	USB_CHARGE_MODE_DISABLED,
	USB_CHARGE_MODE_CHARGE_AUTO,
	USB_CHARGE_MODE_CHARGE_BC12,
	USB_CHARGE_MODE_DOWNSTREAM_500MA,
	USB_CHARGE_MODE_DOWNSTREAM_1500MA,
};
int google_chromeec_set_usb_charge_mode(u8 port_id, enum usb_charge_mode mode);
int google_chromeec_set_usb_pd_role(u8 port, enum usb_pd_control_role role);

/* internal structure to send a command to the EC and wait for response. */
struct chromeec_command {
	uint16_t    cmd_code;	  /* command code in, status out */
	uint8_t     cmd_version;  /* command version */
	const void* cmd_data_in;  /* command data, if any */
	void*	    cmd_data_out; /* command response, if any */
	uint16_t    cmd_size_in;  /* size of command data */
	uint16_t    cmd_size_out; /* expected size of command response in,
				   * actual received size out */
	int         cmd_dev_index;/* device index for passthru */
};

/*
 * There are transport level constraints for sending protov3 packets. Because
 * of this provide a way for the generic protocol layer to request buffers
 * so that there is zero copying being done through the layers.
 *
 * Request the buffer provided the size. If 'req' is non-zero then the
 * buffer requested is for EC requests. Otherwise it's for responses. Return
 * non-NULL on success, NULL on error.
 */
void *crosec_get_buffer(size_t size, int req);

/*
 * The lower level transport works on the buffers handed out to the
 * upper level. Therefore, only the size of the request and response
 * are required.
 */
typedef int (*crosec_io_t)(size_t req_size, size_t resp_size, void *context);
int crosec_command_proto(struct chromeec_command *cec_command,
			 crosec_io_t crosec_io, void *context);

int google_chromeec_command(struct chromeec_command *cec_command);

#endif /* _EC_GOOGLE_CHROMEEC_EC_H */
