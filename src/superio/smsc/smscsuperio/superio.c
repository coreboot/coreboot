/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2007 Uwe Hermann <uwe@hermann-uwe.de>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
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

/*
 * Generic driver for pretty much all known Standard Microsystems Corporation
 * (SMSC) Super I/O chips.
 *
 * Datasheets are available from: http://www.smsc.com/main/datasheet.html
 *
 * Most of the SMSC Super I/O chips seem to be similar enough (for our
 * purposes) so that we can handle them with a unified driver.
 *
 * So far only the ASUS A8000 has been tested on real hardware!
 *
 * The floppy disk controller, the parallel port, the serial ports, and the
 * keyboard controller should work with all the chips. For the more advanced
 * stuff (e.g. HWM, ACPI, SMBus) more work is probably required.
 */

#include <arch/io.h>
#include <device/device.h>
#include <device/pnp.h>
#include <console/console.h>
#include <uart8250.h>
#include <pc80/keyboard.h>
#include <stdlib.h>
#include "chip.h"

/* The following Super I/O chips are currently supported by this driver: */
#define LPC47M172	0x14
#define FDC37B80X	0x42	/* Same ID: FDC37M70X (a.k.a. FDC37M707) */
#define FDC37B78X	0x44
#define FDC37B72X	0x4c
#define FDC37M81X	0x4d
#define FDC37M60X	0x47
#define LPC47B27X	0x51	/* a.k.a. LPC47B272 */
#define LPC47U33X	0x54
#define LPC47M10X	0x59	/* Same ID: LPC47M112, LPC47M13X */
#define LPC47M15X	0x60	/* Same ID: LPC47M192 */
#define LPC47S45X	0x62
#define LPC47B397	0x6f
#define A8000		0x77	/* ASUS A8000, a rebranded DME1737(?) */
#define DME1737		0x78
#define SCH3112		0x7c
#define SCH3114		0x7d
#define SCH5307		0x81	/* Rebranded LPC47B397(?) */
#define SCH5027D	0x89
#define SCH4304		0x90	/* SCH4304, SCH4307 */

/* Register defines */
#define DEVICE_ID_REG	0x20	/* Device ID register */
#define DEVICE_REV_REG	0x21	/* Device revision register */
#define DEVICE_TEST7_REG 0x29   /* Device test 7 register */

/* Static variables for the Super I/O device ID and revision. */
static int first_time = 1;
static u8 superio_id = 0;
static u8 superio_rev = 0;

/**
 * A list of all possible logical devices which may be supported by at least
 * one of the Super I/O chips. These values are used as index into the
 * logical_device_table[i].devs array(s).
 *
 * If you change this enum, you must also adapt the logical_device_table[]
 * array and MAX_LOGICAL_DEVICES!
 */
enum {
	LD_FDC,		/* Floppy disk controller */
	LD_PP,		/* Parallel port */
	LD_SP1,		/* Serial port 1 (COM1) */
	LD_SP2,		/* Serial port 2 (COM2) */
	LD_RTC,		/* Real-time clock */
	LD_KBC,		/* Keyboard controller */
	LD_AUX,		/* Auxiliary I/O */
	LD_XBUS,	/* X-Bus */
	LD_HWM,		/* Hardware monitor */
	LD_GAME,	/* Game port */
	LD_PME,		/* Power management events */
	LD_MPU401,	/* MPU-401 MIDI UART */
	LD_RT,		/* Runtime registers / security key registers */
	LD_ACPI,	/* ACPI */
	LD_SMB,		/* SMBus */
};

/* Note: This value must match the number of items in the enum above! */
#define MAX_LOGICAL_DEVICES 15

/**
 * A table describing the logical devices which are present on the
 * supported Super I/O chips.
 *
 * The first entry (superio_id) is the device ID of the Super I/O chip
 * as stored in the (read-only) DEVICE_ID_REG register.
 *
 * The second entry (devs) is the list of logical device IDs which are
 * present on that particular Super I/O chip. A value of -1 means the
 * device is not present on that chip.
 *
 * Note: Do _not_ list chips with different name but same device ID twice!
 *       The result would be that the init code would be executed twice!
 */
static const struct logical_devices {
	u8 superio_id;
	int devs[MAX_LOGICAL_DEVICES];
} logical_device_table[] = {
	/* Chip   FDC PP SP1 SP2 RTC KBC AUX XBUS HWM GAME PME MPU RT ACPI SMB */
	{LPC47M172,{0, 3, 4,  2, -1,  7, -1,  -1, -1,  -1, -1, -1, 10, -1, -1}},
	{FDC37B80X,{0, 3, 4,  5, -1,  7,  8,  -1, -1,  -1, -1, -1, -1, -1, -1}},
	{FDC37B78X,{0, 3, 4,  5,  6,  7,  8,  -1, -1,  -1, -1, -1, -1, 10, -1}},
	{FDC37B72X,{0, 3, 4,  5, -1,  7,  8,  -1, -1,  -1, -1, -1, -1, 10, -1}},
	{FDC37M81X,{0, 3, 4,  5, -1,  7,  8,  -1, -1,  -1, -1, -1, -1, -1, -1}},
	{FDC37M60X,{0, 3, 4,  5, -1,  7,  8,  -1, -1,  -1, -1, -1, -1, -1, -1}},
	{LPC47B27X,{0, 3, 4,  5, -1,  7, -1,  -1, -1,   9, -1, 11, 10, -1, -1}},
	{LPC47M10X,{0, 3, 4,  5, -1,  7, -1,  -1, -1,   9, 10, 11, -1, -1, -1}},
	{LPC47M15X,{0, 3, 4,  5, -1,  7, -1,  -1, -1,   9, 10, 11, -1, -1, -1}},
	{LPC47S45X,{0, 3, 4,  5,  6,  7, -1,   8, -1,  -1, -1, -1, 10, -1, 11}},
	{LPC47B397,{0, 3, 4,  5, -1,  7, -1,  -1,  8,  -1, -1, -1, 10, -1, -1}},
	{LPC47U33X,{0, 3, 4, -1, -1,  7, -1,  -1, -1,   9,  0,  5, 10,  0, 11}},
	{A8000,    {0, 3, 4,  5, -1,  7, -1,  -1, -1,  -1, -1, -1, 10, -1, -1}},
	{DME1737,  {0, 3, 4,  5, -1,  7, -1,  -1, -1,  -1, -1, -1, 10, -1, -1}},
	{SCH3112,  {0, 3, 4,  5, -1,  7, -1,  -1, -1,  -1, -1, -1, 10, -1, -1}},
	{SCH3114,  {0, 3, 4,  5, -1,  7, -1,  -1, -1,  -1, -1, -1, 10, -1, -1}},
	{SCH5307,  {0, 3, 4,  5, -1,  7, -1,  -1,  8,  -1, -1, -1, 10, -1, -1}},
	{SCH5027D, {0, 3, 4,  5, -1,  7, -1,  -1, -1,  -1, -1, -1, 10, -1, 11}},
	{SCH4304,  {0, 3, 4,  5, -1,  7, -1,  11, -1,  -1, -1, -1, 10, -1, -1}},
};

/**
 * Enter the configuration state by writing 0x55 to the config port.
 *
 * The Super I/O configuration registers can only be modified when the chip
 * is in the configuration state. Thus, to program the registers you have
 * to a) enter config mode, b) program the registers, c) exit config mode.
 *
 * @param dev The device to use.
 */
static void smsc_pnp_enter_conf_state(device_t dev)
{
	outb(0x55, dev->path.pnp.port);
}

/**
 * Exit the configuration state by writing 0xaa to the config port.
 *
 * This puts the chip into the 'run' state again.
 *
 * @param dev The device to use.
 */
static void smsc_pnp_exit_conf_state(device_t dev)
{
	outb(0xaa, dev->path.pnp.port);
}

/** Wrapper for pnp_set_resources(). */
static void smsc_pnp_set_resources(device_t dev)
{
	smsc_pnp_enter_conf_state(dev);
	pnp_set_resources(dev);
	smsc_pnp_exit_conf_state(dev);
}

/** Wrapper for pnp_enable_resources(). */
static void smsc_pnp_enable_resources(device_t dev)
{
	smsc_pnp_enter_conf_state(dev);
	pnp_enable_resources(dev);
	smsc_pnp_exit_conf_state(dev);
}

/**
 * If so configured, enable the specified device, otherwise
 * explicitly disable it.
 *
 * @param dev The device to use.
 */
static void smsc_pnp_enable(device_t dev)
{
	smsc_pnp_enter_conf_state(dev);
	pnp_set_logical_device(dev);
	pnp_set_enable(dev, !!dev->enabled);
	smsc_pnp_exit_conf_state(dev);
}

/**
 * Initialize those logical devices which need a special init.
 *
 * @param dev The device to use.
 */
static void smsc_init(device_t dev)
{
	struct superio_smsc_smscsuperio_config *conf = dev->chip_info;
	int i, ld;

	/* Do not initialize disabled devices. */
	if (!dev->enabled)
		return;

	/* Find the correct Super I/O. */
	for (i = 0; i < ARRAY_SIZE(logical_device_table); i++)
		if (logical_device_table[i].superio_id == superio_id)
			break;

	/* If no Super I/O was found, return. */
	if (i == ARRAY_SIZE(logical_device_table))
		return;

	/* A Super I/O was found, so initialize the respective device. */
	ld = dev->path.pnp.device;
	if (ld == logical_device_table[i].devs[LD_KBC]) {
		pc_keyboard_init(&conf->keyboard);
	}
}

/** Standard device operations. */
static struct device_operations ops = {
	.read_resources   = pnp_read_resources,
	.set_resources    = smsc_pnp_set_resources,
	.enable_resources = smsc_pnp_enable_resources,
	.enable           = smsc_pnp_enable,
	.init             = smsc_init,
};

/**
 * TODO.
 *
 * This table should contain all possible entries for any of the supported
 * Super I/O chips, even if some of them don't have the respective logical
 * devices. That will be handled correctly by our code.
 *
 * The LD_FOO entries are device markers which tell you the type of the logical
 * device (e.g. whether it's a floppy disk controller or a serial port etc.).
 *
 * Before using pnp_dev_info[] in pnp_enable_devices() these markers have
 * to be replaced with the real logical device IDs of the respective
 * Super I/O chip. This is done in enable_dev().
 *
 * TODO: FDC, PP, SP1, SP2, and KBC should work, the rest probably not (yet).
 */
static struct pnp_info pnp_dev_info[] = {
	{ &ops, LD_FDC, PNP_IO0 | PNP_IRQ0 | PNP_DRQ0, {0x07f8, 0}, },
	{ &ops, LD_PP,  PNP_IO0 | PNP_IRQ0 | PNP_DRQ0, {0x07f8, 0}, },
	{ &ops, LD_SP1, PNP_IO0 | PNP_IRQ0, {0x07f8, 0}, },
	{ &ops, LD_SP2, PNP_IO0 | PNP_IRQ0, {0x07f8, 0}, },
	{ &ops, LD_RTC, },
	{ &ops, LD_KBC, PNP_IO0 | PNP_IO1 | PNP_IRQ0 | PNP_IRQ1, {0x07ff, 0}, {0x07ff, 4}, },
	{ &ops, LD_AUX, },
	{ &ops, LD_XBUS, },
	{ &ops, LD_HWM, PNP_IO0, {0x07f0, 0}, },
	{ &ops, LD_GAME, },
	{ &ops, LD_PME, },
	{ &ops, LD_MPU401, },
	{ &ops, LD_RT,  PNP_IO0, {0x0780, 0}, },
	{ &ops, LD_ACPI, },
	{ &ops, LD_SMB, },
};

/**
 * Enable the logical devices of the Super I/O chip.
 *
 * TODO: Think about how to handle the case when a mainboard has multiple
 *       Super I/O chips soldered on.
 * TODO: Can this code be simplified a bit?
 *
 * @param dev The device to use.
 */
static void enable_dev(device_t dev)
{
	int i, j, fn;
	int tmp[MAX_LOGICAL_DEVICES];
	u8 test7;

	if (first_time) {
		/* Read the device ID and revision of the Super I/O chip. */
		smsc_pnp_enter_conf_state(dev);
		superio_id = pnp_read_config(dev, DEVICE_ID_REG);
		superio_rev = pnp_read_config(dev, DEVICE_REV_REG);
		smsc_pnp_exit_conf_state(dev);

		/* TODO: Error handling? */

		printk(BIOS_INFO, "Found SMSC Super I/O (ID=0x%02x, "
		       "rev=0x%02x)\n", superio_id, superio_rev);
		first_time = 0;

		if (superio_id == LPC47M172) {
			/*
			 * Do not use the default logical device number but
			 * instead the standard SMSC registers set.
			 */

			/*
			 * TEST7 configuration register (0x29)
			 * Bit 0: LD_NUM (0 = new, 1 = std SMSC)
			 */
			test7 = pnp_read_config(dev, DEVICE_TEST7_REG);
			test7 |= (1 << 0);
			pnp_write_config(dev, DEVICE_TEST7_REG, test7);
		}
	}

	/* Find the correct Super I/O. */
	for (i = 0; i < ARRAY_SIZE(logical_device_table); i++)
		if (logical_device_table[i].superio_id == superio_id)
			break;

	/* If no Super I/O was found, return. */
	if (i == ARRAY_SIZE(logical_device_table))
		return;

	/* Temporarily save the LD_FOO values. */
	for (j = 0; j < ARRAY_SIZE(pnp_dev_info); j++)
		tmp[j] = pnp_dev_info[j].function;

	/*
	 * Replace the LD_FOO markers in pnp_dev_info[] with
	 * the real logical device IDs of this Super I/O chip.
	 */
	for (j = 0; j < ARRAY_SIZE(pnp_dev_info); j++) {
		fn = pnp_dev_info[j].function;
		pnp_dev_info[j].function = logical_device_table[i].devs[fn];
	}

	/* Enable the specified devices (if present on the chip). */
	pnp_enable_devices(dev, &pnp_ops, ARRAY_SIZE(pnp_dev_info),
			   &pnp_dev_info[0]);

	/* Restore LD_FOO values. */
	for (j = 0; j < ARRAY_SIZE(pnp_dev_info); j++)
		pnp_dev_info[j].function = tmp[j];
}

struct chip_operations superio_smsc_smscsuperio_ops = {
	CHIP_NAME("Various SMSC Super I/Os")
	.enable_dev = enable_dev
};
