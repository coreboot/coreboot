/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2016 Intel Corp.
 * (Written by Alexandru Gagniuc <alexandrux.gagniuc@intel.com> for Intel Corp.)
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#define __SIMPLE_DEVICE__

#include <arch/early_variables.h>
#include <arch/io.h>
#include <console/console.h>
#include <device/device.h>
#include <device/pci.h>
#include <soc/intel/common/spi_flash.h>
#include <soc/pci_devs.h>
#include <soc/spi.h>
#include <spi_flash.h>
#include <spi-generic.h>
#include <stdlib.h>
#include <string.h>

/* Helper to create a SPI context on API entry. */
#define BOILERPLATE_CREATE_CTX(ctx)		\
	struct spi_ctx	real_ctx;		\
	struct spi_ctx *ctx = &real_ctx;	\
	_spi_get_ctx(ctx)

/*
 * Anything that's not success is <0. Provided solely for readability, as these
 * constants are not used outside this file.
 */
enum errors {
	SUCCESS			= 0,
	E_NOT_IMPLEMENTED	= -1,
	E_TIMEOUT		= -2,
	E_HW_ERROR		= -3,
	E_ARGUMENT		= -4,
};

/* Reduce data-passing burden by grouping transaction data in a context. */
struct spi_ctx {
	uintptr_t mmio_base;
	device_t pci_dev;
	uint32_t hsfsts_on_last_error;
};

static void _spi_get_ctx(struct spi_ctx *ctx)
{
	uint32_t bar;

	/* FIXME: use device definition */
	ctx->pci_dev = SPI_DEV;

	bar = pci_read_config32(ctx->pci_dev, PCI_BASE_ADDRESS_0);
	ctx->mmio_base = bar & ~PCI_BASE_ADDRESS_MEM_ATTR_MASK;
	ctx->hsfsts_on_last_error = 0;
}

/* Read register from the SPI controller. 'reg' is the register offset. */
static uint32_t _spi_ctrlr_reg_read(struct spi_ctx *ctx, uint16_t reg)
{
	uintptr_t addr =  ALIGN_DOWN(ctx->mmio_base + reg, 4);
	return read32((void *)addr);
}

uint32_t spi_ctrlr_reg_read(uint16_t reg)
{
	BOILERPLATE_CREATE_CTX(ctx);
	return _spi_ctrlr_reg_read(ctx, reg);
}

/* Write to register in the SPI controller. 'reg' is the register offset. */
static void _spi_ctrlr_reg_write(struct spi_ctx *ctx, uint16_t reg,
				 uint32_t val)
{
	uintptr_t addr =  ALIGN_DOWN(ctx->mmio_base + reg, 4);
	write32((void *)addr, val);

}

/*
 * The hardware datasheet is not clear on what HORD values actually do. It
 * seems that HORD_SFDP provides access to the first 8 bytes of the SFDP, which
 * is the signature and revision fields. HORD_JEDEC provides access to the
 * actual flash parameters, and is most likely what you want to use when
 * probing the flash from software.
 * It's okay to rely on SFPD, since the SPI controller requires an SFDP 1.5 or
 * newer compliant SPI chip.
 * NOTE: Due to the register layout of the hardware, all accesses will be
 * aligned to a 4 byte boundary.
 */
static uint32_t read_spi_sfdp_param(struct spi_ctx *ctx, uint16_t sfdp_reg)
{
	uint32_t ptinx_index = sfdp_reg & SPIBAR_PTINX_IDX_MASK;
	_spi_ctrlr_reg_write(ctx, SPIBAR_PTINX,
			     ptinx_index | SPIBAR_PTINX_HORD_JEDEC);
	return _spi_ctrlr_reg_read(ctx, SPIBAR_PTDATA);
}

/* Fill FDATAn FIFO in preparation for a write transaction. */
static void fill_xfer_fifo(struct spi_ctx *ctx, const void *data, size_t len)
{
	len = min(len, SPIBAR_FDATA_FIFO_SIZE);

	/* YES! memcpy() works. FDATAn does not require 32-bit accesses. */
	memcpy((void*)(ctx->mmio_base + SPIBAR_FDATA(0)), data, len);
}

/* Drain FDATAn FIFO after a read transaction populates data. */
static void drain_xfer_fifo(struct spi_ctx *ctx, void *dest, size_t len)
{
	len = min(len, SPIBAR_FDATA_FIFO_SIZE);

	/* YES! memcpy() works. FDATAn does not require 32-bit accesses. */
	memcpy(dest, (void*)(ctx->mmio_base + SPIBAR_FDATA(0)), len);
}

/* Fire up a transfer using the hardware sequencer. */
static void start_hwseq_xfer(struct spi_ctx *ctx, uint32_t hsfsts_cycle,
				 uint32_t flash_addr, size_t len)
{
	/* Make sure all W1C status bits get cleared. */
	uint32_t hsfsts = SPIBAR_HSFSTS_W1C_BITS;
	/* Set up transaction parameters. */
	hsfsts |= hsfsts_cycle & SPIBAR_HSFSTS_FCYCLE_MASK;
	hsfsts |= SPIBAR_HSFSTS_FBDC(len - 1);

	_spi_ctrlr_reg_write(ctx, SPIBAR_FADDR, flash_addr);
	_spi_ctrlr_reg_write(ctx, SPIBAR_HSFSTS_CTL,
			     hsfsts | SPIBAR_HSFSTS_FGO);
}

static void print_xfer_error(struct spi_ctx *ctx, const char *failure_reason,
			     uint32_t flash_addr)
{
	printk(BIOS_ERR, "SPI Transaction %s at flash offset %x.\n"
			 "\tHSFSTS = 0x%08x\n",
	       failure_reason, flash_addr, ctx->hsfsts_on_last_error);
}

static int wait_for_hwseq_xfer(struct spi_ctx *ctx)
{
	uint32_t hsfsts;
	do {
		hsfsts = _spi_ctrlr_reg_read(ctx, SPIBAR_HSFSTS_CTL);

		if (hsfsts & SPIBAR_HSFSTS_FCERR) {
			ctx->hsfsts_on_last_error = hsfsts;
			return E_HW_ERROR;
		}
	/* TODO: set up timer and abort on timeout */
	} while (!(hsfsts & SPIBAR_HSFSTS_FDONE));

	return SUCCESS;
}

/* Execute SPI transfer. This is a blocking call. */
static int exec_sync_hwseq_xfer(struct spi_ctx *ctx, uint32_t hsfsts_cycle,
				 uint32_t flash_addr, size_t len)
{
	int ret;
	start_hwseq_xfer(ctx, hsfsts_cycle, flash_addr, len);
	ret = wait_for_hwseq_xfer(ctx);
	if (ret != SUCCESS) {
		const char *reason = (ret == E_TIMEOUT) ? "timeout" : "error";
		print_xfer_error(ctx, reason, flash_addr);
	}
	return ret;
}

unsigned int spi_crop_chunk(unsigned int cmd_len, unsigned int buf_len)
{
	return MIN(buf_len, SPIBAR_FDATA_FIFO_SIZE);
}

int spi_xfer(const struct spi_slave *slave, const void *dout,
	     size_t bytesout, void *din, size_t bytesin)
{
	printk(BIOS_DEBUG, "NOT IMPLEMENTED: %s() !!!\n", __func__);
	return E_NOT_IMPLEMENTED;
}

/*
 * Write-protection status for BIOS region (BIOS_CONTROL register):
 * EISS/WPD bits	00	01	10	11
 * 			--	--	--	--
 * normal mode		RO	RW	RO	RO
 * SMM mode		RO	RW	RO	RW
 */
void spi_init(void)
{
	uint32_t bios_ctl;

	BOILERPLATE_CREATE_CTX(ctx);

	bios_ctl = pci_read_config32(ctx->pci_dev, SPIBAR_BIOS_CONTROL);
	bios_ctl |= SPIBAR_BIOS_CONTROL_WPD;
	bios_ctl &= ~SPIBAR_BIOS_CONTROL_EISS;

	/* Enable Prefetching and caching. */
	bios_ctl |= SPIBAR_BIOS_CONTROL_PREFETCH_ENABLE;
	bios_ctl &= ~SPIBAR_BIOS_CONTROL_CACHE_DISABLE;

	pci_write_config32(ctx->pci_dev, SPIBAR_BIOS_CONTROL, bios_ctl);
}

int spi_claim_bus(const struct spi_slave *slave)
{
	/* There's nothing we need to to here. */
	return 0;
}

void spi_release_bus(const struct spi_slave *slave)
{
	/* No magic needed here. */
}

static int nuclear_spi_erase(const struct spi_flash *flash, uint32_t offset,
			     size_t len)
{
	int ret;
	size_t erase_size;
	uint32_t erase_cycle;

	BOILERPLATE_CREATE_CTX(ctx);

	if (!IS_ALIGNED(offset, 4 * KiB) || !IS_ALIGNED(len, 4 * KiB)) {
		printk(BIOS_ERR, "BUG! SPI erase region not sector aligned.\n");
		return E_ARGUMENT;
	}

	while (len) {
		if (IS_ALIGNED(offset, 64 * KiB) && (len >= 64 * KiB)) {
			erase_size = 64 * KiB;
			erase_cycle = SPIBAR_HSFSTS_CYCLE_64K_ERASE;
		} else {
			erase_size = 4 * KiB;
			erase_cycle = SPIBAR_HSFSTS_CYCLE_4K_ERASE;
		}
		printk(BIOS_SPEW, "Erasing flash addr %x + %zu KiB\n",
		       offset, erase_size / KiB);

		ret = exec_sync_hwseq_xfer(ctx, erase_cycle, offset, 0);
		if (ret != SUCCESS)
			return ret;

		offset += erase_size;
		len -= erase_size;
	}

	return SUCCESS;
}

/*
 * Ensure read/write xfer len is not greater than SPIBAR_FDATA_FIFO_SIZE and
 * that the operation does not cross 256-byte boundary.
 */
static size_t get_xfer_len(uint32_t addr, size_t len)
{
	size_t xfer_len = min(len, SPIBAR_FDATA_FIFO_SIZE);
	size_t bytes_left = ALIGN_UP(addr, 256) - addr;

	if (bytes_left)
		xfer_len = min(xfer_len, bytes_left);

	return xfer_len;
}

static int nuclear_spi_read(const struct spi_flash *flash, uint32_t addr,
			size_t len, void *buf)
{
	int ret;
	size_t xfer_len;
	uint8_t *data = buf;

	BOILERPLATE_CREATE_CTX(ctx);

	while (len) {
		xfer_len = get_xfer_len(addr, len);

		ret = exec_sync_hwseq_xfer(ctx, SPIBAR_HSFSTS_CYCLE_READ,
						addr, xfer_len);
		if (ret != SUCCESS)
			return ret;

		drain_xfer_fifo(ctx, data, xfer_len);

		addr += xfer_len;
		data += xfer_len;
		len -= xfer_len;
	}

	return SUCCESS;
}

static int nuclear_spi_write(const struct spi_flash *flash, uint32_t addr,
			size_t len, const void *buf)
{
	int ret;
	size_t xfer_len;
	const uint8_t *data = buf;

	BOILERPLATE_CREATE_CTX(ctx);

	while (len) {
		xfer_len = get_xfer_len(addr, len);
		fill_xfer_fifo(ctx, data, xfer_len);

		ret = exec_sync_hwseq_xfer(ctx, SPIBAR_HSFSTS_CYCLE_WRITE,
						addr, xfer_len);
		if (ret != SUCCESS)
			return ret;

		addr += xfer_len;
		data += xfer_len;
		len -= xfer_len;
	}

	return SUCCESS;
}

static int nuclear_spi_status(const struct spi_flash *flash, uint8_t *reg)
{
	int ret;
	BOILERPLATE_CREATE_CTX(ctx);

	ret = exec_sync_hwseq_xfer(ctx, SPIBAR_HSFSTS_CYCLE_RD_STATUS, 0,
				   sizeof(*reg));
	if (ret != SUCCESS)
		return ret;

	drain_xfer_fifo(ctx, reg, sizeof(*reg));
	return ret;
}

static struct spi_flash boot_flash CAR_GLOBAL;

/*
 * We can't use FDOC and FDOD to read FLCOMP, as previous platforms did.
 * For details see:
 * Ch 31, SPI: p. 194
 * The size of the flash component is always taken from density field in the
 * SFDP table. FLCOMP.C0DEN is no longer used by the Flash Controller.
 */
struct spi_flash *spi_flash_programmer_probe(struct spi_slave *spi, int force)
{
	BOILERPLATE_CREATE_CTX(ctx);
	struct spi_flash *flash;
	uint32_t flash_bits;

	flash = car_get_var_ptr(&boot_flash);

	/*
	 * bytes = (bits + 1) / 8;
	 * But we need to do the addition in a way which doesn't overflow for
	 * 4 Gbit devices (flash_bits == 0xffffffff).
	 */
	/* FIXME: Don't hardcode 0x04 ? */
	flash_bits = read_spi_sfdp_param(ctx, 0x04);
	flash->size = (flash_bits >> 3) + 1;

	flash->spi = spi;
	flash->name = "Apollolake hardware sequencer";

	/* Can erase both 4 KiB and 64 KiB chunks. Declare the smaller size. */
	flash->sector_size = 4 * KiB;
	/*
	 * FIXME: Get erase+cmd, and status_cmd from SFDP.
	 *
	 * flash->erase_cmd = ???
	 * flash->status_cmd = ???
	 */

	flash->internal_write = nuclear_spi_write;
	flash->internal_erase = nuclear_spi_erase;
	flash->internal_read = nuclear_spi_read;
	flash->internal_status = nuclear_spi_status;

	return flash;
}

int spi_setup_slave(unsigned int bus, unsigned int cs, struct spi_slave *slave)
{
	BOILERPLATE_CREATE_CTX(ctx);

	/* This is special hardware. We expect bus 0 and CS line 0 here. */
	if ((bus != 0) || (cs != 0))
		return -1;

	slave->bus = bus;
	slave->cs = cs;

	return 0;
}

int spi_read_status(uint8_t *status)
{
	BOILERPLATE_CREATE_CTX(ctx);

	if (exec_sync_hwseq_xfer(ctx, SPIBAR_HSFSTS_CYCLE_RD_STATUS, 0,
				 sizeof(*status)) != SUCCESS)
		return -1;

	drain_xfer_fifo(ctx, status, sizeof(*status));

	return 0;
}

int spi_flash_get_fpr_info(struct fpr_info *info)
{
	BOILERPLATE_CREATE_CTX(ctx);

	if (!ctx->mmio_base)
		return -1;

	info->base = ctx->mmio_base + SPIBAR_FPR_BASE;
	info->max = SPIBAR_FPR_MAX;

	return 0;
}
