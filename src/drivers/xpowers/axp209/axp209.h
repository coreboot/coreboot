/*
 * Definitions for X-Powers AXP 209 Power Management Unit
 *
 * Copyright (C) 2013  Alexandru Gagniuc <mr.nuke.me@gmail.com>
 * Subject to the GNU GPL v2, or (at your option) any later version.
 */

#include <types.h>
#include "chip.h"

#define AXP209_I2C_ADDR (0x68 >> 1)

enum cb_err axp209_init(u8 bus);
enum cb_err axp209_set_dcdc2_voltage(u8 bus, u16 millivolts);
enum cb_err axp209_set_dcdc3_voltage(u8 bus, u16 millivolts);
enum cb_err axp209_set_ldo2_voltage(u8 bus, u16 millivolts);
enum cb_err axp209_set_ldo3_voltage(u8 bus, u16 millivolts);
enum cb_err axp209_set_ldo4_voltage(u8 bus, u16 millivolts);
enum cb_err axp209_set_voltages(u8 bus, const struct
				drivers_xpowers_axp209_config *cfg);
