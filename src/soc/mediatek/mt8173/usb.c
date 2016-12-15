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

#include <arch/io.h>
#include <console/console.h>
#include <delay.h>
#include <soc/addressmap.h>
#include <soc/pll.h>
#include <soc/usb.h>
#include <timer.h>

#define USBTAG	"[SSUSB] "
#define u3p_msg(fmt, arg...)   printk(BIOS_INFO, USBTAG fmt, ##arg)
#define u3p_err(fmt, arg...)   printk(BIOS_ERR, USBTAG fmt, ##arg)

static struct ssusb_ippc_regs *ippc_regs = (void *)(SSUSB_IPPC_BASE);
static struct ssusb_sif_port *phy_ports = (void *)(SSUSB_SIF_BASE);

static void phy_index_power_on(int index)
{
	struct ssusb_sif_port *phy = phy_ports + index;

	if (!index) {
		/* Set RG_SSUSB_VUSB10_ON as 1 after VUSB10 ready */
		setbits_le32(&phy->u3phya.phya_reg0, P3A_RG_U3_VUSB10_ON);
		/* power domain iso disable */
		clrbits_le32(&phy->u2phy.usbphyacr6, PA6_RG_U2_ISO_EN);
	}
	/* switch to USB function. (system register, force ip into usb mode) */
	clrbits_le32(&phy->u2phy.u2phydtm0, P2C_FORCE_UART_EN);
	clrbits_le32(&phy->u2phy.u2phydtm1, P2C_RG_UART_EN);
	if (!index)
		clrbits_le32(&phy->u2phy.u2phyacr4, P2C_U2_GPIO_CTR_MSK);

	/* (force_suspendm=0) (let suspendm=1, enable usb 480MHz pll) */
	clrbits_le32(&phy->u2phy.u2phydtm0, P2C_FORCE_SUSPENDM |
		P2C_RG_XCVRSEL | P2C_RG_DATAIN | P2C_DTM0_PART_MASK);

	/* DP/DM BC1.1 path Disable */
	clrbits_le32(&phy->u2phy.usbphyacr6, PA6_RG_U2_BC11_SW_EN);
	/* improve Rx sensitivity */
	clrsetbits_le32(&phy->u2phy.usbphyacr6,
		PA6_RG_U2_SQTH, PA6_RG_U2_SQTH_VAL(2));
	/* OTG Enable */
	setbits_le32(&phy->u2phy.usbphyacr6, PA6_RG_U2_OTG_VBUSCMP_EN);

	clrsetbits_le32(&phy->u3phya_da.reg0,
		P3A_RG_XTAL_EXT_EN_U3, P3A_RG_XTAL_EXT_EN_U3_VAL(2));

	clrsetbits_le32(&phy->u3phya.phya_reg9,
		P3A_RG_RX_DAC_MUX, P3A_RG_RX_DAC_MUX_VAL(4));

	if (!index) {
		/* [mt8173]disable Change 100uA current from SSUSB */
		clrbits_le32(&phy->u2phy.usbphyacr5, PA5_RG_U2_HS_100U_U3_EN);
	}

	clrsetbits_le32(&phy->u3phya.phya_reg6,
		P3A_RG_TX_EIDLE_CM, P3A_RG_TX_EIDLE_CM_VAL(0xe));

	clrsetbits_le32(&phy->u3phyd.phyd_cdr1,
		P3D_RG_CDR_BIR_LTD0, P3D_RG_CDR_BIR_LTD0_VAL(0xc));
	clrsetbits_le32(&phy->u3phyd.phyd_cdr1,
		P3D_RG_CDR_BIR_LTD1, P3D_RG_CDR_BIR_LTD1_VAL(0x3));

	clrsetbits_le32(&phy->u2phy.u2phydtm1,
		P2C_RG_SESSEND, P2C_RG_VBUSVALID | P2C_RG_AVALID);

	/* USB 2.0 slew rate calibration */
	clrsetbits_le32(&phy->u2phy.usbphyacr5,
		PA5_RG_U2_HSTX_SRCTRL, PA5_RG_U2_HSTX_SRCTRL_VAL(4));
}

static void u3phy_power_on(void)
{
	phy_index_power_on(0);
	phy_index_power_on(1);
}

static int check_ip_clk_status(void)
{
	int u3_port_num;
	u32 check_bits;
	u32 sts1, sts2;
	struct stopwatch sw;

	u3_port_num = CAP_U3_PORT_NUM(read32(&ippc_regs->ip_xhci_cap));

	check_bits = STS1_SYSPLL_STABLE | STS1_REF_RST | STS1_SYS125_RST;
	check_bits |= (u3_port_num ? STS1_U3_MAC_RST : 0);

	stopwatch_init_usecs_expire(&sw, 50000);

	do {
		if (stopwatch_expired(&sw)) {
			u3p_err("usb clocks are not stable!!!\n");
			return -1;
		}

		sts1 = read32(&ippc_regs->ip_pw_sts1) & check_bits;
		sts2 = read32(&ippc_regs->ip_pw_sts2) & STS2_U2_MAC_RST;
	} while ((sts1 != check_bits) || !sts2);

	return 0;
}

static int u3phy_ports_enable(void)
{
	int i;
	u32 value;
	int u3_port_num;
	int u2_port_num;

	value = read32(&ippc_regs->ip_xhci_cap);
	u3_port_num = CAP_U3_PORT_NUM(value);
	u2_port_num = CAP_U2_PORT_NUM(value);
	u3p_msg("%s u2p:%d, u3p:%d\n", __func__, u2_port_num, u3_port_num);

	/* power on host ip */
	clrbits_le32(&ippc_regs->ip_pw_ctr1, CTRL1_IP_HOST_PDN);

	/* power on and enable all u3 ports */
	for (i = 0; i < u3_port_num; i++) {
		clrsetbits_le32(&ippc_regs->u3_ctrl_p[i],
			CTRL_U3_PORT_PDN | CTRL_U3_PORT_DIS, CTRL_U3_PORT_HOST_SEL);
	}

	/* power on and enable all u2 ports */
	for (i = 0; i < u2_port_num; i++) {
		clrsetbits_le32(&ippc_regs->u2_ctrl_p[i],
			CTRL_U2_PORT_PDN | CTRL_U2_PORT_DIS, CTRL_U2_PORT_HOST_SEL);
	}
	return check_ip_clk_status();
}

static inline void ssusb_soft_reset(void)
{
	/* reset whole ip */
	setbits_le32(&ippc_regs->ip_pw_ctr0, CTRL0_IP_SW_RST);
	clrbits_le32(&ippc_regs->ip_pw_ctr0, CTRL0_IP_SW_RST);
}

void setup_usb_host(void)
{
	int ret;

	u3p_msg("Setting up USB HOST controller...\n");

	mt_pll_enable_ssusb_clk();
	ssusb_soft_reset();
	ret = u3phy_ports_enable();
	if (ret) {
		u3p_err("%s fail to enable ports\n", __func__);
		return;
	}
	u3phy_power_on();
	u3p_msg("phy power-on done.\n");
}
