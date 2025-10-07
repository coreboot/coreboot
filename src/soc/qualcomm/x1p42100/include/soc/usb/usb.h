/* SPDX-License-Identifier: GPL-2.0-only */

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

#define UTMI_CLK_DIS_0					(1 << 8)
#define UTMI_CLK_SEL_0					(1 << 0)
#define PIPE3_PHYSTATUS_SW_0			(1 << 3)
#define PIPE3_SET_PHYSTATUS_SW_0		(1 << 9)
#define USB3_MP_CGCTL_REG_ADDR			((void *)0x0A4F8828)

#define USB3_CGCTL_DBM_FSM_EN_BIT		(1 << 1)
#define USB3_MP_LINK_REGS_1_LU3LFPSRXTIM_ADDR	((void *)0X0A40D090)
#define USB3_MP_LINK_REGS_0_LU3LFPSRXTIM_ADDR	((void *)0X0A40D010)

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

#define USB_HOST_DWC3_MP_GENERAL_CFG_ADDR		0X0A4F8808
#define USB3_MP_GUSB2PHYCFG_REGS_1_ADDR		((void *)0x0A40C204)
#define GUSB2PHYCFG_ENBLSLPM_BIT		BIT(8)
#define USB3_MP_GUSB2PHYCFG_REGS_0_ADDR		((void *)0x0A40C200)
#define USB3_MP_PORTSC_20_REGS_0_ADDR	((void *)0x0A400420)
#define USB3_MP_PORTSC_20_REGS_1_ADDR	((void *)0x0A400430)
#define USB3_MP_PORTSC_30_REGS_0_ADDR	((void *)0x0A400440)
#define USB3_MP_PORTSC_30_REGS_1_ADDR	((void *)0x0A400450)
#define USB3_PORTSC_WCE_BIT				BIT(25)

/* USB4 SS USB3 DRD SP 0 (Primary) Register Addresses */
#define USB4_SS_USB3_DRD_SP_0_USB31_PRIMCGCTL_REG	((void *)0x0A6F8828)
#define USB4_SS_USB3_DRD_SP_0_USB31_PRIMLINK_REGS_0_LU3LFPSRXTIM_ADDR	((void *)0x0A61D010)
#define USB4_SS_USB3_DRD_SP_0_USB31_PRIMGUSB2PHYCFG_REGS_0_GUSB2PHYCFG_ADDR	((void *)0x0A60C200)
#define USB4_SS_USB3_DRD_SP_0_USB31_PRIMPORTSC_20_REGS_0_PORTSC_20_ADDR	((void *)0x0A600420)
#define USB4_SS_USB3_DRD_SP_0_USB31_PRIMPORTSC_30_REGS_0_PORTSC_30_ADDR	((void *)0x0A600430)
#define USB4_SS_USB3_DRD_SP_0_USB31_PRIMGENERAL_CFG 0xA6F8808
/* USB4 SS USB3 DRD SP 1 (Secondary) Register Addresses */
#define USB4_SS_USB3_DRD_SP_1_USB31_SECCGCTL_REG	((void *)0x0A8F8828)
#define USB4_SS_USB3_DRD_SP_1_USB31_SECLINK_REGS_0_LU3LFPSRXTIM_ADDR	((void *)0x0A81D010)
#define USB4_SS_USB3_DRD_SP_1_USB31_SECGUSB2PHYCFG_REGS_0_GUSB2PHYCFG_ADDR	((void *)0x0A80C200)
#define USB4_SS_USB3_DRD_SP_1_USB31_SECPORTSC_20_REGS_0_PORTSC_20_ADDR	((void *)0x0A800420)
#define USB4_SS_USB3_DRD_SP_1_USB31_SECPORTSC_30_REGS_0_PORTSC_30_ADDR	((void *)0x0A800430)
#define USB4_SS_USB3_DRD_SP_1_USB31_SECGENERAL_CFG 0xA8F8808

/* VBUS SS Enable definitions - using same slave IDs as battery charging */
#define SMB1_SLAVE_ID 0x07
#define SMB2_SLAVE_ID 0x0A

/* SCHG DCDC register offsets */
#define SCHG_DCDC_CMD_OTG 0x2740
#define SCHG_DCDC_OTG_CFG 0x2753
#define SCHG_DCDC_OTG_STATUS 0x270D

/* OTG Status register bit definitions */
#define OTG_STATE_MASK 0x07
#define OTG_STATE_DISABLED 0x00
#define OTG_STATE_ENABLING 0x01
#define OTG_STATE_ENABLED 0x02
#define OTG_STATE_DISABLING 0x03
#define OTG_STATE_ERROR 0x04

/* OTG Status check timeout and polling interval */
#define OTG_STATUS_TIMEOUT_MS 100
#define OTG_STATUS_POLL_INTERVAL_MS 2

/* Type-C register offsets */
#define SCHG_TYPE_C_TYPE_C_MISC_STATUS 0x2B0B
#define SCHG_TYPE_C_TYPE_C_SRC_STATUS 0x2B08
#define SCHG_TYPE_C_TYPE_C_MODE_CFG 0x2B44
#define TYPEC_VBUS_STATUS_MASK BIT(5)

/* Forward declaration */
struct dwc3_controller_config;

/* Initializes a specific HS PHY instance */
void hs_usb_phy_init(int index);
/* Initializes and configures the USB HOST0 controller */
void setup_usb_host0(void);
/* Enable USB GDSC/Clocks */
int qcom_enable_usb_clk(void);
/* Enable TCSR REFGEN */
void enable_clock_tcsr(void);
/* Enable TCSR CLKREF */
void usb_update_refclk_for_core(u32 core_num, bool enable);
/* Enables VBUS SuperSpeed for specified USB core */
void enable_vbus_ss(const struct dwc3_controller_config *config);
/* Reads comprehensive Type-C status from PMIC */
void usb_typec_status_check(const struct dwc3_controller_config *config);
