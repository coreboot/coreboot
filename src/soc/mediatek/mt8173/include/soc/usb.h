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

#ifndef SOC_MEDIATEK_MT8173_USB_H
#define SOC_MEDIATEK_MT8173_USB_H

/* ip_pw_ctrl0 */
#define CTRL0_IP_SW_RST			(0x1 << 0)

/* ip_pw_ctrl1 */
#define CTRL1_IP_HOST_PDN		(0x1 << 0)

/* ip_pw_sts1 */
#define STS1_U3_MAC_RST		(0x1 << 16)
#define STS1_SYS125_RST		(0x1 << 10)
#define STS1_REF_RST		(0x1 << 8)
#define STS1_SYSPLL_STABLE	(0x1 << 0)

/* ip_pw_sts2 */
#define STS2_U2_MAC_RST	(0x1 << 0)

/* ip_xhci_cap */
#define CAP_U3_PORT_NUM(p)	((p) & 0xff)
#define CAP_U2_PORT_NUM(p)	(((p) >> 8) & 0xff)

/* u3_ctrl_p */
#define CTRL_U3_PORT_HOST_SEL	(0x1 << 2)
#define CTRL_U3_PORT_PDN		(0x1 << 1)
#define CTRL_U3_PORT_DIS		(0x1 << 0)

/* u2_ctrl_p */
#define CTRL_U2_PORT_HOST_SEL	(0x1 << 2)
#define CTRL_U2_PORT_PDN		(0x1 << 1)
#define CTRL_U2_PORT_DIS		(0x1 << 0)

struct ssusb_ippc_regs {
	u32 ip_pw_ctr0;
	u32 ip_pw_ctr1;
	u32 ip_pw_ctr2;
	u32 ip_pw_ctr3;
	u32 ip_pw_sts1;
	u32 ip_pw_sts2;
	u32 reserved0[3];
	u32 ip_xhci_cap;
	u32 reserved1[2];
	u64 u3_ctrl_p[4];
	u64 u2_ctrl_p[6];
};

/* U2PHY_COM USBPHYACR5 */
#define PA5_RG_U2_HSTX_SRCTRL			(0x7 << 12)
#define PA5_RG_U2_HSTX_SRCTRL_VAL(x)	((0x7 & (x)) << 12)
#define PA5_RG_U2_HS_100U_U3_EN			(0x1 << 11)

/* U2PHY_COM USBPHYACR6 */
#define PA6_RG_U2_ISO_EN			(0x1 << 31)
#define PA6_RG_U2_BC11_SW_EN		(0x1 << 23)
#define PA6_RG_U2_OTG_VBUSCMP_EN	(0x1 << 20)
#define PA6_RG_U2_SQTH			(0xf << 0)
#define PA6_RG_U2_SQTH_VAL(x)		((0xf & (x)) << 0)

/* U2PHY_COM U2PHYACR4 */
#define P2C_RG_USB20_GPIO_CTL	(0x1 << 9)
#define P2C_USB20_GPIO_MODE		(0x1 << 8)
#define P2C_U2_GPIO_CTR_MSK  (P2C_RG_USB20_GPIO_CTL | P2C_USB20_GPIO_MODE)

/* U2PHY_COM U2PHYDTM0 */
#define P2C_FORCE_UART_EN		(0x1 << 26)
#define P2C_FORCE_DATAIN		(0x1 << 23)
#define P2C_FORCE_DM_PULLDOWN	(0x1 << 21)
#define P2C_FORCE_DP_PULLDOWN	(0x1 << 20)
#define P2C_FORCE_XCVRSEL		(0x1 << 19)
#define P2C_FORCE_SUSPENDM		(0x1 << 18)
#define P2C_FORCE_TERMSEL		(0x1 << 17)
#define P2C_RG_DATAIN			(0xf << 10)
#define P2C_RG_DATAIN_VAL(x)	((0xf & (x)) << 10)
#define P2C_RG_DMPULLDOWN		(0x1 << 7)
#define P2C_RG_DPPULLDOWN		(0x1 << 6)
#define P2C_RG_XCVRSEL			(0x3 << 4)
#define P2C_RG_XCVRSEL_VAL(x)	((0x3 & (x)) << 4)
#define P2C_RG_SUSPENDM			(0x1 << 3)
#define P2C_RG_TERMSEL			(0x1 << 2)
#define P2C_DTM0_PART_MASK \
		(P2C_FORCE_DATAIN | P2C_FORCE_DM_PULLDOWN | \
		P2C_FORCE_DP_PULLDOWN | P2C_FORCE_XCVRSEL | \
		P2C_FORCE_TERMSEL | P2C_RG_DMPULLDOWN | \
		P2C_RG_TERMSEL)

/* U2PHY_COM U2PHYDTM1 */
#define P2C_RG_UART_EN		(0x1 << 16)
#define P2C_RG_VBUSVALID	(0x1 << 5)
#define P2C_RG_SESSEND		(0x1 << 4)
#define P2C_RG_AVALID		(0x1 << 2)

/* U3PHYA PHYA_REG0 */
#define P3A_RG_U3_VUSB10_ON			(1 << 5)

/* U3PHYA PHYA_REG6 */
#define P3A_RG_TX_EIDLE_CM			(0xf << 28)
#define P3A_RG_TX_EIDLE_CM_VAL(x)	((0xf & (x)) << 28)

/* U3PHYA PHYA_REG9 */
#define P3A_RG_RX_DAC_MUX			(0x1f << 1)
#define P3A_RG_RX_DAC_MUX_VAL(x)	((0x1f & (x)) << 1)

/* U3PHYA_DA REG0 */
#define P3A_RG_XTAL_EXT_EN_U3			(0x3 << 10)
#define P3A_RG_XTAL_EXT_EN_U3_VAL(x)	((0x3 & (x)) << 10)

/* U3PHYD CDR1 */
#define P3D_RG_CDR_BIR_LTD1				(0x1f << 24)
#define P3D_RG_CDR_BIR_LTD1_VAL(x)		((0x1f & (x)) << 24)
#define P3D_RG_CDR_BIR_LTD0				(0x1f << 8)
#define P3D_RG_CDR_BIR_LTD0_VAL(x)		((0x1f & (x)) << 8)

struct sif_u2_phy_com {
	u32 reserved0[5];
	u32 usbphyacr5;
	u32 usbphyacr6;
	u32 u2phyacr3;
	u32 u2phyacr4;
	u32 reserved1[17];
	u32 u2phydtm0;
	u32 u2phydtm1;
	u32 reserved2[36];  /* 0x70 - 0xff */
};
check_member(sif_u2_phy_com, u2phydtm0, 0x68);

struct sif_u3phyd {
	u32 reserved0[23];
	u32 phyd_cdr1;
	u32 reserved1[40];
};

struct sif_u3phya {
	u32 phya_reg0;
	u32 reserved0[5];
	u32 phya_reg6;
	u32 reserved1[2];
	u32 phya_reg9;
	u32 reserved2[54];
};

struct sif_u3phya_da {
	u32 reg0;
	u32 reserved[63];
};

struct ssusb_sif_port {
	struct sif_u2_phy_com u2phy;
	struct sif_u3phyd u3phyd;
	u32 reserved0[64];
	struct sif_u3phya u3phya;
	struct sif_u3phya_da u3phya_da;
	u32 reserved1[64 * 3];
};
check_member(ssusb_sif_port, u3phyd, 0x100);
check_member(ssusb_sif_port, u3phya, 0x300);
check_member(ssusb_sif_port, u3phya_da, 0x400);
check_member(ssusb_sif_port, reserved1, 0x500);
_Static_assert(sizeof(struct ssusb_sif_port) == 0x800, \
	"ssusb_sif_port size is wrong!");

void setup_usb_host(void);

#endif
