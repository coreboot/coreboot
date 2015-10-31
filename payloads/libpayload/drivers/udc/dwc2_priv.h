/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2015 Rockchip Electronics
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

#ifndef __DWC2_PRIV_H__
#define __DWC2_PRIV_H__
#include <usb/dwc2_registers.h>

#define EP_MAXLEN	(64 * 1024)
#define EP0_MAXLEN	64

#define RX_FIFO_SIZE			0x210
#define DTX_FIFO_SIZE_0_OFFSET		RX_FIFO_SIZE
#define DTX_FIFO_SIZE_0			0x10
#define DTX_FIFO_SIZE_1_OFFSET		(DTX_FIFO_SIZE_0_OFFSET +\
					 DTX_FIFO_SIZE_0)
#define DTX_FIFO_SIZE_1			0x100
#define DTX_FIFO_SIZE_2_OFFSET		(DTX_FIFO_SIZE_1_OFFSET +\
					 DTX_FIFO_SIZE_1)
#define DTX_FIFO_SIZE_2			0x10

struct job {
	SIMPLEQ_ENTRY(job) queue; // linkage
	void *data;
	size_t length;
	size_t xfered_length;
	size_t xfer_length;
	int zlp; // append zero length packet?
	int autofree; // free after processing?
};
SIMPLEQ_HEAD(job_queue, job);

typedef struct dwc2_ep {
	dwc2_ep_reg_t *ep_regs;
	struct job_queue job_queue;
	unsigned txfifo:5;
	unsigned busy:1;
	unsigned ep_num:8;
} dwc2_ep_t;

typedef struct dwc2_pdata {
	dwc2_reg_t *regs;
	dwc2_ep_t eps[MAX_EPS_CHANNELS][2];
	uint32_t fifo_map;
	void *setup_buf;
} dwc2_pdata_t;

#define DWC2_PDATA(ctrl) ((dwc2_pdata_t *)((ctrl)->pdata))

#endif
