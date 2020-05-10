/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef SOC_MEDIATEK_MT8173_USB_H
#define SOC_MEDIATEK_MT8173_USB_H

#include <soc/usb_common.h>

struct ssusb_sif_port {
	struct sif_u2_phy_com u2phy;
	struct sif_u3phyd u3phyd;
	u32 reserved0[64];
	struct sif_u3phya u3phya;
	struct sif_u3phya_da u3phya_da;
	u32 reserved1[64 * 3];
};
check_member(ssusb_sif_port, u3phyd, 0x100);
check_member(ssusb_sif_port, u3phya, 0x300);
check_member(ssusb_sif_port, u3phya_da, 0x400);
check_member(ssusb_sif_port, reserved1, 0x500);

#define USB_PORT_NUMBER 2

#endif
