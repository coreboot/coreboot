/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef __COREBOOT_SRC_SOC_ROCKCHIP_COMMON_INCLUDE_SOC_I2C_H
#define __COREBOOT_SRC_SOC_ROCKCHIP_COMMON_INCLUDE_SOC_I2C_H

void i2c_init(unsigned int bus, unsigned int hz);
void software_i2c_attach(unsigned int bus);
void software_i2c_detach(unsigned int bus);

#endif  /* ! __COREBOOT_SRC_SOC_ROCKCHIP_COMMON_INCLUDE_SOC_I2C_H */
