/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2012 Advanced Micro Devices, Inc.
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
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */

#include <stdint.h>
#include <arch/cpu.h>
#include <console/console.h>
#include <device/device.h>
#include "w83793.h"
#include <device/smbus.h>
#include "chip.h"

static int w83793_fan_limit(device_t dev, int fan, uint16_t limit)
{
	return smbus_write_byte(dev, 0x90 + fan * 2, limit >> 8) ||
		smbus_write_byte(dev, 0x91 + fan * 2, limit & 0xff);
}

static int w83793_bank(device_t dev, int bank)
{
	return smbus_write_byte(dev, 0, bank);
}

static int w83793_td_level(device_t dev, int fan, const char *level)
{
	fan *= 0x10;

	smbus_write_byte(dev, 0x30 + fan, level[0]);
	smbus_write_byte(dev, 0x31 + fan, level[1]);
	smbus_write_byte(dev, 0x32 + fan, level[2]);
	smbus_write_byte(dev, 0x33 + fan, level[3]);
	smbus_write_byte(dev, 0x34 + fan, level[4]);
	smbus_write_byte(dev, 0x35 + fan, level[5]);
	smbus_write_byte(dev, 0x36 + fan, level[6]);
	return 0;
}

static int w83793_tr_level(device_t dev, int fan, const char *level)
{
	fan *= 0x10;

	smbus_write_byte(dev, 0x70 + fan, level[0]);
	smbus_write_byte(dev, 0x71 + fan, level[1]);
	smbus_write_byte(dev, 0x72 + fan, level[2]);
	smbus_write_byte(dev, 0x73 + fan, level[3]);
	smbus_write_byte(dev, 0x74 + fan, level[4]);
	smbus_write_byte(dev, 0x75 + fan, level[5]);
	smbus_write_byte(dev, 0x76 + fan, level[6]);
	return 0;
}


static int w83793_td_fan_level(device_t dev, int fan, const char *level)
{
	fan *= 0x10;

	smbus_write_byte(dev, 0x38 + fan, level[0]);
	smbus_write_byte(dev, 0x39 + fan, level[1]);
	smbus_write_byte(dev, 0x3a + fan, level[2]);
	smbus_write_byte(dev, 0x3b + fan, level[3]);
	smbus_write_byte(dev, 0x3c + fan, level[4]);
	smbus_write_byte(dev, 0x3d + fan, level[5]);
	smbus_write_byte(dev, 0x3e + fan, level[6]);
	return 0;
}

static int w83793_tr_fan_level(device_t dev, int fan, const char *level)
{
	fan *= 0x10;

	smbus_write_byte(dev, 0x78 + fan, level[0]);
	smbus_write_byte(dev, 0x79 + fan, level[1]);
	smbus_write_byte(dev, 0x7a + fan, level[2]);
	smbus_write_byte(dev, 0x7b + fan, level[3]);
	smbus_write_byte(dev, 0x7c + fan, level[4]);
	smbus_write_byte(dev, 0x7d + fan, level[5]);
	smbus_write_byte(dev, 0x7e + fan, level[6]);
	return 0;
}


static void w83793_init(device_t dev)
{
	struct drivers_i2c_w83793_config *config = dev->chip_info;
	uint16_t id;
	int i;

	if (w83793_bank(dev, 0))
		printk(BIOS_ERR, "%s: failed\n", __func__);

	if (!config)
		return;

	id = smbus_read_byte(dev, 0x0d);
	w83793_bank(dev, 0x80);
	id |= smbus_read_byte(dev, 0x0d) << 8;
	printk(BIOS_ERR, "ID: %04x\n", id);

	/* reset configuration */
	smbus_write_byte(dev, 0x40, 0x80);
	smbus_write_byte(dev, 0x51, 0x06);

	/* Multi function control */
	smbus_write_byte(dev, 0x58, config->mfc);

	/* FANIN_Ctrl */
	smbus_write_byte(dev, 0x5c, config->fanin);

	/* Temperature reported by PECI */
	smbus_write_byte(dev, 0x5e, 0xff);
	/* TR monitor enable */
	smbus_write_byte(dev, 0x5f, config->tr_enable);
	/* PECI Agent configuration */
	smbus_write_byte(dev, 0xd0, config->peci_agent_conf);
	/* TCase */
	smbus_write_byte(dev, 0xd1, config->tcase0);
	smbus_write_byte(dev, 0xd2, config->tcase1);
	smbus_write_byte(dev, 0xd3, config->tcase2);
	smbus_write_byte(dev, 0xd4, config->tcase3);
	/* PECI Reportstyle */
	smbus_write_byte(dev, 0xd5, 0x00);

	for (i = 0; i < 9; i++)
		w83793_fan_limit(dev, i, 0x768);

	/* Fan output style control */
	smbus_write_byte(dev, 0xb0, 0x00);
	smbus_write_byte(dev, 0xb1, 0x00);

	/* FAN Uptime */
	smbus_write_byte(dev, 0xc3, 0x02);

	/* FAN downtime */
	smbus_write_byte(dev, 0xc4, 0x03);

	/* ALL FAN critical temperature */
	smbus_write_byte(dev, 0xc5, config->critical_temperature);

	/* Temperature offset */
	smbus_write_byte(dev, 0xa8, 0xf9);
	smbus_write_byte(dev, 0xa9, 0xf9);
	smbus_write_byte(dev, 0xaa, 0xf9);
	smbus_write_byte(dev, 0xab, 0xf9);

	/* BANK 2 */
	smbus_write_byte(dev, 0x00, 0x02);

	/* TD FAN select */
	smbus_write_byte(dev, 0x01, config->td1_fan_select);
	smbus_write_byte(dev, 0x02, config->td2_fan_select);
	smbus_write_byte(dev, 0x03, config->td3_fan_select);
	smbus_write_byte(dev, 0x04, config->td4_fan_select);

	smbus_write_byte(dev, 0x05, config->tr1_fan_select);
	smbus_write_byte(dev, 0x06, config->tr2_fan_select);

	/* FAN control mode */
	smbus_write_byte(dev, 0x07, 0x00);

	/* hysteresis tolerance */
	smbus_write_byte(dev, 0x08, 0xaa);
	smbus_write_byte(dev, 0x09, 0xaa);

	/* FanNonStop */
	smbus_write_byte(dev, 0x18, 0x1d);
	smbus_write_byte(dev, 0x19, 0x04);
	smbus_write_byte(dev, 0x1a, 0x04);
	smbus_write_byte(dev, 0x1b, 0x04);
	smbus_write_byte(dev, 0x1c, 0x04);
	smbus_write_byte(dev, 0x1d, 0x04);
	smbus_write_byte(dev, 0x1e, 0x04);
	smbus_write_byte(dev, 0x1f, 0x04);

	/* FanStart */
	smbus_write_byte(dev, 0x20, 0x08);
	smbus_write_byte(dev, 0x21, 0x08);
	smbus_write_byte(dev, 0x22, 0x08);
	smbus_write_byte(dev, 0x23, 0x08);
	smbus_write_byte(dev, 0x24, 0x08);
	smbus_write_byte(dev, 0x25, 0x08);
	smbus_write_byte(dev, 0x26, 0x08);
	smbus_write_byte(dev, 0x27, 0x08);

	for (i = 0; i < 4; i++)
		w83793_td_level(dev, i, (const char[]){ 0x32, 0x32, 0x32, 0x32, 0x37, 0x41, 0x4b });

	for (i = 0; i < 2; i++)
		w83793_tr_level(dev, i, (const char[]){ 0x1e, 0x23, 0x28, 0x2d, 0x32, 0x37, 0x3c });

	for (i = 0; i < 4; i++)
		w83793_td_fan_level(dev, i, (const char[]){ 0x1d, 0x1d, 0x1d, 0x1d, 0x1d, 0x1d, 0x26 });

	for (i = 0; i < 2; i++)
		w83793_tr_fan_level(dev, i, (const char[]){ 0x08, 0x0c, 0x10, 0x18, 0x20, 0x30, 0x38 });


	smbus_write_byte(dev, 0x00, 0x00);

	/* Fan output style */
	smbus_write_byte(dev, 0xb4, 0x00);
	smbus_write_byte(dev, 0xb5, 0x00);

	/* start monitoring operation */
	smbus_write_byte(dev, 0x40, 0x09);

}

static void w83793_noop(device_t dummy)
{
}

static struct device_operations w83793_operations = {
	.read_resources = w83793_noop,
	.set_resources = w83793_noop,
	.enable_resources = w83793_noop,
	.init = w83793_init,
};

static void enable_dev(device_t dev)
{
	dev->ops = &w83793_operations;
}

struct chip_operations drivers_i2c_w83793_ops = {
	CHIP_NAME("Nuvoton W83793 Hardware Monitor")
	.enable_dev = enable_dev,
};
