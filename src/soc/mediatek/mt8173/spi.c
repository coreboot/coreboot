/*
 * This file is part of the coreboot project.
 *
 * Copyright 2015 MediaTek Inc.
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

#include <arch/io.h>
#include <assert.h>
#include <console/console.h>
#include <delay.h>
#include <endian.h>
#include <spi_flash.h>
#include <stdlib.h>
#include <string.h>
#include <timer.h>
#include <soc/addressmap.h>
#include <soc/flash_controller.h>
#include <soc/gpio.h>
#include <soc/pinmux.h>
#include <soc/pll.h>
#include <soc/spi.h>

enum {
	MTK_FIFO_DEPTH = 32,
	MTK_TXRX_TIMEOUT_US = 1000 * 1000,
	MTK_ARBITRARY_VALUE = 0xdeaddead
};

enum {
	MTK_SPI_IDLE = 0,
	MTK_SPI_PAUSE_IDLE = 1
};

enum {
	MTK_SPI_BUSY_STATUS = 1,
	MTK_SPI_PAUSE_FINISH_INT_STATUS = 3
};

static struct mtk_spi_bus spi_bus[1] = {
	{
		.regs = (void *)SPI_BASE,
		.state = MTK_SPI_IDLE,
	}
};

static inline struct mtk_spi_bus *to_mtk_spi(const struct spi_slave *slave)
{
	assert(slave->bus < ARRAY_SIZE(spi_bus));
	return &spi_bus[slave->bus];
}

static void spi_sw_reset(struct mtk_spi_regs *regs)
{
	setbits_le32(&regs->spi_cmd_reg, SPI_CMD_RST_EN);
	clrbits_le32(&regs->spi_cmd_reg, SPI_CMD_RST_EN);
}

static void mtk_spi_set_gpio_pinmux(enum spi_pad_mask pad_select)
{
	/* TODO: implement support for other pads when needed */
	assert(pad_select == SPI_PAD1_MASK);
	gpio_set_mode(PAD_MSDC2_DAT2, PAD_MSDC2_DAT2_FUNC_SPI_CK_1);
	gpio_set_mode(PAD_MSDC2_DAT3, PAD_MSDC2_DAT3_FUNC_SPI_MI_1);
	gpio_set_mode(PAD_MSDC2_CLK, PAD_MSDC2_CLK_FUNC_SPI_MO_1);
	gpio_set_mode(PAD_MSDC2_CMD, PAD_MSDC2_CMD_FUNC_SPI_CS_1);
}

void mtk_spi_init(unsigned int bus, unsigned int pad_select,
		  unsigned int speed_hz)
{
	u32 div, sck_ticks, cs_ticks, reg_val;

	/* mtk spi HW just supports bus 0 */
	if (bus != 0)
		die("Error: Only SPI bus 0 is supported.\n");

	struct mtk_spi_bus *slave = &spi_bus[bus];
	struct mtk_spi_regs *regs = slave->regs;

	if (speed_hz < SPI_HZ / 2)
		div = div_round_up(SPI_HZ, speed_hz);
	else
		div = 1;

	sck_ticks = div_round_up(div, 2);
	cs_ticks = sck_ticks * 2;

	printk(BIOS_DEBUG, "SPI%u initialized at %u Hz",
	       pad_select, SPI_HZ / (sck_ticks * 2));

	/* set the timing */
	write32(&regs->spi_cfg0_reg,
		((sck_ticks - 1) << SPI_CFG0_SCK_HIGH_SHIFT) |
		((sck_ticks - 1) << SPI_CFG0_SCK_LOW_SHIFT) |
		((cs_ticks - 1) << SPI_CFG0_CS_HOLD_SHIFT) |
		((cs_ticks - 1) << SPI_CFG0_CS_SETUP_SHIFT));
	clrsetbits_le32(&regs->spi_cfg1_reg, SPI_CFG1_CS_IDLE_MASK,
			((cs_ticks - 1) << SPI_CFG1_CS_IDLE_SHIFT));

	reg_val = read32(&regs->spi_cmd_reg);

	reg_val &= ~SPI_CMD_CPHA_EN;
	reg_val &= ~SPI_CMD_CPOL_EN;

	/* set the mlsbx and mlsbtx */
	reg_val |= SPI_CMD_TXMSBF_EN;
	reg_val |= SPI_CMD_RXMSBF_EN;

	/* set the tx/rx endian */
#ifdef __LITTLE_ENDIAN
	reg_val &= ~SPI_CMD_TX_ENDIAN_EN;
	reg_val &= ~SPI_CMD_RX_ENDIAN_EN;
#else
	reg_val |= SPI_CMD_TX_ENDIAN_EN;
	reg_val |= SPI_CMD_RX_ENDIAN_EN;
#endif

	/* clear pause mode */
	reg_val &= ~SPI_CMD_PAUSE_EN;

	/* set finish interrupt always enable */
	reg_val |= SPI_CMD_FINISH_IE_EN;

	/* set pause interrupt always enable */
	reg_val |= SPI_CMD_PAUSE_IE_EN;

	/* disable dma mode */
	reg_val &= ~(SPI_CMD_TX_DMA_EN | SPI_CMD_RX_DMA_EN);

	/* set deassert mode */
	reg_val &= ~SPI_CMD_DEASSERT_EN;

	write32(&regs->spi_cmd_reg, reg_val);

	mtk_spi_set_gpio_pinmux(pad_select);
	/* pad select */
	clrsetbits_le32(&regs->spi_pad_macro_sel_reg, SPI_PAD_SEL_MASK,
			pad_select);
}

static void mtk_spi_dump_data(const char *name, const uint8_t *data,
			      int size)
{
#ifdef MTK_SPI_DEBUG
	int i;

	printk(BIOS_DEBUG, "%s: 0x ", name);
	for (i = 0; i < size; i++)
		printk(BIOS_INFO, "%#x ", data[i]);
	printk(BIOS_DEBUG, "\n");
#endif
}

static int spi_ctrlr_claim_bus(const struct spi_slave *slave)
{
	struct mtk_spi_bus *mtk_slave = to_mtk_spi(slave);
	struct mtk_spi_regs *regs = mtk_slave->regs;

	setbits_le32(&regs->spi_cmd_reg, 1 << SPI_CMD_PAUSE_EN_SHIFT);
	mtk_slave->state = MTK_SPI_IDLE;

	return 0;
}

static int mtk_spi_fifo_transfer(const struct spi_slave *slave, void *in,
				 const void *out, size_t size)
{
	struct mtk_spi_bus *mtk_slave = to_mtk_spi(slave);
	struct mtk_spi_regs *regs = mtk_slave->regs;
	uint8_t *inb = (uint8_t *)in;
	const uint32_t *outb = (const uint32_t *)out;
	uint32_t reg_val = 0;
	uint32_t i, word_count;
	struct stopwatch sw;

	if (!size || size > MTK_FIFO_DEPTH)
		return -1;

	clrsetbits_le32(&regs->spi_cfg1_reg,
			SPI_CFG1_PACKET_LENGTH_MASK | SPI_CFG1_PACKET_LOOP_MASK,
			((size - 1) << SPI_CFG1_PACKET_LENGTH_SHIFT) |
			(0 << SPI_CFG1_PACKET_LOOP_SHIFT));

	word_count = div_round_up(size, sizeof(u32));
	if (inb) {
		/* The SPI controller will transmit in full-duplex for RX,
		 * therefore we need arbitrary data on MOSI which the slave
		 * must ignore.
		 */
		for (i = 0; i < word_count; i++)
			write32(&regs->spi_tx_data_reg, MTK_ARBITRARY_VALUE);
	}
	if (outb) {
		for (i = 0; i < word_count; i++)
			write32(&regs->spi_tx_data_reg, outb[i]);
		mtk_spi_dump_data("the outb data is",
				  (const uint8_t *)outb, size);
	}

	if (mtk_slave->state == MTK_SPI_IDLE) {
		setbits_le32(&regs->spi_cmd_reg, SPI_CMD_ACT_EN);
		mtk_slave->state = MTK_SPI_PAUSE_IDLE;
	} else if (mtk_slave->state == MTK_SPI_PAUSE_IDLE) {
		setbits_le32(&regs->spi_cmd_reg, SPI_CMD_RESUME_EN);
	}

	stopwatch_init_usecs_expire(&sw, MTK_TXRX_TIMEOUT_US);
	while ((read32(&regs->spi_status1_reg) & MTK_SPI_BUSY_STATUS) == 0) {
		if (stopwatch_expired(&sw)) {
			printk(BIOS_ERR,
			       "Timeout waiting for status1 status.\n");
			goto error;
		}
	}
	stopwatch_init_usecs_expire(&sw, MTK_TXRX_TIMEOUT_US);
	while ((read32(&regs->spi_status0_reg) &
	       MTK_SPI_PAUSE_FINISH_INT_STATUS) == 0) {
		if (stopwatch_expired(&sw)) {
			printk(BIOS_ERR,
			       "Timeout waiting for status0 status.\n");
			goto error;
		}
	}

	if (inb) {
		for (i = 0; i < size; i++) {
			if (i % 4 == 0)
				reg_val = read32(&regs->spi_rx_data_reg);
			*(inb + i) = (reg_val >> ((i % 4) * 8)) & 0xff;
		}
		mtk_spi_dump_data("the inb data is", inb, size);
	}

	return 0;
error:
	spi_sw_reset(regs);
	mtk_slave->state = MTK_SPI_IDLE;
	return -1;
}

static int spi_ctrlr_xfer(const struct spi_slave *slave, const void *dout,
			size_t bytes_out, void *din, size_t bytes_in)
{
	size_t min_size = 0;
	int ret;

	/* Driver implementation does not support full duplex. */
	if (bytes_in && bytes_out)
		return -1;

	while (bytes_out || bytes_in) {
		if (bytes_in && bytes_out)
			min_size = MIN(MIN(bytes_out, bytes_in), MTK_FIFO_DEPTH);
		else if (bytes_out)
			min_size = MIN(bytes_out, MTK_FIFO_DEPTH);
		else if (bytes_in)
			min_size = MIN(bytes_in, MTK_FIFO_DEPTH);

		ret = mtk_spi_fifo_transfer(slave, din, dout, min_size);
		if (ret != 0)
			return ret;

		if (bytes_out) {
			bytes_out -= min_size;
			dout = (const uint8_t *)dout + min_size;
		}

		if (bytes_in) {
			bytes_in -= min_size;
			din = (uint8_t *)din + min_size;
		}
	}

	return 0;
}

static void spi_ctrlr_release_bus(const struct spi_slave *slave)
{
	struct mtk_spi_bus *mtk_slave = to_mtk_spi(slave);
	struct mtk_spi_regs *regs = mtk_slave->regs;

	clrbits_le32(&regs->spi_cmd_reg, SPI_CMD_PAUSE_EN);
	spi_sw_reset(regs);
	mtk_slave->state = MTK_SPI_IDLE;
}

static int spi_ctrlr_setup(const struct spi_slave *slave)
{
	struct mtk_spi_bus *eslave = to_mtk_spi(slave);
	assert(read32(&eslave->regs->spi_cfg0_reg) != 0);
	spi_sw_reset(eslave->regs);
	return 0;
}

static const struct spi_ctrlr spi_flash_ctrlr = {
	.max_xfer_size = 65535,
	.flash_probe = mtk_spi_flash_probe,
};

static const struct spi_ctrlr spi_ctrlr = {
	.setup = spi_ctrlr_setup,
	.claim_bus = spi_ctrlr_claim_bus,
	.release_bus = spi_ctrlr_release_bus,
	.xfer = spi_ctrlr_xfer,
	.max_xfer_size = 65535,
};

const struct spi_ctrlr_buses spi_ctrlr_bus_map[] = {
	{
		.ctrlr = &spi_ctrlr,
		.bus_start = 0,
		.bus_end = 0,
	},
	{
		.ctrlr = &spi_flash_ctrlr,
		.bus_start = CONFIG_BOOT_DEVICE_SPI_FLASH_BUS,
		.bus_end = CONFIG_BOOT_DEVICE_SPI_FLASH_BUS,
	},
};

const size_t spi_ctrlr_bus_map_count = ARRAY_SIZE(spi_ctrlr_bus_map);
