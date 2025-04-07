/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <console/console.h>
#include <device/device.h>
#include <device/pnp.h>
#include "fintek_internal.h"
#include "chip.h"
#include "f81966d.h"

/*
 * Set the values for the register using the mask
 * The mask has the writable bits high so the write the bits [3:2] & [0] the mask would be
 * 0xd = 0b00001101.
 */
void set_reg(struct device *dev, u8 port, u8 mask, u8 val)
{
	u8 tmp = pnp_read_config(dev, port);
	pnp_write_config(dev, port, (tmp & ~mask) | (val & mask));
}

/*
 * Select a bank as indicated by the spec.
 * Write the BANKID and 0 registers in the same call.
 */
u8 select_bank(struct device *dev, u8 bank)
{
	if (bank > 3) {
		printk(BIOS_WARNING, "%s: GPIO Bank not in range.\n", __func__);
		return -1;
	}
	/* write 0x27[3:2] = BANKID, 0x27[0] = 0 */
	set_reg(dev, PORT_SELECT_REGISTER, 0x0D, (bank << 2));
	return 0;
}

/*
 * F81966D_gpio_init enables all necessary registers for GPIO
 * Fintek needs to know if pins are used as GPIO or UART pins
 * Share interrupt usage needs to be enabled
 */
void f81966d_gpio_init(struct device *dev)
{
	struct superio_fintek_f81966d_config *conf = dev->chip_info;
	struct resource *res = probe_resource(dev, PNP_IDX_IO0);
	u8 tmp;

	if (!res) {
		printk(BIOS_WARNING, "%s: No GPIO resource found.\n", __func__);
		return;
	}

	pnp_enter_conf_mode(dev);

	if (conf->use_gpio0) {
		select_bank(dev, 0);
		set_reg(dev, MULTI_FUNC_SEL5_REG, 0x1F, 0x1F);
		set_reg(dev, MULTI_FUNC_SEL1_REG, 0x0C, 0x00);

		select_bank(dev, 1);
		set_reg(dev, MULTI_FUNC_SEL5_REG, 0x44, 0x0);

		set_reg(dev, GPIO0X_OUTPUT_ENABLE_REGISTER, 0xFF, conf->gpio0_enable);
		set_reg(dev, GPIO0X_OUTPUT_DATA_REGISTER, 0xFF, conf->gpio0_data);
		set_reg(dev, GPIO0X_DRIVE_ENABLE_REGISTER, 0xFF, conf->gpio0_drive);
	}

	if (conf->use_gpio1) {
		select_bank(dev, 0);
		set_reg(dev, GPIO_FUNC_SEL_REG, 0x77, 0x000);
		set_reg(dev, MULTI_FUNC_SEL1_REG, 0x8C, 0x00);
		set_reg(dev, MULTI_FUNC_SEL5_REG, 0x10, 0x0);

		select_bank(dev, 1);
		set_reg(dev, MULTI_FUNC_SEL5_REG, 0x21, 0x0);
		set_reg(dev, MULTI_FUNC_SEL1_REG, 0x0C, 0x00);
		set_reg(dev, GPIO_FUNC_SEL_REG, 0x77, 0x000);

		select_bank(dev, 2);
		set_reg(dev, GPIO_FUNC_SEL_REG, 0x77, 0x000);

		select_bank(dev, 3);
		set_reg(dev, GPIO_FUNC_SEL_REG, 0x77, 0x000);

		set_reg(dev, GPIO1X_OUTPUT_ENABLE_REGISTER, 0xFF, conf->gpio1_enable);
		set_reg(dev, GPIO1X_OUTPUT_DATA_REGISTER, 0xFF, conf->gpio1_data);
		set_reg(dev, GPIO1X_DRIVE_ENABLE_REGISTER, 0xFF, conf->gpio1_drive);
	}

	if (conf->use_gpio2) {
		select_bank(dev, 2);
		set_reg(dev, MULTI_FUNC_SEL5_REG, 0xFF, 0xFF);
		select_bank(dev, 1);
		set_reg(dev, MULTI_FUNC_SEL5_REG, 0x02, 0x0);

		set_reg(dev, GPIO2X_OUTPUT_ENABLE_REGISTER, 0xFF, conf->gpio2_enable);
		set_reg(dev, GPIO2X_OUTPUT_DATA_REGISTER, 0xFF, conf->gpio2_data);
	}

	if (conf->use_gpio3) {
		select_bank(dev, 0);
		set_reg(dev, MULTI_FUNC_SEL3_REG, 0x30, 0x00);

		set_reg(dev, GPIO3X_OUTPUT_ENABLE_REGISTER, 0xFF, conf->gpio3_enable);
		set_reg(dev, GPIO3X_OUTPUT_DATA_REGISTER, 0xFF, conf->gpio3_data);
		set_reg(dev, GPIO3X_DRIVE_ENABLE_REGISTER, 0xFF, conf->gpio3_drive);
	}

	if (conf->use_gpio4) {
		select_bank(dev, 0);
		set_reg(dev, MULTI_FUNC_SEL3_REG, 0xC0, 0x00);

		set_reg(dev, GPIO4X_OUTPUT_ENABLE_REGISTER, 0xFF, conf->gpio4_enable);
		set_reg(dev, GPIO4X_OUTPUT_DATA_REGISTER, 0xFF, conf->gpio4_data);
		set_reg(dev, GPIO4X_DRIVE_ENABLE_REGISTER, 0xFF, conf->gpio4_drive);
	}

	if (conf->use_gpio5) {
		set_reg(dev, PORT_SELECT_REGISTER, 0x02, 0x00);

		select_bank(dev, 0);
		set_reg(dev, MULTI_FUNC_SEL1_REG, 0x03, 0x00);
		select_bank(dev, 1);
		set_reg(dev, MULTI_FUNC_SEL1_REG, 0x10, 0x00);

		/* set in/output pins */
		set_reg(dev, GPIO5X_OUTPUT_ENABLE_REGISTER, 0xFF, conf->gpio5_enable);
		set_reg(dev, GPIO5X_OUTPUT_DATA_REGISTER, 0xFF, conf->gpio5_data);
		set_reg(dev, GPIO5X_DRIVE_ENABLE_REGISTER, 0xFF, conf->gpio5_drive);
	}

	if (conf->use_gpio6) {
		select_bank(dev, 0);
		set_reg(dev, MULTI_FUNC_SEL1_REG, 0x0C, 0x00);
		set_reg(dev, MULTI_FUNC_SEL4_REG, 0xE0, 0xE0);

		set_reg(dev, GPIO6X_OUTPUT_ENABLE_REGISTER, 0xFF, conf->gpio6_enable);
		set_reg(dev, GPIO6X_OUTPUT_DATA_REGISTER, 0xFF, conf->gpio6_data);
		set_reg(dev, GPIO6X_DRIVE_ENABLE_REGISTER, 0xFF, conf->gpio6_drive);
	}

	if (conf->use_gpio7) {
		select_bank(dev, 0);
		set_reg(dev, MULTI_FUNC_SEL1_REG, 0x20, 0x20);
		set_reg(dev, MULTI_FUNC_SEL4_REG, 0x01, 0x00);

		set_reg(dev, GPIO7X_OUTPUT_ENABLE_REGISTER, 0xFF, conf->gpio7_enable);
		set_reg(dev, GPIO7X_OUTPUT_DATA_REGISTER, 0xFF, conf->gpio7_data);
		set_reg(dev, GPIO7X_DRIVE_ENABLE_REGISTER, 0xFF, conf->gpio7_drive);
	}

	if (conf->use_gpio8) {
		select_bank(dev, 0);
		set_reg(dev, MULTI_FUNC_SEL1_REG, 0x60, 0x20);

		set_reg(dev, GPIO8X_OUTPUT_ENABLE_REGISTER, 0xFF, conf->gpio8_enable);
		set_reg(dev, GPIO8X_OUTPUT_DATA_REGISTER, 0xFF, conf->gpio8_data);
		set_reg(dev, GPIO8X_DRIVE_ENABLE_REGISTER, 0xFF, conf->gpio8_drive);
	}

	if (conf->use_gpio9) {
		select_bank(dev, 0);
		set_reg(dev, MULTI_FUNC_SEL4_REG, 0x02, 0x00);
		select_bank(dev, 1);
		set_reg(dev, MULTI_FUNC_SEL5_REG, 0x80, 0x00);
		select_bank(dev, 3);
		set_reg(dev, MULTI_FUNC_SEL4_REG, 0xFF, 0xFF);

		set_reg(dev, GPIO9X_OUTPUT_ENABLE_REGISTER, 0xFF, conf->gpio8_enable);
		set_reg(dev, GPIO9X_OUTPUT_DATA_REGISTER, 0xFF, conf->gpio8_data);
		set_reg(dev, GPIO9X_DRIVE_ENABLE_REGISTER, 0xFF, conf->gpio8_drive);
	}

	/* Select GPIO in LDN register */
	pnp_write_config(dev, LDN_REG, dev->path.pnp.device & 0xFF);
	/* Set IRQ trigger mode from active low to high (Bit 3) */
	tmp = pnp_read_config(dev, FIFO_SEL_MODE);
	pnp_write_config(dev, FIFO_SEL_MODE, tmp | 0x8);
	/* Enable share interrupt (Bit 0) */
	pnp_write_config(dev, IRQ_SHARE_REGISTER, 0x01);

	pnp_exit_conf_mode(dev);
}
