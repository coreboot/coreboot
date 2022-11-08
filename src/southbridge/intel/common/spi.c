/* SPDX-License-Identifier: GPL-2.0-or-later */

#define __SIMPLE_DEVICE__

/* This file is derived from the flashrom project. */

#include <string.h>
#include <bootstate.h>
#include <commonlib/helpers.h>
#include <delay.h>
#include <device/mmio.h>
#include <device/pci_ops.h>
#include <console/console.h>
#include <device/device.h>
#include <device/pci.h>
#include <spi_flash.h>
#include <spi-generic.h>
#include <timer.h>
#include <types.h>

#include "spi.h"

#define HSFC_FCYCLE_OFF		1	/* 1-2: FLASH Cycle */
#define HSFC_FCYCLE		(0x3 << HSFC_FCYCLE_OFF)
#define HSFC_FDBC_OFF		8	/* 8-13: Flash Data Byte Count */
#define HSFC_FDBC		(0x3f << HSFC_FDBC_OFF)

static int spi_is_multichip(void);

static void spi_set_smm_only_flashing(bool enable);

struct ich7_spi_regs {
	uint16_t spis;
	uint16_t spic;
	uint32_t spia;
	uint64_t spid[8];
	uint64_t _pad;
	uint32_t bbar;
	uint16_t preop;
	uint16_t optype;
	uint8_t opmenu[8];
	uint32_t pbr[3];
} __packed;

struct ich9_spi_regs {
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
} __packed;

struct ich_spi_controller {
	int locked;
	uint32_t flmap0;
	uint32_t flcomp;
	uint32_t hsfs;

	union {
		struct ich9_spi_regs *ich9_spi;
		struct ich7_spi_regs *ich7_spi;
	};
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
	uint32_t *fpr;
	uint8_t fpr_max;
};

static struct ich_spi_controller cntlr;

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

#if CONFIG(DEBUG_SPI_FLASH)

static u8 readb_(const void *addr)
{
	u8 v = read8(addr);

	printk(BIOS_DEBUG, "read %2.2x from %4.4lx\n",
	       v, ((uintptr_t)addr & 0xffff) - 0xf020);
	return v;
}

static u16 readw_(const void *addr)
{
	u16 v = read16(addr);

	printk(BIOS_DEBUG, "read %4.4x from %4.4lx\n",
	       v, ((uintptr_t)addr & 0xffff) - 0xf020);
	return v;
}

static u32 readl_(const void *addr)
{
	u32 v = read32(addr);

	printk(BIOS_DEBUG, "read %8.8x from %4.4lx\n",
	       v, ((uintptr_t)addr & 0xffff) - 0xf020);
	return v;
}

static void writeb_(u8 b, void *addr)
{
	write8(addr, b);
	printk(BIOS_DEBUG, "wrote %2.2x to %4.4lx\n",
	       b, ((uintptr_t)addr & 0xffff) - 0xf020);
}

static void writew_(u16 b, void *addr)
{
	write16(addr, b);
	printk(BIOS_DEBUG, "wrote %4.4x to %4.4lx\n",
	       b, ((uintptr_t)addr & 0xffff) - 0xf020);
}

static void writel_(u32 b, void *addr)
{
	write32(addr, b);
	printk(BIOS_DEBUG, "wrote %8.8x to %4.4lx\n",
	       b, ((uintptr_t)addr & 0xffff) - 0xf020);
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

#if CONFIG(SOUTHBRIDGE_INTEL_I82801GX)
#define MENU_BYTES member_size(struct ich7_spi_regs, opmenu)
#else
#define MENU_BYTES member_size(struct ich9_spi_regs, opmenu)
#endif

#define RCBA 0xf0
#define SBASE 0x54

static void *get_spi_bar(pci_devfn_t dev)
{
	uintptr_t rcba; /* Root Complex Register Block */
	uintptr_t sbase;

	if (CONFIG(SOUTHBRIDGE_INTEL_I82801GX)) {
		rcba = pci_read_config32(dev, RCBA);
		return (void *)((rcba & 0xffffc000) + 0x3020);
	}
	if (CONFIG(SOUTHBRIDGE_INTEL_COMMON_SPI_SILVERMONT)) {
		sbase = pci_read_config32(dev, SBASE);
		sbase &= ~0x1ff;
		return (void *)sbase;
	}
	if (CONFIG(SOUTHBRIDGE_INTEL_COMMON_SPI_ICH9)) {
		rcba = pci_read_config32(dev, RCBA);
		return (void *)((rcba & 0xffffc000) + 0x3800);
	}
}

void spi_init(void)
{
	struct ich9_spi_regs *ich9_spi;
	struct ich7_spi_regs *ich7_spi;
	uint16_t hsfs;

	pci_devfn_t dev = PCI_DEV(0, 31, 0);

	if (CONFIG(SOUTHBRIDGE_INTEL_I82801GX)) {
		ich7_spi = get_spi_bar(dev);
		cntlr.ich7_spi = ich7_spi;
		cntlr.opmenu = ich7_spi->opmenu;
		cntlr.menubytes = sizeof(ich7_spi->opmenu);
		cntlr.optype = &ich7_spi->optype;
		cntlr.addr = &ich7_spi->spia;
		cntlr.data = (uint8_t *)ich7_spi->spid;
		cntlr.databytes = sizeof(ich7_spi->spid);
		cntlr.status = (uint8_t *)&ich7_spi->spis;
		cntlr.control = &ich7_spi->spic;
		cntlr.bbar = &ich7_spi->bbar;
		cntlr.preop = &ich7_spi->preop;
		cntlr.fpr = &ich7_spi->pbr[0];
		cntlr.fpr_max = 3;
	} else {
		ich9_spi = get_spi_bar(dev);
		cntlr.ich9_spi = ich9_spi;
		hsfs = readw_(&ich9_spi->hsfs);
		cntlr.hsfs = hsfs;
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
		cntlr.fpr = &ich9_spi->pr[0];
		cntlr.fpr_max = 5;

		if (cntlr.hsfs & HSFS_FDV) {
			writel_(4, &ich9_spi->fdoc);
			cntlr.flmap0 = readl_(&ich9_spi->fdod);
			writel_(0x1000, &ich9_spi->fdoc);
			cntlr.flcomp = readl_(&ich9_spi->fdod);
		}
	}

	ich_set_bbar(0);

	/* Disable the BIOS write protect so write commands are allowed. */
	spi_set_smm_only_flashing(false);
}

static int spi_locked(void)
{
	if (CONFIG(SOUTHBRIDGE_INTEL_I82801GX)) {
		return !!(readw_(&cntlr.ich7_spi->spis) & HSFS_FLOCKDN);
	} else {
		return !!(readw_(&cntlr.ich9_spi->hsfs) & HSFS_FLOCKDN);
	}
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
	uint8_t opmenu[MENU_BYTES];

	trans->opcode = trans->out[0];
	spi_use_out(trans, 1);
	if (!spi_locked()) {
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
	for (opcode_index = 0; opcode_index < ARRAY_SIZE(opmenu); opcode_index++) {
		if (opmenu[opcode_index] == trans->opcode)
			break;
	}

	if (opcode_index == ARRAY_SIZE(opmenu)) {
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
		printk(BIOS_DEBUG, "Unrecognized SPI transaction type %#x\n", trans->type);
		return -1;
	}
}

/*
 * Wait for up to 6s til status register bit(s) turn 1 (in case wait_til_set
 * below is True) or 0. In case the wait was for the bit(s) to set - write
 * those bits back, which would cause resetting them.
 *
 * Return the last read status value on success or -1 on failure.
 */
static int ich_status_poll(u16 bitmask, int wait_til_set)
{
	int timeout = 600000; /* This will result in 6 seconds */
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

	printk(BIOS_DEBUG, "ICH SPI: SCIP timeout, read %x, bitmask %x\n",
		status, bitmask);
	return -1;
}

static int spi_is_multichip(void)
{
	if (!(cntlr.hsfs & HSFS_FDV))
		return 0;
	return !!((cntlr.flmap0 >> 8) & 3);
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
	if ((opcode_index = spi_setup_opcode(&trans)) < 0)
		return -1;
	if ((with_address = spi_setup_offset(&trans)) < 0)
		return -1;

	if (trans.opcode == SPI_OPCODE_WREN) {
		/*
		 * Treat Write Enable as Atomic Pre-Op if possible
		 * in order to prevent the Management Engine from
		 * issuing a transaction between WREN and DATA.
		 */
		if (!spi_locked())
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
		 * bitesout size was 1, decremented to zero while executing
		 * spi_setup_opcode() above. Tell the chip to send the
		 * command.
		 */
		writew_(control, cntlr.control);

		/* wait for the result */
		status = ich_status_poll(SPIS_CDS | SPIS_FCERR, 1);
		if (status == -1)
			return -1;

		if (status & SPIS_FCERR) {
			printk(BIOS_DEBUG, "ICH SPI: Command transaction error\n");
			return -1;
		}

		goto spi_xfer_exit;
	}

	/*
	 * Check if this is a write command attempting to transfer more bytes
	 * than the controller can handle. Iterations for writes are not
	 * supported here because each SPI write command needs to be preceded
	 * and followed by other SPI commands, and this sequence is controlled
	 * by the SPI chip driver.
	 */
	if (trans.bytesout > cntlr.databytes) {
		printk(BIOS_DEBUG, "ICH SPI: Too much to write. Does your SPI chip driver use"
		     " spi_crop_chunk()?\n");
		return -1;
	}

	/*
	 * Read or write up to databytes bytes at a time until everything has
	 * been sent.
	 */
	while (trans.bytesout || trans.bytesin) {
		uint32_t data_length;

		/* SPI addresses are 24 bit only */
		writel_(trans.offset & 0x00FFFFFF, cntlr.addr);

		if (trans.bytesout)
			data_length = MIN(trans.bytesout, cntlr.databytes);
		else
			data_length = MIN(trans.bytesin, cntlr.databytes);

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

spi_xfer_exit:
	/* Clear atomic preop now that xfer is done */
	writew_(0, cntlr.preop);

	return 0;
}

/* Sets FLA in FADDR to (addr & 0x01FFFFFF) without touching other bits. */
static void ich_hwseq_set_addr(uint32_t addr)
{
	uint32_t addr_old = readl_(&cntlr.ich9_spi->faddr) & ~0x01FFFFFF;

	writel_((addr & 0x01FFFFFF) | addr_old, &cntlr.ich9_spi->faddr);
}

/* Polls for Cycle Done Status, Flash Cycle Error or timeout in 8 us intervals.
   Resets all error flags in HSFS.
   Returns 0 if the cycle completes successfully without errors within
   timeout us, 1 on errors. */
static int ich_hwseq_wait_for_cycle_complete(unsigned int timeout,
					     unsigned int len)
{
	uint16_t hsfs;
	uint32_t addr;

	timeout /= 8; /* scale timeout duration to counter */
	while ((((hsfs = readw_(&cntlr.ich9_spi->hsfs)) &
		 (HSFS_FDONE | HSFS_FCERR)) == 0) &&
	       --timeout) {
		udelay(8);
	}
	writew_(readw_(&cntlr.ich9_spi->hsfs), &cntlr.ich9_spi->hsfs);

	if (!timeout) {
		uint16_t hsfc;
		addr = readl_(&cntlr.ich9_spi->faddr) & 0x01FFFFFF;
		hsfc = readw_(&cntlr.ich9_spi->hsfc);
		printk(BIOS_ERR, "Transaction timeout between offset 0x%08x and "
		       "0x%08x (= 0x%08x + %d) HSFC=%x HSFS=%x!\n",
		       addr, addr + len - 1, addr, len - 1,
		       hsfc, hsfs);
		return 1;
	}

	if (hsfs & HSFS_FCERR) {
		uint16_t hsfc;
		addr = readl_(&cntlr.ich9_spi->faddr) & 0x01FFFFFF;
		hsfc = readw_(&cntlr.ich9_spi->hsfc);
		printk(BIOS_ERR, "Transaction error between offset 0x%08x and "
		       "0x%08x (= 0x%08x + %d) HSFC=%x HSFS=%x!\n",
		       addr, addr + len - 1, addr, len - 1,
		       hsfc, hsfs);
		return 1;
	}
	return 0;
}

static int ich_hwseq_erase(const struct spi_flash *flash, u32 offset,
			size_t len)
{
	u32 start, end, erase_size;
	int ret;
	uint16_t hsfc;
	unsigned int timeout = 1000 * USECS_PER_MSEC; /* 1 second timeout */

	erase_size = flash->sector_size;
	if (offset % erase_size || len % erase_size) {
		printk(BIOS_ERR, "SF: Erase offset/length not multiple of erase size\n");
		return -1;
	}

	ret = spi_claim_bus(&flash->spi);
	if (ret) {
		printk(BIOS_ERR, "SF: Unable to claim SPI bus\n");
		return ret;
	}

	start = offset;
	end = start + len;

	while (offset < end) {
		/* make sure FDONE, FCERR, AEL are cleared by writing 1 to them */
		writew_(readw_(&cntlr.ich9_spi->hsfs), &cntlr.ich9_spi->hsfs);

		ich_hwseq_set_addr(offset);

		offset += erase_size;

		hsfc = readw_(&cntlr.ich9_spi->hsfc);
		hsfc &= ~HSFC_FCYCLE; /* clear operation */
		hsfc |= (0x3 << HSFC_FCYCLE_OFF); /* set erase operation */
		hsfc |= HSFC_FGO; /* start */
		writew_(hsfc, &cntlr.ich9_spi->hsfc);
		if (ich_hwseq_wait_for_cycle_complete(timeout, len)) {
			printk(BIOS_ERR, "SF: Erase failed at %x\n", offset - erase_size);
			ret = -1;
			goto out;
		}
	}

	printk(BIOS_DEBUG, "SF: Successfully erased %zu bytes @ %#x\n", len, start);

out:
	spi_release_bus(&flash->spi);
	return ret;
}

static void ich_read_data(uint8_t *data, int len)
{
	int i;
	uint32_t temp32 = 0;

	for (i = 0; i < len; i++) {
		if ((i % 4) == 0)
			temp32 = readl_(cntlr.data + i);

		data[i] = (temp32 >> ((i % 4) * 8)) & 0xff;
	}
}

static int ich_hwseq_read(const struct spi_flash *flash, u32 addr, size_t len,
			void *buf)
{
	uint16_t hsfc;
	uint16_t timeout = 100 * 60;
	uint8_t block_len;

	if (addr + len > flash->size) {
		printk(BIOS_ERR,
			"Attempt to read %x-%x which is out of chip\n",
			(unsigned int)addr,
			(unsigned int)addr+(unsigned int) len);
		return -1;
	}

	/* clear FDONE, FCERR, AEL by writing 1 to them (if they are set) */
	writew_(readw_(&cntlr.ich9_spi->hsfs), &cntlr.ich9_spi->hsfs);

	while (len > 0) {
		block_len = MIN(len, cntlr.databytes);
		if (block_len > (~addr & 0xff))
			block_len = (~addr & 0xff) + 1;
		ich_hwseq_set_addr(addr);
		hsfc = readw_(&cntlr.ich9_spi->hsfc);
		hsfc &= ~HSFC_FCYCLE; /* set read operation */
		hsfc &= ~HSFC_FDBC; /* clear byte count */
		/* set byte count */
		hsfc |= (((block_len - 1) << HSFC_FDBC_OFF) & HSFC_FDBC);
		hsfc |= HSFC_FGO; /* start */
		writew_(hsfc, &cntlr.ich9_spi->hsfc);

		if (ich_hwseq_wait_for_cycle_complete(timeout, block_len))
			return 1;
		ich_read_data(buf, block_len);
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
static void ich_fill_data(const uint8_t *data, int len)
{
	uint32_t temp32 = 0;
	int i;

	if (len <= 0)
		return;

	for (i = 0; i < len; i++) {
		if ((i % 4) == 0)
			temp32 = 0;

		temp32 |= ((uint32_t)data[i]) << ((i % 4) * 8);

		if ((i % 4) == 3) /* 32 bits are full, write them to regs. */
			writel_(temp32, cntlr.data + (i - (i % 4)));
	}
	i--;
	if ((i % 4) != 3) /* Write remaining data to regs. */
		writel_(temp32, cntlr.data + (i - (i % 4)));
}

static int ich_hwseq_write(const struct spi_flash *flash, u32 addr, size_t len,
			const void *buf)
{
	uint16_t hsfc;
	uint16_t timeout = 100 * 60;
	uint8_t block_len;
	uint32_t start = addr;

	if (addr + len > flash->size) {
		printk(BIOS_ERR,
			"Attempt to write 0x%x-0x%x which is out of chip\n",
			(unsigned int)addr, (unsigned int)(addr+len));
		return -1;
	}

	/* clear FDONE, FCERR, AEL by writing 1 to them (if they are set) */
	writew_(readw_(&cntlr.ich9_spi->hsfs), &cntlr.ich9_spi->hsfs);

	while (len > 0) {
		block_len = MIN(len, cntlr.databytes);
		if (block_len > (~addr & 0xff))
			block_len = (~addr & 0xff) + 1;

		ich_hwseq_set_addr(addr);

		ich_fill_data(buf, block_len);
		hsfc = readw_(&cntlr.ich9_spi->hsfc);
		hsfc &= ~HSFC_FCYCLE; /* clear operation */
		hsfc |= (0x2 << HSFC_FCYCLE_OFF); /* set write operation */
		hsfc &= ~HSFC_FDBC; /* clear byte count */
		/* set byte count */
		hsfc |= (((block_len - 1) << HSFC_FDBC_OFF) & HSFC_FDBC);
		hsfc |= HSFC_FGO; /* start */
		writew_(hsfc, &cntlr.ich9_spi->hsfc);

		if (ich_hwseq_wait_for_cycle_complete(timeout, block_len)) {
			printk(BIOS_ERR, "SF: write failure at %x\n",
				addr);
			return -1;
		}
		addr += block_len;
		buf += block_len;
		len -= block_len;
	}
	printk(BIOS_DEBUG, "SF: Successfully written %u bytes @ %#x\n",
	       (unsigned int)(addr - start), start);
	return 0;
}

static const struct spi_flash_ops spi_flash_ops = {
	.read = ich_hwseq_read,
	.write = ich_hwseq_write,
	.erase = ich_hwseq_erase,
};

static int spi_flash_programmer_probe(const struct spi_slave *spi,
					struct spi_flash *flash)
{

	if (CONFIG(SOUTHBRIDGE_INTEL_I82801GX))
		return spi_flash_generic_probe(spi, flash);

	/* Try generic probing first if spi_is_multichip returns 0. */
	if (!spi_is_multichip() && !spi_flash_generic_probe(spi, flash))
		return 0;

	memcpy(&flash->spi, spi, sizeof(*spi));

	ich_hwseq_set_addr(0);
	switch ((cntlr.hsfs >> 3) & 3) {
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

	flash->size = 1 << (19 + (cntlr.flcomp & 7));

	flash->ops = &spi_flash_ops;

	if ((cntlr.hsfs & HSFS_FDV) && ((cntlr.flmap0 >> 8) & 3))
		flash->size += 1 << (19 + ((cntlr.flcomp >> 3) & 7));
	printk(BIOS_DEBUG, "flash size 0x%x bytes\n", flash->size);

	return 0;
}

static int xfer_vectors(const struct spi_slave *slave,
			struct spi_op vectors[], size_t count)
{
	return spi_flash_vector_helper(slave, vectors, count, spi_ctrlr_xfer);
}

#define SPI_FPR_SHIFT			12
#define ICH7_SPI_FPR_MASK		0xfff
#define ICH9_SPI_FPR_MASK		0x1fff
#define SPI_FPR_BASE_SHIFT		0
#define ICH7_SPI_FPR_LIMIT_SHIFT	12
#define ICH9_SPI_FPR_LIMIT_SHIFT	16
#define ICH9_SPI_FPR_RPE		(1 << 15) /* Read Protect */
#define SPI_FPR_WPE			(1 << 31) /* Write Protect */

static u32 spi_fpr(u32 base, u32 limit)
{
	u32 ret;
	u32 mask, limit_shift;

	if (CONFIG(SOUTHBRIDGE_INTEL_I82801GX)) {
		mask = ICH7_SPI_FPR_MASK;
		limit_shift = ICH7_SPI_FPR_LIMIT_SHIFT;
	} else {
		mask = ICH9_SPI_FPR_MASK;
		limit_shift = ICH9_SPI_FPR_LIMIT_SHIFT;
	}
	ret = ((limit >> SPI_FPR_SHIFT) & mask) << limit_shift;
	ret |= ((base >> SPI_FPR_SHIFT) & mask) << SPI_FPR_BASE_SHIFT;
	return ret;
}

/*
 * Protect range of SPI flash defined by [start, start+size-1] using Flash
 * Protected Range (FPR) register if available.
 * Returns 0 on success, -1 on failure of programming fpr registers.
 */
static int spi_flash_protect(const struct spi_flash *flash,
			const struct region *region,
			const enum ctrlr_prot_type type)
{
	u32 start = region_offset(region);
	u32 end = start + region_sz(region) - 1;
	u32 reg;
	u32 protect_mask = 0;
	int fpr;
	uint32_t *fpr_base;

	fpr_base = cntlr.fpr;

	/* Find first empty FPR */
	for (fpr = 0; fpr < cntlr.fpr_max; fpr++) {
		reg = read32(&fpr_base[fpr]);
		if (reg == 0)
			break;
	}

	if (fpr == cntlr.fpr_max) {
		printk(BIOS_ERR, "No SPI FPR free!\n");
		return -1;
	}

	switch (type) {
	case WRITE_PROTECT:
		protect_mask |= SPI_FPR_WPE;
		break;
	case READ_PROTECT:
		if (CONFIG(SOUTHBRIDGE_INTEL_I82801GX))
			return -1;
		protect_mask |= ICH9_SPI_FPR_RPE;
		break;
	case READ_WRITE_PROTECT:
		if (CONFIG(SOUTHBRIDGE_INTEL_I82801GX))
			return -1;
		protect_mask |= (ICH9_SPI_FPR_RPE | SPI_FPR_WPE);
		break;
	default:
		printk(BIOS_ERR, "Seeking invalid protection!\n");
		return -1;
	}

	/* Set protected range base and limit */
	reg = spi_fpr(start, end) | protect_mask;

	/* Set the FPR register and verify it is protected */
	write32(&fpr_base[fpr], reg);
	if (reg != read32(&fpr_base[fpr])) {
		printk(BIOS_ERR, "Unable to set SPI FPR %d\n", fpr);
		return -1;
	}

	printk(BIOS_INFO, "%s: FPR %d is enabled for range 0x%08x-0x%08x\n",
	       __func__, fpr, start, end);
	return 0;
}

void spi_finalize_ops(void)
{
	u16 spi_opprefix;
	u16 optype = 0;
	struct intel_swseq_spi_config spi_config_default = {
		{0x06, 0x50},  /* OPPREFIXES: EWSR and WREN */
		{ /* OPCODE and OPTYPE */
			{0x01, WRITE_NO_ADDR},		/* WRSR: Write Status Register */
			{0x02, WRITE_WITH_ADDR},	/* BYPR: Byte Program */
			{0x03, READ_WITH_ADDR},		/* READ: Read Data */
			{0x05, READ_NO_ADDR},		/* RDSR: Read Status Register */
			{0x20, WRITE_WITH_ADDR},	/* SE20: Sector Erase 0x20 */
			{0x9f, READ_NO_ADDR},		/* RDID: Read ID */
			{0xd8, WRITE_WITH_ADDR},	/* BED8: Block Erase 0xd8 */
			{0x0b, READ_WITH_ADDR},		/* FAST: Fast Read */
		}
	};
	struct intel_swseq_spi_config spi_config_aai_write = {
		{0x06, 0x50}, /* OPPREFIXES: EWSR and WREN */
		{ /* OPCODE and OPTYPE */
			{0x01, WRITE_NO_ADDR},		/* WRSR: Write Status Register */
			{0x02, WRITE_WITH_ADDR},	/* BYPR: Byte Program */
			{0x03, READ_WITH_ADDR},		/* READ: Read Data */
			{0x05, READ_NO_ADDR},		/* RDSR: Read Status Register */
			{0x20, WRITE_WITH_ADDR},	/* SE20: Sector Erase 0x20 */
			{0x9f, READ_NO_ADDR},		/* RDID: Read ID */
			{0xad, WRITE_NO_ADDR},		/* Auto Address Increment Word Program */
			{0x04, WRITE_NO_ADDR}		/* Write Disable */
		}
	};
	const struct spi_flash *flash = boot_device_spi_flash();
	struct intel_swseq_spi_config *spi_config = &spi_config_default;
	int i;

	/*
	 * Some older SST SPI flashes support AAI write but use 0xaf opcde for
	 * that. Flashrom uses the byte program opcode to write those flashes,
	 * so this configuration is fine too. SST25VF064C (id = 0x4b) is an
	 * exception.
	 */
	if (flash && flash->vendor == VENDOR_ID_SST && (flash->model & 0x00ff) != 0x4b)
		spi_config = &spi_config_aai_write;

	if (spi_locked())
		return;

	intel_southbridge_override_spi(spi_config);

	spi_opprefix = spi_config->opprefixes[0]
		| (spi_config->opprefixes[1] << 8);
	writew_(spi_opprefix, cntlr.preop);
	for (i = 0; i < ARRAY_SIZE(spi_config->ops); i++) {
		optype |= (spi_config->ops[i].type & 3) << (i * 2);
		writeb_(spi_config->ops[i].op, &cntlr.opmenu[i]);
	}
	writew_(optype, cntlr.optype);

	spi_set_smm_only_flashing(CONFIG(BOOTMEDIA_SMM_BWP));
}

__weak void intel_southbridge_override_spi(struct intel_swseq_spi_config *spi_config)
{
}

#define BIOS_CNTL		0xdc
#define  BIOS_CNTL_BIOSWE	(1 << 0)
#define  BIOS_CNTL_BLE		(1 << 1)
#define  BIOS_CNTL_SMM_BWP	(1 << 5)

static void spi_set_smm_only_flashing(bool enable)
{
	if (!(CONFIG(SOUTHBRIDGE_INTEL_I82801GX) || CONFIG(SOUTHBRIDGE_INTEL_COMMON_SPI_ICH9)))
		return;

	const pci_devfn_t dev = PCI_DEV(0, 31, 0);

	uint8_t bios_cntl = pci_read_config8(dev, BIOS_CNTL);

	if (enable) {
		bios_cntl &= ~BIOS_CNTL_BIOSWE;
		bios_cntl |= BIOS_CNTL_BLE | BIOS_CNTL_SMM_BWP;
	} else {
		bios_cntl &= ~(BIOS_CNTL_BLE | BIOS_CNTL_SMM_BWP);
		bios_cntl |= BIOS_CNTL_BIOSWE;
	}

	pci_write_config8(dev, BIOS_CNTL, bios_cntl);
}

static const struct spi_ctrlr spi_ctrlr = {
	.xfer_vector = xfer_vectors,
	.max_xfer_size = member_size(struct ich9_spi_regs, fdata),
	.flash_probe = spi_flash_programmer_probe,
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
