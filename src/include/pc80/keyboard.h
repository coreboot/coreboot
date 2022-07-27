/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef PC80_KEYBOARD_H
#define PC80_KEYBOARD_H

#include <stdint.h>

#define NO_AUX_DEVICE		0
#define PROBE_AUX_DEVICE	1

uint8_t pc_keyboard_init(uint8_t probe_aux);
void set_kbc_ps2_mode(void);

#endif /* PC80_KEYBOARD_H */
