/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef PC80_KEYBOARD_H
#define PC80_KEYBOARD_H

#include <types.h>

#define NO_AUX_DEVICE		0
#define PROBE_AUX_DEVICE	1

/*
 * The Keyboard controller command byte
 *
 *  BIT | Description
 *  ----+-------------------------------------------------------
 *   7  | reserved, must be zero   :
 *   6  | XT Translation           : 1 = on,      0 = off
 *   5  | Disable Mouse Port       : 1 = disable, 0 = enable
 *   4  | Disable Keyboard Port    : 1 = disable, 0 = enable
 *   3  | reserved, must be zero   :
 *   2  | System Flag              : 1 = self-test passed (DO NOT SET TO ZERO)
 *   1  | Mouse Port Interrupts    : 1 = enable,  0 = disable
 *   0  | Keyboard Port Interrupts : 1 = enable,  0 = disable
 *  ----+-------------------------------------------------------
 */
enum {
	PC_KBC_KBD_INT     = 0,
	PC_KBC_AUX_INT     = 1,
	PC_KBC_SYS         = 2,
	PC_KBC_KBD_DISABLE = 4,
	PC_KBC_AUX_DISABLE = 5,
	PC_KBC_TRANSLATE   = 6,
};

uint8_t pc_keyboard_init(uint8_t probe_aux);
void set_kbc_ps2_mode(void);
enum cb_err pc_keyboard_set_command_byte_bit(u8 bit, u8 value);

#endif /* PC80_KEYBOARD_H */
