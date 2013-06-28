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
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 *
 * Mailbox EC communication interface for Google Chrome Embedded Controller.
 */

#ifndef _EC_GOOGLE_CHROMEEC_EC_H
#define _EC_GOOGLE_CHROMEEC_EC_H
#include <stddef.h>
#include <stdint.h>

#ifndef __PRE_RAM__
int google_chromeec_i2c_xfer(uint8_t chip, uint8_t addr, int alen,
			     uint8_t *buffer, int len, int is_read);
u32 google_chromeec_get_wake_mask(void);
int google_chromeec_set_sci_mask(u32 mask);
int google_chromeec_set_smi_mask(u32 mask);
int google_chromeec_set_wake_mask(u32 mask);
u8 google_chromeec_get_event(void);
int google_ec_running_ro(void);
u16 google_chromeec_get_board_version(void);
void google_chromeec_init(void);
#endif

void google_chromeec_early_init(void);
uint8_t google_chromeec_calc_checksum(const uint8_t *data, int size);
u32 google_chromeec_get_events_b(void);
int google_chromeec_kbbacklight(int percent);
void google_chromeec_post(u8 postcode);
void google_chromeec_log_events(u32 mask);

enum usb_charge_mode {
	USB_CHARGE_MODE_DISABLED,
	USB_CHARGE_MODE_CHARGE_AUTO,
	USB_CHARGE_MODE_CHARGE_BC12,
	USB_CHARGE_MODE_DOWNSTREAM_500MA,
	USB_CHARGE_MODE_DOWNSTREAM_1500MA,
};
int google_chromeec_set_usb_charge_mode(u8 port_id, enum usb_charge_mode mode);

/* internal structure to send a command to the EC and wait for response. */
struct chromeec_command {
	uint16_t    cmd_code;	  /* command code in, status out */
	uint8_t     cmd_version;  /* command version */
	const void* cmd_data_in;  /* command data, if any */
	void*	    cmd_data_out; /* command response, if any */
	uint16_t    cmd_size_in;  /* size of command data */
	uint16_t    cmd_size_out; /* expected size of command response in,
				   * actual received size out */
};

/* internal standard implementation for EC command protocols. */
typedef int (*crosec_io_t)(uint8_t *write_bytes, size_t write_size,
			   uint8_t *read_bytes, size_t read_size,
			   void *context);
int crosec_command_proto(struct chromeec_command *cec_command,
			 crosec_io_t crosec_io, void *context);

int google_chromeec_command(struct chromeec_command *cec_command);

#endif /* _EC_GOOGLE_CHROMEEC_EC_H */
