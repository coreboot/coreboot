/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Secure Digital (SD) Host Controller interface specific code
 */

#include "bouncebuf.h"
#include <commonlib/sd_mmc_ctrlr.h>
#include <commonlib/sdhci.h>
#include <commonlib/stdlib.h>
#include <commonlib/storage.h>
#include <delay.h>
#include <endian.h>
#include <lib.h>
#include "sdhci.h"
#include "sd_mmc.h"
#include "storage.h"
#include <timer.h>

#define DMA_AVAILABLE	((CONFIG(SDHCI_ADMA_IN_BOOTBLOCK) && ENV_BOOTBLOCK) \
			|| (CONFIG(SDHCI_ADMA_IN_VERSTAGE) && ENV_SEPARATE_VERSTAGE) \
			|| (CONFIG(SDHCI_ADMA_IN_ROMSTAGE) && ENV_ROMSTAGE) \
			|| ENV_POSTCAR || ENV_RAMSTAGE)

__weak void *dma_malloc(size_t length_in_bytes)
{
	return malloc(length_in_bytes);
}

void sdhci_reset(struct sdhci_ctrlr *sdhci_ctrlr, u8 mask)
{
	struct stopwatch sw;

	/* Wait max 100 ms */
	stopwatch_init_msecs_expire(&sw, 100);

	sdhci_writeb(sdhci_ctrlr, mask, SDHCI_SOFTWARE_RESET);
	while (sdhci_readb(sdhci_ctrlr, SDHCI_SOFTWARE_RESET) & mask) {
		if (stopwatch_expired(&sw)) {
			sdhc_error("Reset 0x%x never completed.\n", (int)mask);
			return;
		}
		udelay(1000);
	}
}

void sdhci_cmd_done(struct sdhci_ctrlr *sdhci_ctrlr, struct mmc_command *cmd)
{
	int i;
	if (cmd->resp_type & CARD_RSP_136) {
		/* CRC is stripped so we need to do some shifting. */
		for (i = 0; i < 4; i++) {
			cmd->response[i] = sdhci_readl(sdhci_ctrlr,
					SDHCI_RESPONSE + (3-i)*4) << 8;
			if (i != 3)
				cmd->response[i] |= sdhci_readb(sdhci_ctrlr,
						SDHCI_RESPONSE + (3-i)*4-1);
		}
		sdhc_log_response(4, &cmd->response[0]);
		sdhc_trace("Response: 0x%08x.%08x.%08x.%08x\n",
			cmd->response[3], cmd->response[2], cmd->response[1],
			cmd->response[0]);
	} else {
		cmd->response[0] = sdhci_readl(sdhci_ctrlr, SDHCI_RESPONSE);
		sdhc_log_response(1, &cmd->response[0]);
		sdhc_trace("Response: 0x%08x\n", cmd->response[0]);
	}
}

static int sdhci_transfer_data(struct sdhci_ctrlr *sdhci_ctrlr,
	struct mmc_data *data, unsigned int start_addr)
{
	uint32_t block_count;
	uint32_t *buffer;
	uint32_t *buffer_end;
	uint32_t ps;
	uint32_t ps_mask;
	uint32_t stat;
	struct stopwatch sw;

	block_count = 0;
	buffer = (uint32_t *)data->dest;
	ps_mask = (data->flags & DATA_FLAG_READ)
		? SDHCI_DATA_AVAILABLE : SDHCI_SPACE_AVAILABLE;
	stopwatch_init_msecs_expire(&sw, 100);
	do {
		/* Stop transfers if there is an error */
		stat = sdhci_readl(sdhci_ctrlr, SDHCI_INT_STATUS);
		sdhci_writel(sdhci_ctrlr, stat, SDHCI_INT_STATUS);
		if (stat & SDHCI_INT_ERROR) {
			sdhc_error("Error detected in status(0x%X)!\n", stat);
			return -1;
		}

		/* Determine if the buffer is ready to move data */
		ps = sdhci_readl(sdhci_ctrlr, SDHCI_PRESENT_STATE);
		if (!(ps & ps_mask)) {
			if (stopwatch_expired(&sw)) {
				sdhc_error("Transfer data timeout\n");
				return -1;
			}
			udelay(1);
			continue;
		}

		/* Transfer a block of data */
		buffer_end = &buffer[data->blocksize >> 2];
		if (data->flags == DATA_FLAG_READ)
			while (buffer_end > buffer)
				*buffer++ = sdhci_readl(sdhci_ctrlr,
					SDHCI_BUFFER);
		else
			while (buffer_end > buffer)
				sdhci_writel(sdhci_ctrlr, *buffer++,
					SDHCI_BUFFER);
		if (++block_count >= data->blocks)
			break;
	} while (!(stat & SDHCI_INT_DATA_END));
	return 0;
}

static int sdhci_send_command_bounced(struct sd_mmc_ctrlr *ctrlr,
	struct mmc_command *cmd, struct mmc_data *data,
	struct bounce_buffer *bbstate)
{
	struct sdhci_ctrlr *sdhci_ctrlr = (struct sdhci_ctrlr *)ctrlr;
	u16 mode = 0;
	unsigned int stat = 0;
	int ret = 0;
	u32 mask, flags;
	unsigned int timeout, start_addr = 0;
	struct stopwatch sw;

	/* Wait max 1 s */
	timeout = 1000;

	sdhci_writel(sdhci_ctrlr, SDHCI_INT_ALL_MASK, SDHCI_INT_STATUS);
	mask = SDHCI_CMD_INHIBIT | SDHCI_DATA_INHIBIT;

	/* We shouldn't wait for data inhibit for stop commands, even
	   though they might use busy signaling */
	if (cmd->flags & CMD_FLAG_IGNORE_INHIBIT)
		mask &= ~SDHCI_DATA_INHIBIT;

	while (sdhci_readl(sdhci_ctrlr, SDHCI_PRESENT_STATE) & mask) {
		if (timeout == 0) {
			sdhc_trace("Cmd: %2d, Arg: 0x%08x, not sent\n",
					cmd->cmdidx, cmd->cmdarg);
			sdhc_error("Controller never released inhibit bit(s), "
			       "present state %#8.8x.\n",
			       sdhci_readl(sdhci_ctrlr, SDHCI_PRESENT_STATE));
			return CARD_COMM_ERR;
		}
		timeout--;
		udelay(1000);
	}

	mask = SDHCI_INT_RESPONSE;
	if (!(cmd->resp_type & CARD_RSP_PRESENT))
		flags = SDHCI_CMD_RESP_NONE;
	else if (cmd->resp_type & CARD_RSP_136)
		flags = SDHCI_CMD_RESP_LONG;
	else if (cmd->resp_type & CARD_RSP_BUSY) {
		flags = SDHCI_CMD_RESP_SHORT_BUSY;
		mask |= SDHCI_INT_DATA_END;
	} else
		flags = SDHCI_CMD_RESP_SHORT;

	if (cmd->resp_type & CARD_RSP_CRC)
		flags |= SDHCI_CMD_CRC;
	if (cmd->resp_type & CARD_RSP_OPCODE)
		flags |= SDHCI_CMD_INDEX;
	if (data)
		flags |= SDHCI_CMD_DATA;

	/* Set Transfer mode regarding to data flag */
	if (data) {
		sdhci_writew(sdhci_ctrlr,
			SDHCI_MAKE_BLKSZ(SDHCI_DEFAULT_BOUNDARY_ARG,
			data->blocksize), SDHCI_BLOCK_SIZE);

		if (data->flags == DATA_FLAG_READ)
			mode |= SDHCI_TRNS_READ;

		if (data->blocks > 1)
			mode |= SDHCI_TRNS_BLK_CNT_EN |
				SDHCI_TRNS_MULTI | SDHCI_TRNS_ACMD12;

		sdhci_writew(sdhci_ctrlr, data->blocks, SDHCI_BLOCK_COUNT);

		if (DMA_AVAILABLE && (ctrlr->caps & DRVR_CAP_AUTO_CMD12)
			&& (cmd->cmdidx != MMC_CMD_AUTO_TUNING_SEQUENCE)) {
			if (sdhci_setup_adma(sdhci_ctrlr, data))
				return -1;
			mode |= SDHCI_TRNS_DMA;
		}
		sdhci_writew(sdhci_ctrlr, mode, SDHCI_TRANSFER_MODE);
	}

	sdhc_trace("Cmd: %2d, Arg: 0x%08x\n", cmd->cmdidx, cmd->cmdarg);
	sdhci_writel(sdhci_ctrlr, cmd->cmdarg, SDHCI_ARGUMENT);
	sdhci_writew(sdhci_ctrlr, SDHCI_MAKE_CMD(cmd->cmdidx, flags),
		SDHCI_COMMAND);
	sdhc_log_command_issued();

	if (DMA_AVAILABLE && (mode & SDHCI_TRNS_DMA))
		return sdhci_complete_adma(sdhci_ctrlr, cmd);

	stopwatch_init_msecs_expire(&sw, 2550);
	do {
		stat = sdhci_readl(sdhci_ctrlr, SDHCI_INT_STATUS);
		if (stat & SDHCI_INT_ERROR) {
			sdhc_trace("Error - IntStatus: 0x%08x\n", stat);
			break;
		}

		if (stat & SDHCI_INT_DATA_AVAIL) {
			sdhci_writel(sdhci_ctrlr, stat, SDHCI_INT_STATUS);
			return 0;
		}

		/* Apply max timeout for R1b-type CMD defined in eMMC ext_csd
		   except for erase ones */
		if (stopwatch_expired(&sw)) {
			if (ctrlr->caps & DRVR_CAP_BROKEN_R1B)
				return 0;
			sdhc_error(
				"Timeout for status update!  IntStatus: 0x%08x\n",
				stat);
			return CARD_TIMEOUT;
		}
	} while ((stat & mask) != mask);

	if ((stat & (SDHCI_INT_ERROR | mask)) == mask) {
		if (cmd->cmdidx)
			sdhci_cmd_done(sdhci_ctrlr, cmd);
		sdhci_writel(sdhci_ctrlr, mask, SDHCI_INT_STATUS);
	} else
		ret = -1;

	if (!ret && data)
		ret = sdhci_transfer_data(sdhci_ctrlr, data, start_addr);

	if (ctrlr->udelay_wait_after_cmd)
		udelay(ctrlr->udelay_wait_after_cmd);

	stat = sdhci_readl(sdhci_ctrlr, SDHCI_INT_STATUS);
	sdhci_writel(sdhci_ctrlr, SDHCI_INT_ALL_MASK, SDHCI_INT_STATUS);

	if (!ret)
		return 0;

	sdhci_reset(sdhci_ctrlr, SDHCI_RESET_CMD);
	sdhci_reset(sdhci_ctrlr, SDHCI_RESET_DATA);
	if (stat & SDHCI_INT_TIMEOUT) {
		sdhc_error("CMD%d timeout, IntStatus: 0x%08x\n", cmd->cmdidx,
			stat);
		return CARD_TIMEOUT;
	}

	sdhc_error("CMD%d failed, IntStatus: 0x%08x\n", cmd->cmdidx, stat);
	return CARD_COMM_ERR;
}

__weak void sdhc_log_command(struct mmc_command *cmd)
{
}

__weak void sdhc_log_command_issued(void)
{
}

__weak void sdhc_log_response(uint32_t entries,
	uint32_t *response)
{
}

__weak void sdhc_log_ret(int ret)
{
}

static void sdhci_led_control(struct sd_mmc_ctrlr *ctrlr, int on)
{
	uint8_t host_ctrl;
	struct sdhci_ctrlr *sdhci_ctrlr = (struct sdhci_ctrlr *)ctrlr;

	host_ctrl = sdhci_readb(sdhci_ctrlr, SDHCI_HOST_CONTROL);
	host_ctrl &= ~SDHCI_CTRL_LED;
	if (on)
		host_ctrl |= SDHCI_CTRL_LED;
	sdhci_writeb(sdhci_ctrlr, host_ctrl, SDHCI_HOST_CONTROL);
}

static int sdhci_send_command(struct sd_mmc_ctrlr *ctrlr,
	struct mmc_command *cmd, struct mmc_data *data)
{
	void *buf;
	unsigned int bbflags;
	size_t len;
	struct bounce_buffer *bbstate = NULL;
	struct bounce_buffer bbstate_val;
	int ret;

	sdhc_log_command(cmd);

	if (CONFIG(SDHCI_BOUNCE_BUFFER) && data) {
		if (data->flags & DATA_FLAG_READ) {
			buf = data->dest;
			bbflags = GEN_BB_WRITE;
		} else {
			buf = (void *)data->src;
			bbflags = GEN_BB_READ;
		}
		len = data->blocks * data->blocksize;

		/*
		 * on some platform(like rk3399 etc) need to worry about
		 * cache coherency, so check the buffer, if not dma
		 * coherent, use bounce_buffer to do DMA management.
		 */
		if (!dma_coherent(buf)) {
			bbstate = &bbstate_val;
			if (bounce_buffer_start(bbstate, buf, len, bbflags)) {
				sdhc_error("Failed to get bounce buffer.\n");
				return -1;
			}
		}
	}

	sdhci_led_control(ctrlr, 1);
	ret = sdhci_send_command_bounced(ctrlr, cmd, data, bbstate);
	sdhci_led_control(ctrlr, 0);
	sdhc_log_ret(ret);

	if (CONFIG(SDHCI_BOUNCE_BUFFER) && bbstate)
		bounce_buffer_stop(bbstate);

	return ret;
}

static int sdhci_set_clock(struct sdhci_ctrlr *sdhci_ctrlr, unsigned int clock)
{
	struct sd_mmc_ctrlr *ctrlr = &sdhci_ctrlr->sd_mmc_ctrlr;
	unsigned int actual, div, clk, timeout;

	/* Turn off the clock if requested */
	actual = clock;
	if (actual == 0) {
		sdhci_writew(sdhci_ctrlr, 0, SDHCI_CLOCK_CONTROL);
		sdhc_debug("SDHCI bus clock: Off\n");
		return 0;
	}

	/* Compute the divisor for the new clock frequency */
	actual = MIN(actual, ctrlr->f_max);
	actual = MAX(actual, ctrlr->f_min);
	if (ctrlr->clock_base <= actual)
		div = 0;
	else {
		/* Version 3.00 divisors must be a multiple of 2. */
		if ((ctrlr->version & SDHCI_SPEC_VER_MASK)
			>= SDHCI_SPEC_300) {
			div = MIN(((ctrlr->clock_base + actual - 1)
				/ actual), SDHCI_MAX_DIV_SPEC_300);
			actual = ctrlr->clock_base / div;
			div += 1;
		} else {
			/* Version 2.00 divisors must be a power of 2. */
			for (div = 1; div < SDHCI_MAX_DIV_SPEC_200; div *= 2) {
				if ((ctrlr->clock_base / div) <= actual)
					break;
			}
			actual = ctrlr->clock_base / div;
		}
		div >>= 1;
	}

	/* Set the new clock frequency */
	if (actual != ctrlr->bus_hz) {
		/* Turn off the clock */
		sdhci_writew(sdhci_ctrlr, 0, SDHCI_CLOCK_CONTROL);

		/* Set the new clock frequency */
		clk = (div & SDHCI_DIV_MASK) << SDHCI_DIVIDER_SHIFT;
		clk |= ((div & SDHCI_DIV_HI_MASK) >> SDHCI_DIV_MASK_LEN)
			<< SDHCI_DIVIDER_HI_SHIFT;
		clk |= SDHCI_CLOCK_INT_EN;
		sdhci_writew(sdhci_ctrlr, clk, SDHCI_CLOCK_CONTROL);

		/* Display the requested clock frequency */
		sdhc_debug("SDHCI bus clock: %d.%03d MHz\n",
				actual / 1000000,
				(actual / 1000) % 1000);

		/* Wait max 20 ms */
		timeout = 20;
		while (!((clk = sdhci_readw(sdhci_ctrlr, SDHCI_CLOCK_CONTROL))
			& SDHCI_CLOCK_INT_STABLE)) {
			if (timeout == 0) {
				sdhc_error(
					"Internal clock never stabilised.\n");
				return -1;
			}
			timeout--;
			udelay(1000);
		}

		clk |= SDHCI_CLOCK_CARD_EN;
		sdhci_writew(sdhci_ctrlr, clk, SDHCI_CLOCK_CONTROL);
		ctrlr->bus_hz = actual;
	}
	return 0;
}

static void sdhci_set_power(struct sdhci_ctrlr *sdhci_ctrlr,
	unsigned short power)
{
	struct sd_mmc_ctrlr *ctrlr = &sdhci_ctrlr->sd_mmc_ctrlr;
	u8 pwr = 0;
	u8 pwr_ctrl;
	const char *voltage;

	if (power != (unsigned short)-1) {
		switch (1 << power) {
		case MMC_VDD_165_195:
			voltage = "1.8";
			pwr = SDHCI_POWER_180;
			break;
		case MMC_VDD_29_30:
		case MMC_VDD_30_31:
			voltage = "3.0";
			pwr = SDHCI_POWER_300;
			break;
		case MMC_VDD_32_33:
		case MMC_VDD_33_34:
			voltage = "3.3";
			pwr = SDHCI_POWER_330;
			break;
		}
	}

	/* Determine the power state */
	pwr_ctrl = sdhci_readb(sdhci_ctrlr, SDHCI_POWER_CONTROL);
	if (pwr == 0) {
		if (pwr_ctrl & SDHCI_POWER_ON)
			sdhc_debug("SDHCI voltage: Off\n");
		sdhci_writeb(sdhci_ctrlr, 0, SDHCI_POWER_CONTROL);
		return;
	}

	/* Determine if the power has changed */
	if (pwr_ctrl != (pwr | SDHCI_POWER_ON)) {
		sdhc_debug("SDHCI voltage: %s Volts\n", voltage);

		/* Select the voltage */
		if (ctrlr->caps & DRVR_CAP_NO_SIMULT_VDD_AND_POWER)
			sdhci_writeb(sdhci_ctrlr, pwr, SDHCI_POWER_CONTROL);

		/* Apply power to the SD/MMC device */
		pwr |= SDHCI_POWER_ON;
		sdhci_writeb(sdhci_ctrlr, pwr, SDHCI_POWER_CONTROL);
	}
}

const u16 speed_driver_voltage[] = {
	0, /*  0: BUS_TIMING_LEGACY */
	0, /*  1: BUS_TIMING_MMC_HS */
	0, /*  2: BUS_TIMING_SD_HS */
	SDHCI_CTRL_UHS_SDR12 | SDHCI_CTRL_VDD_180, /* 3: BUS_TIMING_UHS_SDR12 */
	SDHCI_CTRL_UHS_SDR25 | SDHCI_CTRL_VDD_180, /* 4: BUS_TIMING_UHS_SDR25 */
	SDHCI_CTRL_UHS_SDR50 | SDHCI_CTRL_VDD_180, /* 5: BUS_TIMING_UHS_SDR50 */
	/*  6: BUS_TIMING_UHS_SDR104 */
	SDHCI_CTRL_UHS_SDR104 | SDHCI_CTRL_DRV_TYPE_A | SDHCI_CTRL_VDD_180,
	SDHCI_CTRL_UHS_DDR50 | SDHCI_CTRL_VDD_180, /* 7: BUS_TIMING_UHS_DDR50 */
	SDHCI_CTRL_UHS_DDR50 | SDHCI_CTRL_VDD_180, /* 8: BUS_TIMING_MMC_DDR52 */
	/*  9: BUS_TIMING_MMC_HS200 */
	SDHCI_CTRL_UHS_SDR104 | SDHCI_CTRL_DRV_TYPE_A | SDHCI_CTRL_VDD_180,
	/* 10: BUS_TIMING_MMC_HS400 */
	SDHCI_CTRL_HS400 | SDHCI_CTRL_DRV_TYPE_A | SDHCI_CTRL_VDD_180,
	/* 11: BUS_TIMING_MMC_HS400ES */
	SDHCI_CTRL_HS400 | SDHCI_CTRL_DRV_TYPE_A | SDHCI_CTRL_VDD_180
};

static void sdhci_set_uhs_signaling(struct sdhci_ctrlr *sdhci_ctrlr,
	uint32_t timing)
{
	u16 ctrl_2;

	/* Select bus speed mode, driver and VDD 1.8 volt support */
	ctrl_2 = sdhci_readw(sdhci_ctrlr, SDHCI_HOST_CONTROL2);
	ctrl_2 &= ~(SDHCI_CTRL_UHS_MASK | SDHCI_CTRL_DRV_TYPE_MASK
		| SDHCI_CTRL_VDD_180);
	if (timing < ARRAY_SIZE(speed_driver_voltage))
		ctrl_2 |= speed_driver_voltage[timing];
	sdhci_writew(sdhci_ctrlr, ctrl_2, SDHCI_HOST_CONTROL2);
}

static void sdhci_set_ios(struct sd_mmc_ctrlr *ctrlr)
{
	struct sdhci_ctrlr *sdhci_ctrlr = (struct sdhci_ctrlr *)ctrlr;
	u32 ctrl;
	u32 previous_ctrl;
	u32 bus_width;
	int version;

	/* Set the clock frequency */
	if (ctrlr->bus_hz != ctrlr->request_hz)
		sdhci_set_clock(sdhci_ctrlr, ctrlr->request_hz);

	/* Switch to 1.8 volt for HS200 */
	if (ctrlr->caps & DRVR_CAP_1V8_VDD)
		if (ctrlr->bus_hz == CLOCK_200MHZ)
			sdhci_set_power(sdhci_ctrlr, MMC_VDD_165_195_SHIFT);

	/* Determine the new bus width */
	bus_width = 1;
	ctrl = sdhci_readb(sdhci_ctrlr, SDHCI_HOST_CONTROL);
	previous_ctrl = ctrl;
	ctrl &= ~SDHCI_CTRL_4BITBUS;
	version = ctrlr->version & SDHCI_SPEC_VER_MASK;
	if (version >= SDHCI_SPEC_300)
		ctrl &= ~SDHCI_CTRL_8BITBUS;

	if ((ctrlr->bus_width == 8) && (version >= SDHCI_SPEC_300)) {
		ctrl |= SDHCI_CTRL_8BITBUS;
		bus_width = 8;
	} else if (ctrlr->bus_width == 4) {
		ctrl |= SDHCI_CTRL_4BITBUS;
		bus_width = 4;
	}

	if (!(ctrlr->timing == BUS_TIMING_LEGACY) &&
	    !(ctrlr->caps & DRVR_CAP_NO_HISPD_BIT))
		ctrl |= SDHCI_CTRL_HISPD;
	else
		ctrl &= ~SDHCI_CTRL_HISPD;

	sdhci_set_uhs_signaling(sdhci_ctrlr, ctrlr->timing);

	if (DMA_AVAILABLE) {
		if (ctrlr->caps & DRVR_CAP_AUTO_CMD12) {
			ctrl &= ~SDHCI_CTRL_DMA_MASK;
			if (ctrlr->caps & DRVR_CAP_DMA_64BIT)
				ctrl |= SDHCI_CTRL_ADMA64;
			else
				ctrl |= SDHCI_CTRL_ADMA32;
		}
	}

	/* Set the new bus width */
	if (CONFIG(SDHC_DEBUG)
		&& ((ctrl ^ previous_ctrl) & (SDHCI_CTRL_4BITBUS
		| ((version >= SDHCI_SPEC_300) ? SDHCI_CTRL_8BITBUS : 0))))
		sdhc_debug("SDHCI bus width: %d bit%s\n", bus_width,
			(bus_width != 1) ? "s" : "");
	sdhci_writeb(sdhci_ctrlr, ctrl, SDHCI_HOST_CONTROL);
}

static void sdhci_tuning_start(struct sd_mmc_ctrlr *ctrlr, int retune)
{
	uint16_t host_ctrl2;
	struct sdhci_ctrlr *sdhci_ctrlr = (struct sdhci_ctrlr *)ctrlr;

	/* Start the bus tuning */
	host_ctrl2 = sdhci_readw(sdhci_ctrlr, SDHCI_HOST_CONTROL2);
	host_ctrl2 &= ~SDHCI_CTRL_TUNED_CLK;
	host_ctrl2 |= (retune ? SDHCI_CTRL_TUNED_CLK : 0)
		| SDHCI_CTRL_EXEC_TUNING;
	sdhci_writew(sdhci_ctrlr, host_ctrl2, SDHCI_HOST_CONTROL2);
}

static int sdhci_is_tuning_complete(struct sd_mmc_ctrlr *ctrlr, int *successful)
{
	uint16_t host_ctrl2;
	struct sdhci_ctrlr *sdhci_ctrlr = (struct sdhci_ctrlr *)ctrlr;

	/* Determine if the bus tuning has completed */
	host_ctrl2 = sdhci_readw(sdhci_ctrlr, SDHCI_HOST_CONTROL2);
	*successful = ((host_ctrl2 & SDHCI_CTRL_TUNED_CLK) != 0);
	return ((host_ctrl2 & SDHCI_CTRL_EXEC_TUNING) == 0);
}

/* Prepare SDHCI controller to be initialized */
static int sdhci_pre_init(struct sdhci_ctrlr *sdhci_ctrlr)
{
	struct sd_mmc_ctrlr *ctrlr = &sdhci_ctrlr->sd_mmc_ctrlr;
	unsigned int caps, caps_1;

	/* Get controller version and capabilities */
	ctrlr->version = sdhci_readw(sdhci_ctrlr, SDHCI_HOST_VERSION) & 0xff;
	caps = sdhci_readl(sdhci_ctrlr, SDHCI_CAPABILITIES);
	caps_1 = sdhci_readl(sdhci_ctrlr, SDHCI_CAPABILITIES_1);

	/* Determine the supported voltages */
	if (caps & SDHCI_CAN_VDD_330)
		ctrlr->voltages |= MMC_VDD_32_33 | MMC_VDD_33_34;
	if (caps & SDHCI_CAN_VDD_300)
		ctrlr->voltages |= MMC_VDD_29_30 | MMC_VDD_30_31;
	if (caps & SDHCI_CAN_VDD_180)
		ctrlr->voltages |= MMC_VDD_165_195;

	/* Get the controller's base clock frequency */
	if ((ctrlr->version & SDHCI_SPEC_VER_MASK) >= SDHCI_SPEC_300)
		ctrlr->clock_base = (caps & SDHCI_CLOCK_V3_BASE_MASK)
			>> SDHCI_CLOCK_BASE_SHIFT;
	else
		ctrlr->clock_base = (caps & SDHCI_CLOCK_BASE_MASK)
			>> SDHCI_CLOCK_BASE_SHIFT;
	ctrlr->clock_base *= 1000000;
	ctrlr->f_max = ctrlr->clock_base;

	/* Determine the controller's clock frequency range */
	ctrlr->f_min = 0;
	if ((ctrlr->version & SDHCI_SPEC_VER_MASK) >= SDHCI_SPEC_300)
		ctrlr->f_min =
			ctrlr->clock_base / SDHCI_MAX_DIV_SPEC_300;
	else
		ctrlr->f_min =
			ctrlr->clock_base / SDHCI_MAX_DIV_SPEC_200;

	/* Determine the controller's modes of operation */
	ctrlr->caps |= DRVR_CAP_HS52 | DRVR_CAP_HS;
	if (ctrlr->clock_base >= CLOCK_200MHZ) {
		ctrlr->caps |= DRVR_CAP_HS200 | DRVR_CAP_HS200_TUNING;
		if (caps_1 & SDHCI_SUPPORT_HS400)
			ctrlr->caps |= DRVR_CAP_HS400
				| DRVR_CAP_ENHANCED_STROBE;
	}

	/* Determine the bus widths the controller supports */
	ctrlr->caps |= DRVR_CAP_4BIT;
	if (caps & SDHCI_CAN_DO_8BIT)
		ctrlr->caps |= DRVR_CAP_8BIT;

	/* Determine the controller's DMA support */
	if (caps & SDHCI_CAN_DO_ADMA2)
		ctrlr->caps |= DRVR_CAP_AUTO_CMD12;
	if (DMA_AVAILABLE && (caps & SDHCI_CAN_64BIT))
		ctrlr->caps |= DRVR_CAP_DMA_64BIT;

	/* Specify the modes that the driver stack supports */
	ctrlr->caps |= DRVR_CAP_HC;

	/* Let the SOC adjust the configuration to handle controller quirks */
	soc_sd_mmc_controller_quirks(&sdhci_ctrlr->sd_mmc_ctrlr);
	if (ctrlr->clock_base == 0) {
		sdhc_error("Hardware doesn't specify base clock frequency\n");
		return -1;
	}
	if (!ctrlr->f_max)
		ctrlr->f_max = ctrlr->clock_base;

	/* Display the results */
	sdhc_trace("0x%08x: ctrlr->caps\n", ctrlr->caps);
	sdhc_trace("%d.%03d MHz: ctrlr->clock_base\n",
		ctrlr->clock_base / 1000000,
		(ctrlr->clock_base / 1000) % 1000);
	sdhc_trace("%d.%03d MHz: ctrlr->f_max\n",
		ctrlr->f_max / 1000000,
		(ctrlr->f_max / 1000) % 1000);
	sdhc_trace("%d.%03d MHz: ctrlr->f_min\n",
		ctrlr->f_min / 1000000,
		(ctrlr->f_min / 1000) % 1000);
	sdhc_trace("0x%08x: ctrlr->voltages\n", ctrlr->voltages);

	sdhci_reset(sdhci_ctrlr, SDHCI_RESET_ALL);

	return 0;
}

__weak void soc_sd_mmc_controller_quirks(struct sd_mmc_ctrlr
	*ctrlr)
{
}

static int sdhci_init(struct sdhci_ctrlr *sdhci_ctrlr)
{
	struct sd_mmc_ctrlr *ctrlr = &sdhci_ctrlr->sd_mmc_ctrlr;
	int rv;

	/* Only initialize the controller upon reset or card insertion */
	if (ctrlr->initialized)
		return 0;

	sdhc_debug("SDHCI Controller Base Address: %p\n",
			sdhci_ctrlr->ioaddr);

	rv = sdhci_pre_init(sdhci_ctrlr);
	if (rv)
		return rv; /* The error has been already reported */

	sdhci_set_power(sdhci_ctrlr, __fls(ctrlr->voltages));

	if (ctrlr->caps & DRVR_CAP_NO_CD) {
		unsigned int status;

		sdhci_writel(sdhci_ctrlr, SDHCI_CTRL_CD_TEST_INS
			| SDHCI_CTRL_CD_TEST, SDHCI_HOST_CONTROL);

		status = sdhci_readl(sdhci_ctrlr, SDHCI_PRESENT_STATE);
		while ((!(status & SDHCI_CARD_PRESENT)) ||
		    (!(status & SDHCI_CARD_STATE_STABLE)) ||
		    (!(status & SDHCI_CARD_DETECT_PIN_LEVEL)))
			status = sdhci_readl(sdhci_ctrlr, SDHCI_PRESENT_STATE);
	}

	/* Enable only interrupts served by the SD controller */
	sdhci_writel(sdhci_ctrlr, SDHCI_INT_DATA_MASK | SDHCI_INT_CMD_MASK,
		     SDHCI_INT_ENABLE);
	/* Mask all sdhci interrupt sources */
	sdhci_writel(sdhci_ctrlr, 0x0, SDHCI_SIGNAL_ENABLE);

	/* Set timeout to maximum, shouldn't happen if everything's right. */
	sdhci_writeb(sdhci_ctrlr, 0xe, SDHCI_TIMEOUT_CONTROL);

	mdelay(10);
	ctrlr->initialized = 1;
	return 0;
}

static int sdhci_update(struct sdhci_ctrlr *sdhci_ctrlr)
{
	struct sd_mmc_ctrlr *ctrlr = &sdhci_ctrlr->sd_mmc_ctrlr;

	if (ctrlr->caps & DRVR_CAP_REMOVABLE) {
		int present = (sdhci_readl(sdhci_ctrlr, SDHCI_PRESENT_STATE) &
			       SDHCI_CARD_PRESENT) != 0;

		if (!present) {
			/* A card was present indicate the controller needs
			 * initialization on the next call.
			 */
			ctrlr->initialized = 0;
			return 0;
		}
	}

	/* A card is present, get it ready. */
	if (sdhci_init(sdhci_ctrlr))
		return -1;
	return 0;
}

void sdhci_update_pointers(struct sdhci_ctrlr *sdhci_ctrlr)
{
	struct sd_mmc_ctrlr *ctrlr = &sdhci_ctrlr->sd_mmc_ctrlr;

	/* Update the routine pointers */
	ctrlr->send_cmd = &sdhci_send_command;
	ctrlr->set_ios = &sdhci_set_ios;
	ctrlr->tuning_start = &sdhci_tuning_start;
	ctrlr->is_tuning_complete = &sdhci_is_tuning_complete;
}

int add_sdhci(struct sdhci_ctrlr *sdhci_ctrlr)
{
	struct sd_mmc_ctrlr *ctrlr = &sdhci_ctrlr->sd_mmc_ctrlr;

	sdhci_update_pointers(sdhci_ctrlr);

	/* TODO(vbendeb): check if SDHCI spec allows to retrieve this value. */
	ctrlr->b_max = 65535;

	/* Initialize the SDHC controller */
	return sdhci_update(sdhci_ctrlr);
}
