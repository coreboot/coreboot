/* SPDX-License-Identifier: GPL-2.0-or-later */

/*
 * SPI flash internal definitions
 */

#ifndef SPI_FLASH_INTERNAL_H
#define SPI_FLASH_INTERNAL_H

#include <types.h>

/* Common commands */
#define CMD_READ_ID			0x9f

#define CMD_READ_ARRAY_SLOW		0x03
#define CMD_READ_ARRAY_FAST		0x0b
#define CMD_READ_ARRAY_LEGACY		0xe8

#define CMD_READ_FAST_DUAL_OUTPUT	0x3b
#define CMD_READ_FAST_DUAL_IO		0xbb

#define CMD_READ_STATUS			0x05
#define CMD_WRITE_ENABLE		0x06

#define CMD_FORCE_4BYTE_ADDR_MODE	0xb7

#define CMD_BLOCK_ERASE			0xD8

#define CMD_EXIT_4BYTE_ADDR_MODE	0xe9

/* Common status */
#define STATUS_WIP			0x01

/* Send a single-byte command to the device and read the response */
int spi_flash_cmd(const struct spi_slave *spi, u8 cmd, void *response, size_t len);

/* Send a multi-byte command to the device and read the response */
int spi_flash_cmd_multi(const struct spi_slave *spi, const u8 *dout, size_t bytes_out,
			void *din, size_t bytes_in);

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

/* Write to flash utilizing page program semantics. */
int spi_flash_cmd_write_page_program(const struct spi_flash *flash, u32 offset,
				size_t len, const void *buf);

/* Read len bytes into buf at offset. */
int spi_flash_cmd_read(const struct spi_flash *flash, u32 offset, size_t len, void *buf);

/* Release from deep sleep an provide alternative rdid information. */
int stmicro_release_deep_sleep_identify(const struct spi_slave *spi, u8 *idcode);

struct spi_flash_part_id {
	/* rdid command constructs 2x 16-bit id using the following method
	 * for matching after reading 5 bytes (1st byte is manuf id):
	 *    id[0] = (id[1] << 8) | id[2]
	 *    id[1] = (id[3] << 8) | id[4]
	 */
	uint16_t id[2];
	/* Log based 2 total number of sectors. */
	uint16_t nr_sectors_shift : 4;
	uint16_t fast_read_dual_output_support : 1;	/*  1-1-2 read */
	uint16_t fast_read_dual_io_support : 1;		/*  1-2-2 read */
	uint16_t _reserved_for_flags : 2;
	/* Block protection. Currently used by Winbond. */
	uint16_t protection_granularity_shift : 5;
	uint16_t bp_bits : 3;
};

struct spi_flash_ops_descriptor {
	uint8_t erase_cmd; /* Sector Erase */
	uint8_t status_cmd; /* Read Status Register */
	uint8_t pp_cmd; /* Page program command, if supported. */
	uint8_t wren_cmd; /* Write Enable command. */
	struct spi_flash_ops ops;
};

/* Vendor info represents a common set of organization and commands by a given
 * vendor. One can implement multiple sets from a single vendor by having
 * separate objects. */
struct spi_flash_vendor_info {
	uint8_t id;
	uint8_t page_size_shift : 4; /* if page programming oriented. */
	/* Log based 2 sector size */
	uint8_t sector_size_kib_shift : 4;
	uint16_t nr_part_ids;
	const struct spi_flash_part_id *ids;
	uint16_t match_id_mask[2]; /* matching bytes of the id for this set*/
	const struct spi_flash_ops_descriptor *desc;
	const struct spi_flash_protection_ops *prot_ops;
	/* Returns 0 on success. !0 otherwise. */
	int (*after_probe)(const struct spi_flash *flash);
};

/* Manufacturer-specific probe information */
extern const struct spi_flash_vendor_info spi_flash_adesto_vi;
extern const struct spi_flash_vendor_info spi_flash_amic_vi;
extern const struct spi_flash_vendor_info spi_flash_atmel_vi;
extern const struct spi_flash_vendor_info spi_flash_eon_vi;
extern const struct spi_flash_vendor_info spi_flash_gigadevice_vi;
extern const struct spi_flash_vendor_info spi_flash_macronix_vi;
/* Probing order matters between the Spansion sequence. */
extern const struct spi_flash_vendor_info spi_flash_spansion_ext1_vi;
extern const struct spi_flash_vendor_info spi_flash_spansion_ext2_vi;
extern const struct spi_flash_vendor_info spi_flash_spansion_vi;
extern const struct spi_flash_vendor_info spi_flash_sst_ai_vi;
extern const struct spi_flash_vendor_info spi_flash_sst_vi;
extern const struct spi_flash_vendor_info spi_flash_stmicro1_vi;
extern const struct spi_flash_vendor_info spi_flash_stmicro2_vi;
extern const struct spi_flash_vendor_info spi_flash_stmicro3_vi;
extern const struct spi_flash_vendor_info spi_flash_stmicro4_vi;
extern const struct spi_flash_vendor_info spi_flash_winbond_vi;
extern const struct spi_flash_vendor_info spi_flash_issi_vi;

/* Page Programming Command Set with 0x20 Sector Erase command. */
extern const struct spi_flash_ops_descriptor spi_flash_pp_0x20_sector_desc;
/* Page Programming Command Set with 0xd8 Sector Erase command. */
extern const struct spi_flash_ops_descriptor spi_flash_pp_0xd8_sector_desc;

struct sfdp_rpmc_info {
	bool flash_hardening;
	enum {
		SFDP_RPMC_COUNTER_BITS_32		= 0,
		SFDP_RPMC_COUNTER_BITS_RESERVED		= 1,
	} monotonic_counter_size;
	enum {
		SFDP_RPMC_POLL_OP2_EXTENDED_STATUS	= 0,
		SFDP_RPMC_POLL_READ_STATUS		= 1,
	} busy_polling_method;
	uint8_t number_of_counters;
	uint8_t op1_write_command;
	uint8_t op2_read_command;
	uint64_t update_rate_s;
	uint64_t read_counter_polling_delay_us;
	uint64_t write_counter_polling_short_delay_us;
	uint64_t write_counter_polling_long_delay_us;
};

/* Get RPMC information from the SPI flash's SFDP table */
enum cb_err spi_flash_get_sfdp_rpmc(const struct spi_flash *flash,
				    struct sfdp_rpmc_info *rpmc_info);

/* Fill rpmc_caps field in spi_flash struct with RPMC config from SFDP */
void spi_flash_fill_rpmc_caps(struct spi_flash *flash);

#endif /* SPI_FLASH_INTERNAL_H */
