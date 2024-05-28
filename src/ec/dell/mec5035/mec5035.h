/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef _EC_DELL_MEC5035_H_
#define _EC_DELL_MEC5035_H_

#include <stdint.h>

#define NUM_REGISTERS	32

enum mec5035_cmd {
	CMD_MOUSE_TP = 0x1a,
	CMD_RADIO_CTRL = 0x2b,
	CMD_CPU_OK = 0xc2,
};

/* Touchpad (TP) and mouse related. The EC seems to
   default to 0 which results in the TP not working. */
enum ec_mouse_setting {
	SERIAL_MOUSE = 0, /* Disable TP, force use of a serial mouse */
	PS2_MOUSE, /* Disable TP when using a PS/2 mouse */
	TP_PS2_MOUSE /* Leave TP enabled when using a PS/2 mouse */
};

#define RADIO_CTRL_NUM_ARGS 3
enum ec_radio_dev {
	RADIO_WLAN = 0,
	RADIO_WWAN,
	RADIO_BT
};

enum ec_radio_state {
	RADIO_OFF = 0,
	RADIO_ON
};

u8 mec5035_mouse_touchpad(enum ec_mouse_setting setting);
void mec5035_cpu_ok(void);
void mec5035_early_init(void);
void mec5035_control_radio(enum ec_radio_dev device, enum ec_radio_state state);

#endif /* _EC_DELL_MEC5035_H_ */
