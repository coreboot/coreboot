/* SPDX-License-Identifier: GPL-2.0-only */

#include <string.h>
#include <types.h>
#include <device/device.h>
#include <console/console.h>
#include <drivers/intel/gma/int15.h>
#include <option.h>
#include <stdio.h>
#include <superio/hwm5_conf.h>
#include <superio/nuvoton/common/hwm.h>

/* Hardware Monitor */

static u16 hwm_base = 0xa00;

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
	unsigned int cpufan_control = 0, sysfan_control = 0;
	unsigned int cpufan_speed = 0, sysfan_speed = 0;
	unsigned int cpufan_temperature = 0, sysfan_temperature = 0;

	cpufan_control = get_uint_option("cpufan_cruise_control", FAN_CRUISE_CONTROL_DISABLED);
	cpufan_speed = get_uint_option("cpufan_speed", FAN_SPEED_5625);

	sysfan_control = get_uint_option("sysfan_cruise_control", FAN_CRUISE_CONTROL_DISABLED);
	sysfan_speed = get_uint_option("sysfan_speed", FAN_SPEED_5625);

	nuvoton_hwm_select_bank(hwm_base, 0);
	pnp_write_hwm5_index(hwm_base, 0x59, 0x20); /* Diode Selection */
	pnp_write_hwm5_index(hwm_base, 0x5d, 0x0f); /* All Sensors Diode, not Thermistor */

	nuvoton_hwm_select_bank(hwm_base, 4);
	pnp_write_hwm5_index(hwm_base, 0x54, 0xf1); /* SYSTIN temperature offset */
	pnp_write_hwm5_index(hwm_base, 0x55, 0x19); /* CPUTIN temperature offset */
	pnp_write_hwm5_index(hwm_base, 0x56, 0xfc); /* AUXTIN temperature offset */

	nuvoton_hwm_select_bank(hwm_base, 0x80); /* Default */

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
	pnp_write_hwm5_index(hwm_base, 0x04, fan_config);

	switch (cpufan_control) {
	case FAN_CRUISE_CONTROL_SPEED: /* CPUFANIN target speed */
		printk(BIOS_DEBUG, "Fan Cruise Control setting CPU fan to %d RPM\n",
				fan_speeds[cpufan_speed].fan_speed);
		pnp_write_hwm5_index(hwm_base, 0x06, fan_speeds[cpufan_speed].fan_in);
		break;
	case FAN_CRUISE_CONTROL_THERMAL: /* CPUFANIN target temperature */
		printk(BIOS_DEBUG, "Fan Cruise Control setting CPU fan to activation at %d deg C/%d deg F\n",
				temperatures[cpufan_temperature].deg_celsius,
				temperatures[cpufan_temperature].deg_fahrenheit);
		pnp_write_hwm5_index(hwm_base, 0x06,
				     temperatures[cpufan_temperature].deg_celsius);
		break;
	}

	switch (sysfan_control) {
	case FAN_CRUISE_CONTROL_SPEED: /* SYSFANIN target speed */
		printk(BIOS_DEBUG, "Fan Cruise Control setting system fan to %d RPM\n",
				fan_speeds[sysfan_speed].fan_speed);
		pnp_write_hwm5_index(hwm_base, 0x05, fan_speeds[sysfan_speed].fan_in);
		break;
	case FAN_CRUISE_CONTROL_THERMAL: /* SYSFANIN target temperature */
		printk(BIOS_DEBUG, "Fan Cruise Control setting system fan to activation at %d deg C/%d deg F\n",
				temperatures[sysfan_temperature].deg_celsius,
				temperatures[sysfan_temperature].deg_fahrenheit);
		pnp_write_hwm5_index(hwm_base, 0x05,
				     temperatures[sysfan_temperature].deg_celsius);
		break;
	}

	pnp_write_hwm5_index(hwm_base, 0x0e, 0x02); /* Fan Output Step Down Time */
	pnp_write_hwm5_index(hwm_base, 0x0f, 0x02); /* Fan Output Step Up Time */

	pnp_write_hwm5_index(hwm_base, 0x47, 0xaf); /* FAN divisor register */
	pnp_write_hwm5_index(hwm_base, 0x4b, 0x84); /* AUXFANIN speed divisor */

	pnp_write_hwm5_index(hwm_base, 0x40, 0x01); /* Init, but no SMI# */
}

static void mainboard_enable(struct device *dev)
{
	install_intel_vga_int15_handler(GMA_INT15_ACTIVE_LFP_INT_LVDS, GMA_INT15_PANEL_FIT_DEFAULT, GMA_INT15_BOOT_DISPLAY_DEFAULT, 3);
	hwm_setup();
}

static void mainboard_init(void *chip_info)
{
	int i;
	struct device *dev;

	for (i = 1; i <= 3; i++) {
		char cmos_option_name[] = "ethernetx";
		snprintf(cmos_option_name, sizeof(cmos_option_name),
			 "ethernet%01d", i);
		unsigned int ethernet_disable = get_uint_option(cmos_option_name, 0);
		if (!ethernet_disable)
			continue;
		printk(BIOS_DEBUG, "Disabling Ethernet NIC #%d\n", i);
		dev = pcidev_on_root(28, i - 1);
		if (dev == NULL) {
			printk(BIOS_ERR,
			       "Disabling Ethernet NIC: Cannot find 00:1c.%d!\n",
			       i - 1);
			continue;
		}
		dev->enabled = 0;
	}
}

struct chip_operations mainboard_ops = {
	.init = mainboard_init,
	.enable_dev = mainboard_enable,
};
