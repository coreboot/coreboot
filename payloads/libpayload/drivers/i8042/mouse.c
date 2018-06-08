/*
 * This file is part of the libpayload project.
 *
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

#include <libpayload-config.h>
#include <libpayload.h>

static int x_axis;
static int y_axis;
static int z_axis;
static u32 buttons;
static u8 is_intellimouse;
static u8 is_explorer_intellimouse;
static u8 initialized;
static unsigned char mouse_buf[4];
static unsigned char mouse_buf_idx;

static u8 mouse_cmd(unsigned char cmd)
{
	i8042_cmd(0xd4);

	i8042_write_data(cmd);

	return i8042_wait_read_aux() == 0xfa;
}

static u8 mouse_cmd_data(u8 cmd, u8 val)
{
	if (!mouse_cmd(cmd))
		return 0;

	return mouse_cmd(val);
}

/** Try to detect Microsoft Intelli mouse */
static u8 mouse_is_intellimouse(void)
{
	/* Silence mouse. */
	if (!mouse_cmd(0xf5))
		return 0;

	/* Set standard. */
	if (!mouse_cmd(0xf6))
		return 0;

	/* Magic sequence. */
	if (!mouse_cmd_data(0xf3, 0xc8))
		return 0;
	if (!mouse_cmd_data(0xf3, 0x64))
		return 0;
	if (!mouse_cmd_data(0xf3, 0x50))
		return 0;

	/* Get mouse id */
	if (!mouse_cmd(0xf2))
		return 0;

	if (i8042_wait_read_aux() != 0x03)
		return 0;

	return 1;
}

/** Try to detect Microsoft Explorer mouse */
static u8 mouse_is_intellimouse_explorer(void)
{
	/* Silence mouse. */
	if (!mouse_cmd(0xf5))
		return 0;

	/* Set standard. */
	if (!mouse_cmd(0xf6))
		return 0;

	/* Magic sequence. */
	if (!mouse_cmd_data(0xf3, 0xc8))
		return 0;
	if (!mouse_cmd_data(0xf3, 0xc8))
		return 0;
	if (!mouse_cmd_data(0xf3, 0x50))
		return 0;

	/* Get mouse id */
	if (!mouse_cmd(0xf2))
		return 0;

	if (i8042_wait_read_aux() != 4)
		return 0;

	return 1;
}

/** Decode temporary buffer
 * Sanity check to prevent out of order decode.
 * Decode PS/2 data.
 * Supported devices:
 *  Generic 3 button mouse
 *  Microsoft Intelli mouse
 *  Microsoft Explorer mouse
 */
static void mouse_decode(void)
{
	/* Buffer full check and sanity check */
	if (is_intellimouse) {
		if (mouse_buf_idx < 4)
			return;
		if ((mouse_buf[3] & 0x10) != (mouse_buf[3] & 0x08)) {
			mouse_buf_idx = 0;
			return;
		}
	} else if (is_explorer_intellimouse) {
		if (mouse_buf_idx < 4)
			return;
		if (mouse_buf[3] & 0xc0) {
			mouse_buf_idx = 0;
			return;
		}
	} else {
		if (mouse_buf_idx < 3)
			return;
	}

	/* Common protocol */
	x_axis += mouse_buf[1] ? mouse_buf[1] - ((mouse_buf[0] << 4) & 0x100) : 0;
	y_axis += mouse_buf[2] ? ((mouse_buf[0] << 3) & 0x100) - mouse_buf[2] : 0;
	buttons = mouse_buf[0] & 0x7;

	/* Extended protocol */
	if (is_intellimouse) {
		z_axis += (mouse_buf[3] & 0x7) - (mouse_buf[3] & 0x08) ? 8 : 0;
	} else if (is_explorer_intellimouse) {
		z_axis += (mouse_buf[3] & 0x7) - (mouse_buf[3] & 0x08) ? 8 : 0;
		buttons = (mouse_buf[0] & 0x7) | (mouse_buf[3] & 0x30) >> 1;
	}

	mouse_buf_idx = 0;
}

/** Insert data into internal temporary buffer. */
static void insert_buf(unsigned char c)
{
	/* Validate input:
	 * First byte shall have bit 3 set ! */
	if (!mouse_buf_idx && !(c & 8))
		return;

	mouse_buf[mouse_buf_idx++] = c;
}

/** Probe i8042 for new aux data and try to decode it. */
static void mouse_sample(void)
{
	if (!initialized)
		return;

	while (i8042_data_ready_aux()) {
		insert_buf(i8042_read_data_aux());
		mouse_decode();
	}
}

/** Mouse cursor interface method
 * Return and reset internal state.
 */
static void mouse_state(int *x, int *y, int *z, u32 *b)
{
	if (!initialized)
		return;

	mouse_sample();

	if (x) {
		*x = x_axis;
		x_axis = 0;
	}
	if (y) {
		*y = y_axis;
		y_axis = 0;
	}
	if (z) {
		*z = z_axis;
		z_axis = 0;
	}
	if (b)
		*b = buttons;
}

static struct mouse_cursor_input_driver curs = {
	.get_state = mouse_state,
	.input_type = CURSOR_INPUT_TYPE_PS2,
};

/** Probe for PS/2 mouse */
void i8042_mouse_init(void)
{
	int ret;

	/**
	 * Initialize keyboard controller.
	 * Might fail in case no AUX port or firmware disabled the AUX port.
	 */
	if (!i8042_probe() || !i8042_has_aux())
		return;

	/* Empty mouse buffer. */
	while (i8042_data_ready_aux())
		i8042_read_data_aux();

	/* Enable mouse.
	 * Documentation is unclear at this point.
	 * Some recommend to wait for response, some claim there's none.
	 * No response on Lenovo H8 EC.
	 * Ignore it ... */
	ret = i8042_cmd(0xa8);
	if (ret == -1)
		return;

	/* Silence mouse. */
	if (!mouse_cmd(0xf5))
		return;

	/* Read mouse id. */
	if (!mouse_cmd(0xf2))
		return;

	ret = i8042_wait_read_aux();
	if (ret)
		return;

	/* Get and enable features (scroll wheel and 5 buttons) */
	is_intellimouse = mouse_is_intellimouse();
	is_explorer_intellimouse = mouse_is_intellimouse_explorer();

	/* Set defaults. */
	if (!mouse_cmd(0xf6))
		return;

	/* Enable data transmission. */
	if (!mouse_cmd(0xf4))
		return;

	initialized = 1;

	/* Register mouse cursor driver */
	mouse_cursor_add_input_driver(&curs);
}

/* Disable PS/2 mouse. */
void i8042_mouse_disconnect(void)
{
	/* If 0x64 returns 0xff, then we have no keyboard
	 * controller */
	if (inb(0x64) == 0xFF || !initialized)
		return;

	/* Empty keyboard buffer */
	while (i8042_data_ready_aux())
		i8042_read_data_aux();

	/* Disable mouse. */
	i8042_cmd(0xa7);

	initialized = 0;

	/* Release keyboard controller driver */
	i8042_close();
}
