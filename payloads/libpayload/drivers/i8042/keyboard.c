/*
 *
 * Copyright (C) 2008 Advanced Micro Devices, Inc.
 * Copyright (C) 2017 Patrick Rudolph <siro@das-labor.org>
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#include <stdbool.h>
#include <stdint.h>

#include <keycodes.h>
#include <libpayload-config.h>
#include <libpayload.h>

#include "i8042.h"

#ifdef DEBUG
#define debug(x...) printf(x)
#else
#define debug(x...) do {} while (0)
#endif

#define POWER_BUTTON         0x90
#define MEDIA_KEY_PREFIX     0xE0

struct layout_maps {
	const char *country;
	const unsigned short map[4][0x59];
};

static struct layout_maps *map;
static int modifier = 0;
int (*media_key_mapping_callback)(char ch);

static struct layout_maps keyboard_layouts[] = {
#if CONFIG(LP_PC_KEYBOARD_LAYOUT_US)
{ .country = "us", .map = {
	{ /* No modifier */
	 0x00, 0x1B, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36,
	 0x37, 0x38, 0x39, 0x30, 0x2D, 0x3D, 0x08, 0x09,
	 0x71, 0x77, 0x65, 0x72, 0x74, 0x79, 0x75, 0x69,
	 0x6F, 0x70, 0x5B, 0x5D, 0x0A, 0x00, 0x61, 0x73,
	 0x64, 0x66, 0x67, 0x68, 0x6A, 0x6B, 0x6C, 0x3B,
	 0x27, 0x60, 0x00, 0x5C, 0x7A, 0x78, 0x63, 0x76,
	 0x62, 0x6E, 0x6D, 0x2C, 0x2E, 0x2F, 0x00, 0x2A,
	 0x00, 0x20, 0x00, KEY_F(1), KEY_F(2), KEY_F(3), KEY_F(4), KEY_F(5),
	 KEY_F(6), KEY_F(7), KEY_F(8), KEY_F(9), KEY_F(10), 0x00, 0x00, KEY_HOME,
	 KEY_UP, KEY_NPAGE, 0x00, KEY_LEFT, 0x00, KEY_RIGHT, 0x00, KEY_END,
	 KEY_DOWN, KEY_PPAGE, 0x00, KEY_DC, 0x00, 0x00, 0x00, KEY_F(11),
	 KEY_F(12),
	 },
	{ /* Shift */
	 0x00, 0x1B, 0x21, 0x40, 0x23, 0x24, 0x25, 0x5E,
	 0x26, 0x2A, 0x28, 0x29, 0x5F, 0x2B, 0x08, 0x00,
	 0x51, 0x57, 0x45, 0x52, 0x54, 0x59, 0x55, 0x49,
	 0x4F, 0x50, 0x7B, 0x7D, 0x0A, 0x00, 0x41, 0x53,
	 0x44, 0x46, 0x47, 0x48, 0x4A, 0x4B, 0x4C, 0x3A,
	 0x22, 0x7E, 0x00, 0x7C, 0x5A, 0x58, 0x43, 0x56,
	 0x42, 0x4E, 0x4D, 0x3C, 0x3E, 0x3F, 0x00, 0x2A,
	 0x00, 0x20, 0x00, KEY_F(1), KEY_F(2), KEY_F(3), KEY_F(4), KEY_F(5),
	 KEY_F(6), KEY_F(7), KEY_F(8), KEY_F(9), KEY_F(10), 0x00, 0x00, KEY_HOME,
	 KEY_UP, KEY_NPAGE, 0x00, KEY_LEFT, 0x00, KEY_RIGHT, 0x00, KEY_END,
	 KEY_DOWN, KEY_PPAGE, 0x00, KEY_DC, 0x00, 0x00, 0x00, KEY_F(11),
	 KEY_F(12),
	 },
	{ /* ALT */
	 0x00, 0x1B, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36,
	 0x37, 0x38, 0x39, 0x30, 0x2D, 0x3D, 0x08, 0x09,
	 0x71, 0x77, 0x65, 0x72, 0x74, 0x79, 0x75, 0x69,
	 0x6F, 0x70, 0x5B, 0x5D, 0x0A, 0x00, 0x61, 0x73,
	 0x64, 0x66, 0x67, 0x68, 0x6A, 0x6B, 0x6C, 0x3B,
	 0x27, 0x60, 0x00, 0x5C, 0x7A, 0x78, 0x63, 0x76,
	 0x62, 0x6E, 0x6D, 0x2C, 0x2E, 0x2F, 0x00, 0x2A,
	 0x00, 0x20, 0x00, KEY_F(1), KEY_F(2), KEY_F(3), KEY_F(4), KEY_F(5),
	 KEY_F(6), KEY_F(7), KEY_F(8), KEY_F(9), KEY_F(10), 0x00, 0x00, KEY_HOME,
	 KEY_UP, KEY_NPAGE, 0x00, KEY_LEFT, 0x00, KEY_RIGHT, 0x00, KEY_END,
	 KEY_DOWN, KEY_PPAGE, 0x00, KEY_DC, 0x00, 0x00, 0x00, KEY_F(11),
	 KEY_F(12),
	 },
	{ /* Shift-ALT */
	 0x00, 0x1B, 0x21, 0x40, 0x23, 0x24, 0x25, 0x5E,
	 0x26, 0x2A, 0x28, 0x29, 0x5F, 0x2B, 0x08, 0x00,
	 0x51, 0x57, 0x45, 0x52, 0x54, 0x59, 0x55, 0x49,
	 0x4F, 0x50, 0x7B, 0x7D, 0x0A, 0x00, 0x41, 0x53,
	 0x44, 0x46, 0x47, 0x48, 0x4A, 0x4B, 0x4C, 0x3A,
	 0x22, 0x7E, 0x00, 0x7C, 0x5A, 0x58, 0x43, 0x56,
	 0x42, 0x4E, 0x4D, 0x3C, 0x3E, 0x3F, 0x00, 0x2A,
	 0x00, 0x20, 0x00, KEY_F(1), KEY_F(2), KEY_F(3), KEY_F(4), KEY_F(5),
	 KEY_F(6), KEY_F(7), KEY_F(8), KEY_F(9), KEY_F(10), 0x00, 0x00, KEY_HOME,
	 KEY_UP, KEY_NPAGE, 0x00, KEY_LEFT, 0x00, KEY_RIGHT, 0x00, KEY_END,
	 KEY_DOWN, KEY_PPAGE, 0x00, KEY_DC, 0x00, 0x00, 0x00, KEY_F(11),
	 KEY_F(12),
	 }
}},
#endif
#if CONFIG(LP_PC_KEYBOARD_LAYOUT_DE)
{ .country = "de", .map = {
	{ /* No modifier */
	 0x00, 0x1B, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36,
	 0x37, 0x38, 0x39, 0x30, 0x00, 0x27, 0x08, 0x09,
	 0x71, 0x77, 0x65, 0x72, 0x74, 0x7A, 0x75, 0x69,
	 0x6F, 0x70, 0x00, 0x2B, 0x0A, 0x00, 0x61, 0x73,
	 0x64, 0x66, 0x67, 0x68, 0x6A, 0x6B, 0x6C, 0x00,
	 0x00, 0x5E, 0x00, 0x23, 0x79, 0x78, 0x63, 0x76,
	 0x62, 0x6E, 0x6D, 0x2C, 0x2E, 0x2D, 0x00, 0x2A,
	 0x00, 0x20, 0x00, KEY_F(1), KEY_F(2), KEY_F(3), KEY_F(4), KEY_F(5),
	 KEY_F(6), KEY_F(7), KEY_F(8), KEY_F(9), KEY_F(10), 0x00, 0x00, KEY_HOME,
	 KEY_UP, KEY_NPAGE, 0x00, KEY_LEFT, 0x00, KEY_RIGHT, 0x00, KEY_END,
	 KEY_DOWN, KEY_PPAGE, 0x00, KEY_DC, 0x00, 0x00, 0x3C, KEY_F(11),
	 KEY_F(12),
	 },
	{ /* Shift */
	 0x00, 0x1B, 0x21, 0x22, 0xA7, 0x24, 0x25, 0x26,
	 0x2F, 0x28, 0x29, 0x3D, 0x3F, 0x60, 0x08, 0x00,
	 0x51, 0x57, 0x45, 0x52, 0x54, 0x5A, 0x55, 0x49,
	 0x4F, 0x50, 0x00, 0x2A, 0x0A, 0x00, 0x41, 0x53,
	 0x44, 0x46, 0x47, 0x48, 0x4A, 0x4B, 0x4C, 0x00,
	 0x00, 0x7E, 0x00, 0x27, 0x59, 0x58, 0x43, 0x56,
	 0x42, 0x4E, 0x4D, 0x3B, 0x3A, 0x5F, 0x00, 0x2A,
	 0x00, 0x20, 0x00, KEY_F(1), KEY_F(2), KEY_F(3), KEY_F(4), KEY_F(5),
	 KEY_F(6), KEY_F(7), KEY_F(8), KEY_F(9), KEY_F(10), 0x00, 0x00, KEY_HOME,
	 KEY_UP, KEY_NPAGE, 0x00, KEY_LEFT, 0x00, KEY_RIGHT, 0x00, KEY_END,
	 KEY_DOWN, KEY_PPAGE, 0x00, KEY_DC, 0x00, 0x00, 0x3E, KEY_F(11),
	 KEY_F(12),
	 },
	{ /* ALT */
	 0x00, 0x1B, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36,
	 0x7B, 0x5B, 0x5D, 0x7D, 0x5C, 0x3D, 0x08, 0x09,
	 0x40, 0x77, 0x65, 0x72, 0x74, 0x79, 0x75, 0x69,
	 0x6F, 0x70, 0x5B, 0x7E, 0x0A, 0x00, 0x61, 0x73,
	 0x64, 0x66, 0x67, 0x68, 0x6A, 0x6B, 0x6C, 0x3B,
	 0x27, 0x60, 0x00, 0x5C, 0x7A, 0x78, 0x63, 0x76,
	 0x62, 0x6E, 0x6D, 0x2C, 0x2E, 0x2F, 0x00, 0x2A,
	 0x00, 0x20, 0x00, KEY_F(1), KEY_F(2), KEY_F(3), KEY_F(4), KEY_F(5),
	 KEY_F(6), KEY_F(7), KEY_F(8), KEY_F(9), KEY_F(10), 0x00, 0x00, KEY_HOME,
	 KEY_UP, KEY_NPAGE, 0x00, KEY_LEFT, 0x00, KEY_RIGHT, 0x00, KEY_END,
	 KEY_DOWN, KEY_PPAGE, 0x00, KEY_DC, 0x00, 0x00, 0x7C, KEY_F(11),
	 KEY_F(12),
	 },
	{ /* Shift-ALT */
	 /* copied from US */
	 0x00, 0x1B, 0x21, 0x40, 0x23, 0x24, 0x25, 0x5E,
	 0x26, 0x2A, 0x28, 0x29, 0x5F, 0x2B, 0x08, 0x00,
	 0x51, 0x57, 0x45, 0x52, 0x54, 0x59, 0x55, 0x49,
	 0x4F, 0x50, 0x7B, 0x7D, 0x0A, 0x00, 0x41, 0x53,
	 0x44, 0x46, 0x47, 0x48, 0x4A, 0x4B, 0x4C, 0x3A,
	 0x22, 0x7E, 0x00, 0x7C, 0x5A, 0x58, 0x43, 0x56,
	 0x42, 0x4E, 0x4D, 0x3C, 0x3E, 0x3F, 0x00, 0x2A,
	 0x00, 0x20, 0x00, KEY_F(1), KEY_F(2), KEY_F(3), KEY_F(4), KEY_F(5),
	 KEY_F(6), KEY_F(7), KEY_F(8), KEY_F(9), KEY_F(10), 0x00, 0x00, KEY_HOME,
	 KEY_UP, KEY_NPAGE, 0x00, KEY_LEFT, 0x00, KEY_RIGHT, 0x00, KEY_END,
	 KEY_DOWN, KEY_PPAGE, 0x00, KEY_DC, 0x00, 0x00, 0x00, KEY_F(11),
	 KEY_F(12),
	 }
}},
#endif
};

static void keyboard_drain_input(void)
{
	while (i8042_data_ready_ps2())
		(void)i8042_read_data_ps2();
}

static bool keyboard_cmd(unsigned char cmd)
{
	const uint64_t timeout_us = cmd == I8042_KBCMD_RESET ? 1*1000*1000 : 200*1000;
	const uint64_t start_time = timer_us(0);

	i8042_write_data(cmd);

	do {
		if (!i8042_data_ready_ps2()) {
			udelay(50);
			continue;
		}

		const uint8_t data = i8042_read_data_ps2();
		switch (data) {
		case 0xfa:
			return true;
		case 0xfe:
			return false;
		default:
			/* Warn only if we already disabled keyboard input. */
			if (cmd != I8042_KBCMD_DEFAULT_DIS)
				debug("WARNING: Keyboard sent spurious 0x%02x.\n", data);
			break;
		}
	} while (timer_us(start_time) < timeout_us);

	debug("ERROR: Keyboard command timed out.\n");
	return false;
}

static bool set_scancode_set(const unsigned char set)
{
	bool ret;

	if (set < 1 || set > 3)
		return false;

	ret = keyboard_cmd(I8042_KBCMD_SET_SCANCODE);
	if (!ret) {
		debug("ERROR: Keyboard set scancode failed!\n");
		return ret;
	}

	ret = keyboard_cmd(set);
	if (!ret) {
		debug("ERROR: Keyboard scancode set#%u failed!\n", set);
		return ret;
	}

	return ret;
}

static enum keyboard_state {
	STATE_INIT = 0,
	STATE_SIMPLIFIED_INIT,
	STATE_DISABLE_SCAN,
	STATE_DRAIN_INPUT,
	STATE_DISABLE_TRANSLATION,
	STATE_START_SELF_TEST,
	STATE_SELF_TEST,
	STATE_CONFIGURE,
	STATE_CONFIGURE_SET1,
	STATE_ENABLE_TRANSLATION,
	STATE_ENABLE_SCAN,
	STATE_RUNNING,
	STATE_IGNORE,
} keyboard_state;

static uint64_t keyboard_time;
static uint64_t state_time;

static void keyboard_poll(void)
{
	enum keyboard_state next_state = keyboard_state;
	unsigned int i;

	switch (keyboard_state) {

	case STATE_INIT:
		/* Wait until keyboard_init() has been called. */
		break;

	case STATE_SIMPLIFIED_INIT:
		/* On the first try, start opportunistically, do
		   the first steps at once and skip the self-test. */
		(void)keyboard_cmd(I8042_KBCMD_DEFAULT_DIS);
		keyboard_drain_input();
		(void)i8042_set_kbd_translation(false);
		next_state = STATE_CONFIGURE;
		break;

	case STATE_DISABLE_SCAN:
		(void)keyboard_cmd(I8042_KBCMD_DEFAULT_DIS);
		next_state = STATE_DRAIN_INPUT;
		break;

	case STATE_DRAIN_INPUT:
		/* Limit number of bytes drained per poll. */
		for (i = 0; i < 50 && i8042_data_ready_ps2(); ++i)
			(void)i8042_read_data_ps2();
		if (i == 0)
			next_state = STATE_DISABLE_TRANSLATION;
		break;

	case STATE_DISABLE_TRANSLATION:
		/* Be opportunistic and assume it's disabled on failure. */
		(void)i8042_set_kbd_translation(false);
		next_state = STATE_START_SELF_TEST;
		break;

	case STATE_START_SELF_TEST:
		if (!keyboard_cmd(I8042_KBCMD_RESET))
			debug("ERROR: Keyboard self-test couldn't be started.\n");
		/* We ignore errors and always move to the self-test state
		   which will simply try again if necessary. */
		next_state = STATE_SELF_TEST;
		break;

	case STATE_SELF_TEST:
		if (!i8042_data_ready_ps2()) {
			if (timer_us(state_time) > 5*1000*1000)
				next_state = STATE_DISABLE_SCAN;
			break;
		}

		const uint8_t self_test_result = i8042_read_data_ps2();
		switch (self_test_result) {
		case 0xaa:
			/* Success! */
			next_state = STATE_CONFIGURE;
			break;
		case 0xfc:
		case 0xfd:
			/* Failure. Try again. */
			next_state = STATE_START_SELF_TEST;
			break;
		default:
			debug("WARNING: Keyboard self-test received spurious 0x%02x\n",
			       self_test_result);
			break;
		}
		break;

	case STATE_CONFIGURE:
		if (set_scancode_set(2))
			next_state = STATE_ENABLE_TRANSLATION;
		else
			next_state = STATE_CONFIGURE_SET1;
		break;

	case STATE_CONFIGURE_SET1:
		if (!set_scancode_set(1)) {
			debug("ERROR: Keyboard failed to set any scancode set.\n");
			next_state = STATE_DISABLE_SCAN;
			break;
		}

		next_state = STATE_ENABLE_SCAN;
		break;

	case STATE_ENABLE_TRANSLATION:
		if (i8042_set_kbd_translation(true) != 0) {
			debug("ERROR: Keyboard controller set translation failed!\n");
			next_state = STATE_DISABLE_SCAN;
			break;
		}

		next_state = STATE_ENABLE_SCAN;
		break;

	case STATE_ENABLE_SCAN:
		if (!keyboard_cmd(I8042_KBCMD_EN)) {
			debug("ERROR: Keyboard enable scanning failed!\n");
			next_state = STATE_DISABLE_SCAN;
			break;
		}

		next_state = STATE_RUNNING;
		break;

	case STATE_RUNNING:
		/* TODO: Use echo command to detect detach. */
		break;

	case STATE_IGNORE:
		/* TODO: Try again after timeout if it ever seems useful. */
		break;

	}

	switch (next_state) {
	case STATE_INIT:
	case STATE_RUNNING:
	case STATE_IGNORE:
		break;
	default:
		if (timer_us(keyboard_time) > 30*1000*1000)
			next_state = STATE_IGNORE;
		break;
	}

	if (keyboard_state != next_state) {
		keyboard_state = next_state;
		state_time = timer_us(0);
	}
}

bool keyboard_havechar(void)
{
	keyboard_poll();
	return keyboard_state == STATE_RUNNING && i8042_data_ready_ps2();
}

unsigned char keyboard_get_scancode(void)
{
	unsigned char ch;

	while (!keyboard_havechar()) ;

	ch = i8042_read_data_ps2();

	switch (ch) {
	case 0x36:
	case 0x2a:
		modifier |= KB_MOD_SHIFT;
		break;
	case 0x80 | 0x36:
	case 0x80 | 0x2a:
		modifier &= ~KB_MOD_SHIFT;
		break;
	case 0x38:
		modifier |= KB_MOD_ALT;
		break;
	case 0x80 | 0x38:
		modifier &= ~KB_MOD_ALT;
		break;
	case 0x1d:
		modifier |= KB_MOD_CTRL;
		break;
	case 0x80 | 0x1d:
		modifier &= ~KB_MOD_CTRL;
		break;
	case 0x3a:
		if (modifier & KB_MOD_CAPSLOCK) {
			modifier &= ~KB_MOD_CAPSLOCK;
			if (keyboard_cmd(I8042_KBCMD_SET_MODE_IND))
				keyboard_cmd(I8042_MODE_CAPS_LOCK_OFF);
		} else {
			modifier |= KB_MOD_CAPSLOCK;
			if (keyboard_cmd(I8042_KBCMD_SET_MODE_IND))
				keyboard_cmd(I8042_MODE_CAPS_LOCK_ON);
		}
		break;
	}

	return ch;
}

int keyboard_getmodifier(void)
{
	return modifier;
}

void initialize_keyboard_media_key_mapping_callback(int (*media_key_mapper)(char))
{
	media_key_mapping_callback = media_key_mapper;
}

int keyboard_getchar(void)
{
	unsigned char ch;
	int shift;
	int ret = 0;

	while (!keyboard_havechar()) ;

	ch = keyboard_get_scancode();
	if ((media_key_mapping_callback != NULL) && (ch == MEDIA_KEY_PREFIX)) {
		ch = keyboard_get_scancode();
		return media_key_mapping_callback(ch);
	}

	if (!(ch & 0x80) && ch < 0x59) {
		shift =
		    (modifier & KB_MOD_SHIFT) ^ (modifier & KB_MOD_CAPSLOCK) ? 1 : 0;

		if (modifier & KB_MOD_ALT)
			shift += 2;

		ret = map->map[shift][ch];

		if (modifier & KB_MOD_CTRL) {
			switch (ret) {
			case 'a' ... 'z':
				ret &= 0x1f;
				break;
			case KEY_DC:
				/* vulcan nerve pinch */
				if ((modifier & KB_MOD_ALT) && reset_handler)
					reset_handler();
				/* fallthrough */
			default:
				ret = 0;
			}
		}
	}

	if (ch == 0x5e)
		ret = POWER_BUTTON;

	return ret;
}

/**
 * Set keyboard layout
 * @param country string describing the keyboard layout language.
 * Valid values are "us", "de".
 */

int keyboard_set_layout(char *country)
{
	int i;

	for (i=0; i<ARRAY_SIZE(keyboard_layouts); i++) {
		if (strncmp(keyboard_layouts[i].country, country,
					strlen(keyboard_layouts[i].country)))
			continue;

		/* Found, changing keyboard layout */
		map = &keyboard_layouts[i];
		return 0;
	}

	/* Nothing found, not changed */
	return -1;
}

static struct console_input_driver cons = {
	.havekey = (int (*)(void))keyboard_havechar,
	.getchar = keyboard_getchar,
	.input_type = CONSOLE_INPUT_TYPE_EC,
};

void keyboard_init(void)
{
	if (keyboard_state != STATE_INIT)
		return;

	map = &keyboard_layouts[0];

	/* Initialized keyboard controller. */
	if (!i8042_probe() || !i8042_has_ps2())
		return;

	/* Enable first PS/2 port */
	i8042_cmd(I8042_CMD_EN_KB);

	keyboard_state = STATE_SIMPLIFIED_INIT;
	keyboard_time = state_time = timer_us(0);

	console_add_input_driver(&cons);
}

void keyboard_disconnect(void)
{
	/* If 0x64 returns 0xff, then we have no keyboard
	 * controller */
	if (inb(0x64) == 0xFF)
		return;

	if (!i8042_has_ps2())
		return;

	/* Disable scanning */
	keyboard_cmd(I8042_KBCMD_DEFAULT_DIS);
	keyboard_drain_input();

	/* Send keyboard disconnect command */
	i8042_cmd(I8042_CMD_DIS_KB);

	/* Hand off with empty buffer */
	keyboard_drain_input();

	/* Release keyboard controller driver */
	i8042_close();

	keyboard_state = STATE_INIT;
}
