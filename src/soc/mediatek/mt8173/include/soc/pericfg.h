/*
 * This file is part of the coreboot project.
 *
 * Copyright 2015 MediaTek Inc.
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

#ifndef __SOC_MEDIATEK_MT8173_PERICFG_H__
#define __SOC_MEDIATEK_MT8173_PERICFG_H__

#include <soc/addressmap.h>
#include <types.h>

struct mt8173_pericfg_regs {
	u32 rst0;
	u32 rst1;
	u32 pdn0_set;
	u32 pdn1_set;
	u32 pdn0_clr;
	u32 pdn1_clr;
	u32 pdn0_sta;
	u32 pdn1_sta;
	u32 pdn_md1_set;
	u32 pdn_md2_set;
	u32 pdn_md1_clr;
	u32 pdn_md2_clr;
	u32 pdn_md1_sta;
	u32 pdn_md2_sta;
	u32 pdn_md_mask;
	u32 reserved0[5];
	u32 dcmctl;
	u32 dcmdbc;
	u32 dcmfsel;
	u32 cksel;
	u32 reserved1[104];
	u32 axi_bus_ctl1;
	u32 axi_bus_ctl2;
	u32 axi_bus_ctl3;
	u32 axi_si0_ctl;
	u32 axi_si1_ctl;
	u32 axi_mi_sta;
	u32 reserved2[58];
	u32 axi_ahb_lmt_con1;
	u32 axi_ahb_lmt_con2;
	u32 axi_ahb_lmt_con3;
	u32 axi_ahb_lmt_con4;
	u32 axi_ahb_lmt_con5;
	u32 axi_ahb_lmt_con6;
	u32 reserved3[2];
	u32 axi_axi_lmt_con1;
	u32 axi_axi_lmt_con2;
	u32 axi_axi_lmt_con3;
	u32 axi_axi_lmt_con4;
	u32 axi_axi_lmt_con5;
	u32 axi_axi_lmt_con6;
	u32 axi_axi_lmt_con7;
	u32 axi_axi_lmt_con8;
	u32 axi_axi_lmt_con9;
	u32 reserved4[47];
	u32 usb_wakeup_dec_con0;
	u32 usb_wakeup_dec_con1;
	u32 usb_wakeup_dec_con2;
	u32 uart_ck_source_sel;
	u32 reserved5[1];
	u32 usb_wakeup_dec_con3a;
	u32 reserved6[58];
	u32 ssusb_rst;
	u32 ssusb_pdn_set;
	u32 ssusb_pdn_clr;
	u32 ssusb_pdn_sta;
};

static struct mt8173_pericfg_regs *const mt8173_pericfg =
					(void *)PERI_CON_BASE;

/*
 * UART power down control
 */

enum {
        PERICFG_UART0_PDN = 1 << 19
};

/*
 * PERI 4GB control
 */

enum {
        PERISYS_4G_SUPPORT = 1 << 15
};

#endif	/* __SOC_MEDIATEK_MT8173_PERICFG_H__ */
