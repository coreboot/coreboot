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
#include <arch/io.h>
#include <console/console.h>
#include <device/pci_ids.h>
#include <spi_flash.h>
#include <spi-generic.h>
#include <soc/pci_devs.h>
#include <soc/spi.h>

#define HSFC_FCYCLE_OFF		1	/* 1-2: FLASH Cycle */
#define HSFC_FCYCLE		(0x3 << HSFC_FCYCLE_OFF)
#define HSFC_FCYCLE_WR		(0x2 << HSFC_FCYCLE_OFF)
#define HSFC_FDBC_OFF		8	/* 8-13: Flash Data Byte Count */
#define HSFC_FDBC		(0x3f << HSFC_FDBC_OFF)

#if ENV_SMM
#define pci_read_config_byte(dev, reg, targ)\
	(*(targ) = pci_read_config8(dev, reg))
#define pci_read_config_word(dev, reg, targ)\
	(*(targ) = pci_read_config16(dev, reg))
#define pci_read_config_dword(dev, reg, targ)\
	(*(targ) = pci_read_config32(dev, reg))
#define pci_write_config_byte(dev, reg, val)\
	pci_write_config8(dev, reg, val)
#define pci_write_config_word(dev, reg, val)\
	pci_write_config16(dev, reg, val)
#define pci_write_config_dword(dev, reg, val)\
	pci_write_config32(dev, reg, val)
#else /* !ENV_SMM */
#include <device/device.h>
#include <device/pci.h>
#define pci_read_config_byte(dev, reg, targ)\
	(*(targ) = pci_read_config8(dev, reg))
#define pci_read_config_word(dev, reg, targ)\
	(*(targ) = pci_read_config16(dev, reg))
#define pci_read_config_dword(dev, reg, targ)\
	(*(targ) = pci_read_config32(dev, reg))
#define pci_write_config_byte(dev, reg, val)\
	pci_write_config8(dev, reg, val)
#define pci_write_config_word(dev, reg, val)\
	pci_write_config16(dev, reg, val)
#define pci_write_config_dword(dev, reg, val)\
	pci_write_config32(dev, reg, val)
#endif /* ENV_SMM */

#define B_PCH_SPI_BAR0_MASK   0x0FFF

typedef struct spi_slave pch_spi_slave;
static struct spi_flash *spi_flash_hwseq_probe(struct spi_slave *spi);
static int pch_hwseq_write(struct spi_flash *flash,
			   u32 addr, size_t len, const void *buf);
static int pch_hwseq_read(struct spi_flash *flash,
			  u32 addr, size_t len, void *buf);

typedef struct pch_spi_regs {
	uint32_t bfpr;
	uint16_t hsfs;
	uint16_t hsfc;
	uint32_t faddr;
	uint32_t _reserved0;
	uint32_t fdata[16];
	uint32_t frap;
	uint32_t freg[6];
	uint32_t _reserved1[6];
	uint32_t pr[5];
	uint32_t _reserved2[2];
	uint8_t ssfs;
	uint8_t ssfc[3];
	uint16_t preop;
	uint16_t optype;
	uint8_t opmenu[8];
	uint32_t bbar;
	uint32_t  fdoc;
	uint32_t fdod;
	uint8_t _reserved4[8];
	uint32_t afc;
	uint32_t lvscc;
	uint32_t uvscc;
	uint8_t _reserved5[4];
	uint32_t fpb;
	uint8_t _reserved6[28];
	uint32_t srdl;
	uint32_t srdc;
	uint32_t srd;
} __attribute__((packed)) pch_spi_regs;

typedef struct pch_spi_controller {
	int locked;
	uint32_t flmap0;
	uint32_t hsfs;
	pch_spi_regs *pch_spi;
	uint8_t *opmenu;
	int menubytes;
	uint16_t *preop;
	uint16_t *optype;
	uint32_t *addr;
	uint8_t *data;
	unsigned databytes;
	uint8_t *status;
	uint16_t *control;
	uint32_t *bbar;
} pch_spi_controller;

static pch_spi_controller cntlr;

enum {
	HSFS_FDONE =		0x0001,
	HSFS_FCERR =		0x0002,
	HSFS_AEL =		0x0004,
	HSFS_BERASE_MASK =	0x0018,
	HSFS_BERASE_SHIFT =	3,
	HSFS_SCIP =		0x0020,
	HSFS_FDOPSS =		0x2000,
	HSFS_FDV =		0x4000,
	HSFS_FLOCKDN =		0x8000
};

enum {
	HSFC_FGO =		0x0001,
	HSFC_FCYCLE_MASK =	0x0006,
	HSFC_FCYCLE_SHIFT =	1,
	HSFC_FDBC_MASK =	0x3f00,
	HSFC_FDBC_SHIFT =	8,
	HSFC_FSMIE =		0x8000
};

#if IS_ENABLED(CONFIG_DEBUG_SPI_FLASH)

static u8 readb_(const void *addr)
{
	u8 v = read8(addr);
	printk(BIOS_DEBUG, "read %2.2x from %4.4x\n",
	       v, ((unsigned) addr & 0xffff) - 0xf020);
	return v;
}

static u16 readw_(const void *addr)
{
	u16 v = read16(addr);
	printk(BIOS_DEBUG, "read %4.4x from %4.4x\n",
	       v, ((unsigned) addr & 0xffff) - 0xf020);
	return v;
}

static u32 readl_(const void *addr)
{
	u32 v = read32(addr);
	printk(BIOS_DEBUG, "read %8.8x from %4.4x\n",
	       v, ((unsigned) addr & 0xffff) - 0xf020);
	return v;
}

static void writeb_(u8 b, void *addr)
{
	write8(addr, b);
	printk(BIOS_DEBUG, "wrote %2.2x to %4.4x\n",
	       b, ((unsigned) addr & 0xffff) - 0xf020);
}

static void writew_(u16 b, void *addr)
{
	write16(addr, b);
	printk(BIOS_DEBUG, "wrote %4.4x to %4.4x\n",
	       b, ((unsigned) addr & 0xffff) - 0xf020);
}

static void writel_(u32 b, void *addr)
{
	write32(addr, b);
	printk(BIOS_DEBUG, "wrote %8.8x to %4.4x\n",
	       b, ((unsigned) addr & 0xffff) - 0xf020);
}

#else /* CONFIG_DEBUG_SPI_FLASH ^^^ enabled  vvv NOT enabled */

#define readb_(a) read8(a)
#define readw_(a) read16(a)
#define readl_(a) read32(a)
#define writeb_(val, addr) write8(addr, val)
#define writew_(val, addr) write16(addr, val)
#define writel_(val, addr) write32(addr, val)

#endif  /* CONFIG_DEBUG_SPI_FLASH ^^^ NOT enabled */

static void pch_set_bbar(uint32_t minaddr)
{
	uint32_t pchspi_bbar;

	minaddr &= SPIBAR_MEMBAR_MASK;
	pchspi_bbar = readl_(cntlr.bbar) & ~SPIBAR_MEMBAR_MASK;
	pchspi_bbar |= minaddr;
	writel_(pchspi_bbar, cntlr.bbar);
}

unsigned int spi_crop_chunk(unsigned int cmd_len, unsigned int buf_len)
{
	return min(cntlr.databytes, buf_len);
}

struct spi_slave *spi_setup_slave(unsigned int bus, unsigned int cs)
{
	pch_spi_slave *slave = malloc(sizeof(*slave));

	if (!slave) {
		printk(BIOS_DEBUG, "ICH SPI: Bad allocation\n");
		return NULL;
	}

	memset(slave, 0, sizeof(*slave));

	slave->bus = bus;
	slave->cs = cs;
	slave->force_programmer_specific = 1;
	slave->programmer_specific_probe = spi_flash_hwseq_probe;

	return slave;
}

static u32 spi_get_flash_size(void)
{
	uint32_t flcomp;
	u32 size;

	writel_(SPIBAR_FDOC_COMPONENT, &cntlr.pch_spi->fdoc);
	flcomp = readl_(&cntlr.pch_spi->fdod);
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
	pch_spi_regs *pch_spi;
	uint16_t hsfs;

	/* Root Complex Register Block */
	pch_spi = (pch_spi_regs *)(get_spi_bar());
	cntlr.pch_spi = pch_spi;
	hsfs = readw_(&pch_spi->hsfs);
	cntlr.hsfs = hsfs;
	cntlr.opmenu = pch_spi->opmenu;
	cntlr.menubytes = sizeof(pch_spi->opmenu);
	cntlr.optype = &pch_spi->optype;
	cntlr.addr = &pch_spi->faddr;
	cntlr.data = (uint8_t *)pch_spi->fdata;
	cntlr.databytes = sizeof(pch_spi->fdata);
	cntlr.status = &pch_spi->ssfs;
	cntlr.control = (uint16_t *)pch_spi->ssfc;
	cntlr.bbar = &pch_spi->bbar;
	cntlr.preop = &pch_spi->preop;

	if (cntlr.hsfs & HSFS_FDV) {
		/* Select Flash Descriptor Section Index to 1 */
		writel_(SPIBAR_FDOC_FDSI_1, &pch_spi->fdoc);
		cntlr.flmap0 = readl_(&pch_spi->fdod);
	}

	pch_set_bbar(0);

	/* Disable the BIOS write protect so write commands are allowed. */
	pci_read_config_byte(dev, SPIBAR_BIOS_CNTL, &bios_cntl);
	bios_cntl &= ~SPIBAR_BC_EISS;
	bios_cntl |= SPIBAR_BC_WPD;
	pci_write_config_byte(dev, SPIBAR_BIOS_CNTL, bios_cntl);
}

#if ENV_RAMSTAGE

static void spi_init_cb(void *unused)
{
	spi_init();
}

BOOT_STATE_INIT_ENTRY(BS_DEV_INIT, BS_ON_ENTRY, spi_init_cb, NULL);

#endif /* ENV_RAMSTAGE */

int spi_claim_bus(struct spi_slave *slave)
{
	/* Handled by PCH automatically. */
	return 0;
}

void spi_release_bus(struct spi_slave *slave)
{
	/* Handled by PCH automatically. */
}

static void pch_hwseq_set_addr(uint32_t addr)
{
	uint32_t addr_old = readl_(&cntlr.pch_spi->faddr) & ~SPIBAR_FADDR_MASK;
	writel_((addr & SPIBAR_FADDR_MASK) | addr_old, &cntlr.pch_spi->faddr);
}

/*
 * Polls for Cycle Done Status, Flash Cycle Error or timeout in 8 us intervals.
 * Resets all error flags in HSFS.
 * Returns 0 if the cycle completes successfully without errors within
 * timeout us, 1 on errors.
 */
static int pch_hwseq_wait_for_cycle_complete(unsigned int timeout,
					     unsigned int len)
{
	uint16_t hsfs;
	uint32_t addr;

	timeout /= 8; /* scale timeout duration to counter */
	while ((((hsfs = readw_(&cntlr.pch_spi->hsfs)) &
		 (HSFS_FDONE | HSFS_FCERR)) == 0) && --timeout) {
		udelay(8);
	}
	writew_(readw_(&cntlr.pch_spi->hsfs), &cntlr.pch_spi->hsfs);

	if (!timeout) {
		uint16_t hsfc;
		addr = readl_(&cntlr.pch_spi->faddr) & SPIBAR_FADDR_MASK;
		hsfc = readw_(&cntlr.pch_spi->hsfc);
		printk(BIOS_ERR, "Transaction timeout between offset 0x%08x \
			and 0x%08x (= 0x%08x + %d) HSFC=%x HSFS=%x!\n",
			addr, addr + len - 1, addr, len - 1,
			hsfc, hsfs);
		return 1;
	}

	if (hsfs & HSFS_FCERR) {
		uint16_t hsfc;
		addr = readl_(&cntlr.pch_spi->faddr) & SPIBAR_FADDR_MASK;
		hsfc = readw_(&cntlr.pch_spi->hsfc);
		printk(BIOS_ERR, "Transaction error between offset 0x%08x and \
		       0x%08x (= 0x%08x + %d) HSFC=%x HSFS=%x!\n",
		       addr, addr + len - 1, addr, len - 1,
		       hsfc, hsfs);
		return 1;
	}
	return 0;
}


static int pch_hwseq_erase(struct spi_flash *flash, u32 offset, size_t len)
{
	u32 start, end, erase_size;
	int ret;
	uint16_t hsfc;
	uint16_t timeout = 1000 * 60;

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
		writew_(readw_(&cntlr.pch_spi->hsfs), &cntlr.pch_spi->hsfs);

		pch_hwseq_set_addr(offset);

		offset += erase_size;

		hsfc = readw_(&cntlr.pch_spi->hsfc);
		hsfc &= ~HSFC_FCYCLE; /* clear operation */
		hsfc |= HSFC_FCYCLE; /* set erase operation */
		hsfc |= HSFC_FGO; /* start */
		writew_(hsfc, &cntlr.pch_spi->hsfc);
		if (pch_hwseq_wait_for_cycle_complete(timeout, len)) {
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

static void pch_read_data(uint8_t *data, int len)
{
	int i;
	uint32_t temp32 = 0;

	for (i = 0; i < len; i++) {
		if ((i % 4) == 0)
			temp32 = readl_(cntlr.data + i);

		data[i] = (temp32 >> ((i % 4) * 8)) & 0xff;
	}
}

static int pch_hwseq_read(struct spi_flash *flash,
			  u32 addr, size_t len, void *buf)
{
	uint16_t hsfc;
	uint16_t timeout = 100 * 60;
	uint8_t block_len;

	if (addr + len > spi_get_flash_size()) {
		printk(BIOS_ERR,
			"Attempt to read %x-%x which is out of chip\n",
			(unsigned) addr,
			(unsigned) addr+(unsigned) len);
		return -1;
	}

	/* clear FDONE, FCERR, AEL by writing 1 to them (if they are set) */
	writew_(readw_(&cntlr.pch_spi->hsfs), &cntlr.pch_spi->hsfs);

	while (len > 0) {
		block_len = min(len, cntlr.databytes);
		if (block_len > (~addr & 0xff))
			block_len = (~addr & 0xff) + 1;
		pch_hwseq_set_addr(addr);
		hsfc = readw_(&cntlr.pch_spi->hsfc);
		hsfc &= ~HSFC_FCYCLE; /* set read operation */
		hsfc &= ~HSFC_FDBC; /* clear byte count */
		/* set byte count */
		hsfc |= (((block_len - 1) << HSFC_FDBC_OFF) & HSFC_FDBC);
		hsfc |= HSFC_FGO; /* start */
		writew_(hsfc, &cntlr.pch_spi->hsfc);

		if (pch_hwseq_wait_for_cycle_complete(timeout, block_len))
			return 1;
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

	if (len <= 0)
		return;

	for (i = 0; i < len; i++) {
		if ((i % 4) == 0)
			temp32 = 0;

		temp32 |= ((uint32_t) data[i]) << ((i % 4) * 8);

		if ((i % 4) == 3) /* 32 bits are full, write them to regs. */
			writel_(temp32, cntlr.data + (i - (i % 4)));
	}
	i--;
	if ((i % 4) != 3) /* Write remaining data to regs. */
		writel_(temp32, cntlr.data + (i - (i % 4)));
}

static int pch_hwseq_write(struct spi_flash *flash,
			   u32 addr, size_t len, const void *buf)
{
	uint16_t hsfc;
	uint16_t timeout = 100 * 60;
	uint8_t block_len;
	uint32_t start = addr;

	if (addr + len > spi_get_flash_size()) {
		printk(BIOS_ERR,
			"Attempt to write 0x%x-0x%x which is out of chip\n",
			(unsigned)addr, (unsigned) (addr+len));
		return -1;
	}

	/* clear FDONE, FCERR, AEL by writing 1 to them (if they are set) */
	writew_(readw_(&cntlr.pch_spi->hsfs), &cntlr.pch_spi->hsfs);

	while (len > 0) {
		block_len = min(len, cntlr.databytes);
		if (block_len > (~addr & 0xff))
			block_len = (~addr & 0xff) + 1;

		pch_hwseq_set_addr(addr);

		pch_fill_data(buf, block_len);
		hsfc = readw_(&cntlr.pch_spi->hsfc);
		hsfc &= ~HSFC_FCYCLE; /* clear operation */
		hsfc |= HSFC_FCYCLE_WR; /* set write operation */
		hsfc &= ~HSFC_FDBC; /* clear byte count */
		/* set byte count */
		hsfc |= (((block_len - 1) << HSFC_FDBC_OFF) & HSFC_FDBC);
		hsfc |= HSFC_FGO; /* start */
		writew_(hsfc, &cntlr.pch_spi->hsfc);

		if (pch_hwseq_wait_for_cycle_complete(timeout, block_len)) {
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


static struct spi_flash *spi_flash_hwseq_probe(struct spi_slave *spi)
{
	struct spi_flash *flash = NULL;
	u32 berase;

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
	pch_hwseq_set_addr(0);

	berase = (cntlr.hsfs >> SPIBAR_HSFS_BERASE_OFFSET) &
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

	flash->size = spi_get_flash_size();

	return flash;
}
