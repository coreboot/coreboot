/* SPDX-License-Identifier: GPL-2.0-only */

#include <arch/io.h>
#include <console/console.h>
#include <device/device.h>
#include <delay.h>

#include "ec.h"
#include "chip.h"

static u16 ec_data_port;
static u16 ec_cmd_port;

#define   KBD_IBF	(1 << 1) /* 1: input buffer full (data ready for ec) */
#define   KBD_OBF	(1 << 0) /* 1: output buffer full (data ready for host) */

static void ec_setports(u16 data, u16 cmd)
{
	ec_data_port = data;
	ec_cmd_port = cmd;
}

static int send_kbd_command(u8 command)
{
	int timeout;

	timeout = 100000; /* 1 second */
	while ((inb(ec_cmd_port) & KBD_IBF) && --timeout) {
		udelay(10);
		if ((timeout & 0xff) == 0)
			printk(BIOS_SPEW, ".");
	}
	if (!timeout) {
		printk(BIOS_DEBUG, "Timeout while sending command 0x%02x to EC!\n",
				command);
		return -1;
	}

	outb(command, ec_cmd_port);
	return 0;
}

static int send_kbd_data(u8 data)
{
	int timeout;

	timeout = 100000; /* 1 second */
	while ((inb(ec_cmd_port) & KBD_IBF) && --timeout) { /* wait for IBF = 0 */
		udelay(10);
		if ((timeout & 0xff) == 0)
			printk(BIOS_SPEW, ".");
	}
	if (!timeout) {
		printk(BIOS_DEBUG, "Timeout while sending data 0x%02x to EC!\n",
				data);
		return -1;
	}

	outb(data, ec_data_port);
	return 0;
}

/*
 * kbc1126_thermalinit: initialize fan control
 * The code is found in EcThermalInit of the vendor firmware.
 */
static int kbc1126_thermalinit(u8 cmd, u8 value)
{
	printk(BIOS_DEBUG, "KBC1126: initialize fan control.");

	if (send_kbd_command(cmd) < 0)
		return -1;

	if (send_kbd_data(0x27) < 0)
		return -1;

	if (send_kbd_data(0x01) < 0)
		return -1;

	/*
	 * The following code is needed for fan control when AC is plugged in.
	 */

	if (send_kbd_command(cmd) < 0)
		return -1;

	if (send_kbd_data(0xd5) < 0)
		return -1;

	if (send_kbd_data(value) < 0)
		return -1;

	printk(BIOS_DEBUG, "KBC1126: fan control initialized.\n");
	return 0;
}

/*
 * kbc1126_kbdled: set CapsLock and NumLock LEDs
 * This is used in MemoryErrorReport of the vendor firmware.
 */
static void kbc1126_kbdled(u8 cmd, u8 val)
{
	if (send_kbd_command(cmd) < 0)
		return;

	if (send_kbd_data(0xf0) < 0)
		return;

	if (send_kbd_data(val) < 0)
		return;
}

static void kbc1126_enable(struct device *dev)
{
	struct ec_hp_kbc1126_config *conf = dev->chip_info;
	ec_setports(conf->ec_data_port, conf->ec_cmd_port);
	kbc1126_kbdled(conf->ec_ctrl_reg, 0);
	if (kbc1126_thermalinit(conf->ec_ctrl_reg, conf->ec_fan_ctrl_value) < 0)
		printk(BIOS_DEBUG, "KBC1126: error when initializing fan control.\n");
}

struct chip_operations ec_hp_kbc1126_ops = {
	CHIP_NAME("SMSC KBC1126 for HP laptops")
	.enable_dev = kbc1126_enable
};
