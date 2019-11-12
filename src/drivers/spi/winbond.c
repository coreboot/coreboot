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

#include <console/console.h>
#include <stdlib.h>
#include <spi_flash.h>
#include <spi-generic.h>
#include <string.h>
#include <delay.h>
#include <lib.h>

#include "spi_flash_internal.h"
#include "spi_winbond.h"

struct winbond_spi_flash_params {
	uint16_t id;
	uint8_t dual_spi : 1;
	uint8_t _reserved_for_flags : 3;
	uint8_t l2_page_size_shift : 4;
	uint8_t pages_per_sector_shift : 4;
	uint8_t sectors_per_block_shift : 4;
	uint8_t nr_blocks_shift;
	uint8_t bp_bits : 3;
	uint8_t protection_granularity_shift : 5;
	char name[10];
};

union status_reg1_bp3 {
	uint8_t u;
	struct {
		uint8_t busy : 1;
		uint8_t wel  : 1;
		uint8_t bp   : 3;
		uint8_t tb   : 1;
		uint8_t sec  : 1;
		uint8_t srp0 : 1;
	};
};

union status_reg1_bp4 {
	uint8_t u;
	struct {
		uint8_t busy : 1;
		uint8_t wel  : 1;
		uint8_t bp   : 4;
		uint8_t tb   : 1;
		uint8_t srp0 : 1;
	};
};

union status_reg2 {
	uint8_t u;
	struct {
		uint8_t srp1 : 1;
		uint8_t   qe : 1;
		uint8_t  res : 1;
		uint8_t   lb : 3;
		uint8_t  cmp : 1;
		uint8_t  sus : 1;
	};
};

struct status_regs {
	union {
		struct {
#if defined(__BIG_ENDIAN)
			union status_reg2 reg2;
			union {
				union status_reg1_bp3 reg1_bp3;
				union status_reg1_bp4 reg1_bp4;
			};
#else
			union {
				union status_reg1_bp3 reg1_bp3;
				union status_reg1_bp4 reg1_bp4;
			};
			union status_reg2 reg2;
#endif
		};
		u16 u;
	};
};

static const struct winbond_spi_flash_params winbond_spi_flash_table[] = {
	{
		.id				= 0x2014,
		.l2_page_size_shift		= 8,
		.pages_per_sector_shift		= 4,
		.sectors_per_block_shift	= 4,
		.nr_blocks_shift		= 4,
		.name				= "W25P80",
	},
	{
		.id				= 0x2015,
		.l2_page_size_shift		= 8,
		.pages_per_sector_shift		= 4,
		.sectors_per_block_shift	= 4,
		.nr_blocks_shift		= 5,
		.name				= "W25P16",
	},
	{
		.id				= 0x2016,
		.l2_page_size_shift		= 8,
		.pages_per_sector_shift		= 4,
		.sectors_per_block_shift	= 4,
		.nr_blocks_shift		= 6,
		.name				= "W25P32",
	},
	{
		.id				= 0x3014,
		.l2_page_size_shift		= 8,
		.pages_per_sector_shift		= 4,
		.sectors_per_block_shift	= 4,
		.nr_blocks_shift		= 4,
		.name				= "W25X80",
		.dual_spi			= 1,
	},
	{
		.id				= 0x3015,
		.l2_page_size_shift		= 8,
		.pages_per_sector_shift		= 4,
		.sectors_per_block_shift	= 4,
		.nr_blocks_shift		= 5,
		.name				= "W25X16",
		.dual_spi			= 1,
	},
	{
		.id				= 0x3016,
		.l2_page_size_shift		= 8,
		.pages_per_sector_shift		= 4,
		.sectors_per_block_shift	= 4,
		.nr_blocks_shift		= 6,
		.name				= "W25X32",
		.dual_spi			= 1,
	},
	{
		.id				= 0x3017,
		.l2_page_size_shift		= 8,
		.pages_per_sector_shift		= 4,
		.sectors_per_block_shift	= 4,
		.nr_blocks_shift		= 7,
		.name				= "W25X64",
		.dual_spi			= 1,
	},
	{
		.id				= 0x4014,
		.l2_page_size_shift		= 8,
		.pages_per_sector_shift		= 4,
		.sectors_per_block_shift	= 4,
		.nr_blocks_shift		= 4,
		.name				= "W25Q80_V",
		.dual_spi			= 1,
	},
	{
		.id				= 0x4015,
		.l2_page_size_shift		= 8,
		.pages_per_sector_shift		= 4,
		.sectors_per_block_shift	= 4,
		.nr_blocks_shift		= 5,
		.name				= "W25Q16_V",
		.dual_spi			= 1,
		.protection_granularity_shift	= 16,
		.bp_bits			= 3,
	},
	{
		.id				= 0x6015,
		.l2_page_size_shift		= 8,
		.pages_per_sector_shift		= 4,
		.sectors_per_block_shift	= 4,
		.nr_blocks_shift		= 5,
		.name				= "W25Q16DW",
		.dual_spi			= 1,
		.protection_granularity_shift	= 16,
		.bp_bits			= 3,
	},
	{
		.id				= 0x4016,
		.l2_page_size_shift		= 8,
		.pages_per_sector_shift		= 4,
		.sectors_per_block_shift	= 4,
		.nr_blocks_shift		= 6,
		.name				= "W25Q32_V",
		.dual_spi			= 1,
		.protection_granularity_shift	= 16,
		.bp_bits			= 3,
	},
	{
		.id				= 0x6016,
		.l2_page_size_shift		= 8,
		.pages_per_sector_shift		= 4,
		.sectors_per_block_shift	= 4,
		.nr_blocks_shift		= 6,
		.name				= "W25Q32DW",
		.dual_spi			= 1,
		.protection_granularity_shift	= 16,
		.bp_bits			= 3,
	},
	{
		.id				= 0x4017,
		.l2_page_size_shift		= 8,
		.pages_per_sector_shift		= 4,
		.sectors_per_block_shift	= 4,
		.nr_blocks_shift		= 7,
		.name				= "W25Q64_V",
		.dual_spi			= 1,
		.protection_granularity_shift	= 17,
		.bp_bits			= 3,
	},
	{
		.id				= 0x6017,
		.l2_page_size_shift		= 8,
		.pages_per_sector_shift		= 4,
		.sectors_per_block_shift	= 4,
		.nr_blocks_shift		= 7,
		.name				= "W25Q64DW",
		.dual_spi			= 1,
		.protection_granularity_shift	= 17,
		.bp_bits			= 3,
	},
	{
		.id				= 0x4018,
		.l2_page_size_shift		= 8,
		.pages_per_sector_shift		= 4,
		.sectors_per_block_shift	= 4,
		.nr_blocks_shift		= 8,
		.name				= "W25Q128_V",
		.dual_spi			= 1,
		.protection_granularity_shift	= 18,
		.bp_bits			= 3,
	},
	{
		.id				= 0x6018,
		.l2_page_size_shift		= 8,
		.pages_per_sector_shift		= 4,
		.sectors_per_block_shift	= 4,
		.nr_blocks_shift		= 8,
		.name				= "W25Q128FW",
		.dual_spi			= 1,
		.protection_granularity_shift	= 18,
		.bp_bits			= 3,
	},
	{
		.id				= 0x7018,
		.l2_page_size_shift		= 8,
		.pages_per_sector_shift		= 4,
		.sectors_per_block_shift	= 4,
		.nr_blocks_shift		= 8,
		.name				= "W25Q128J",
		.dual_spi			= 1,
		.protection_granularity_shift	= 18,
		.bp_bits			= 3,
	},
	{
		.id                             = 0x8018,
		.l2_page_size_shift             = 8,
		.pages_per_sector_shift         = 4,
		.sectors_per_block_shift        = 4,
		.nr_blocks_shift                = 8,
		.name                           = "W25Q128JW",
		.dual_spi                       = 1,
		.protection_granularity_shift   = 18,
		.bp_bits                        = 3,
	},
	{
		.id				= 0x4019,
		.l2_page_size_shift		= 8,
		.pages_per_sector_shift		= 4,
		.sectors_per_block_shift	= 4,
		.nr_blocks_shift		= 9,
		.name				= "W25Q256_V",
		.dual_spi			= 1,
		.protection_granularity_shift	= 16,
		.bp_bits			= 4,
	},
	{
		.id				= 0x7019,
		.l2_page_size_shift		= 8,
		.pages_per_sector_shift		= 4,
		.sectors_per_block_shift	= 4,
		.nr_blocks_shift		= 9,
		.name				= "W25Q256J",
		.dual_spi			= 1,
		.protection_granularity_shift	= 16,
		.bp_bits			= 4,
	},
};

static int winbond_write(const struct spi_flash *flash, u32 offset, size_t len,
			const void *buf)
{
	unsigned long byte_addr;
	unsigned long page_size;
	size_t chunk_len;
	size_t actual;
	int ret = 0;
	u8 cmd[4];

	page_size = flash->page_size;

	for (actual = 0; actual < len; actual += chunk_len) {
		byte_addr = offset % page_size;
		chunk_len = min(len - actual, page_size - byte_addr);
		chunk_len = spi_crop_chunk(&flash->spi, sizeof(cmd), chunk_len);

		cmd[0] = CMD_W25_PP;
		cmd[1] = (offset >> 16) & 0xff;
		cmd[2] = (offset >> 8) & 0xff;
		cmd[3] = offset & 0xff;
#if CONFIG(DEBUG_SPI_FLASH)
		printk(BIOS_SPEW, "PP: 0x%p => cmd = { 0x%02x 0x%02x%02x%02x }"
		        " chunk_len = %zu\n", buf + actual,
			cmd[0], cmd[1], cmd[2], cmd[3], chunk_len);
#endif

		ret = spi_flash_cmd(&flash->spi, CMD_W25_WREN, NULL, 0);
		if (ret < 0) {
			printk(BIOS_WARNING, "SF: Enabling Write failed\n");
			goto out;
		}

		ret = spi_flash_cmd_write(&flash->spi, cmd, sizeof(cmd),
				buf + actual, chunk_len);
		if (ret < 0) {
			printk(BIOS_WARNING, "SF: Winbond Page Program failed\n");
			goto out;
		}

		ret = spi_flash_cmd_wait_ready(flash,
				SPI_FLASH_PROG_TIMEOUT_MS);
		if (ret)
			goto out;

		offset += chunk_len;
	}

#if CONFIG(DEBUG_SPI_FLASH)
	printk(BIOS_SPEW, "SF: Winbond: Successfully programmed %zu bytes @"
			" 0x%lx\n", len, (unsigned long)(offset - len));
#endif
	ret = 0;

out:
	return ret;
}

/*
 * Convert BPx, TB and CMP to a region.
 * SEC (if available) must be zero.
 */
static void winbond_bpbits_to_region(const size_t granularity,
				     const u8 bp,
				     bool tb,
				     const bool cmp,
				     const size_t flash_size,
				     struct region *out)
{
	size_t protected_size =
		min(bp ? granularity << (bp - 1) : 0, flash_size);

	if (cmp) {
		protected_size = flash_size - protected_size;
		tb = !tb;
	}

	out->offset = tb ? 0 : flash_size - protected_size;
	out->size = protected_size;
}

/*
 * Available on all devices.
 * Read block protect bits from Status/Status2 Reg.
 * Converts block protection bits to a region.
 *
 * Returns:
 * -1    on error
 *  1    if region is covered by write protection
 *  0    if a part of region isn't covered by write protection
 */
static int winbond_get_write_protection(const struct spi_flash *flash,
					const struct region *region)
{
	const struct winbond_spi_flash_params *params;
	struct region wp_region;
	union status_reg2 reg2;
	u8 bp, tb;
	int ret;

	params = (const struct winbond_spi_flash_params *)flash->driver_private;
	const size_t granularity = (1 << params->protection_granularity_shift);

	if (params->bp_bits == 3) {
		union status_reg1_bp3 reg1_bp3 = { .u = 0 };

		ret = spi_flash_cmd(&flash->spi, flash->status_cmd, &reg1_bp3.u,
				    sizeof(reg1_bp3.u));

		if (reg1_bp3.sec) {
			// FIXME: not supported
			return -1;
		}

		bp = reg1_bp3.bp;
		tb = reg1_bp3.tb;
	} else if (params->bp_bits == 4) {
		union status_reg1_bp4 reg1_bp4 = { .u = 0 };

		ret = spi_flash_cmd(&flash->spi, flash->status_cmd, &reg1_bp4.u,
				    sizeof(reg1_bp4.u));

		bp = reg1_bp4.bp;
		tb = reg1_bp4.tb;
	} else {
		// FIXME: not supported
		return -1;
	}
	if (ret)
		return ret;

	ret = spi_flash_cmd(&flash->spi, CMD_W25_RDSR2, &reg2.u,
			    sizeof(reg2.u));
	if (ret)
		return ret;

	winbond_bpbits_to_region(granularity, bp, tb, reg2.cmp, flash->size,
				 &wp_region);

	if (!region_sz(&wp_region)) {
		printk(BIOS_DEBUG, "WINBOND: flash isn't protected\n");

		return 0;
	}

	printk(BIOS_DEBUG, "WINBOND: flash protected range 0x%08zx-0x%08zx\n",
	       region_offset(&wp_region), region_end(&wp_region));

	return region_is_subregion(&wp_region, region);
}

/**
 * Common method to write some bit of the status register 1 & 2 at the same
 * time. Only change bits that are one in @mask.
 * Compare the final result to make sure that the register isn't locked.
 *
 * @param mask: The bits that are affected by @val
 * @param val: The bits to write
 * @param non_volatile: Make setting permanent
 *
 * @return 0 on success
 */
static int winbond_flash_cmd_status(const struct spi_flash *flash,
				    const u16 mask,
				    const u16 val,
				    const bool non_volatile)
{
	struct {
		u8 cmd;
		u16 sreg;
	} __packed cmdbuf;
	u8 reg8;
	int ret;

	if (!flash)
		return -1;

	ret = spi_flash_cmd(&flash->spi, CMD_W25_RDSR, &reg8, sizeof(reg8));
	if (ret)
		return ret;

	cmdbuf.sreg = reg8;

	ret = spi_flash_cmd(&flash->spi, CMD_W25_RDSR2, &reg8, sizeof(reg8));
	if (ret)
		return ret;

	cmdbuf.sreg |= reg8 << 8;

	if ((val & mask) == (cmdbuf.sreg & mask))
		return 0;

	if (non_volatile) {
		ret = spi_flash_cmd(&flash->spi, CMD_W25_WREN, NULL, 0);
	} else {
		ret = spi_flash_cmd(&flash->spi, CMD_VOLATILE_SREG_WREN, NULL,
				    0);
	}
	if (ret)
		return ret;

	cmdbuf.sreg &= ~mask;
	cmdbuf.sreg |= val & mask;
	cmdbuf.cmd = CMD_W25_WRSR;

	/* Legacy method of writing status register 1 & 2 */
	ret = spi_flash_cmd_write(&flash->spi, (u8 *)&cmdbuf, sizeof(cmdbuf),
				  NULL, 0);
	if (ret)
		return ret;

	if (non_volatile) {
		/* Wait tw */
		ret = spi_flash_cmd_wait_ready(flash, WINBOND_FLASH_TIMEOUT);
		if (ret)
			return ret;
	} else {
		/* Wait tSHSL */
		udelay(1);
	}

	/* Now read the status register to make sure it's not locked */
	ret = spi_flash_cmd(&flash->spi, CMD_W25_RDSR, &reg8, sizeof(reg8));
	if (ret)
		return ret;

	cmdbuf.sreg = reg8;

	ret = spi_flash_cmd(&flash->spi, CMD_W25_RDSR2, &reg8, sizeof(reg8));
	if (ret)
		return ret;

	cmdbuf.sreg |= reg8 << 8;

	printk(BIOS_DEBUG, "WINBOND: SREG=%02x SREG2=%02x\n",
	       cmdbuf.sreg & 0xff,
	       cmdbuf.sreg >> 8);

	/* Compare against expected result */
	if ((val & mask) != (cmdbuf.sreg & mask)) {
		printk(BIOS_ERR, "WINBOND: SREG is locked!\n");
		ret = -1;
	}

	return ret;
}

/*
 * Available on all devices.
 * Protect a region starting from start of flash or end of flash.
 * The caller must provide a supported protected region size.
 * SEC isn't supported and set to zero.
 * Write block protect bits to Status/Status2 Reg.
 * Optionally lock the status register if lock_sreg is set with the provided
 * mode.
 *
 * @param flash: The flash to operate on
 * @param region: The region to write protect
 * @param non_volatile: Make setting permanent
 * @param mode: Optional status register lock-down mode
 *
 * @return 0 on success
 */
static int
winbond_set_write_protection(const struct spi_flash *flash,
			     const struct region *region,
			     const bool non_volatile,
			     const enum spi_flash_status_reg_lockdown mode)
{
	const struct winbond_spi_flash_params *params;
	struct status_regs mask, val;
	struct region wp_region;
	u8 cmp, bp, tb;
	int ret;

	/* Need to touch TOP or BOTTOM */
	if (region_offset(region) != 0 && region_end(region) != flash->size)
		return -1;

	params = (const struct winbond_spi_flash_params *)flash->driver_private;
	if (!params)
		return -1;

	if (params->bp_bits != 3 && params->bp_bits != 4) {
		/* FIXME: not implemented */
		return -1;
	}

	wp_region = *region;

	if (region_offset(&wp_region) == 0)
		tb = 1;
	else
		tb = 0;

	if (region_sz(&wp_region) > flash->size / 2) {
		cmp = 1;
		wp_region.offset = tb ? 0 : region_sz(&wp_region);
		wp_region.size = flash->size - region_sz(&wp_region);
		tb = !tb;
	} else {
		cmp = 0;
	}

	if (region_sz(&wp_region) == 0) {
		bp = 0;
	} else if (IS_POWER_OF_2(region_sz(&wp_region)) &&
		   (region_sz(&wp_region) >=
		    (1 << params->protection_granularity_shift))) {
		bp = log2(region_sz(&wp_region)) -
			  params->protection_granularity_shift + 1;
	} else {
		printk(BIOS_ERR, "WINBOND: ERROR: unsupported region size\n");
		return -1;
	}

	/* Write block protection bits */

	if (params->bp_bits == 3) {
		val.reg1_bp3 = (union status_reg1_bp3) { .bp = bp, .tb = tb,
							.sec = 0 };
		mask.reg1_bp3 = (union status_reg1_bp3) { .bp = ~0, .tb = 1,
							.sec = 1 };
	} else {
		val.reg1_bp4 = (union status_reg1_bp4) { .bp = bp, .tb = tb };
		mask.reg1_bp4 = (union status_reg1_bp4) { .bp = ~0, .tb = 1 };
	}

	val.reg2 = (union status_reg2) { .cmp = cmp };
	mask.reg2 = (union status_reg2) { .cmp = 1 };

	if (mode != SPI_WRITE_PROTECTION_PRESERVE) {
		u8 srp;
		switch (mode) {
		case SPI_WRITE_PROTECTION_NONE:
			srp = 0;
		break;
		case SPI_WRITE_PROTECTION_PIN:
			srp = 1;
		break;
		case SPI_WRITE_PROTECTION_REBOOT:
			srp = 2;
		break;
		case SPI_WRITE_PROTECTION_PERMANENT:
			srp = 3;
		break;
		default:
			return -1;
		}

		if (params->bp_bits == 3) {
			val.reg1_bp3.srp0 = !!(srp & 1);
			mask.reg1_bp3.srp0 = 1;
		} else {
			val.reg1_bp4.srp0 = !!(srp & 1);
			mask.reg1_bp4.srp0 = 1;
		}

		val.reg2.srp1 = !!(srp & 2);
		mask.reg2.srp1 = 1;
	}

	ret = winbond_flash_cmd_status(flash, mask.u, val.u, non_volatile);
	if (ret)
		return ret;

	printk(BIOS_DEBUG, "WINBOND: write-protection set to range "
	       "0x%08zx-0x%08zx\n", region_offset(region), region_end(region));

	return ret;
}

static const struct spi_flash_ops spi_flash_ops = {
	.write = winbond_write,
	.erase = spi_flash_cmd_erase,
	.status = spi_flash_cmd_status,
	.get_write_protection = winbond_get_write_protection,
	.set_write_protection = winbond_set_write_protection,
};

int spi_flash_probe_winbond(const struct spi_slave *spi, u8 *idcode,
			    struct spi_flash *flash)
{
	const struct winbond_spi_flash_params *params;
	unsigned int i;

	for (i = 0; i < ARRAY_SIZE(winbond_spi_flash_table); i++) {
		params = &winbond_spi_flash_table[i];
		if (params->id == ((idcode[1] << 8) | idcode[2]))
			break;
	}

	if (i == ARRAY_SIZE(winbond_spi_flash_table)) {
		printk(BIOS_WARNING, "SF: Unsupported Winbond ID %02x%02x\n",
				idcode[1], idcode[2]);
		return -1;
	}

	memcpy(&flash->spi, spi, sizeof(*spi));
	flash->name = params->name;

	/* Params are in power-of-two. */
	flash->page_size = 1 << params->l2_page_size_shift;
	flash->sector_size = flash->page_size *
			(1 << params->pages_per_sector_shift);
	flash->size = flash->sector_size *
			(1 << params->sectors_per_block_shift) *
			(1 << params->nr_blocks_shift);
	flash->erase_cmd = CMD_W25_SE;
	flash->status_cmd = CMD_W25_RDSR;

	flash->flags.dual_spi = params->dual_spi;

	flash->ops = &spi_flash_ops;
	flash->driver_private = params;

	return 0;
}
