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
	u32 *usb3_bcr;
};

static struct usb_dwc3_cfg usb_ports = {
	.usb_host_dwc3 = (void *)USB_HOST_DWC3_MP_BASE,
	.usb3_bcr = &gcc->gcc_usb30_mp_bcr,
};

static bool hs_speed_only;
static u32 *usb3_general_cfg_addr = (void *)USB_HOST_DWC3_MP_GENERAL_CFG_ADDR;

/* Configure TCSR QREFS CXO repeater/receiver registers */
void enable_clock_tcsr(void)
{
	write32(TCSR_QREFS_CXO_RX1_CONFIG_ADDR, 0x3);
	write32(TCSR_QREFS_CXO_1_RPT0_CONFIG_ADDR, 0x3);
	write32(TCSR_QREFS_CXO_RPT1_CONFIG_ADDR, 0x3);
	write32(TCSR_QREFS_CXO_RX5_CONFIG_ADDR, 0x3);
}

/* Enable USB Type-A (MP) clocks and GDSCs */
enum cb_err qcom_enable_usb_clk(void)
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
	usb_update_refclk_for_core(3, true);

	/* Switch MP SS PHY PIPE clocks to PHY source after PHY init */
	if (usb_clock_configure_mux(USB3_PHY_PIPE_0, USB_PHY_PIPE_SRC_SEL) != CB_SUCCESS)
		printk(BIOS_ERR, "USB3 PHY PIPE 0: PHY mux config failed\n");

	if (usb_clock_configure_mux(USB3_PHY_PIPE_1, USB_PHY_PIPE_SRC_SEL) != CB_SUCCESS)
		printk(BIOS_ERR, "USB3 PHY PIPE 1: PHY mux config failed\n");

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

/*
 * setup_usb_host - Initialize DWC3 controller and USB PHYs for Type-A (MP) ports
 * @dwc3: USB DWC3 configuration containing controller base addresses
 *
 * Performs USB initialization sequence for Type-A (MP) ports:
 *   1. Enable clocks and GDSCs
 *   2. Reset and initialize MP HS/SS PHYs
 *   3. Configure DWC3 controller in host mode
 *   4. Enable VBUS for Type-A ports
 */
static void setup_usb_host(struct usb_dwc3_cfg *dwc3)
{
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

	hs_usb_phy_init(0);
	hs_usb_phy_init(1);

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

	setup_dwc3(dwc3->usb_host_dwc3);
	setup_usb_typea_vbus();

	printk(BIOS_INFO, "USB MP: DWC3 and PHY initialization complete\n");
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
 * @core_num: USB core number (0-4)
 * @enable: true to enable, false to disable reference clock
 */
void usb_update_refclk_for_core(u32 core_num, bool enable)
{
	u32 value = enable ? USB_CLKREF_ENABLE_VALUE : 0;

	switch (core_num) {
	case 3:
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
