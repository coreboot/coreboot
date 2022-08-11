/* SPDX-License-Identifier: GPL-2.0-only OR MIT */

#include <console/console.h>
#include <device/i2c_simple.h>
#include <soc/mt6691.h>

#define MT6691_MIN_VOLTAGE		300000
#define MT6691_MAX_VOLTAGE		1300000
#define MT6691_STEP_UV			5000

enum {
	MT6691_SLAVE_ADDR = 0x51,
};

int mt6691_set_voltage(uint8_t i2c_num, unsigned int volt_uv)
{
	uint8_t selector;

	if (volt_uv > MT6691_MAX_VOLTAGE || volt_uv < MT6691_MIN_VOLTAGE) {
		printk(BIOS_ERR, "%s: voltage out of range\n", __func__);
		return -1;
	}

	selector = DIV_ROUND_UP(volt_uv - MT6691_MIN_VOLTAGE, MT6691_STEP_UV);

	return i2c_write_field(i2c_num, MT6691_SLAVE_ADDR,
			       MT6691_VSEL0, selector, 0xFF, 0);
}

int mt6691_get_voltage(uint8_t i2c_num)
{
	uint8_t selector = 0;
	unsigned int volt;

	if (i2c_read_field(i2c_num, MT6691_SLAVE_ADDR, MT6691_VSEL0,
			   &selector, 0xFF, 0) < 0) {
		printk(BIOS_ERR, "%s: failed to get voltage from i2c\n", __func__);
		return -1;
	}

	volt = (selector * MT6691_STEP_UV) + MT6691_MIN_VOLTAGE;

	if (volt > MT6691_MAX_VOLTAGE) {
		printk(BIOS_ERR, "%s: voltage out of range\n", __func__);
		return -1;
	}

	return volt;
}

static uint8_t get_mt6691_chip_id(uint8_t i2c_num)
{
	uint8_t id;

	if (i2c_read_field(i2c_num, MT6691_SLAVE_ADDR, MT6691_MONITOR,
			   &id, 0x1, MT6691_PGOOD_SHIFT) < 0) {
		printk(BIOS_ERR, "%s: failed to read from i2c", __func__);
		return 0;
	}

	return id;
}

void mt6691_probe(uint8_t i2c_num)
{
	/* Check device ID is MT6691 */
	if (!get_mt6691_chip_id(i2c_num)) {
		printk(BIOS_ERR, "unknown MT6691 chip_id\n");
		return;
	}
	/* Slew rate 12mV */
	i2c_write_field(i2c_num, MT6691_SLAVE_ADDR, MT6691_CTRL2, 0x1,
			MT6691_DN_SR_MASK, MT6691_DN_SR_SHIFT);
}
