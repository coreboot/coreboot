/* SPDX-License-Identifier: GPL-2.0-only */

#include <console/console.h>
#include <delay.h>
#include <device/pci.h>
#include <fast_spi_dma.h>
#include <static.h>
#include <thread.h>
#include <timer.h>

#define REGION_BIOS	1

/* BIOS DMA Fast SPI registers */
enum fast_spi_dma_register {
	FAST_SPI_DMA_DEST_ADDRESS_HIGH = 0xb0,
	FAST_SPI_DMA_DEST_ADDRESS_LOW = 0xb4,
	FAST_SPI_DMA_REGION = 0xb8,
	FAST_SPI_DMA_CONTROL = 0xbc,
	FAST_SPI_DMA_STATUS = 0xc0
};

/* FAST_SPI_DMA_REGION */
union fast_spi_dma_region {
	uint32_t data;
	struct {
		uint32_t reserved: 16;
		uint32_t offset_kb: 16;
	} fields;
};

/* FAST_SPI_DMA_CONTROL */
union fast_spi_dma_control {
	uint32_t data;
	struct {
		uint32_t start: 1;
		uint32_t reserved0: 5;
		uint32_t reset_write_pointer: 1;
		uint32_t reserved1: 1;
		uint32_t flash_region: 4;
		uint32_t reserved2: 3;
		uint32_t lock: 1;
		uint32_t size: 16;
	} fields;
};

/* FAST_SPI_DMA_STATUS */
union fast_spi_dma_status {
	uint32_t data;
	struct {
		uint32_t reserved0: 2;
		uint32_t complete: 1;
		uint32_t error: 5;
		uint32_t reserved1: 8;
		uint32_t write_pointer: 16;
	} fields;
};

#define FAST_SPI_DMA_BLOCK_ALIGN	KiB
#define FAST_SPI_DMA_TIMEOUT_MSEC	1000
#define FAST_SPI_DMA_DELAY_STEP_USEC	200
#define FAST_SPI_DEV			DEV_PTR(fast_spi)

/*
 * The bios_top variable stores the offset indicating the end of the BIOS region within
 * the SPI flash memory map. It is calculated based on the size and arrangement of memory
 * regions within the flash device. This value is used in DMA operations to determine the
 * correct addressing and transfer boundaries.
 */
static uint32_t bios_top;

/*
 * Pointer to the operations structure for memory-mapped region devices. This pointer is
 * used to store and access the original operations associated with the memory-mapped
 * device, allowing for custom operations such as DMA transfers to be installed and
 * utilized while preserving the original functionalities.
 */
static const struct region_device_ops *mmap_ops;

/*
 * The fast_spi_dma_mutex is used to ensure thread-safe access to DMA operations. This
 * synchronization mechanism guarantees that only one thread can perform DMA operations
 * at a time.
 */
static struct thread_mutex fast_spi_dma_mutex;

static inline uint32_t fast_spi_read(enum fast_spi_dma_register reg)
{
	return pci_read_config32(FAST_SPI_DEV, reg);
}

static inline void fast_spi_write(enum fast_spi_dma_register reg, uint32_t value)
{
	pci_write_config32(FAST_SPI_DEV, reg, value);
}

/*
 * Perform a DMA transfer using the Fast SPI interface.
 *
 * This function initiates a DMA transfer from the SPI flash to a specified buffer. It
 * configures the necessary registers, handles potential errors, and ensures proper
 * completion of the DMA operation. If the operation succeeds, it returns CB_SUCCESS;
 * otherwise, it returns CB_ERR.
 *
 * @param buffer Pointer to the destination buffer where the data will be transferred.
 * @param size Number of blocks to transfer, specified in 1KB units.
 * @param offset Offset within the flash region from where the transfer begins.
 *
 * @return CB_SUCCESS if the DMA transfer completes successfully,
 *         CB_ERR if the operation encounters an error or times out.
 */
static enum cb_err fast_spi_do_dma_transfer(void *buffer, uint32_t size, uint32_t offset)
{
	enum cb_err ret = CB_ERR;

	union fast_spi_dma_control control = {fast_spi_read(FAST_SPI_DMA_CONTROL)};
	union fast_spi_dma_status status = {fast_spi_read(FAST_SPI_DMA_STATUS)};

	/* Verify if the DMA is locked; if so, exit. FSP-S should lock the SPI DMA prior
	   to exiting.  */
	if (control.fields.lock) {
		printk(BIOS_WARNING, "Fast-SPI: DMA is locked\n");
		goto out;
	}

	if (control.fields.start && !status.fields.complete) {
		printk(BIOS_ERR, "Fast-SPI: DMA operation is still ongoing...\n");
		goto out;
	}

	/* Clear any error bits in the status register. */
	if (status.fields.error)
		fast_spi_write(FAST_SPI_DMA_STATUS, status.data);

	/* Configure the destination address for the DMA transfer. */
	fast_spi_write(FAST_SPI_DMA_DEST_ADDRESS_HIGH, (uint32_t)((uintptr_t)buffer >> 32));
	fast_spi_write(FAST_SPI_DMA_DEST_ADDRESS_LOW, (uint32_t)((uintptr_t)buffer));

	/* Ensure the PCI command master bit is set. */
	uint16_t pci_command = pci_read_config16(FAST_SPI_DEV, PCI_COMMAND);
	if (!(pci_command & PCI_COMMAND_MASTER))
		pci_write_config16(FAST_SPI_DEV, PCI_COMMAND,
				   pci_command | PCI_COMMAND_MASTER);

	/* Configure the region offset for the transfer. */
	union fast_spi_dma_region region = {fast_spi_read(FAST_SPI_DMA_REGION)};
	region.fields.offset_kb = offset - 1;
	fast_spi_write(FAST_SPI_DMA_REGION, region.data);

	/* Set the buffer size and start the DMA transaction. */
	control.fields.size = size - 1;
	control.fields.start = 1;
	fast_spi_write(FAST_SPI_DMA_CONTROL, control.data);

	printk(BIOS_DEBUG, "Fast-SPI: Reading %d blocks via DMA\n", size);

	/* Initialize a stopwatch to track the timeout period. */
	struct stopwatch timer;
	stopwatch_init_msecs_expire(&timer, FAST_SPI_DMA_TIMEOUT_MSEC);

	/* Poll for completion of the DMA operation. */
	do {
		udelay(FAST_SPI_DMA_DELAY_STEP_USEC);

		control.data = fast_spi_read(FAST_SPI_DMA_CONTROL);
		status.data = fast_spi_read(FAST_SPI_DMA_STATUS);

		if (status.fields.complete || status.fields.error || control.fields.lock) {
			break;
		}
	} while (!stopwatch_expired(&timer));

	if (status.fields.error) {
		printk(BIOS_ERR, "Fast-SPI: error(s) detected, 0x%08x\n", status.data);
		goto out;
	}

	/* Check if the DMA has been locked since we started the DMA transaction; if it
	   is locked, exit with an error as we cannot guarantee that the data has been
	   loaded prior to locking. */
	if (control.fields.lock) {
		printk(BIOS_ERR, "Fast-SPI: DMA has been locked\n");
		goto out;
	}

	/* Check if the operation has timed out. */
	if (!status.fields.complete && stopwatch_expired(&timer)) {
		printk(BIOS_ERR, "Fast-SPI: DMA operation timed out\n");
		goto out;
	}

	printk(BIOS_DEBUG, "Fast-SPI: transfer completed in %" PRId64 " us\n",
	       stopwatch_duration_usecs(&timer));

	/* Clear the complete status. */
	status.data = 0;
	status.fields.complete = 1;
	fast_spi_write(FAST_SPI_DMA_STATUS, status.data);

	/* Restore PCI command if it was changed. */
	if (!(pci_command & PCI_COMMAND_MASTER))
		pci_write_config16(FAST_SPI_DEV, PCI_COMMAND, pci_command);

	ret = CB_SUCCESS;

out:
	return ret;
}

static enum cb_err fast_spi_dma_mmap_readat(const struct region_device *rd, void *b,
					    size_t offset, size_t size, ssize_t *transferred)
{
	ssize_t count = mmap_ops->readat(rd, b, offset, size);
	if (count < 0) {
		printk(BIOS_ERR, "Fast-SPI: Failed to read %zu bytes", size);
		return CB_ERR;
	}
	*transferred += count;
	if (count != size) {
		printk(BIOS_ERR, "Fast-SPI: Partial read detected %zu/%zu bytes",
		       count, size);
		return CB_ERR;
	}

	return CB_SUCCESS;
}

/*
 * Perform DMA transfer from SPI flash to a buffer.
 *
 * This function is responsible for transferring data blocks from SPI flash memory to a
 * specified buffer using DMA. It handles alignment of memory addresses, calculates the
 * correct offset and block count, performs the DMA transfer, and falls back to
 * memory-mapped operations if DMA transfer fails. Additionally, it ensures that leading
 * and trailing bytes around the DMA blocks are correctly read and transferred.
 *
 * Address transformation:
 *
 * The address needs to be transformed from a memory map address into a DMA
 * address. Consider the following diagram illustrating SPINOR addressing with a 32 MB
 * SPI flash as an example:
 *
 * - On the left is memory addressing
 * - On the right is DMA addressing
 *
 *      BIOS end (32M) --->   +--------------+  |  <- DMA start (0)
 *                         ^  |              |  |
 *                         |  |              |  |
 *      offset ----------> |  |  <address>   |  |  <- bios end - offset
 *                         |  |              |  |
 *                         |  |              |  |
 *                         |  |              |  |
 *                         |  |              |  |
 *                         |  |              |  |
 *      BIOS start (9M) -> |  +--------------+  v  <- DMA end (32 - 9 = 23M)
 *                         |  | DESC + other |
 *      bottom (0M) -----> |  +--------------+
 *
 * @param rd Pointer to the region device structure, representing the memory region.
 * @param b Pointer to the destination buffer where data will be transferred.
 * @param offset Address offset from where the transfer begins.
 * @param size Total number of bytes to transfer.
 *
 * @return The total number of bytes transferred if successful, or -1 if the DMA
 *         transfer fails and fallback to memory map operation is required.
 */
static ssize_t fast_spi_dma_transfer(const struct region_device *rd, void *b,
				     size_t offset, size_t size)
{
	/* Calculate aligned offset and flash offset for DMA transfer. */
	uint32_t aligned_offset = ALIGN_UP(offset, FAST_SPI_DMA_BLOCK_ALIGN);
	uint32_t flash_offset = (bios_top - aligned_offset) / FAST_SPI_DMA_BLOCK_ALIGN;
	ssize_t early_bytes = aligned_offset - offset;
	uint32_t block_count = (size - early_bytes) / FAST_SPI_DMA_BLOCK_ALIGN;

	/* Perform DMA transfer. */
	thread_mutex_lock(&fast_spi_dma_mutex);
	enum cb_err ret = fast_spi_do_dma_transfer(b, block_count, flash_offset);
	thread_mutex_unlock(&fast_spi_dma_mutex);
	if (ret != CB_SUCCESS) {
		printk(BIOS_ERR, "Fast-SPI: DMA transfer failed, falling back to memory map operation\n");
		return -1;
	}

	/* Handle bytes preceding DMA blocks. */
	ssize_t transferred = block_count * FAST_SPI_DMA_BLOCK_ALIGN;
	if (early_bytes) {
		memmove((char *)b + early_bytes, b, transferred);
		ret = fast_spi_dma_mmap_readat(rd, b, offset, early_bytes, &transferred);
		if (ret != CB_SUCCESS) {
			printk(BIOS_ERR, "Fast-SPI: Failed to read leading DMA block bytes");
			return -1;
		}
	}

	/* Handle bytes following DMA blocks. */
	size_t remaining = size - transferred;
	if (remaining) {
		ret = fast_spi_dma_mmap_readat(rd, (char *)b + transferred,
					       offset + transferred, remaining, &transferred);
		if (ret != CB_SUCCESS) {
			printk(BIOS_ERR, "Fast-SPI: Failed to read the remaining bytes");
			return transferred;
		}
	}
	return transferred;
}

/*
 * Perform a read operation using DMA if applicable.
 *
 * This function attempts to perform a read operation using DMA for the specified region
 * device if the size and alignment conditions are met. If the conditions are not met or
 * if the DMA operation fails, it falls back to using the memory-mapped read operation.
 *
 * @param rd Pointer to the region device structure, representing the memory region.
 * @param b Pointer to the destination buffer where data will be read.
 * @param offset Memory map address offset from where the read begins.
 * @param size Total number of bytes to read.
 *
 * @return The total number of bytes read if successful, or the result of the
 *         fallback memory-mapped read operation.
 */
static ssize_t fast_spi_dma_readat(const struct region_device *rd, void *b,
				   size_t offset, size_t size)
{
	/* Check if the size is greater than or equal to the DMA block alignment and if
	   the destination buffer is aligned accordingly. */
	if (size >= FAST_SPI_DMA_BLOCK_ALIGN &&
	    IS_ALIGNED((uintptr_t)b, FAST_SPI_DMA_BLOCK_ALIGN)) {
		ssize_t ret = fast_spi_dma_transfer(rd, b, offset, size);
		if (ret != -1)
			return ret;
	}

	return mmap_ops->readat(rd, b, offset, size);
}

/*
 * Check if Fast SPI DMA is supported by the current device.
 *
 * This function determines whether Fast SPI DMA operations are supported by the current
 * hardware configuration. It performs a one-time initialization to check the PCI vendor
 * ID and the Fast SPI DMA control register. If the vendor ID is invalid or the control
 * register does not indicate support for the BIOS region, the function returns false.
 * Otherwise, it returns true.
 *
 * @return True if Fast SPI DMA is supported; false otherwise.
 */
bool fast_spi_dma_is_supported(void)
{
	static bool initialized, supported;

	if (initialized)
		return supported;
	initialized = true;

	if (pci_read_config32(FAST_SPI_DEV, PCI_VENDOR_ID) == 0xffffffff) {
		printk(BIOS_ERR, "Fast-SPI: DMA is not supported\n");
		return false;
	}

	union fast_spi_dma_control control = {fast_spi_read(FAST_SPI_DMA_CONTROL)};
	supported = control.data != 0xffffffff &&
		control.fields.flash_region == REGION_BIOS;
	return supported;
}

/*
 * Install custom DMA operations for SPI flash memory access.
 *
 * This function sets up a specialized set of operations to access SPI flash memory
 * regions using DMA transfers. The new operation set is derived from the original
 * one. Only the original readat() callback is replaced by a DMA-specific version. The
 * original operations are kept available for fallback use. The function is meant to run
 * only once, ensuring the proper installation of the custom operations.
 *
 * @param mmap_xlate_rdev Pointer to the xlate_region_device structure, which contains
 *                        information about the memory-mapped regions and operations.
 */
void spi_flash_dma_install_ops(struct xlate_region_device *mmap_xlate_rdev)
{
	static struct region_device_ops fast_spi_dma_rdev_ro_ops;

	/* Check if the operations have already been installed. */
	if (mmap_xlate_rdev->rdev.ops == &fast_spi_dma_rdev_ro_ops)
		return;

	/* Calculate the top boundary of the BIOS region within the SPI flash. */
	for (size_t i = 0; i < mmap_xlate_rdev->window_count; i++) {
		uint32_t begin = mmap_xlate_rdev->window_arr[i].sub_region.offset;
		uint32_t end = begin + mmap_xlate_rdev->window_arr[i].sub_region.size;
		if (bios_top < end)
			bios_top = end;
	}

	/* Store the original region device operations for fallback purposes. */
	mmap_ops = mmap_xlate_rdev->rdev.ops;

	/* Copy the original operations and replace the readat() callback with DMA-based
	   readat(). */
	memcpy(&fast_spi_dma_rdev_ro_ops, mmap_xlate_rdev->rdev.ops,
	       sizeof(fast_spi_dma_rdev_ro_ops));
	fast_spi_dma_rdev_ro_ops.readat = fast_spi_dma_readat;
	mmap_xlate_rdev->rdev.ops = &fast_spi_dma_rdev_ro_ops;
}
