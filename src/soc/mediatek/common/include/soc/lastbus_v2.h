/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef SOC_MEDIATEK_COMMON_LASTBUS_V2_H
#define SOC_MEDIATEK_COMMON_LASTBUS_V2_H

#define TIMEOUT_THRES_SHIFT		16
#define TIMEOUT_TYPE_SHIFT		1
#define LASTBUS_TIMEOUT_CLR		0x0200
#define LASTBUS_DEBUG_CKEN		0x0008
#define LASTBUS_DEBUG_EN		0x0004
#define LASTBUS_TIMEOUT			0x0001

struct lastbus_idle_mask {
	u32 reg_offset;
	u32 reg_value;
};

struct lastbus_monitor {
	const char *name;
	uintptr_t base;
	size_t num_ports;
	u16 bus_freq_mhz;
	size_t num_idle_mask;
	const struct lastbus_idle_mask *idle_masks;
};

struct lastbus_config {
	const char *latch_platform;
	unsigned int timeout_ms;
	unsigned int timeout_type;
	unsigned int num_used_monitors;
	const struct lastbus_monitor *monitors;
};

void lastbus_init(void);
extern const struct lastbus_config lastbus_cfg;

#endif
