/*
 * Copyright (C) 2015 Broadcom Corporation
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
#include <timer.h>
#include <delay.h>
#include <stdlib.h>
#include <spi-generic.h>
#include <spi_flash.h>
#include <soc/addressmap.h>

#define IPROC_QSPI_CLK	100000000

/* SPI mode flags */
#define	SPI_CPHA	0x01		/* clock phase */
#define	SPI_CPOL	0x02		/* clock polarity */
#define	SPI_MODE_0	(0|0)		/* original MicroWire */
#define	SPI_MODE_1	(0|SPI_CPHA)
#define	SPI_MODE_2	(SPI_CPOL|0)
#define	SPI_MODE_3	(SPI_CPOL|SPI_CPHA)

#define QSPI_MAX_HZ			50000000
#define QSPI_MODE			SPI_MODE_3

#define QSPI_WAIT_TIMEOUT		200U  /* msec */

/* Controller attributes */
#define SPBR_MIN			8U
#define SPBR_MAX			255U
#define NUM_TXRAM			32
#define NUM_RXRAM			32
#define NUM_CDRAM			16

/*
 * Register fields
 */
#define MSPI_SPCR0_MSB_BITS_8		0x00000020

/* BSPI registers */
#define BSPI_MAST_N_BOOT_CTRL_REG	0x008
#define BSPI_BUSY_STATUS_REG		0x00c

/* MSPI registers */
#define MSPI_SPCR0_LSB_REG		0x200
#define MSPI_SPCR0_MSB_REG		0x204
#define MSPI_SPCR1_LSB_REG		0x208
#define MSPI_SPCR1_MSB_REG		0x20c
#define MSPI_NEWQP_REG			0x210
#define MSPI_ENDQP_REG			0x214
#define MSPI_SPCR2_REG			0x218
#define MSPI_STATUS_REG			0x220
#define MSPI_CPTQP_REG			0x224
#define MSPI_TXRAM_REG			0x240
#define MSPI_RXRAM_REG			0x2c0
#define MSPI_CDRAM_REG			0x340
#define MSPI_WRITE_LOCK_REG		0x380
#define MSPI_DISABLE_FLUSH_GEN_REG	0x384

/*
 * Register access macros
 */
#define REG_RD(x)	read32(x)
#define REG_WR(x, y)	write32((x), (y))
#define REG_CLR(x, y)	REG_WR((x), REG_RD(x) & ~(y))
#define REG_SET(x, y)	REG_WR((x), REG_RD(x) | (y))

/* QSPI private data */
struct qspi_priv {
	/* Specified SPI parameters */
	unsigned int max_hz;
	unsigned int spi_mode;

	int mspi_enabled;
	int mspi_16bit;

	int bus_claimed;

	/* Registers */
	void *reg;
};

static struct qspi_priv qspi_slave;

static struct qspi_priv *to_qspi_slave(const struct spi_slave *slave)
{
	return &qspi_slave;
}

static int mspi_enable(struct qspi_priv *priv)
{
	struct stopwatch sw;

	/* Switch to MSPI if not yet */
	if ((REG_RD(priv->reg + BSPI_MAST_N_BOOT_CTRL_REG) & 1) == 0) {
		stopwatch_init_msecs_expire(&sw, QSPI_WAIT_TIMEOUT);
		while (!stopwatch_expired(&sw)) {
			if ((REG_RD(priv->reg + BSPI_BUSY_STATUS_REG) & 1)
			    == 0) {
				REG_WR(priv->reg + BSPI_MAST_N_BOOT_CTRL_REG,
				       1);
				udelay(1);
				break;
			}
			udelay(1);
		}
		if (REG_RD(priv->reg + BSPI_MAST_N_BOOT_CTRL_REG) != 1)
			return -1;
	}
	priv->mspi_enabled = 1;
	return 0;
}

static int spi_ctrlr_claim_bus(const struct spi_slave *slave)
{
	struct qspi_priv *priv = to_qspi_slave(slave);

	if (priv->bus_claimed)
		return -1;

	if (!priv->mspi_enabled)
		if (mspi_enable(priv))
			return -1;

	/* MSPI: Enable write lock */
	REG_WR(priv->reg + MSPI_WRITE_LOCK_REG, 1);

	priv->bus_claimed = 1;

	return 0;
}

static void spi_ctrlr_release_bus(const struct spi_slave *slave)
{
	struct qspi_priv *priv = to_qspi_slave(slave);

	/* MSPI: Disable write lock */
	REG_WR(priv->reg + MSPI_WRITE_LOCK_REG, 0);

	priv->bus_claimed = 0;
}

#define RXRAM_16B(p, i)	(REG_RD((p)->reg + MSPI_RXRAM_REG + ((i) << 2)) & 0xff)
#define RXRAM_8B(p, i)	(REG_RD((p)->reg + MSPI_RXRAM_REG + \
				((((i) << 1) + 1) << 2)) & 0xff)

static int spi_ctrlr_xfer(const struct spi_slave *slave, const void *dout,
			   size_t bytesout, void *din, size_t bytesin)
{
	struct qspi_priv *priv = to_qspi_slave(slave);
	const u8 *tx = (const u8 *)dout;
	u8 *rx = (u8 *)din;
	unsigned int bytes = bytesout + bytesin;
	unsigned int rx_idx = 0;
	unsigned int tx_idx = 0;
	unsigned int in = 0;
	unsigned int chunk;
	unsigned int queues;
	unsigned int i;
	struct stopwatch sw;

	if (!priv->bus_claimed)
		return -1;

	if (bytes & 1) {
		/* Use 8-bit queue for odd-bytes transfer */
		if (priv->mspi_16bit) {
			REG_SET(priv->reg + MSPI_SPCR0_MSB_REG,
				MSPI_SPCR0_MSB_BITS_8);
			priv->mspi_16bit = 0;
		}
	} else {
		/* Use 16-bit queue for even-bytes transfer */
		if (!priv->mspi_16bit) {
			REG_CLR(priv->reg + MSPI_SPCR0_MSB_REG,
				MSPI_SPCR0_MSB_BITS_8);
			priv->mspi_16bit = 1;
		}
	}

	while (bytes) {
		/* Separate code for 16bit and 8bit transfers for performance */
		if (priv->mspi_16bit) {
			/* Determine how many bytes to process this time */
			chunk = min(bytes, NUM_CDRAM * 2);
			queues = (chunk - 1) / 2 + 1;
			bytes -= chunk;

			/* Fill CDRAMs */
			for (i = 0; i < queues; i++)
				REG_WR(priv->reg + MSPI_CDRAM_REG + (i << 2),
				       0xc2);

			/* Fill TXRAMs */
			for (i = 0; i < chunk; i++) {
				REG_WR(priv->reg + MSPI_TXRAM_REG + (i << 2),
				       (tx && (tx_idx < bytesout)) ?
						tx[tx_idx] : 0xff);
				tx_idx++;
			}
		} else {
			/* Determine how many bytes to process this time */
			chunk = min(bytes, NUM_CDRAM);
			queues = chunk;
			bytes -= chunk;

			/* Fill CDRAMs and TXRAMS */
			for (i = 0; i < chunk; i++) {
				REG_WR(priv->reg + MSPI_CDRAM_REG + (i << 2),
				       0x82);
				REG_WR(priv->reg + MSPI_TXRAM_REG + (i << 3),
				       (tx && (tx_idx < bytesout)) ?
						tx[tx_idx] : 0xff);
				tx_idx++;
			}
		}

		/* Setup queue pointers */
		REG_WR(priv->reg + MSPI_NEWQP_REG, 0);
		REG_WR(priv->reg + MSPI_ENDQP_REG, queues - 1);

		/* Deassert CS */
		if (bytes == 0)
			REG_CLR(priv->reg + MSPI_CDRAM_REG +
				((queues - 1) << 2), 0x0);

		/* Kick off */
		REG_WR(priv->reg + MSPI_STATUS_REG, 0);
		REG_WR(priv->reg + MSPI_SPCR2_REG, 0xc0);	/* cont | spe */

		/* Wait for completion */
		stopwatch_init_msecs_expire(&sw, QSPI_WAIT_TIMEOUT);
		while (!stopwatch_expired(&sw)) {
			if (REG_RD(priv->reg + MSPI_STATUS_REG) & 1)
				break;
		}
		if ((REG_RD(priv->reg + MSPI_STATUS_REG) & 1) == 0) {
			/* Make sure no operation is in progress */
			REG_WR(priv->reg + MSPI_SPCR2_REG, 0);
			udelay(1);
			return -1;
		}

		/* Read data */
		if (rx) {
			if (priv->mspi_16bit) {
				for (i = 0; i < chunk; i++) {
					if (rx_idx >= bytesout) {
						rx[in] = RXRAM_16B(priv, i);
						in++;
					}
					rx_idx++;
				}
			} else {
				for (i = 0; i < chunk; i++) {
					if (rx_idx >= bytesout) {
						rx[in] = RXRAM_8B(priv, i);
						in++;
					}
					rx_idx++;
				}
			}
		}
	}

	return 0;
}

static int spi_ctrlr_setup(const struct spi_slave *slave)
{
	struct qspi_priv *priv = &qspi_slave;
	unsigned int spbr;

	priv->max_hz = QSPI_MAX_HZ;
	priv->spi_mode = QSPI_MODE;
	priv->reg = (void *)(IPROC_QSPI_BASE);
	priv->mspi_enabled = 0;
	priv->bus_claimed = 0;

	/* MSPI: Basic hardware initialization */
	REG_WR(priv->reg + MSPI_SPCR1_LSB_REG, 0);
	REG_WR(priv->reg + MSPI_SPCR1_MSB_REG, 0);
	REG_WR(priv->reg + MSPI_NEWQP_REG, 0);
	REG_WR(priv->reg + MSPI_ENDQP_REG, 0);
	REG_WR(priv->reg + MSPI_SPCR2_REG, 0);

	/* MSPI: SCK configuration */
	spbr = (IPROC_QSPI_CLK - 1) / (2 * priv->max_hz) + 1;
	REG_WR(priv->reg + MSPI_SPCR0_LSB_REG,
	       MAX(MIN(spbr, SPBR_MAX), SPBR_MIN));

	/* MSPI: Mode configuration (8 bits by default) */
	priv->mspi_16bit = 0;
	REG_WR(priv->reg + MSPI_SPCR0_MSB_REG,
	       0x80 |			/* Master */
	       (8 << 2) |		/* 8 bits per word */
	       (priv->spi_mode & 3));	/* mode: CPOL / CPHA */

	return 0;
}

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
		.bus_start = CONFIG_BOOT_DEVICE_SPI_FLASH_BUS,
		.bus_end = CONFIG_BOOT_DEVICE_SPI_FLASH_BUS,
	},
};

const size_t spi_ctrlr_bus_map_count = ARRAY_SIZE(spi_ctrlr_bus_map);
