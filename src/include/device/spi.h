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
