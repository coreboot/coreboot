/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2012 Samsung Electronics
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */

#ifndef CPU_SAMSUNG_EXYNOS5420_USB_H
#define CPU_SAMSUNG_EXYNOS5420_USB_H

#define CLK_24MHZ               5

#define HOST_CTRL0_PHYSWRSTALL                  (1 << 31)
#define HOST_CTRL0_COMMONON_N                   (1 << 9)
#define HOST_CTRL0_SIDDQ                        (1 << 6)
#define HOST_CTRL0_FORCESLEEP                   (1 << 5)
#define HOST_CTRL0_FORCESUSPEND                 (1 << 4)
#define HOST_CTRL0_WORDINTERFACE                (1 << 3)
#define HOST_CTRL0_UTMISWRST                    (1 << 2)
#define HOST_CTRL0_LINKSWRST                    (1 << 1)
#define HOST_CTRL0_PHYSWRST                     (1 << 0)

#define HOST_CTRL0_FSEL_MASK                    (7 << 16)

#define EHCICTRL_ENAINCRXALIGN                  (1 << 29)
#define EHCICTRL_ENAINCR4                       (1 << 28)
#define EHCICTRL_ENAINCR8                       (1 << 27)
#define EHCICTRL_ENAINCR16                      (1 << 26)

/* Register map for PHY control */
struct usb_phy {
        uint32_t usbphyctrl0;
        uint32_t usbphytune0;
        uint32_t reserved1[2];
        uint32_t hsicphyctrl1;
        uint32_t hsicphytune1;
        uint32_t reserved2[2];
        uint32_t hsicphyctrl2;
        uint32_t hsicphytune2;
        uint32_t reserved3[2];
        uint32_t ehcictrl;
        uint32_t ohcictrl;
        uint32_t usbotgsys;
        uint32_t reserved4;
        uint32_t usbotgtune;
};

void usb_init(device_t dev);

#endif
