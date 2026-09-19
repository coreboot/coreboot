/* SPDX-License-Identifier: GPL-2.0-only */

#include <console/console.h>
#include <delay.h>
#include <device/mmio.h>
#include <gpio.h>
#include <soc/addressmap.h>
#include <soc/clock.h>
#include <soc/pmic_gpio.h>
#include <soc/qcom_spmi.h>
#include <soc/usb/usb.h>

#define USB_HW_STABILIZE_DELAY_US	10

/*
 * HS (eUSB2) PHY instance indices passed to hs_usb_phy_init(). The Type-A (MP)
 * PHYs come first, followed by the three Type-C PHYs.
 */
enum hs_usb_phy_index {
	HS_PHY_MP0,
	HS_PHY_MP1,
	HS_PHY_SS0,
	HS_PHY_SS1,
	HS_PHY_SS2,
};

/*
 * Core numbers accepted by usb_update_refclk_for_core(). Cores 0-2 are the
 * Type-C ports, core 3 covers both Type-A (MP) ports.
 */
enum usb_refclk_core {
	USB_REFCLK_CORE_PRIM,
	USB_REFCLK_CORE_SEC,
	USB_REFCLK_CORE_TERT,
	USB_REFCLK_CORE_MP,
};

static void usb_update_refclk_for_core(u32 core_num, bool enable);

struct usb_dwc3 {
	u32 sbuscfg0;
	u32 sbuscfg1;
	u32 txthrcfg;
	u32 rxthrcfg;
	u32 ctl;
	u32 pmsts;
	u32 sts;
	u32 uctl1;
	u32 snpsid;
	u32 gpio;
	u32 uid;
	u32 uctl;
	u64 buserraddr;
	u64 prtbimap;
	u8 reserved1[32];
	u32 dbgfifospace;
	u32 dbgltssm;
	u32 dbglnmcc;
	u32 dbgbmu;
	u32 dbglspmux;
	u32 dbglsp;
	u32 dbgepinfo0;
	u32 dbgepinfo1;
	u64 prtbimap_hs;
	u64 prtbimap_fs;
	u8 reserved2[112];
	u32 usb2phycfg;
	u32 usb2phycfg_mp1;
	u8 reserved3[120];
	u32 usb2phyacc;
	u8 reserved4[60];
	u32 usb3pipectl;
	u32 usb3pipectl_mp1;
	u8 reserved5[56];
};
check_member(usb_dwc3, usb2phycfg_mp1, 0x104);
check_member(usb_dwc3, usb3pipectl_mp1, 0x1c4);

/* Configuration for the USB30 MP (Type-A) DWC3 controller */
struct usb_dwc3_cfg {
	struct usb_dwc3 *usb_host_dwc3;
	struct usb_dwc3 *usb_host_dwc3_prim;
	struct usb_dwc3 *usb_host_dwc3_sec;
	struct usb_dwc3 *usb_host_dwc3_tert;
	u32 *usb3_bcr;
};

static struct usb_dwc3_cfg usb_ports = {
	.usb_host_dwc3 = (void *)USB_HOST_DWC3_MP_BASE,
	.usb_host_dwc3_prim = (void *)USB_HOST_DWC3_PRIM_BASE,
	.usb_host_dwc3_sec = (void *)USB_HOST_DWC3_SEC_BASE,
	.usb_host_dwc3_tert = (void *)USB_HOST_DWC3_TERT_BASE,
	.usb3_bcr = &gcc->gcc_usb30_mp_bcr,
};

static bool hs_speed_only;
static u32 *usb3_general_cfg_addr = (void *)USB_HOST_DWC3_MP_GENERAL_CFG_ADDR;

/* Configure TCSR QREFS CXO repeater/receiver registers */
static void enable_clock_tcsr(void)
{
	write32(TCSR_QREFS_CXO_RX1_CONFIG_ADDR, 0x3);
	write32(TCSR_QREFS_CXO_1_RPT0_CONFIG_ADDR, 0x3);
	write32(TCSR_QREFS_CXO_RPT1_CONFIG_ADDR, 0x3);
	write32(TCSR_QREFS_CXO_RX5_CONFIG_ADDR, 0x3);
}

/* Enable USB clocks and GDSCs for all ports (MP, PRIM, SEC, TERT) */
static enum cb_err qcom_enable_usb_clk(void)
{
	int clk, gdsc;

	/* Enable USB MP GDSCs; log failures but continue */
	for (gdsc = USB30_MP_GDSC; gdsc < MAX_USB_GDSC; gdsc++) {
		if (clock_enable_usb_gdsc(gdsc) != CB_SUCCESS)
			printk(BIOS_ERR, "Failed to enable USB GDSC %d, skipping\n", gdsc);
	}

	clock_configure_usb();

	/* Vote-gate CFG_NOC_USB_ANOC_SOUTH_AHB before enabling clocks */
	setbits32(&gcc->apcs_clk_br_en1, BIT(CFG_NOC_USB_ANOC_SOUTH_AHB_CLK_ENA));

	/* Configure MP SS PHY PIPE clocks to XO source before PHY init */
	if (usb_clock_configure_mux(USB3_PHY_PIPE_0, USB_PHY_XO_SRC_SEL) != CB_SUCCESS)
		printk(BIOS_ERR, "USB3 PHY PIPE 0: XO mux config failed\n");

	if (usb_clock_configure_mux(USB3_PHY_PIPE_1, USB_PHY_XO_SRC_SEL) != CB_SUCCESS)
		printk(BIOS_ERR, "USB3 PHY PIPE 1: XO mux config failed\n");

	/* Enable USB MP clocks; log failures but continue */
	for (clk = USB30_MP_MASTER_CBCR; clk < USB_CLK_COUNT; clk++) {
		if (usb_mp_clock_enable(clk) != CB_SUCCESS)
			printk(BIOS_ERR, "Failed to enable USB MP clock %d\n", clk);
	}

	/* Enable MP reference clocks (MP0: USB3_MP0 + USB2_2, MP1: USB3_MP1) */
	usb_update_refclk_for_core(USB_REFCLK_CORE_MP, true);

	/* Switch MP SS PHY PIPE clocks to PHY source after PHY init */
	if (usb_clock_configure_mux(USB3_PHY_PIPE_0, USB_PHY_PIPE_SRC_SEL) != CB_SUCCESS)
		printk(BIOS_ERR, "USB3 PHY PIPE 0: PHY mux config failed\n");

	if (usb_clock_configure_mux(USB3_PHY_PIPE_1, USB_PHY_PIPE_SRC_SEL) != CB_SUCCESS)
		printk(BIOS_ERR, "USB3 PHY PIPE 1: PHY mux config failed\n");

	if (usb_clock_configure_mux(USB3_PRIM_PHY_PIPE, USB_PHY_XO_SRC_SEL) != CB_SUCCESS) {
		printk(BIOS_ERR, "%s(): USB3 PRIM PHY PIPE clock XO config failed\n", __func__);
		return CB_ERR;
	}

	/* Enable USB PRIM clocks */
	for (clk = USB_PRIM_CFG_NOC_USB_ANOC_AHB_CBCR; clk < USB_PRIM_CLK_COUNT; clk++) {
		if (usb_prim_clock_enable(clk) != CB_SUCCESS) {
			printk(BIOS_ERR, "Failed to enable USB PRIM clock %d\n", clk);
			return CB_ERR;
		}
	}

	if (usb_clock_configure_mux(USB3_PRIM_PHY_PIPE, USB_PHY_PIPE_SRC_SEL) != CB_SUCCESS) {
		printk(BIOS_ERR, "%s(): USB3 PRIM PHY PIPE clock PHY config failed\n", __func__);
		return CB_ERR;
	}

	usb_update_refclk_for_core(USB_REFCLK_CORE_PRIM, true);

	if (usb_clock_configure_mux(USB3_SEC_PHY_PIPE, USB_PHY_XO_SRC_SEL) != CB_SUCCESS) {
		printk(BIOS_ERR, "%s(): USB3 SEC PHY PIPE clock XO config failed\n", __func__);
		return CB_ERR;
	}

	/* Enable USB SEC clocks */
	for (clk = USB_SEC_CFG_NOC_USB3_SEC_AXI_CBCR; clk < USB_SEC_CLK_COUNT; clk++) {
		if (usb_sec_clock_enable(clk) != CB_SUCCESS) {
			printk(BIOS_ERR, "Failed to enable USB SEC clock %d\n", clk);
			return CB_ERR;
		}
	}

	if (usb_clock_configure_mux(USB3_SEC_PHY_PIPE, USB_PHY_PIPE_SRC_SEL) != CB_SUCCESS) {
		printk(BIOS_ERR, "%s(): USB3 SEC PHY PIPE clock PHY config failed\n", __func__);
		return CB_ERR;
	}

	usb_update_refclk_for_core(USB_REFCLK_CORE_SEC, true);

	if (usb_clock_configure_mux(USB3_TERT_PHY_PIPE, USB_PHY_XO_SRC_SEL) != CB_SUCCESS) {
		printk(BIOS_ERR, "%s(): USB3 TERT PHY PIPE clock XO config failed\n", __func__);
		return CB_ERR;
	}

	/* Enable USB TERT clocks */
	for (clk = USB_TERT_CFG_NOC_USB3_TERT_AXI_CBCR; clk < USB_TERT_CLK_COUNT; clk++) {
		if (usb_tert_clock_enable(clk) != CB_SUCCESS) {
			printk(BIOS_ERR, "Failed to enable USB TERT clock %d\n", clk);
			return CB_ERR;
		}
	}

	if (usb_clock_configure_mux(USB3_TERT_PHY_PIPE, USB_PHY_PIPE_SRC_SEL) != CB_SUCCESS) {
		printk(BIOS_ERR, "%s(): USB3 TERT PHY PIPE clock PHY config failed\n", __func__);
		return CB_ERR;
	}

	usb_update_refclk_for_core(USB_REFCLK_CORE_TERT, true);

	return CB_SUCCESS;
}

/* Configure DWC3 USB controller for Type-A (MP) host operation */
static void setup_dwc3(struct usb_dwc3 *dwc3)
{
	u32 *reg = usb3_general_cfg_addr;

	if (hs_speed_only) {
		/* HS-only: disable PIPE clock, switch to UTMI for port 0 */
		setbits32(reg, UTMI_CLK_DIS_0);
		udelay(USB_HW_STABILIZE_DELAY_US);
		setbits32(reg, UTMI_CLK_SEL_0);
		setbits32(reg, PIPE3_PHYSTATUS_SW_0);
		clrbits32(reg, PIPE3_SET_PHYSTATUS_SW_0);
		udelay(USB_HW_STABILIZE_DELAY_US);
		clrbits32(reg, UTMI_CLK_DIS_0);

		setbits32(reg, UTMI_CLK_DIS_1);
		udelay(USB_HW_STABILIZE_DELAY_US);
		setbits32(reg, UTMI_CLK_SEL_1);
		setbits32(reg, PIPE3_PHYSTATUS_SW_1);
		clrbits32(reg, PIPE3_SET_PHYSTATUS_SW_1);
		udelay(USB_HW_STABILIZE_DELAY_US);
		clrbits32(reg, UTMI_CLK_DIS_1);
	} else {
		/* SS: exit U1/U2/U3 in PHY P1/P2/P3, allow P2 from P3 suspend */
		clrsetbits32(&dwc3->usb3pipectl,
			DWC3_GUSB3PIPECTL_DELAYP1TRANS,
			DWC3_GUSB3PIPECTL_UX_EXIT_IN_PX | DWC3_GUSB3PIPECTL_P3EXSIGP2);

		clrsetbits32(&dwc3->usb3pipectl_mp1,
			DWC3_GUSB3PIPECTL_DELAYP1TRANS,
			DWC3_GUSB3PIPECTL_UX_EXIT_IN_PX | DWC3_GUSB3PIPECTL_P3EXSIGP2);
	}

	/* Configure UTMI+ 8-bit PHY interface */
	clrsetbits32(&dwc3->usb2phycfg,
			(DWC3_GUSB2PHYCFG_USB2TRDTIM_MASK |
			DWC3_GUSB2PHYCFG_PHYIF_MASK |
		    DWC3_GUSB2PHYCFG_ENBLSLPM_MASK),
			(DWC3_GUSB2PHYCFG_PHYIF(UTMI_PHYIF_8_BIT) |
			DWC3_GUSB2PHYCFG_USBTRDTIM(USBTRDTIM_UTMI_8_BIT)));

	clrsetbits32(&dwc3->usb2phycfg_mp1,
			(DWC3_GUSB2PHYCFG_USB2TRDTIM_MASK |
			DWC3_GUSB2PHYCFG_PHYIF_MASK |
		    DWC3_GUSB2PHYCFG_ENBLSLPM_MASK),
			(DWC3_GUSB2PHYCFG_PHYIF(UTMI_PHYIF_8_BIT) |
			DWC3_GUSB2PHYCFG_USBTRDTIM(USBTRDTIM_UTMI_8_BIT)));

	/* Enable hardware-based clock gating (DBM FSM) */
	setbits32(USB3_MP_CGCTL_REG_ADDR, USB3_CGCTL_DBM_FSM_EN_BIT);

	/* Disable software clock gating (GCTL.DSBLCLKGTNG=1) */
	clrsetbits32(&dwc3->ctl, (DWC3_GCTL_SCALEDOWN_MASK |
			DWC3_GCTL_DISSCRAMBLE),
			DWC3_GCTL_U2EXIT_LFPS | DWC3_GCTL_DSBLCLKGTNG);

	/* Reduce U3 exit handshake timer to 300ns */
	clrsetbits32(USB3_MP_LINK_REGS_0_LU3LFPSRXTIM_ADDR,
				LFPS_RSP_RX_CLK_CLR_MASK, LFPS_RSP_RX_CLK_SET_MASK);

	clrsetbits32(USB3_MP_LINK_REGS_1_LU3LFPSRXTIM_ADDR,
				LFPS_RSP_RX_CLK_CLR_MASK, LFPS_RSP_RX_CLK_SET_MASK);

	/* Configure L1 exit and IP gap bits */
	clrsetbits32(&dwc3->uctl1,
					DWC3_GUCTL1_CLR_MASK,
					DWC3_GUCTL1_SET_MASK);

	/* Disable USB2 PHY suspend (ENBLSLPM) for MP port 0 and port 1 */
	clrbits32(USB3_MP_GUSB2PHYCFG_REGS_0_ADDR, GUSB2PHYCFG_ENBLSLPM_BIT);
	clrbits32(USB3_MP_GUSB2PHYCFG_REGS_1_ADDR, GUSB2PHYCFG_ENBLSLPM_BIT);

	clrsetbits32(&dwc3->ctl, (DWC3_GCTL_PRTCAPDIR(DWC3_GCTL_PRTCAP_OTG)),
			DWC3_GCTL_PRTCAPDIR(DWC3_GCTL_PRTCAP_HOST));
	printk(BIOS_DEBUG, "USB MP: DWC3 configured in host mode\n");

	/* Enable wake on connect/disconnect/overcurrent for USB2 and USB3 ports */
	setbits32(USB3_MP_PORTSC_20_REGS_0_ADDR, USB3_PORTSC_WCE_BIT);
	setbits32(USB3_MP_PORTSC_20_REGS_1_ADDR, USB3_PORTSC_WCE_BIT);
	setbits32(USB3_MP_PORTSC_30_REGS_0_ADDR, USB3_PORTSC_WCE_BIT);
	setbits32(USB3_MP_PORTSC_30_REGS_1_ADDR, USB3_PORTSC_WCE_BIT);

	/* Disable USB2.0 internal retry (GUCTL3.USB20_RETRY_DISABLE) */
	setbits32((u32 *)((uintptr_t)dwc3 + DWC3_GUCTL3_OFFSET),
		  DWC3_GUCTL3_USB20_RETRY_DISABLE);

	if (CONFIG(USB_MP_FORCE_GEN1_SPEED)) {
		setbits32(USB3_MP_LINK_REGS_0_LLUCTL_ADDR, USB3_MP_LLUCTL_FORCE_GEN1_BIT);
		setbits32(USB3_MP_LINK_REGS_1_LLUCTL_ADDR, USB3_MP_LLUCTL_FORCE_GEN1_BIT);
		printk(BIOS_WARNING, "USB3 MP: Force Gen1 speed enabled for port 0 and port 1\n");
	}
}

/*
 * setup_usb_typea_vbus - Enable VBUS for USB Type-A ports via PMIC GPIO
 *
 * Drives GPIO6 (USB3_HOST_EN) and GPIO8 (USB4_6_HOST_EN) on PMCX0103
 * (Fury4I) PMIC_K (SID=11, BUS_ID=1) to enable VBUS for the two
 * Type-A host ports.
 */
static void setup_usb_typea_vbus(void)
{
	pmic_gpio_output(PMIC_K_SID, USB3_HOST_EN_GPIO, true);
	pmic_gpio_output(PMIC_K_SID, USB4_6_HOST_EN_GPIO, true);
	printk(BIOS_INFO, "USB Type-A VBUS enabled: GPIO%d (USB3_HOST_EN), GPIO%d (USB4_6_HOST_EN) on PMIC_K SID=%d\n",
	       USB3_HOST_EN_GPIO, USB4_6_HOST_EN_GPIO, PMIC_K_SID);
}

/* DWC3 controller configuration for Type-C (PRIM/SEC) ports */
struct dwc3_controller_config {
	const char *name;
	u32 *general_cfg_addr;
	u32 *cgctl_reg_addr;
	u32 *link_regs_lu3lfpsrxtim_addr;
	u32 *gusb2phycfg_regs_addr;
	u32 *portsc_20_regs_addr;
	u32 *portsc_30_regs_addr;
	u8 smb_slave_addr;
};

static void setup_dwc3_controller(struct usb_dwc3 *dwc3,
				  const struct dwc3_controller_config *config,
				  bool hs_only)
{
	u32 *reg = config->general_cfg_addr;

	if (hs_only) {
		/* HS-only: disable PIPE clock, switch to UTMI */
		setbits32(reg, UTMI_CLK_DIS_0);
		udelay(USB_HW_STABILIZE_DELAY_US);
		setbits32(reg, UTMI_CLK_SEL_0);
		setbits32(reg, PIPE3_PHYSTATUS_SW_0);
		clrbits32(reg, PIPE3_SET_PHYSTATUS_SW_0);
		udelay(USB_HW_STABILIZE_DELAY_US);
		clrbits32(reg, UTMI_CLK_DIS_0);
	} else {
		/* Super-speed mode: core exits U1/U2/U3 only in PHY power state P1/P2/P3 */
		clrsetbits32(&dwc3->usb3pipectl,
			DWC3_GUSB3PIPECTL_DELAYP1TRANS,
			DWC3_GUSB3PIPECTL_UX_EXIT_IN_PX);
	}

	/* Configure USB2 PHY interface: Select UTMI+ PHY with 8-bit interface */
	clrsetbits32(&dwc3->usb2phycfg,
		(DWC3_GUSB2PHYCFG_USB2TRDTIM_MASK |
		 DWC3_GUSB2PHYCFG_PHYIF_MASK |
		 DWC3_GUSB2PHYCFG_ENBLSLPM_MASK),
		(DWC3_GUSB2PHYCFG_PHYIF(UTMI_PHYIF_8_BIT) |
		 DWC3_GUSB2PHYCFG_USBTRDTIM(USBTRDTIM_UTMI_8_BIT)));

	/* Enable hardware-based clock gating */
	setbits32(config->cgctl_reg_addr, USB3_CGCTL_DBM_FSM_EN_BIT);

	/* Disable clock gating: DWC_USB3_GCTL.DSBLCLKGTNG = 1 */
	clrsetbits32(&dwc3->ctl,
		(DWC3_GCTL_SCALEDOWN_MASK | DWC3_GCTL_DISSCRAMBLE),
		DWC3_GCTL_U2EXIT_LFPS | DWC3_GCTL_DSBLCLKGTNG);

	/* Allow PHY to transition to P2 from suspend (P3) state */
	setbits32(&dwc3->usb3pipectl,
		(DWC3_GUSB3PIPECTL_P3EXSIGP2 |
		 DWC3_GUSB3PIPECTL_UX_EXIT_IN_PX));

	/* Reduce U3 exit handshake timer to 300ns */
	clrsetbits32(config->link_regs_lu3lfpsrxtim_addr,
		LFPS_RSP_RX_CLK_CLR_MASK, LFPS_RSP_RX_CLK_SET_MASK);

	/* Configure L1 exit and IP gap settings */
	clrsetbits32(&dwc3->uctl1,
		DWC3_GUCTL1_CLR_MASK,
		DWC3_GUCTL1_SET_MASK);

	/* Configure RX threshold for optimal throughput */
	setbits32(&dwc3->rxthrcfg,
		DWC3_GRXTHRCFG_USBMAXRXBURSTSIZE(3) |
		DWC3_GRXTHRCFG_USBRXPKTCNT(3) |
		DWC3_GRXTHRCFG_USBRXPKTCNTSEL);

	/* Set the bus configuration 1K page pipe limit */
	setbits32(&dwc3->sbuscfg1,
		DWC3_GSBUSCFG1_PIPETRANSLIMIT(0xE) |
		DWC3_GSBUSCFG1_EN1KPAGE);

	/* Set Sparse Control Transaction Enable */
	setbits32(&dwc3->uctl, DWC3_GUCTL_SPRSCTRLTRANSEN);

	/* Disable USB2.0 internal retry mechanism (GUCTL3.USB20_RETRY_DISABLE) */
	setbits32((u32 *)((uintptr_t)dwc3 + DWC3_GUCTL3_OFFSET),
		  DWC3_GUCTL3_USB20_RETRY_DISABLE);

	/* Disable sleep mode */
	clrbits32(config->gusb2phycfg_regs_addr, GUSB2PHYCFG_ENBLSLPM_BIT);

	/* Configure controller in Host mode */
	clrsetbits32(&dwc3->ctl,
		DWC3_GCTL_PRTCAPDIR(DWC3_GCTL_PRTCAP_OTG),
		DWC3_GCTL_PRTCAPDIR(DWC3_GCTL_PRTCAP_HOST));

	printk(BIOS_SPEW, "Configure USB %s in Host mode\n", config->name);

	printk(BIOS_DEBUG, "DWC3 %s: SNPSID=0x%08x GCTL=0x%08x "
	       "USB2PHYCFG=0x%08x USB3PIPECTL=0x%08x\n",
	       config->name, read32(&dwc3->snpsid), read32(&dwc3->ctl),
	       read32(&dwc3->usb2phycfg), read32(&dwc3->usb3pipectl));

	/* Enable wake on connect/disconnect/overcurrent */
	setbits32(config->portsc_20_regs_addr, USB3_PORTSC_WCE_BIT);
	setbits32(config->portsc_30_regs_addr, USB3_PORTSC_WCE_BIT);
}

static const struct dwc3_controller_config prim_config = {
	.name = "Primary",
	.general_cfg_addr = (u32 *)USB4_SS_USB3_DRD_SP_0_USB31_PRIMGENERAL_CFG,
	.cgctl_reg_addr = USB4_SS_USB3_DRD_SP_0_USB31_PRIMCGCTL_REG,
	.link_regs_lu3lfpsrxtim_addr =
		USB4_SS_USB3_DRD_SP_0_USB31_PRIMLINK_REGS_0_LU3LFPSRXTIM_ADDR,
	.gusb2phycfg_regs_addr =
		USB4_SS_USB3_DRD_SP_0_USB31_PRIMGUSB2PHYCFG_REGS_0_GUSB2PHYCFG_ADDR,
	.portsc_20_regs_addr =
		USB4_SS_USB3_DRD_SP_0_USB31_PRIMPORTSC_20_REGS_0_PORTSC_20_ADDR,
	.portsc_30_regs_addr =
		USB4_SS_USB3_DRD_SP_0_USB31_PRIMPORTSC_30_REGS_0_PORTSC_30_ADDR,
	.smb_slave_addr = SMB1_SLAVE_ID,
};

static const struct dwc3_controller_config sec_config = {
	.name = "Secondary",
	.general_cfg_addr = (u32 *)USB4_SS_USB3_DRD_SP_1_USB31_SECGENERAL_CFG,
	.cgctl_reg_addr = USB4_SS_USB3_DRD_SP_1_USB31_SECCGCTL_REG,
	.link_regs_lu3lfpsrxtim_addr =
		USB4_SS_USB3_DRD_SP_1_USB31_SECLINK_REGS_0_LU3LFPSRXTIM_ADDR,
	.gusb2phycfg_regs_addr =
		USB4_SS_USB3_DRD_SP_1_USB31_SECGUSB2PHYCFG_REGS_0_GUSB2PHYCFG_ADDR,
	.portsc_20_regs_addr =
		USB4_SS_USB3_DRD_SP_1_USB31_SECPORTSC_20_REGS_0_PORTSC_20_ADDR,
	.portsc_30_regs_addr =
		USB4_SS_USB3_DRD_SP_1_USB31_SECPORTSC_30_REGS_0_PORTSC_30_ADDR,
	.smb_slave_addr = SMB2_SLAVE_ID,
};

static const struct dwc3_controller_config tert_config = {
	.name = "Tertiary",
	.general_cfg_addr = (u32 *)USB4_SS_USB3_DRD_SP_2_USB31_TERTGENERAL_CFG,
	.cgctl_reg_addr = USB4_SS_USB3_DRD_SP_2_USB31_TERTCGCTL_REG,
	.link_regs_lu3lfpsrxtim_addr =
		USB4_SS_USB3_DRD_SP_2_USB31_TERTLINK_REGS_0_LU3LFPSRXTIM_ADDR,
	.gusb2phycfg_regs_addr =
		USB4_SS_USB3_DRD_SP_2_USB31_TERTGUSB2PHYCFG_REGS_0_GUSB2PHYCFG_ADDR,
	.portsc_20_regs_addr =
		USB4_SS_USB3_DRD_SP_2_USB31_TERTPORTSC_20_REGS_0_PORTSC_20_ADDR,
	.portsc_30_regs_addr =
		USB4_SS_USB3_DRD_SP_2_USB31_TERTPORTSC_30_REGS_0_PORTSC_30_ADDR,
	.smb_slave_addr = SMB3_SLAVE_ID,
};

/*
 * usb_repeater_spmi_tune - Configures USB repeater SPMI tuning parameters
 * @config: Controller configuration containing SMB slave address
 */
static void usb_repeater_spmi_tune(const struct dwc3_controller_config *config)
{
	u8 slave = config->smb_slave_addr;

	/* Configure eUSB2 PHY tuning parameters */
	spmi_write8(SPMI_ADDR(slave, EUSB2_TUNE_IUSB2),           EUSB2_TUNE_IUSB2_DEFAULT);
	spmi_write8(SPMI_ADDR(slave, EUSB2_TUNE_HSDISC),          EUSB2_TUNE_HSDISC_VAL);
	spmi_write8(SPMI_ADDR(slave, EUSB2_TUNE_SQUELCH_U),       EUSB2_TUNE_SQUELCH_U_VAL);
	spmi_write8(SPMI_ADDR(slave, EUSB2_TUNE_USB2_SLEW),       EUSB2_TUNE_USB2_SLEW_FAST);
	spmi_write8(SPMI_ADDR(slave, EUSB2_TUNE_USB2_PREEM),      EUSB2_TUNE_USB2_PREEM_25PCT);
	spmi_write8(SPMI_ADDR(slave, EUSB2_TUNE_EUSB2_EQU),       EUSB2_TUNE_EUSB2_EQU_VAL);
	spmi_write8(SPMI_ADDR(slave, EUSB2_TUNE_EHS_COMP_CURRENT), EUSB2_TUNE_EHS_COMP_CURRENT_VAL);
}

/*
 * usb_repeater_spmi_init - Reset the repeater (disable, delay, enable) and tune it
 * @config: Controller configuration containing SMB slave address
 */
static void usb_repeater_spmi_init(const struct dwc3_controller_config *config)
{
	u8 slave = config->smb_slave_addr;

	spmi_write8(SPMI_ADDR(slave, EUSB2_EN_CTL1), EUSB2_EN_CTL1_DISABLE);
	udelay(50);
	spmi_write8(SPMI_ADDR(slave, EUSB2_EN_CTL1), EUSB2_EN_CTL1_ENABLE);

	usb_repeater_spmi_tune(config);

	u8 rptr_status = spmi_read8(SPMI_ADDR(slave, EUSB2_RPTR_STATUS));
	u8 rptr_infra_status = spmi_read8(SPMI_ADDR(slave, EUSB2_RPTR_INFRA_STATUS));
	printk(BIOS_INFO, "%s EUSB2 repeater: RPTR_STATUS=0x%02x INFRA_STATUS=0x%02x\n",
	       config->name, rptr_status, rptr_infra_status);
}

/*
 * usb_typec_status_check - Log the Type-C connection status reported by the PMIC
 * @config: Controller configuration containing SMB slave address
 */
static void usb_typec_status_check(const struct dwc3_controller_config *config)
{
	u8 slave = config->smb_slave_addr;
	u8 misc_status = spmi_read8(SPMI_ADDR(slave, SCHG_TYPE_C_TYPE_C_MISC_STATUS));
	u8 src_status = spmi_read8(SPMI_ADDR(slave, SCHG_TYPE_C_TYPE_C_SRC_STATUS));
	u8 state = spmi_read8(SPMI_ADDR(slave, SCHG_TYPE_C_STATE_MACHINE_STATUS));
	u8 mode = spmi_read8(SPMI_ADDR(slave, SCHG_TYPE_C_TYPE_C_MODE_CFG));

	printk(BIOS_INFO, "%s Type-C: MISC_STATUS=0x%02x SRC_STATUS=0x%02x "
	       "STATE_MACHINE_STATUS=0x%02x MODE_CFG=0x%02x\n",
	       config->name, misc_status, src_status, state, mode);
}

/*
 * get_usb_typec_polarity - Reads Type-C polarity from PMIC
 * @config: Controller configuration containing SMB slave address
 *
 * @return true if polarity is inverted (Lane B)
 * @return false if polarity is normal (Lane A)
 */
static bool get_usb_typec_polarity(const struct dwc3_controller_config *config)
{
	u8 slave = config->smb_slave_addr;
	u8 misc_status = spmi_read8(SPMI_ADDR(slave, SCHG_TYPE_C_TYPE_C_MISC_STATUS));
	bool inverse = (misc_status & CCOUT_INVERT_POLARITY) == CCOUT_INVERT_POLARITY;

	return inverse;
}

/*
 * setup_usb_host - Initialize all USB controllers and PHYs
 * @dwc3: USB DWC3 configuration containing controller base addresses
 *
 * Initialization sequence:
 *   1. Enable clocks and GDSCs for all USB ports
 *   2. Reset and initialize MP HS/SS PHYs (Type-A)
 *   3. Reset and initialize PRIM/SEC HS/SS PHYs (Type-C)
 *   4. Configure all DWC3 controllers in host mode
 *   5. Enable VBUS for Type-A ports
 */
static void setup_usb_host(struct usb_dwc3_cfg *dwc3)
{
	bool high_speed_only_primary = false;
	bool high_speed_only_secondary = false;
	bool high_speed_only_tertiary = false;

	qcom_enable_usb_clk();
	enable_clock_tcsr();

	clock_reset_bcr(dwc3->usb3_bcr, 1);
	udelay(USB_HW_STABILIZE_DELAY_US);
	clock_reset_bcr(dwc3->usb3_bcr, 0);
	udelay(USB_HW_STABILIZE_DELAY_US);

	gpio_output(GPIO_EUSB3_RESET_N, 1);
	gpio_output(GPIO_EUSB6_RESET_N, 1);
	udelay(USB_HW_STABILIZE_DELAY_US);

	clock_reset_bcr(&gcc->qusb2phy_hs0_mp_bcr, 1);
	clock_reset_bcr(&gcc->qusb2phy_hs1_mp_bcr, 1);
	udelay(USB_HW_STABILIZE_DELAY_US);
	clock_reset_bcr(&gcc->qusb2phy_hs1_mp_bcr, 0);
	clock_reset_bcr(&gcc->qusb2phy_hs0_mp_bcr, 0);
	udelay(USB_HW_STABILIZE_DELAY_US);

	hs_usb_phy_init(HS_PHY_MP0);
	hs_usb_phy_init(HS_PHY_MP1);

	usb_mp_clock_reset(USB3_MP_PHY_PIPE_0_CBCR, 1);
	usb_mp_clock_reset(USB3_MP_PHY_PIPE_1_CBCR, 1);
	udelay(USB_HW_STABILIZE_DELAY_US);

	usb_mp_clock_reset(USB30_MP_MASTER_CBCR, 1);
	udelay(USB_HW_STABILIZE_DELAY_US);
	usb_mp_clock_reset(USB30_MP_MASTER_CBCR, 0);

	clock_reset_bcr(&gcc->gcc_usb3_uniphy_mp0_bcr, 1);
	clock_reset_bcr(&gcc->gcc_usb3_uniphy_mp1_bcr, 1);
	clock_reset_bcr(&gcc->gcc_usb3uniphy_phy_mp0_bcr, 1);
	clock_reset_bcr(&gcc->gcc_usb3uniphy_phy_mp1_bcr, 1);
	udelay(USB_HW_STABILIZE_DELAY_US);
	clock_reset_bcr(&gcc->gcc_usb3uniphy_phy_mp0_bcr, 0);
	clock_reset_bcr(&gcc->gcc_usb3uniphy_phy_mp1_bcr, 0);
	clock_reset_bcr(&gcc->gcc_usb3_uniphy_mp0_bcr, 0);
	clock_reset_bcr(&gcc->gcc_usb3_uniphy_mp1_bcr, 0);
	udelay(USB_HW_STABILIZE_DELAY_US);

	usb_mp_clock_reset(USB3_MP_PHY_PIPE_0_CBCR, 0);
	usb_mp_clock_reset(USB3_MP_PHY_PIPE_1_CBCR, 0);

	/* Initialize MP QMP SS PHY; fall back to HS-only on failure */
	bool ret0 = ss_qmp_phy_init(0);
	bool ret1 = ss_qmp_phy_init(1);
	if (!ret0 || !ret1)
		hs_speed_only = true;

	/*
	 * C0 (PRIM) - sequence:
	 *   eUSB2 repeater init -> HS BCR reset -> HS PHY init -> SS+USB4 BCR reset -> SS PHY init
	 *
	 * Step 1: eUSB2 repeater init
	 */
	usb_repeater_spmi_init(&prim_config);

	/* Step 2: HS BCR reset (HS_TYPE: gcc_qusb2phy_prim_bcr) */
	clock_reset_bcr(&gcc->gcc_qusb2phy_prim_bcr, 1);
	udelay(USB_HW_STABILIZE_DELAY_US);
	clock_reset_bcr(&gcc->gcc_qusb2phy_prim_bcr, 0);
	udelay(USB_HW_STABILIZE_DELAY_US);

	/* Step 3: HS PHY init */
	hs_usb_phy_init(HS_PHY_SS0);
	udelay(USB_HW_STABILIZE_DELAY_US);

	/*
	 * Step 4: SS+USB4 BCR reset
	 *   SS_TYPE:   gcc_usb3_phy_prim_bcr, gcc_usb3phy_phy_prim_bcr
	 *   USB4_TYPE: gcc_usb4_0_dp0_phy_prim_bcr
	 */
	clock_reset_bcr(&gcc->gcc_usb3_phy_prim_bcr, 1);
	clock_reset_bcr(&gcc->gcc_usb3phy_phy_prim_bcr, 1);
	clock_reset_bcr(&gcc->gcc_usb4_0_dp0_phy_prim_bcr, 1);
	udelay(USB_HW_STABILIZE_DELAY_US);
	clock_reset_bcr(&gcc->gcc_usb4_0_dp0_phy_prim_bcr, 0);
	clock_reset_bcr(&gcc->gcc_usb3phy_phy_prim_bcr, 0);
	clock_reset_bcr(&gcc->gcc_usb3_phy_prim_bcr, 0);
	udelay(USB_HW_STABILIZE_DELAY_US);

	/* Step 5: SS PHY init */
	bool polarity_prim = get_usb_typec_polarity(&prim_config);
	int ss0_ret = qmp_usb4_dp_phy_ss_init(0, polarity_prim);
	if (ss0_ret != CB_SUCCESS) {
		printk(BIOS_WARNING, "SS0 unavailable; using HS-only mode\n");
		high_speed_only_primary = true;
	}

	usb_typec_status_check(&prim_config);

	/*
	 * C1 (SEC) - sequence:
	 *   eUSB2 repeater init -> HS BCR reset -> HS PHY init -> SS+USB4 BCR reset -> SS PHY init
	 *
	 * Step 1: eUSB2 repeater init
	 */
	usb_repeater_spmi_init(&sec_config);

	/* Step 2: HS BCR reset (HS_TYPE: gcc_qusb2phy_sec_bcr) */
	clock_reset_bcr(&gcc->gcc_qusb2phy_sec_bcr, 1);
	udelay(USB_HW_STABILIZE_DELAY_US);
	clock_reset_bcr(&gcc->gcc_qusb2phy_sec_bcr, 0);
	udelay(USB_HW_STABILIZE_DELAY_US);

	/* Step 3: HS PHY init */
	hs_usb_phy_init(HS_PHY_SS1);
	udelay(USB_HW_STABILIZE_DELAY_US);

	/*
	 * Step 4: SS+USB4 BCR reset
	 *   SS_TYPE:   gcc_usb3_phy_sec_bcr, gcc_usb3phy_phy_sec_bcr
	 *   USB4_TYPE: gcc_usb4_1_dp0_phy_sec_bcr
	 */
	clock_reset_bcr(&gcc->gcc_usb4_1_dp0_phy_sec_bcr, 1);
	clock_reset_bcr(&gcc->gcc_usb3_phy_sec_bcr, 1);
	clock_reset_bcr(&gcc->gcc_usb3phy_phy_sec_bcr, 1);
	udelay(USB_HW_STABILIZE_DELAY_US);
	clock_reset_bcr(&gcc->gcc_usb4_1_dp0_phy_sec_bcr, 0);
	clock_reset_bcr(&gcc->gcc_usb3phy_phy_sec_bcr, 0);
	clock_reset_bcr(&gcc->gcc_usb3_phy_sec_bcr, 0);
	udelay(USB_HW_STABILIZE_DELAY_US);

	/* Step 5: SS PHY init */
	bool polarity_sec = get_usb_typec_polarity(&sec_config);
	int ss1_ret = qmp_usb4_dp_phy_ss_init(1, polarity_sec);
	if (ss1_ret != CB_SUCCESS) {
		printk(BIOS_WARNING, "SS1 unavailable; using HS-only mode\n");
		high_speed_only_secondary = true;
	}

	usb_typec_status_check(&sec_config);

	/*
	 * C2 (TERT) - sequence:
	 *   eUSB2 repeater init -> HS BCR reset -> HS PHY init -> SS+USB4 BCR reset -> SS PHY init
	 *
	 * Step 1: eUSB2 repeater init
	 */
	usb_repeater_spmi_init(&tert_config);

	/* Step 2: HS BCR reset (HS_TYPE: gcc_qusb2phy_tert_bcr) */
	clock_reset_bcr(&gcc->gcc_qusb2phy_tert_bcr, 1);
	udelay(USB_HW_STABILIZE_DELAY_US);
	clock_reset_bcr(&gcc->gcc_qusb2phy_tert_bcr, 0);
	udelay(USB_HW_STABILIZE_DELAY_US);

	/* Step 3: HS PHY init */
	hs_usb_phy_init(HS_PHY_SS2);
	udelay(USB_HW_STABILIZE_DELAY_US);

	/*
	 * Step 4: SS+USB4 BCR reset
	 *   SS_TYPE:   gcc_usb3_phy_tert_bcr, gcc_usb3phy_phy_tert_bcr
	 *   USB4_TYPE: gcc_usb4_2_dp0_phy_tert_bcr
	 */
	clock_reset_bcr(&gcc->gcc_usb4_2_dp0_phy_tert_bcr, 1);
	clock_reset_bcr(&gcc->gcc_usb3_phy_tert_bcr, 1);
	clock_reset_bcr(&gcc->gcc_usb3phy_phy_tert_bcr, 1);
	udelay(USB_HW_STABILIZE_DELAY_US);
	clock_reset_bcr(&gcc->gcc_usb4_2_dp0_phy_tert_bcr, 0);
	clock_reset_bcr(&gcc->gcc_usb3phy_phy_tert_bcr, 0);
	clock_reset_bcr(&gcc->gcc_usb3_phy_tert_bcr, 0);
	udelay(USB_HW_STABILIZE_DELAY_US);

	/* Step 5: SS PHY init */
	bool polarity_tert = get_usb_typec_polarity(&tert_config);
	int ss2_ret = qmp_usb4_dp_phy_ss_init(2, polarity_tert);
	if (ss2_ret != CB_SUCCESS) {
		printk(BIOS_WARNING, "SS2 unavailable; using HS-only mode\n");
		high_speed_only_tertiary = true;
	}

	usb_typec_status_check(&tert_config);

	/* Initialize USB Controller for Type A (MP) */
	setup_dwc3(dwc3->usb_host_dwc3);
	setup_usb_typea_vbus();
	/* Initialize USB Controller for Type C port 0 (C0) */
	setup_dwc3_controller(dwc3->usb_host_dwc3_prim, &prim_config, high_speed_only_primary);
	/* Initialize USB Controller for Type C port 1 (C1) */
	setup_dwc3_controller(dwc3->usb_host_dwc3_sec, &sec_config, high_speed_only_secondary);
	/* Initialize USB Controller for Type C port 2 (C2) */
	setup_dwc3_controller(dwc3->usb_host_dwc3_tert, &tert_config, high_speed_only_tertiary);

	printk(BIOS_INFO, "USB: DWC3 and PHY initialization complete\n");
}

/*
 * setup_usb_host0 - Sets up USB HOST0 controller.
 * Initializes and configures the USB HOST0 controller, including clocks,
 * PHY resets, and DWC3 core for host mode.
 */
void setup_usb_host0(void)
{
	printk(BIOS_INFO, "Setting up USB HOST controller\n");
	setup_usb_host(&usb_ports);
}

/*
 * usb_update_refclk_for_core - Updates USB reference clock for specified core
 * @core_num: USB core number, see enum usb_refclk_core
 * @enable: true to enable, false to disable reference clock
 */
static void usb_update_refclk_for_core(u32 core_num, bool enable)
{
	u32 value = enable ? USB_CLKREF_ENABLE_VALUE : 0;

	switch (core_num) {
	case USB_REFCLK_CORE_PRIM:
		/* No clkref for primary core (PRIM/C0) USB2, USB3 PHYs */
		break;
	case USB_REFCLK_CORE_SEC:
		/* SEC (C1): USB4_1_CLKREF_EN only - no USB2 clkref for secondary */
		clrsetbits32(TCSR_GCC_USB4_1_CLKREF_EN_ADDR, 0x1, value);
		break;
	case USB_REFCLK_CORE_TERT:
		/* TERT (C2): USB4_2_CLKREF_EN + USB2_4_CLKREF_EN
		 * cxo_network: usb2_2 phy (u_cm_dwc_usb2_tert) */
		clrsetbits32(TCSR_GCC_USB4_2_CLKREF_EN_ADDR, 0x1, value);
		clrsetbits32(TCSR_GCC_USB2_4_CLKREF_EN_ADDR, 0x1, value);
		break;
	case USB_REFCLK_CORE_MP:
		/* MP0: USB3_MP0 + USB2_2 (cxo_network: usb2_hs3 phy, u_cm_dwc_usb2_mp0) */
		clrsetbits32(TCSR_GCC_USB3_MP0_CLKREF_EN_ADDR, 0x1, value);
		clrsetbits32(TCSR_GCC_USB2_2_CLKREF_EN_ADDR, 0x1, value);
		/* MP1: USB3_MP1 */
		clrsetbits32(TCSR_GCC_USB3_MP1_CLKREF_EN_ADDR, 0x1, value);
		break;
	default:
		/* No clkref */
		break;
	}
}
