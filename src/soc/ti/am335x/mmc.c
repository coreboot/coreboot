/* SPDX-License-Identifier: GPL-2.0-only */

#include <string.h>
#include <console/console.h>
#include <commonlib/sd_mmc_ctrlr.h>
#include <device/mmio.h>
#include <timer.h>
#include "mmc.h"

#define AM335X_TIMEOUT_MSEC 1000

#define SYSCONFIG_SOFTRESET (0x1 << 1)

#define SYSSTATUS_RESETDONE (0x01 << 0)

#define CON_INIT (0x1 << 1)

#define CMD_INDEX(x) (x << 24)
#define CMD_RSP_TYPE_NO_RESP (0x0 << 16)
#define CMD_RSP_TYPE_136B (0x1 << 16)
#define CMD_RSP_TYPE_48B (0x2 << 16)
#define CMD_RSP_TYPE_48B_BUSY (0x3 << 16)
#define CMD_DP_DATA (0x1 << 21)
#define CMD_DDIR_READ (0x1 << 4)

#define HCTL_DTW_1BIT (0x0 << 1)
#define HCTL_SDBP (0x1 << 8)
#define HCTL_SDVS_VS30 (0x6 << 9)

#define SYSCTL_ICE (0x1 << 0)
#define SYSCTL_ICS (0x1 << 1)
#define SYSCTL_CEN (0x1 << 2)
#define SYSCTL_DTO_15 (0xE << 16)

#define STAT_ERRI (0x01 << 15)
#define STAT_ERROR_MASK (0xff << 15 | 0x3 << 24 | 0x03 << 28)
#define STAT_CC (0x1 << 0)

#define IE_CC (0x1 << 0)
#define IE_TC (0x1 << 1)
#define IE_BRR (0x1 << 5)
#define IE_ERRORS (0xff << 15 | 0x3 << 24 | 0x03 << 28)

#define CAPA_VS18 (0x01 << 26)
#define CAPA_VS30 (0x01 << 25)

static int am335x_wait_for_reg(const void *addr, uint32_t mask, unsigned long timeout)
{
	struct mono_time current, end;

	timer_monotonic_get(&current);
	end = current;
	mono_time_add_msecs(&end, timeout);

	do {
		if ((read32(addr) & mask))
			return 0;

		timer_monotonic_get(&current);
	} while (!mono_time_after(&current, &end));

	printk(BIOS_DEBUG, "am335x MMC timeout: %ld msec\n", timeout);
	return -1;
}

static int am335x_mmc_init(struct am335x_mmc *mmc)
{
	// Follows the initialisiation from the AM335X technical reference manual
	setbits32(&mmc->sysconfig, SYSCONFIG_SOFTRESET);

	if (am335x_wait_for_reg(&mmc->sysstatus, SYSSTATUS_RESETDONE, AM335X_TIMEOUT_MSEC))
		return -1;

	setbits32(&mmc->capa, CAPA_VS30);
	setbits32(&mmc->hctl, HCTL_SDVS_VS30 | HCTL_DTW_1BIT);
	setbits32(&mmc->hctl, HCTL_SDBP);

	if (am335x_wait_for_reg(&mmc->hctl, HCTL_SDBP, AM335X_TIMEOUT_MSEC))
		return -1;

	// Assumes the default input clock speed of 96MHz to set a minimum SD
	// speed of 400 KHz
	write32(&mmc->sysctl, read32(&mmc->sysctl) | 240 << 6 | SYSCTL_DTO_15);

	setbits32(&mmc->sysctl, SYSCTL_ICE | SYSCTL_CEN);

	if (am335x_wait_for_reg(&mmc->sysctl, SYSCTL_ICS, AM335X_TIMEOUT_MSEC))
		return -1;

	write32(&mmc->ie, IE_ERRORS | IE_TC | IE_CC);

	// Clear interrupts
	write32(&mmc->stat, 0xffffffffu);

	setbits32(&mmc->con, CON_INIT);
	write32(&mmc->cmd, 0x00);

	if (am335x_wait_for_reg(&mmc->stat, STAT_CC, AM335X_TIMEOUT_MSEC))
		return -1;

	write32(&mmc->stat, 0xffffffffu);
	clrbits32(&mmc->con, CON_INIT);

	return 0;
}

static int am335x_send_cmd(struct sd_mmc_ctrlr *ctrlr, struct mmc_command *cmd,
			   struct mmc_data *data)
{
	struct am335x_mmc_host *mmc;
	struct am335x_mmc *reg;

	mmc = container_of(ctrlr, struct am335x_mmc_host, sd_mmc_ctrlr);
	reg = mmc->reg;

	if (read32(&reg->stat)) {
		printk(BIOS_WARNING, "AM335X MMC: Interrupt already raised\n");
		return 1;
	}

	uint32_t transfer_type = 0;

	if (data) {
		if (data->flags & DATA_FLAG_READ) {
			setbits32(&mmc->reg->ie, IE_BRR);
			write32(&mmc->reg->blk, data->blocksize);
			transfer_type |= CMD_DP_DATA | CMD_DDIR_READ;
		}

		if (data->flags & DATA_FLAG_WRITE) {
			printk(BIOS_ERR, "AM335X MMC: Writes currently not supported\n");
			return 1;
		}
	}

	switch (cmd->resp_type) {
	case CARD_RSP_R1b:
		transfer_type |= CMD_RSP_TYPE_48B_BUSY;
		break;
	case CARD_RSP_R1:
	case CARD_RSP_R3:
		transfer_type |= CMD_RSP_TYPE_48B;
		break;
	case CARD_RSP_R2:
		transfer_type |= CMD_RSP_TYPE_136B;
		break;
	case CARD_RSP_NONE:
		transfer_type |= CMD_RSP_TYPE_NO_RESP;
		break;
	default:
		printk(BIOS_ERR, "AM335X MMC: Unknown response type\n");
		return 1;
	}

	if (cmd->cmdidx == MMC_CMD_SET_BLOCKLEN) {
		// todo: Support bigger blocks for faster transfers
		return 0;
	}

	write32(&reg->arg, cmd->cmdarg);
	write32(&reg->cmd, CMD_INDEX(cmd->cmdidx) | transfer_type);

	// Wait for any interrupt
	if (am335x_wait_for_reg(&reg->stat, 0xffffffff, AM335X_TIMEOUT_MSEC))
		return -1;

	// Check to ensure that there was not any errors
	if (read32(&reg->stat) & STAT_ERRI) {
		printk(BIOS_WARNING, "AM335X MMC: Error while reading %08x\n",
		       read32(&reg->stat));

		// Clear the errors
		write32(&reg->stat, STAT_ERROR_MASK);
		return 1;
	}

	if (cmd->resp_type == CARD_RSP_R1b) {
		if (am335x_wait_for_reg(&reg->stat, IE_TC, AM335X_TIMEOUT_MSEC))
			return -1;

		write32(&reg->stat, IE_TC);
	}

	write32(&reg->stat, IE_CC);

	switch (cmd->resp_type) {
	case CARD_RSP_R1:
	case CARD_RSP_R1b:
	case CARD_RSP_R3:
		cmd->response[0] = read32(&reg->rsp10);
		break;
	case CARD_RSP_R2:
		cmd->response[3] = read32(&reg->rsp10);
		cmd->response[2] = read32(&reg->rsp32);
		cmd->response[1] = read32(&reg->rsp54);
		cmd->response[0] = read32(&reg->rsp76);
		break;
	case CARD_RSP_NONE:
		break;
	}

	if (data != NULL && data->flags & DATA_FLAG_READ) {
		if (am335x_wait_for_reg(&reg->stat, IE_BRR, AM335X_TIMEOUT_MSEC))
			return -1;

		uint32_t *dest32 = (uint32_t *)data->dest;

		for (int count = 0; count < data->blocksize; count += 4) {
			*dest32 = read32(&reg->data);
			dest32++;
		}

		write32(&reg->stat, IE_TC);
		write32(&reg->stat, IE_BRR);
		clrbits32(&reg->ie, IE_BRR);
	}

	return 0;
}

static void set_ios(struct sd_mmc_ctrlr *ctrlr)
{
	struct am335x_mmc_host *mmc;
	struct am335x_mmc *reg;

	mmc = container_of(ctrlr, struct am335x_mmc_host, sd_mmc_ctrlr);
	reg = mmc->reg;

	if (ctrlr->request_hz != ctrlr->bus_hz) {
		uint32_t requested_hz = ctrlr->request_hz;

		requested_hz = MIN(requested_hz, ctrlr->f_min);
		requested_hz = MAX(requested_hz, ctrlr->f_max);

		uint32_t divisor = mmc->sd_clock_hz / requested_hz;
		uint32_t actual = mmc->sd_clock_hz * divisor;

		if (actual != ctrlr->bus_hz) {
			clrbits32(&reg->sysctl, SYSCTL_CEN);

			uint32_t new_sysctl = read32(&reg->sysctl);
			new_sysctl &= ~(0x3ff << 6);
			new_sysctl |= ((divisor & 0x3ff) << 6);

			write32(&reg->sysctl, new_sysctl);

			// Wait for clock stability
			am335x_wait_for_reg(&reg->sysctl, SYSCTL_ICS, AM335X_TIMEOUT_MSEC);

			setbits32(&reg->sysctl, SYSCTL_CEN);

			ctrlr->bus_hz = mmc->sd_clock_hz / divisor;
		}
	}
}

int am335x_mmc_init_storage(struct am335x_mmc_host *mmc_host)
{
	int err = 0;

	struct sd_mmc_ctrlr *mmc_ctrlr = &mmc_host->sd_mmc_ctrlr;
	memset(mmc_ctrlr, 0, sizeof(*mmc_ctrlr));


	err = am335x_mmc_init(mmc_host->reg);
	if (err != 0) {
		printk(BIOS_ERR, "Initialising AM335X SD failed.\n");
		return err;
	}

	mmc_ctrlr->send_cmd = &am335x_send_cmd;
	mmc_ctrlr->set_ios = &set_ios;

	mmc_ctrlr->voltages = MMC_VDD_30_31;
	mmc_ctrlr->b_max = 1;
	mmc_ctrlr->bus_width = 1;
	mmc_ctrlr->f_max = 48000000;
	mmc_ctrlr->f_min = 400000;
	mmc_ctrlr->bus_hz = 400000;

	return 0;
}
