/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2019 Silverback Ltd.
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

#include <console/console.h>
#include <spi_flash.h>
#include <soc/southbridge.h>
#include <soc/pci_devs.h>
#include <amdblocks/fch_spi.h>
#include <amdblocks/lpc.h>
#include <drivers/spi/spi_flash_internal.h>
#include <device/pci_ops.h>
#include <timer.h>
#include <lib.h>

static struct spi_data ctrl_spi_data;
static uint32_t spibar;

static inline uint8_t spi_read8(uint8_t reg)
{
	return read8((void *)(spibar + reg));
}

static inline uint32_t spi_read32(uint8_t reg)
{
	return read32((void *)(spibar + reg));
}

static inline void spi_write8(uint8_t reg, uint8_t val)
{
	write8((void *)(spibar + reg), val);
}

static inline void spi_write32(uint8_t reg, uint32_t val)
{
	write32((void *)(spibar + reg), val);
}

static void dump_state(const char *str, u8 phase)
{
	u8 dump_size;
	u32 addr;

	if (!CONFIG(SOC_AMD_COMMON_BLOCK_SPI_DEBUG))
		return;

	printk(BIOS_DEBUG, "SPI: %s\n", str);
	printk(BIOS_DEBUG, "Cntrl0: %x\n", spi_read32(SPI_CNTRL0));
	printk(BIOS_DEBUG, "Status: %x\n", spi_read32(SPI_STATUS));

	addr = spibar + SPI_FIFO;
	if (phase == 0) {
		dump_size = spi_read8(SPI_TX_BYTE_COUNT);
		printk(BIOS_DEBUG, "TxByteCount: %x\n", dump_size);
		printk(BIOS_DEBUG, "CmdCode: %x\n", spi_read8(SPI_CMD_CODE));
	} else {
		dump_size = spi_read8(SPI_RX_BYTE_COUNT);
		printk(BIOS_DEBUG, "RxByteCount: %x\n", dump_size);
		addr += spi_read8(SPI_TX_BYTE_COUNT);
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
	dump_state("Before execute", 0);

	spi_write8(SPI_CMD_TRIGGER, SPI_CMD_TRIGGER_EXECUTE);

	if (wait_for_ready())
		printk(BIOS_ERR,
			"FCH_SC Error: Timeout executing command\n");

	dump_state("Transaction finished", 1);

	return 0;
}

void spi_init(void)
{
	spibar = lpc_get_spibase();
	printk(BIOS_DEBUG, "%s: Spibar at 0x%08x\n", __func__, spibar);
}

const struct spi_data *get_ctrl_spi_data(void)
{
	return &ctrl_spi_data;
}

static int spi_ctrlr_xfer(const void *dout, size_t bytesout, void *din, size_t bytesin)
{
	size_t count;
	uint8_t cmd;
	uint8_t *bufin = din;
	const uint8_t *bufout = dout;

	if (CONFIG(SOC_AMD_COMMON_BLOCK_SPI_DEBUG))
		printk(BIOS_DEBUG, "%s(%zx, %zx)\n", __func__, bytesout,
			bytesin);

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
		printk(BIOS_WARNING, "FCH_SC: Too much to transfer, code error!\n");
		return -1;
	}

	if (wait_for_ready())
		return -1;

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

static int amd_xfer_vectors(struct spi_op vectors[], size_t count)
{
	int ret;
	void *din;
	size_t bytes_in;

	if (count < 1 || count > 2)
		return -1;

	/* SPI flash commands always have a command first... */
	if (!vectors[0].dout || !vectors[0].bytesout)
		return -1;
	/* And not read any data during the command. */
	if (vectors[0].din || vectors[0].bytesin)
		return -1;

	if (count == 2) {
		/* If response bytes requested ensure the buffer is valid. */
		if (vectors[1].bytesin && !vectors[1].din)
			return -1;
		/* No sends can accompany a receive. */
		if (vectors[1].dout || vectors[1].bytesout)
			return -1;
		din = vectors[1].din;
		bytes_in = vectors[1].bytesin;
	} else {
		din = NULL;
		bytes_in = 0;
	}

	ret = spi_ctrlr_xfer(vectors[0].dout, vectors[0].bytesout, din, bytes_in);

	if (ret) {
		vectors[0].status = SPI_OP_FAILURE;
		if (count == 2)
			vectors[1].status = SPI_OP_FAILURE;
	} else {
		vectors[0].status = SPI_OP_SUCCESS;
		if (count == 2)
			vectors[1].status = SPI_OP_SUCCESS;
	}

	return ret;
}

int fch_spi_flash_cmd(const void *dout, size_t bytes_out, void *din, size_t bytes_in)
{
	/*
	 * SPI flash requires command-response kind of behavior. Thus, two
	 * separate SPI vectors are required -- first to transmit dout and other
	 * to receive in din.
	 */
	struct spi_op vectors[] = {
		[0] = { .dout = dout, .bytesout = bytes_out,
			.din = NULL, .bytesin = 0, },
		[1] = { .dout = NULL, .bytesout = 0,
			.din = din, .bytesin = bytes_in },
	};
	size_t count = ARRAY_SIZE(vectors);
	if (!bytes_in)
		count = 1;

	return amd_xfer_vectors(vectors, count);
}

static void set_ctrl_spi_data(struct spi_flash *flash)
{
	u8 cmd = SPI_PAGE_WRITE;

	ctrl_spi_data.name = flash->name;
	ctrl_spi_data.size = flash->size;
	ctrl_spi_data.sector_size = flash->sector_size;
	ctrl_spi_data.status_cmd = flash->status_cmd;
	ctrl_spi_data.erase_cmd = flash->erase_cmd;
	ctrl_spi_data.write_enable_cmd = SPI_WRITE_ENABLE;

	if (flash->vendor == VENDOR_ID_SST) {
		ctrl_spi_data.non_standard = NON_STANDARD_SPI_SST;
		if ((flash->model & 0x00ff) == SST_256)
			ctrl_spi_data.page_size = 256;
		else {
			ctrl_spi_data.page_size = 2;
			cmd = CMD_SST_AAI_WP;
		}
	} else {
		ctrl_spi_data.page_size = flash->page_size;
		ctrl_spi_data.non_standard = NON_STANDARD_SPI_NONE;
	}
	ctrl_spi_data.write_cmd = cmd;

	if (CONFIG(SPI_FLASH_NO_FAST_READ)) {
		ctrl_spi_data.read_cmd_len = 4;
		ctrl_spi_data.read_cmd = CMD_READ_ARRAY_SLOW;
	} else {
		ctrl_spi_data.read_cmd_len = 5;
		ctrl_spi_data.read_cmd = CMD_READ_ARRAY_FAST;
	}
}

static int fch_spi_flash_probe(const struct spi_slave *spi, struct spi_flash *flash)
{
	int ret, i, shift, table_size;
	u8 idcode[IDCODE_LEN], *idp, cmd = CMD_READ_ID;
	const struct spi_flash_table *flash_ptr = get_spi_flash_table(&table_size);

	/* Read the ID codes */
	ret = fch_spi_flash_cmd(&cmd, 1, idcode, sizeof(idcode));
	if (ret)
		return -1;

	if (CONFIG(SOC_AMD_COMMON_BLOCK_SPI_DEBUG)) {
		printk(BIOS_SPEW, "SF: Got idcode: ");
		for (i = 0; i < sizeof(idcode); i++)
			printk(BIOS_SPEW, "%02x ", idcode[i]);
		printk(BIOS_SPEW, "\n");
	}

	/*
	 * All solid state devices have vendor id defined by JEDEC specification JEP106,
	 * which originally allocated only 7 bits for it plus parity. When number of
	 * vendors exploded beyond 126, a banking proposition came maintaining
	 * compatibility with older vendors while allowing for 4 extra bits (16 banks)
	 * through the introduction of the concept "Continuation Code", denoted by the
	 * byte value of 0x7f.
	 * Examples:
	 * 0xfe, 0x60, 0x18, 0x00, 0x00, 0x00 => vendor 0xfe of bank o
	 * 0x7f, 0x7f, 0xfe, 0x60, 0x18, 0x00 => vendor 0xfe of bank 2
	 * count the number of continuation code bytes
	 */
	for (shift = 0, idp = idcode; *idp == IDCODE_CONT_CODE; ++shift, ++idp) {
		if (shift < IDCODE_CONT_LEN)
			continue;
		printk(BIOS_ERR, "unsupported ID code bank\n");
		return -1;
	}

	printk(BIOS_INFO, "Manufacturer: %02x on bank %d\n", *idp, shift);

	/* search the table for matches in shift and id */
	for (i = 0; i < table_size; ++i) {
		if (flash_ptr->shift == shift && flash_ptr->idcode == *idp) {
			/* we have a match, call probe */
			if (flash_ptr->probe(spi, idp, flash) == 0) {
				flash->vendor = idp[0];
				flash->model = (idp[1] << 8) | idp[2];
				set_ctrl_spi_data(flash);
				fch_spi_flash_ops_init(flash);
				return 0;
			}
		}
		flash_ptr++;
	}

	/* No match, return error. */
	return -1;
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
		reg32 |= (ctrl_spi_data.write_enable_cmd << 24);
		reg32 |= (ctrl_spi_data.write_cmd << 16);
		reg32 |= (ctrl_spi_data.erase_cmd << 8);
	}
	if (type & READ_PROTECT)
		reg32 |= ctrl_spi_data.read_cmd;

	/* Final steps to protect region */
	pci_write_config32(SOC_LPC_DEV, SPI_RESTRICTED_CMD1, reg32);
	reg32 = spi_read32(SPI_CNTRL0);
	reg32 &= ~SPI_ACCESS_MAC_ROM_EN;
	spi_write32(SPI_CNTRL0, reg32);

	return 0;
}

const struct spi_ctrlr fch_spi_flash_ctrlr = {
	.max_xfer_size = SPI_FIFO_DEPTH,
	.flash_probe = fch_spi_flash_probe,
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
