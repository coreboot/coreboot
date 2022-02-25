/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef _EDP_AUX_H
#define _EDP_AUX_H

#include <types.h>

#define DP_AUX_I2C_WRITE		0x0
#define DP_AUX_I2C_READ			0x1
#define DP_AUX_I2C_STATUS		0x2
#define DP_AUX_I2C_MOT			0x4
#define DP_AUX_NATIVE_WRITE		0x8
#define DP_AUX_NATIVE_READ		0x9
#define REG_EDP_AUX_CTRL			(0x00000030)
#define EDP_AUX_CTRL_ENABLE			(0x00000001)
#define EDP_AUX_CTRL_RESET			(0x00000002)

#define REG_EDP_AUX_DATA			(0x00000034)
#define EDP_AUX_DATA_READ			(0x00000001)
#define EDP_AUX_DATA_DATA__MASK			(0x0000ff00)
#define EDP_AUX_DATA_DATA__SHIFT		(8)

#define EDP_AUX_DATA_INDEX__MASK		(0x00ff0000)
#define EDP_AUX_DATA_INDEX__SHIFT		(16)

#define EDP_AUX_DATA_INDEX_WRITE		(0x80000000)

#define REG_EDP_AUX_TRANS_CTRL			(0x00000038)
#define EDP_AUX_TRANS_CTRL_I2C			(0x00000100)
#define EDP_AUX_TRANS_CTRL_GO			(0x00000200)
#define EDP_AUX_TRANS_CTRL_NO_SEND_ADDR		(0x00000400)
#define EDP_AUX_TRANS_CTRL_NO_SEND_STOP		(0x00000800)

#define REG_EDP_TIMEOUT_COUNT			(0x0000003C)
#define REG_EDP_AUX_LIMITS			(0x00000040)
#define REG_EDP_AUX_STATUS			(0x00000044)
#define AUX_CMD_READ				(BIT(4))

enum {
	EDID_LENGTH = 128,
	EDID_I2C_ADDR = 0x50,
	EDID_EXTENSION_FLAG = 0x7e,
};

static inline uint32_t EDP_AUX_DATA_DATA(uint32_t val)
{
	return ((val) << EDP_AUX_DATA_DATA__SHIFT) & EDP_AUX_DATA_DATA__MASK;
}

static inline uint32_t EDP_AUX_DATA_INDEX(uint32_t val)
{
	return ((val) << EDP_AUX_DATA_INDEX__SHIFT) & EDP_AUX_DATA_INDEX__MASK;
}

void edp_aux_ctrl(int enable);
int edp_read_edid(struct edid *out);
ssize_t edp_aux_transfer(unsigned int address, u8 request, void *buffer, size_t size);

#endif
