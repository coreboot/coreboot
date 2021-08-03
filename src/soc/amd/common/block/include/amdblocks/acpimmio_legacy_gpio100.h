/* SPDX-License-Identifier: GPL-2.0-or-later */

#ifndef AMD_BLOCK_ACPIMMIO_LEGACY_GPIO_100_H
#define AMD_BLOCK_ACPIMMIO_LEGACY_GPIO_100_H

#include <amdblocks/acpimmio.h>
#include <device/mmio.h>
#include <types.h>

/* These iomux_read/write8 are to be deprecated to enforce proper
   use of <gpio.h> API for pin configurations. */
static inline uint8_t iomux_read8(uint8_t reg)
{
	return read8(acpimmio_iomux + reg);
}

static inline void iomux_write8(uint8_t reg, uint8_t value)
{
	write8(acpimmio_iomux + reg, value);
}

/* Old GPIO configuration registers */
static inline uint8_t gpio_100_read8(uint8_t reg)
{
	return read8(acpimmio_gpio_100 + reg);
}

static inline void gpio_100_write8(uint8_t reg, uint8_t value)
{
	write8(acpimmio_gpio_100 + reg, value);
}

#endif /* AMD_BLOCK_ACPIMMIO_LEGACY_GPIO_100_H */
