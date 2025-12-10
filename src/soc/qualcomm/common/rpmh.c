/* SPDX-License-Identifier: GPL-2.0-only */

#include <console/console.h>
#include <delay.h>
#include <device/device.h>
#include <string.h>
#include <soc/rpmh.h>
#include <soc/cmd_db.h>
#include <soc/rpmh_internal.h>

#define RPMH_TIMEOUT_MS			10000

#define DEFINE_RPMH_MSG_ONSTACK(s, name)		\
	struct rpmh_request name = {			\
		.msg = {				\
			.state = s,			\
			.cmds = name.cmd,		\
			.num_cmds = 0,			\
			.wait_for_compl = 1,		\
		},					\
		.cmd = { { 0 } },			\
	}

#define ctrlr_to_drv(ctrlr) \
	((struct rsc_drv *)((char *)(ctrlr) - offsetof(struct rsc_drv, client)))

bool rpmh_standalone;

static struct rsc_drv *g_rsc_drv = NULL;

static struct rpmh_ctrlr *get_rpmh_ctrlr(void)
{
	return g_rsc_drv ? &g_rsc_drv->client : NULL;
}

void rpmh_set_rsc_drv(struct rsc_drv *drv)
{
	g_rsc_drv = drv;
}

static int check_ctrlr_state(struct rpmh_ctrlr *ctrlr, enum rpmh_state state)
{
	if (state != RPMH_ACTIVE_ONLY_STATE)
		return 0;

	if (!(ctrlr->flags & SOLVER_PRESENT))
		return 0;

	if (ctrlr->in_solver_mode)
		return -1;

	return 0;
}

static struct cache_req *get_non_batch_cache_req(struct rpmh_ctrlr *ctrlr, u32 addr)
{
	struct cache_req *req = ctrlr->non_batch_cache;
	unsigned int i;

	if (ctrlr->non_batch_cache_idx >= CMD_DB_MAX_RESOURCES)
		return NULL;

	for (i = 0; i < ctrlr->non_batch_cache_idx; i++) {
		req = &ctrlr->non_batch_cache[i];
		if (req->addr == addr)
			return req;
	}

	req = &ctrlr->non_batch_cache[ctrlr->non_batch_cache_idx];
	req->sleep_val = req->wake_val = UINT_MAX;
	ctrlr->non_batch_cache_idx++;

	return req;
}

static struct cache_req *cache_rpm_request(struct rpmh_ctrlr *ctrlr,
					   enum rpmh_state state,
					   struct tcs_cmd *cmd)
{
	struct cache_req *req;
	u32 old_sleep_val, old_wake_val;

	req = get_non_batch_cache_req(ctrlr, cmd->addr);
	if (!req)
		return NULL;

	req->addr = cmd->addr;
	old_sleep_val = req->sleep_val;
	old_wake_val = req->wake_val;

	switch (state) {
	case RPMH_ACTIVE_ONLY_STATE:
	case RPMH_WAKE_ONLY_STATE:
		req->wake_val = cmd->data;
		break;
	case RPMH_SLEEP_STATE:
		req->sleep_val = cmd->data;
		break;
	}

	ctrlr->dirty |= (req->sleep_val != old_sleep_val ||
			 req->wake_val != old_wake_val) &&
			 req->sleep_val != UINT_MAX &&
			 req->wake_val != UINT_MAX;

	return req;
}

static int __rpmh_write(enum rpmh_state state, struct rpmh_request *rpm_msg)
{
	struct rpmh_ctrlr *ctrlr = get_rpmh_ctrlr();
	int ret = -1;
	struct cache_req *req;
	int i, ch;

	if (rpmh_standalone)
		return 0;

	if (!ctrlr)
		return -1;

	for (i = 0; i < rpm_msg->msg.num_cmds; i++) {
		req = cache_rpm_request(ctrlr, state, &rpm_msg->msg.cmds[i]);
		if (!req)
			return -1;
	}

	if (state == RPMH_ACTIVE_ONLY_STATE) {
		ch = rpmh_rsc_get_channel(ctrlr_to_drv(ctrlr));
		if (ch < 0)
			return ch;

		ret = rpmh_rsc_send_data(ctrlr_to_drv(ctrlr), &rpm_msg->msg, ch);
	} else {
		ret = 0;
	}

	return ret;
}

static int __fill_rpmh_msg(struct rpmh_request *req, enum rpmh_state state,
		const struct tcs_cmd *cmd, u32 num_cmds)
{
	if (!cmd || !num_cmds || num_cmds > MAX_RPMH_PAYLOAD)
		return -1;

	memcpy(req->cmd, cmd, num_cmds * sizeof(*cmd));

	req->msg.state = state;
	req->msg.cmds = req->cmd;
	req->msg.num_cmds = num_cmds;

	return 0;
}

int rpmh_write_async(enum rpmh_state state, const struct tcs_cmd *cmd, u32 n)
{
	DEFINE_RPMH_MSG_ONSTACK(state, rpm_msg);
	struct rpmh_ctrlr *ctrlr = get_rpmh_ctrlr();
	int ret;

	if (rpmh_standalone)
		return 0;

	if (!ctrlr)
		return -1;

	rpm_msg.msg.wait_for_compl = 0;
	ret = check_ctrlr_state(ctrlr, state);
	if (ret)
		return ret;

	ret = __fill_rpmh_msg(&rpm_msg, state, cmd, n);
	if (ret)
		return ret;

	return __rpmh_write(state, &rpm_msg);
}

int rpmh_write(enum rpmh_state state, const struct tcs_cmd *cmd, u32 n)
{
	DEFINE_RPMH_MSG_ONSTACK(state, rpm_msg);
	struct rpmh_ctrlr *ctrlr = get_rpmh_ctrlr();
	int ret;

	if (rpmh_standalone)
		return 0;

	if (!ctrlr)
		return -1;

	ret = check_ctrlr_state(ctrlr, state);
	if (ret)
		return ret;

	ret = __fill_rpmh_msg(&rpm_msg, state, cmd, n);
	if (ret)
		return ret;

	ret = __rpmh_write(state, &rpm_msg);

	return ret;
}

static int flush_batch(struct rpmh_ctrlr *ctrlr, int ch)
{
	int ret;

	/* Send Sleep/Wake requests to the controller */
	ret = rpmh_rsc_write_ctrl_data(ctrlr_to_drv(ctrlr),
				       &ctrlr->batch_cache[RPMH_SLEEP_STATE].msg, ch);
	if (ret)
		return ret;

	return rpmh_rsc_write_ctrl_data(ctrlr_to_drv(ctrlr),
					&ctrlr->batch_cache[RPMH_WAKE_ONLY_STATE].msg, ch);
}

int rpmh_write_batch(enum rpmh_state state, const struct tcs_cmd *cmd, u32 *n)
{
	DEFINE_RPMH_MSG_ONSTACK(state, rpm_msg);
	struct rpmh_ctrlr *ctrlr = get_rpmh_ctrlr();
	int ret, ch;

	if (rpmh_standalone)
		return 0;

	if (!ctrlr)
		return -1;

	ret = check_ctrlr_state(ctrlr, state);
	if (ret)
		return ret;

	if (state == RPMH_ACTIVE_ONLY_STATE) {
		ret = __fill_rpmh_msg(&rpm_msg, state, cmd, *n);
		if (ret)
			return ret;
	} else {
		memset(&ctrlr->batch_cache[state], 0, sizeof(struct rpmh_request));
		ret = __fill_rpmh_msg(&ctrlr->batch_cache[state], state, cmd, *n);
		ctrlr->dirty = 1;
		return ret;
	}

	ch = rpmh_rsc_get_channel(ctrlr_to_drv(ctrlr));
	if (ch < 0)
		return ch;

	ret = rpmh_rsc_send_data(ctrlr_to_drv(ctrlr), &rpm_msg.msg, ch);
	if (ret) {
		printk(BIOS_ERR, "RPMH: Error sending message addr=%#x\n",
		       rpm_msg.msg.cmds[0].addr);
		return ret;
	}

	return 0;
}

static int is_req_valid(struct cache_req *req)
{
	return (req->sleep_val != UINT_MAX &&
		req->wake_val != UINT_MAX &&
		req->sleep_val != req->wake_val);
}

static int send_single(struct rpmh_ctrlr *ctrlr, enum rpmh_state state,
		       u32 addr, u32 data, int ch)
{
	DEFINE_RPMH_MSG_ONSTACK(state, rpm_msg);

	/* Wake sets are always complete and sleep sets are not */
	rpm_msg.msg.wait_for_compl = (state == RPMH_WAKE_ONLY_STATE);
	rpm_msg.cmd[0].addr = addr;
	rpm_msg.cmd[0].data = data;
	rpm_msg.msg.num_cmds = 1;

	return rpmh_rsc_write_ctrl_data(ctrlr_to_drv(ctrlr), &rpm_msg.msg, ch);
}

int rpmh_flush(struct rpmh_ctrlr *ctrlr, int ch)
{
	struct cache_req *p;
	int ret = 0, i;

	if (!ctrlr->dirty) {
		printk(BIOS_DEBUG, "RPMH: Skipping flush, TCS has latest data\n");
		return 0;
	}

	/* Invalidate the TCSes first to avoid stale data */
	rpmh_rsc_invalidate(ctrlr_to_drv(ctrlr), ch);

	/* First flush the cached batch requests */
	ret = flush_batch(ctrlr, ch);
	if (ret)
		return ret;

	for (i = 0; i < ctrlr->non_batch_cache_idx; i++) {
		p = &ctrlr->non_batch_cache[i];
		if (!is_req_valid(p)) {
			printk(BIOS_DEBUG, "RPMH: skipping req: a:%#x s:%#x w:%#x\n",
				 p->addr, p->sleep_val, p->wake_val);
			continue;
		}
		ret = send_single(ctrlr, RPMH_SLEEP_STATE, p->addr,
				  p->sleep_val, ch);
		if (ret)
			return ret;
		ret = send_single(ctrlr, RPMH_WAKE_ONLY_STATE, p->addr,
				  p->wake_val, ch);
		if (ret)
			return ret;
	}

	ctrlr->dirty = 0;
	return ret;
}

int rpmh_write_sleep_and_wake(void)
{
	struct rpmh_ctrlr *ctrlr = get_rpmh_ctrlr();
	int ch, ret;

	if (!ctrlr)
		return -1;

	ch = rpmh_rsc_get_channel(ctrlr_to_drv(ctrlr));
	if (ch < 0)
		return ch;

	ret = rpmh_flush(ctrlr, ch);
	if (ret || !(ctrlr->flags & HW_CHANNEL_PRESENT))
		return ret;

	return rpmh_rsc_switch_channel(ctrlr_to_drv(ctrlr), ch);
}

void rpmh_invalidate(void)
{
	struct rpmh_ctrlr *ctrlr = get_rpmh_ctrlr();

	if (rpmh_standalone || !ctrlr)
		return;

	memset(&ctrlr->batch_cache[RPMH_SLEEP_STATE], 0, sizeof(struct rpmh_request));
	memset(&ctrlr->batch_cache[RPMH_WAKE_ONLY_STATE], 0, sizeof(struct rpmh_request));
	ctrlr->dirty = 1;
}

int rpmh_mode_solver_set(bool enable)
{
	struct rpmh_ctrlr *ctrlr = get_rpmh_ctrlr();
	int ret;

	if (rpmh_standalone || !ctrlr)
		return 0;

	if (!(ctrlr->flags & SOLVER_PRESENT))
		return -1;

	ret = rpmh_rsc_mode_solver_set(ctrlr_to_drv(ctrlr), enable);
	if (!ret)
		ctrlr->in_solver_mode = enable;

	return ret;
}

int rpmh_init_fast_path(struct tcs_cmd *cmd, int n)
{
	struct rpmh_ctrlr *ctrlr = get_rpmh_ctrlr();
	struct tcs_request req;
	int ch;

	if (rpmh_standalone || !ctrlr)
		return 0;

	ch = rpmh_rsc_get_channel(ctrlr_to_drv(ctrlr));
	if (ch < 0)
		return ch;

	req.cmds = cmd;
	req.num_cmds = n;
	req.wait_for_compl = 0;

	return rpmh_rsc_init_fast_path(ctrlr_to_drv(ctrlr), &req, ch);
}

int rpmh_update_fast_path(struct tcs_cmd *cmd, int n, u32 update_mask)
{
	struct rpmh_ctrlr *ctrlr = get_rpmh_ctrlr();
	struct tcs_request req;
	int ch;

	if (rpmh_standalone || !ctrlr)
		return 0;

	ch = rpmh_rsc_get_channel(ctrlr_to_drv(ctrlr));
	if (ch < 0)
		return ch;

	req.cmds = cmd;
	req.num_cmds = n;
	req.wait_for_compl = 0;

	return rpmh_rsc_update_fast_path(ctrlr_to_drv(ctrlr), &req,
					 update_mask, ch);
}
