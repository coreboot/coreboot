/*
 * This file is part of the coreboot project.
 *
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 and
 * only version 2 as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */
#include <types.h>

#ifndef _QCS405_USB_H_
#define _QCS405_USB_H_

/* QSCRATCH_GENERAL_CFG register bit offset */
#define PIPE_UTMI_CLK_SEL			BIT(0)
#define PIPE3_PHYSTATUS_SW			BIT(3)
#define PIPE_UTMI_CLK_DIS			BIT(8)

/* Global USB3 Control  Registers */
#define DWC3_GUSB3PIPECTL_DELAYP1TRANS		BIT(18)
#define DWC3_GUSB3PIPECTL_UX_EXIT_IN_PX		BIT(27)
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

#define DWC3_GCTL_SCALEDOWN(n)			((n) << 4)
#define DWC3_GCTL_SCALEDOWN_MASK		DWC3_GCTL_SCALEDOWN(3)
#define DWC3_GCTL_DISSCRAMBLE			(1 << 3)
#define DWC3_GCTL_U2EXIT_LFPS			(1 << 2)
#define DWC3_GCTL_DSBLCLKGTNG			(1 << 0)

/* USB2 PHY register values */
#define USB2PHY_TCSR_CTRL			0x01
#define USB2PHY_REFCLK_CTRL			0x0d
#define USB2PHY_UTMI_CTRL5			0x12
#define USB2PHY_PARAMETER_OVERRIDE_X0		0x63
#define USB2PHY_PARAMETER_OVERRIDE_X1		0x03
#define USB2PHY_PARAMETER_OVERRIDE_X2		0x1d
#define USB2PHY_PARAMETER_OVERRIDE_X3		0x03
#define USB2PHY_HS_PHY_CTRL1			0x23
#define QUSB2PHY_HS_PHY_CTRL_COMMON0		0x08
#define QUSB2PHY_HS_PHY_CTRL_COMMON1		0xdc
#define USB2PHY_HS_PHY_CTRL2			0xe0
#define USB2PHY_UTMI_CTRL5_POR_CLEAR		0x10
#define USB2PHY_HS_PHY_CTRL2_SUSPEND_N_SEL	0x60

struct usb_board_data {
	/* Register values going to override from the boardfile */
	u8 parameter_override_x0;
	u8 parameter_override_x1;
	u8 parameter_override_x2;
	u8 parameter_override_x3;
};

enum usb_port {
	HSUSB_SS_PORT_0,
	HSUSB_HS_PORT_1,
};

void setup_usb_host(enum usb_port port, struct usb_board_data *data);
/* Call reset_ before setup_ */
void reset_usb(enum usb_port port);

#endif /* _QCS405_USB_H_ */
