/*
 * MMC and SD card access code
 *
 * Based on sunxi/uboot code, which in turn is based (loosely) on the Linux code
 *
 * Copyright (C) 2008,2010 Freescale Semiconductor, Inc
 *            written by Andy Fleming
 * Copyright (C) 2013  Alexandru Gagniuc <mr.nuke.me@gmail.com>
 * Subject to the GNU GPL v2, or (at your option) any later version.
 */

#ifndef _MMC_PRIVATE_H_
#define _MMC_PRIVATE_H_

#include <drivers/mmc/mmc.h>

#include <arch/byteorder.h>
#include <stdlib.h>
#include <console/console.h>
#include <delay.h>

#define ERROR(x...) printk(BIOS_ERR, "[mmc] " x)
#define LOG(x...) printk(BIOS_INFO, "[mmc] " x)
#if CONFIG_DEBUG_MMC
#define DEBUG(x...) printk(BIOS_DEBUG, "[mmc] " x)
#define SPEW(x...) printk(BIOS_SPEW, "[mmc] " x)
#else
#define DEBUG(x...)
#define SPEW(x...)
#endif

/* FIXME: Global MMC is a bad idea */
static struct mmc *g_mmc = 0;

extern enum cb_err mmc_send_cmd(struct mmc *mmc, struct mmc_cmd *cmd,
				struct mmc_data *data);
extern enum cb_err mmc_send_status(struct mmc *mmc, int timeout);
extern enum cb_err mmc_set_blocklen(struct mmc *mmc, int len);

#endif				/* _MMC_PRIVATE_H_ */

/* Set block count limit because of 16 bit register limit on some hardware*/
#ifndef CONFIG_SYS_MMC_MAX_BLK_COUNT
#define CONFIG_SYS_MMC_MAX_BLK_COUNT 65535
#endif

////static struct list_head mmc_devices;
static int cur_dev_num = -1;

/* FIXME: __weak */
static int board_mmc_getwp(struct mmc *mmc)
{
	return -1;
}

int mmc_getwp(struct mmc *mmc)
{
	int wp;

	wp = board_mmc_getwp(mmc);

	if (wp < 0) {
		if (mmc->getwp)
			wp = mmc->getwp(mmc);
		else
			wp = 0;
	}

	return wp;
}

static int __board_mmc_getcd(struct mmc *mmc)
{
	return -1;
}

enum cb_err board_mmc_getcd(struct mmc *mmc) __attribute__ ((weak,
							     alias
							     ("__board_mmc_getcd")));

enum cb_err mmc_send_cmd(struct mmc *mmc, struct mmc_cmd *cmd,
			 struct mmc_data *data)
{
	enum cb_err ret;

	DEBUG("Send command: %i\targ: 0x%08x\n", cmd->cmdidx, cmd->cmdarg);
	ret = mmc->send_cmd(mmc, cmd, data);

#ifdef CONFIG_DEBUG_MMC
	int i;
	u8 *ptr;


	switch (cmd->resp_type) {
	case MMC_RSP_NONE:
		SPEW("\tMMC_RSP_NONE\n");
		break;
	case MMC_RSP_R1:
		SPEW("\tMMC_RSP_R1,5,6,7 \t 0x%08x\n", cmd->response[0]);
		break;
	case MMC_RSP_R1b:
		SPEW("\tMMC_RSP_R1b\t 0x%08x\n", cmd->response[0]);
		break;
	case MMC_RSP_R2:
		SPEW("\tMMC_RSP_R2\t0x%08x\n", cmd->response[0]);
		SPEW("\t          \t0x%08x\n", cmd->response[1]);
		SPEW("\t          \t0x%08x\n", cmd->response[2]);
		SPEW("\t          \t0x%08x\n", cmd->response[3]);
		SPEW("\t\tDUMPING DATA\n");
		for (i = 0; i < 4; i++) {
			ptr = (u8 *) & cmd->response[i];
			SPEW("\t\t%03d - %02x %02x %02x %02x\n", i * 4,
				ptr[3], ptr[2], ptr[1], ptr[0]);
		}
		break;
	case MMC_RSP_R3:
		SPEW("\tMMC_RSP_R3,4\t0x%08X \n", cmd->response[0]);
		break;
	default:
		SPEW("\tERROR MMC rsp not supported\n");
		break;
	}
#endif
	return ret;
}

enum cb_err mmc_send_status(struct mmc *mmc, int timeout)
{
	struct mmc_cmd cmd;
	enum cb_err err;
	int retries = 5;

	cmd.cmdidx = MMC_CMD_SEND_STATUS;
	cmd.resp_type = MMC_RSP_R1;
	if (!mmc_host_is_spi(mmc))
		cmd.cmdarg = mmc->rca << 16;

	do {
		err = mmc_send_cmd(mmc, &cmd, NULL);
		if (!err) {
			if ((cmd.response[0] & MMC_STATUS_RDY_FOR_DATA) &&
			    (cmd.response[0] & MMC_STATUS_CURR_STATE) !=
			    MMC_STATE_PRG)
				break;
			else if (cmd.response[0] & MMC_STATUS_MASK) {
				LOG("Status Error: 0x%08X\n", cmd.response[0]);
				return CB_MMC_COMM_ERROR;
			}
		} else if (--retries < 0)
			return err;

		udelay(1000);

	} while (timeout--);

	DEBUG("CURR STATE:%d\n", (cmd.response[0] & MMC_STATUS_CURR_STATE) >> 9);

	if (timeout <= 0) {
		LOG("Timeout waiting card ready\n");
		return CB_MMC_TIMEOUT;
	}

	return CB_SUCCESS;
}

enum cb_err mmc_set_blocklen(struct mmc *mmc, int len)
{
	struct mmc_cmd cmd;

	cmd.cmdidx = MMC_CMD_SET_BLOCKLEN;
	cmd.resp_type = MMC_RSP_R1;
	cmd.cmdarg = len;

	return mmc_send_cmd(mmc, &cmd, NULL);
}

struct mmc *find_mmc_device(int dev_num)
{
	/* FIXME */
	return g_mmc;


	DEBUG("MMC Device %d not found\n", dev_num);

	return NULL;
}

static enum cb_err mmc_read_blocks(struct mmc *mmc, void *dst, lbaint_t start,
				   lbaint_t blkcnt)
{
	struct mmc_cmd cmd;
	struct mmc_data data;

	if (blkcnt > 1)
		cmd.cmdidx = MMC_CMD_READ_MULTIPLE_BLOCK;
	else
		cmd.cmdidx = MMC_CMD_READ_SINGLE_BLOCK;

	if (mmc->high_capacity)
		cmd.cmdarg = start;
	else
		cmd.cmdarg = start * mmc->read_bl_len;

	cmd.resp_type = MMC_RSP_R1;

	data.dest = dst;
	data.blocks = blkcnt;
	data.blocksize = mmc->read_bl_len;
	data.flags = MMC_DATA_READ;

	if (mmc_send_cmd(mmc, &cmd, &data))
		return 0;

	if (blkcnt > 1) {
		cmd.cmdidx = MMC_CMD_STOP_TRANSMISSION;
		cmd.cmdarg = 0;
		cmd.resp_type = MMC_RSP_R1b;
		if (mmc_send_cmd(mmc, &cmd, NULL)) {
			LOG("mmc fail to send stop cmd\n");
			return 0;
		}
	}

	return blkcnt;
}

static u32 mmc_bread(int dev_num, lbaint_t start, lbaint_t blkcnt, void *dst)
{
	lbaint_t cur, blocks_todo = blkcnt;

	print_debug("MMC bread, no butter\n");
	if (blkcnt == 0)
		return CB_SUCCESS;

	struct mmc *mmc = find_mmc_device(dev_num);
	if (!mmc)
		return 0;

	if ((start + blkcnt) > mmc->block_dev.lba) {
		return 0;
	}

	if (mmc_set_blocklen(mmc, mmc->read_bl_len))
		return 0;

	DEBUG("MMC blocklen %x\n", mmc->read_bl_len);

	do {
		cur = (blocks_todo > mmc->b_max) ? mmc->b_max : blocks_todo;
		if (mmc_read_blocks(mmc, dst, start, cur) != cur)
			return 0;
		blocks_todo -= cur;
		start += cur;
		dst += cur * mmc->read_bl_len;
	} while (blocks_todo > 0);

	return blkcnt;
}

static enum cb_err mmc_go_idle(struct mmc *mmc)
{
	struct mmc_cmd cmd;
	enum cb_err err;

	udelay(1000);

	cmd.cmdidx = MMC_CMD_GO_IDLE_STATE;
	cmd.cmdarg = 0;
	cmd.resp_type = MMC_RSP_NONE;

	err = mmc_send_cmd(mmc, &cmd, NULL);

	if (err != CB_SUCCESS)
		return err;

	udelay(2000);

	return 0;
}

static enum cb_err mmc_app_cmd(struct mmc *mmc, u32 arg)
{
	struct mmc_cmd cmd;
	cmd.cmdidx = MMC_CMD_APP_CMD;
	cmd.resp_type = MMC_RSP_R1;
	cmd.cmdarg = arg;

	return mmc_send_cmd(mmc, &cmd, NULL);
}

static int sd_send_op_cond(struct mmc *mmc)
{
	int timeout = 1000;
	enum cb_err err;
	struct mmc_cmd cmd;

	do {

		if ((err = mmc_app_cmd(mmc, 0)) != CB_SUCCESS)
			return err;

		cmd.cmdidx = SD_CMD_APP_SEND_OP_COND;
		cmd.resp_type = MMC_RSP_R3;

		/*
		 * Most cards do not answer if some reserved bits
		 * in the ocr are set. However, Some controller
		 * can set bit 7 (reserved for low voltages), but
		 * how to manage low voltages SD card is not yet
		 * specified.
		 */
		cmd.cmdarg = mmc_host_is_spi(mmc) ? 0 :
		    (mmc->voltages & 0xff8000);
		cmd.cmdarg |= (mmc->version == SD_VERSION_2) ? OCR_HCS : 0;

		if ((err = mmc_send_cmd(mmc, &cmd, NULL)) != CB_SUCCESS)
			return err;

		udelay(1000);
	} while ((!(cmd.response[0] & OCR_BUSY)) && timeout--);

	if (timeout <= 0)
		return CB_MMC_UNUSABLE;

	if (mmc->version != SD_VERSION_2)
		mmc->version = SD_VERSION_1_0;

	if (mmc_host_is_spi(mmc)) {	/* read OCR for spi */
		cmd.cmdidx = MMC_CMD_SPI_READ_OCR;
		cmd.resp_type = MMC_RSP_R3;
		cmd.cmdarg = 0;

		err = mmc_send_cmd(mmc, &cmd, NULL);

		if (err != CB_SUCCESS)
			return err;
	}

	mmc->ocr = cmd.response[0];

	mmc->high_capacity = ((mmc->ocr & OCR_HCS) == OCR_HCS);
	mmc->rca = 0;

	return 0;
}

/* We pass in the cmd since otherwise the init seems to fail */
static enum cb_err mmc_send_op_cond_iter(struct mmc *mmc, struct mmc_cmd *cmd,
					 int use_arg)
{
	enum cb_err err;

	cmd->cmdidx = MMC_CMD_SEND_OP_COND;
	cmd->resp_type = MMC_RSP_R3;
	cmd->cmdarg = 0;
	if (use_arg && !mmc_host_is_spi(mmc)) {
		cmd->cmdarg =
		    (mmc->voltages &
		     (mmc->op_cond_response & OCR_VOLTAGE_MASK)) |
		    (mmc->op_cond_response & OCR_ACCESS_MODE);

		if (mmc->host_caps & MMC_MODE_HC)
			cmd->cmdarg |= OCR_HCS;
	}
	err = mmc_send_cmd(mmc, cmd, NULL);
	if (err != CB_SUCCESS)
		return err;
	mmc->op_cond_response = cmd->response[0];
	return 0;
}

static enum cb_err mmc_send_op_cond(struct mmc *mmc)
{
	struct mmc_cmd cmd;
	enum cb_err err;
	int i;

	/* Some cards seem to need this */
	mmc_go_idle(mmc);

	/* Asking to the card its capabilities */
	mmc->op_cond_pending = 1;
	for (i = 0; i < 2; i++) {
		err = mmc_send_op_cond_iter(mmc, &cmd, i != 0);
		if (err != CB_SUCCESS)
			return err;

		/* exit if not busy (flag seems to be inverted) */
		if (mmc->op_cond_response & OCR_BUSY)
			return CB_SUCCESS;
	}
	return CB_MMC_IN_PROGRESS;
}

static enum cb_err mmc_complete_op_cond(struct mmc *mmc)
{
	struct mmc_cmd cmd;
	int timeout = 1000;
	u32 start = 0;
	enum cb_err err;

	mmc->op_cond_pending = 0;
	/* FIXME *///start = get_timer(0);
	do {
		err = mmc_send_op_cond_iter(mmc, &cmd, 1);
		if (err != CB_SUCCESS)
			return err;
		if ( /* FIXME *//*get_timer(start) */ start > timeout)
			return CB_MMC_UNUSABLE;
		udelay(100);
	} while (!(mmc->op_cond_response & OCR_BUSY));

	if (mmc_host_is_spi(mmc)) {	/* read OCR for spi */
		cmd.cmdidx = MMC_CMD_SPI_READ_OCR;
		cmd.resp_type = MMC_RSP_R3;
		cmd.cmdarg = 0;

		err = mmc_send_cmd(mmc, &cmd, NULL);

		if (err != CB_SUCCESS)
			return err;
	}

	mmc->version = MMC_VERSION_UNKNOWN;
	mmc->ocr = cmd.response[0];

	mmc->high_capacity = ((mmc->ocr & OCR_HCS) == OCR_HCS);
	mmc->rca = 0;

	return CB_SUCCESS;
}

static int mmc_send_ext_csd(struct mmc *mmc, u8 *ext_csd)
{
	struct mmc_cmd cmd;
	struct mmc_data data;

	/* Get the Card Status Register */
	cmd.cmdidx = MMC_CMD_SEND_EXT_CSD;
	cmd.resp_type = MMC_RSP_R1;
	cmd.cmdarg = 0;

	data.dest = ext_csd;
	data.blocks = 1;
	data.blocksize = MMC_MAX_BLOCK_LEN;
	data.flags = MMC_DATA_READ;

	return mmc_send_cmd(mmc, &cmd, &data);
}

static int mmc_switch(struct mmc *mmc, u8 set, u8 index, u8 value)
{
	struct mmc_cmd cmd;
	int timeout = 1000;
	int ret;

	cmd.cmdidx = MMC_CMD_SWITCH;
	cmd.resp_type = MMC_RSP_R1b;
	cmd.cmdarg = (MMC_SWITCH_MODE_WRITE_BYTE << 24) |
	    (index << 16) | (value << 8);

	ret = mmc_send_cmd(mmc, &cmd, NULL);

	/* Waiting for the ready status */
	if (!ret)
		ret = mmc_send_status(mmc, timeout);

	return ret;

}

#define MAX_BLOCK_LEN 0x100
static enum cb_err mmc_change_freq(struct mmc *mmc)
{
	//ALLOC_CACHE_ALIGN_BUFFER(u8, ext_csd, MMC_MAX_BLOCK_LEN);
	u8 ext_csd[MAX_BLOCK_LEN];

	u8 cardtype;
	enum cb_err err;

	mmc->card_caps = 0;

	if (mmc_host_is_spi(mmc))
		return 0;

	/* Only version 4 supports high-speed */
	if (mmc->version < MMC_VERSION_4)
		return 0;

	err = mmc_send_ext_csd(mmc, ext_csd);

	if (err != CB_SUCCESS)
		return err;

	cardtype = ext_csd[EXT_CSD_CARD_TYPE] & 0xf;

	err = mmc_switch(mmc, EXT_CSD_CMD_SET_NORMAL, EXT_CSD_HS_TIMING, 1);

	if (err != CB_SUCCESS)
		return err;

	/* Now check to see that it worked */
	err = mmc_send_ext_csd(mmc, ext_csd);

	if (err != CB_SUCCESS)
		return err;

	/* No high-speed support */
	if (!ext_csd[EXT_CSD_HS_TIMING])
		return CB_SUCCESS;

	/* High Speed is set, there are two types: 52MHz and 26MHz */
	if (cardtype & MMC_HS_52MHZ)
		mmc->card_caps |= MMC_MODE_HS_52MHz | MMC_MODE_HS;
	else
		mmc->card_caps |= MMC_MODE_HS;

	return CB_SUCCESS;
}

static enum cb_err mmc_set_capacity(struct mmc *mmc, int part_num)
{
	switch (part_num) {
	case 0:
		mmc->capacity = mmc->capacity_user;
		break;
	case 1:
	case 2:
		mmc->capacity = mmc->capacity_boot;
		break;
	case 3:
		mmc->capacity = mmc->capacity_rpmb;
		break;
	case 4:
	case 5:
	case 6:
	case 7:
		mmc->capacity = mmc->capacity_gp[part_num - 4];
		break;
	default:
		return CB_ERR;
	}

	//mmc->block_dev.lba = lldiv(mmc->capacity, mmc->read_bl_len);
	mmc->block_dev.lba = mmc->capacity / mmc->read_bl_len;

	return 0;
}

enum cb_err mmc_switch_part(int dev_num, unsigned int part_num)
{
	struct mmc *mmc = find_mmc_device(dev_num);
	int ret;

	if (!mmc)
		return -1;

	ret = mmc_switch(mmc, EXT_CSD_CMD_SET_NORMAL, EXT_CSD_PART_CONF,
			 (mmc->part_config & ~PART_ACCESS_MASK)
			 | (part_num & PART_ACCESS_MASK));
	if (ret)
		return ret;

	return mmc_set_capacity(mmc, part_num);
}

enum cb_err mmc_getcd(struct mmc *mmc)
{
	int cd;

	cd = board_mmc_getcd(mmc);

	if (cd < 0) {
		if (mmc->getcd)
			cd = mmc->getcd(mmc);
		else
			cd = 1;
	}

	return cd;
}

static enum cb_err sd_switch(struct mmc *mmc, int mode, int group, u8 value,
			     u8 * resp)
{
	struct mmc_cmd cmd;
	struct mmc_data data;

	/* Switch the frequency */
	cmd.cmdidx = SD_CMD_SWITCH_FUNC;
	cmd.resp_type = MMC_RSP_R1;
	cmd.cmdarg = (mode << 31) | 0xffffff;
	cmd.cmdarg &= ~(0xf << (group * 4));
	cmd.cmdarg |= value << (group * 4);

	data.dest = resp;
	data.blocksize = 64;
	data.blocks = 1;
	data.flags = MMC_DATA_READ;

	return mmc_send_cmd(mmc, &cmd, &data);
}

static enum cb_err sd_change_freq(struct mmc *mmc)
{
	enum cb_err err;
	struct mmc_cmd cmd;
	//ALLOC_CACHE_ALIGN_BUFFER(u32, scr, 2);
	u32 scr[2];
	//ALLOC_CACHE_ALIGN_BUFFER(u32, switch_status, 16);
	u32 switch_status[16];
	struct mmc_data data;
	int timeout;

	mmc->card_caps = 0;

	if (mmc_host_is_spi(mmc))
		return 0;

	/* Read the SCR to find out if this card supports higher speeds */
	if ((err = mmc_app_cmd(mmc, (mmc->rca << 16))) != CB_SUCCESS)
		return err;

	cmd.cmdidx = SD_CMD_APP_SEND_SCR;
	cmd.resp_type = MMC_RSP_R1;
	cmd.cmdarg = 0;

	timeout = 3;

 retry_scr:
	/* FIXME: Possible endianess violation (u32* -> u8*) */
	data.dest = (u8 *) scr;
	data.blocksize = 8;
	data.blocks = 1;
	data.flags = MMC_DATA_READ;

	err = mmc_send_cmd(mmc, &cmd, &data);

	if (err != CB_SUCCESS) {
		if (timeout--)
			goto retry_scr;

		return err;
	}

	mmc->scr[0] = be32_to_cpu(scr[0]);
	mmc->scr[1] = be32_to_cpu(scr[1]);

	switch ((mmc->scr[0] >> 24) & 0xf) {
	case 0:
		mmc->version = SD_VERSION_1_0;
		break;
	case 1:
		mmc->version = SD_VERSION_1_10;
		break;
	case 2:
		mmc->version = SD_VERSION_2;
		if ((mmc->scr[0] >> 15) & 0x1)
			mmc->version = SD_VERSION_3;
		break;
	default:
		mmc->version = SD_VERSION_1_0;
		break;
	}

	if (mmc->scr[0] & SD_DATA_4BIT)
		mmc->card_caps |= MMC_MODE_4BIT;

	/* Version 1.0 doesn't support switching */
	if (mmc->version == SD_VERSION_1_0)
		return 0;

	timeout = 4;
	while (timeout--) {
		err = sd_switch(mmc, SD_SWITCH_CHECK, 0, 1,
				(u8 *) switch_status);

		if (err != CB_SUCCESS)
			return err;

		/* The high-speed function is busy.  Try again */
		if (!(be32_to_cpu(switch_status[7]) & SD_HIGHSPEED_BUSY))
			break;
	}

	/* If high-speed isn't supported, we return */
	if (!(be32_to_cpu(switch_status[3]) & SD_HIGHSPEED_SUPPORTED))
		return 0;

	/*
	 * If the host doesn't support SD_HIGHSPEED, do not switch card to
	 * HIGHSPEED mode even if the card support SD_HIGHSPPED.
	 * This can avoid furthur problem when the card runs in different
	 * mode between the host.
	 */
	if (!((mmc->host_caps & MMC_MODE_HS_52MHz) &&
	      (mmc->host_caps & MMC_MODE_HS)))
		return 0;

	err = sd_switch(mmc, SD_SWITCH_SWITCH, 0, 1, (u8 *) switch_status);

	if (err != CB_SUCCESS)
		return err;

	if ((be32_to_cpu(switch_status[4]) & 0x0f000000) == 0x01000000)
		mmc->card_caps |= MMC_MODE_HS;

	return 0;
}

/* frequency bases */
/* divided by 10 to be nice to platforms without floating point */
static const int fbase[] = {
	10000,
	100000,
	1000000,
	10000000,
};

/* Multiplier values for TRAN_SPEED.  Multiplied by 10 to be nice
 * to platforms without floating point.
 */
static const int multipliers[] = {
	0,			/* reserved */
	10,
	12,
	13,
	15,
	20,
	25,
	30,
	35,
	40,
	45,
	50,
	55,
	60,
	70,
	80,
};

static void mmc_set_ios(struct mmc *mmc)
{
	mmc->set_ios(mmc);
}

void mmc_set_clock(struct mmc *mmc, u32 clock)
{
	if (clock > mmc->f_max)
		clock = mmc->f_max;

	if (clock < mmc->f_min)
		clock = mmc->f_min;

	mmc->clock = clock;

	mmc_set_ios(mmc);
}

static void mmc_set_bus_width(struct mmc *mmc, u32 width)
{
	mmc->bus_width = width;

	mmc_set_ios(mmc);
}

#include <string.h>
static enum cb_err mmc_startup(struct mmc *mmc)
{
	enum cb_err err;
	int i;
	u32 mult, freq;
	u64 cmult, csize, capacity;
	struct mmc_cmd cmd;
	//ALLOC_CACHE_ALIGN_BUFFER(u8, ext_csd, MMC_MAX_BLOCK_LEN);
	u8 ext_csd[MAX_BLOCK_LEN];
	//ALLOC_CACHE_ALIGN_BUFFER(u8, test_csd, MMC_MAX_BLOCK_LEN);
	u8 test_csd[MAX_BLOCK_LEN];
	int timeout = 1000;

	if (mmc_host_is_spi(mmc)) {	/* enable CRC check for spi */
		cmd.cmdidx = MMC_CMD_SPI_CRC_ON_OFF;
		cmd.resp_type = MMC_RSP_R1;
		cmd.cmdarg = 1;
		err = mmc_send_cmd(mmc, &cmd, NULL);

		if (err != CB_SUCCESS)
			return err;
	}

	/* Put the Card in Identify Mode */
	cmd.cmdidx = mmc_host_is_spi(mmc) ? MMC_CMD_SEND_CID : MMC_CMD_ALL_SEND_CID;	/* cmd not supported in spi */
	cmd.resp_type = MMC_RSP_R2;
	cmd.cmdarg = 0;

	err = mmc_send_cmd(mmc, &cmd, NULL);

	if (err != CB_SUCCESS)
		return err;

	memcpy(mmc->cid, cmd.response, 16);

	/*
	 * For MMC cards, set the Relative Address.
	 * For SD cards, get the Relatvie Address.
	 * This also puts the cards into Standby State
	 */
	if (!mmc_host_is_spi(mmc)) {	/* cmd not supported in spi */
		cmd.cmdidx = SD_CMD_SEND_RELATIVE_ADDR;
		cmd.cmdarg = mmc->rca << 16;
		cmd.resp_type = MMC_RSP_R6;

		err = mmc_send_cmd(mmc, &cmd, NULL);

		if (err != CB_SUCCESS)
			return err;

		if (IS_SD(mmc))
			mmc->rca = (cmd.response[0] >> 16) & 0xffff;
	}

	/* Get the Card-Specific Data */
	cmd.cmdidx = MMC_CMD_SEND_CSD;
	cmd.resp_type = MMC_RSP_R2;
	cmd.cmdarg = mmc->rca << 16;

	err = mmc_send_cmd(mmc, &cmd, NULL);

	/* Waiting for the ready status */
	mmc_send_status(mmc, timeout);

	if (err != CB_SUCCESS)
		return err;

	mmc->csd[0] = cmd.response[0];
	mmc->csd[1] = cmd.response[1];
	mmc->csd[2] = cmd.response[2];
	mmc->csd[3] = cmd.response[3];

	if (mmc->version == MMC_VERSION_UNKNOWN) {
		int version = (cmd.response[0] >> 26) & 0xf;

		switch (version) {
		case 0:
			mmc->version = MMC_VERSION_1_2;
			break;
		case 1:
			mmc->version = MMC_VERSION_1_4;
			break;
		case 2:
			mmc->version = MMC_VERSION_2_2;
			break;
		case 3:
			mmc->version = MMC_VERSION_3;
			break;
		case 4:
			mmc->version = MMC_VERSION_4;
			break;
		default:
			mmc->version = MMC_VERSION_1_2;
			break;
		}
	}

	/* divide frequency by 10, since the mults are 10x bigger */
	freq = fbase[(cmd.response[0] & 0x7)];
	mult = multipliers[((cmd.response[0] >> 3) & 0xf)];

	mmc->tran_speed = freq * mult;

	mmc->read_bl_len = 1 << ((cmd.response[1] >> 16) & 0xf);

	if (IS_SD(mmc))
		mmc->write_bl_len = mmc->read_bl_len;
	else
		mmc->write_bl_len = 1 << ((cmd.response[3] >> 22) & 0xf);

	if (mmc->high_capacity) {
		csize = (mmc->csd[1] & 0x3f) << 16
		    | (mmc->csd[2] & 0xffff0000) >> 16;
		cmult = 8;
	} else {
		csize = (mmc->csd[1] & 0x3ff) << 2
		    | (mmc->csd[2] & 0xc0000000) >> 30;
		cmult = (mmc->csd[2] & 0x00038000) >> 15;
	}

	mmc->capacity_user = (csize + 1) << (cmult + 2);
	mmc->capacity_user *= mmc->read_bl_len;
	mmc->capacity_boot = 0;
	mmc->capacity_rpmb = 0;
	for (i = 0; i < 4; i++)
		mmc->capacity_gp[i] = 0;

	if (mmc->read_bl_len > MMC_MAX_BLOCK_LEN)
		mmc->read_bl_len = MMC_MAX_BLOCK_LEN;

	if (mmc->write_bl_len > MMC_MAX_BLOCK_LEN)
		mmc->write_bl_len = MMC_MAX_BLOCK_LEN;

	/* Select the card, and put it into Transfer Mode */
	if (!mmc_host_is_spi(mmc)) {	/* cmd not supported in spi */
		cmd.cmdidx = MMC_CMD_SELECT_CARD;
		cmd.resp_type = MMC_RSP_R1;
		cmd.cmdarg = mmc->rca << 16;
		err = mmc_send_cmd(mmc, &cmd, NULL);

		if (err != CB_SUCCESS)
			return err;
	}

	/*
	 * For SD, its erase group is always one sector
	 */
	mmc->erase_grp_size = 1;
	mmc->part_config = MMCPART_NOAVAILABLE;
	if (!IS_SD(mmc) && (mmc->version >= MMC_VERSION_4)) {
		/* check  ext_csd version and capacity */
		err = mmc_send_ext_csd(mmc, ext_csd);
		if (!err && (ext_csd[EXT_CSD_REV] >= 2)) {
			/*
			 * According to the JEDEC Standard, the value of
			 * ext_csd's capacity is valid if the value is more
			 * than 2GB
			 */
			capacity = ext_csd[EXT_CSD_SEC_CNT] << 0
			    | ext_csd[EXT_CSD_SEC_CNT + 1] << 8
			    | ext_csd[EXT_CSD_SEC_CNT + 2] << 16
			    | ext_csd[EXT_CSD_SEC_CNT + 3] << 24;
			capacity *= MMC_MAX_BLOCK_LEN;
			if ((capacity >> 20) > 2 * 1024)
				mmc->capacity_user = capacity;
		}

		switch (ext_csd[EXT_CSD_REV]) {
		case 1:
			mmc->version = MMC_VERSION_4_1;
			break;
		case 2:
			mmc->version = MMC_VERSION_4_2;
			break;
		case 3:
			mmc->version = MMC_VERSION_4_3;
			break;
		case 5:
			mmc->version = MMC_VERSION_4_41;
			break;
		case 6:
			mmc->version = MMC_VERSION_4_5;
			break;
		}

		/*
		 * Host needs to enable ERASE_GRP_DEF bit if device is
		 * partitioned. This bit will be lost every time after a reset
		 * or power off. This will affect erase size.
		 */
		if ((ext_csd[EXT_CSD_PARTITIONING_SUPPORT] & PART_SUPPORT) &&
		    (ext_csd[EXT_CSD_PARTITIONS_ATTRIBUTE] & PART_ENH_ATTRIB)) {
			err = mmc_switch(mmc, EXT_CSD_CMD_SET_NORMAL,
					 EXT_CSD_ERASE_GROUP_DEF, 1);

			if (err != CB_SUCCESS)
				return err;

			/* Read out group size from ext_csd */
			mmc->erase_grp_size =
			    ext_csd[EXT_CSD_HC_ERASE_GRP_SIZE] *
			    MMC_MAX_BLOCK_LEN * 1024;
		} else {
			/* Calculate the group size from the csd value. */
			int erase_gsz, erase_gmul;
			erase_gsz = (mmc->csd[2] & 0x00007c00) >> 10;
			erase_gmul = (mmc->csd[2] & 0x000003e0) >> 5;
			mmc->erase_grp_size = (erase_gsz + 1)
			    * (erase_gmul + 1);
		}

		/* store the partition info of emmc */
		if ((ext_csd[EXT_CSD_PARTITIONING_SUPPORT] & PART_SUPPORT) ||
		    ext_csd[EXT_CSD_BOOT_MULT])
			mmc->part_config = ext_csd[EXT_CSD_PART_CONF];

		mmc->capacity_boot = ext_csd[EXT_CSD_BOOT_MULT] << 17;

		mmc->capacity_rpmb = ext_csd[EXT_CSD_RPMB_MULT] << 17;

		for (i = 0; i < 4; i++) {
			int idx = EXT_CSD_GP_SIZE_MULT + i * 3;
			mmc->capacity_gp[i] = (ext_csd[idx + 2] << 16) +
			    (ext_csd[idx + 1] << 8) + ext_csd[idx];
			mmc->capacity_gp[i] *=
			    ext_csd[EXT_CSD_HC_ERASE_GRP_SIZE];
			mmc->capacity_gp[i] *= ext_csd[EXT_CSD_HC_WP_GRP_SIZE];
		}
	}

	err = mmc_set_capacity(mmc, mmc->part_num);
	if (err != CB_SUCCESS)
		return err;

	if (IS_SD(mmc))
		err = sd_change_freq(mmc);
	else
		err = mmc_change_freq(mmc);

	if (err != CB_SUCCESS)
		return err;

	/* Restrict card's capabilities by what the host can do */
	mmc->card_caps &= mmc->host_caps;

	if (IS_SD(mmc)) {
		if (mmc->card_caps & MMC_MODE_4BIT) {
			err = mmc_app_cmd(mmc, (mmc->rca << 16));
			if (err != CB_SUCCESS)
				return err;

			cmd.cmdidx = SD_CMD_APP_SET_BUS_WIDTH;
			cmd.resp_type = MMC_RSP_R1;
			cmd.cmdarg = 2;
			err = mmc_send_cmd(mmc, &cmd, NULL);
			if (err != CB_SUCCESS)
				return err;

			mmc_set_bus_width(mmc, 4);
		}

		if (mmc->card_caps & MMC_MODE_HS)
			mmc->tran_speed = 50000000;
		else
			mmc->tran_speed = 25000000;
	} else {
		int idx;

		/* An array of possible bus widths in order of preference */
		static unsigned ext_csd_bits[] = {
			EXT_CSD_BUS_WIDTH_8,
			EXT_CSD_BUS_WIDTH_4,
			EXT_CSD_BUS_WIDTH_1,
		};

		/* An array to map CSD bus widths to host cap bits */
		static unsigned ext_to_hostcaps[] = {
			[EXT_CSD_BUS_WIDTH_4] = MMC_MODE_4BIT,
			[EXT_CSD_BUS_WIDTH_8] = MMC_MODE_8BIT,
		};

		/* An array to map chosen bus width to an integer */
		static unsigned widths[] = {
			8, 4, 1,
		};

		for (idx = 0; idx < ARRAY_SIZE(ext_csd_bits); idx++) {
			unsigned int extw = ext_csd_bits[idx];

			/*
			 * Check to make sure the controller supports
			 * this bus width, if it's more than 1
			 */
			if (extw != EXT_CSD_BUS_WIDTH_1 &&
			    !(mmc->host_caps & ext_to_hostcaps[extw]))
				continue;

			err = mmc_switch(mmc, EXT_CSD_CMD_SET_NORMAL,
					 EXT_CSD_BUS_WIDTH, extw);

			if (err != CB_SUCCESS)
				continue;

			mmc_set_bus_width(mmc, widths[idx]);

			err = mmc_send_ext_csd(mmc, test_csd);
			if (!err && ext_csd[EXT_CSD_PARTITIONING_SUPPORT]
			    == test_csd[EXT_CSD_PARTITIONING_SUPPORT]
			    && ext_csd[EXT_CSD_ERASE_GROUP_DEF]
			    == test_csd[EXT_CSD_ERASE_GROUP_DEF]
			    && ext_csd[EXT_CSD_REV]
			    == test_csd[EXT_CSD_REV]
			    && ext_csd[EXT_CSD_HC_ERASE_GRP_SIZE]
			    == test_csd[EXT_CSD_HC_ERASE_GRP_SIZE]
			    && memcmp(&ext_csd[EXT_CSD_SEC_CNT],
				      &test_csd[EXT_CSD_SEC_CNT], 4) == 0) {

				mmc->card_caps |= ext_to_hostcaps[extw];
				break;
			}
		}

		if (mmc->card_caps & MMC_MODE_HS) {
			if (mmc->card_caps & MMC_MODE_HS_52MHz)
				mmc->tran_speed = 52000000;
			else
				mmc->tran_speed = 26000000;
		}
	}

	mmc_set_clock(mmc, mmc->tran_speed);

	/* fill in device description */
	mmc->block_dev.lun = 0;
	mmc->block_dev.type = 0;
	mmc->block_dev.blksz = mmc->read_bl_len;
	/* FIXME *///mmc->block_dev.log2blksz = LOG2(mmc->block_dev.blksz);
	//mmc->block_dev.lba = lldiv(mmc->capacity, mmc->read_bl_len);
	mmc->block_dev.lba = mmc->capacity / mmc->read_bl_len;
	mmc->block_dev.vendor[0] = 0;
	mmc->block_dev.product[0] = 0;
	mmc->block_dev.revision[0] = 0;
	return 0;
}

static enum cb_err sd_send_if_cond(struct mmc *mmc)
{
	struct mmc_cmd cmd;
	enum cb_err err;

	DEBUG("Sending SD SEND_IF_COND\n");
	cmd.cmdidx = SD_CMD_SEND_IF_COND;
	/* We set the bit if the host supports voltages between 2.7 and 3.6 V */
	cmd.cmdarg = ((mmc->voltages & 0xff8000) != 0) << 8 | 0xaa;
	cmd.resp_type = MMC_RSP_R7;

	err = mmc_send_cmd(mmc, &cmd, NULL);

	if (err != CB_SUCCESS)
		return err;

	if ((cmd.response[0] & 0xff) != 0xaa)
		return CB_MMC_UNUSABLE;
	else
		mmc->version = SD_VERSION_2;

	return 0;
}

enum cb_err mmc_register(struct mmc *mmc)
{
	/* Setup the universal parts of the block interface just once */
	/////mmc->block_dev.if_type = IF_TYPE_MMC;
	mmc->block_dev.dev = cur_dev_num++;
	mmc->block_dev.removable = 1;
	mmc->block_dev.block_read = mmc_bread;
	mmc->block_dev.block_write = NULL;	//mmc_bwrite;
	mmc->block_dev.block_erase = NULL;	//mmc_berase;
	if (!mmc->b_max)
		mmc->b_max = CONFIG_SYS_MMC_MAX_BLK_COUNT;

	g_mmc = mmc;
	////INIT_LIST_HEAD (&mmc->link);

	////list_add_tail (&mmc->link, &mmc_devices);

	return 0;
}

enum cb_err mmc_start_init(struct mmc *mmc)
{
	enum cb_err err;

	if (mmc_getcd(mmc) == 0) {
		mmc->init_completed = 0;
		LOG("MMC: no card present\n");
		return CB_MMC_NO_CARD;
	}

	if (mmc->init_completed)
		return CB_SUCCESS;

	err = mmc->init(mmc);

	if (err != CB_SUCCESS)
		return err;

	mmc_set_bus_width(mmc, 1);
	mmc_set_clock(mmc, 1);

	/* Reset the Card */
	if ((err = mmc_go_idle(mmc)) != CB_SUCCESS)
		return err;

	/* The internal partition reset to user partition(0) at every CMD0 */
	mmc->part_num = 0;

	/* Test for SD version 2 */
	err = sd_send_if_cond(mmc);

	/* Now try to get the SD card's operating condition */
	err = sd_send_op_cond(mmc);

	/* If the command timed out, we check for an MMC card */
	if (err == CB_MMC_TIMEOUT) {
		err = mmc_send_op_cond(mmc);

		if (err && err != CB_MMC_IN_PROGRESS) {
			LOG("Card did not respond to voltage select!\n");
			return CB_MMC_UNUSABLE;
		}
	}

	if (err == CB_MMC_IN_PROGRESS)
		mmc->init_in_progress = 1;

	return err;
}

static enum cb_err mmc_complete_init(struct mmc *mmc)
{
	enum cb_err err = CB_SUCCESS;

	if (mmc->op_cond_pending)
		err = mmc_complete_op_cond(mmc);

	if (!err)
		err = mmc_startup(mmc);

	mmc->init_completed = (err != CB_SUCCESS) ? 0 : 1;
	mmc->init_in_progress = 0;
	return err;
}

enum cb_err mmc_init(struct mmc *mmc)
{
	enum cb_err err = CB_MMC_IN_PROGRESS;
	////unsigned start = 0;////get_timer(0);

	if (mmc->init_completed)
		return 0;
	if (!mmc->init_in_progress)
		err = mmc_start_init(mmc);

	if (!err || err == CB_MMC_IN_PROGRESS)
		err = mmc_complete_init(mmc);
	LOG("%s: %d, time %lu\n", __func__, err, 0L /*get_timer(start) */ );
	return err;
}

/*
 * CPU and board-specific MMC initializations.  Aliased function
 * signals caller to move on
 */

void print_mmc_devices(char separator)
{
}

enum cb_err get_mmc_num(void)
{
	return cur_dev_num;
}

void mmc_set_preinit(struct mmc *mmc, int preinit)
{
	mmc->preinit = preinit;
}
