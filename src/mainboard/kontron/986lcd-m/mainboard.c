/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2007-2009 coresystems GmbH
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

#include <types.h>
#include <device/device.h>
#include <console/console.h>
#include <drivers/intel/gma/int15.h>
#include <pc80/mc146818rtc.h>
#include <arch/io.h>
#include <arch/interrupt.h>

/* Hardware Monitor */

static u16 hwm_base = 0xa00;

static void hwm_write(u8 reg, u8 value)
{
	outb(reg, hwm_base + 0x05);
	outb(value, hwm_base + 0x06);
}

static void hwm_bank(u8 bank)
{
	hwm_write(0x4e, bank);
}

#define FAN_CRUISE_CONTROL_DISABLED	0
#define FAN_CRUISE_CONTROL_SPEED	1
#define FAN_CRUISE_CONTROL_THERMAL	2

#define FAN_SPEED_5625	0


struct fan_speed {
	u8 fan_in;
	u16 fan_speed;
};

/* FANIN Target Speed Register */
/* FANIN = 337500 / RPM */
struct fan_speed fan_speeds[] = {
	{ 0x3c, 5625 }, { 0x41, 5192 }, { 0x47, 4753 }, { 0x4e, 4326 },
	{ 0x56, 3924 }, { 0x5f, 3552 }, { 0x69, 3214 }, { 0x74, 2909 },
	{ 0x80, 2636 }, { 0x8d, 2393 }, { 0x9b, 2177 }, { 0xaa, 1985 },
	{ 0xba, 1814 }, { 0xcb, 1662 }, { 0xdd, 1527 }, { 0xf0, 1406 }
};

struct temperature {
	u8 deg_celsius;
	u8 deg_fahrenheit;
};

struct temperature temperatures[] = {
	{ 30,  86 }, { 33,  91 }, { 36,  96 }, { 39, 102 },
	{ 42, 107 }, { 45, 113 }, { 48, 118 }, { 51, 123 },
	{ 54, 129 }, { 57, 134 }, { 60, 140 }, { 63, 145 },
	{ 66, 150 }, { 69, 156 }, { 72, 161 }, { 75, 167 }
};

static void hwm_setup(void)
{
	int cpufan_control = 0, sysfan_control = 0;
	int cpufan_speed = 0, sysfan_speed = 0;
	int cpufan_temperature = 0, sysfan_temperature = 0;

	cpufan_control = FAN_CRUISE_CONTROL_DISABLED;
	get_option(&cpufan_control, "cpufan_cruise_control");
	cpufan_speed = FAN_SPEED_5625;
	get_option(&cpufan_speed, "cpufan_speed");

	sysfan_control = FAN_CRUISE_CONTROL_DISABLED;
	get_option(&sysfan_control, "sysfan_cruise_control");
	sysfan_speed = FAN_SPEED_5625;
	get_option(&sysfan_speed, "sysfan_speed");

	hwm_bank(0);
	hwm_write(0x59, 0x20); /* Diode Selection */
	hwm_write(0x5d, 0x0f); /* All Sensors Diode, not Thermistor */

	hwm_bank(4);
	hwm_write(0x54, 0xf1); /* SYSTIN temperature offset */
	hwm_write(0x55, 0x19); /* CPUTIN temperature offset */
	hwm_write(0x56, 0xfc); /* AUXTIN temperature offset */

	hwm_bank(0x80); /* Default */

	u8 fan_config = 0;
	/* 00 FANOUT is Manual Mode */
	/* 01 FANOUT is Thermal Cruise Mode */
	/* 10 FANOUT is Fan Speed Cruise Mode */
	switch (cpufan_control) {
	case FAN_CRUISE_CONTROL_SPEED:	 fan_config |= (2 << 4); break;
	case FAN_CRUISE_CONTROL_THERMAL: fan_config |= (1 << 4); break;
	}
	switch (sysfan_control) {
	case FAN_CRUISE_CONTROL_SPEED:	 fan_config |= (2 << 2); break;
	case FAN_CRUISE_CONTROL_THERMAL: fan_config |= (1 << 2); break;
	}
	/* This register must be written first */
	hwm_write(0x04, fan_config);

	switch (cpufan_control) {
	case FAN_CRUISE_CONTROL_SPEED:
		printk(BIOS_DEBUG, "Fan Cruise Control setting CPU fan to %d RPM\n",
				fan_speeds[cpufan_speed].fan_speed);
		hwm_write(0x06, fan_speeds[cpufan_speed].fan_in);  /* CPUFANIN target speed */
		break;
	case FAN_CRUISE_CONTROL_THERMAL:
		printk(BIOS_DEBUG, "Fan Cruise Control setting CPU fan to activation at %d deg C/%d deg F\n",
				temperatures[cpufan_temperature].deg_celsius,
				temperatures[cpufan_temperature].deg_fahrenheit);
		hwm_write(0x06, temperatures[cpufan_temperature].deg_celsius);  /* CPUFANIN target temperature */
		break;
	}

	switch (sysfan_control) {
	case FAN_CRUISE_CONTROL_SPEED:
		printk(BIOS_DEBUG, "Fan Cruise Control setting system fan to %d RPM\n",
				fan_speeds[sysfan_speed].fan_speed);
		hwm_write(0x05, fan_speeds[sysfan_speed].fan_in);  /* SYSFANIN target speed */
		break;
	case FAN_CRUISE_CONTROL_THERMAL:
		printk(BIOS_DEBUG, "Fan Cruise Control setting system fan to activation at %d deg C/%d deg F\n",
				temperatures[sysfan_temperature].deg_celsius,
				temperatures[sysfan_temperature].deg_fahrenheit);
		hwm_write(0x05, temperatures[sysfan_temperature].deg_celsius); /* SYSFANIN target temperature */
		break;
	}

	hwm_write(0x0e, 0x02); /* Fan Output Step Down Time */
	hwm_write(0x0f, 0x02); /* Fan Output Step Up Time */

	hwm_write(0x47, 0xaf); /* FAN divisor register */
	hwm_write(0x4b, 0x84); /* AUXFANIN speed divisor */

	hwm_write(0x40, 0x01); /* Init, but no SMI# */
}

/* mainboard_enable is executed as first thing after */
/* enumerate_buses(). */

static void mainboard_enable(struct device *dev)
{
	install_intel_vga_int15_handler(GMA_INT15_ACTIVE_LFP_INT_LVDS, GMA_INT15_PANEL_FIT_DEFAULT, GMA_INT15_BOOT_DISPLAY_DEFAULT, 3);
	hwm_setup();
}

struct chip_operations mainboard_ops = {
	.enable_dev = mainboard_enable,
};
