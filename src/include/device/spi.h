/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef __DEVICE_SPI_H__
#define __DEVICE_SPI_H__

struct device;
struct spi_bus_operations {
	/*
	 * This is a SoC-specific method that can be provided to translate the
	 * 'struct device' for a SPI controller into a unique SPI bus
	 * number. Returns -1 if the bus number for this bus cannot be
	 * determined.
	 */
	int (*dev_to_bus)(struct device *dev);
};

#endif	/* __DEVICE_SPI_H__ */
