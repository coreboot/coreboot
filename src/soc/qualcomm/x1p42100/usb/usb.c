/* SPDX-License-Identifier: GPL-2.0-only */

#include <console/console.h>
#include <device/mmio.h>
#include <soc/usb/usb.h>
#include <soc/addressmap.h>
#include <soc/clock.h>
#include <delay.h>
#include <gpio.h>

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
	u32 *usb3_bcr;
	u32 *qusb2phy_bcr;
	u32 *qusb2phy1_bcr;
	u32 *gcc_usb3phy_bcr_reg;
	u32 *gcc_qmpphy_bcr_reg;
	u32 *gcc_usb3phy1_bcr_reg;
	u32 *gcc_qmpphy1_bcr_reg;
	u32 *qusb2secphy_bcr;
	u32 *gcc_usb3secphy_bcr_reg;
	u32 *gcc_qmpsecphy_bcr_reg;
};

static struct usb_dwc3_cfg usb_port0 = {
	.usb_host_dwc3 =	(void *)USB_HOST_DWC3_BASE,
	.usb3_bcr =		&gcc->gcc_usb30_mp_bcr,
	.qusb2phy_bcr =		&gcc->qusb2phy_hs0_mp_bcr,
	.gcc_usb3phy_bcr_reg =	&gcc->usb3uniphy_phy_mp0_bcr,
	.gcc_qmpphy_bcr_reg =	&gcc->usb3_uniphy_mp0_bcr,
	.qusb2phy1_bcr =		&gcc->qusb2phy_hs1_mp_bcr,
	.gcc_usb3phy1_bcr_reg =	&gcc->usb3uniphy_phy_mp1_bcr,
	.gcc_qmpphy1_bcr_reg =	&gcc->usb3_uniphy_mp1_bcr,
};

bool hs_speed_only = false;
u32 *usb3_general_cfg_addr = (void *)USB_HOST_DWC3_GENERAL_CFG_ADDR;

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
	/* Set USB3 PHY PIPE 0 clock source to X0  */
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

	write32(TCSR_GCC_USB3_MP0_CLKREF_EN_ADDR, USB3_CLKREF_ENABLE_VALUE);
	write32(TCSR_GCC_USB3_MP1_CLKREF_EN_ADDR, USB3_CLKREF_ENABLE_VALUE);
	/* Set USB3 PHY PIPE 1 clock source to USB PHY */

	if (usb_clock_configure_mux(USB3_PHY_PIPE_0, USB_PHY_PIPE_SRC_SEL)) {
		printk(BIOS_ERR, "%s(): USB3 PHY PIPE 1 clock enable failed\n", __func__);
		return -1;
	}

	if (usb_clock_configure_mux(USB3_PHY_PIPE_1, USB_PHY_PIPE_SRC_SEL)) {
		printk(BIOS_ERR, "%s(): USB3 PHY PIPE 1 clock enable failed\n", __func__);
		return -1;
	}

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
	setbits32(USB3_MP_CGCTL_REG_ADDR, USB3_MP_DBM_FSM_EN_BIT);
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

/* Initialization of DWC3 Core and PHY */
static void setup_usb_host(struct usb_dwc3_cfg *dwc3)
{
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

	setup_dwc3(dwc3->usb_host_dwc3);

	printk(BIOS_INFO, "DWC3 and PHY setup finished\n");
}
/*
 * setup_usb_host0 - Sets up USB HOST0 controller.
 * Initializes and configures the USB HOST0 controller, including clocks,
 * PHY resets, and DWC3 core for host mode.
*/
void setup_usb_host0(void)
{
	printk(BIOS_INFO, "Setting up USB HOST0 controller.\n");
	setup_usb_host(&usb_port0);
}
