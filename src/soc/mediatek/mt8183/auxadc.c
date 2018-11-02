/*
 * This file is part of the coreboot project.
 *
 * Copyright 2018 MediaTek Inc.
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

#include <arch/io.h>
#include <assert.h>
#include <console/console.h>
#include <delay.h>
#include <soc/addressmap.h>
#include <soc/auxadc.h>
#include <soc/infracfg.h>
#include <timer.h>

static struct mtk_auxadc_regs *const mtk_auxadc = (void *)AUXADC_BASE;

static uint32_t auxadc_get_rawdata(int channel)
{
	setbits_le32(&mt8183_infracfg->module_sw_cg_1_clr, 1 << 10);
	assert(wait_ms(300, !(read32(&mtk_auxadc->con2) & 0x1)));

	clrbits_le32(&mtk_auxadc->con1, 1 << channel);
	assert(wait_ms(300, !(read32(&mtk_auxadc->data[channel]) & (1 << 12))));

	setbits_le32(&mtk_auxadc->con1, 1 << channel);
	udelay(25);
	assert(wait_ms(300, read32(&mtk_auxadc->data[channel]) & (1 << 12)));

	uint32_t value = read32(&mtk_auxadc->data[channel]) & 0x0FFF;

	setbits_le32(&mt8183_infracfg->module_sw_cg_1_set, 1 << 10);

	return value;
}

int auxadc_get_voltage(unsigned int channel)
{
	assert(channel < 16);

	/* 1.5V in 4096 steps */
	return (int)((int64_t)auxadc_get_rawdata(channel) * 1500000 / 4096);
}
