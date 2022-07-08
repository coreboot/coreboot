/* SPDX-License-Identifier: GPL-2.0-only */

#include <amdblocks/lpc.h>
#include <amdblocks/spi.h>
#include <assert.h>
#include <boot_device.h>
#include <commonlib/bsd/helpers.h>
#include <commonlib/region.h>
#include <console/console.h>
#include <delay.h>
#include <device/pci_ops.h>
#include <soc/pci_devs.h>
#include <spi_flash.h>
#include <string.h>
#include <thread.h>
#include <types.h>

/* The ROM is memory mapped just below 4GiB. Form a pointer for the base. */
#define rom_base ((void *)(uintptr_t)(0x100000000ULL - CONFIG_ROM_SIZE))

struct spi_dma_transaction {
	uint8_t *destination;
	size_t source;
	size_t size;
	size_t transfer_size;
	size_t remaining;
};

static void *spi_dma_mmap(const struct region_device *rd, size_t offset,
			  size_t size __always_unused)
{
	const struct mem_region_device *mdev;

	mdev = container_of(rd, __typeof__(*mdev), rdev);

	return &mdev->base[offset];
}

static int spi_dma_munmap(const struct region_device *rd __always_unused,
			  void *mapping __always_unused)
{
	return 0;
}

static ssize_t spi_dma_readat_mmap(const struct region_device *rd, void *b, size_t offset,
				   size_t size)
{
	const struct mem_region_device *mdev;

	mdev = container_of(rd, __typeof__(*mdev), rdev);

	memcpy(b, &mdev->base[offset], size);

	return size;
}

static bool spi_dma_is_busy(void)
{
	return pci_read_config32(SOC_LPC_DEV, LPC_ROM_DMA_EC_HOST_CONTROL)
	       & LPC_ROM_DMA_CTRL_START;
}

static bool spi_dma_has_error(void)
{
	return pci_read_config32(SOC_LPC_DEV, LPC_ROM_DMA_EC_HOST_CONTROL)
	       & LPC_ROM_DMA_CTRL_ERROR;
}

static bool can_use_dma(void *destination, size_t source, size_t size)
{
	/*
	 * Print a notice if reading more than 1024 bytes using mmap. This makes
	 * it easier to debug why the SPI DMA wasn't used.
	 */
	const size_t warning_size = 1024;

	if (size < LPC_ROM_DMA_MIN_ALIGNMENT)
		return false;

	if (!IS_ALIGNED((uintptr_t)destination, LPC_ROM_DMA_MIN_ALIGNMENT)) {
		if (size > warning_size)
			printk(BIOS_DEBUG, "Target %p is unaligned\n", destination);
		return false;
	}

	if (!IS_ALIGNED(source, LPC_ROM_DMA_MIN_ALIGNMENT)) {
		if (size > warning_size)
			printk(BIOS_DEBUG, "Source %#zx is unaligned\n", source);
		return false;
	}

	return true;
}

static void start_spi_dma_transaction(struct spi_dma_transaction *transaction)
{
	uint32_t ctrl;

	printk(BIOS_SPEW, "%s: dest: %p, source: %#zx, remaining: %zu\n", __func__,
	       transaction->destination, transaction->source, transaction->remaining);

	/*
	 * We should have complete control over the DMA controller, so there shouldn't
	 * be any outstanding transactions.
	 */
	assert(!spi_dma_is_busy());
	assert(IS_ALIGNED((uintptr_t)transaction->destination, LPC_ROM_DMA_MIN_ALIGNMENT));
	assert(IS_ALIGNED(transaction->source, LPC_ROM_DMA_MIN_ALIGNMENT));
	assert(transaction->remaining >= LPC_ROM_DMA_MIN_ALIGNMENT);

	pci_write_config32(SOC_LPC_DEV, LPC_ROM_DMA_SRC_ADDR, transaction->source);
	pci_write_config32(SOC_LPC_DEV, LPC_ROM_DMA_DST_ADDR,
			   (uintptr_t)transaction->destination);

	ctrl = pci_read_config32(SOC_LPC_DEV, LPC_ROM_DMA_EC_HOST_CONTROL);
	ctrl &= ~LPC_ROM_DMA_CTRL_DW_COUNT_MASK;

	transaction->transfer_size =
		MIN(LPC_ROM_DMA_CTRL_MAX_BYTES,
		    ALIGN_DOWN(transaction->remaining, LPC_ROM_DMA_MIN_ALIGNMENT));

	ctrl |= LPC_ROM_DMA_CTRL_DW_COUNT(transaction->transfer_size);
	ctrl |= LPC_ROM_DMA_CTRL_ERROR; /* Clear error */
	ctrl |= LPC_ROM_DMA_CTRL_START;

	/*
	 * Ensure we have exclusive access to the SPI controller before starting the LPC SPI DMA
	 * transaction.
	 */
	thread_mutex_lock(&spi_hw_mutex);

	pci_write_config32(SOC_LPC_DEV, LPC_ROM_DMA_EC_HOST_CONTROL, ctrl);
}

/* Returns true if transaction is still in progress. */
static bool continue_spi_dma_transaction(const struct region_device *rd,
					 struct spi_dma_transaction *transaction)
{
	/* Verify we are looking at the correct transaction */
	assert(pci_read_config32(SOC_LPC_DEV, LPC_ROM_DMA_SRC_ADDR) == transaction->source);

	if (spi_dma_is_busy())
		return true;

	/*
	 * Unlock the SPI mutex between DMA transactions to allow other users of the SPI
	 * controller to interleave their transactions.
	 */
	thread_mutex_unlock(&spi_hw_mutex);

	if (spi_dma_has_error()) {
		printk(BIOS_ERR, "SPI DMA failure: dest: %p, source: %#zx, size: %zu\n",
		       transaction->destination, transaction->source,
		       transaction->transfer_size);
		return false;
	}

	transaction->destination += transaction->transfer_size;
	transaction->source += transaction->transfer_size;
	transaction->remaining -= transaction->transfer_size;

	if (transaction->remaining >= LPC_ROM_DMA_MIN_ALIGNMENT) {
		start_spi_dma_transaction(transaction);
		return true;
	}

	if (transaction->remaining > 0) {
		/* Use mmap to finish off the transfer */
		spi_dma_readat_mmap(rd, transaction->destination, transaction->source,
				    transaction->remaining);

		transaction->destination += transaction->remaining;
		transaction->source += transaction->remaining;
		transaction->remaining -= transaction->remaining;
	}

	return false;
}

static struct thread_mutex spi_dma_hw_mutex;

static ssize_t spi_dma_readat_dma(const struct region_device *rd, void *destination,
				  size_t source, size_t size)
{
	struct spi_dma_transaction transaction = {
		.destination = destination,
		.source = source,
		.size = size,
		.remaining = size,
	};

	printk(BIOS_SPEW, "%s: start: dest: %p, source: %#zx, size: %zu\n", __func__,
	       destination, source, size);

	thread_mutex_lock(&spi_dma_hw_mutex);

	start_spi_dma_transaction(&transaction);

	do {
		udelay(2);
	} while (continue_spi_dma_transaction(rd, &transaction));

	thread_mutex_unlock(&spi_dma_hw_mutex);

	printk(BIOS_SPEW, "%s: end: dest: %p, source: %#zx, remaining: %zu\n",
	       __func__, destination, source, transaction.remaining);

	/* Allow queued up transaction to continue */
	thread_yield();

	if (transaction.remaining)
		return -1;

	return transaction.size;
}

static ssize_t spi_dma_readat(const struct region_device *rd, void *b, size_t offset,
			      size_t size)
{
	if (can_use_dma(b, offset, size))
		return spi_dma_readat_dma(rd, b, offset, size);
	else
		return spi_dma_readat_mmap(rd, b, offset, size);
}

const struct region_device_ops spi_dma_rdev_ro_ops = {
	.mmap = spi_dma_mmap,
	.munmap = spi_dma_munmap,
	.readat = spi_dma_readat,
};

static const struct mem_region_device boot_dev = {
	.base = rom_base,
	.rdev = REGION_DEV_INIT(&spi_dma_rdev_ro_ops, 0, CONFIG_ROM_SIZE),
};

const struct region_device *boot_device_ro(void)
{
	return &boot_dev.rdev;
}

uint32_t spi_flash_get_mmap_windows(struct flash_mmap_window *table)
{
	table->flash_base = 0;
	table->host_base = (uint32_t)(uintptr_t)rom_base;
	table->size = CONFIG_ROM_SIZE;

	return 1;
}

/*
 * Without this magic bit, the SPI DMA controller will write 0s into the destination if an MMAP
 * read happens while a DMA transaction is in progress. i.e., PSP is reading from SPI. The bit
 * that fixes this was added to Cezanne, Renoir and later SoCs. So the SPI DMA controller is not
 * reliable on any prior generations.
 */
static void spi_dma_fix(void)
{
	/* Internal only registers */
	uint8_t val = spi_read8(0xfc);
	val |= BIT(6);
	spi_write8(0xfc, val);
}

void boot_device_init(void)
{
	spi_dma_fix();
}
