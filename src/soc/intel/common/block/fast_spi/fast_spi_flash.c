/* SPDX-License-Identifier: GPL-2.0-only */

#include <device/mmio.h>
#include <commonlib/helpers.h>
#include <console/console.h>
#include <fast_spi_def.h>
#include <intelblocks/fast_spi.h>
#include <soc/pci_devs.h>
#include <spi_flash.h>
#include <string.h>
#include <timer.h>

/* Helper to create a FAST_SPI context on API entry. */
#define BOILERPLATE_CREATE_CTX(ctx)		\
	struct fast_spi_flash_ctx	real_ctx;	\
	struct fast_spi_flash_ctx *ctx = &real_ctx;	\
	_fast_spi_flash_get_ctx(ctx)

/*
 * Anything that's not success is <0. Provided solely for readability, as these
 * constants are not used outside this file.
 */
enum errors {
	SUCCESS			= 0,
	E_TIMEOUT		= -1,
	E_HW_ERROR		= -2,
	E_ARGUMENT		= -3,
};

/* Reduce data-passing burden by grouping transaction data in a context. */
struct fast_spi_flash_ctx {
	uintptr_t mmio_base;
};

static void _fast_spi_flash_get_ctx(struct fast_spi_flash_ctx *ctx)
{
	ctx->mmio_base = (uintptr_t)fast_spi_get_bar();
}

/* Read register from the FAST_SPI flash controller. */
static uint32_t fast_spi_flash_ctrlr_reg_read(struct fast_spi_flash_ctx *ctx,
	uint16_t reg)
{
	uintptr_t addr =  ALIGN_DOWN(ctx->mmio_base + reg, sizeof(uint32_t));
	return read32p(addr);
}

/* Write to register in FAST_SPI flash controller. */
static void fast_spi_flash_ctrlr_reg_write(struct fast_spi_flash_ctx *ctx,
					uint16_t reg, uint32_t val)
{
	uintptr_t addr =  ALIGN_DOWN(ctx->mmio_base + reg, sizeof(uint32_t));
	write32p(addr, val);
}

/*
 * The hardware datasheet is not clear on what HORD values actually do. It
 * seems that HORD_SFDP provides access to the first 8 bytes of the SFDP, which
 * is the signature and revision fields. HORD_JEDEC provides access to the
 * actual flash parameters, and is most likely what you want to use when
 * probing the flash from software.
 * It's okay to rely on SFDP, since the SPI flash controller requires an SFDP
 * 1.5 or newer compliant FAST_SPI flash chip.
 * NOTE: Due to the register layout of the hardware, all accesses will be
 * aligned to a 4 byte boundary.
 */
static uint32_t fast_spi_flash_read_sfdp_param(struct fast_spi_flash_ctx *ctx,
					  uint16_t sfdp_reg)
{
	uint32_t ptinx_index = sfdp_reg & SPIBAR_PTINX_IDX_MASK;
	fast_spi_flash_ctrlr_reg_write(ctx, SPIBAR_PTINX,
				   ptinx_index | SPIBAR_PTINX_HORD_JEDEC);
	return fast_spi_flash_ctrlr_reg_read(ctx, SPIBAR_PTDATA);
}

/* Fill FDATAn FIFO in preparation for a write transaction. */
static void fill_xfer_fifo(struct fast_spi_flash_ctx *ctx, const void *data,
			   size_t len)
{
	/* YES! memcpy() works. FDATAn does not require 32-bit accesses. */
	memcpy((void *)(ctx->mmio_base + SPIBAR_FDATA(0)), data, len);
}

/* Drain FDATAn FIFO after a read transaction populates data. */
static void drain_xfer_fifo(struct fast_spi_flash_ctx *ctx, void *dest,
				size_t len)
{
	/* YES! memcpy() works. FDATAn does not require 32-bit accesses. */
	memcpy(dest, (void *)(ctx->mmio_base + SPIBAR_FDATA(0)), len);
}

/* Fire up a transfer using the hardware sequencer. */
static void start_hwseq_xfer(struct fast_spi_flash_ctx *ctx,
		uint32_t hsfsts_cycle, uint32_t flash_addr, size_t len)
{
	/* Make sure all W1C status bits get cleared. */
	uint32_t hsfsts = SPIBAR_HSFSTS_W1C_BITS;
	/* Set up transaction parameters. */
	hsfsts |= hsfsts_cycle & SPIBAR_HSFSTS_FCYCLE_MASK;
	hsfsts |= SPIBAR_HSFSTS_FDBC(len - 1);

	fast_spi_flash_ctrlr_reg_write(ctx, SPIBAR_FADDR, flash_addr);
	fast_spi_flash_ctrlr_reg_write(ctx, SPIBAR_HSFSTS_CTL,
			     hsfsts | SPIBAR_HSFSTS_FGO);
}

static int wait_for_hwseq_xfer(struct fast_spi_flash_ctx *ctx,
					uint32_t flash_addr)
{
	struct stopwatch sw;
	uint32_t hsfsts;

	stopwatch_init_msecs_expire(&sw, SPIBAR_HWSEQ_XFER_TIMEOUT_MS);
	do {
		hsfsts = fast_spi_flash_ctrlr_reg_read(ctx, SPIBAR_HSFSTS_CTL);

		if (hsfsts & SPIBAR_HSFSTS_FCERR) {
			printk(BIOS_ERR, "SPI Transaction Error at Flash Offset %x HSFSTS = 0x%08x\n",
				flash_addr, hsfsts);
			return E_HW_ERROR;
		}

		if (hsfsts & SPIBAR_HSFSTS_FDONE)
			return SUCCESS;
	} while (!(stopwatch_expired(&sw)));

	printk(BIOS_ERR, "SPI Transaction Timeout (Exceeded %d ms) at Flash Offset %x HSFSTS = 0x%08x\n",
		SPIBAR_HWSEQ_XFER_TIMEOUT_MS, flash_addr, hsfsts);
	return E_TIMEOUT;
}

static int wait_for_hwseq_spi_cycle_complete(struct fast_spi_flash_ctx *ctx)
{
	struct stopwatch sw;
	uint32_t hsfsts;

	stopwatch_init_msecs_expire(&sw, SPIBAR_HWSEQ_XFER_TIMEOUT_MS);
	do {
		hsfsts = fast_spi_flash_ctrlr_reg_read(ctx, SPIBAR_HSFSTS_CTL);

		if (!(hsfsts & SPIBAR_HSFSTS_SCIP))
			return SUCCESS;
	} while (!(stopwatch_expired(&sw)));

	return E_TIMEOUT;
}

/* Execute FAST_SPI flash transfer. This is a blocking call. */
static int exec_sync_hwseq_xfer(struct fast_spi_flash_ctx *ctx,
				uint32_t hsfsts_cycle, uint32_t flash_addr,
				size_t len)
{
	if (wait_for_hwseq_spi_cycle_complete(ctx) != SUCCESS) {
		printk(BIOS_ERR, "SPI Transaction Timeout (Exceeded %d ms) due to prior"
				" operation at Flash Offset %x\n",
				SPIBAR_HWSEQ_XFER_TIMEOUT_MS, flash_addr);
		return E_TIMEOUT;
	}

	start_hwseq_xfer(ctx, hsfsts_cycle, flash_addr, len);
	return wait_for_hwseq_xfer(ctx, flash_addr);
}

int fast_spi_cycle_in_progress(void)
{
	BOILERPLATE_CREATE_CTX(ctx);

	int ret = wait_for_hwseq_spi_cycle_complete(ctx);
	if (ret != SUCCESS)
		printk(BIOS_ERR, "SPI Transaction Timeout (Exceeded %d ms) due to prior"
				" operation is pending\n", SPIBAR_HWSEQ_XFER_TIMEOUT_MS);

	return ret;
}

/*
 * Ensure read/write xfer len is not greater than SPIBAR_FDATA_FIFO_SIZE and
 * that the operation does not cross page boundary.
 */
static size_t get_xfer_len(const struct spi_flash *flash, uint32_t addr,
			   size_t len)
{
	size_t xfer_len = MIN(len, SPIBAR_FDATA_FIFO_SIZE);
	size_t bytes_left = ALIGN_UP(addr, flash->page_size) - addr;

	if (bytes_left)
		xfer_len = MIN(xfer_len, bytes_left);

	return xfer_len;
}

static int fast_spi_flash_erase(const struct spi_flash *flash,
				uint32_t offset, size_t len)
{
	int ret;
	size_t erase_size;
	uint32_t erase_cycle;

	BOILERPLATE_CREATE_CTX(ctx);

	if (!IS_ALIGNED(offset, 4 * KiB) || !IS_ALIGNED(len, 4 * KiB)) {
		printk(BIOS_ERR, "BUG! SPI erase region not sector aligned\n");
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

static int fast_spi_flash_read(const struct spi_flash *flash,
			uint32_t addr, size_t len, void *buf)
{
	int ret;
	size_t xfer_len;
	uint8_t *data = buf;

	BOILERPLATE_CREATE_CTX(ctx);

	while (len) {
		xfer_len = get_xfer_len(flash, addr, len);

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

static int fast_spi_flash_write(const struct spi_flash *flash,
		uint32_t addr, size_t len, const void *buf)
{
	int ret;
	size_t xfer_len;
	const uint8_t *data = buf;

	BOILERPLATE_CREATE_CTX(ctx);

	while (len) {
		xfer_len = get_xfer_len(flash, addr, len);
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

static int fast_spi_flash_status(const struct spi_flash *flash,
						uint8_t *reg)
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

const struct spi_flash_ops fast_spi_flash_ops = {
	.read = fast_spi_flash_read,
	.write = fast_spi_flash_write,
	.erase = fast_spi_flash_erase,
	.status = fast_spi_flash_status,
};

/*
 * We can't use FDOC and FDOD to read FLCOMP, as previous platforms did.
 * For details see:
 * Ch 31, SPI: p. 194
 * The size of the flash component is always taken from density field in the
 * SFDP table. FLCOMP.C0DEN is no longer used by the Flash Controller.
 */
static int fast_spi_flash_probe(const struct spi_slave *dev,
				struct spi_flash *flash)
{
	BOILERPLATE_CREATE_CTX(ctx);
	uint32_t flash_bits;

	/*
	 * bytes = (bits + 1) / 8;
	 * But we need to do the addition in a way which doesn't overflow for
	 * 4 Gbit devices (flash_bits == 0xffffffff).
	 */
	flash_bits = fast_spi_flash_read_sfdp_param(ctx, 0x04);
	flash->size = (flash_bits >> 3) + 1;

	memcpy(&flash->spi, dev, sizeof(*dev));

	/* Can erase both 4 KiB and 64 KiB chunks. Declare the smaller size. */
	flash->sector_size = 4 * KiB;
	flash->page_size = 256;
	/*
	 * FIXME: Get erase+cmd, and status_cmd from SFDP.
	 *
	 * flash->erase_cmd = ???
	 * flash->status_cmd = ???
	 */

	flash->ops = &fast_spi_flash_ops;
	return 0;
}

static int fast_spi_flash_ctrlr_setup(const struct spi_slave *dev)
{
	if (dev->cs != 0) {
		printk(BIOS_ERR, "%s: Invalid CS for fast SPI bus=0x%x,cs=0x%x!\n",
		       __func__, dev->bus, dev->cs);
		return -1;
	}

	return 0;
}

#define SPI_FPR_SHIFT		12
#define SPI_FPR_MASK		0x7fff
#define SPI_FPR_BASE_SHIFT	0
#define SPI_FPR_LIMIT_SHIFT	16
#define SPI_FPR_RPE		(1 << 15) /* Read Protect */
#define SPI_FPR_WPE		(1 << 31) /* Write Protect */
#define SPI_FPR(base, limit)	\
	(((((limit) >> SPI_FPR_SHIFT) & SPI_FPR_MASK) << SPI_FPR_LIMIT_SHIFT) |\
	 ((((base) >> SPI_FPR_SHIFT) & SPI_FPR_MASK) << SPI_FPR_BASE_SHIFT))

/*
 * Protect range of SPI flash defined by [start, start+size-1] using Flash
 * Protected Range (FPR) register if available.
 */
static int fast_spi_flash_protect(const struct spi_flash *flash,
				  const struct region *region,
				  const enum ctrlr_prot_type type)
{
	u32 start = region_offset(region);
	u32 end = start + region_sz(region) - 1;
	u32 reg;
	u32 protect_mask = 0;
	int fpr;
	uintptr_t fpr_base;
	BOILERPLATE_CREATE_CTX(ctx);

	fpr_base = ctx->mmio_base + SPIBAR_FPR_BASE;

	/* Find first empty FPR */
	for (fpr = 0; fpr < SPIBAR_FPR_MAX; fpr++) {
		reg = read32p(fpr_base);
		if (reg == 0)
			break;
		fpr_base += sizeof(uint32_t);
	}

	if (fpr >= SPIBAR_FPR_MAX) {
		printk(BIOS_ERR, "No SPI FPR free!\n");
		return -1;
	}

	switch (type) {
	case WRITE_PROTECT:
		protect_mask |= SPI_FPR_WPE;
		break;
	case READ_PROTECT:
		protect_mask |= SPI_FPR_RPE;
		break;
	case READ_WRITE_PROTECT:
		protect_mask |= (SPI_FPR_RPE | SPI_FPR_WPE);
		break;
	default:
		printk(BIOS_ERR, "Seeking invalid protection!\n");
		return -1;
	}

	/* Set protected range base and limit */
	reg = SPI_FPR(start, end) | protect_mask;

	/* Set the FPR register and verify it is protected */
	write32p(fpr_base, reg);
	reg = read32p(fpr_base);
	if (!(reg & protect_mask)) {
		printk(BIOS_ERR, "Unable to set SPI FPR %d\n", fpr);
		return -1;
	}

	printk(BIOS_INFO, "%s: FPR %d is enabled for range 0x%08x-0x%08x\n",
	       __func__, fpr, start, end);
	return 0;
}

const struct spi_ctrlr fast_spi_flash_ctrlr = {
	.setup = fast_spi_flash_ctrlr_setup,
	.max_xfer_size = SPI_CTRLR_DEFAULT_MAX_XFER_SIZE,
	.flash_probe = fast_spi_flash_probe,
	.flash_protect = fast_spi_flash_protect,
};
