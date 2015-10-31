/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2014 Rockchip Electronics
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

#ifndef __DWC2_REGS_H__
#define __DWC2_REGS_H__
#include <usb/dwc2_registers.h>

typedef struct dwc_ctrl {
#define DMA_SIZE (64 * 1024)
	void *dma_buffer;
	u32 *hprt0;
	u32 frame;
} dwc_ctrl_t;

typedef struct {
	u8 *data;
	endpoint_t *endp;
	int reqsize;
	u32 reqtiming;
	u32 timestamp;
} intr_queue_t;

typedef struct {
	int hubaddr;
	int hubport;
} split_info_t;

#define DWC2_INST(controller) ((dwc_ctrl_t *)((controller)->instance))
#define DWC2_REG(controller) ((dwc2_reg_t *)((controller)->reg_base))

typedef enum {
	HCSTAT_DONE = 0,
	HCSTAT_XFERERR,
	HCSTAT_BABBLE,
	HCSTAT_STALL,
	HCSTAT_ACK,
	HCSTAT_NAK,
	HCSTAT_NYET,
	HCSTAT_UNKNOW,
	HCSTAT_TIMEOUT,
	HCSTAT_DISCONNECTED,
} hcstat_t;
#endif
