/* SPDX-License-Identifier: GPL-2.0-only */

#include <console/console.h>
#include <spi_flash.h>
#include <soc/pci_devs.h>
#include <amdblocks/lpc.h>
#include <amdblocks/spi.h>
#include <device/pci_ops.h>
#include <lib.h>
#include <timer.h>
#include <types.h>

#define GRANULARITY_TEST_4k		0x0000f000		/* bits 15-12 */
#define WORD_TO_DWORD_UPPER(x)		((x << 16) & 0xffff0000)

/* SPI MMIO registers */
#define SPI_RESTRICTED_CMD1		0x04
#define SPI_RESTRICTED_CMD2		0x08
#define SPI_CNTRL1			0x0c
#define SPI_CMD_CODE			0x45
#define SPI_CMD_TRIGGER			0x47
#define   SPI_CMD_TRIGGER_EXECUTE	BIT(7)
#define SPI_TX_BYTE_COUNT		0x48
#define SPI_RX_BYTE_COUNT		0x4b
#define SPI_STATUS			0x4c
#define   SPI_DONE_BYTE_COUNT_SHIFT	0
#define   SPI_DONE_BYTE_COUNT_MASK	0xff
#define   SPI_FIFO_WR_PTR_SHIFT		8
#define   SPI_FIFO_WR_PTR_MASK		0x7f
#define   SPI_FIFO_RD_PTR_SHIFT		16
#define   SPI_FIFO_RD_PTR_MASK		0x7f

enum spi_dump_state_phase {
	SPI_DUMP_STATE_BEFORE_CMD,
	SPI_DUMP_STATE_AFTER_CMD,
};

static void dump_state(enum spi_dump_state_phase phase)
{
	u8 dump_size;
	uintptr_t addr;
	u32 status;

	if (!CONFIG(SOC_AMD_COMMON_BLOCK_SPI_DEBUG))
		return;

	switch (phase) {
	case SPI_DUMP_STATE_BEFORE_CMD:
		printk(BIOS_DEBUG, "SPI: Before execute\n");
		break;
	case SPI_DUMP_STATE_AFTER_CMD:
		printk(BIOS_DEBUG, "SPI: Transaction finished\n");
		break;
	default: /* We shouldn't reach this */
		return;
	}

	printk(BIOS_DEBUG, "Cntrl0: %x\n", spi_read32(SPI_CNTRL0));

	status = spi_read32(SPI_STATUS);
	printk(BIOS_DEBUG, "Status: %x\n", status);
	printk(BIOS_DEBUG,
	       "  Busy: %u, FIFO Read Pointer: %u, FIFO Write Pointer: %u, Done Bytes: %u\n",
	       !!(status & SPI_BUSY),
	       (status >> SPI_FIFO_RD_PTR_SHIFT) & SPI_FIFO_RD_PTR_MASK,
	       (status >> SPI_FIFO_WR_PTR_SHIFT) & SPI_FIFO_WR_PTR_MASK,
	       (status >> SPI_DONE_BYTE_COUNT_SHIFT) & SPI_DONE_BYTE_COUNT_MASK);

	printk(BIOS_DEBUG, "CmdCode: %x\n", spi_read8(SPI_CMD_CODE));

	addr = spi_get_bar() + SPI_FIFO;

	switch (phase) {
	case SPI_DUMP_STATE_BEFORE_CMD:
		dump_size = spi_read8(SPI_TX_BYTE_COUNT);
		printk(BIOS_DEBUG, "TxByteCount: %x\n", dump_size);
		break;
	case SPI_DUMP_STATE_AFTER_CMD:
		dump_size = spi_read8(SPI_RX_BYTE_COUNT);
		printk(BIOS_DEBUG, "RxByteCount: %x\n", dump_size);
		addr += spi_read8(SPI_TX_BYTE_COUNT);
		break;
	}

	if (dump_size > 0)
		hexdump((void *)addr, dump_size);
}

static int wait_for_ready(void)
{
	const uint32_t timeout_ms = 500;
	struct stopwatch sw;

	stopwatch_init_msecs_expire(&sw, timeout_ms);

	do {
		if (!(spi_read32(SPI_STATUS) & SPI_BUSY))
			return 0;
	} while (!stopwatch_expired(&sw));

	return -1;
}

static int execute_command(void)
{
	dump_state(SPI_DUMP_STATE_BEFORE_CMD);

	spi_write8(SPI_CMD_TRIGGER, SPI_CMD_TRIGGER_EXECUTE);

	if (wait_for_ready()) {
		printk(BIOS_ERR, "FCH SPI Error: Timeout executing command\n");
		return -1;
	}

	dump_state(SPI_DUMP_STATE_AFTER_CMD);

	return 0;
}

void spi_init(void)
{
	printk(BIOS_DEBUG, "%s: SPI BAR at 0x%08lx\n", __func__, spi_get_bar());
}

static int spi_ctrlr_xfer(const struct spi_slave *slave, const void *dout,
			size_t bytesout, void *din, size_t bytesin)
{
	size_t count;
	uint8_t cmd;
	uint8_t *bufin = din;
	const uint8_t *bufout = dout;

	if (CONFIG(SOC_AMD_COMMON_BLOCK_SPI_DEBUG))
		printk(BIOS_DEBUG, "%s(%zx, %zx)\n", __func__, bytesout, bytesin);

	/* First byte is cmd which cannot be sent through FIFO */
	cmd = bufout[0];
	bufout++;
	bytesout--;

	/*
	 * Check if this is a write command attempting to transfer more bytes
	 * than the controller can handle.  Iterations for writes are not
	 * supported here because each SPI write command needs to be preceded
	 * and followed by other SPI commands.
	 */
	if (bytesout + bytesin > SPI_FIFO_DEPTH) {
		printk(BIOS_WARNING, "FCH SPI: Too much to transfer, code error!\n");
		return -1;
	}

	if (wait_for_ready()) {
		printk(BIOS_ERR, "FCH SPI: Failed to acquire the SPI bus\n");
		return -1;
	}

	spi_write8(SPI_CMD_CODE, cmd);
	spi_write8(SPI_TX_BYTE_COUNT, bytesout);
	spi_write8(SPI_RX_BYTE_COUNT, bytesin);

	for (count = 0; count < bytesout; count++)
		spi_write8(SPI_FIFO + count, bufout[count]);

	if (execute_command())
		return -1;

	for (count = 0; count < bytesin; count++)
		bufin[count] = spi_read8(SPI_FIFO + count + bytesout);

	return 0;
}

static int xfer_vectors(const struct spi_slave *slave,
			struct spi_op vectors[], size_t count)
{
	int rc;

	thread_mutex_lock(&spi_hw_mutex);
	rc = spi_flash_vector_helper(slave, vectors, count, spi_ctrlr_xfer);
	thread_mutex_unlock(&spi_hw_mutex);

	return rc;
}

static int protect_a_range(u32 value)
{
	u32 reg32;
	u8 n;

	/* find a free protection register */
	for (n = 0; n < MAX_ROM_PROTECT_RANGES; n++) {
		reg32 = pci_read_config32(SOC_LPC_DEV, ROM_PROTECT_RANGE_REG(n));
		if (!reg32)
			break;
	}
	if (n == MAX_ROM_PROTECT_RANGES)
		return -1; /* no free range */

	pci_write_config32(SOC_LPC_DEV, ROM_PROTECT_RANGE_REG(n), value);
	return 0;
}

/*
 * Protect range of SPI flash defined by region using the SPI flash controller.
 *
 * Note: Up to 4 ranges can be protected, though if a particular region requires more than one
 * range, total number of regions decreases accordingly. Each range can be programmed to 4KiB or
 * 64KiB granularity.
 *
 * Warning: If more than 1 region needs protection, and they need mixed protections (read/write)
 * than start with the region that requires the most protection. After the restricted commands
 * have been written, they can't be changed (write once). So if first region is write protection
 * and second region is read protection, it's best to define first region as read and write
 * protection.
 */
static int fch_spi_flash_protect(const struct spi_flash *flash, const struct region *region,
				 const enum ctrlr_prot_type type)
{
	int ret;
	u32 reg32, rom_base, range_base;
	size_t addr, len, gran_value, total_ranges, range;
	bool granularity_64k = true; /* assume 64k granularity */

	addr = region->offset;
	len = region->size;

	reg32 = pci_read_config32(SOC_LPC_DEV, ROM_ADDRESS_RANGE2_START);
	rom_base = WORD_TO_DWORD_UPPER(reg32);
	if (addr < rom_base)
		return -1;
	range_base = addr % rom_base;

	/* Define granularity to be used */
	if (GRANULARITY_TEST_4k & range_base)
		granularity_64k = false; /* use 4K granularity */
	if (GRANULARITY_TEST_4k & len)
		granularity_64k = false; /* use 4K granularity */

	/* Define the first range and total number of ranges required */
	if (granularity_64k) {
		gran_value = 0x00010000; /* 64 KiB */
		range_base = range_base >> 16;
	} else {
		gran_value = 0x00001000; /* 4 KiB */
		range_base = range_base >> 12;
	}
	total_ranges = len / gran_value;
	range_base &= RANGE_ADDR_MASK;

	/* Create reg32 to be written into a range register and program required ranges */
	reg32 = rom_base & ROM_BASE_MASK;
	reg32 |= range_base;
	if (granularity_64k)
		reg32 |= RANGE_UNIT;
	if (type & WRITE_PROTECT)
		reg32 |= ROM_RANGE_WP;
	if (type & READ_PROTECT)
		reg32 |= ROM_RANGE_RP;

	for (range = 0; range < total_ranges; range++) {
		ret = protect_a_range(reg32);
		if (ret)
			return ret;
		/*
		 * Next range (lower 8 bits). Range points to the start address of a region.
		 * The range value must be multiplied by the granularity (which is also the
		 * size of the region) to get the actual offset from the SPI start address.
		 */
		reg32++;
	}

	/* define commands to be blocked if in range */
	reg32 = 0;
	if (type & WRITE_PROTECT) {
		/* FIXME */
		printk(BIOS_INFO, "%s: Write Enable and Write Cmd not blocked\n", __func__);
		reg32 |= (flash->erase_cmd << 8);
	}
	if (type & READ_PROTECT) {
		/* FIXME */
		printk(BIOS_INFO, "%s: READ_PROTECT not supported.\n", __func__);
	}

	/* Final steps to protect region */
	pci_write_config32(SOC_LPC_DEV, SPI_RESTRICTED_CMD1, reg32);
	reg32 = spi_read32(SPI_CNTRL0);
	reg32 &= ~SPI_ACCESS_MAC_ROM_EN;
	spi_write32(SPI_CNTRL0, reg32);

	return 0;
}

static const struct spi_ctrlr fch_spi_flash_ctrlr = {
	.xfer_vector = xfer_vectors,
	.max_xfer_size = SPI_FIFO_DEPTH,
	.flags = SPI_CNTRLR_DEDUCT_CMD_LEN | SPI_CNTRLR_DEDUCT_OPCODE_LEN,
	.flash_protect = fch_spi_flash_protect,
};

const struct spi_ctrlr_buses spi_ctrlr_bus_map[] = {
	{
		.ctrlr = &fch_spi_flash_ctrlr,
		.bus_start = 0,
		.bus_end = 0,
	},
};

const size_t spi_ctrlr_bus_map_count = ARRAY_SIZE(spi_ctrlr_bus_map);
