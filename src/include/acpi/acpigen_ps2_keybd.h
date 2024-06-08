/* SPDX-License-Identifier: GPL-2.0-or-later */

#ifndef __ACPI_ACPIGEN_PS2_KEYBD_H__
#define __ACPI_ACPIGEN_PS2_KEYBD_H__

#include <types.h>

enum ps2_action_key {
	PS2_KEY_ABSENT = 0,
	PS2_KEY_BACK,
	PS2_KEY_FORWARD,
	PS2_KEY_REFRESH,
	PS2_KEY_FULLSCREEN,
	PS2_KEY_OVERVIEW,
	PS2_KEY_BRIGHTNESS_DOWN,
	PS2_KEY_BRIGHTNESS_UP,
	PS2_KEY_VOL_MUTE,
	PS2_KEY_VOL_DOWN,
	PS2_KEY_VOL_UP,
	PS2_KEY_SNAPSHOT,
	PS2_KEY_PRIVACY_SCRN_TOGGLE,
	PS2_KEY_KBD_BKLIGHT_DOWN,
	PS2_KEY_KBD_BKLIGHT_UP,
	PS2_KEY_PLAY_PAUSE,
	PS2_KEY_NEXT_TRACK,
	PS2_KEY_PREV_TRACK,
	PS2_KEY_KBD_BKLIGHT_TOGGLE,
	PS2_KEY_MICMUTE,
	PS2_KEY_MENU,
	PS2_KEY_DICTATE,
	PS2_KEY_ACCESSIBILITY,
	PS2_KEY_DO_NOT_DISTURB,
};

#define PS2_MIN_TOP_ROW_KEYS		2
#define PS2_MAX_TOP_ROW_KEYS		15

void acpigen_ps2_keyboard_dsd(const char *scope, uint8_t num_top_row_keys,
			      enum ps2_action_key action_keys[],
			      bool can_send_function_keys,
			      bool has_numeric_keypad, bool has_scrnlock_key,
			      bool has_assistant_key,
			      bool has_alpha_num_punct_keys);

#endif /* __ACPI_ACPIGEN_PS2_KEYBD_H__ */
