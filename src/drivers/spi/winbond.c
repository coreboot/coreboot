/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <console/console.h>
#include <commonlib/helpers.h>
#include <spi_flash.h>
#include <spi-generic.h>
#include <delay.h>
#include <lib.h>

#include "spi_flash_internal.h"
#include "spi_winbond.h"

union status_reg1 {
	uint8_t u;
	struct {
		uint8_t busy : 1;
		uint8_t wel  : 1;
		uint8_t bp   : 3;
		uint8_t tb   : 1;
		uint8_t sec  : 1;
		uint8_t srp0 : 1;
	} bp3;
	struct {
		uint8_t busy : 1;
		uint8_t wel  : 1;
		uint8_t bp   : 4;
		uint8_t tb   : 1;
		uint8_t srp0 : 1;
	} bp4;
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
			union status_reg1 reg1;
#else
			union status_reg1 reg1;
			union status_reg2 reg2;
#endif
		};
		u16 u;
	};
};

static const struct spi_flash_part_id flash_table[] = {
	{
		/* W25P80 */
		.id[0]				= 0x2014,
		.nr_sectors_shift		= 8,
	},
	{
		/* W25P16 */
		.id[0]				= 0x2015,
		.nr_sectors_shift		= 9,
	},
	{
		/* W25P32 */
		.id[0]				= 0x2016,
		.nr_sectors_shift		= 10,
	},
	{
		/* W25X80 */
		.id[0]				= 0x3014,
		.nr_sectors_shift		= 8,
		.fast_read_dual_output_support	= 1,
	},
	{
		/* W25X16 */
		.id[0]				= 0x3015,
		.nr_sectors_shift		= 9,
		.fast_read_dual_output_support	= 1,
	},
	{
		/* W25X32 */
		.id[0]				= 0x3016,
		.nr_sectors_shift		= 10,
		.fast_read_dual_output_support	= 1,
	},
	{
		/* W25X64 */
		.id[0]				= 0x3017,
		.nr_sectors_shift		= 11,
		.fast_read_dual_output_support	= 1,
	},
	{
		/* W25Q80_V */
		.id[0]				= 0x4014,
		.nr_sectors_shift		= 8,
		.fast_read_dual_output_support	= 1,
		.fast_read_dual_io_support	= 1,
	},
	{
		/* W25Q16_V */
		.id[0]				= 0x4015,
		.nr_sectors_shift		= 9,
		.fast_read_dual_output_support	= 1,
		.fast_read_dual_io_support	= 1,
		.protection_granularity_shift	= 16,
		.bp_bits			= 3,
	},
	{
		/* W25Q16DW */
		.id[0]				= 0x6015,
		.nr_sectors_shift		= 9,
		.fast_read_dual_output_support	= 1,
		.fast_read_dual_io_support	= 1,
		.protection_granularity_shift	= 16,
		.bp_bits			= 3,
	},
	{
		/* W25Q32_V */
		.id[0]				= 0x4016,
		.nr_sectors_shift		= 10,
		.fast_read_dual_output_support	= 1,
		.fast_read_dual_io_support	= 1,
		.protection_granularity_shift	= 16,
		.bp_bits			= 3,
	},
	{
		/* W25Q32DW */
		.id[0]				= 0x6016,
		.nr_sectors_shift		= 10,
		.fast_read_dual_output_support	= 1,
		.fast_read_dual_io_support	= 1,
		.protection_granularity_shift	= 16,
		.bp_bits			= 3,
	},
	{
		/* W25Q64_V */
		.id[0]				= 0x4017,
		.nr_sectors_shift		= 11,
		.fast_read_dual_output_support	= 1,
		.fast_read_dual_io_support	= 1,
		.protection_granularity_shift	= 17,
		.bp_bits			= 3,
	},
	{
		/* W25Q64DW */
		.id[0]				= 0x6017,
		.nr_sectors_shift		= 11,
		.fast_read_dual_output_support	= 1,
		.fast_read_dual_io_support	= 1,
		.protection_granularity_shift	= 17,
		.bp_bits			= 3,
	},
	{
		/* W25Q64JW */
		.id[0]				= 0x8017,
		.nr_sectors_shift		= 11,
		.fast_read_dual_output_support	= 1,
		.fast_read_dual_io_support	= 1,
		.protection_granularity_shift	= 17,
		.bp_bits			= 3,
	},
	{
		/* W25Q128_V */
		.id[0]				= 0x4018,
		.nr_sectors_shift		= 12,
		.fast_read_dual_output_support	= 1,
		.fast_read_dual_io_support	= 1,
		.protection_granularity_shift	= 18,
		.bp_bits			= 3,
	},
	{
		/* W25Q128FW */
		.id[0]				= 0x6018,
		.nr_sectors_shift		= 12,
		.fast_read_dual_output_support	= 1,
		.fast_read_dual_io_support	= 1,
		.protection_granularity_shift	= 18,
		.bp_bits			= 3,
	},
	{
		/* W25Q128J */
		.id[0]				= 0x7018,
		.nr_sectors_shift		= 12,
		.fast_read_dual_output_support	= 1,
		.fast_read_dual_io_support	= 1,
		.protection_granularity_shift	= 18,
		.bp_bits			= 3,
	},
	{
		/* W25Q128JW */
		.id[0]				= 0x8018,
		.nr_sectors_shift		= 12,
		.fast_read_dual_output_support	= 1,
		.fast_read_dual_io_support	= 1,
		.protection_granularity_shift	= 18,
		.bp_bits			= 3,
	},
	{
		/* W25Q512NW-IM */
		.id[0]				= 0x8020,
		.nr_sectors_shift		= 14,
		.fast_read_dual_output_support	= 1,
		.fast_read_dual_io_support	= 1,
		.protection_granularity_shift	= 16,
		.bp_bits			= 4,
	},
	{
		/* W25Q256_V */
		.id[0]				= 0x4019,
		.nr_sectors_shift		= 13,
		.fast_read_dual_output_support	= 1,
		.fast_read_dual_io_support	= 1,
		.protection_granularity_shift	= 16,
		.bp_bits			= 4,
	},
	{
		/* W25Q256J */
		.id[0]				= 0x7019,
		.nr_sectors_shift		= 13,
		.fast_read_dual_output_support	= 1,
		.fast_read_dual_io_support	= 1,
		.protection_granularity_shift	= 16,
		.bp_bits			= 4,
	},
	{
		/* W25Q256JW */
		.id[0]				= 0x6019,
		.nr_sectors_shift		= 13,
		.fast_read_dual_output_support	= 1,
		.fast_read_dual_io_support	= 1,
		.protection_granularity_shift	= 16,
		.bp_bits			= 4,
	},
};

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
		MIN(bp ? granularity << (bp - 1) : 0, flash_size);

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
	const struct spi_flash_part_id *params;
	struct region wp_region;
	union status_reg2 reg2;
	u8 bp, tb;
	int ret;

	params = flash->part;

	if (!params)
		return -1;

	const size_t granularity = (1 << params->protection_granularity_shift);

	union status_reg1 reg1 = { .u = 0 };

	ret = spi_flash_cmd(&flash->spi, flash->status_cmd, &reg1.u,
			    sizeof(reg1.u));
	if (ret)
		return ret;

	if (params->bp_bits == 3) {
		if (reg1.bp3.sec) {
			// FIXME: not supported
			return -1;
		}

		bp = reg1.bp3.bp;
		tb = reg1.bp3.tb;
	} else if (params->bp_bits == 4) {
		bp = reg1.bp4.bp;
		tb = reg1.bp4.tb;
	} else {
		// FIXME: not supported
		return -1;
	}

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
 * @param mode: Optional status register lock-down mode
 *
 * @return 0 on success
 */
static int
winbond_set_write_protection(const struct spi_flash *flash,
			     const struct region *region,
			     const enum spi_flash_status_reg_lockdown mode)
{
	const struct spi_flash_part_id *params;
	struct status_regs mask, val;
	struct region wp_region;
	u8 cmp, bp, tb;
	int ret;

	/* Need to touch TOP or BOTTOM */
	if (region_offset(region) != 0 && region_end(region) != flash->size)
		return -1;

	params = flash->part;

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
		val.reg1 = (union status_reg1) {
			.bp3 = { .bp = bp, .tb = tb, .sec = 0 }
		};
		mask.reg1 = (union status_reg1) {
			.bp3 = { .bp = ~0, .tb = 1, .sec = 1 }
		};
	} else {
		val.reg1 = (union status_reg1) {
			.bp4 = { .bp = bp, .tb = tb }
		};
		mask.reg1 = (union status_reg1) {
			.bp4 = { .bp = ~0, .tb = 1 }
		};
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
			val.reg1.bp3.srp0 = !!(srp & 1);
			mask.reg1.bp3.srp0 = 1;
		} else {
			val.reg1.bp4.srp0 = !!(srp & 1);
			mask.reg1.bp4.srp0 = 1;
		}

		val.reg2.srp1 = !!(srp & 2);
		mask.reg2.srp1 = 1;
	}

	ret = winbond_flash_cmd_status(flash, mask.u, val.u, true);
	if (ret)
		return ret;

	printk(BIOS_DEBUG, "WINBOND: write-protection set to range "
	       "0x%08zx-0x%08zx\n", region_offset(region), region_end(region));

	return ret;
}

static const struct spi_flash_protection_ops spi_flash_protection_ops = {
	.get_write = winbond_get_write_protection,
	.set_write = winbond_set_write_protection,
};

const struct spi_flash_vendor_info spi_flash_winbond_vi = {
	.id = VENDOR_ID_WINBOND,
	.page_size_shift = 8,
	.sector_size_kib_shift = 2,
	.match_id_mask[0] = 0xffff,
	.ids = flash_table,
	.nr_part_ids = ARRAY_SIZE(flash_table),
	.desc = &spi_flash_pp_0x20_sector_desc,
	.prot_ops = &spi_flash_protection_ops,
};
