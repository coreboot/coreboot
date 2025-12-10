/* SPDX-License-Identifier: GPL-2.0-only */

#include <console/console.h>
#include <delay.h>
#include <device/mmio.h>
#include <string.h>
#include <timer.h>
#include <soc/cmd_db.h>
#include <soc/tcs.h>
#include <soc/rpmh_internal.h>

#define TCS_AMC_MODE_ENABLE		BIT(16)
#define TCS_AMC_MODE_TRIGGER		BIT(24)

/* TCS CMD register bit mask */
#define CMD_MSGID_LEN			8
#define CMD_MSGID_RESP_REQ		BIT(8)
#define CMD_MSGID_WRITE			BIT(16)
#define CMD_STATUS_ISSUED		BIT(8)
#define CMD_STATUS_COMPL		BIT(16)

/* Offsets for DRV channel status register */
#define CH0_CHN_BUSY			BIT(0)
#define CH1_CHN_BUSY			BIT(1)
#define CH0_WAKE_TCS_STATUS		BIT(0)
#define CH0_SLEEP_TCS_STATUS		BIT(1)
#define CH1_WAKE_TCS_STATUS		BIT(2)
#define CH1_SLEEP_TCS_STATUS		BIT(3)
#define CH_CLEAR_STATUS			BIT(31)

static inline void *tcs_reg_addr(const struct rsc_drv *drv, int reg, int tcs_id)
{
	if (!drv->tcs_distance)
		return drv->tcs_base + drv->regs[RSC_DRV_TCS_OFFSET] * tcs_id + reg;

	return drv->tcs_base + drv->tcs_distance * tcs_id + reg;
}

static inline void *tcs_cmd_addr(const struct rsc_drv *drv, int reg, int tcs_id, int cmd_id)
{
	return tcs_reg_addr(drv, reg, tcs_id) + drv->regs[RSC_DRV_CMD_OFFSET] * cmd_id;
}

static u32 read_tcs_cmd(const struct rsc_drv *drv, int reg, int tcs_id, int cmd_id)
{
	return read32(tcs_cmd_addr(drv, reg, tcs_id, cmd_id));
}

static u32 read_tcs_reg(const struct rsc_drv *drv, int reg, int tcs_id)
{
	return read32(tcs_reg_addr(drv, reg, tcs_id));
}

static void write_tcs_cmd(const struct rsc_drv *drv, int reg, int tcs_id,
			  int cmd_id, u32 data)
{
	write32(tcs_cmd_addr(drv, reg, tcs_id, cmd_id), data);
}

static void write_tcs_reg(const struct rsc_drv *drv, int reg, int tcs_id, u32 data)
{
	write32(tcs_reg_addr(drv, reg, tcs_id), data);
}

static void write_tcs_reg_sync(const struct rsc_drv *drv, int reg, int tcs_id, u32 data)
{
	struct stopwatch sw;

	write32(tcs_reg_addr(drv, reg, tcs_id), data);

	/* Wait until we read back the same value */
	stopwatch_init_usecs_expire(&sw, USECS_1S);
	while (read32(tcs_reg_addr(drv, reg, tcs_id)) != data) {
		if (stopwatch_expired(&sw)) {
			printk(BIOS_ERR, "%s: error writing %#x to %d:%#x\n", drv->name,
			       data, tcs_id, reg);
			break;
		}
		udelay(LOOP_DELAY_US);
	}
}

static void tcs_invalidate(struct rsc_drv *drv, int type, int ch)
{
	int m;
	struct tcs_group *tcs = &drv->ch[ch].tcs[type];

	/* Check if already empty */
	if (tcs->slots[0] == 0)
		return;

	for (m = tcs->offset; m < tcs->offset + tcs->num_tcs; m++) {
		write_tcs_reg_sync(drv, drv->regs[RSC_DRV_CMD_ENABLE], m, 0);
		write_tcs_reg_sync(drv, drv->regs[RSC_DRV_CMD_WAIT_FOR_CMPL], m, 0);
	}

	memset(tcs->slots, 0, sizeof(tcs->slots));
}

int rpmh_rsc_get_channel(struct rsc_drv *drv)
{
	int chn_update, chn_busy;

	if (drv->num_channels == 1)
		return CH0;

	/* Select Unused channel */
	do {
		chn_update = read32(drv->base + drv->regs[RSC_DRV_CHN_UPDATE]);
		chn_busy = read32(drv->base + drv->regs[RSC_DRV_CHN_BUSY]);
	} while (chn_busy != chn_update);

	if (chn_busy & CH0_CHN_BUSY)
		return CH1;
	else if (chn_busy & CH1_CHN_BUSY)
		return CH0;
	else
		return -1;
}

void rpmh_rsc_invalidate(struct rsc_drv *drv, int ch)
{
	tcs_invalidate(drv, SLEEP_TCS, ch);
	tcs_invalidate(drv, WAKE_TCS, ch);
}

static struct tcs_group *get_tcs_for_msg(struct rsc_drv *drv,
					 enum rpmh_state state,
					 int ch)
{
	int type;
	struct tcs_group *tcs;

	switch (state) {
	case RPMH_ACTIVE_ONLY_STATE:
		type = ACTIVE_TCS;
		break;
	case RPMH_WAKE_ONLY_STATE:
		type = WAKE_TCS;
		break;
	case RPMH_SLEEP_STATE:
		type = SLEEP_TCS;
		break;
	default:
		return NULL;
	}

	tcs = &drv->ch[ch].tcs[type];
	if (state == RPMH_ACTIVE_ONLY_STATE && !tcs->num_tcs)
		tcs = &drv->ch[ch].tcs[WAKE_TCS];

	return tcs;
}

static void __tcs_set_trigger(struct rsc_drv *drv, int tcs_id, bool trigger)
{
	u32 enable;
	u32 reg = drv->regs[RSC_DRV_CONTROL];

	enable = read_tcs_reg(drv, reg, tcs_id);
	enable &= ~TCS_AMC_MODE_TRIGGER;
	write_tcs_reg_sync(drv, reg, tcs_id, enable);
	enable &= ~TCS_AMC_MODE_ENABLE;
	write_tcs_reg_sync(drv, reg, tcs_id, enable);

	if (trigger) {
		enable = TCS_AMC_MODE_ENABLE;
		write_tcs_reg_sync(drv, reg, tcs_id, enable);
		enable |= TCS_AMC_MODE_TRIGGER;
		write_tcs_reg(drv, reg, tcs_id, enable);
	}
}

static void __tcs_buffer_write(struct rsc_drv *drv, int tcs_id, int cmd_id,
			       const struct tcs_request *msg)
{
	u32 msgid;
	u32 cmd_msgid = CMD_MSGID_LEN | CMD_MSGID_WRITE;
	u32 cmd_enable = 0;
	u32 cmd_complete;
	struct tcs_cmd *cmd;
	int i, j;

	cmd_msgid |= msg->wait_for_compl ? CMD_MSGID_RESP_REQ : 0;
	cmd_complete = read_tcs_reg(drv, drv->regs[RSC_DRV_CMD_WAIT_FOR_CMPL], tcs_id);

	for (i = 0, j = cmd_id; i < msg->num_cmds; i++, j++) {
		cmd = &msg->cmds[i];
		cmd_enable |= BIT(j);
		cmd_complete |= cmd->wait << j;
		msgid = cmd_msgid;
		msgid |= cmd->wait ? CMD_MSGID_RESP_REQ : 0;

		write_tcs_cmd(drv, drv->regs[RSC_DRV_CMD_MSGID], tcs_id, j, msgid);
		write_tcs_cmd(drv, drv->regs[RSC_DRV_CMD_ADDR], tcs_id, j, cmd->addr);
		write_tcs_cmd(drv, drv->regs[RSC_DRV_CMD_DATA], tcs_id, j, cmd->data);
	}

	write_tcs_reg(drv, drv->regs[RSC_DRV_CMD_WAIT_FOR_CMPL], tcs_id, cmd_complete);
	cmd_enable |= read_tcs_reg(drv, drv->regs[RSC_DRV_CMD_ENABLE], tcs_id);
	write_tcs_reg(drv, drv->regs[RSC_DRV_CMD_ENABLE], tcs_id, cmd_enable);
}

static int check_for_req_inflight(struct rsc_drv *drv, struct tcs_group *tcs,
				  const struct tcs_request *msg)
{
	u32 curr_enabled;
	u32 addr;
	int j, k;
	int i = tcs->offset;

	for (; i < tcs->offset + tcs->num_tcs; i++) {
		if (!(drv->tcs_in_use[i / 32] & BIT(i % 32)))
			continue;

		curr_enabled = read_tcs_reg(drv, drv->regs[RSC_DRV_CMD_ENABLE], i);

		for (j = 0; j < tcs->ncpt; j++) {
			if (!(curr_enabled & BIT(j)))
				continue;

			addr = read_tcs_cmd(drv, drv->regs[RSC_DRV_CMD_ADDR], i, j);
			for (k = 0; k < msg->num_cmds; k++) {
				if (cmd_db_match_resource_addr(msg->cmds[k].addr, addr))
					return -1;
			}
		}
	}

	return 0;
}

static int find_free_tcs(struct tcs_group *tcs)
{
	const struct rsc_drv *drv = tcs->drv;
	struct stopwatch sw;
	int i;
	u32 sts;

	for (i = tcs->offset; i < tcs->offset + tcs->num_tcs; i++) {
		if (drv->tcs_in_use[i / 32] & BIT(i % 32))
			continue;

		stopwatch_init_usecs_expire(&sw, USECS_100US);
		while (1) {
			sts = read_tcs_reg(drv, drv->regs[RSC_DRV_STATUS], i);
			if (sts)
				return i;
			if (stopwatch_expired(&sw))
				return -1;
			udelay(LOOP_DELAY_US);
		}
	}

	return -1;
}

static int claim_tcs_for_req(struct rsc_drv *drv, struct tcs_group *tcs,
			     const struct tcs_request *msg)
{
	int ret;

	ret = check_for_req_inflight(drv, tcs, msg);
	if (ret)
		return ret;

	return find_free_tcs(tcs);
}

int rpmh_rsc_send_data(struct rsc_drv *drv, const struct tcs_request *msg, int ch)
{
	struct tcs_group *tcs;
	struct stopwatch sw;
	int tcs_id;
	u32 status;

	tcs = get_tcs_for_msg(drv, msg->state, ch);
	if (!tcs)
		return -1;

	/* Controller is busy in 'solver' mode */
	if (drv->in_solver_mode) {
		printk(BIOS_ERR, "RPMH RSC: Cannot send ACTIVE request in solver mode\n");
		return -1;
	}

	/* Wait for a free tcs with hardware status verification */
	stopwatch_init_usecs_expire(&sw, USECS_100MS);
	while (1) {
		tcs_id = claim_tcs_for_req(drv, tcs, msg);
		if (tcs_id >= 0) {
			/* Double-check hardware status register */
			status = read_tcs_reg(drv, drv->regs[RSC_DRV_STATUS], tcs_id);
			if (status) {  /* 1 = IDLE, 0 = BUSY */
				break;
			}
			/* TCS claimed but hardware still busy, retry */
			printk(BIOS_DEBUG, "RPMH RSC: TCS %d claimed but HW busy, retrying\n", tcs_id);
		}
		if (stopwatch_expired(&sw)) {
			printk(BIOS_ERR, "RPMH RSC: No free TCS available (timeout after 100ms)\n");
			return -1;
		}
		udelay(LOOP_DELAY_10US);
	}

	tcs->req[tcs_id - tcs->offset] = msg;
	drv->tcs_in_use[tcs_id / 32] |= BIT(tcs_id % 32);

	write_tcs_reg_sync(drv, drv->regs[RSC_DRV_CMD_ENABLE], tcs_id, 0);
	write_tcs_reg_sync(drv, drv->regs[RSC_DRV_CMD_WAIT_FOR_CMPL], tcs_id, 0);

	__tcs_buffer_write(drv, tcs_id, 0, msg);
	__tcs_set_trigger(drv, tcs_id, 1);

	/* Poll for completion if wait_for_compl is set */
	if (msg->wait_for_compl) {
		stopwatch_init_usecs_expire(&sw, USECS_10MS);
		while (1) {
			u32 sts = read_tcs_reg(drv, drv->regs[RSC_DRV_STATUS], tcs_id);
			if (sts)
				break;
			if (stopwatch_expired(&sw)) {
				printk(BIOS_ERR, "RPMH RSC: TCS %d timeout\n", tcs_id);
				return -1;
			}
			udelay(LOOP_DELAY_US);
		}
	}

	drv->tcs_in_use[tcs_id / 32] &= ~BIT(tcs_id % 32);

	return 0;
}

static int find_slots(struct tcs_group *tcs, const struct tcs_request *msg,
		      int *tcs_id, int *cmd_id)
{
	int slot, offset;
	int i = 0;
	int slots_needed = msg->num_cmds;

	/* Find contiguous slots */
	do {
		slot = 0;
		for (i = 0; i < tcs->ncpt * tcs->num_tcs; i++) {
			if (tcs->slots[i / 32] & BIT(i % 32)) {
				slot = 0;
			} else {
				slot++;
				if (slot >= slots_needed)
					break;
			}
		}
		if (slot < slots_needed)
			return -1;

		i = i - slot + 1;
	} while (i + slots_needed - 1 >= ((i / tcs->ncpt) + 1) * tcs->ncpt);

	/* Mark slots as used */
	for (slot = i; slot < i + slots_needed; slot++)
		tcs->slots[slot / 32] |= BIT(slot % 32);

	offset = i / tcs->ncpt;
	*tcs_id = offset + tcs->offset;
	*cmd_id = i % tcs->ncpt;

	return 0;
}

int rpmh_rsc_write_ctrl_data(struct rsc_drv *drv, const struct tcs_request *msg, int ch)
{
	struct tcs_group *tcs;
	int tcs_id = 0, cmd_id = 0;
	int ret;

	if (!msg->num_cmds)
		return 0;

	tcs = get_tcs_for_msg(drv, msg->state, ch);
	if (!tcs)
		return -1;

	ret = find_slots(tcs, msg, &tcs_id, &cmd_id);
	if (!ret)
		__tcs_buffer_write(drv, tcs_id, cmd_id, msg);

	return ret;
}

int rpmh_rsc_mode_solver_set(struct rsc_drv *drv, bool enable)
{
	drv->in_solver_mode = enable;
	return 0;
}

int rpmh_rsc_switch_channel(struct rsc_drv *drv, int ch)
{
	struct stopwatch sw;
	u32 sts;
	u32 mask = (ch == 0) ? CH0_WAKE_TCS_STATUS : CH1_WAKE_TCS_STATUS;

	write32(drv->base + drv->regs[RSC_DRV_CHN_UPDATE], BIT(ch));

	stopwatch_init_usecs_expire(&sw, USECS_100US);
	while (1) {
		sts = read32(drv->base + drv->regs[RSC_DRV_CHN_TCS_COMPLETE]);
		if (sts & mask)
			break;
		if (stopwatch_expired(&sw))
			return -1;
		udelay(LOOP_DELAY_US);
	}

	write32(drv->base + drv->regs[RSC_DRV_CHN_TCS_COMPLETE], CH_CLEAR_STATUS);

	return 0;
}

int rpmh_rsc_drv_enable(struct rsc_drv *drv, bool enable)
{
	int ret = 0, ch;
	u32 chn_en;

	chn_en = read32(drv->base + drv->regs[RSC_DRV_CHN_EN]);
	if (chn_en == enable)
		return -1;

	if (enable) {
		ch = 0;
		ret = rpmh_flush(&drv->client, ch);
		if (ret)
			return ret;

		write32(drv->base + drv->regs[RSC_DRV_CHN_EN], enable);
		ret = rpmh_rsc_switch_channel(drv, ch);
	} else {
		ch = rpmh_rsc_get_channel(drv);
		if (ch < 0)
			return ch;

		ret = rpmh_flush(&drv->client, ch);
		if (ret)
			return ret;

		ret = rpmh_rsc_switch_channel(drv, ch);
		if (ret)
			return ret;

		write32(drv->base + drv->regs[RSC_DRV_CHN_UPDATE], 0);
		write32(drv->base + drv->regs[RSC_DRV_CHN_EN], enable);
	}

	return ret;
}

int rpmh_rsc_init_fast_path(struct rsc_drv *drv, const struct tcs_request *msg, int ch)
{
	int tcs_id;

	if (!drv->ch[ch].tcs[FAST_PATH_TCS].num_tcs)
		return -1;

	tcs_id = drv->ch[ch].tcs[FAST_PATH_TCS].offset;
	__tcs_buffer_write(drv, tcs_id, 0, msg);

	return 0;
}

int rpmh_rsc_update_fast_path(struct rsc_drv *drv,
			      const struct tcs_request *msg,
			      u32 mask, int ch)
{
	struct stopwatch sw;
	int i;
	u32 sts;
	int tcs_id;
	struct tcs_cmd *cmd;

	if (!drv->ch[ch].tcs[FAST_PATH_TCS].num_tcs)
		return -1;

	tcs_id = drv->ch[ch].tcs[FAST_PATH_TCS].offset;

	/* Ensure the TCS is free before writing */
	stopwatch_init_usecs_expire(&sw, USECS_5US);
	while (1) {
		sts = read_tcs_reg(drv, drv->regs[RSC_DRV_STATUS], tcs_id);
		if (sts)
			break;
		if (stopwatch_expired(&sw)) {
			printk(BIOS_ERR, "Fast-path TCS is too busy\n");
			return -1;
		}
		udelay(LOOP_DELAY_US);
	}

	/* Update only the data fields */
	for (i = 0; i < msg->num_cmds; i++) {
		if (!(mask & BIT(i)))
			continue;
		cmd = &msg->cmds[i];
		write_tcs_cmd(drv, drv->regs[RSC_DRV_CMD_DATA], tcs_id, i, cmd->data);
	}

	/* Trigger the TCS */
	__tcs_set_trigger(drv, tcs_id, 1);

	return 0;
}
