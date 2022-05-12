/* SPDX-License-Identifier: GPL-2.0-only */

#include <spi-generic.h>
#include <spi_flash.h>
#include <arch/cache.h>
#include <device/mmio.h>
#include <soc/addressmap.h>
#include <soc/qspi_common.h>
#include <soc/gpio.h>
#include <soc/clock.h>
#include <symbols.h>
#include <assert.h>
#include <gpio.h>
#include <string.h>

#define CACHE_LINE_SIZE	64

static int curr_desc_idx = -1;

struct cmd_desc {
	uint32_t data_address;
	uint32_t next_descriptor;
	uint32_t direction:1;
	uint32_t multi_io_mode:3;
	uint32_t reserved1:4;
	uint32_t fragment:1;
	uint32_t reserved2:7;
	uint32_t length:16;
	//------------------------//
	uint32_t bounce_src;
	uint32_t bounce_dst;
	uint32_t bounce_length;
	uint64_t padding[5];
};

enum qspi_mode {
	SDR_1BIT = 1,
	SDR_2BIT = 2,
	SDR_4BIT = 3,
	DDR_1BIT = 5,
	DDR_2BIT = 6,
	DDR_4BIT = 7,
};

enum cs_state {
	CS_DEASSERT,
	CS_ASSERT
};

struct xfer_cfg {
	enum qspi_mode mode;
};

enum bus_xfer_direction {
	MASTER_READ = 0,
	MASTER_WRITE = 1,
};

struct {
	struct cmd_desc descriptors[3];
	uint8_t buffers[3][CACHE_LINE_SIZE];
} *dma = (void *)_dma_coherent;

static void dma_transfer_chain(struct cmd_desc *chain)
{
	uint32_t mstr_int_status;

	write32(&qcom_qspi->mstr_int_sts, 0xFFFFFFFF);
	write32(&qcom_qspi->next_dma_desc_addr, (uint32_t)(uintptr_t) chain);

	while (1) {
		mstr_int_status = read32(&qcom_qspi->mstr_int_sts);
		if (mstr_int_status & DMA_CHAIN_DONE)
			break;
	}
}

static void flush_chain(void)
{
	struct cmd_desc *desc = &dma->descriptors[0];
	uint8_t *src;
	uint8_t *dst;

	dma_transfer_chain(desc);

	while (desc) {
		if (desc->direction == MASTER_READ) {
			if (desc->bounce_length == 0)
				dcache_invalidate_by_mva(
					(void *)(uintptr_t) desc->data_address,
					desc->length);
			else {
				src = (void *)(uintptr_t) desc->bounce_src;
				dst = (void *)(uintptr_t) desc->bounce_dst;
				memcpy(dst, src, desc->bounce_length);
			}
		}
		desc = (void *)(uintptr_t) desc->next_descriptor;
	}
	curr_desc_idx = -1;
}

static struct cmd_desc *allocate_descriptor(void)
{
	struct cmd_desc *current;
	struct cmd_desc *next;
	uint8_t index;

	current = (curr_desc_idx == -1) ?
		NULL : &dma->descriptors[curr_desc_idx];

	index = ++curr_desc_idx;
	next = &dma->descriptors[index];

	next->data_address = (uint32_t) (uintptr_t) dma->buffers[index];

	next->next_descriptor = 0;
	next->direction = MASTER_READ;
	next->multi_io_mode = 0;
	next->reserved1 = 0;
	/*
	 * QSPI controller doesn't support transfer starts with read segment.
	 * So to support read transfers that are not preceded by write, set
	 * transfer fragment bit = 1
	 */
	next->fragment = 1;
	next->reserved2 = 0;
	next->length = 0;
	next->bounce_src = 0;
	next->bounce_dst = 0;
	next->bounce_length = 0;

	if (current)
		current->next_descriptor = (uint32_t)(uintptr_t) next;

	return next;
}

static void cs_change(enum cs_state state)
{
	gpio_set(QSPI_CS, state == CS_DEASSERT);
}

static void configure_gpios(void)
{
	gpio_output(QSPI_CS, 1);

	gpio_configure(QSPI_DATA_0, GPIO_FUNC_QSPI_DATA_0,
		GPIO_NO_PULL, GPIO_8MA, GPIO_OUTPUT);

	gpio_configure(QSPI_DATA_1, GPIO_FUNC_QSPI_DATA_1,
		GPIO_NO_PULL, GPIO_8MA, GPIO_OUTPUT);

	gpio_configure(QSPI_CLK, GPIO_FUNC_QSPI_CLK,
		GPIO_NO_PULL, GPIO_8MA, GPIO_OUTPUT);
}

static void queue_bounce_data(uint8_t *data, uint32_t data_bytes,
			      enum qspi_mode data_mode, bool write)
{
	struct cmd_desc *desc;
	uint8_t *ptr;

	desc = allocate_descriptor();
	desc->direction = write;
	desc->multi_io_mode = data_mode;
	ptr = (void *)(uintptr_t) desc->data_address;

	if (write) {
		memcpy(ptr, data, data_bytes);
	} else {
		desc->bounce_src = (uint32_t)(uintptr_t) ptr;
		desc->bounce_dst = (uint32_t)(uintptr_t) data;
		desc->bounce_length = data_bytes;
	}

	desc->length = data_bytes;
}

static void queue_direct_data(uint8_t *data, uint32_t data_bytes,
			      enum qspi_mode data_mode, bool write)
{
	struct cmd_desc *desc;

	desc = allocate_descriptor();
	desc->direction = write;
	desc->multi_io_mode = data_mode;
	desc->data_address = (uint32_t)(uintptr_t) data;
	desc->length = data_bytes;

	if (write)
		dcache_clean_by_mva(data, data_bytes);
	else
		dcache_invalidate_by_mva(data, data_bytes);
}

static void queue_data(uint8_t *data, uint32_t data_bytes,
	enum qspi_mode data_mode, bool write)
{
	uint8_t *aligned_ptr;
	uint8_t *epilog_ptr;
	uint32_t prolog_bytes, aligned_bytes, epilog_bytes;

	if (data_bytes == 0)
		return;

	aligned_ptr =
		(uint8_t *)ALIGN_UP((uintptr_t)data, CACHE_LINE_SIZE);

	prolog_bytes = MIN(data_bytes, aligned_ptr - data);
	aligned_bytes = ALIGN_DOWN(data_bytes - prolog_bytes, CACHE_LINE_SIZE);
	epilog_bytes = data_bytes - prolog_bytes - aligned_bytes;

	epilog_ptr = data + prolog_bytes + aligned_bytes;

	if (prolog_bytes)
		queue_bounce_data(data, prolog_bytes, data_mode, write);
	if (aligned_bytes)
		queue_direct_data(aligned_ptr, aligned_bytes, data_mode, write);
	if (epilog_bytes)
		queue_bounce_data(epilog_ptr, epilog_bytes, data_mode, write);
}

/*
 * The way to encode the sampling delay is:
 *
 * QSPI_SAMPLE_CLK_CONFIG	delay (cycle)
 * ----------------------------------------
 * 0xFFFh = 1111 1111 1111b	7/8
 * 0xDB6h = 1101 1011 0110b	6/8
 * 0xB6Dh = 1011 0110 1101b	5/8
 * 0x924h = 1001 0010 0100b	4/8
 * 0x6DBh = 0110 1101 1011b	3/8
 * 0x492h = 0100 1001 0010b	2/8
 * 0x249h = 0010 0100 1001b	1/8
 * 0x000h = 0000 0000 0000b	None
 */
static void reg_init(uint32_t sdelay)
{
	uint32_t spi_mode;
	uint32_t tx_data_oe_delay, tx_data_delay;
	uint32_t mstr_config;
	uint32_t sampling_delay;

	spi_mode = 0;

	tx_data_oe_delay = 0;
	tx_data_delay = 0;

	mstr_config = (tx_data_oe_delay << TX_DATA_OE_DELAY_SHIFT) |
		(tx_data_delay << TX_DATA_DELAY_SHIFT) | (SBL_EN) |
		(spi_mode << SPI_MODE_SHIFT) |
		(PIN_HOLDN) |
		(DMA_ENABLE) |
		(FULL_CYCLE_MODE);

	write32(&qcom_qspi->mstr_cfg, mstr_config);
	write32(&qcom_qspi->ahb_mstr_cfg, 0xA42);
	write32(&qcom_qspi->mstr_int_en, 0x0);
	write32(&qcom_qspi->mstr_int_sts, 0xFFFFFFFF);
	write32(&qcom_qspi->rd_fifo_cfg, 0x0);
	write32(&qcom_qspi->rd_fifo_rst, RESET_FIFO);
	sampling_delay = sdelay << 9 | sdelay << 6 | sdelay << 3 | sdelay << 0;
	write32(&qcom_qspi->sampling_clk_cfg, sampling_delay);
}

void quadspi_init(uint32_t hz, uint32_t sdelay)
{
	assert(dcache_line_bytes() == CACHE_LINE_SIZE);
	clock_configure_qspi(hz * 4);
	configure_gpios();
	reg_init(sdelay);
}

int qspi_claim_bus(const struct spi_slave *slave)
{
	cs_change(CS_ASSERT);
	return 0;
}

void qspi_release_bus(const struct spi_slave *slave)
{
	cs_change(CS_DEASSERT);
}

static int xfer(enum qspi_mode mode, const void *dout, size_t out_bytes,
		void *din, size_t in_bytes)
{
	if ((out_bytes && !dout) || (in_bytes && !din) ||
		(in_bytes && out_bytes)) {
		return -1;
	}

	queue_data((uint8_t *) (out_bytes ? dout : din),
		in_bytes | out_bytes, mode, !!out_bytes);

	flush_chain();

	return 0;
}

int qspi_xfer(const struct spi_slave *slave, const void *dout,
		size_t out_bytes, void *din, size_t in_bytes)
{
	return xfer(SDR_1BIT, dout, out_bytes, din, in_bytes);
}

int qspi_xfer_dual(const struct spi_slave *slave, const void *dout,
		     size_t out_bytes, void *din, size_t in_bytes)
{
	return xfer(SDR_2BIT, dout, out_bytes, din, in_bytes);
}
