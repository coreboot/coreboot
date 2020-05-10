/* SPDX-License-Identifier: GPL-2.0-only */

#include <types.h>
#include <console/console.h>
#include <device/device.h>
#include <option.h>
#include <superio/hwm5_conf.h>
#include <superio/nuvoton/common/hwm.h>

#include "superio_hwm.h"

/* Hardware Monitor */

#define FAN_CRUISE_CONTROL_DISABLED	0
#define FAN_CRUISE_CONTROL_SPEED	1
#define FAN_CRUISE_CONTROL_THERMAL	2

#define FAN_SPEED_5625	0
//#define FAN_TEMPERATURE_30DEGC	0

#define HWM_BASE 0x290

struct fan_speed {
	u8 fan_in;
	u16 fan_speed;
};

// FANIN Target Speed Register
// FANIN = 337500 / RPM
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

void hwm_setup(void)
{
	int cpufan_control = 0, sysfan_control = 0;
	int cpufan_speed = 0, sysfan_speed = 0;
	int cpufan_temperature = 0, sysfan_temperature = 0;

	cpufan_control = FAN_CRUISE_CONTROL_DISABLED;
	get_option(&cpufan_control, "cpufan_cruise_control");
	cpufan_speed = FAN_SPEED_5625;
	get_option(&cpufan_speed, "cpufan_speed");
	//cpufan_temperature = FAN_TEMPERATURE_30DEGC;
	//get_option(&cpufan_temperature, "cpufan_temperature");

	sysfan_control = FAN_CRUISE_CONTROL_DISABLED;
	get_option(&sysfan_control, "sysfan_cruise_control");
	sysfan_speed = FAN_SPEED_5625;
	get_option(&sysfan_speed, "sysfan_speed");
	//sysfan_temperature = FAN_TEMPERATURE_30DEGC;
	//get_option(&sysfan_temperature, "sysfan_temperature");

	// pnp_write_hwm5_index(HWM_BASE, 0x31, 0x20); // AVCC high limit
	// pnp_write_hwm5_index(HWM_BASE, 0x34, 0x06); // VIN2 low limit

	nuvoton_hwm_select_bank(HWM_BASE, 0);
	pnp_write_hwm5_index(HWM_BASE, 0x59, 0x20); // Diode Selection
	pnp_write_hwm5_index(HWM_BASE, 0x5d, 0x0f); // All Sensors Diode, not Thermistor

	nuvoton_hwm_select_bank(HWM_BASE, 4);
	pnp_write_hwm5_index(HWM_BASE, 0x54, 0xf1); // SYSTIN temperature offset
	pnp_write_hwm5_index(HWM_BASE, 0x55, 0x19); // CPUTIN temperature offset
	pnp_write_hwm5_index(HWM_BASE, 0x56, 0xfc); // AUXTIN temperature offset

	nuvoton_hwm_select_bank(HWM_BASE, 0x80); // Default

	u8 fan_config = 0;
	// 00 FANOUT is Manual Mode
	// 01 FANOUT is Thermal Cruise Mode
	// 10 FANOUT is Fan Speed Cruise Mode
	switch (cpufan_control) {
	case FAN_CRUISE_CONTROL_SPEED:	 fan_config |= (2 << 4); break;
	case FAN_CRUISE_CONTROL_THERMAL: fan_config |= (1 << 4); break;
	}
	switch (sysfan_control) {
	case FAN_CRUISE_CONTROL_SPEED:	 fan_config |= (2 << 2); break;
	case FAN_CRUISE_CONTROL_THERMAL: fan_config |= (1 << 2); break;
	}
	// This register must be written first
	pnp_write_hwm5_index(HWM_BASE, 0x04, fan_config);

	switch (cpufan_control) {
	case FAN_CRUISE_CONTROL_SPEED: /* CPUFANIN target speed */
		printk(BIOS_DEBUG, "Fan Cruise Control setting CPU fan to %d RPM\n",
				fan_speeds[cpufan_speed].fan_speed);
		pnp_write_hwm5_index(HWM_BASE, 0x06, fan_speeds[cpufan_speed].fan_in);
		break;
	case FAN_CRUISE_CONTROL_THERMAL: /* CPUFANIN target temperature */
		printk(BIOS_DEBUG, "Fan Cruise Control setting CPU fan to activation at %d deg C/%d deg F\n",
				temperatures[cpufan_temperature].deg_celsius,
				temperatures[cpufan_temperature].deg_fahrenheit);
		pnp_write_hwm5_index(HWM_BASE, 0x06,
				     temperatures[cpufan_temperature].deg_celsius);
		break;
	}

	switch (sysfan_control) {
	case FAN_CRUISE_CONTROL_SPEED: /* SYSFANIN target speed */
		printk(BIOS_DEBUG, "Fan Cruise Control setting system fan to %d RPM\n",
				fan_speeds[sysfan_speed].fan_speed);
		pnp_write_hwm5_index(HWM_BASE, 0x05, fan_speeds[sysfan_speed].fan_in);
		break;
	case FAN_CRUISE_CONTROL_THERMAL: /* SYSFANIN target temperature */
		printk(BIOS_DEBUG, "Fan Cruise Control setting system fan to activation at %d deg C/%d deg F\n",
				temperatures[sysfan_temperature].deg_celsius,
				temperatures[sysfan_temperature].deg_fahrenheit);
		pnp_write_hwm5_index(HWM_BASE,
				     0x05, temperatures[sysfan_temperature].deg_celsius);
		break;
	}

	pnp_write_hwm5_index(HWM_BASE, 0x0e, 0x02); // Fan Output Step Down Time
	pnp_write_hwm5_index(HWM_BASE, 0x0f, 0x02); // Fan Output Step Up Time

	pnp_write_hwm5_index(HWM_BASE, 0x47, 0xaf); // FAN divisor register
	pnp_write_hwm5_index(HWM_BASE, 0x4b, 0x84); // AUXFANIN speed divisor

	pnp_write_hwm5_index(HWM_BASE, 0x40, 0x01); // Init, but no SMI#
}
