/* SPDX-License-Identifier: GPL-2.0-only */

#include <console/console.h>
#include <device/mmio.h>
#include <soc/usb/usb.h>
#include <soc/addressmap.h>
#include <soc/clock.h>
#include <soc/qcom_spmi.h>
#include <delay.h>
#include <gpio.h>
#include <timer.h>

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

struct usb_dwc3_cfg {
	struct usb_dwc3 *usb_host_dwc3;
	struct usb_dwc3 *usb_host_dwc3_prim;
	struct usb_dwc3 *usb_host_dwc3_sec;
	u32 *usb3_bcr;
	u32 *qusb2phy_bcr;
	u32 *qusb2phy1_bcr;
	u32 *gcc_qusb2phy_prim_bcr;
	u32 *gcc_qusb2phy_sec_bcr;
	u32 *gcc_usb3phy_bcr_reg;
	u32 *gcc_qmpphy_bcr_reg;
	u32 *gcc_usb3phy1_bcr_reg;
	u32 *gcc_qmpphy1_bcr_reg;
	u32 *gcc_usb4_0_dp0_phy_prim_bcr;
	u32 *gcc_usb4_1_dp0_phy_sec_bcr;
	u32 *gcc_usb3_phy_prim_bcr;
	u32 *gcc_usb3_phy_sec_bcr;
	u32 *gcc_usb3phy_phy_prim_bcr;
	u32 *gcc_usb3phy_phy_sec_bcr;
};

static struct usb_dwc3_cfg usb_ports = {
	.usb_host_dwc3 =	(void *)USB_HOST_DWC3_MP_BASE,
	.usb_host_dwc3_prim =	(void *)USB_HOST_DWC3_PRIM_BASE,
	.usb_host_dwc3_sec =	(void *)USB_HOST_DWC3_SEC_BASE,
	.usb3_bcr =		&gcc->gcc_usb30_mp_bcr,
	.qusb2phy_bcr =		&gcc->qusb2phy_hs0_mp_bcr,
	.gcc_usb3phy_bcr_reg =	&gcc->usb3uniphy_phy_mp0_bcr,
	.gcc_qmpphy_bcr_reg =	&gcc->usb3_uniphy_mp0_bcr,
	.qusb2phy1_bcr =		&gcc->qusb2phy_hs1_mp_bcr,
	.gcc_usb3phy1_bcr_reg =	&gcc->usb3uniphy_phy_mp1_bcr,
	.gcc_qmpphy1_bcr_reg =	&gcc->usb3_uniphy_mp1_bcr,
	.gcc_qusb2phy_prim_bcr =	&gcc->gcc_qusb2phy_prim_bcr,
	.gcc_qusb2phy_sec_bcr =		&gcc->gcc_qusb2phy_sec_bcr,
	.gcc_usb4_0_dp0_phy_prim_bcr =	&gcc->gcc_usb4_0_dp0_phy_prim_bcr,
	.gcc_usb4_1_dp0_phy_sec_bcr =	&gcc->gcc_usb4_1_dp0_phy_sec_bcr,
	.gcc_usb3_phy_prim_bcr =	&gcc->gcc_usb3_phy_prim_bcr,
	.gcc_usb3_phy_sec_bcr =		&gcc->gcc_usb3_phy_sec_bcr,
	.gcc_usb3phy_phy_prim_bcr =	&gcc->gcc_usb3phy_phy_prim_bcr,
	.gcc_usb3phy_phy_sec_bcr =	&gcc->gcc_usb3phy_phy_sec_bcr,
};

static bool hs_speed_only;
static u32 *usb3_general_cfg_addr = (void *)USB_HOST_DWC3_MP_GENERAL_CFG_ADDR;

/* Enables Repeaters /Refgen blocks for USB */
void enable_clock_tcsr(void)
{
	write32(TCSR_GCC_CXO_1_REFGEN_BIAS_SEL__SEL_REFGEN_ADDR, 0x00000001);
	write32(TCSR_GCC_CXO_0_REFGEN_BIAS_SEL__SEL_REFGEN_ADDR, 0x00000001);

	write32(TCSR_QREFS_CXO_0_RPT3_CONFIG_ADDR, 0x3);
	write32(TCSR_QREFS_CXO_0_RPT4_CONFIG_ADDR, 0x3);
	write32(TCSR_QREFS_CXO_1_RPT0_CONFIG_ADDR, 0x3);
	write32(TCSR_QREFS_CXO_1_RX3_CONFIG_ADDR, 0x3);
	write32(TCSR_QREFS_CXO_1_RX0_CONFIG_ADDR, 0x3);
	write32(TCSR_QREFS_CXO_0_RX3_CONFIG_ADDR, 0x3);
	write32(TCSR_QREFS_CXO_0_RX4_CONFIG_ADDR, 0x3);
}
/*
 * qcom_enable_usb_clk - Enables USB clocks and GDSCs for Qualcomm SoCs.
 * @return 0 on success, or a negative error code on failure.
 */
int32_t qcom_enable_usb_clk(void)
{
	int32_t ret, clk, gdsc;

	/* Enable USB GDSCs before enabling USB clocks */
	for (gdsc = USB30_MP_GDSC; gdsc < MAX_USB_GDSC; gdsc++) {
		ret = clock_enable_usb_gdsc(gdsc);
		if (ret) {
			printk(BIOS_ERR, "Failed to enable USB GDSC %d\n", gdsc);
			return ret;
		}
	}

	clock_configure_usb();

	if (usb_clock_configure_mux(USB3_PHY_PIPE_0, USB_PHY_XO_SRC_SEL)) {
		printk(BIOS_ERR, "%s(): USB3 PHY PIPE 0 clock enable failed\n", __func__);
		return -1;
	}
	if (usb_clock_configure_mux(USB3_PHY_PIPE_1, USB_PHY_XO_SRC_SEL)) {
		printk(BIOS_ERR, "%s(): USB3 PHY PIPE 0 clock enable failed\n", __func__);
		return -1;
	}
	/* Enable USB MP clocks */
	for (clk = USB30_MP_MASTER_CBCR; clk < USB_CLK_COUNT; clk++) {
		ret = usb_clock_enable(clk);
		if (ret) {
			printk(BIOS_ERR, "Failed to enable %d clock\n", clk);
			return ret;
		}
	}

	usb_update_refclk_for_core(3, true);
	/* Set USB3 PHY PIPE 1 clock source to USB PHY */

	if (usb_clock_configure_mux(USB3_PHY_PIPE_0, USB_PHY_PIPE_SRC_SEL)) {
		printk(BIOS_ERR, "%s(): USB3 PHY PIPE 1 clock enable failed\n", __func__);
		return -1;
	}

	if (usb_clock_configure_mux(USB3_PHY_PIPE_1, USB_PHY_PIPE_SRC_SEL)) {
		printk(BIOS_ERR, "%s(): USB3 PHY PIPE 1 clock enable failed\n", __func__);
		return -1;
	}

	if (usb_clock_configure_mux(USB3_PRIM_PHY_PIPE, USB_PHY_XO_SRC_SEL)) {
		printk(BIOS_ERR, "%s(): USB3 PRIM PHY PIPE clock XO config failed\n", __func__);
		return -1;
	}

	/* Enable USB0 PRIM clocks */
	for (clk = USB_PRIM_SYS_NOC_USB_AXI_CBCR; clk < USB_PRIM_CLK_COUNT; clk++) {
		ret = usb_prim_clock_enable(clk);
		if (ret) {
			printk(BIOS_ERR, "Failed to enable USB0 clock %d\n", clk);
			return ret;
		}
	}

	if (usb_clock_configure_mux(USB3_PRIM_PHY_PIPE, USB_PHY_PIPE_SRC_SEL)) {
		printk(BIOS_ERR, "%s(): USB3 PRIM PHY PIPE clock PHY config failed\n", __func__);
		return -1;
	}

	usb_update_refclk_for_core(0, true);

	if (usb_clock_configure_mux(USB3_SEC_PHY_PIPE, USB_PHY_XO_SRC_SEL)) {
		printk(BIOS_ERR, "%s(): USB3 SEC PHY PIPE clock XO config failed\n", __func__);
		return -1;
	}

	/* Enable USB SEC clocks */
	for (clk = USB_SEC_CFG_NOC_USB3_SEC_AXI_CBCR; clk < USB_SEC_CLK_COUNT; clk++) {
		ret = usb_sec_clock_enable(clk);
		if (ret) {
			printk(BIOS_ERR, "Failed to enable USB SEC clock %d\n", clk);
			return ret;
		}
	}

	if (usb_clock_configure_mux(USB3_SEC_PHY_PIPE, USB_PHY_PIPE_SRC_SEL)) {
		printk(BIOS_ERR, "%s(): USB3 SEC PHY PIPE clock PHY config failed\n", __func__);
		return -1;
	}

	usb_update_refclk_for_core(1, true);

	return ret;
}

/*
 * setup_dwc3 - Configures the DWC3 USB controller.
 * @dwc3: Pointer to the USB DWC3 configuration structure.
 * Handles high-speed operation, power management, and sets to host mode.
 */

static void setup_dwc3(struct usb_dwc3 *dwc3)
{
	u32 *reg = usb3_general_cfg_addr;
	if (hs_speed_only) {
		/* Set UTMI_CLK_DIS_0 */
		setbits32(reg, UTMI_CLK_DIS_0);
		udelay(10);

		/* Set UTMI_CLK_SEL_0 */
		setbits32(reg, UTMI_CLK_SEL_0);

		/* Set PIPE3_PHYSTATUS_SW_0 */
		setbits32(reg, PIPE3_PHYSTATUS_SW_0);

		/* Clear PIPE3_SET_PHYSTATUS_SW_0 */
		clrbits32(reg, PIPE3_SET_PHYSTATUS_SW_0);
		udelay(10);

		/* Clear UTMI_CLK_DIS_0 */
		clrbits32(reg, UTMI_CLK_DIS_0);
	} else {
		/* core exits U1/U2/U3 only in PHY power state P1/P2/P3 respectively */
		clrsetbits32(&dwc3->usb3pipectl,
			DWC3_GUSB3PIPECTL_DELAYP1TRANS,
			DWC3_GUSB3PIPECTL_UX_EXIT_IN_PX);

		clrsetbits32(&dwc3->usb3pipectl_mp1,
			DWC3_GUSB3PIPECTL_DELAYP1TRANS,
			DWC3_GUSB3PIPECTL_UX_EXIT_IN_PX);
	}
	/*
	 * Configure USB phy interface of DWC3 core.
	 * 1. Select UTMI+ PHY with 16-bit interface.
	 * 2. Set USBTRDTIM to the corresponding value
	 * according to the UTMI+ PHY interface.
	 */
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
	/*
	To save power, enable the hardware-based clock gating (not relevant for PBL):
	a. usb30_reg_cgctl[DBM_FSM_EN] = 0x1
	*/
	setbits32(USB3_MP_CGCTL_REG_ADDR, USB3_CGCTL_DBM_FSM_EN_BIT);
	//Disable clock gating: DWC_USB3_GCTL.DSBLCLKGTNG = 1
	clrsetbits32(&dwc3->ctl, (DWC3_GCTL_SCALEDOWN_MASK |
			DWC3_GCTL_DISSCRAMBLE),
			DWC3_GCTL_U2EXIT_LFPS | DWC3_GCTL_DSBLCLKGTNG);

	//Allow PHY to transition to P2 from suspend (P3) state.
	setbits32(&dwc3->usb3pipectl,
		(DWC3_GUSB3PIPECTL_P3EXSIGP2|
		DWC3_GUSB3PIPECTL_UX_EXIT_IN_PX));

	setbits32(&dwc3->usb3pipectl_mp1,
		(DWC3_GUSB3PIPECTL_P3EXSIGP2|
		DWC3_GUSB3PIPECTL_UX_EXIT_IN_PX));

	// Reduce U3 exit handshake timer to 300ns
	clrsetbits32(USB3_MP_LINK_REGS_0_LU3LFPSRXTIM_ADDR,
				LFPS_RSP_RX_CLK_CLR_MASK, LFPS_RSP_RX_CLK_SET_MASK);

	clrsetbits32(USB3_MP_LINK_REGS_1_LU3LFPSRXTIM_ADDR,
				LFPS_RSP_RX_CLK_CLR_MASK, LFPS_RSP_RX_CLK_SET_MASK);

	clrsetbits32(&dwc3->uctl1,
					DWC3_GUCTL1_CLR_MASK, // Clear L1 exit and IP gap bits
					DWC3_GUCTL1_SET_MASK);

	clrbits32(USB3_MP_GUSB2PHYCFG_REGS_0_ADDR, GUSB2PHYCFG_ENBLSLPM_BIT);

	clrbits32(USB3_MP_GUSB2PHYCFG_REGS_1_ADDR, GUSB2PHYCFG_ENBLSLPM_BIT);

	/* configure controller in Host mode */
	clrsetbits32(&dwc3->ctl, (DWC3_GCTL_PRTCAPDIR(DWC3_GCTL_PRTCAP_OTG)),
			DWC3_GCTL_PRTCAPDIR(DWC3_GCTL_PRTCAP_HOST));
	printk(BIOS_SPEW, "Configure USB in Host mode\n");

	setbits32(USB3_MP_PORTSC_20_REGS_0_ADDR, USB3_PORTSC_WCE_BIT);

	setbits32(USB3_MP_PORTSC_20_REGS_1_ADDR, USB3_PORTSC_WCE_BIT);

	setbits32(USB3_MP_PORTSC_30_REGS_0_ADDR, USB3_PORTSC_WCE_BIT);

	setbits32(USB3_MP_PORTSC_30_REGS_1_ADDR, USB3_PORTSC_WCE_BIT);

}

/*
 * setup_dwc3 - Configures the DWC3 USB controller.
 * @dwc3: Pointer to the USB DWC3 configuration structure.
 * Handles high-speed operation, power management, and sets to host mode.
 */

/* DWC3 Controller Configuration Structure */
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

/*
 * setup_dwc3_controller - Configure DWC3 USB controller
 * @dwc3: Pointer to the USB DWC3 structure
 * @config: Controller-specific configuration
 * @hs_only: true for high-speed only mode, false for super-speed mode
 *
 * Handles both high-speed and super-speed operation, power management,
 * and sets controller to host mode.
 */
static void setup_dwc3_controller(struct usb_dwc3 *dwc3,
				  const struct dwc3_controller_config *config,
				  bool hs_only)
{
	u32 *reg = config->general_cfg_addr;

	/* Configure speed mode */
	if (hs_only) {
		/* High-speed only mode configuration */
		setbits32(reg, UTMI_CLK_DIS_0);
		udelay(10);
		setbits32(reg, UTMI_CLK_SEL_0);
		setbits32(reg, PIPE3_PHYSTATUS_SW_0);
		clrbits32(reg, PIPE3_SET_PHYSTATUS_SW_0);
		udelay(10);
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

	/* Set GRXTHRCFG based on case 8000615753 values */
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

	/* Disable sleep mode */
	clrbits32(config->gusb2phycfg_regs_addr, GUSB2PHYCFG_ENBLSLPM_BIT);

	/* Configure controller in Host mode */
	clrsetbits32(&dwc3->ctl,
		DWC3_GCTL_PRTCAPDIR(DWC3_GCTL_PRTCAP_OTG),
		DWC3_GCTL_PRTCAPDIR(DWC3_GCTL_PRTCAP_HOST));

	printk(BIOS_SPEW, "Configure USB %s in Host mode\n", config->name);

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

/*
 * usb_repeater_spmi_tune - Configures USB repeater SPMI tuning parameters
 * @config: Controller configuration containing SMB slave address
 */
static void usb_repeater_spmi_tune(const struct dwc3_controller_config *config)
{
	u8 slave = config->smb_slave_addr;

	/* Configure USB 2.0 output current tuning - default +11.1% amplitude */
	spmi_write8(SPMI_ADDR(slave, EUSB2_TUNE_IUSB2), EUSB2_TUNE_IUSB2_DEFAULT);

	/* Configure USB 2.0 HS TX slew rate - +14.9% faster than default */
	spmi_write8(SPMI_ADDR(slave, EUSB2_TUNE_USB2_SLEW), EUSB2_TUNE_USB2_SLEW_FAST);

	/* Configure USB 2.0 HS TX pre-emphasis - +25% current boost */
	spmi_write8(SPMI_ADDR(slave, EUSB2_TUNE_USB2_PREEM), EUSB2_TUNE_USB2_PREEM_25PCT);
}

/*
 * usb_repeater_spmi_init - Initializes USB repeater SPMI with enable-delay-disable sequence
 * @config: Controller configuration containing SMB slave address
 */
static void usb_repeater_spmi_init(const struct dwc3_controller_config *config)
{
	u8 slave = config->smb_slave_addr;

	spmi_write8(SPMI_ADDR(slave, EUSB2_EN_CTL1), EUSB2_EN_CTL1_DISABLE);
	udelay(50);
	spmi_write8(SPMI_ADDR(slave, EUSB2_EN_CTL1), EUSB2_EN_CTL1_ENABLE);

	printk(BIOS_INFO, "Enabling %s EUSB2_EN_CTL1\n", config->name);

	/* Call tune API after initialization */
	usb_repeater_spmi_tune(config);
}

/*
 * get_usb_typec_polarity - Reads Type-C polarity from PMIC
 * @config: Controller configuration containing SMB slave address
 *
 * @return TRUE if polarify is inverse
 * @return FALSE if polarify is normal
 */
static bool get_usb_typec_polarity(const struct dwc3_controller_config *config)
{
	u8 slave = config->smb_slave_addr;
	u8 misc_status = spmi_read8(SPMI_ADDR(slave, SCHG_TYPE_C_TYPE_C_MISC_STATUS));
	return (misc_status & CCOUT_INVERT_POLARITY) == CCOUT_INVERT_POLARITY;
}

/* Initialization of DWC3 Core and PHY */
static void setup_usb_host(struct usb_dwc3_cfg *dwc3)
{
	bool high_speed_only_primary = false;
	bool high_speed_only_secondary = false;

	/* Call Clock Enable */
	qcom_enable_usb_clk();

	enable_clock_tcsr();

	/* Clear core reset. */
	clock_reset_bcr(dwc3->usb3_bcr, 1);
	udelay(10);
	clock_reset_bcr(dwc3->usb3_bcr, 0);
	udelay(10);

	/* Repeater enable MP0*/
	gpio_output(GPIO(6), 0x1);

	/* Repeater enable MP1*/
	gpio_output(GPIO(184), 0x1);

	/* Clear QUSB PHY reset. */
	clock_reset_bcr(dwc3->qusb2phy_bcr, 1);
	clock_reset_bcr(dwc3->qusb2phy1_bcr, 1);
	udelay(10);
	clock_reset_bcr(dwc3->qusb2phy_bcr, 0);
	clock_reset_bcr(dwc3->qusb2phy1_bcr, 0);

	/* Initialize HS PHY */
	hs_usb_phy_init(0);
	hs_usb_phy_init(1);

	/* Clear QMP PHY resets. */
	usb_clock_reset(USB3_MP_PHY_PIPE_0_CBCR, 1);
	usb_clock_reset(USB3_MP_PHY_PIPE_1_CBCR, 1);
	udelay(10);

	usb_clock_reset(USB30_MP_MASTER_CBCR, 1);
	usb_clock_reset(USB30_MP_MASTER_CBCR, 0);
	udelay(10);

	clock_reset_bcr(dwc3->gcc_usb3phy_bcr_reg, 1);
	clock_reset_bcr(dwc3->gcc_qmpphy_bcr_reg, 1);

	clock_reset_bcr(dwc3->gcc_usb3phy1_bcr_reg, 1);
	clock_reset_bcr(dwc3->gcc_qmpphy1_bcr_reg, 1);
	udelay(10);

	clock_reset_bcr(dwc3->gcc_usb3phy_bcr_reg, 0);
	clock_reset_bcr(dwc3->gcc_qmpphy_bcr_reg, 0);

	clock_reset_bcr(dwc3->gcc_usb3phy1_bcr_reg, 0);
	clock_reset_bcr(dwc3->gcc_qmpphy1_bcr_reg, 0);
	udelay(10);

	usb_clock_reset(USB3_MP_PHY_PIPE_0_CBCR, 0);
	usb_clock_reset(USB3_MP_PHY_PIPE_1_CBCR, 0);

	/* Initialize QMP PHY */
	bool ret0 = ss_qmp_phy_init(0);
	bool ret1 = ss_qmp_phy_init(1);

	/* If SS PHY init fails fall back to HS speed */
	if (!ret0 || !ret1)
		hs_speed_only = true;

	/* Type C port 0 - C0 */
	clock_reset_bcr(dwc3->gcc_qusb2phy_prim_bcr, 1);
	udelay(10);
	clock_reset_bcr(dwc3->gcc_qusb2phy_prim_bcr, 0);
	usb_repeater_spmi_init(&prim_config);
	/* Initialize secondary HS PHY */
	hs_usb_phy_init(2);

	/* Reset USB4.0 DP0 PHY, USB3 PHY, and USB3PHY PHY PRIM BCRs */
	clock_reset_bcr(dwc3->gcc_usb4_0_dp0_phy_prim_bcr, 1);
	clock_reset_bcr(dwc3->gcc_usb3_phy_prim_bcr, 1);
	clock_reset_bcr(dwc3->gcc_usb3phy_phy_prim_bcr, 1);
	udelay(10);
	clock_reset_bcr(dwc3->gcc_usb3phy_phy_prim_bcr, 0);
	clock_reset_bcr(dwc3->gcc_usb3_phy_prim_bcr, 0);
	clock_reset_bcr(dwc3->gcc_usb4_0_dp0_phy_prim_bcr, 0);
	udelay(10);
	/* Initialize USB4/USB3 EDP_DP_Con PHY Configuration */
	int ss0_ret = qmp_usb4_dp_phy_ss_init(0, get_usb_typec_polarity(&prim_config));
	if (ss0_ret != CB_SUCCESS) {
		printk(BIOS_ERR, "SS0 QMP PHY initialization failed\n");
		high_speed_only_primary = true;
	}
	/* Enable VBUS for C0 */
	enable_vbus_ss(&prim_config);
	udelay(50);
	usb_typec_status_check(&prim_config);

	/* Type C port 1 - C1 */
	/* Reset USB secondary[C1] PHY BCRs */
	clock_reset_bcr(dwc3->gcc_qusb2phy_sec_bcr, 1);
	udelay(10);
	clock_reset_bcr(dwc3->gcc_qusb2phy_sec_bcr, 0);
	usb_repeater_spmi_init(&sec_config);
	/* Initialize secondary HS PHY */
	hs_usb_phy_init(3);

	/* Reset USB4.1 DP0 PHY, USB3 PHY, and USB3PHY PHY SEC BCRs */
	clock_reset_bcr(dwc3->gcc_usb4_1_dp0_phy_sec_bcr, 1);
	clock_reset_bcr(dwc3->gcc_usb3_phy_sec_bcr, 1);
	clock_reset_bcr(dwc3->gcc_usb3phy_phy_sec_bcr, 1);
	udelay(10);
	clock_reset_bcr(dwc3->gcc_usb3phy_phy_sec_bcr, 0);
	clock_reset_bcr(dwc3->gcc_usb3_phy_sec_bcr, 0);
	clock_reset_bcr(dwc3->gcc_usb4_1_dp0_phy_sec_bcr, 0);
	udelay(10);
	/* Initialize USB4/USB3 EDP_DP_Con PHY Configuration (secondary) */
	int ss1_ret = qmp_usb4_dp_phy_ss_init(1, get_usb_typec_polarity(&sec_config));
	if (ss1_ret != CB_SUCCESS) {
		printk(BIOS_ERR, "SS1 QMP PHY initialization failed\n");
		high_speed_only_secondary = true;
	}
	/* Enable VBUS for C1 */
	enable_vbus_ss(&sec_config);
	udelay(50);
	usb_typec_status_check(&sec_config);

	/* Initialize USB Controller for Type A */
	setup_dwc3(dwc3->usb_host_dwc3);
	/* Initialize USB Controller for Type C port 0(C0) */
	setup_dwc3_controller(dwc3->usb_host_dwc3_prim, &prim_config, high_speed_only_primary);
	/* Initialize USB Controller for Type C port 1(C1) */
	setup_dwc3_controller(dwc3->usb_host_dwc3_sec, &sec_config, high_speed_only_secondary);

	printk(BIOS_INFO, "DWC3 and PHY setup finished\n");
}
/*
 * setup_usb_host0 - Sets up USB HOST0 controller.
 * Initializes and configures the USB HOST0 controller, including clocks,
 * PHY resets, and DWC3 core for host mode.
*/
void setup_usb_host0(void)
{
	printk(BIOS_INFO, "Setting up USB HOST controller.\n");
	setup_usb_host(&usb_ports);
}

/*
 * usb_update_refclk_for_core - Updates USB reference clock for specified core
 * @core_num: USB core number (0-4)
 * @enable: true to enable, false to disable reference clock
 */
void usb_update_refclk_for_core(u32 core_num, bool enable)
{
	u32 value = enable ? USB_CLKREF_ENABLE_VALUE : 0;

	switch (core_num) {
	case 1:
		clrsetbits32(TCSR_GCC_USB4_1_CLKREF_EN_ADDR, 0x1, value);
		clrsetbits32(TCSR_GCC_USB2_1_CLKREF_EN_ADDR, 0x1, value);
		break;
	case 2:
		clrsetbits32(TCSR_GCC_USB4_2_CLKREF_EN_ADDR, 0x1, value);
		clrsetbits32(TCSR_GCC_USB2_2_CLKREF_EN_ADDR, 0x1, value);
		break;
	case 3:
		clrsetbits32(TCSR_GCC_USB3_MP0_CLKREF_EN_ADDR, 0x1, value);
		clrsetbits32(TCSR_GCC_USB3_MP1_CLKREF_EN_ADDR, 0x1, value);
		break;
	default:
		/* No clkref */
		break;
	}
}

/*
 * enable_vbus_ss - Enables VBUS SuperSpeed for specified USB core
 * @config: Controller configuration containing SMB slave address
 */
void enable_vbus_ss(const struct dwc3_controller_config *config)
{
	u8 slave = config->smb_slave_addr;
	u8 misc_status, otg_status = 0, otg_state = 0;
	struct stopwatch sw;

	printk(BIOS_INFO, "Enabling %s VBUS SuperSpeed\n", config->name);

	/* Configure SDCDC CFG7 register before enabling OTG */
	spmi_write8(SPMI_ADDR(slave, SCHG_DCDC_ENG_SDCDC_CFG7),
		    SCHG_DCDC_ENG_SDCDC_GM_CLOOP_PD_OTG_BUCK_MASK);

	spmi_write8(SPMI_ADDR(slave, SCHG_DCDC_OTG_CFG), 0x20);
	spmi_write8(SPMI_ADDR(slave, SCHG_DCDC_CMD_OTG), 0x1);

	/* Check Type-C mode */
	misc_status = spmi_read8(SPMI_ADDR(slave, SCHG_TYPE_C_TYPE_C_MISC_STATUS));

	/* Check SNK_SRC_MODE bit (bit 6): 0 = SNK, 1 = SRC */
	if (misc_status & TYPEC_SNK_SRC_MODE) {
		/* In SRC mode, poll OTG status until enabled */
		stopwatch_init_msecs_expire(&sw, OTG_STATUS_TIMEOUT_MS);
		while (!stopwatch_expired(&sw)) {
			otg_status = spmi_read8(SPMI_ADDR(slave, SCHG_DCDC_OTG_STATUS));
			otg_state = otg_status & OTG_STATE_MASK;

			if (otg_state == OTG_STATE_ENABLED) {
				printk(BIOS_INFO, "%s in SRC mode - OTG Status: 0x%02x, State: 0x%02x (OTG Enabled)\n",
				       config->name, otg_status, otg_state);
				return;
			}
			mdelay(OTG_STATUS_POLL_DELAY_MS);
		}

		/* Timeout - log final state */
		printk(BIOS_INFO, "%s in SRC mode - OTG Status: 0x%02x, State: 0x%02x - Timeout after %d ms\n",
		       config->name, otg_status, otg_state, OTG_STATUS_TIMEOUT_MS);
	} else {
		printk(BIOS_INFO, "%s in SNK mode - skipping OTG status read\n", config->name);
	}
}

/*
 * usb_typec_status_check - Reads comprehensive Type-C status from PMIC
 * @config: Controller configuration containing SMB slave address
 */
void usb_typec_status_check(const struct dwc3_controller_config *config)
{
	u8 slave = config->smb_slave_addr;
	u8 misc_status, src_status, mode_cfg;

	misc_status = spmi_read8(SPMI_ADDR(slave, SCHG_TYPE_C_TYPE_C_MISC_STATUS));
	src_status = spmi_read8(SPMI_ADDR(slave, SCHG_TYPE_C_TYPE_C_SRC_STATUS));
	mode_cfg = spmi_read8(SPMI_ADDR(slave, SCHG_TYPE_C_TYPE_C_MODE_CFG));

	printk(BIOS_INFO, "%s Type-C Status:\n", config->name);
	printk(BIOS_INFO, "  Misc Status (0x2B0B): 0x%02x\n", misc_status);
	printk(BIOS_INFO, "  Src Status (0x2B08): 0x%02x\n", src_status);
	printk(BIOS_INFO, "  Mode Config (0x2B44): 0x%02x\n", mode_cfg);
}
