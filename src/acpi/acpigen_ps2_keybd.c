/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <acpi/acpigen.h>
#include <acpi/acpigen_ps2_keybd.h>
#include <console/console.h>
#include <input-event-codes.h>
#include <stdbool.h>
#include <stdint.h>

#define KEYMAP(scancode, keycode) (((uint32_t)(scancode) << 16) | (keycode & 0xFFFF))
#define SCANCODE(keymap) ((keymap >> 16) & 0xFFFF)

/* Possible keymaps for function keys in the top row */
static const uint32_t function_keymaps[] = {
	KEYMAP(0x3b, KEY_F1),
	KEYMAP(0x3c, KEY_F2),
	KEYMAP(0x3d, KEY_F3),
	KEYMAP(0x3e, KEY_F4),
	KEYMAP(0x3f, KEY_F5),
	KEYMAP(0x40, KEY_F6),
	KEYMAP(0x41, KEY_F7),
	KEYMAP(0x42, KEY_F8),
	KEYMAP(0x43, KEY_F9),
	KEYMAP(0x44, KEY_F10),
	KEYMAP(0x57, KEY_F11),
	KEYMAP(0x58, KEY_F12),
	KEYMAP(0x59, KEY_F13),
	KEYMAP(0x5a, KEY_F14),
	KEYMAP(0x5b, KEY_F15),
};

/*
 * Possible keymaps for action keys in the top row. This is a superset of
 * possible keys. Individual keyboards will have a subset of these keys.
 * The scancodes are true / condensed 1 byte scancodes from set-1
 */
static const uint32_t action_keymaps[] = {
	[PS2_KEY_BACK] = KEYMAP(0xea, KEY_BACK),		/* e06a */
	[PS2_KEY_FORWARD] = KEYMAP(0xe9, KEY_FORWARD),		/* e069 */
	[PS2_KEY_REFRESH] = KEYMAP(0xe7, KEY_REFRESH),		/* e067 */
	[PS2_KEY_FULLSCREEN] = KEYMAP(0x91, KEY_FULL_SCREEN),	/* e011 */
	[PS2_KEY_OVERVIEW] = KEYMAP(0x92, KEY_SCALE),		/* e012 */
	[PS2_KEY_VOL_MUTE] = KEYMAP(0xa0, KEY_MUTE),		/* e020 */
	[PS2_KEY_VOL_DOWN] = KEYMAP(0xae, KEY_VOLUMEDOWN),	/* e02e */
	[PS2_KEY_VOL_UP] = KEYMAP(0xb0, KEY_VOLUMEUP),		/* e030 */
	[PS2_KEY_PLAY_PAUSE] = KEYMAP(0x9a, KEY_PLAYPAUSE),	/* e01a */
	[PS2_KEY_NEXT_TRACK] = KEYMAP(0x99, KEY_NEXTSONG),	/* e019 */
	[PS2_KEY_PREV_TRACK] = KEYMAP(0x90, KEY_PREVIOUSSONG),	/* e010 */
	[PS2_KEY_SNAPSHOT] = KEYMAP(0x93, KEY_SYSRQ),		/* e013 */
	[PS2_KEY_BRIGHTNESS_DOWN] = KEYMAP(0x94, KEY_BRIGHTNESSDOWN),	/* e014 */
	[PS2_KEY_BRIGHTNESS_UP] = KEYMAP(0x95, KEY_BRIGHTNESSUP),	/* e015 */
	[PS2_KEY_KBD_BKLIGHT_DOWN] = KEYMAP(0x97, KEY_KBDILLUMDOWN),	/* e017 */
	[PS2_KEY_KBD_BKLIGHT_UP] = KEYMAP(0x98, KEY_KBDILLUMUP),	/* e018 */
	[PS2_KEY_PRIVACY_SCRN_TOGGLE] = KEYMAP(0x96,			/* e016 */
					  KEY_PRIVACY_SCREEN_TOGGLE),
	[PS2_KEY_MICMUTE] = KEYMAP(0x9b, KEY_MICMUTE),			/* e01b */
	[PS2_KEY_KBD_BKLIGHT_TOGGLE] = KEYMAP(0x9e, KEY_KBDILLUMTOGGLE),	/* e01e */
	[PS2_KEY_MENU] = KEYMAP(0xdd, KEY_CONTROLPANEL),	/* e0d5 */
	[PS2_KEY_DICTATE] = KEYMAP(0xa7, KEY_DICTATE),		/* e027*/
	[PS2_KEY_ACCESSIBILITY] = KEYMAP(0xa9, KEY_ACCESSIBILITY),	/* e029 */
	[PS2_KEY_DO_NOT_DISTURB] = KEYMAP(0xa8, KEY_DO_NOT_DISTURB),	/* e028 */
};

/* Keymap for numeric keypad keys */
static uint32_t numeric_keypad_keymaps[] = {
	/* Row-0 */
	KEYMAP(0xc9, KEY_PAGEUP),
	KEYMAP(0xd1, KEY_PAGEDOWN),
	KEYMAP(0xc7, KEY_HOME),
	KEYMAP(0xcf, KEY_END),
	/* Row-1 */
	KEYMAP(0xd3, KEY_DELETE),
	KEYMAP(0xb5, KEY_KPSLASH),
	KEYMAP(0x37, KEY_KPASTERISK),
	KEYMAP(0x4a, KEY_KPMINUS),
	/* Row-2 */
	KEYMAP(0x47, KEY_KP7),
	KEYMAP(0x48, KEY_KP8),
	KEYMAP(0x49, KEY_KP9),
	KEYMAP(0x4e, KEY_KPPLUS),
	/* Row-3 */
	KEYMAP(0x4b, KEY_KP4),
	KEYMAP(0x4c, KEY_KP5),
	KEYMAP(0x4d, KEY_KP6),
	/* Row-4 */
	KEYMAP(0x4f, KEY_KP1),
	KEYMAP(0x50, KEY_KP2),
	KEYMAP(0x51, KEY_KP3),
	KEYMAP(0x9c, KEY_KPENTER),
	/* Row-5 */
	KEYMAP(0x52, KEY_KP0),
	KEYMAP(0x53, KEY_KPDOT),
};

/*
 * Keymap for rest of non-top-row keys. This is a superset of all the possible
 * keys that any chromeos keyboards can have.
 */
static uint32_t rest_of_keymaps[] = {
	/* Row-0 */
	KEYMAP(0x01, KEY_ESC),
	/* Row-1 */
	KEYMAP(0x29, KEY_GRAVE),
	KEYMAP(0x02, KEY_1),
	KEYMAP(0x03, KEY_2),
	KEYMAP(0x04, KEY_3),
	KEYMAP(0x05, KEY_4),
	KEYMAP(0x06, KEY_5),
	KEYMAP(0x07, KEY_6),
	KEYMAP(0x08, KEY_7),
	KEYMAP(0x09, KEY_8),
	KEYMAP(0x0a, KEY_9),
	KEYMAP(0x0b, KEY_0),
	KEYMAP(0x0c, KEY_MINUS),
	KEYMAP(0x0d, KEY_EQUAL),
	KEYMAP(0x7d, KEY_YEN),		/* JP keyboards only */
	KEYMAP(0x0e, KEY_BACKSPACE),
	/* Row-2 */
	KEYMAP(0x0f, KEY_TAB),
	KEYMAP(0x10, KEY_Q),
	KEYMAP(0x11, KEY_W),
	KEYMAP(0x12, KEY_E),
	KEYMAP(0x13, KEY_R),
	KEYMAP(0x14, KEY_T),
	KEYMAP(0x15, KEY_Y),
	KEYMAP(0x16, KEY_U),
	KEYMAP(0x17, KEY_I),
	KEYMAP(0x18, KEY_O),
	KEYMAP(0x19, KEY_P),
	KEYMAP(0x1a, KEY_LEFTBRACE),
	KEYMAP(0x1b, KEY_RIGHTBRACE),
	KEYMAP(0x2b, KEY_BACKSLASH),
	/* Row-3 */
	KEYMAP(0xdb, KEY_LEFTMETA),	/* Search Key */
	KEYMAP(0x1e, KEY_A),
	KEYMAP(0x1f, KEY_S),
	KEYMAP(0x20, KEY_D),
	KEYMAP(0x21, KEY_F),
	KEYMAP(0x22, KEY_G),
	KEYMAP(0x23, KEY_H),
	KEYMAP(0x24, KEY_J),
	KEYMAP(0x25, KEY_K),
	KEYMAP(0x26, KEY_L),
	KEYMAP(0x27, KEY_SEMICOLON),
	KEYMAP(0x28, KEY_APOSTROPHE),
	KEYMAP(0x1c, KEY_ENTER),
	/* Row-4 */
	KEYMAP(0x2a, KEY_LEFTSHIFT),
	KEYMAP(0x56, KEY_102ND),	/* UK keyboards only */
	KEYMAP(0x2c, KEY_Z),
	KEYMAP(0x2d, KEY_X),
	KEYMAP(0x2e, KEY_C),
	KEYMAP(0x2f, KEY_V),
	KEYMAP(0x30, KEY_B),
	KEYMAP(0x31, KEY_N),
	KEYMAP(0x32, KEY_M),
	KEYMAP(0x33, KEY_COMMA),
	KEYMAP(0x34, KEY_DOT),
	KEYMAP(0x35, KEY_SLASH),
	KEYMAP(0x73, KEY_RO),		/* JP keyboards only */
	KEYMAP(0x36, KEY_RIGHTSHIFT),
	/* Row-5 */
	KEYMAP(0x1d, KEY_LEFTCTRL),
	KEYMAP(0x38, KEY_LEFTALT),
	KEYMAP(0x7b, KEY_MUHENKAN),	/* JP keyboards only */
	KEYMAP(0x39, KEY_SPACE),
	KEYMAP(0x79, KEY_HENKAN),	/* JP keyboards only */
	KEYMAP(0xb8, KEY_RIGHTALT),
	KEYMAP(0x9d, KEY_RIGHTCTRL),
	/* Arrow keys */
	KEYMAP(0xcb, KEY_LEFT),
	KEYMAP(0xd0, KEY_DOWN),
	KEYMAP(0xcd, KEY_RIGHT),
	KEYMAP(0xc8, KEY_UP),
	/* Power Key */
	KEYMAP(0xde, KEY_POWER),
};

static void ssdt_generate_physmap(struct acpi_dp *dp, uint8_t num_top_row_keys,
				  enum ps2_action_key action_keys[])
{
	struct acpi_dp *dp_array;
	enum ps2_action_key key;
	uint32_t keymap, i;

	dp_array = acpi_dp_new_table("function-row-physmap");
	if (!dp_array) {
		printk(BIOS_ERR, "PS2K: couldn't write function-row-physmap\n");
		return;
	}

	printk(BIOS_INFO, "PS2K: Physmap: [");
	for (i = 0; i < num_top_row_keys; i++) {
		key = action_keys[i];
		if (key && key < ARRAY_SIZE(action_keymaps)) {
			keymap = action_keymaps[key];
		} else {
			keymap = 0;
			printk(BIOS_ERR,
			       "PS2K: invalid top-action-key-%u: %u(skipped)\n",
			       i, key);
		}
		acpi_dp_add_integer(dp_array, NULL, SCANCODE(keymap));
		printk(BIOS_INFO, " %X", SCANCODE(keymap));
	}

	printk(BIOS_INFO, " ]\n");
	acpi_dp_add_array(dp, dp_array);
}

static void ssdt_generate_keymap(struct acpi_dp *dp, uint8_t num_top_row_keys,
				 enum ps2_action_key action_keys[],
				 bool can_send_function_keys,
				 bool has_numeric_keypad,
				 bool has_scrnlock_key,
				 bool has_assistant_key,
				 bool has_alpha_num_punct_keys)
{
	struct acpi_dp *dp_array;
	enum ps2_action_key key;
	uint32_t keymap;
	unsigned int i, total = 0;

	dp_array = acpi_dp_new_table("linux,keymap");
	if (!dp_array) {
		printk(BIOS_ERR, "PS2K: couldn't write linux,keymap\n");
		return;
	}

	/* Write out keymap for top row action keys */
	for (i = 0; i < num_top_row_keys; i++) {
		key = action_keys[i];
		if (!key || key >= ARRAY_SIZE(action_keymaps)) {
			printk(BIOS_ERR,
			       "PS2K: invalid top-action-key-%u: %u\n", i, key);
			continue;
		}
		keymap = action_keymaps[key];
		acpi_dp_add_integer(dp_array, NULL, keymap);
		total++;
	}

	/* Write out keymap for function keys, if keyboard can send them */
	if (can_send_function_keys) {
		for (i = 0; i < num_top_row_keys; i++) {
			keymap = function_keymaps[i];
			acpi_dp_add_integer(dp_array, NULL, keymap);
		}

		/* Add the Fn-key */
		if (CONFIG_ACPI_FNKEY_GEN_SCANCODE != 0) {
			acpi_dp_add_integer(dp_array, NULL, KEYMAP(CONFIG_ACPI_FNKEY_GEN_SCANCODE,
								   KEY_FN));
			total++;
		}

		total += num_top_row_keys;
	}

	/* Write out keymap for numeric keypad, if the keyboard has it */
	if (has_numeric_keypad) {
		for (i = 0; i < ARRAY_SIZE(numeric_keypad_keymaps); i++) {
			keymap = numeric_keypad_keymaps[i];
			acpi_dp_add_integer(dp_array, NULL, keymap);
		}

		total += ARRAY_SIZE(numeric_keypad_keymaps);
	}

	/* Provide keymap for screenlock only if it is present */
	if (has_scrnlock_key) {
		acpi_dp_add_integer(dp_array, NULL, KEYMAP(0x5d, KEY_SLEEP));
		total++;
	}

	/* Add the keymap for the assistant key if present */
	if (has_assistant_key) {
		acpi_dp_add_integer(dp_array, NULL, KEYMAP(0x5c, KEY_ASSISTANT));
		total++;
	}

	/* Provide alphanumeric and punctuation keys (rest of the keyboard) if
	 * present
	 */
	if (has_alpha_num_punct_keys) {
		for (i = 0; i < ARRAY_SIZE(rest_of_keymaps); i++) {
			keymap = rest_of_keymaps[i];
			acpi_dp_add_integer(dp_array, NULL, keymap);
		}
	}

	total += ARRAY_SIZE(rest_of_keymaps);
	printk(BIOS_INFO, "PS2K: Passing %u keymaps to kernel\n", total);

	acpi_dp_add_array(dp, dp_array);
}

void acpigen_ps2_keyboard_dsd(const char *scope, uint8_t num_top_row_keys,
			      enum ps2_action_key action_keys[],
			      bool can_send_function_keys,
			      bool has_numeric_keypad,
			      bool has_scrnlock_key,
			      bool has_assistant_key,
			      bool has_alpha_num_punct_keys)
{
	struct acpi_dp *dsd;

	if (!scope ||
	    num_top_row_keys < PS2_MIN_TOP_ROW_KEYS ||
	    num_top_row_keys > PS2_MAX_TOP_ROW_KEYS) {
		printk(BIOS_ERR, "PS2K: %s: invalid args\n", __func__);
		return;
	}

	dsd = acpi_dp_new_table("_DSD");
	if (!dsd) {
		printk(BIOS_ERR, "PS2K: couldn't write _DSD\n");
		return;
	}

	acpigen_write_scope(scope);
	ssdt_generate_physmap(dsd, num_top_row_keys, action_keys);
	ssdt_generate_keymap(dsd, num_top_row_keys, action_keys,
			     can_send_function_keys, has_numeric_keypad,
			     has_scrnlock_key, has_assistant_key,
			     has_alpha_num_punct_keys);
	acpi_dp_write(dsd);
	acpigen_pop_len(); /* Scope */
}
