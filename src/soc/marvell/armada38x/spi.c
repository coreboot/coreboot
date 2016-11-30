/*
 * This file is part of the coreboot project.
 *
 * Copyright 2015 Marvell Inc.
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
#include <delay.h>
#include <spi_flash.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <console/console.h>
#include <soc/common.h>
#include <soc/clock.h>

/******************************************************************************
base type define
*******************************************************************************/
#define MV_SPI_REG_READ mrvl_reg_read
#define MV_SPI_REG_WRITE mrvl_reg_write
#define MV_SPI_REG_BIT_SET mrvl_reg_bit_set
#define MV_SPI_REG_BIT_RESET mrvl_reg_bit_reset

#define MV_SPI_REGS_OFFSET(unit) (0x10600 + (unit * 0x80))
#define MV_SPI_REGS_BASE(unit) (MV_SPI_REGS_OFFSET(unit))
#define MV_SPI_IF_CONFIG_REG(spi_id) (MV_SPI_REGS_BASE(spi_id) + 0x04)
#define MV_SPI_SPR_OFFSET 0
#define MV_SPI_SPR_MASK (0xF << MV_SPI_SPR_OFFSET)
#define MV_SPI_SPPR_0_OFFSET 4
#define MV_SPI_SPPR_0_MASK (0x1 << MV_SPI_SPPR_0_OFFSET)
#define MV_SPI_SPPR_HI_OFFSET 6
#define MV_SPI_SPPR_HI_MASK (0x3 << MV_SPI_SPPR_HI_OFFSET)

#define MV_SPI_BYTE_LENGTH_OFFSET 5 /* bit 5 */
#define MV_SPI_BYTE_LENGTH_MASK (0x1 << MV_SPI_BYTE_LENGTH_OFFSET)

#define MV_SPI_IF_CTRL_REG(spi_id) (MV_SPI_REGS_BASE(spi_id) + 0x00)
#define MV_SPI_CS_ENABLE_OFFSET 0 /* bit 0 */
#define MV_SPI_CS_ENABLE_MASK (0x1 << MV_SPI_CS_ENABLE_OFFSET)

#define MV_SPI_CS_NUM_OFFSET 2
#define MV_SPI_CS_NUM_MASK (0x7 << MV_SPI_CS_NUM_OFFSET)
#define MV_SPI_CPOL_OFFSET 11
#define MV_SPI_CPOL_MASK (0x1 << MV_SPI_CPOL_OFFSET)
#define MV_SPI_CPHA_OFFSET 12
#define MV_SPI_CPHA_MASK (0x1 << MV_SPI_CPHA_OFFSET)
#define MV_SPI_TXLSBF_OFFSET 13
#define MV_SPI_TXLSBF_MASK (0x1 << MV_SPI_TXLSBF_OFFSET)
#define MV_SPI_RXLSBF_OFFSET 14
#define MV_SPI_RXLSBF_MASK (0x1 << MV_SPI_RXLSBF_OFFSET)

/* SPI transfer flags */
#define SPI_XFER_BEGIN 0x01
#define SPI_XFER_END 0x02

#define MV_SPI_INT_CAUSE_REG(spi_id) (MV_SPI_REGS_BASE(spi_id) + 0x10)
#define MV_SPI_DATA_OUT_REG(spi_id) (MV_SPI_REGS_BASE(spi_id) + 0x08)
#define MV_SPI_WAIT_RDY_MAX_LOOP 100000
#define MV_SPI_DATA_IN_REG(spi_id) (MV_SPI_REGS_BASE(spi_id) + 0x0c)

#define MV_SPI_TMNG_PARAMS_REG(spi_id) (MV_SPI_REGS_BASE(spi_id) + 0x18)
#define MV_SPI_TMISO_SAMPLE_OFFSET 6
#define MV_SPI_TMISO_SAMPLE_MASK (0x3 << MV_SPI_TMISO_SAMPLE_OFFSET)

#define CONFIG_ENV_SPI_MAX_HZ 50000000
#define CONFIG_SF_DEFAULT_SPEED CONFIG_ENV_SPI_MAX_HZ

#define CMD_READ_ARRAY_FAST 0x0b

/******************************************************************************
base type define end
*******************************************************************************/

/******************************************************************************
struct define
*******************************************************************************/
typedef enum {
	SPI_TYPE_FLASH = 0,
	SPI_TYPE_SLIC_ZARLINK_SILABS,
	SPI_TYPE_SLIC_LANTIQ,
	SPI_TYPE_SLIC_ZSI,
	SPI_TYPE_SLIC_ISI
} MV_SPI_TYPE;

typedef struct {
	unsigned short ctrl_model;
	unsigned int tclk;
} MV_SPI_HAL_DATA;

typedef struct {
	int clock_pol_low;
	enum { SPI_CLK_HALF_CYC, SPI_CLK_BEGIN_CYC } clock_phase;
	int tx_msb_first;
	int rx_msb_first;
} MV_SPI_IF_PARAMS;

typedef struct {
	/* Does this device support 16 bits access */
	int en16_bit;
	/* should we assert / disassert CS for each byte we read / write */
	int byte_cs_asrt;
	int clock_pol_low;
	unsigned int baud_rate;
	unsigned int clk_phase;
} MV_SPI_TYPE_INFO;

/******************************************************************************
struct define end
*******************************************************************************/

/******************************************************************************
param define
*******************************************************************************/
static MV_SPI_HAL_DATA spi_hal_data;
static MV_SPI_TYPE_INFO *curr_spi_info = NULL;
static MV_SPI_TYPE_INFO spi_types[] = { {.en16_bit = MV_TRUE,
				       .clock_pol_low = MV_TRUE,
				       .byte_cs_asrt = MV_FALSE,
				       .baud_rate = (20 << 20), /*  20_m */
				       .clk_phase = SPI_CLK_BEGIN_CYC},
				      {.en16_bit = MV_FALSE,
				       .clock_pol_low = MV_TRUE,
				       .byte_cs_asrt = MV_TRUE,
				       .baud_rate = 0x00800000,
				       .clk_phase = SPI_CLK_BEGIN_CYC},
				      {.en16_bit = MV_FALSE,
				       .clock_pol_low = MV_TRUE,
				       .byte_cs_asrt = MV_FALSE,
				       .baud_rate = 0x00800000,
				       .clk_phase = SPI_CLK_BEGIN_CYC},
				      {.en16_bit = MV_FALSE,
				       .clock_pol_low = MV_TRUE,
				       .byte_cs_asrt = MV_TRUE,
				       .baud_rate = 0x00800000,
				       .clk_phase = SPI_CLK_HALF_CYC},
				      {.en16_bit = MV_FALSE,
				       .clock_pol_low = MV_FALSE,
				       .byte_cs_asrt = MV_TRUE,
				       .baud_rate = 0x00200000,
				       .clk_phase = SPI_CLK_HALF_CYC} };

/******************************************************************************
param define end
*******************************************************************************/

static struct spi_slave s_spi;

static int mv_spi_baud_rate_set(unsigned char spi_id,
			unsigned int serial_baud_rate);
static void mv_spi_cs_deassert(unsigned char spi_id);
static int mv_spi_cs_set(unsigned char spi_id, unsigned char cs_id);
static int mv_spi_if_config_set(unsigned char spi_id,
			MV_SPI_IF_PARAMS *if_params);
static int mv_spi_params_set(unsigned char spi_id,
			  unsigned char cs_id,
			  MV_SPI_TYPE type);
static int mv_spi_init(unsigned char spi_id,
		     unsigned char cs_id,
		     unsigned int serial_baud_rate,
		     MV_SPI_HAL_DATA *hal_data);
static int mv_spi_sys_init(unsigned char spi_id,
		    unsigned char cs_id,
		    unsigned int serial_baud_rate);
static void mv_spi_cs_assert(unsigned char spi_id);
static int mv_spi_8bit_data_tx_rx(unsigned char spi_id,
			     unsigned char tx_data,
			     unsigned char *p_rx_data);

int mv_spi_baud_rate_set(unsigned char spi_id, unsigned int serial_baud_rate)
{
	unsigned int spr, sppr;
	unsigned int divider;
	unsigned int best_spr = 0, best_sppr = 0;
	unsigned char exact_match = 0;
	unsigned int min_baud_offset = 0xFFFFFFFF;
	unsigned int cpu_clk = spi_hal_data.tclk; /*mv_cpu_pclk_get();*/
	unsigned int temp_reg;

	assert(cpu_clk != serial_baud_rate);
	/* Find the best prescale configuration - less or equal */
	for (spr = 1; spr <= 15; spr++) {
		for (sppr = 0; sppr <= 7; sppr++) {
			divider = spr * (1 << sppr);
			/* check for higher - irrelevant */
			if ((cpu_clk / divider) > serial_baud_rate)
				continue;

			/* check for exact fit */
			if ((cpu_clk / divider) == serial_baud_rate) {
				best_spr = spr;
				best_sppr = sppr;
				exact_match = 1;
				break;
			}

			/* check if this is better than the previous one */
			if ((serial_baud_rate - (cpu_clk / divider)) <
			    min_baud_offset) {
				min_baud_offset =
				    serial_baud_rate - cpu_clk / divider;
				best_spr = spr;
				best_sppr = sppr;
			}
		}

		if (exact_match == 1)
			break;
	}

	if (best_spr == 0) {
		printk(BIOS_INFO, "%s ERROR: SPI baud rate prescale error!\n",
		       __func__);
		return MV_OUT_OF_RANGE;
	}

	/* configure the Prescale */
	temp_reg = MV_SPI_REG_READ(MV_SPI_IF_CONFIG_REG(spi_id)) &
		  ~(MV_SPI_SPR_MASK | MV_SPI_SPPR_0_MASK | MV_SPI_SPPR_HI_MASK);
	temp_reg |= ((best_spr << MV_SPI_SPR_OFFSET) |
		    ((best_sppr & 0x1) << MV_SPI_SPPR_0_OFFSET) |
		    ((best_sppr >> 1) << MV_SPI_SPPR_HI_OFFSET));
	MV_SPI_REG_WRITE(MV_SPI_IF_CONFIG_REG(spi_id), temp_reg);

	return MV_OK;
}

void mv_spi_cs_deassert(unsigned char spi_id)
{
	MV_SPI_REG_BIT_RESET(MV_SPI_IF_CTRL_REG(spi_id), MV_SPI_CS_ENABLE_MASK);
}

int mv_spi_cs_set(unsigned char spi_id, unsigned char cs_id)
{
	unsigned int ctrl_reg;
	static unsigned char last_cs_id = 0xFF;
	static unsigned char last_spi_id = 0xFF;

	if (cs_id > 7)
		return MV_BAD_PARAM;

	if ((last_spi_id == spi_id) && (last_cs_id == cs_id))
		return MV_OK;

	ctrl_reg = MV_SPI_REG_READ(MV_SPI_IF_CTRL_REG(spi_id));
	ctrl_reg &= ~MV_SPI_CS_NUM_MASK;
	ctrl_reg |= (cs_id << MV_SPI_CS_NUM_OFFSET);
	MV_SPI_REG_WRITE(MV_SPI_IF_CTRL_REG(spi_id), ctrl_reg);

	last_spi_id = spi_id;
	last_cs_id = cs_id;

	return MV_OK;
}

int mv_spi_if_config_set(unsigned char spi_id, MV_SPI_IF_PARAMS *if_params)
{
	unsigned int ctrl_reg;

	ctrl_reg = MV_SPI_REG_READ(MV_SPI_IF_CONFIG_REG(spi_id));

	/* Set Clock Polarity */
	ctrl_reg &= ~(MV_SPI_CPOL_MASK | MV_SPI_CPHA_MASK | MV_SPI_TXLSBF_MASK |
		     MV_SPI_RXLSBF_MASK);
	if (if_params->clock_pol_low)
		ctrl_reg |= MV_SPI_CPOL_MASK;

	if (if_params->clock_phase == SPI_CLK_BEGIN_CYC)
		ctrl_reg |= MV_SPI_CPHA_MASK;

	if (if_params->tx_msb_first)
		ctrl_reg |= MV_SPI_TXLSBF_MASK;

	if (if_params->rx_msb_first)
		ctrl_reg |= MV_SPI_RXLSBF_MASK;

	MV_SPI_REG_WRITE(MV_SPI_IF_CONFIG_REG(spi_id), ctrl_reg);

	return MV_OK;
}

int mv_spi_params_set(unsigned char spi_id,
		unsigned char cs_id,
		MV_SPI_TYPE type)
{
	MV_SPI_IF_PARAMS if_params;

	if (MV_OK != mv_spi_cs_set(spi_id, cs_id)) {
		printk(BIOS_INFO, "Error, setting SPI CS failed\n");
		return MV_ERROR;
	}

	if (curr_spi_info != (&(spi_types[type]))) {
		curr_spi_info = &(spi_types[type]);
		mv_spi_baud_rate_set(spi_id, curr_spi_info->baud_rate);

		if_params.clock_pol_low = curr_spi_info->clock_pol_low;
		if_params.clock_phase = curr_spi_info->clk_phase;
		if_params.tx_msb_first = MV_FALSE;
		if_params.rx_msb_first = MV_FALSE;
		mv_spi_if_config_set(spi_id, &if_params);
	}

	return MV_OK;
}

int mv_spi_init(unsigned char spi_id,
	      unsigned char cs_id,
	      unsigned int serial_baud_rate,
	      MV_SPI_HAL_DATA *hal_data)
{
	int ret;
	unsigned int timing_reg;

	spi_hal_data.ctrl_model = hal_data->ctrl_model;
	spi_hal_data.tclk = hal_data->tclk;

	/* Set the serial clock */
	ret = mv_spi_baud_rate_set(spi_id, serial_baud_rate);
	if (ret != MV_OK)
		return ret;

	/* Configure the default SPI mode to be 8bit */
	MV_SPI_REG_BIT_RESET(MV_SPI_IF_CONFIG_REG(spi_id),
			   MV_SPI_BYTE_LENGTH_MASK);

	timing_reg = MV_SPI_REG_READ(MV_SPI_TMNG_PARAMS_REG(spi_id));
	timing_reg &= ~MV_SPI_TMISO_SAMPLE_MASK;
	timing_reg |= (0x2) << MV_SPI_TMISO_SAMPLE_OFFSET;
	MV_SPI_REG_WRITE(MV_SPI_TMNG_PARAMS_REG(spi_id), timing_reg);

	/* Verify that the CS is deasserted */
	mv_spi_cs_deassert(spi_id);

	mv_spi_params_set(spi_id, cs_id, SPI_TYPE_FLASH);

	return MV_OK;
}

int mv_spi_sys_init(unsigned char spi_id,
		unsigned char cs_id,
		unsigned int serial_baud_rate)
{
	MV_SPI_HAL_DATA hal_data;

	hal_data.ctrl_model = MV_6810_DEV_ID;
	hal_data.tclk = mv_tclk_get();

	return mv_spi_init(spi_id, cs_id, serial_baud_rate, &hal_data);
}

void mv_spi_cs_assert(unsigned char spi_id)
{
	MV_SPI_REG_BIT_SET(MV_SPI_IF_CTRL_REG(spi_id), MV_SPI_CS_ENABLE_MASK);
}

int mv_spi_8bit_data_tx_rx(unsigned char spi_id,
		      unsigned char tx_data,
		      unsigned char *p_rx_data)
{
	unsigned int i;
	int ready = MV_FALSE;

	if (curr_spi_info->byte_cs_asrt)
		mv_spi_cs_assert(spi_id);

	/* First clear the bit in the interrupt cause register */
	MV_SPI_REG_WRITE(MV_SPI_INT_CAUSE_REG(spi_id), 0x0);

	/* Transmit data */
	MV_SPI_REG_WRITE(MV_SPI_DATA_OUT_REG(spi_id), tx_data);

	/* wait with timeout for memory ready */
	for (i = 0; i < MV_SPI_WAIT_RDY_MAX_LOOP; i++) {
		if (MV_SPI_REG_READ(MV_SPI_INT_CAUSE_REG(spi_id))) {
			ready = MV_TRUE;
			break;
		}
	}

	if (!ready) {
		if (curr_spi_info->byte_cs_asrt) {
			mv_spi_cs_deassert(spi_id);
			/* WA to compansate Zarlink SLIC CS off time */
			udelay(4);
		}
		return MV_TIMEOUT;
	}

	/* check that the RX data is needed */
	if (p_rx_data)
		*p_rx_data = MV_SPI_REG_READ(MV_SPI_DATA_IN_REG(spi_id));

	if (curr_spi_info->byte_cs_asrt) {
		mv_spi_cs_deassert(spi_id);
		/* WA to compansate Zarlink SLIC CS off time */
		udelay(4);
	}

	return MV_OK;
}

static int mrvl_spi_xfer(const struct spi_slave *slave,
			 size_t bitlen,
			 const void *dout,
			 void *din)
{
	int ret;
	unsigned char *pdout = (unsigned char *)dout;
	unsigned char *pdin = (unsigned char *)din;
	int tmp_bitlen = bitlen;
	unsigned char tmp_dout = 0;

	/* Verify that the SPI mode is in 8bit mode */
	MV_SPI_REG_BIT_RESET(MV_SPI_IF_CONFIG_REG(slave->bus),
			 MV_SPI_BYTE_LENGTH_MASK);

	while (tmp_bitlen > 0) {
		if (pdout)
			tmp_dout = (*pdout) & 0xff;

		/* Transmitted and wait for the transfer to be completed */
		ret = mv_spi_8bit_data_tx_rx(slave->bus, tmp_dout, pdin);
		if (ret != MV_OK)
			return ret;

		/* increment the pointers */
		if (pdin)
			pdin++;
		if (pdout)
			pdout++;

		tmp_bitlen -= 8;
	}
	return 0;
}

struct spi_slave *spi_setup_slave(unsigned int bus, unsigned int cs)
{
	struct spi_slave *slave = &s_spi;

	slave->bus = bus;
	slave->cs = cs;
	mv_spi_sys_init(bus, cs, CONFIG_SF_DEFAULT_SPEED);
	return slave;
}

int spi_claim_bus(const struct spi_slave *slave)
{
	mv_spi_cs_set(slave->bus, slave->cs);
	mv_spi_cs_assert(slave->bus);
	return 0;
}

void spi_release_bus(const struct spi_slave *slave)
{
	mv_spi_cs_deassert(slave->bus);
}

unsigned int spi_crop_chunk(unsigned int cmd_len, unsigned int buf_len)
{
	return buf_len;
}

int spi_xfer(const struct spi_slave *slave,
	     const void *dout,
	     size_t out_bytes,
	     void *din,
	     size_t in_bytes)
{
	int ret = -1;

	if (out_bytes)
		ret = mrvl_spi_xfer(slave, out_bytes * 8, dout, din);
	else if (in_bytes)
		ret = mrvl_spi_xfer(slave, in_bytes * 8, dout, din);
	else
		die("Unexpected condition in spi_xfer\n");
	return ret;
}
