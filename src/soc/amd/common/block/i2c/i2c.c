/* SPDX-License-Identifier: GPL-2.0-only */

#include <assert.h>
#include <delay.h>
#include <amdblocks/acpimmio.h>
#include <amdblocks/gpio_banks.h>
#include <amdblocks/gpio_defs.h>
#include <amdblocks/i2c.h>

#define MAX_PIN_COUNT 4

struct common_i2c_save {
	uint32_t control_value;
	uint8_t mux_value;
};

/*
 * To program I2C pins without destroying their programming, the registers
 * that will be changed need to be saved first.
 */
static void save_i2c_pin_registers(uint8_t gpio, struct common_i2c_save *save_table)
{
	save_table->mux_value = iomux_read8(gpio);
	save_table->control_value = gpio_read32(gpio);
}

static void restore_i2c_pin_registers(uint8_t gpio, struct common_i2c_save *save_table)
{
	/* Write and flush posted writes. */
	iomux_write8(gpio, save_table->mux_value);
	iomux_read8(gpio);
	gpio_write32(gpio, save_table->control_value);
	gpio_read32(gpio);
}

static void drive_scl(const struct soc_i2c_peripheral_reset_info *reset_info, uint32_t val)
{
	uint8_t j;

	for (j = 0; j < reset_info->num_pins; j++) {
		if (reset_info->i2c_scl_reset_mask & reset_info->i2c_scl[j].pin_mask)
			gpio_write32(reset_info->i2c_scl[j].pin.gpio, val);
	}

	gpio_read32(0); /* Flush posted write */
	/*
	 * TODO(b/183010197): 4usec gets 85KHz for 1 pin, 70KHz for 4 pins. Ensure this delay
	 * works fine for all SoCs and make this delay configurable if required.
	 */
	udelay(4);
}

void sb_reset_i2c_peripherals(const struct soc_i2c_peripheral_reset_info *reset_info)
{
	struct common_i2c_save save_table[MAX_PIN_COUNT];
	uint8_t i;

	if (!reset_info || !reset_info->i2c_scl || !reset_info->num_pins ||
						!reset_info->i2c_scl_reset_mask)
		return;

	assert(reset_info->num_pins <= MAX_PIN_COUNT);

	/* Save and reprogram I2C SCL pins */
	for (i = 0; i < reset_info->num_pins; i++) {
		save_i2c_pin_registers(reset_info->i2c_scl[i].pin.gpio, &save_table[i]);
		program_gpios(&reset_info->i2c_scl[i].pin, 1);
	}

	/*
	 * Toggle SCL back and forth 9 times under 100KHz. A single read is
	 * needed after the writes to force the posted write to complete.
	 */
	for (i = 0; i < 9; i++) {
		drive_scl(reset_info, GPIO_OUTPUT_OUT_HIGH);
		drive_scl(reset_info, GPIO_OUTPUT_OUT_LOW);
	}

	/* Restore I2C pins. */
	for (i = 0; i < reset_info->num_pins; i++)
		restore_i2c_pin_registers(reset_info->i2c_scl[i].pin.gpio, &save_table[i]);
}
