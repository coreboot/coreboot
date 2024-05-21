/* SPDX-License-Identifier: GPL-2.0-only */

#include <bootstate.h>
#include <arch/io.h>
#include <cbfs.h>
#include <console/system76_ec.h>
#include <pc80/mc146818rtc.h>
#include <security/vboot/misc.h>
#include <security/vboot/vboot_common.h>
#include <timer.h>
#include "acpi.h"
#include "commands.h"

// This is the command region for Dasharo EC firmware. It must be
// enabled for LPC in the mainboard.
#define DASHARO_EC_BASE 0x0E00
#define DASHARO_EC_SIZE 256

#define SPI_SECTOR_SIZE 1024

#define REG_CMD    0
#define REG_RESULT 1
#define REG_DATA   2 // Start of command data

// When command register is 0, command is complete
#define CMD_FINISHED 0

static inline uint8_t dasharo_ec_read(uint8_t addr)
{
	return inb(DASHARO_EC_BASE + (uint16_t)addr);
}

static inline void dasharo_ec_write(uint8_t addr, uint8_t data)
{
	outb(data, DASHARO_EC_BASE + (uint16_t)addr);
}

uint8_t dasharo_ec_smfi_cmd(uint8_t cmd, uint8_t len, uint8_t *data)
{
	int i;

	if (len > DASHARO_EC_SIZE - REG_DATA)
		return -1;

	// Wait for previous command completion, for up to 10 milliseconds, with a
	// test period of 1 microsecond
	wait_us(10000, dasharo_ec_read(REG_CMD) == CMD_FINISHED);

	// Write data first
	for (i = 0; i < len; ++i)
		dasharo_ec_write(REG_DATA + i, data[i]);

	// Write command register, which starts command
	dasharo_ec_write(REG_CMD, cmd);

	// Wait for previous command completion, for up to 10 milliseconds, with a
	// test period of 1 microsecond
	wait_us(10000, dasharo_ec_read(REG_CMD) == CMD_FINISHED);

	return dasharo_ec_read(REG_RESULT);
}

int dasharo_ec_get_bat_threshold(enum bat_threshold_type type)
{
	int ret = -1;

	switch (type) {
	case BAT_THRESHOLD_START:
		ret = ec_read(DASHARO_EC_REG_BATTERY_START_THRESHOLD);
		break;
	case BAT_THRESHOLD_STOP:
		ret = ec_read(DASHARO_EC_REG_BATTERY_STOP_THRESHOLD);
		break;
	default:
		break;
	}

	return ret;
}

void dasharo_ec_set_bat_threshold(enum bat_threshold_type type, uint8_t value)
{
	switch (type) {
	case BAT_THRESHOLD_START:
		ec_write(DASHARO_EC_REG_BATTERY_START_THRESHOLD, value);
		break;
	case BAT_THRESHOLD_STOP:
		ec_write(DASHARO_EC_REG_BATTERY_STOP_THRESHOLD, value);
		break;
	default:
		break;
	}
}
