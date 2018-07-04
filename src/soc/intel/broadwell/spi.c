/*
 * Copyright (C) 2014 Google Inc.
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
#include <stdint.h>
#include <compiler.h>
#include <stdlib.h>
#include <string.h>
#include <bootstate.h>
#include <commonlib/helpers.h>
#include <delay.h>
#include <arch/io.h>
#include <console/console.h>
#include <device/pci_ids.h>
#include <spi_flash.h>
#include <spi-generic.h>
#include <soc/pci_devs.h>
#include <soc/rcba.h>
#include <soc/spi.h>

#ifdef __SMM__
#define pci_read_config_byte(dev, reg, targ)\
	*(targ) = pci_read_config8(dev, reg)
#define pci_read_config_word(dev, reg, targ)\
	*(targ) = pci_read_config16(dev, reg)
#define pci_read_config_dword(dev, reg, targ)\
	*(targ) = pci_read_config32(dev, reg)
#define pci_write_config_byte(dev, reg, val)\
	pci_write_config8(dev, reg, val)
#define pci_write_config_word(dev, reg, val)\
	pci_write_config16(dev, reg, val)
#define pci_write_config_dword(dev, reg, val)\
	pci_write_config32(dev, reg, val)
#else /* !__SMM__ */
#include <device/device.h>
#include <device/pci.h>
#define pci_read_config_byte(dev, reg, targ)\
	*(targ) = pci_read_config8(dev, reg)
#define pci_read_config_word(dev, reg, targ)\
	*(targ) = pci_read_config16(dev, reg)
#define pci_read_config_dword(dev, reg, targ)\
	*(targ) = pci_read_config32(dev, reg)
#define pci_write_config_byte(dev, reg, val)\
	pci_write_config8(dev, reg, val)
#define pci_write_config_word(dev, reg, val)\
	pci_write_config16(dev, reg, val)
#define pci_write_config_dword(dev, reg, val)\
	pci_write_config32(dev, reg, val)
#endif /* !__SMM__ */

typedef struct spi_slave ich_spi_slave;

static int ichspi_lock = 0;

typedef struct ich9_spi_regs {
	uint32_t bfpr;
	uint16_t hsfs;
	uint16_t hsfc;
	uint32_t faddr;
	uint32_t _reserved0;
	uint32_t fdata[16];
	uint32_t frap;
	uint32_t freg[5];
	uint32_t _reserved1[3];
	uint32_t pr[5];
	uint32_t _reserved2[2];
	uint8_t ssfs;
	uint8_t ssfc[3];
	uint16_t preop;
	uint16_t optype;
	uint8_t opmenu[8];
	uint32_t bbar;
	uint8_t _reserved3[12];
	uint32_t fdoc;
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
} __packed ich9_spi_regs;

typedef struct ich_spi_controller {
	int locked;

	uint8_t *opmenu;
	int menubytes;
	uint16_t *preop;
	uint16_t *optype;
	uint32_t *addr;
	uint8_t *data;
	unsigned int databytes;
	uint8_t *status;
	uint16_t *control;
	uint32_t *bbar;
} ich_spi_controller;

static ich_spi_controller cntlr;

enum {
	SPIS_SCIP =		0x0001,
	SPIS_GRANT =		0x0002,
	SPIS_CDS =		0x0004,
	SPIS_FCERR =		0x0008,
	SSFS_AEL =		0x0010,
	SPIS_LOCK =		0x8000,
	SPIS_RESERVED_MASK =	0x7ff0,
	SSFS_RESERVED_MASK =	0x7fe2
};

enum {
	SPIC_SCGO =		0x000002,
	SPIC_ACS =		0x000004,
	SPIC_SPOP =		0x000008,
	SPIC_DBC =		0x003f00,
	SPIC_DS =		0x004000,
	SPIC_SME =		0x008000,
	SSFC_SCF_MASK =		0x070000,
	SSFC_RESERVED =		0xf80000
};

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

enum {
	SPI_OPCODE_TYPE_READ_NO_ADDRESS =	0,
	SPI_OPCODE_TYPE_WRITE_NO_ADDRESS =	1,
	SPI_OPCODE_TYPE_READ_WITH_ADDRESS =	2,
	SPI_OPCODE_TYPE_WRITE_WITH_ADDRESS =	3
};

#if IS_ENABLED(CONFIG_DEBUG_SPI_FLASH)

static u8 readb_(const void *addr)
{
	u8 v = read8(addr);
	printk(BIOS_DEBUG, "read %2.2x from %4.4x\n",
	       v, ((unsigned int) addr & 0xffff) - 0xf020);
	return v;
}

static u16 readw_(const void *addr)
{
	u16 v = read16(addr);
	printk(BIOS_DEBUG, "read %4.4x from %4.4x\n",
	       v, ((unsigned int) addr & 0xffff) - 0xf020);
	return v;
}

static u32 readl_(const void *addr)
{
	u32 v = read32(addr);
	printk(BIOS_DEBUG, "read %8.8x from %4.4x\n",
	       v, ((unsigned int) addr & 0xffff) - 0xf020);
	return v;
}

static void writeb_(u8 b, void *addr)
{
	write8(addr, b);
	printk(BIOS_DEBUG, "wrote %2.2x to %4.4x\n",
	       b, ((unsigned int) addr & 0xffff) - 0xf020);
}

static void writew_(u16 b, void *addr)
{
	write16(addr, b);
	printk(BIOS_DEBUG, "wrote %4.4x to %4.4x\n",
	       b, ((unsigned int) addr & 0xffff) - 0xf020);
}

static void writel_(u32 b, void *addr)
{
	write32(addr, b);
	printk(BIOS_DEBUG, "wrote %8.8x to %4.4x\n",
	       b, ((unsigned int) addr & 0xffff) - 0xf020);
}

#else /* CONFIG_DEBUG_SPI_FLASH ^^^ enabled  vvv NOT enabled */

#define readb_(a) read8(a)
#define readw_(a) read16(a)
#define readl_(a) read32(a)
#define writeb_(val, addr) write8(addr, val)
#define writew_(val, addr) write16(addr, val)
#define writel_(val, addr) write32(addr, val)

#endif  /* CONFIG_DEBUG_SPI_FLASH ^^^ NOT enabled */

static void write_reg(const void *value, void *dest, uint32_t size)
{
	const uint8_t *bvalue = value;
	uint8_t *bdest = dest;

	while (size >= 4) {
		writel_(*(const uint32_t *)bvalue, bdest);
		bdest += 4; bvalue += 4; size -= 4;
	}
	while (size) {
		writeb_(*bvalue, bdest);
		bdest++; bvalue++; size--;
	}
}

static void read_reg(const void *src, void *value, uint32_t size)
{
	const uint8_t *bsrc = src;
	uint8_t *bvalue = value;

	while (size >= 4) {
		*(uint32_t *)bvalue = readl_(bsrc);
		bsrc += 4; bvalue += 4; size -= 4;
	}
	while (size) {
		*bvalue = readb_(bsrc);
		bsrc++; bvalue++; size--;
	}
}

static void ich_set_bbar(uint32_t minaddr)
{
	const uint32_t bbar_mask = 0x00ffff00;
	uint32_t ichspi_bbar;

	minaddr &= bbar_mask;
	ichspi_bbar = readl_(cntlr.bbar) & ~bbar_mask;
	ichspi_bbar |= minaddr;
	writel_(ichspi_bbar, cntlr.bbar);
}

void spi_init(void)
{
	uint8_t *rcrb; /* Root Complex Register Block */
	uint32_t rcba; /* Root Complex Base Address */
	uint8_t bios_cntl;
#if defined(__SIMPLE_DEVICE__)
	pci_devfn_t dev = PCH_DEV_LPC;
#else
	struct device *dev = PCH_DEV_LPC;
#endif
	ich9_spi_regs *ich9_spi;

	pci_read_config_dword(dev, 0xf0, &rcba);
	/* Bits 31-14 are the base address, 13-1 are reserved, 0 is enable. */
	rcrb = (uint8_t *)(rcba & 0xffffc000);
	ich9_spi = (ich9_spi_regs *)(rcrb + 0x3800);
	ichspi_lock = readw_(&ich9_spi->hsfs) & HSFS_FLOCKDN;
	cntlr.opmenu = ich9_spi->opmenu;
	cntlr.menubytes = sizeof(ich9_spi->opmenu);
	cntlr.optype = &ich9_spi->optype;
	cntlr.addr = &ich9_spi->faddr;
	cntlr.data = (uint8_t *)ich9_spi->fdata;
	cntlr.databytes = sizeof(ich9_spi->fdata);
	cntlr.status = &ich9_spi->ssfs;
	cntlr.control = (uint16_t *)ich9_spi->ssfc;
	cntlr.bbar = &ich9_spi->bbar;
	cntlr.preop = &ich9_spi->preop;
	ich_set_bbar(0);

	/* Disable the BIOS write protect so write commands are allowed. */
	pci_read_config_byte(dev, 0xdc, &bios_cntl);
	bios_cntl &= ~(1 << 5);
	pci_write_config_byte(dev, 0xdc, bios_cntl | 0x1);
}

static void spi_init_cb(void *unused)
{
	spi_init();
}

BOOT_STATE_INIT_ENTRY(BS_DEV_INIT, BS_ON_ENTRY, spi_init_cb, NULL);

typedef struct spi_transaction {
	const uint8_t *out;
	uint32_t bytesout;
	uint8_t *in;
	uint32_t bytesin;
	uint8_t type;
	uint8_t opcode;
	uint32_t offset;
} spi_transaction;

static inline void spi_use_out(spi_transaction *trans, unsigned int bytes)
{
	trans->out += bytes;
	trans->bytesout -= bytes;
}

static inline void spi_use_in(spi_transaction *trans, unsigned int bytes)
{
	trans->in += bytes;
	trans->bytesin -= bytes;
}

static void spi_setup_type(spi_transaction *trans)
{
	trans->type = 0xFF;

	/* Try to guess spi type from read/write sizes. */
	if (trans->bytesin == 0) {
		if (trans->bytesout > 4)
			/*
			 * If bytesin = 0 and bytesout > 4, we presume this is
			 * a write data operation, which is accompanied by an
			 * address.
			 */
			trans->type = SPI_OPCODE_TYPE_WRITE_WITH_ADDRESS;
		else
			trans->type = SPI_OPCODE_TYPE_WRITE_NO_ADDRESS;
		return;
	}

	if (trans->bytesout == 1) { /* and bytesin is > 0 */
		trans->type = SPI_OPCODE_TYPE_READ_NO_ADDRESS;
		return;
	}

	if (trans->bytesout == 4) { /* and bytesin is > 0 */
		trans->type = SPI_OPCODE_TYPE_READ_WITH_ADDRESS;
	}

	/* Fast read command is called with 5 bytes instead of 4 */
	if (trans->out[0] == SPI_OPCODE_FAST_READ && trans->bytesout == 5) {
		trans->type = SPI_OPCODE_TYPE_READ_WITH_ADDRESS;
		--trans->bytesout;
	}
}

static int spi_setup_opcode(spi_transaction *trans)
{
	uint16_t optypes;
	uint8_t opmenu[cntlr.menubytes];

	trans->opcode = trans->out[0];
	spi_use_out(trans, 1);
	if (!ichspi_lock) {
		/* The lock is off, so just use index 0. */
		writeb_(trans->opcode, cntlr.opmenu);
		optypes = readw_(cntlr.optype);
		optypes = (optypes & 0xfffc) | (trans->type & 0x3);
		writew_(optypes, cntlr.optype);
		return 0;
	}

	/* The lock is on. See if what we need is on the menu. */
	uint8_t optype;
	uint16_t opcode_index;

	/* Write Enable is handled as atomic prefix */
	if (trans->opcode == SPI_OPCODE_WREN)
		return 0;

	read_reg(cntlr.opmenu, opmenu, sizeof(opmenu));
	for (opcode_index = 0; opcode_index < cntlr.menubytes;
			opcode_index++) {
		if (opmenu[opcode_index] == trans->opcode)
			break;
	}

	if (opcode_index == cntlr.menubytes) {
		printk(BIOS_DEBUG, "ICH SPI: Opcode %x not found\n",
			trans->opcode);
		return -1;
	}

	optypes = readw_(cntlr.optype);
	optype = (optypes >> (opcode_index * 2)) & 0x3;
	if (trans->type == SPI_OPCODE_TYPE_WRITE_NO_ADDRESS &&
		optype == SPI_OPCODE_TYPE_WRITE_WITH_ADDRESS &&
		trans->bytesout >= 3) {
		/* We guessed wrong earlier. Fix it up. */
		trans->type = optype;
	}
	if (optype != trans->type) {
		printk(BIOS_DEBUG, "ICH SPI: Transaction doesn't fit type %d\n",
			optype);
		return -1;
	}
	return opcode_index;
}

static int spi_setup_offset(spi_transaction *trans)
{
	/* Separate the SPI address and data. */
	switch (trans->type) {
	case SPI_OPCODE_TYPE_READ_NO_ADDRESS:
	case SPI_OPCODE_TYPE_WRITE_NO_ADDRESS:
		return 0;
	case SPI_OPCODE_TYPE_READ_WITH_ADDRESS:
	case SPI_OPCODE_TYPE_WRITE_WITH_ADDRESS:
		trans->offset = ((uint32_t)trans->out[0] << 16) |
				((uint32_t)trans->out[1] << 8) |
				((uint32_t)trans->out[2] << 0);
		spi_use_out(trans, 3);
		return 1;
	default:
		printk(BIOS_DEBUG, "Unrecognized SPI transaction type %#x\n",
			trans->type);
		return -1;
	}
}

/*
 * Wait for up to 60ms til status register bit(s) turn 1 (in case wait_til_set
 * below is True) or 0. In case the wait was for the bit(s) to set - write
 * those bits back, which would cause resetting them.
 *
 * Return the last read status value on success or -1 on failure.
 */
static int ich_status_poll(u16 bitmask, int wait_til_set)
{
	int timeout = 6000; /* This will result in 60 ms */
	u16 status = 0;

	while (timeout--) {
		status = readw_(cntlr.status);
		if (wait_til_set ^ ((status & bitmask) == 0)) {
			if (wait_til_set)
				writew_((status & bitmask), cntlr.status);
			return status;
		}
		udelay(10);
	}

	printk(BIOS_DEBUG, "ICH SPI: SCIP timeout, read %x, expected %x\n",
		status, bitmask);
	return -1;
}

static int spi_ctrlr_xfer(const struct spi_slave *slave, const void *dout,
		    size_t bytesout, void *din, size_t bytesin)
{
	uint16_t control;
	int16_t opcode_index;
	int with_address;
	int status;

	spi_transaction trans = {
		dout, bytesout,
		din, bytesin,
		0xff, 0xff, 0
	};

	/* There has to always at least be an opcode. */
	if (!bytesout || !dout) {
		printk(BIOS_DEBUG, "ICH SPI: No opcode for transfer\n");
		return -1;
	}
	/* Make sure if we read something we have a place to put it. */
	if (bytesin != 0 && !din) {
		printk(BIOS_DEBUG, "ICH SPI: Read but no target buffer\n");
		return -1;
	}

	if (ich_status_poll(SPIS_SCIP, 0) == -1)
		return -1;

	writew_(SPIS_CDS | SPIS_FCERR, cntlr.status);

	spi_setup_type(&trans);
	opcode_index = spi_setup_opcode(&trans);
	if (opcode_index < 0)
		return -1;
	with_address = spi_setup_offset(&trans);
	if (with_address < 0)
		return -1;

	if (trans.opcode == SPI_OPCODE_WREN) {
		/*
		 * Treat Write Enable as Atomic Pre-Op if possible
		 * in order to prevent the Management Engine from
		 * issuing a transaction between WREN and DATA.
		 */
		if (!ichspi_lock)
			writew_(trans.opcode, cntlr.preop);
		return 0;
	}

	/* Preset control fields */
	control = SPIC_SCGO | ((opcode_index & 0x07) << 4);

	/* Issue atomic preop cycle if needed */
	if (readw_(cntlr.preop))
		control |= SPIC_ACS;

	if (!trans.bytesout && !trans.bytesin) {
		/* SPI addresses are 24 bit only */
		if (with_address)
			writel_(trans.offset & 0x00FFFFFF, cntlr.addr);

		/*
		 * This is a 'no data' command (like Write Enable), its
		 * bytesout size was 1, decremented to zero while executing
		 * spi_setup_opcode() above. Tell the chip to send the
		 * command.
		 */
		writew_(control, cntlr.control);

		/* wait for the result */
		status = ich_status_poll(SPIS_CDS | SPIS_FCERR, 1);
		if (status == -1)
			return -1;

		if (status & SPIS_FCERR) {
			printk(BIOS_DEBUG,
				"ICH SPI: Command transaction error\n");
			return -1;
		}

		return 0;
	}

	/*
	 * Check if this is a write command attempting to transfer more bytes
	 * than the controller can handle. Iterations for writes are not
	 * supported here because each SPI write command needs to be preceded
	 * and followed by other SPI commands, and this sequence is controlled
	 * by the SPI chip driver.
	 */
	if (trans.bytesout > cntlr.databytes) {
		printk(BIOS_DEBUG, "ICH SPI: Too much to write. Does your SPI"
		     " chip driver use CONTROLLER_PAGE_LIMIT?\n");
		return -1;
	}

	/*
	 * Read or write up to databytes bytes at a time until everything has
	 * been sent.
	 */
	while (trans.bytesout || trans.bytesin) {
		uint32_t data_length;

		/* SPI addresses are 24 bit only */
		/* http://www.intel.com/content/dam/www/public/us/en/documents/
		 * datasheets/pentium-n3520-j2850-celeron-n2920-n2820-n2815-
		 * n2806-j1850-j1750-datasheet.pdf
		 */
		writel_(trans.offset & 0x00FFFFFF, cntlr.addr);

		if (trans.bytesout)
			data_length = min(trans.bytesout, cntlr.databytes);
		else
			data_length = min(trans.bytesin, cntlr.databytes);

		/* Program data into FDATA0 to N */
		if (trans.bytesout) {
			write_reg(trans.out, cntlr.data, data_length);
			spi_use_out(&trans, data_length);
			if (with_address)
				trans.offset += data_length;
		}

		/* Add proper control fields' values */
		control &= ~((cntlr.databytes - 1) << 8);
		control |= SPIC_DS;
		control |= (data_length - 1) << 8;

		/* write it */
		writew_(control, cntlr.control);

		/* Wait for Cycle Done Status or Flash Cycle Error. */
		status = ich_status_poll(SPIS_CDS | SPIS_FCERR, 1);
		if (status == -1)
			return -1;

		if (status & SPIS_FCERR) {
			printk(BIOS_DEBUG, "ICH SPI: Data transaction error\n");
			return -1;
		}

		if (trans.bytesin) {
			read_reg(cntlr.data, trans.in, data_length);
			spi_use_in(&trans, data_length);
			if (with_address)
				trans.offset += data_length;
		}
	}

	/* Clear atomic preop now that xfer is done */
	writew_(0, cntlr.preop);

	return 0;
}

/* Use first empty Protected Range Register to cover region of flash */
static int spi_flash_protect(const struct spi_flash *flash,
				const struct region *region)
{
	u32 start = region_offset(region);
	u32 end = start + region_sz(region) - 1;
	u32 reg;
	int prr;

	/* Find first empty PRR */
	for (prr = 0; prr < SPI_PRR_MAX; prr++) {
		reg = SPIBAR32(SPI_PRR(prr));
		if (reg == 0)
			break;
	}
	if (prr >= SPI_PRR_MAX) {
		printk(BIOS_ERR, "ERROR: No SPI PRR free!\n");
		return -1;
	}

	/* Set protected range base and limit */
	reg = ((end >> SPI_PRR_SHIFT) & SPI_PRR_MASK);
	reg <<= SPI_PRR_LIMIT_SHIFT;
	reg |= ((start >> SPI_PRR_SHIFT) & SPI_PRR_MASK);
	reg |= SPI_PRR_WPE;

	/* Set the PRR register and verify it is protected */
	SPIBAR32(SPI_PRR(prr)) = reg;
	reg = SPIBAR32(SPI_PRR(prr));
	if (!(reg & SPI_PRR_WPE)) {
		printk(BIOS_ERR, "ERROR: Unable to set SPI PRR %d\n", prr);
		return -1;
	}

	printk(BIOS_INFO, "%s: PRR %d is enabled for range 0x%08x-0x%08x\n",
	       __func__, prr, start, end);
	return 0;
}

static int xfer_vectors(const struct spi_slave *slave,
			struct spi_op vectors[], size_t count)
{
	return spi_flash_vector_helper(slave, vectors, count, spi_ctrlr_xfer);
}

static const struct spi_ctrlr spi_ctrlr = {
	.xfer_vector = xfer_vectors,
	.max_xfer_size = member_size(ich9_spi_regs, fdata),
	.flash_protect = spi_flash_protect,
};

const struct spi_ctrlr_buses spi_ctrlr_bus_map[] = {
	{
		.ctrlr = &spi_ctrlr,
		.bus_start = 0,
		.bus_end = 0,
	},
};

const size_t spi_ctrlr_bus_map_count = ARRAY_SIZE(spi_ctrlr_bus_map);
