/* SPDX-License-Identifier: GPL-2.0-only */

#include <console/console.h>
#include <delay.h>
#include <device/i2c_simple.h>
#include <gpio.h>
#include <soc/i2c.h>
#include <soc/tps65132s.h>

static int tps65132s_reg_mask(unsigned int bus, uint8_t chip, uint8_t addr,
			      uint8_t val, uint8_t mask)
{
	uint8_t msg = 0;

	if (i2c_read_field(bus, chip, addr, &msg, 0xFF, 0) < 0) {
		printk(BIOS_ERR, "%s: Failed to read i2c(%u): addr(%u)\n",
			__func__, bus, addr);
		return -1;
	}

	msg &= ~mask;
	msg |= val;

	return i2c_write_field(bus, chip, addr, msg, 0xFF, 0);
}

enum cb_err tps65132s_setup(const struct tps65132s_cfg *cfg)
{
	bool write_to_eeprom = false;
	u8 val;
	int i;

	gpio_output(cfg->en, 1);
	gpio_output(cfg->sync, 1);
	mdelay(10);

	for (i = 0; i < cfg->setting_counts; i++) {
		i2c_read_field(cfg->i2c_bus, PMIC_TPS65132_SLAVE,
			       cfg->settings[i].addr, &val, 0xFF, 0);
		if (val != cfg->settings[i].val) {
			if  (tps65132s_reg_mask(cfg->i2c_bus, PMIC_TPS65132_SLAVE,
						cfg->settings[i].addr,
						cfg->settings[i].val,
						cfg->settings[i].mask) < 0) {
				printk(BIOS_ERR, "Failed to program TPS65132S at %x\n",
				       cfg->settings[i].addr);
				return CB_ERR;
			}
			write_to_eeprom = true;
		}
	}

	if (write_to_eeprom) {
		if (tps65132s_reg_mask(cfg->i2c_bus, PMIC_TPS65132_SLAVE,
					PMIC_TPS65132_CONTROL, 0x80, 0xFC) < 0)
			return CB_ERR;
		printk(BIOS_INFO, "Program TPS65132S EEPROM at first boot\n");
		mdelay(50);
	}

	return CB_SUCCESS;
}
