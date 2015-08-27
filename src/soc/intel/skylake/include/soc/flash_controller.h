/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2015 Intel Corporation
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
 * Foundation, Inc.,
 */

#ifndef _SOC_FLASH_CONTROLLER__H_
#define _SOC_FLASH_CONTROLLER__H_

#include <rules.h>
#include <arch/io.h>
#include <console/console.h>
#include <spi_flash.h>

int pch_hwseq_erase(struct spi_flash *flash, u32 offset, size_t len);
int pch_hwseq_write(struct spi_flash *flash,
			   u32 addr, size_t len, const void *buf);

int pch_hwseq_read(struct spi_flash *flash,
			  u32 addr, size_t len, void *buf);
int pch_hwseq_read_status(struct spi_flash *flash, u8 *reg);


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

#define HSFC_FCYCLE_MASK	(0xf << HSFC_FCYCLE_SHIFT)
#define HSFC_FCYCLE_RD		(0x0 << HSFC_FCYCLE_SHIFT)
#define HSFC_FCYCLE_WR		(0x2 << HSFC_FCYCLE_SHIFT)
#define HSFC_FCYCLE_4KE		(0x3 << HSFC_FCYCLE_SHIFT)
#define HSFC_FCYCLE_64KE	(0x4 << HSFC_FCYCLE_SHIFT)
#define HSFC_FCYCLE_SFDP	(0x5 << HSFC_FCYCLE_SHIFT)
#define HSFC_FCYCLE_JEDECID	(0x6 << HSFC_FCYCLE_SHIFT)
#define HSFC_FCYCLE_WS		(0x7 << HSFC_FCYCLE_SHIFT)
#define HSFC_FCYCLE_RS		(0x8 << HSFC_FCYCLE_SHIFT)
#define HSFC_FDBC_MASK		(0x3f << HSFC_FDBC_SHIFT)

#define SPI_READ_STATUS_LENGTH 1 /* Read Status Register 1 */

#define WPSR_MASK_SRP0_BIT 0x80

#define SPI_FDATA_REGS 16
#define SPI_FDATA_BYTES (SPI_FDATA_REGS * sizeof(uint32_t))

typedef struct pch_spi_regs {
	uint32_t bfpr;
	uint16_t hsfs;
	uint16_t hsfc;
	uint32_t faddr;
	uint32_t dlock;
	uint32_t fdata[SPI_FDATA_REGS];
	uint32_t frap;
	uint32_t freg[6];
	uint32_t _reserved1[6];
	uint32_t pr[5];
	uint32_t gpr0;
	uint32_t _reserved2;
	uint32_t _reserved3;
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

enum {
	HSFS_FDONE =		0x0001,
	HSFS_FCERR =		0x0002,
	HSFS_FDV =		0x4000,
};

enum {
	HSFC_FGO =		0x0001,
	HSFC_FCYCLE_SHIFT =	1,
	HSFC_FDBC_SHIFT =	8,
};
#endif	/* _SOC_FLASH_CONTROLLER__H_ */
