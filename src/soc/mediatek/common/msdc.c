/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * MTK MSDC Host Controller interface specific code
 */
#include <assert.h>
#include <cbmem.h>
#include <commonlib/bsd/helpers.h>
#include <commonlib/storage/sd_mmc.h>
#include <console/console.h>
#include <delay.h>
#include <device/mmio.h>
#include <lib.h>
#include <soc/msdc.h>
#include <string.h>
#include <timer.h>

static inline void msdc_set_field(void *reg, u32 field, u32 val)
{
	clrsetbits32(reg, field, val << __ffs(field));
}

/*
 * Periodically poll an address until a condition is met or a timeout occurs
 * @addr: Address to poll
 * @mask: mask condition
 * @timeout: Timeout in us, 0 means never timeout
 *
 * Returns 0 on success and -MSDC_NOT_READY upon a timeout.
 */
static int msdc_poll_timeout(void *addr, u32 mask)
{
	struct stopwatch timer;
	stopwatch_init_usecs_expire(&timer, MSDC_TIMEOUT_US);
	u32 reg;

	do {
		reg = read32(addr);
		if (stopwatch_expired(&timer))
			return -MSDC_NOT_READY;
		udelay(1);
	} while (reg & mask);

	return MSDC_SUCCESS;
}

/*
 * Wait for a bit mask in a given register. To avoid endless loops, a
 * time-out is implemented here.
 */
static int msdc_wait_done(void *addr, u32 mask, u32 *status)
{
	struct stopwatch timer;
	stopwatch_init_usecs_expire(&timer, CMD_TIMEOUT_MS);
	u32 reg;

	do {
		reg = read32(addr);
		if (stopwatch_expired(&timer)) {
			if (status)
				*status = reg;
			return -MSDC_NOT_READY;
		}
		udelay(1);
	} while (!(reg & mask));

	if (status)
		*status = reg;

	return MSDC_SUCCESS;
}

static void msdc_reset_hw(struct msdc_ctrlr *host)
{
	u32 val;

	setbits32(host->base + MSDC_CFG, MSDC_CFG_RST);
	if (msdc_poll_timeout(host->base + MSDC_CFG, MSDC_CFG_RST) != MSDC_SUCCESS)
		msdc_error("Softwave reset timeout!\n");

	setbits32(host->base + MSDC_FIFOCS, MSDC_FIFOCS_CLR);
	if (msdc_poll_timeout(host->base + MSDC_FIFOCS, MSDC_FIFOCS_CLR) != MSDC_SUCCESS)
		msdc_error("Clear FIFO timeout!\n");

	val = read32(host->base + MSDC_INT);
	write32(host->base + MSDC_INT, val);
}

static void msdc_init_hw(struct msdc_ctrlr *host)
{
	/* Configure to MMC/SD mode */
	setbits32(host->base + MSDC_CFG, MSDC_CFG_MODE);

	/* Reset */
	msdc_reset_hw(host);

	/* Set PIO mode */
	setbits32(host->base + MSDC_CFG, MSDC_CFG_PIO);

	write32(host->top_base + EMMC_TOP_CONTROL, 0);
	write32(host->top_base + EMMC_TOP_CMD, 0);

	write32(host->base + MSDC_IOCON, 0);
	clrbits32(host->base + MSDC_IOCON, MSDC_IOCON_DDLSEL);
	write32(host->base + MSDC_PATCH_BIT, 0x403c0046);
	msdc_set_field(host->base + MSDC_PATCH_BIT, MSDC_CKGEN_MSDC_DLY_SEL, 1);
	write32(host->base + MSDC_PATCH_BIT1, 0xffff4089);
	setbits32(host->base + EMMC50_CFG0, EMMC50_CFG_CFCSTS_SEL);

	msdc_set_field(host->base + MSDC_PATCH_BIT1,
		       MSDC_PATCH_BIT1_STOP_DLY, 3);
	clrbits32(host->base + SDC_FIFO_CFG, SDC_FIFO_CFG_WRVALIDSEL);
	clrbits32(host->base + SDC_FIFO_CFG, SDC_FIFO_CFG_RDVALIDSEL);

	clrbits32(host->base + MSDC_PATCH_BIT1, (1 << 7));

	msdc_set_field(host->base + MSDC_PATCH_BIT2, MSDC_PB2_RESPWAIT, 3);
	if (host->top_base)
		setbits32(host->top_base + EMMC_TOP_CONTROL, SDC_RX_ENH_EN);
	else
		setbits32(host->base + SDC_ADV_CFG0, SDC_RX_ENHANCE_EN);
	/* Use async fifo, then no need to tune internal delay */
	clrbits32(host->base + MSDC_PATCH_BIT2, MSDC_PATCH_BIT2_CFGRESP);
	setbits32(host->base + MSDC_PATCH_BIT2, MSDC_PATCH_BIT2_CFGCRCSTS);

	if (host->top_base) {
		setbits32(host->top_base + EMMC_TOP_CONTROL,
			  PAD_DAT_RD_RXDLY_SEL);
		clrbits32(host->top_base + EMMC_TOP_CONTROL, DATA_K_VALUE_SEL);
		setbits32(host->top_base + EMMC_TOP_CMD, PAD_CMD_RD_RXDLY_SEL);
	} else {
		setbits32(host->base + MSDC_PAD_TUNE,
			  MSDC_PAD_TUNE_RD_SEL | MSDC_PAD_TUNE_CMD_SEL);
	}

	/* Configure to enable SDIO mode. Otherwise, sdio cmd5 will fail. */
	setbits32(host->base + SDC_CFG, SDC_CFG_SDIO);

	/* Config SDIO device detect interrupt function */
	clrbits32(host->base + SDC_CFG, SDC_CFG_SDIOIDE);
	setbits32(host->base + SDC_ADV_CFG0, SDC_DAT1_IRQ_TRIGGER);

	/* Configure to default data timeout */
	msdc_set_field(host->base + SDC_CFG, SDC_CFG_DTOC, 3);

	msdc_debug("init hardware done!\n");
}

static void msdc_fifo_clr(struct msdc_ctrlr *host)
{
	setbits32(host->base + MSDC_FIFOCS, MSDC_FIFOCS_CLR);

	if (msdc_poll_timeout(host->base + MSDC_FIFOCS, MSDC_FIFOCS_CLR) != MSDC_SUCCESS)
		msdc_error("Clear FIFO timeout!\n");
}

static u32 msdc_cmd_find_resp(struct msdc_ctrlr *host, struct mmc_command *cmd)
{
	switch (cmd->resp_type) {
	case CARD_RSP_R1:
		return 0x1;
	case CARD_RSP_R1b:
		return 0x7;
	case CARD_RSP_R2:
		return 0x2;
	case CARD_RSP_R3:
		return 0x3;
	case CARD_RSP_NONE:
	default:
		return 0x0;
	}
}

static bool msdc_cmd_is_ready(struct msdc_ctrlr *host)
{
	int ret;

	ret = msdc_poll_timeout(host->base + SDC_STS, SDC_STS_CMDBUSY);
	if (ret != MSDC_SUCCESS) {
		msdc_error("CMD bus busy detected, SDC_STS: %#x\n",
			   read32(host->base + SDC_STS));
		msdc_reset_hw(host);
		return false;
	}

	ret = msdc_poll_timeout(host->base + SDC_STS, SDC_STS_SDCBUSY);
	if (ret != MSDC_SUCCESS) {
		msdc_error("SD controller busy detected, SDC_STS: %#x\n",
			   read32(host->base + SDC_STS));
		msdc_reset_hw(host);
		return false;
	}

	return true;
}

static u32 msdc_cmd_prepare_raw_cmd(struct msdc_ctrlr *host,
				    struct mmc_command *cmd,
				    struct mmc_data *data)
{
	u32 opcode = cmd->cmdidx;
	u32 resp_type = msdc_cmd_find_resp(host, cmd);
	u32 blocksize = 0;
	u32 dtype = 0;
	u32 rawcmd = 0;

	switch (opcode) {
	case MMC_CMD_WRITE_MULTIPLE_BLOCK:
	case MMC_CMD_READ_MULTIPLE_BLOCK:
		dtype = 2;
		break;
	case MMC_CMD_WRITE_SINGLE_BLOCK:
	case MMC_CMD_READ_SINGLE_BLOCK:
	case MMC_CMD_AUTO_TUNING_SEQUENCE:
		dtype = 1;
		break;
	case MMC_CMD_SEND_STATUS:
		if (data)
			dtype = 1;
	}

	if (data) {
		if (data->flags == DATA_FLAG_READ)
			rawcmd |= SDC_CMD_WR;

		if (data->blocks > 1)
			dtype = 2;

		blocksize = data->blocksize;
	}

	rawcmd |= (opcode << SDC_CMD_CMD_S) & SDC_CMD_CMD_M;
	rawcmd |= (resp_type << SDC_CMD_RSPTYP_S) & SDC_CMD_RSPTYP_M;
	rawcmd |= (blocksize << SDC_CMD_BLK_LEN_S) & SDC_CMD_BLK_LEN_M;
	rawcmd |= (dtype << SDC_CMD_DTYPE_S) & SDC_CMD_DTYPE_M;

	if (opcode == MMC_CMD_STOP_TRANSMISSION)
		rawcmd |= SDC_CMD_STOP;

	return rawcmd;
}

static int msdc_cmd_done(struct msdc_ctrlr *host, int events,
			 struct mmc_command *cmd)
{
	u32 *rsp = cmd->response;
	int ret = 0;

	if (cmd->resp_type & CARD_RSP_PRESENT) {
		if (cmd->resp_type & CARD_RSP_136) {
			rsp[0] = read32(host->base + SDC_RESP3);
			rsp[1] = read32(host->base + SDC_RESP2);
			rsp[2] = read32(host->base + SDC_RESP1);
			rsp[3] = read32(host->base + SDC_RESP0);
		} else {
			rsp[0] = read32(host->base + SDC_RESP0);
		}
	}

	if (!(events & MSDC_INT_CMDRDY)) {
		if (cmd->cmdidx != MMC_CMD_AUTO_TUNING_SEQUENCE) {
			/*
			 * should not clear fifo/interrupt as the tune data
			 * may have already come.
			 */
			msdc_reset_hw(host);
		}
		if (events & MSDC_INT_CMDTMO)
			ret = -ETIMEDOUT;
		else
			ret = -EIO;
	}

	return ret;
}

static int msdc_start_command(struct msdc_ctrlr *host, struct mmc_command *cmd,
			      struct mmc_data *data)
{
	u32 rawcmd, status;
	u32 blocks = 0;
	int ret;

	if (!msdc_cmd_is_ready(host))
		return -EIO;

	if (read32(host->base + MSDC_FIFOCS) &
	    (MSDC_FIFOCS_TXCNT | MSDC_FIFOCS_RXCNT)) {
		msdc_error("TX/RX FIFO non-empty before start of IO. Reset\n");
		msdc_reset_hw(host);
	}

	msdc_fifo_clr(host);

	rawcmd = msdc_cmd_prepare_raw_cmd(host, cmd, data);

	if (data)
		blocks = data->blocks;

	write32(host->base + MSDC_INT, CMD_INTS_MASK);
	write32(host->base + SDC_BLK_NUM, blocks);
	write32(host->base + SDC_ARG, cmd->cmdarg);
	write32(host->base + SDC_CMD, rawcmd);

	ret = msdc_wait_done(host->base + MSDC_INT, CMD_INTS_MASK, &status);
	if (ret != MSDC_SUCCESS)
		status = MSDC_INT_CMDTMO;

	return msdc_cmd_done(host, status, cmd);
}

static int msdc_send_command(struct sd_mmc_ctrlr *ctrlr,
	struct mmc_command *cmd, struct mmc_data *data)
{
	struct msdc_ctrlr *host;

	host = container_of(ctrlr, struct msdc_ctrlr, sd_mmc_ctrlr);

	return msdc_start_command(host, cmd, data);
}

static void msdc_set_clock(struct msdc_ctrlr *host, u32 clock)
{
	u32 mode, mode_shift;
	u32 div, div_mask;
	const u32 div_width = 12;
	u32 sclk;
	u32 hclk = host->src_hz;
	struct sd_mmc_ctrlr *ctrlr = &host->sd_mmc_ctrlr;

	if (clock >= hclk) {
		mode = 0x1;     /* no divisor */
		div = 0;
		sclk = hclk;
	} else {
		mode = 0x0;     /* use divisor */
		if (clock >= (hclk / 2)) {
			div = 0;        /* mean div = 1/2 */
			sclk = hclk / 2;        /* sclk = clk / 2 */
		} else {
			div = DIV_ROUND_UP(hclk, clock * 4);
			sclk = (hclk >> 2) / div;
		}
	}

	div_mask = (1 << div_width) - 1;
	mode_shift = 8 + div_width;
	assert(div <= div_mask);

	clrsetbits_le32(host->base + MSDC_CFG, (0x3 << mode_shift) | (div_mask << 8),
			(mode << mode_shift) | (div << 8));
	if (msdc_wait_done(host->base + MSDC_CFG, MSDC_CFG_CKSTB, NULL) != MSDC_SUCCESS)
		msdc_error("Failed while wait clock stable!\n");

	ctrlr->bus_hz = sclk;
	msdc_debug("sclk: %d\n", sclk);
}

static void msdc_set_buswidth(struct msdc_ctrlr *host, u32 width)
{
	u32 val = read32(host->base + SDC_CFG);

	val &= ~SDC_CFG_BUSWIDTH;

	switch (width) {
	default:
	case 1:
		val |= (MSDC_BUS_1BITS << 16);
		break;
	case 4:
		val |= (MSDC_BUS_4BITS << 16);
		break;
	case 8:
		val |= (MSDC_BUS_8BITS << 16);
		break;
	}

	write32(host->base + SDC_CFG, val);
	msdc_trace("Bus Width = %d\n", width);
}

static void msdc_set_ios(struct sd_mmc_ctrlr *ctrlr)
{
	struct msdc_ctrlr *host;

	host = container_of(ctrlr, struct msdc_ctrlr, sd_mmc_ctrlr);

	/* Set the clock frequency */
	if (ctrlr->bus_hz != ctrlr->request_hz)
		msdc_set_clock(host, ctrlr->request_hz);

	msdc_set_buswidth(host, ctrlr->bus_width);

}

static void msdc_update_pointers(struct msdc_ctrlr *host)
{
	struct sd_mmc_ctrlr *ctrlr = &host->sd_mmc_ctrlr;

	/* Update the routine pointers */
	ctrlr->send_cmd = &msdc_send_command;
	ctrlr->set_ios = &msdc_set_ios;

	ctrlr->f_min = 400 * 1000;
	ctrlr->f_max = 52 * 1000 * 1000;
	ctrlr->bus_width = 1;
	ctrlr->caps |= DRVR_CAP_HS | DRVR_CAP_HC;
	ctrlr->voltages = 0x40ff8080;
}

static void add_msdc(struct msdc_ctrlr *host)
{
	struct sd_mmc_ctrlr *ctrlr = &host->sd_mmc_ctrlr;

	msdc_update_pointers(host);

	/* Initialize the MTK MSDC controller */
	msdc_init_hw(host);

	/* Display the results */
	msdc_trace("%#010x: ctrlr->caps\n", ctrlr->caps);
	msdc_trace("%d.%03d MHz: ctrlr->f_max\n",
		    ctrlr->f_max / 1000000,
		    (ctrlr->f_max / 1000) % 1000);
	msdc_trace("%d.%03d MHz: ctrlr->f_min\n",
		    ctrlr->f_min / 1000000,
		    (ctrlr->f_min / 1000) % 1000);
	msdc_trace("%#010x: ctrlr->voltages\n", ctrlr->voltages);
}

static void msdc_controller_init(struct msdc_ctrlr *host, void *base, void *top_base)
{
	memset(host, 0, sizeof(*host));
	host->base = base;
	host->top_base = top_base;
	host->src_hz = 50 * 1000 * 1000;

	add_msdc(host);
}

static void set_early_mmc_wake_status(int32_t status)
{
	int32_t *ms_cbmem;

	ms_cbmem = cbmem_add(CBMEM_ID_MMC_STATUS, sizeof(status));

	if (ms_cbmem == NULL) {
		printk(BIOS_ERR,
		       "%s: Failed to add early mmc wake status to cbmem!\n",
		       __func__);
		return;
	}

	printk(BIOS_DEBUG, "Early init status = %d\n", status);
	*ms_cbmem = status;
}

int mtk_emmc_early_init(void *base, void *top_base)
{
	struct storage_media media = { 0 };
	int err;
	struct msdc_ctrlr msdc_host;
	struct sd_mmc_ctrlr *mmc_ctrlr = &msdc_host.sd_mmc_ctrlr;

	/* Init mtk mmc ctrlr */
	msdc_controller_init(&msdc_host, base, top_base);

	media.ctrlr = mmc_ctrlr;
	SET_CLOCK(mmc_ctrlr, 400 * 1000);
	SET_BUS_WIDTH(mmc_ctrlr, 1);

	/* Reset emmc, send CMD0 */
	if (sd_mmc_go_idle(&media))
		goto out_err;

	/* Send CMD1 */
	err = mmc_send_op_cond(&media);
	if (err == 0)
		set_early_mmc_wake_status(MMC_STATUS_CMD1_READY);
	else if (err == CARD_IN_PROGRESS)
		set_early_mmc_wake_status(MMC_STATUS_CMD1_IN_PROGRESS);
	else
		goto out_err;

	return 0;

out_err:
	set_early_mmc_wake_status(MMC_STATUS_NEED_RESET);
	return -1;
}
