/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef __SOC_NVIDIA_TEGRA_USB_H__
#define __SOC_NVIDIA_TEGRA_USB_H__

enum usb_phy_type {		/* For use in lpm_ctrl[31:29] */
	USB_PHY_UTMIP = 0,
	USB_PHY_ULPI = 2,
	USB_PHY_ICUSB_SER = 3,
	USB_PHY_HSIC = 4,
};

void usb_setup_utmip(void *usb_base);

#endif
