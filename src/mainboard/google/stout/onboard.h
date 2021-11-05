/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef STOUT_ONBOARD_H
#define STOUT_ONBOARD_H

#define STOUT_NIC_VENDOR_ID		0x10EC
#define STOUT_NIC_DEVICE_ID		0x8168

#define XHCI_MODE     2  // Auto
#define XHCI_PORTS    3  // Port 0 & 1
#define XHCI_PREBOOT  0  // No PreOS boot support
#define XHCI_STREAMS  1  // Sure, lets have streams

/* Write protect is active low */
#define GPIO_SPI_WP	7

#endif
