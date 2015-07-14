/*
 * Copyright (C) 2014 Google Inc.
 * Copyright (C) 2015 Intel Corporation.
 *
 * See file CREDITS for list of people who contributed to this
 * project.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc.
 */

/* This file is derived from the flashrom project. */
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <bootstate.h>
#include <delay.h>
#include <device/pci_ids.h>
#include <flash_controller.h>
#include <spi_flash.h>
#include <spi-generic.h>
#include <soc/pci_devs.h>
#include <soc/spi.h>

#if !(ENV_ROMSTAGE)
typedef struct spi_slave pch_spi_slave;
static struct spi_flash *spi_flash_hwseq_probe(struct spi_slave *spi);
#endif

unsigned int spi_crop_chunk(unsigned int cmd_len, unsigned int buf_len)
{
	pch_spi_regs *spi_bar;

	spi_bar = get_spi_bar();
	return min(sizeof(spi_bar->fdata), buf_len);
}

#if !(ENV_ROMSTAGE)
struct spi_slave *spi_setup_slave(unsigned int bus, unsigned int cs)
{
	pch_spi_slave *slave = malloc(sizeof(*slave));

	if (!slave) {
		printk(BIOS_DEBUG, "PCH SPI: Bad allocation\n");
		return NULL;
	}

	memset(slave, 0, sizeof(*slave));

	slave->bus = bus;
	slave->cs = cs;
	slave->force_programmer_specific = 1;
	slave->programmer_specific_probe = spi_flash_hwseq_probe;

	return slave;
}
#endif

static u32 spi_get_flash_size(pch_spi_regs *spi_bar)
{
	uint32_t flcomp;
	u32 size;

	writel_(SPIBAR_FDOC_COMPONENT, &spi_bar->fdoc);
	flcomp = readl_(&spi_bar->fdod);
	printk(BIOS_DEBUG, "flcomp = %x\n", flcomp);

	switch (flcomp & FLCOMP_C0DEN_MASK) {
	case FLCOMP_C0DEN_8MB:
		size = 0x100000;
		break;
	case FLCOMP_C0DEN_16MB:
		size = 0x1000000;
		break;
	case FLCOMP_C0DEN_32MB:
		size = 0x10000000;
		break;
	default:
		size = 0x1000000;
	}

	printk(BIOS_DEBUG, "flash size 0x%x bytes\n", size);

	return size;
}

int spi_xfer(struct spi_slave *slave, const void *dout,
		unsigned int bytesout, void *din, unsigned int bytesin)
{
	/* TODO: Define xfer for hardware sequencing. */
	return -1;
}

void spi_init(void)
{
	uint8_t bios_cntl;
	device_t dev = PCH_DEV_SPI;
	pch_spi_regs *spi_bar;
	uint16_t hsfs;

	/* Root Complex Register Block */
	spi_bar = get_spi_bar();
	hsfs = readw_(&spi_bar->hsfs);
	if (hsfs & HSFS_FDV) {
		/* Select Flash Descriptor Section Index to 1 */
		writel_(SPIBAR_FDOC_FDSI_1, &spi_bar->fdoc);
	}

	/* Disable the BIOS write protect so write commands are allowed. */
	pci_read_config_byte(dev, SPIBAR_BIOS_CNTL, &bios_cntl);
	bios_cntl &= ~SPIBAR_BC_EISS;
	bios_cntl |= SPIBAR_BC_WPD;
	pci_write_config_byte(dev, SPIBAR_BIOS_CNTL, bios_cntl);
}

int spi_claim_bus(struct spi_slave *slave)
{
	/* Handled by PCH automatically. */
	return 0;
}

void spi_release_bus(struct spi_slave *slave)
{
	/* Handled by PCH automatically. */
}

static void pch_hwseq_set_addr(uint32_t addr, pch_spi_regs *spi_bar)
{
	uint32_t addr_old = readl_(&spi_bar->faddr) & ~SPIBAR_FADDR_MASK;
	writel_((addr & SPIBAR_FADDR_MASK) | addr_old, &spi_bar->faddr);
}

/*
 * Polls for Cycle Done Status, Flash Cycle Error or timeout in 8 us intervals.
 * Resets all error flags in HSFS.
 * Returns 0 if the cycle completes successfully without errors within
 * timeout us, 1 on errors.
 */
static int pch_hwseq_wait_for_cycle_complete(unsigned int timeout,
			unsigned int len, pch_spi_regs *spi_bar)
{
	uint16_t hsfs;
	uint32_t addr;

	timeout /= 8; /* scale timeout duration to counter */
	while ((((hsfs = readw_(&spi_bar->hsfs)) &
		 (HSFS_FDONE | HSFS_FCERR)) == 0) && --timeout) {
		udelay(8);
	}
	writew_(readw_(&spi_bar->hsfs), &spi_bar->hsfs);

	if (!timeout) {
		uint16_t hsfc;
		addr = readl_(&spi_bar->faddr) & SPIBAR_FADDR_MASK;
		hsfc = readw_(&spi_bar->hsfc);
		printk(BIOS_ERR, "Transaction timeout between offset 0x%08x \
			and 0x%08x (= 0x%08x + %d) HSFC=%x HSFS=%x!\n",
			addr, addr + len - 1, addr, len - 1,
			hsfc, hsfs);
		return 1;
	}

	if (hsfs & HSFS_FCERR) {
		uint16_t hsfc;
		addr = readl_(&spi_bar->faddr) & SPIBAR_FADDR_MASK;
		hsfc = readw_(&spi_bar->hsfc);
		printk(BIOS_ERR, "Transaction error between offset 0x%08x and \
		       0x%08x (= 0x%08x + %d) HSFC=%x HSFS=%x!\n",
		       addr, addr + len - 1, addr, len - 1,
		       hsfc, hsfs);
		return 1;
	}
	return 0;
}

int pch_hwseq_erase(struct spi_flash *flash, u32 offset, size_t len)
{
	u32 start, end, erase_size;
	int ret;
	uint16_t hsfc;
	uint32_t timeout = 5000 * 1000; /* 5 s for max 64 kB */
	pch_spi_regs *spi_bar;

	spi_bar = get_spi_bar();
	erase_size = flash->sector_size;
	if (offset % erase_size || len % erase_size) {
		printk(BIOS_ERR, "SF: Erase offset/length not multiple of erase size\n");
		return -1;
	}

	flash->spi->rw = SPI_WRITE_FLAG;
	ret = spi_claim_bus(flash->spi);
	if (ret) {
		printk(BIOS_ERR, "SF: Unable to claim SPI bus\n");
		return ret;
	}

	start = offset;
	end = start + len;

	while (offset < end) {
		/*
		 * Make sure FDONE, FCERR, AEL are
		 * cleared by writing 1 to them.
		 */
		writew_(readw_(&spi_bar->hsfs), &spi_bar->hsfs);

		pch_hwseq_set_addr(offset, spi_bar);

		offset += erase_size;

		hsfc = readw_(&spi_bar->hsfc);
		hsfc &= ~HSFC_FCYCLE; /* clear operation */
		hsfc |= HSFC_FCYCLE; /* set erase operation */
		hsfc |= HSFC_FGO; /* start */
		writew_(hsfc, &spi_bar->hsfc);
		if (pch_hwseq_wait_for_cycle_complete(timeout, len, spi_bar)) {
			printk(BIOS_ERR, "SF: Erase failed at %x\n",
				offset - erase_size);
			ret = -1;
			goto out;
		}
	}

	printk(BIOS_DEBUG, "SF: Successfully erased %zu bytes @ %#x\n",
		len, start);

out:
	spi_release_bus(flash->spi);
	return ret;
}

static void pch_read_data(uint8_t *data, int len, pch_spi_regs *spi_bar)
{
	int i;
	uint32_t temp32 = 0;

	for (i = 0; i < len; i++) {
		if ((i % 4) == 0)
			temp32 = readl_((uint8_t *)spi_bar->fdata + i);

		data[i] = (temp32 >> ((i % 4) * 8)) & 0xff;
	}
}
int pch_hwseq_read(struct spi_flash *flash,
			  u32 addr, size_t len, void *buf)
{
	uint16_t hsfc;
	uint16_t timeout = 100 * 60;  /* 6 mili secs timeout */
	uint8_t block_len;
	pch_spi_regs *spi_bar;

	spi_bar = get_spi_bar();
	if (addr + len > spi_get_flash_size(spi_bar)) {
		printk(BIOS_ERR,
			"Attempt to read %x-%x which is out of chip\n",
			(unsigned) addr,
			(unsigned) addr+(unsigned) len);
		return -1;
	}

	/* clear FDONE, FCERR, AEL by writing 1 to them (if they are set) */
	writew_(readw_(&spi_bar->hsfs), &spi_bar->hsfs);

	while (len > 0) {
		block_len = min(len, sizeof(spi_bar->fdata));
		if (block_len > (~addr & 0xff))
			block_len = (~addr & 0xff) + 1;
		pch_hwseq_set_addr(addr, spi_bar);
		hsfc = readw_(&spi_bar->hsfc);
		hsfc &= ~HSFC_FCYCLE; /* set read operation */
		hsfc &= ~HSFC_FDBC; /* clear byte count */
		/* set byte count */
		hsfc |= (((block_len - 1) << HSFC_FDBC_SHIFT) & HSFC_FDBC);
		hsfc |= HSFC_FGO; /* start */
		writew_(hsfc, &spi_bar->hsfc);

		if (pch_hwseq_wait_for_cycle_complete
			(timeout, block_len, spi_bar))
			return -1;
		pch_read_data(buf, block_len, spi_bar);
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

int pch_hwseq_write(struct spi_flash *flash,
			   u32 addr, size_t len, const void *buf)
{
	uint16_t hsfc;
	uint16_t timeout = 100 * 60;   /* 6 mili secs  timeout */
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

	/* clear FDONE, FCERR, AEL by writing 1 to them (if they are set) */
	writew_(readw_(&spi_bar->hsfs), &spi_bar->hsfs);

	while (len > 0) {
		block_len = min(len, sizeof(spi_bar->fdata));
		if (block_len > (~addr & 0xff))
			block_len = (~addr & 0xff) + 1;

		pch_hwseq_set_addr(addr, spi_bar);

		pch_fill_data(buf, block_len);
		hsfc = readw_(&spi_bar->hsfc);
		hsfc &= ~HSFC_FCYCLE; /* clear operation */
		hsfc |= HSFC_FCYCLE_WR; /* set write operation */
		hsfc &= ~HSFC_FDBC; /* clear byte count */
		/* set byte count */
		hsfc |= (((block_len - 1) << HSFC_FDBC_SHIFT) & HSFC_FDBC);
		hsfc |= HSFC_FGO; /* start */
		writew_(hsfc, &spi_bar->hsfc);

		if (pch_hwseq_wait_for_cycle_complete
			(timeout, block_len, spi_bar)) {
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

int pch_hwseq_read_status(struct spi_flash *flash, u8 *reg)
{
	uint16_t hsfc;
	uint16_t timeout = 100 * 60;   /* 6 mili secs timeout */
	uint8_t block_len = SPI_READ_STATUS_LENGTH;
	pch_spi_regs *spi_bar;

	spi_bar = get_spi_bar();
	/* clear FDONE, FCERR, AEL by writing 1 to them (if they are set) */
	writew_(readw_(&spi_bar->hsfs), &spi_bar->hsfs);

	hsfc = readw_(&spi_bar->hsfc);
	hsfc &= ~HSFC_FCYCLE; /* set read operation */
	/* read status register */
	hsfc |= HSFC_FCYCLE_RS;
	hsfc &= ~HSFC_FDBC; /* clear byte count */
	/* set byte count */
	hsfc |= (((block_len - 1) << HSFC_FDBC_SHIFT) & HSFC_FDBC);
	hsfc |= HSFC_FGO; /* start */
	writew_(hsfc, &spi_bar->hsfc);

	if (pch_hwseq_wait_for_cycle_complete(timeout,
			block_len, spi_bar))
		return -1;
	pch_read_data(reg, block_len, spi_bar);
	/* clear read status register */
	writew_(readw_(&spi_bar->hsfc) &
			~HSFC_FCYCLE_RS, &spi_bar->hsfc);

	return 0;
}

#if !(ENV_ROMSTAGE)
static struct spi_flash *spi_flash_hwseq_probe(struct spi_slave *spi)
{
	struct spi_flash *flash = NULL;
	u32 berase;
	pch_spi_regs *spi_bar;

	spi_bar = get_spi_bar();
	flash = malloc(sizeof(*flash));
	if (!flash) {
		printk(BIOS_WARNING, "SF: Failed to allocate memory\n");
		return NULL;
	}

	flash->spi = spi;
	flash->name = "Opaque HW-sequencing";

	flash->write = pch_hwseq_write;
	flash->erase = pch_hwseq_erase;
	flash->read = pch_hwseq_read;
	flash->status = pch_hwseq_read_status;
	pch_hwseq_set_addr(0, spi_bar);

	berase = ((readw_(&spi_bar->hsfs)) >> SPIBAR_HSFS_BERASE_OFFSET) &
		  SPIBAR_HSFS_BERASE_MASK;

	switch (berase) {
	case 0:
		flash->sector_size = 256;
		break;
	case 1:
		flash->sector_size = 4096;
		break;
	case 2:
		flash->sector_size = 8192;
		break;
	case 3:
		flash->sector_size = 65536;
		break;
	}

	flash->size = spi_get_flash_size(spi_bar);

	return flash;
}
#endif

