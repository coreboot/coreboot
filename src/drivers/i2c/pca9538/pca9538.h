/* SPDX-License-Identifier: GPL-2.0-only */
/* This file is part of the coreboot project. */

#ifndef _I2C_PCA9538_H_
#define _I2C_PCA9538_H_

#include <types.h>
#include <device/device.h>

/* Register layout */
#define INPUT_REG		0x00
#define OUTPUT_REG		0x01
#define INPUT_INVERT_REG	0x02
#define IO_CONFIG_REG		0x03

/* Provide some functions to read input and write output values. */
uint8_t pca9538_read_input(void);
void pca9538_set_output(uint8_t val);
/*
 * Provide a way to get the right device structure for the I/O expander.
 * The user of this driver has to provide this function if read/write of I/O
 * values on the I/O expander is needed.
 */
struct device *pca9538_get_dev(void);

#endif /* _I2C_PCA9538_H_ */
