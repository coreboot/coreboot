/* Interface to SPI flash */
/* SPDX-License-Identifier: GPL-2.0-only */
#ifndef _SPI_FLASH_H_
#define _SPI_FLASH_H_

#include <stdint.h>
#include <stddef.h>
#include <spi-generic.h>
#include <boot/coreboot_tables.h>

/* SPI Flash opcodes */
#define SPI_OPCODE_WREN 0x06
#define SPI_OPCODE_FAST_READ 0x0b

/* SPI RPMC field lengths in bytes */
#define SPI_RPMC_TAG_LEN		12
#define SPI_RPMC_SIG_LEN		32

struct spi_flash;

/*
 * SPI write protection is enforced by locking the status register.
 * The following modes are known. It depends on the flash chip if the
 * mode is actually supported.
 *
 * PRESERVE : Keep the previous status register lock-down setting (noop)
 * NONE     : Status register isn't locked
 * PIN      : Status register is locked as long as the ~WP pin is active
 * REBOOT   : Status register is locked until power failure
 * PERMANENT: Status register is permanently locked
 */
enum spi_flash_status_reg_lockdown {
	SPI_WRITE_PROTECTION_PRESERVE = -1,
	SPI_WRITE_PROTECTION_NONE = 0,
	SPI_WRITE_PROTECTION_PIN,
	SPI_WRITE_PROTECTION_REBOOT,
	SPI_WRITE_PROTECTION_PERMANENT
};

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
};

struct spi_flash_bpbits {
	unsigned int bp;   /*< block protection select bits */
	bool cmp;          /*< complement protect */
	bool tb;           /*< top=0 / bottom=1 select */
	union {
		struct {
			bool srp1, srp0;
		} winbond;
	};
};

/* Current code assumes all callbacks are supplied in this object. */
struct spi_flash_protection_ops {
	/*
	 * Returns 1 if the whole region is software write protected.
	 * Hardware write protection mechanism aren't accounted.
	 * If the write protection could be changed, due to unlocked status
	 * register for example, 0 should be returned.
	 * Returns 0 on success.
	 */
	int (*get_write)(const struct spi_flash *flash,
				    const struct region *region);
	/*
	 * Enable the status register write protection, if supported on the
	 * requested region, and optionally enable status register lock-down.
	 * Returns 0 if the whole region was software write protected.
	 * Hardware write protection mechanism aren't accounted.
	 * If the status register is locked and the requested configuration
	 * doesn't match the selected one, return an error.
	 * Only a single region is supported !
	 *
	 * @return 0 on success
	 */
	int
	(*set_write)(const struct spi_flash *flash,
				const struct region *region,
				const enum spi_flash_status_reg_lockdown mode);

};

struct spi_flash_part_id;

struct spi_flash {
	struct spi_slave spi;
	u8 vendor;
	union {
		u8 raw;
		struct {
			u8 dual_output	: 1;
			u8 dual_io	: 1;
			u8 _reserved	: 6;
		};
	} flags;
	u16 model;
	u32 size;
	u32 sector_size;
	u32 page_size;
	u8 erase_cmd;
	u8 status_cmd;
	u8 pp_cmd; /* Page program command. */
	u8 wren_cmd; /* Write Enable command. */
	const struct spi_flash_ops *ops;
	/* If !NULL all protection callbacks exist. */
	const struct spi_flash_protection_ops *prot_ops;
	const struct spi_flash_part_id *part;
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
 * Enable the vendor dependent SPI flash write protection. The region not
 * covered by write-protection will be set to write-able state.
 * Only a single write-protected region is supported.
 * Some flash ICs require the region to be aligned in the block size, sector
 * size or page size.
 * Some flash ICs require the region to start at TOP or BOTTOM.
 *
 * @param flash : A SPI flash device
 * @param region: A subregion of the device's region
 * @param mode: Optional lock-down of status register

 * @return 0 on success
 */
int
spi_flash_set_write_protected(const struct spi_flash *flash,
			      const struct region *region,
			      const enum spi_flash_status_reg_lockdown mode);

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
 * if CONFIG(BOOT_DEVICE_SPI_FLASH) is enabled. */
const struct spi_flash *boot_device_spi_flash(void);

/* Protect a region of spi flash using its controller, if available. Returns
 * < 0 on error, else 0 on success. */
int spi_flash_ctrlr_protect_region(const struct spi_flash *flash,
				   const struct region *region,
				   const enum ctrlr_prot_type type);

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

/*
 * Fill in the memory mapped windows used by the SPI flash device. This is useful for payloads
 * to identify SPI flash to host space mapping.
 *
 * Returns number of windows added to the table.
 */
uint32_t spi_flash_get_mmap_windows(struct flash_mmap_window *table);

/* Print the SFDP headers read from the SPI flash */
void spi_flash_print_sfdp_headers(const struct spi_flash *flash);

#endif /* _SPI_FLASH_H_ */
