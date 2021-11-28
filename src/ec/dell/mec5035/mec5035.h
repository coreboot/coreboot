/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef _EC_DELL_MEC5035_H_
#define _EC_DELL_MEC5035_H_

#include <stdint.h>

#define NUM_REGISTERS	32

/* Touchpad (TP) and mouse related. The EC seems to
   default to 0 which results in the TP not working. */
#define CMD_MOUSE_TP	0x1a
#define SERIAL_MOUSE	0 /* Disable TP, force use of a serial mouse */
#define PS2_MOUSE	1 /* Disable TP when using a PS/2 mouse */
#define TP_PS2_MOUSE	2 /* Leave TP enabled when using a PS/2 mouse */

#define CMD_CPU_OK	0xc2

u8 mec5035_mouse_touchpad(u8 setting);
void mec5035_cpu_ok(void);
void mec5035_early_init(void);

#endif /* _EC_DELL_MEC5035_H_ */
