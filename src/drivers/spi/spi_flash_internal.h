/*
 * This file is part of the coreboot project.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

/*
 * SPI flash internal definitions
 */

#ifndef SPI_FLASH_INTERNAL_H
#define SPI_FLASH_INTERNAL_H

/* Common commands */
#define CMD_READ_ID			0x9f

#define CMD_READ_ARRAY_SLOW		0x03
#define CMD_READ_ARRAY_FAST		0x0b
#define CMD_READ_ARRAY_LEGACY		0xe8

#define CMD_READ_FAST_DUAL_OUTPUT	0x3b

#define CMD_READ_STATUS			0x05
#define CMD_WRITE_ENABLE		0x06

#define CMD_BLOCK_ERASE			0xD8

/* Common status */
#define STATUS_WIP			0x01

/* Send a single-byte command to the device and read the response */
int spi_flash_cmd(const struct spi_slave *spi, u8 cmd, void *response, size_t len);

/*
 * Send a multi-byte command to the device followed by (optional)
 * data. Used for programming the flash array, etc.
 */
int spi_flash_cmd_write(const struct spi_slave *spi, const u8 *cmd,
			size_t cmd_len, const void *data, size_t data_len);

/* Send a command to the device and wait for some bit to clear itself. */
int spi_flash_cmd_poll_bit(const struct spi_flash *flash, unsigned long timeout,
			   u8 cmd, u8 poll_bit);

/*
 * Send the read status command to the device and wait for the wip
 * (write-in-progress) bit to clear itself.
 */
int spi_flash_cmd_wait_ready(const struct spi_flash *flash, unsigned long timeout);

/* Erase sectors. */
int spi_flash_cmd_erase(const struct spi_flash *flash, u32 offset, size_t len);

/* Read status register. */
int spi_flash_cmd_status(const struct spi_flash *flash, u8 *reg);

/* Manufacturer-specific probe functions */
int spi_flash_probe_spansion(const struct spi_slave *spi, u8 *idcode,
			     struct spi_flash *flash);
int spi_flash_probe_amic(const struct spi_slave *spi, u8 *idcode,
			 struct spi_flash *flash);
int spi_flash_probe_atmel(const struct spi_slave *spi, u8 *idcode,
			  struct spi_flash *flash);
int spi_flash_probe_eon(const struct spi_slave *spi, u8 *idcode,
			struct spi_flash *flash);
int spi_flash_probe_macronix(const struct spi_slave *spi, u8 *idcode,
			     struct spi_flash *flash);
int spi_flash_probe_sst(const struct spi_slave *spi, u8 *idcode,
			struct spi_flash *flash);
int spi_flash_probe_stmicro(const struct spi_slave *spi, u8 *idcode,
			    struct spi_flash *flash);
int spi_flash_probe_winbond(const struct spi_slave *spi, u8 *idcode,
			    struct spi_flash *flash);
int spi_flash_probe_gigadevice(const struct spi_slave *spi, u8 *idcode,
			       struct spi_flash *flash);
int spi_flash_probe_adesto(const struct spi_slave *spi, u8 *idcode,
			   struct spi_flash *flash);

#endif /* SPI_FLASH_INTERNAL_H */
