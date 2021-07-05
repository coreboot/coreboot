/* SPDX-License-Identifier: GPL-2.0-only */

#include "qmp_usb_phy.h"

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

void hs_usb_phy_init(void  *board_data);
void setup_usb_host0(void  *board_data);

/* Call reset_ before setup_ */
void reset_usb0(void);
