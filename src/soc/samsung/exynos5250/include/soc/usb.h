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
 */

#ifndef CPU_SAMSUNG_EXYNOS5250_USB_H
#define CPU_SAMSUNG_EXYNOS5250_USB_H

#include <soc/cpu.h>

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
struct exynos5_usb_host_phy {
        uint32_t usbphyctrl0;
        uint32_t usbphytune0;
        uint8_t reserved1[8];
        uint32_t hsicphyctrl1;
        uint32_t hsicphytune1;
        uint8_t reserved2[8];
        uint32_t hsicphyctrl2;
        uint32_t hsicphytune2;
        uint8_t reserved3[8];
        uint32_t ehcictrl;
        uint32_t ohcictrl;
        uint32_t usbotgsys;
        uint8_t reserved4[4];
        uint32_t usbotgtune;
};
check_member(exynos5_usb_host_phy, usbotgtune, 0x40);

static struct exynos5_usb_host_phy * const exynos_usb_host_phy =
		(void *)EXYNOS5_USB_HOST_PHY_BASE;

struct exynos5_usb_drd_phy {
	uint8_t reserved1[4];
	uint32_t linksystem;
	uint32_t utmi;
	uint32_t pipe;
	uint32_t clkrst;
	uint32_t reg0;
	uint32_t reg1;
	uint32_t param0;
	uint32_t param1;
	uint32_t term;
	uint32_t test;
	uint32_t adp;
	uint32_t utmiclksel;
	uint32_t resume;
	uint8_t reserved2[8];
	uint32_t linkhcbelt;
	uint32_t linkport;
};
check_member(exynos5_usb_drd_phy, linkport, 0x44);

static struct exynos5_usb_drd_phy * const exynos_usb_drd_phy =
		(void *)EXYNOS5_USB_DRD_PHY_BASE;

struct exynos5_usb_drd_dwc3 {
	uint32_t sbuscfg0;
	uint32_t sbuscfg1;
	uint32_t txthrcfg;
	uint32_t rxthrcfg;
	uint32_t ctl;
	uint32_t evten;
	uint32_t sts;
	uint8_t reserved0[4];
	uint32_t snpsid;
	uint32_t gpio;
	uint32_t uid;
	uint32_t uctl;
	uint64_t buserraddr;
	uint64_t prtbimap;
	uint8_t reserved1[32];
	uint32_t dbgfifospace;
	uint32_t dbgltssm;
	uint32_t dbglnmcc;
	uint32_t dbgbmu;
	uint32_t dbglspmux;
	uint32_t dbglsp;
	uint32_t dbgepinfo0;
	uint32_t dbgepinfo1;
	uint64_t prtbimap_hs;
	uint64_t prtbimap_fs;
	uint8_t reserved2[112];
	uint32_t usb2phycfg;
	uint8_t reserved3[60];
	uint32_t usb2i2cctl;
	uint8_t reserved4[60];
	uint32_t usb2phyacc;
	uint8_t reserved5[60];
	uint32_t usb3pipectl;
	uint8_t reserved6[60];
};
check_member(exynos5_usb_drd_dwc3, usb3pipectl, 0x1c0);

static struct exynos5_usb_drd_dwc3 * const exynos_usb_drd_dwc3 =
		(void *)EXYNOS5_USB_DRD_DWC3_BASE;

/* Leave hsic_gpio at 0 to not enable HSIC. */
void setup_usb_host_phy(int hsic_gpio);

void setup_usb_drd_phy(void);

/* Call reset_ before setup_, ensure at least 100ms pass in between. */
void reset_usb_drd_dwc3(void);
void setup_usb_drd_dwc3(void);

#endif
