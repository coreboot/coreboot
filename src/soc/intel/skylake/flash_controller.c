/*
 * Copyright (C) 2014 Google Inc.
 * Copyright (C) 2015 Intel Corporation.
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

/* This file is derived from the flashrom project. */
#include <arch/early_variables.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <bootstate.h>
#include <timer.h>
#include <soc/flash_controller.h>
#include <soc/intel/common/spi_flash.h>
#include <soc/pci_devs.h>
#include <soc/spi.h>
#include <spi-generic.h>

static inline uint16_t spi_read_hsfs(pch_spi_regs * const regs)
{
	return readw_(&regs->hsfs);
}

static inline void spi_clear_status(pch_spi_regs * const regs)
{
	/* clear FDONE, FCERR, AEL by writing 1 to them (if they are set) */
	writew_(spi_read_hsfs(regs), &regs->hsfs);
}

static inline uint16_t spi_read_hsfc(pch_spi_regs * const regs)
{
	return readw_(&regs->hsfc);
}

static inline uint32_t spi_read_faddr(pch_spi_regs * const regs)
{
	return readl_(&regs->faddr) & SPIBAR_FADDR_MASK;
}

/*
 * Polls for Cycle Done Status, Flash Cycle Error
 * Resets all error flags in HSFS.
 * Returns 0 if the cycle completes successfully without errors within
 * timeout, 1 on errors.
 */
static int wait_for_completion(pch_spi_regs * const regs, int timeout_ms,
				size_t len)
{
	uint16_t hsfs;
	uint16_t hsfc;
	uint32_t addr;
	struct stopwatch sw;
	int timeout = 0;

	stopwatch_init_msecs_expire(&sw, timeout_ms);

	do {
		hsfs = spi_read_hsfs(regs);

		if ((hsfs & (HSFS_FDONE | HSFS_FCERR)))
			break;
	} while (!(timeout = stopwatch_expired(&sw)));

	if (timeout) {
		addr = spi_read_faddr(regs);
		hsfc = spi_read_hsfc(regs);
		printk(BIOS_ERR, "%ld ms Transaction timeout between offset "
			"0x%08x and 0x%08zx (= 0x%08x + %zd) HSFC=%x HSFS=%x!\n",
			stopwatch_duration_msecs(&sw), addr, addr + len - 1,
			addr, len - 1, hsfc, hsfs);
		return 1;
	}

	if (hsfs & HSFS_FCERR) {
		addr = spi_read_faddr(regs);
		hsfc = spi_read_hsfc(regs);
		printk(BIOS_ERR, "Transaction error between offset 0x%08x and "
		       "0x%08zx (= 0x%08x + %zd) HSFC=%x HSFS=%x!\n",
		       addr, addr + len - 1, addr, len - 1,
		       hsfc, hsfs);
		return 1;
	}

	return 0;
}

/* Start operation returning 0 on success, non-zero on error or timeout. */
static int spi_do_operation(int op, size_t offset, size_t size, int timeout_ms)
{
	uint16_t hsfc;
	pch_spi_regs * const regs = get_spi_bar();

	/* Clear status prior to operation. */
	spi_clear_status(regs);

	/* Set the FADDR */
	writel_(offset & SPIBAR_FADDR_MASK, &regs->faddr);

	hsfc = readw_(&regs->hsfc);
	/* Clear then set the correct op. */
	hsfc &= ~HSFC_FCYCLE_MASK;
	hsfc |= op;
	/* Set the size field */
	hsfc &= ~HSFC_FDBC_MASK;
	/* Check for sizes of confirming operations. */
	if (size && size <= SPI_FDATA_BYTES)
		hsfc |= ((size - 1) << HSFC_FDBC_SHIFT) & HSFC_FDBC_MASK;
	/* start operation */
	hsfc |= HSFC_FGO;
	writew_(hsfc, &regs->hsfc);

	return wait_for_completion(regs, timeout_ms, size);
}

unsigned int spi_crop_chunk(unsigned int cmd_len, unsigned int buf_len)
{
	return min(SPI_FDATA_BYTES, buf_len);
}

static size_t spi_get_flash_size(pch_spi_regs *spi_bar)
{
	uint32_t flcomp;
	size_t size;

	writel_(SPIBAR_FDOC_COMPONENT, &spi_bar->fdoc);
	flcomp = readl_(&spi_bar->fdod);

	switch (flcomp & FLCOMP_C0DEN_MASK) {
	case FLCOMP_C0DEN_8MB:
		size = 8*MiB;
		break;
	case FLCOMP_C0DEN_16MB:
		size = 16*MiB;
		break;
	case FLCOMP_C0DEN_32MB:
		size = 32*MiB;
		break;
	default:
		size = 16*MiB;
	}

	return size;
}

void spi_init(void)
{
	uint8_t bios_cntl;
	device_t dev = PCH_DEV_SPI;

	/* Disable the BIOS write protect so write commands are allowed. */
	pci_read_config_byte(dev, SPIBAR_BIOS_CNTL, &bios_cntl);
	bios_cntl &= ~SPIBAR_BC_EISS;
	bios_cntl |= SPIBAR_BC_WPD;
	pci_write_config_byte(dev, SPIBAR_BIOS_CNTL, bios_cntl);
}

int pch_hwseq_erase(const struct spi_flash *flash, u32 offset, size_t len)
{
	u32 start, end, erase_size;
	int ret = 0;

	erase_size = flash->sector_size;
	if (offset % erase_size || len % erase_size) {
		printk(BIOS_ERR, "SF: Erase offset/length not multiple of erase size\n");
		return -1;
	}

	start = offset;
	end = start + len;

	while (offset < end) {
		if (spi_do_operation(HSFC_FCYCLE_4KE, offset, 0, 5000)) {
			printk(BIOS_ERR, "SF: Erase failed at %x\n", offset);
			ret = -1;
			goto out;
		}

		offset += erase_size;
	}

	printk(BIOS_DEBUG, "SF: Successfully erased %zu bytes @ %#x\n",
		len, start);

out:
	spi_release_bus(flash->spi);
	return ret;
}

static void pch_read_data(uint8_t *data, int len)
{
	int i;
	pch_spi_regs *spi_bar;
	uint32_t temp32 = 0;

	spi_bar = get_spi_bar();

	for (i = 0; i < len; i++) {
		if ((i % 4) == 0)
			temp32 = readl_((uint8_t *)spi_bar->fdata + i);

		data[i] = (temp32 >> ((i % 4) * 8)) & 0xff;
	}
}

int pch_hwseq_read(const struct spi_flash *flash, u32 addr, size_t len,
		void *buf)
{
	uint8_t block_len;

	if (addr + len > spi_get_flash_size(get_spi_bar())) {
		printk(BIOS_ERR,
			"Attempt to read %x-%x which is out of chip\n",
			(unsigned) addr,
			(unsigned) addr+(unsigned) len);
		return -1;
	}

	while (len > 0) {
		const int timeout_ms = 6;

		block_len = min(len, SPI_FDATA_BYTES);
		if (block_len > (~addr & 0xff))
			block_len = (~addr & 0xff) + 1;

		if (spi_do_operation(HSFC_FCYCLE_RD, addr, block_len,
					timeout_ms))
			return -1;

		pch_read_data(buf, block_len);
		addr += block_len;
		buf += block_len;
		len -= block_len;
	}
	return 0;
}

/* Fill len bytes from the data array into the fdata/spid registers.
 *
 * Note that using len > flash->pgm->spi.max_data_write will trash the registers
 * following the data registers.
 */
static void pch_fill_data(const uint8_t *data, int len)
{
	uint32_t temp32 = 0;
	int i;
	pch_spi_regs *spi_bar;

	spi_bar = get_spi_bar();
	if (len <= 0)
		return;

	for (i = 0; i < len; i++) {
		if ((i % 4) == 0)
			temp32 = 0;

		temp32 |= ((uint32_t) data[i]) << ((i % 4) * 8);

		if ((i % 4) == 3) /* 32 bits are full, write them to regs. */
			writel_(temp32,
				(uint8_t *)spi_bar->fdata + (i - (i % 4)));
	}
	i--;
	if ((i % 4) != 3) /* Write remaining data to regs. */
		writel_(temp32, (uint8_t *)spi_bar->fdata + (i - (i % 4)));
}

int pch_hwseq_write(const struct spi_flash *flash, u32 addr, size_t len,
		const void *buf)
{
	uint8_t block_len;
	uint32_t start = addr;
	pch_spi_regs *spi_bar;

	spi_bar = get_spi_bar();

	if (addr + len > spi_get_flash_size(spi_bar)) {
		printk(BIOS_ERR,
			"Attempt to write 0x%x-0x%x which is out of chip\n",
			(unsigned)addr, (unsigned) (addr+len));
		return -1;
	}

	while (len > 0) {
		const int timeout_ms = 6;

		block_len = min(len, sizeof(spi_bar->fdata));
		if (block_len > (~addr & 0xff))
			block_len = (~addr & 0xff) + 1;

		pch_fill_data(buf, block_len);
		if (spi_do_operation(HSFC_FCYCLE_WR, addr, block_len,
					timeout_ms)) {
			printk(BIOS_ERR, "SF: write failure at %x\n", addr);
			return -1;
		}
		addr += block_len;
		buf += block_len;
		len -= block_len;
	}
	printk(BIOS_DEBUG, "SF: Successfully written %u bytes @ %#x\n",
	       (unsigned) (addr - start), start);
	return 0;
}

int pch_hwseq_read_status(const struct spi_flash *flash, u8 *reg)
{
	size_t block_len = SPI_READ_STATUS_LENGTH;
	const int timeout_ms = 6;

	if (spi_do_operation(HSFC_FCYCLE_RS, 0, block_len, timeout_ms))
		return -1;

	pch_read_data(reg, block_len);

	return 0;
}

static struct spi_flash boot_flash CAR_GLOBAL;

struct spi_flash *spi_flash_programmer_probe(struct spi_slave *spi, int force)
{
	struct spi_flash *flash;

	flash = car_get_var_ptr(&boot_flash);

	/* Ensure writes can take place to the flash. */
	spi_init();

	flash->spi = spi;
	flash->name = "Opaque HW-sequencing";

	flash->internal_write = pch_hwseq_write;
	flash->internal_erase = pch_hwseq_erase;
	flash->internal_read = pch_hwseq_read;
	flash->internal_status = pch_hwseq_read_status;

	/* The hardware sequencing supports 4KiB or 64KiB erase. Use 4KiB. */
	flash->sector_size = 4*KiB;

	flash->size = spi_get_flash_size(get_spi_bar());

	return flash;
}

int spi_setup_slave(unsigned int bus, unsigned int cs, struct spi_slave *slave)
{
	/* This is special hardware. We expect bus 0 and CS line 0 here. */
	if ((bus != 0) || (cs != 0))
		return -1;

	slave->bus = bus;
	slave->cs = cs;
	slave->ctrlr = NULL;

	return 0;
}

int spi_flash_get_fpr_info(struct fpr_info *info)
{
	pch_spi_regs *spi_bar = get_spi_bar();

	if (!spi_bar)
		return -1;

	info->base = (uintptr_t)&spi_bar->pr[0];
	info->max = SPI_FPR_MAX;

	return 0;
}

#if ENV_RAMSTAGE
/*
 * spi_init() needs run unconditionally in every boot (including resume) to
 * allow write protect to be disabled for eventlog and firmware updates.
 */
static void spi_init_cb(void *unused)
{
	spi_init();
}

BOOT_STATE_INIT_ENTRY(BS_PRE_DEVICE, BS_ON_ENTRY, spi_init_cb, NULL);
#endif
