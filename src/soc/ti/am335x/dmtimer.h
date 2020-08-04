/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef __SOC_TI_AM335X_DMTIMER_H__
#define __SOC_TI_AM335X_DMTIMER_H__

#include <stdint.h>

#define M_OSC_MHZ (24)

struct am335x_dmtimer {
	uint32_t tidr;
	uint8_t res1[12];
	uint32_t tiocp_cfg;
	uint8_t res2[12];
	uint32_t irq_eoi;
	uint32_t irqstatus_raw;
	uint32_t irqstatus;
	uint32_t irqenable_set;
	uint32_t irqenable_clr;
	uint32_t irqwakeen;
	uint32_t tclr;
	uint32_t tcrr;
	uint32_t tldr;
	uint32_t ttgr;
	uint32_t twps;
	uint32_t tmar;
	uint32_t tcar1;
	uint32_t tsicr;
	uint32_t tcar2;
};

#define TCLR_ST (0x01 << 0)
#define TCLR_AR (0x01 << 1)

#define DMTIMER_2 (0x48040000)

#endif
