/* SPDX-License-Identifier: GPL-2.0-only */

#include <arch/io.h>
#include <console/console.h>
#include <device/device.h>
#include <device/pnp.h>
#include <option.h>
#include <pc80/keyboard.h>
#include <stdint.h>
#include "mec5035.h"

static const u16 MAILBOX_INDEX = 0x910;
static const u16 MAILBOX_DATA = MAILBOX_INDEX + 1;

static inline u8 __get_mailbox_register(u8 index)
{
	outb(index + 0x10, MAILBOX_INDEX);
	return inb(MAILBOX_DATA);
}

static inline void __set_mailbox_register(u8 index, u8 data)
{
	outb(index + 0x10, MAILBOX_INDEX);
	outb(data, MAILBOX_DATA);
}

static void wait_ec(void)
{
	u8 busy;
	do {
		outb(0, MAILBOX_INDEX);
		busy = inb(MAILBOX_DATA);
	} while (busy);
}


static enum cb_err read_mailbox_regs(u8 *data, u8 start, u8 count)
{
	if (start + count >= NUM_REGISTERS) {
		printk(BIOS_ERR, "%s: Invalid start or count argument.\n", __func__);
		return CB_ERR_ARG;
	}

	while (count--) {
		*data = __get_mailbox_register(start);
		data++;
		start++;
	}

	return CB_SUCCESS;
}

static enum cb_err write_mailbox_regs(const u8 *data, u8 start, u8 count)
{
	if (start + count >= NUM_REGISTERS) {
		printk(BIOS_ERR, "%s: Invalid start or count argument.\n", __func__);
		return CB_ERR_ARG;
	}

	while (count--) {
		__set_mailbox_register(start, *data);
		data++;
		start++;
	}

	return CB_SUCCESS;
}

static void ec_command(enum mec5035_cmd cmd)
{
	outb(0, MAILBOX_INDEX);
	outb((u8)cmd, MAILBOX_DATA);
	wait_ec();
}

u8 mec5035_mouse_touchpad(enum ec_mouse_setting setting)
{
	u8 buf[15] = {(u8)setting};
	write_mailbox_regs(buf, 2, 1);
	ec_command(CMD_MOUSE_TP);
	/* The vendor firmware reads 15 bytes starting at index 1, presumably
	   to get some sort of return code. Though I don't know for sure if
	   this is the case. Assume the first byte is the return code. */
	read_mailbox_regs(buf, 1, 15);
	return buf[0];
}

void mec5035_control_radio(enum ec_radio_dev dev, enum ec_radio_state state)
{
	/* From LPC traces and userspace testing with other values,
	   the second byte has to be 2 for an unknown reason. */
	u8 buf[RADIO_CTRL_NUM_ARGS] = {(u8)dev, 2, (u8)state};
	write_mailbox_regs(buf, 2, RADIO_CTRL_NUM_ARGS);
	ec_command(CMD_RADIO_CTRL);
}

void mec5035_early_init(void)
{
	/* If this isn't sent the EC shuts down the system after about 15
	   seconds, flashing a pattern on the keyboard LEDs corresponding
	   to "processor failure" according to Dell service manuals. */
	ec_command(CMD_CPU_OK);
}

static void mec5035_init(struct device *dev)
{
	/* Unconditionally use this argument for now as this setting
	   is probably the most sensible default out of the 3 choices. */
	mec5035_mouse_touchpad(TP_PS2_MOUSE);

	pc_keyboard_init(NO_AUX_DEVICE);

	mec5035_control_radio(RADIO_WLAN, get_uint_option("wlan", RADIO_ON));
	mec5035_control_radio(RADIO_WWAN, get_uint_option("wwan", RADIO_ON));
	mec5035_control_radio(RADIO_BT, get_uint_option("bluetooth", RADIO_ON));
}

static struct device_operations ops = {
	.init = mec5035_init,
	.read_resources = noop_read_resources,
	.set_resources = noop_set_resources
};

static struct pnp_info pnp_dev_info[] = {
	{ NULL, 0, 0, 0, }
};

static void mec5035_enable(struct device *dev)
{
	pnp_enable_devices(dev, &ops, ARRAY_SIZE(pnp_dev_info), pnp_dev_info);
}

struct chip_operations ec_dell_mec5035_ops = {
	.name = "MEC5035 EC",
	.enable_dev = mec5035_enable,
};
