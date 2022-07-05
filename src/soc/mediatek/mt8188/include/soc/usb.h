/* SPDX-License-Identifier: GPL-2.0-only */

/*
 * This file is created based on MT8188 Functional Specification
 * Chapter number: 5.5
 */

#ifndef SOC_MEDIATEK_MT8188_USB_H
#define SOC_MEDIATEK_MT8188_USB_H

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

#define USB_PORT_NUMBER		1

#endif
