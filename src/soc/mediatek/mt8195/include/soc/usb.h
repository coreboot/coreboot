/* SPDX-License-Identifier: GPL-2.0-only OR MIT */

#ifndef SOC_MEDIATEK_MT8195_USB_H
#define SOC_MEDIATEK_MT8195_USB_H

#include <soc/usb_common.h>

struct ssusb_sif_port {
	struct sif_u2_phy_com u2phy;
	u32 reserved0[64 * 5];
	struct sif_u3phyd u3phyd;
	u32 reserved1[64];
	struct sif_u3phya u3phya;
	struct sif_u3phya_da u3phya_da;
	u32 reserved2[64 * 3];
};
check_member(ssusb_sif_port, u3phyd, 0x600);
check_member(ssusb_sif_port, u3phya, 0x800);
check_member(ssusb_sif_port, u3phya_da, 0x900);
check_member(ssusb_sif_port, reserved2, 0xa00);

DEFINE_BIT(AUTO_LOAD_DIS, 12)
DEFINE_BITFIELD(TX_IMP_CAL, 28, 24)
DEFINE_BIT(TX_IMP_CAL_EN, 31)
DEFINE_BITFIELD(RX_IMP_CAL, 28, 24)
DEFINE_BIT(RX_IMP_CAL_EN, 31)
DEFINE_BITFIELD(INTR_CAL, 15, 10)

#define TX_IMP_MASK		0x1F
#define TX_IMP_SHIFT		0
#define RX_IMP_MASK		0x3E0
#define RX_IMP_SHIFT		5
#define INTR_CAL_MASK		0xFC00
#define INTR_CAL_SHIFT		10

#define USB_PHY_SETTING_REG	0x11C10184
#define USB_PORT_NUMBER		1

#endif
