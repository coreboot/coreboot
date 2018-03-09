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
#include <spi-generic.h>
#include <boot/coreboot_tables.h>

/* SPI Flash opcodes */
#define SPI_OPCODE_WREN 0x06
#define SPI_OPCODE_FAST_READ 0x0b

struct spi_flash;

/*
 * Representation of SPI flash operations:
 * read:	Flash read operation.
 * write:	Flash write operation.
 * erase:	Flash erase operation.
 * status:	Read flash status register.
 */
struct spi_flash_ops {
	int (*read)(const struct spi_flash *flash, u32 offset, size_t len,
			void *buf);
	int (*write)(const struct spi_flash *flash, u32 offset, size_t len,
			const void *buf);
	int (*erase)(const struct spi_flash *flash, u32 offset, size_t len);
	int (*status)(const struct spi_flash *flash, u8 *reg);
	/*
	 * Returns 1 if the whole region is software write protected.
	 * Hardware write protection mechanism aren't accounted.
	 * If the write protection could be changed, due to unlocked status
	 * register for example, 0 should be returned.
	 * Returns -1 on error.
	 */
	int (*get_write_protection)(const struct spi_flash *flash,
				    const struct region *region);

};

struct spi_flash {
	struct spi_slave spi;
	const char *name;
	u32 size;
	u32 sector_size;
	u32 page_size;
	u8 erase_cmd;
	u8 status_cmd;
	const struct spi_flash_ops *ops;
	const void *driver_private;
};

void lb_spi_flash(struct lb_header *header);

/* SPI Flash Driver Public API */

/*
 * Probe for SPI flash chip on given SPI bus and chip select and fill info in
 * spi_flash structure.
 *
 * Params:
 * bus   = SPI Bus # for the flash chip
 * cs    = Chip select # for the flash chip
 * flash = Pointer to spi flash structure that needs to be filled
 *
 * Return value:
 * 0 = success
 * non-zero = error
 */
int spi_flash_probe(unsigned int bus, unsigned int cs, struct spi_flash *flash);

/*
 * Generic probing for SPI flash chip based on the different flashes provided.
 *
 * Params:
 * spi   = Pointer to spi_slave structure
 * flash = Pointer to spi_flash structure that needs to be filled.
 *
 * Return value:
 * 0        = success
 * non-zero = error
 */
int spi_flash_generic_probe(const struct spi_slave *slave,
				struct spi_flash *flash);

/* All the following functions return 0 on success and non-zero on error. */
int spi_flash_read(const struct spi_flash *flash, u32 offset, size_t len,
		   void *buf);
int spi_flash_write(const struct spi_flash *flash, u32 offset, size_t len,
		    const void *buf);
int spi_flash_erase(const struct spi_flash *flash, u32 offset, size_t len);
int spi_flash_status(const struct spi_flash *flash, u8 *reg);

/*
 * Return the vendor dependent SPI flash write protection state.
 * @param flash : A SPI flash device
 * @param region: A subregion of the device's region
 *
 * Returns:
 *  -1   on error
 *   0   if the device doesn't support block protection
 *   0   if the device doesn't enable block protection
 *   0   if given range isn't covered by block protection
 *   1   if given range is covered by block protection
 */
int spi_flash_is_write_protected(const struct spi_flash *flash,
				 const struct region *region);
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

/* Return spi_flash object reference for the boot device. This is only valid
 * if CONFIG_BOOT_DEVICE_SPI_FLASH is enabled. */
const struct spi_flash *boot_device_spi_flash(void);

/* Protect a region of spi flash using its controller, if available. Returns
 * < 0 on error, else 0 on success. */
int spi_flash_ctrlr_protect_region(const struct spi_flash *flash,
					const struct region *region);

/*
 * This function is provided to support spi flash command-response transactions.
 * Only 2 vectors are supported and the 'func' is called with appropriate
 * write and read buffers together. This can be used for chipsets that
 * have specific spi flash controllers that don't conform to the normal
 * spi xfer API because they are specialized controllers and not generic.
 *
 * Returns 0 on success and non-zero on failure.
 */
int spi_flash_vector_helper(const struct spi_slave *slave,
	struct spi_op vectors[], size_t count,
	int (*func)(const struct spi_slave *slave, const void *dout,
		    size_t bytesout, void *din, size_t bytesin));

#endif /* _SPI_FLASH_H_ */
