/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2008 Advanced Micro Devices, Inc.
 * Copyright (C) ???? Ollie Lo <ollielo@hotmail.com>
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
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
 */


#include <console/console.h>
#include <pc80/keyboard.h>
#include <device/device.h>
#include <arch/io.h>

static int kbc_input_buffer_empty(void)
{
	u32 timeout;
	for(timeout = 1000000; timeout && (inb(0x64) & 0x02); timeout--) {
		inb(0x80);
	}

	if (!timeout) {
		printk_err("Unexpected Keyboard controller input buffer full\n");
	}
	return !!timeout;
}


static int kbc_output_buffer_full(void)
{
	u32 timeout;
	for(timeout = 1000000; timeout && ((inb(0x64) & 0x01) == 0); timeout--) {
		inb(0x80);
	}

	if (!timeout) {
		printk_err("Keyboard controller output buffer result timeout\n");
	}
	return !!timeout;
}


static int kbc_cleanup_buffers(void)
{
	u32 timeout;
	for(timeout = 1000000; timeout && (inb(0x64) & 0x03); timeout--) {
		inb(0x60);
	}

	if (!timeout) {
		printk_err("Couldn't cleanup the keyboard controller buffers\n");
		printk_err("0x64: 0x%x, 0x60: 0x%x\n", inb(0x64), inb(0x60));
	}
	return !!timeout;
}


static u8 send_keyboard(u8 command)
{
	u8 regval = 0;
	u8 resend = 10;

	do {
		if (!kbc_input_buffer_empty()) return 0;
		outb(command, 0x60);
		if (!kbc_output_buffer_full()) return 0;
		regval = inb(0x60);
		--resend;
	} while (regval == 0xFE && resend > 0);

	return regval;
}


static void pc_keyboard_init(struct pc_keyboard *keyboard)
{
	u8 regval;
	u8 resend;
	printk_debug("Keyboard init...\n");

	/* clean up any junk that might have been in the kbc */
	if (!kbc_cleanup_buffers()) return;

	/* reset/self test 8042 - send cmd 0xAA,  */
	if (!kbc_input_buffer_empty()) return;
	outb(0xAA, 0x64);
	if (!kbc_output_buffer_full()) return;

	/* read self-test result, 0x55 is returned in the output buffer (0x60) */
	if ((regval = inb(0x60) != 0x55)) {
		printk_err("Keyboard Controller selftest failed: 0x%x\n", regval);
		return;
	}

	/* Enable keyboard interface - No IRQ*/
	resend = 10;
	regval = 0;
	do {
		if (!kbc_input_buffer_empty()) return;
		outb(0x60, 0x64);
		if (!kbc_input_buffer_empty()) return;
		outb(0x20, 0x60);	/* send cmd: enable keyboard and IRQ 1 */
		if ((inb(0x64) & 0x01)) {
			regval = inb(0x60);
		}
		--resend;
	} while (regval == 0xFE && resend > 0);

	/* clean up any junk that might have been in the keyboard */
	if (!kbc_cleanup_buffers()) return;

	/* reset keyboard and self test (keyboard side) */
	regval = send_keyboard(0xFF);
	if (regval != 0xFA) {
		printk_err("Keyboard selftest failed ACK: 0x%x\n", regval);
		return;
	}
	if (!kbc_output_buffer_full()) return;
	regval = inb(0x60);
	if (regval != 0xAA) {
		printk_err("Keyboard selftest failed: 0x%x\n", regval);
		return;
	}

	/*
	 * The following set scancode stuff is what normal BIOS do. It could be
	 * argued that coreboot shouldn't set the scan code.....
	 */

	/* disable the keyboard */
	regval = send_keyboard(0xF5);
	if (regval != 0xFA) {
		printk_err("Keyboard disable failed ACK: 0x%x\n", regval);
		return;
	}

	/* Set scancode command */
	regval = send_keyboard(0xF0);
	if (regval != 0xFA) {
		printk_err("Keyboard set scancode cmd failed ACK: 0x%x\n", regval);
		return;
	}
	/* Set scancode mode 2 */
	regval = send_keyboard(0x02);
	if (regval != 0xFA) {
		printk_err("Keyboard set scancode mode failed ACK: 0x%x\n", regval);
		return;
	}

	/* enable the keyboard */
	regval = send_keyboard(0xF4);
	if (regval != 0xFA) {
		printk_err("Keyboard enable failed ACK: 0x%x\n", regval);
		return;
	}

	/* All is well - enable keyboard interface */
	resend = 10;
	regval = 0;
	do {
		if (!kbc_input_buffer_empty()) return;
		outb(0x60, 0x64);
		if (!kbc_input_buffer_empty()) return;
		outb(0x61, 0x60);	/* send cmd: enable keyboard and IRQ 1 */
		if ((inb(0x64) & 0x01)) {
			regval = inb(0x60);
		}
		--resend;
	} while (regval == 0xFE && resend > 0);
}


void init_pc_keyboard(unsigned port0, unsigned port1, struct pc_keyboard *kbd)
{
	if ((port0 == 0x60) && (port1 == 0x64)) {
		pc_keyboard_init(kbd);
	}
}

/*
 * Support PS/2 mode -  oddball SIOs(KBC) need this setup
 * Not well documented. Google - 0xcb keyboard controller
 * This is called before pc_keyboard_init().
 */
void set_kbc_ps2_mode()
{
	/* clean up any junk that might have been in the kbc */
	if (!kbc_cleanup_buffers()) return;

	/* reset/self test 8042 before we can do anything */
	if (!kbc_input_buffer_empty()) return;
	outb(0xAA, 0x64);
	if (!kbc_output_buffer_full()) return;

	/* read self-test result, 0x55 is returned in the output buffer (0x60) */
	if ((inb(0x60) != 0x55)) {
		printk_err("Keyboard Controller selftest failed\n");
		return;
	}

	/* Support PS/2 mode */
	if (!kbc_input_buffer_empty()) return;
	outb(0xcb, 0x64);
	if (!kbc_input_buffer_empty()) return;
	outb(0x01, 0x60);
	kbc_cleanup_buffers();
}
