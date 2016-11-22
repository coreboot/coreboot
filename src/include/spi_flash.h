/*
 * Interface to SPI flash
 *
 * Copyright (C) 2008 Atmel Corporation
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * version 2 as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */
#ifndef _SPI_FLASH_H_
#define _SPI_FLASH_H_

#include <stdint.h>
#include <stddef.h>
#include <boot/coreboot_tables.h>

/* SPI Flash opcodes */
#define SPI_OPCODE_WREN 0x06
#define SPI_OPCODE_FAST_READ 0x0b

struct spi_flash {
	struct spi_slave *spi;
	const char *name;
	u32 size;
	u32 sector_size;
	u8 erase_cmd;
	u8 status_cmd;
	/*
	 * Internal functions are expected to be called ONLY by spi flash
	 * driver. External components should only use the public API calls
	 * spi_flash_{read,write,erase,status,volatile_group_begin,
	 * volatile_group_end}.
	 */
	int (*internal_read)(const struct spi_flash *flash, u32 offset,
				size_t len, void *buf);
	int (*internal_write)(const struct spi_flash *flash, u32 offset,
				size_t len, const void *buf);
	int (*internal_erase)(const struct spi_flash *flash, u32 offset,
				size_t len);
	int (*internal_status)(const struct spi_flash *flash, u8 *reg);
};

void lb_spi_flash(struct lb_header *header);

/* SPI Flash Driver Public API */
struct spi_flash *spi_flash_probe(unsigned int bus, unsigned int cs);
/*
 * Specialized probing performed by platform. This is a weak function which can
 * be overriden by platform driver.
 * spi   = Pointer to spi_slave structure.
 * force = Indicates if the platform driver can skip specialized probing.
 */
struct spi_flash *spi_flash_programmer_probe(struct spi_slave *spi, int force);

/* All the following functions return 0 on success and non-zero on error. */
int spi_flash_read(const struct spi_flash *flash, u32 offset, size_t len,
		   void *buf);
int spi_flash_write(const struct spi_flash *flash, u32 offset, size_t len,
		    const void *buf);
int spi_flash_erase(const struct spi_flash *flash, u32 offset, size_t len);
int spi_flash_status(const struct spi_flash *flash, u8 *reg);
/*
 * Some SPI controllers require exclusive access to SPI flash when volatile
 * operations like erase or write are being performed. In such cases,
 * volatile_group_begin will gain exclusive access to SPI flash if not already
 * acquired and volatile_group_end will end exclusive access if this was the
 * last request in the group. spi_flash_{write,erase} operations call
 * volatile_group_begin at the start of function and volatile_group_end after
 * erase/write operation is performed. These functions can also be used by any
 * components that wish to club multiple volatile operations into a single
 * group.
 */
int spi_flash_volatile_group_begin(const struct spi_flash *flash);
int spi_flash_volatile_group_end(const struct spi_flash *flash);

/*
 * These are callbacks for marking the start and end of volatile group as
 * handled by the chipset. Not every chipset requires this special handling. So,
 * these functions are expected to be implemented in Kconfig option for volatile
 * group is enabled (SPI_FLASH_HAS_VOLATILE_GROUP).
 */
int chipset_volatile_group_begin(const struct spi_flash *flash);
int chipset_volatile_group_end(const struct spi_flash *flash);

#endif /* _SPI_FLASH_H_ */
