/*
 * This file is part of the coreboot project.
 *
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

#ifndef __PICASSO_CHIP_H__
#define __PICASSO_CHIP_H__

#include <stddef.h>
#include <stdint.h>
#include <commonlib/helpers.h>
#include <drivers/i2c/designware/dw_i2c.h>
#include <soc/i2c.h>
#include <arch/acpi_device.h>

#define PICASSO_I2C_DEV_MAX 4

struct soc_amd_picasso_config {
	/*
	 * If sb_reset_i2c_slaves() is called, this devicetree register
	 * defines which I2C SCL will be toggled 9 times at 100 KHz.
	 * For example, should we need I2C0 and  I2C3 have their slave
	 * devices reseted by toggling SCL, use:
	 *
	 * register i2c_scl_reset = (GPIO_I2C0_SCL | GPIO_I2C3_SCL)
	 */
	u8 i2c_scl_reset;
	struct dw_i2c_bus_config i2c[PICASSO_I2C_DEV_MAX];
	enum {
		I2S_PINS_MAX_HDA = 0,	/* HDA w/reset  3xSDI, SW w/Data0 */
		I2S_PINS_MAX_MHDA = 1,	/* HDA no reset 3xSDI, SW w/Data0-1 */
		I2S_PINS_MIN_HDA = 2,	/* HDA w/reset  1xSDI, SW w/Data0-2 */
		I2S_PINS_MIN_MHDA = 3,	/* HDA no reset 1xSDI, SW w/Data0-3 */
		I2S_PINS_I2S_TDM = 4,
		I2S_PINS_UNCONF = 7,	/* All pads will be input mode */
	} acp_pin_cfg;
};

typedef struct soc_amd_picasso_config config_t;

extern struct device_operations pci_domain_ops;

#endif /* __PICASSO_CHIP_H__ */
