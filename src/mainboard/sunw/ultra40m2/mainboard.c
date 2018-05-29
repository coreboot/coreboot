/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2007 AMD
 * Written by Yinghai Lu <yinghailu@amd.com> for AMD.
 * Copyright (C) 2010 coresystems GmbH
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <arch/io.h>
#include <console/console.h>
#include <device/device.h>
#include <device/pnp_def.h>
#include <device/smbus.h>
#include <stdlib.h>
#include <superio/smsc/dme1737/dme1737.h>

static void emc6d103_init(void)
{
	size_t i;
	int reg;

	static const struct { uint8_t idx; uint8_t msk; uint8_t set; } script[] = {
		{ 0x7f, 0x7f, 0x80 }, // INIT
		{ 0x4f, 0x00, 0x64 }, // Diode 1 High 100°C
		{ 0x53, 0x00, 0x64 }, // Diode 2 High 100°C (Sun firmware didn't set this)
		{ 0x54, 0x00, 0x30 }, // Tach1 Minimum LSB
		{ 0x55, 0x00, 0x2a }, // Tach1 Minimum MSB
		{ 0x56, 0x00, 0x30 }, // Tach2 Minimum LSB
		{ 0x57, 0x00, 0x2a }, // Tach2 Minimum MSB
		{ 0x5c, 0x00, 0x02 }, // PWM 1 Config
		{ 0x5d, 0x00, 0x42 }, // PWM 2 Config
		{ 0x5f, 0x00, 0x8a }, // Zone 1 range, Fan 1 freq
		{ 0x60, 0x00, 0xca }, // Zone 2 range, Fan 2 freq
		{ 0x61, 0x00, 0x8a }, // Zone 3 range, Fan 3 freq
		{ 0x62, 0x00, 0x67 }, // Min/Off, PWM 1 ramp rate
		{ 0x63, 0x00, 0x70 }, // PWM 2, PWM 3 ramp rate
		{ 0x64, 0x00, 0x59 }, // PWM1 Minimum Duty Cycle
		{ 0x65, 0x00, 0x59 }, // PWM2 Minimum Duty Cycle
		{ 0x67, 0x00, 0x47 }, // Zone 1 Low Temp Limit
		{ 0x69, 0x00, 0x47 }, // Zone 3 Low Temp Limit
		{ 0x80, 0x00, 0x07 }, // Interrupt Enable 2
		{ 0x40, 0xfe, 0x01 }, // START
	};

	struct device * const dev = dev_find_slot_on_smbus(2, 0x2d);
	if (dev == NULL) {
		printk(BIOS_WARNING, "EMC6D103 not found\n");
		return;
	}

	printk(BIOS_SPEW, "%s EMC6D103 id: %x %x\n", __func__, smbus_read_byte(dev, 0x3e), smbus_read_byte(dev, 0x3f));

	for (i = 0; i < ARRAY_SIZE(script); i++) {
		reg = smbus_read_byte(dev, script[i].idx);
		if (reg < 0)
			goto fail;
		reg &= script[i].msk;
		reg |= script[i].set;
		reg = smbus_write_byte(dev, script[i].idx, reg & 0xff);
		if (reg < 0)
			goto fail;
	}

	return;

fail:
	printk(BIOS_WARNING, "failed to initialize EMC6D103\n");
}

/* set up DME1737 runtime registers for FAN/PWM 5/6 */
static void dme1737_runtime_init(void)
{
	size_t i;
	uint8_t reg;

	static const struct { uint8_t idx; uint8_t msk; uint8_t set; } rttab[] = {
		{ 0x43, 0xf3, 0x08 },
		{ 0x44, 0xf0, 0x08 },
		{ 0x45, 0xf3, 0x08 },
		{ 0x46, 0xf0, 0x08 },
	};

	/* find DME1737 runtime device (LDN 10) */
	struct device * const dev = dev_find_slot_pnp(0x2e, DME1737_RT);
	if (dev == NULL)
		return;

	struct resource * const res = find_resource(dev, PNP_IDX_IO0);
	if (res == NULL)
		return;

	for (i = 0; i < ARRAY_SIZE(rttab); i++) {
		reg = inb(res->base + rttab[i].idx);
		reg &= rttab[i].msk;
		reg |= rttab[i].set;
		outb(reg, res->base + rttab[i].idx);
	}
}

static void dme1737_hwm_init(void)
{
	size_t i;
	int reg;

	static const struct { uint8_t idx; uint8_t msk; uint8_t set; } script[] = {
		//{ 0x7f, 0x7f, 0x80 }, // INIT
		{ 0x4f, 0x00, 0x32 }, // High
		{ 0x54, 0x00, 0x30 }, // Tach0 Minimum LSB
		{ 0x55, 0x00, 0x2a }, // Tach0 Minimum MSB
		{ 0x56, 0x00, 0x30 }, // Tach1 Minimum LSB
		{ 0x57, 0x00, 0x2a }, // Tach1 Minimum MSB
		{ 0x5a, 0x00, 0x30 }, // Tach3 Minimum LSB
		{ 0x5b, 0x00, 0x2a }, // Tach3 Minimum MSB
		{ 0x5d, 0x00, 0x07 }, // PWM 1 Config: Zone 0
		{ 0x5f, 0x0f, 0x50 }, // Zone 0 range, PWM freq
		{ 0x62, 0x00, 0x67 }, // Ramp Rate
		{ 0x63, 0x00, 0x80 }, // Ramp Rate
		{ 0x65, 0x00, 0x0d }, // PWM 1 Minimum
		{ 0x67, 0x00, 0x23 }, // Zone 0 Low
		{ 0x6a, 0x00, 0x32 }, // Zone 0 High
		{ 0x6c, 0x00, 0x5a }, // Zone 2 Abs
		{ 0x80, 0x00, 0x17 }, // Interrupt Enable 2?
		{ 0xa5, 0x00, 0x40 }, // PMW 4: 25% duty
		{ 0xa6, 0x00, 0x40 }, // PWM 5: 25% duty
		{ 0x40, 0xfe, 0x01 }, // START
	};

	struct device * const dev = dev_find_slot_on_smbus(2, 0x2e);
	if (dev == NULL) {
		printk(BIOS_INFO, "SMBus DME1737 not found\n");
		return;
	}

	printk(BIOS_SPEW, "%s DME1737 id: %x %x\n", __func__, smbus_read_byte(dev, 0x3e), smbus_read_byte(dev, 0x3f));

	for (i = 0; i < ARRAY_SIZE(script); i++) {
		reg = smbus_read_byte(dev, script[i].idx);
		if (reg < 0)
			goto fail;
		reg &= script[i].msk;
		reg |= script[i].set;
		reg = smbus_write_byte(dev, script[i].idx, reg & 0xff);
		if (reg < 0)
			goto fail;
	}

	return;

fail:
	printk(BIOS_WARNING, "failed to initialize EMC6D103\n");
}

static void mainboard_init(struct device *dev)
{
	emc6d103_init();
	dme1737_runtime_init();
	dme1737_hwm_init();

	printk(BIOS_DEBUG, "%s done\n", __func__);
}

static void mainboard_enable(struct device *dev)
{
	dev->ops->init = mainboard_init;
}

struct chip_operations mainboard_ops = {
	.enable_dev = mainboard_enable,
};
