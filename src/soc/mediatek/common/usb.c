/* SPDX-License-Identifier: GPL-2.0-only */

#include <device/mmio.h>
#include <console/console.h>
#include <soc/addressmap.h>
#include <soc/usb.h>
#include <timer.h>

#define USBTAG	"[SSUSB] "
#define u3p_msg(fmt, arg...)   printk(BIOS_INFO, USBTAG fmt, ##arg)
#define u3p_err(fmt, arg...)   printk(BIOS_ERR, USBTAG fmt, ##arg)

static struct ssusb_ippc_regs *ippc_regs = (void *)(SSUSB_IPPC_BASE);
static struct ssusb_sif_port *phy_ports = (void *)(SSUSB_SIF_BASE);

void update_usb_base_regs(uintptr_t ippc_base, uintptr_t sif_base)
{
	ippc_regs = (void *)ippc_base;
	phy_ports = (void *)sif_base;
}

static void phy_index_power_on(int index)
{
	struct ssusb_sif_port *phy = phy_ports + index;

	if (!index) {
		/* Set RG_SSUSB_VUSB10_ON as 1 after VUSB10 ready */
		setbits32(&phy->u3phya.phya_reg0, P3A_RG_U3_VUSB10_ON);
		/* Disable power domain ISO */
		clrbits32(&phy->u2phy.usbphyacr6, PA6_RG_U2_ISO_EN);
	}
	/* Switch system IP to USB mode */
	clrbits32(&phy->u2phy.u2phydtm0, P2C_FORCE_UART_EN);
	clrbits32(&phy->u2phy.u2phydtm1, P2C_RG_UART_EN);
	if (!index)
		clrbits32(&phy->u2phy.u2phyacr4, P2C_U2_GPIO_CTR_MSK);

	/* Disable force settings */
	clrbits32(&phy->u2phy.u2phydtm0, P2C_FORCE_SUSPENDM |
		P2C_RG_XCVRSEL | P2C_RG_DATAIN | P2C_DTM0_PART_MASK);

	clrbits32(&phy->u2phy.usbphyacr6, PA6_RG_U2_BC11_SW_EN);
	/* Improve Rx sensitivity */
	clrsetbits32(&phy->u2phy.usbphyacr6,
		PA6_RG_U2_SQTH, PA6_RG_U2_SQTH_VAL(2));

	setbits32(&phy->u2phy.usbphyacr6, PA6_RG_U2_OTG_VBUSCMP_EN);

	clrsetbits32(&phy->u3phya_da.reg0,
		P3A_RG_XTAL_EXT_EN_U3, P3A_RG_XTAL_EXT_EN_U3_VAL(2));

	clrsetbits32(&phy->u3phya.phya_reg9,
		P3A_RG_RX_DAC_MUX, P3A_RG_RX_DAC_MUX_VAL(4));

	if (!index)
		clrbits32(&phy->u2phy.usbphyacr5, PA5_RG_U2_HS_100U_U3_EN);

	clrsetbits32(&phy->u3phya.phya_reg6,
		P3A_RG_TX_EIDLE_CM, P3A_RG_TX_EIDLE_CM_VAL(0xe));

	clrsetbits32(&phy->u3phyd.phyd_cdr1,
		P3D_RG_CDR_BIR_LTD0, P3D_RG_CDR_BIR_LTD0_VAL(0xc));
	clrsetbits32(&phy->u3phyd.phyd_cdr1,
		P3D_RG_CDR_BIR_LTD1, P3D_RG_CDR_BIR_LTD1_VAL(0x3));

	clrsetbits32(&phy->u2phy.u2phydtm1,
		P2C_RG_SESSEND, P2C_RG_VBUSVALID | P2C_RG_AVALID);

	/* Set USB 2.0 slew rate value */
	clrsetbits32(&phy->u2phy.usbphyacr5,
		PA5_RG_U2_HSTX_SRCTRL, PA5_RG_U2_HSTX_SRCTRL_VAL(4));

	/* Set USB 2.0 disconnect threshold */
	clrsetbits32(&phy->u2phy.usbphyacr6,
		PA6_RG_U2_DISCTH, PA6_RG_U2_DISCTH_VAL(15));
}

static void u3phy_power_on(void)
{
	for (int i = 0; i < USB_PORT_NUMBER; i++)
		phy_index_power_on(i);
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
			u3p_err("USB clocks are not stable!!!\n");
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

	/* Power on host ip */
	clrbits32(&ippc_regs->ip_pw_ctr1, CTRL1_IP_HOST_PDN);

	/* Power on and enable all u3 ports */
	for (i = 0; i < u3_port_num; i++) {
		clrsetbits32(&ippc_regs->u3_ctrl_p[i],
			CTRL_U3_PORT_PDN | CTRL_U3_PORT_DIS,
			CTRL_U3_PORT_HOST_SEL);
	}

	/* Power on and enable all u2 ports */
	for (i = 0; i < u2_port_num; i++) {
		clrsetbits32(&ippc_regs->u2_ctrl_p[i],
			CTRL_U2_PORT_PDN | CTRL_U2_PORT_DIS,
			CTRL_U2_PORT_HOST_SEL);
	}
	return check_ip_clk_status();
}

static inline void ssusb_soft_reset(void)
{
	/* Reset whole ip */
	setbits32(&ippc_regs->ip_pw_ctr0, CTRL0_IP_SW_RST);
	clrbits32(&ippc_regs->ip_pw_ctr0, CTRL0_IP_SW_RST);
}

__weak void mtk_usb_prepare(void)
{
	/* do nothing */
}

__weak void mtk_usb_adjust_phy_shift(void)
{
	/* do nothing */
}

void setup_usb_host_controller(void)
{
	u3p_msg("Setting up USB HOST controller...\n");

	mtk_usb_prepare();
	ssusb_soft_reset();
	if (u3phy_ports_enable()) {
		u3p_err("%s fail to enable ports\n", __func__);
		return;
	}
	u3phy_power_on();
	mtk_usb_adjust_phy_shift();
	u3p_msg("phy power-on done.\n");
}

void setup_usb_host(void)
{
	update_usb_base_regs(SSUSB_IPPC_BASE, SSUSB_SIF_BASE);
	setup_usb_host_controller();
}
