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
#include <usb/usb.h>

static struct mouse_cursor_input_driver *cursor_in;

static int
mouse_cursor_driver_exists(const struct mouse_cursor_input_driver *const in)
{
	struct mouse_cursor_input_driver *head = cursor_in;

	while (head) {
		if (head == in)
			return 1;
		head = head->next;
	}

	return 0;
}

/** Add new mouse cursor input driver */
void mouse_cursor_add_input_driver(struct mouse_cursor_input_driver *const in)
{
	/* Check if this driver was already added to the console list */
	if (mouse_cursor_driver_exists(in))
		return;
	in->next = cursor_in;
	cursor_in = in;
}

/** Init enabled mouse cursor drivers */
void mouse_cursor_init(void)
{
#if IS_ENABLED(CONFIG_LP_PC_MOUSE)
	i8042_mouse_init();
#endif
}

static u32 mouse_buttons;
static u8 mouse_acceleration = 0x10;
/* Fixed-point 1/256 units */
static int mouse_rel_x;
static int mouse_rel_y;
static int mouse_rel_z;
static u32 mouse_speed = 0x299;

/** Test for fast moving cursor */
static u8 mouse_cusor_is_fast(int x, int y)
{
	return (x * x + y * y) > (mouse_acceleration * mouse_acceleration);
}

/**
 * Poll for mouse data.
 *
 * Polls all drivers for new mouse data.
 * Applies accelerations to relative movement.
 * Logical ORs all buttons states.
 * Call often to prevent driver's queue overrun !
 */
void mouse_cursor_poll(void)
{
	struct mouse_cursor_input_driver *in;
	int rel_x, rel_y, rel_z;
	u32 buttons;

	mouse_buttons = 0;
	/* Iterate over all connected drivers */
	for (in = cursor_in; in != NULL; in = in->next)
		if (in->get_state) {
			in->get_state(&rel_x, &rel_y, &rel_z, &buttons);

			/* Accumulate relative movement */
			if (mouse_cusor_is_fast(rel_x, rel_y)) {
				/* Apply simple cursor acceleration. Default: x2.6 */
				mouse_rel_x += rel_x * mouse_speed;
				mouse_rel_y += rel_y * mouse_speed;
				mouse_rel_z += rel_z * mouse_speed;
			} else {
				mouse_rel_x += rel_x * 256;
				mouse_rel_y += rel_y * 256;
				mouse_rel_z += rel_z * 256;
			}

			/* Logic or all buttons */
			mouse_buttons |= buttons;
		}
}

/**
 * Get relative mouse movement.
 *
 * Returns relative mouse movement with acceleration
 * applied. The internal state will be cleared and stays
 * clear until one of the drivers provide motion input again.
 */
void mouse_cursor_get_rel(int *x, int *y, int *z)
{
	mouse_cursor_poll();

	if (x) {
		*x = mouse_rel_x / 256;
		mouse_rel_x = 0;
	}
	if (y) {
		*y = mouse_rel_y / 256;
		mouse_rel_y = 0;
	}
	if (z) {
		*z = mouse_rel_z / 256;
		mouse_rel_z = 0;
	}
}

/**
 * Get mouse button state.
 *
 * Returns the current button states.
 * There are up to 32 possible buttons,
 * but most devices only implement three buttons.
 */
u32 mouse_cursor_get_buttons(void)
{
	mouse_cursor_poll();

	return mouse_buttons;
}

/**
 * Set cursor speed.
 *
 * Sets the mouse cursor speed coefficient.
 * It is used in case the cursor is moving faster
 * than the mouse acceleration coefficient.
 */
void mouse_cursor_set_speed(u32 val)
{
	mouse_speed = val;
}

/**
 * Get cursor speed.
 *
 * Returns the internal mouse cursor speed in
 * 1/256th units.
 * Default: 0x299 ~ 2.6
 */
u32 mouse_cursor_get_speed(void)
{
	return mouse_speed;
}

/**
 * Set cursor acceleration.
 *
 * Sets the mouse cursor acceleration coefficient.
 * The acceleration is used to compare the raw relative
 * cursor movement against. If greater the raw values are
 * multiplied by mouse_speed to fasten the cursor.
 */
void mouse_cursor_set_acceleration(u8 val)
{
	mouse_acceleration = val;
}

/**
 * Get cursor acceleration.
 *
 * Returns the current cursor acceleration coefficient.
 * Default: 0x10
 */
u8 mouse_cursor_get_acceleration(void)
{
	return mouse_acceleration;
}
