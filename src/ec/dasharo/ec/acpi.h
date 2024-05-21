/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef EC_DASHARO_EC_ACPI_H
#define EC_DASHARO_EC_ACPI_H

#include <ec/acpi/ec.h>

#define DASHARO_EC_REG_LSTE		0x03
#define DASHARO_EC_REG_LSTE_LID_STATE	0x01

#define DASHARO_EC_REG_BATTERY_START_THRESHOLD	0xBC
#define DASHARO_EC_REG_BATTERY_STOP_THRESHOLD	0xBD

int dasharo_ec_get_lid_state(void);

enum bat_threshold_type {
	BAT_THRESHOLD_START,
	BAT_THRESHOLD_STOP
};

int dasharo_ec_get_bat_threshold(enum bat_threshold_type type);
void dasharo_ec_set_bat_threshold(enum bat_threshold_type type, uint8_t value);

#endif /* EC_DASHARO_EC_ACPI_H */
