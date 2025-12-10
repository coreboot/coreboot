/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef __SOC_QUALCOMM_RPMH_INTERNAL_H__
#define __SOC_QUALCOMM_RPMH_INTERNAL_H__

#include <types.h>
#include <soc/tcs.h>

#define MAX_NAME_LENGTH			20

#define USECS_1S		1000000
#define USECS_100MS		100000
#define USECS_10MS		10000
#define USECS_100US		100
#define USECS_5US		5
#define LOOP_DELAY_US		1
#define LOOP_DELAY_10US		10

/* DRV Channel IDs */
enum {
	CH0,
	CH1,
	MAX_CHANNEL,
};

#define TCS_TYPE_NR			5
#define MAX_CMDS_PER_TCS		16
#define MAX_TCS_PER_TYPE		3
#define MAX_TCS_NR			(MAX_TCS_PER_TYPE * TCS_TYPE_NR)
#define MAX_TCS_SLOTS			(MAX_CMDS_PER_TCS * MAX_TCS_PER_TYPE)

#define SOLVER_PRESENT			1
#define HW_CHANNEL_PRESENT		2

#define	CMD_DB_MAX_RESOURCES		250

/* TCS types */
enum {
	ACTIVE_TCS,
	SLEEP_TCS,
	WAKE_TCS,
	CONTROL_TCS,
	FAST_PATH_TCS,
};

/* RSC register offsets */
enum {
	RSC_DRV_TCS_OFFSET,
	RSC_DRV_CMD_OFFSET,
	DRV_SOLVER_CONFIG,
	DRV_PRNT_CHLD_CONFIG,
	RSC_DRV_IRQ_ENABLE,
	RSC_DRV_IRQ_STATUS,
	RSC_DRV_IRQ_CLEAR,
	RSC_DRV_CMD_WAIT_FOR_CMPL,
	RSC_DRV_CONTROL,
	RSC_DRV_STATUS,
	RSC_DRV_CMD_ENABLE,
	RSC_DRV_CMD_MSGID,
	RSC_DRV_CMD_ADDR,
	RSC_DRV_CMD_DATA,
	RSC_DRV_CMD_STATUS,
	RSC_DRV_CMD_RESP_DATA,
	RSC_DRV_CHN_TCS_TRIGGER,
	RSC_DRV_CHN_TCS_COMPLETE,
	RSC_DRV_CHN_SEQ_BUSY,
	RSC_DRV_CHN_SEQ_PC,
	RSC_DRV_CHN_UPDATE,
	RSC_DRV_CHN_BUSY,
	RSC_DRV_CHN_EN,
};

/* DRV TCS Configuration Information Register */
#define DRV_NUM_TCS_MASK		0x3F
#define DRV_NUM_TCS_SHIFT		6
#define DRV_NCPT_MASK			0x1F
#define DRV_NCPT_SHIFT			27

struct rsc_drv;

/**
 * struct cache_req: the request object for caching
 *
 * @addr: the address of the resource
 * @sleep_val: the sleep vote
 * @wake_val: the wake vote
 */
struct cache_req {
	u32 addr;
	u32 sleep_val;
	u32 wake_val;
};

/**
 * struct tcs_group: group of Trigger Command Sets (TCS) to send state requests
 * to the controller
 *
 * @drv:       The controller.
 * @type:      Type of the TCS in this group - active, sleep, wake.
 * @mask:      Mask of the TCSes relative to all the TCSes in the RSC.
 * @offset:    Start of the TCS group relative to the TCSes in the RSC.
 * @num_tcs:   Number of TCSes in this type.
 * @ncpt:      Number of commands in each TCS.
 * @req:       Requests that are sent from the TCS.
 * @slots:     Bitmap indicating which slots are occupied.
 */
struct tcs_group {
	struct rsc_drv *drv;
	int type;
	u32 mask;
	u32 offset;
	int num_tcs;
	int ncpt;
	const struct tcs_request *req[MAX_TCS_PER_TYPE];
	u32 slots[MAX_TCS_SLOTS / 32];
};

/**
 * struct rpmh_request: the message to be sent to rpmh-rsc
 *
 * @msg: the request
 * @cmd: the payload that will be part of the @msg
 */
struct rpmh_request {
	struct tcs_request msg;
	struct tcs_cmd cmd[MAX_RPMH_PAYLOAD];
};

/**
 * struct rpmh_ctrlr: our representation of the controller
 *
 * @dirty: was the cache updated since flush
 * @in_solver_mode: Controller is busy in solver mode
 * @flags: Controller specific flags
 * @batch_cache: Cache sleep and wake requests sent as batch
 * @non_batch_cache_idx: Index for non-batch cache
 * @non_batch_cache: Cache for non-batch requests
 */
struct rpmh_ctrlr {
	bool dirty;
	bool in_solver_mode;
	u32 flags;
	struct rpmh_request batch_cache[RPMH_ACTIVE_ONLY_STATE];
	u32 non_batch_cache_idx;
	struct cache_req *non_batch_cache;
};

/**
 * struct drv_channel: our representation of the drv channels
 *
 * @tcs:                TCS groups
 * @drv:                DRV containing the channel
 * @initialized:        Whether channel is initialized
 */
struct drv_channel {
	struct tcs_group tcs[TCS_TYPE_NR];
	struct rsc_drv *drv;
	bool initialized;
};

/**
 * struct rsc_drv: the Direct Resource Voter (DRV) of the
 * Resource State Coordinator controller (RSC)
 *
 * @name:               Controller identifier
 * @base:               Start address of the DRV registers in this controller
 * @tcs_base:           Start address of the TCS registers in this controller
 * @tcs_distance:       Distance between two TCSes
 * @id:                 Instance id in the controller (Direct Resource Voter)
 * @num_tcs:            Number of TCSes in this DRV
 * @num_channels:       Number of channels in this DRV
 * @in_solver_mode:     Controller is busy in solver mode
 * @initialized:        Whether DRV is initialized
 * @tcs:                TCS groups
 * @ch:                 DRV channels
 * @tcs_in_use:         S/W state of the TCS bitmap
 * @client:             Handle to the DRV's client
 * @dev:                RSC device
 * @regs:               Register offsets for RSC controller
 */
struct rsc_drv {
	char name[MAX_NAME_LENGTH];
	void *base;
	void *tcs_base;
	u32 tcs_distance;
	unsigned int id;
	int num_tcs;
	int num_channels;
	bool in_solver_mode;
	bool initialized;
	struct tcs_group tcs[TCS_TYPE_NR];
	struct drv_channel ch[MAX_CHANNEL];
	u32 tcs_in_use[(MAX_TCS_NR + 31) / 32];
	struct rpmh_ctrlr client;
	struct device *dev;
	u32 *regs;
};

extern bool rpmh_standalone;

int rpmh_rsc_send_data(struct rsc_drv *drv, const struct tcs_request *msg, int ch);
int rpmh_rsc_write_ctrl_data(struct rsc_drv *drv,
			     const struct tcs_request *msg,
			     int ch);
void rpmh_rsc_invalidate(struct rsc_drv *drv, int ch);
int rpmh_rsc_mode_solver_set(struct rsc_drv *drv, bool enable);
int rpmh_rsc_get_channel(struct rsc_drv *drv);
int rpmh_rsc_switch_channel(struct rsc_drv *drv, int ch);
int rpmh_rsc_drv_enable(struct rsc_drv *drv, bool enable);

int rpmh_flush(struct rpmh_ctrlr *ctrlr, int ch);

int rpmh_rsc_init_fast_path(struct rsc_drv *drv, const struct tcs_request *msg, int ch);
int rpmh_rsc_update_fast_path(struct rsc_drv *drv,
			      const struct tcs_request *msg,
			      u32 update_mask, int ch);

void rpmh_set_rsc_drv(struct rsc_drv *drv);

#endif /* __SOC_QUALCOMM_RPMH_INTERNAL_H__ */
