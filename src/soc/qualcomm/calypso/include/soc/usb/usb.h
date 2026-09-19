/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef __SOC_QUALCOMM_CALYPSO_USB_H__
#define __SOC_QUALCOMM_CALYPSO_USB_H__

#include "qmp_usb_phy.h"

/* QSCRATCH_GENERAL_CFG register bit offset */
#define PIPE_UTMI_CLK_SEL			BIT(0)
#define PIPE3_PHYSTATUS_SW			BIT(3)
#define PIPE_UTMI_CLK_DIS			BIT(8)

/* Global USB3 Control  Registers */
#define DWC3_GUSB3PIPECTL_DELAYP1TRANS		BIT(18)
#define DWC3_GUSB3PIPECTL_PHYSOFTRS		BIT(31)
#define DWC3_GUSB3PIPECTL_UX_EXIT_IN_PX		BIT(27)
#define DWC3_GUSB3PIPECTL_P3EXSIGP2		BIT(10)
#define DWC3_GCTL_PRTCAPDIR(n)			((n) << 12)
#define DWC3_GCTL_PRTCAP_OTG			3
#define DWC3_GCTL_PRTCAP_HOST			1

/* Global USB2 PHY Configuration Register */
#define DWC3_GUSB2PHYCFG_USBTRDTIM(n)		((n) << 10)
#define DWC3_GUSB2PHYCFG_USB2TRDTIM_MASK	DWC3_GUSB2PHYCFG_USBTRDTIM(0xf)
#define DWC3_GUSB2PHYCFG_PHYIF(n)		((n) << 3)
#define DWC3_GUSB2PHYCFG_PHYIF_MASK		DWC3_GUSB2PHYCFG_PHYIF(1)
#define USBTRDTIM_UTMI_8_BIT			9
#define UTMI_PHYIF_8_BIT			0
#define DWC3_GUSB2PHYCFG_ENBLSLPM_MASK	        (0x1 << 0x8)
#define DWC3_GCTL_SCALEDOWN(n)			((n) << 4)
#define DWC3_GCTL_SCALEDOWN_MASK		DWC3_GCTL_SCALEDOWN(3)
#define DWC3_GCTL_DISSCRAMBLE			(1 << 3)
#define DWC3_GCTL_U2EXIT_LFPS			(1 << 2)
#define DWC3_GCTL_DSBLCLKGTNG			(1 << 0)

/* Global RX Threshold Configuration Register */
#define DWC3_GRXTHRCFG_USBMAXRXBURSTSIZE_SHFT	19
#define DWC3_GRXTHRCFG_USBRXPKTCNT_SHFT		24
#define DWC3_GRXTHRCFG_USBRXPKTCNTSEL		BIT(29)
#define DWC3_GRXTHRCFG_USBMAXRXBURSTSIZE(n)	((n) << DWC3_GRXTHRCFG_USBMAXRXBURSTSIZE_SHFT)
#define DWC3_GRXTHRCFG_USBRXPKTCNT(n)		((n) << DWC3_GRXTHRCFG_USBRXPKTCNT_SHFT)

/* Global SBus Configuration 1 Register */
#define DWC3_GSBUSCFG1_PIPETRANSLIMIT_SHFT	8
#define DWC3_GSBUSCFG1_EN1KPAGE			BIT(12)
#define DWC3_GSBUSCFG1_PIPETRANSLIMIT(n)	((n) << DWC3_GSBUSCFG1_PIPETRANSLIMIT_SHFT)

/* Global User Control Register */
#define DWC3_GUCTL_SPRSCTRLTRANSEN		BIT(17)

/* USB3_MP_GENERAL_CFG (0xA4F8808) port 0 bits */
#define UTMI_CLK_SEL_0				(1 << 0)   /* bit  0: select UTMI clk for port 0 */
#define PIPE3_PHYSTATUS_SW_0			(1 << 3)   /* bit  3: SW override PHYSTATUS port 0 */
#define UTMI_CLK_DIS_0				(1 << 8)   /* bit  8: disable PIPE clk for port 0 */
#define PIPE3_SET_PHYSTATUS_SW_0		(1 << 9)   /* bit  9: assert PHYSTATUS SW port 0 */
/* USB3_MP_GENERAL_CFG (0xA4F8808) port 1 bits */
#define UTMI_CLK_SEL_1				(1 << 16)  /* bit 16: select UTMI clk for port 1 */
#define PIPE3_PHYSTATUS_SW_1			(1 << 19)  /* bit 19: SW override PHYSTATUS port 1 */
#define UTMI_CLK_DIS_1				(1 << 24)  /* bit 24: disable PIPE clk for port 1 */
#define PIPE3_SET_PHYSTATUS_SW_1		(1 << 25)  /* bit 25: assert PHYSTATUS SW port 1 */
#define USB3_MP_CGCTL_REG_ADDR			((void *)0x0A4F8828)

#define USB3_CGCTL_DBM_FSM_EN_BIT		(1 << 1)
#define USB3_MP_LINK_REGS_1_LU3LFPSRXTIM_ADDR	((void *)0X0A40D090)
#define USB3_MP_LINK_REGS_0_LU3LFPSRXTIM_ADDR	((void *)0X0A40D010)

/*
 * USB3 MP LLUCTL (Link Layer USB Control) register addresses.
 *   Port 0: USB3_MP_DWC_USB3_BASE + 0xD024 = 0x0A40D024
 *   Port 1: USB3_MP_DWC_USB3_BASE + 0xD0A4 = 0x0A40D0A4
 * FORCE_GEN1 (bit 10): forces the link to operate at Gen1 (5 Gbps).
 */
#define USB3_MP_LINK_REGS_0_LLUCTL_ADDR		((void *)0x0A40D024)
#define USB3_MP_LINK_REGS_1_LLUCTL_ADDR		((void *)0x0A40D0A4)
#define USB3_MP_LLUCTL_FORCE_GEN1_BIT		BIT(10)

#define GEN2_U3_EXIT_RSP_RX_CLK_MASK			(0xFF << 16)
#define GEN1_U3_EXIT_RSP_RX_CLK_MASK			(0xFF << 0)

#define GEN2_U3_EXIT_RSP_RX_CLK_VALUE			(0x06 << 16)
#define GEN1_U3_EXIT_RSP_RX_CLK_VALUE			(0x05 << 0)

#define LFPS_RSP_RX_CLK_CLR_MASK				(GEN2_U3_EXIT_RSP_RX_CLK_MASK | GEN1_U3_EXIT_RSP_RX_CLK_MASK)
#define LFPS_RSP_RX_CLK_SET_MASK				(GEN2_U3_EXIT_RSP_RX_CLK_VALUE | GEN1_U3_EXIT_RSP_RX_CLK_VALUE)

#define DWC3_GUCTL1_CLR_MASK	(BIT(31) | BIT(24) | (0x7 << 21))
#define DWC3_GUCTL1_DEV_L1_EXIT_BY_HW		BIT(24)
#define DWC3_GUCTL1_IP_GAP_ADD_ON(val)		((val & 0x7) << 21)
#define DWC3_GUCTL1_DEV_DECOUPLE_L1L2_EVT	BIT(31)

#define DWC3_GUCTL1_SET_MASK	(DWC3_GUCTL1_DEV_L1_EXIT_BY_HW | \
								DWC3_GUCTL1_IP_GAP_ADD_ON(0x3) | \
								DWC3_GUCTL1_DEV_DECOUPLE_L1L2_EVT)

/*
 * DWC3 GUCTL3 register offset from the usb_dwc3 struct base (DWC3 global regs at 0xC100).
 * USB20_RETRY_DISABLE (bit 14): disables the USB2.0 internal retry mechanism.
 */
#define DWC3_GUCTL3_OFFSET			0x50C
#define DWC3_GUCTL3_USB20_RETRY_DISABLE		BIT(14)

#define USB_HOST_DWC3_MP_GENERAL_CFG_ADDR		0X0A4F8808
#define USB3_MP_GUSB2PHYCFG_REGS_1_ADDR		((void *)0x0A40C204)
#define GUSB2PHYCFG_ENBLSLPM_BIT		BIT(8)
#define USB3_MP_GUSB2PHYCFG_REGS_0_ADDR		((void *)0x0A40C200)
/* PORTSC addresses (USB3 MP DWC3 IP base: 0x0A400000) */
#define USB3_MP_PORTSC_20_REGS_0_ADDR	((void *)0x0A400430)
#define USB3_MP_PORTSC_20_REGS_1_ADDR	((void *)0x0A400440)
#define USB3_MP_PORTSC_30_REGS_0_ADDR	((void *)0x0A400450)
#define USB3_MP_PORTSC_30_REGS_1_ADDR	((void *)0x0A400460)
#define USB3_PORTSC_WCE_BIT				BIT(25)

/* USB MP eUSB repeater reset GPIOs (active-low, output high = deassert reset) */
#define GPIO_EUSB3_RESET_N	GPIO(8)		/* MP0 eUSB repeater, active-low */
#define GPIO_EUSB6_RESET_N	GPIO(184)	/* MP1 eUSB repeater, active-low */

/* PMCX0103 (Fury4I) PMIC_K: USB Type-A VBUS enable GPIOs */
#define PMIC_K_SID		11	/* PMIC_K slave ID on BUS_ID=1 */
#define USB3_HOST_EN_GPIO	6	/* GPIO6: USB3 + eUSB3 VBUS enable */
#define USB4_6_HOST_EN_GPIO	8	/* GPIO8: USB4 SS + eUSB6 VBUS enable */

/* USB4 SS USB3 DRD SP 0 (Primary) Register Addresses */
#define USB4_SS_USB3_DRD_SP_0_USB31_PRIMCGCTL_REG \
	((void *)0x0A6F8828)
#define USB4_SS_USB3_DRD_SP_0_USB31_PRIMLINK_REGS_0_LU3LFPSRXTIM_ADDR \
	((void *)0x0A61D010)
#define USB4_SS_USB3_DRD_SP_0_USB31_PRIMGUSB2PHYCFG_REGS_0_GUSB2PHYCFG_ADDR \
	((void *)0x0A60C200)
#define USB4_SS_USB3_DRD_SP_0_USB31_PRIMPORTSC_20_REGS_0_PORTSC_20_ADDR \
	((void *)0x0A600420)
#define USB4_SS_USB3_DRD_SP_0_USB31_PRIMPORTSC_30_REGS_0_PORTSC_30_ADDR \
	((void *)0x0A600430)
#define USB4_SS_USB3_DRD_SP_0_USB31_PRIMGENERAL_CFG	0xA6F8808
/* USB4 SS USB3 DRD SP 1 (Secondary) Register Addresses */
#define USB4_SS_USB3_DRD_SP_1_USB31_SECCGCTL_REG \
	((void *)0x0A8F8828)
#define USB4_SS_USB3_DRD_SP_1_USB31_SECLINK_REGS_0_LU3LFPSRXTIM_ADDR \
	((void *)0x0A81D010)
#define USB4_SS_USB3_DRD_SP_1_USB31_SECGUSB2PHYCFG_REGS_0_GUSB2PHYCFG_ADDR \
	((void *)0x0A80C200)
#define USB4_SS_USB3_DRD_SP_1_USB31_SECPORTSC_20_REGS_0_PORTSC_20_ADDR \
	((void *)0x0A800420)
#define USB4_SS_USB3_DRD_SP_1_USB31_SECPORTSC_30_REGS_0_PORTSC_30_ADDR \
	((void *)0x0A800430)
#define USB4_SS_USB3_DRD_SP_1_USB31_SECGENERAL_CFG	0xA8F8808
/* USB4 SS USB3 DRD SP 2 (Tertiary) Register Addresses */
#define USB4_SS_USB3_DRD_SP_2_USB31_TERTCGCTL_REG \
	((void *)0x0A0F8828)
#define USB4_SS_USB3_DRD_SP_2_USB31_TERTLINK_REGS_0_LU3LFPSRXTIM_ADDR \
	((void *)0x0A00D010)
#define USB4_SS_USB3_DRD_SP_2_USB31_TERTGUSB2PHYCFG_REGS_0_GUSB2PHYCFG_ADDR \
	((void *)0x0A00C200)
#define USB4_SS_USB3_DRD_SP_2_USB31_TERTPORTSC_20_REGS_0_PORTSC_20_ADDR \
	((void *)0x0A000430)
#define USB4_SS_USB3_DRD_SP_2_USB31_TERTPORTSC_30_REGS_0_PORTSC_30_ADDR \
	((void *)0x0A000440)
#define USB4_SS_USB3_DRD_SP_2_USB31_TERTGENERAL_CFG	0xA0F8808

/* Type-C register offsets */
#define SCHG_TYPE_C_TYPE_C_MISC_STATUS		0x2B0B	/* CC polarity, VBUS status */
#define SCHG_TYPE_C_TYPE_C_SRC_STATUS		0x2B08	/* Source detection status */
#define SCHG_TYPE_C_STATE_MACHINE_STATUS	0x2B09	/* Current state machine state */
#define SCHG_TYPE_C_CC_STATUS			0x2B0E	/* CC connection state */
#define SCHG_TYPE_C_U_USB_STATUS		0x2B0F	/* USB active status */
#define SCHG_TYPE_C_INT_RT_STS			0x2B10	/* Interrupt real-time status */
#define SCHG_TYPE_C_TYPE_C_MODE_CFG		0x2B44	/* Mode configuration */
#define CCOUT_INVERT_POLARITY			0x03

/* USB Repeater SPMI Tune register offsets */
#define EUSB2_TUNE_IUSB2		0xFD51
#define EUSB2_TUNE_HSDISC		0xFD53	/* HS disconnect threshold */
#define EUSB2_TUNE_SQUELCH_U		0xFD54	/* Squelch threshold */
#define EUSB2_TUNE_USB2_SLEW		0xFD55	/* HS TX slew rate */
#define EUSB2_TUNE_USB2_PREEM		0xFD57	/* HS TX pre-emphasis */
#define EUSB2_TUNE_EUSB2_EQU		0xFD5A	/* eUSB2 equalization */
#define EUSB2_TUNE_EHS_COMP_CURRENT	0xFD5B	/* EHS comparator current */

/* USB 2.0 tuning parameter values (from board repeater_cfg table) */
#define EUSB2_TUNE_IUSB2_DEFAULT		0x0B
#define EUSB2_TUNE_HSDISC_VAL			0x07
#define EUSB2_TUNE_SQUELCH_U_VAL		0x03
#define EUSB2_TUNE_USB2_SLEW_FAST		0x05
#define EUSB2_TUNE_USB2_PREEM_25PCT		0x05
#define EUSB2_TUNE_EUSB2_EQU_VAL		0x03
#define EUSB2_TUNE_EHS_COMP_CURRENT_VAL		0x03

/* USB Shared SPMI Init register offsets */
#define EUSB2_EN_CTL1			0xFD46
#define EUSB2_EN_CTL1_ENABLE		BIT(7)
#define EUSB2_EN_CTL1_DISABLE		0x00
#define EUSB2_RPTR_STATUS		0xFD08
#define EUSB2_RPTR_INFRA_STATUS		0xFD09

/* SMB2370 (Barrington) charger Slave IDs on eSPMI Bus E2 (Glymur PMIC table) */
#define SMB1_SLAVE_ID 0x09	/* Port-0, eSPMI E2 Slave 9 (J_2) */
#define SMB2_SLAVE_ID 0x0A	/* Port-1, eSPMI E2 Slave 10 (K_2) */
#define SMB3_SLAVE_ID	0x0B
/* Initialize HS USB PHY for the given port index */
void hs_usb_phy_init(int index);
/* Initializes and configures the USB HOST0 controller */
void setup_usb_host0(void);

#endif /* __SOC_QUALCOMM_CALYPSO_USB_H__ */
