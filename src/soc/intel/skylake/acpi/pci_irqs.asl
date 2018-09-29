/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2007-2009 coresystems GmbH
 * Copyright (C) 2014 Google Inc.
 * Copyright (C) 2015 Intel Corporation.
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

#include <soc/interrupt.h>
#include <soc/irq.h>

Name (PICP, Package () {
	/* D31: cAVS, SMBus, GbE, Nothpeak */
	Package () { 0x001FFFFF, 0, 0, cAVS_INTA_IRQ },
	Package () { 0x001FFFFF, 1, 0, SMBUS_INTB_IRQ },
	Package () { 0x001FFFFF, 2, 0, GbE_INTC_IRQ },
	Package () { 0x001FFFFF, 3, 0, TRACE_HUB_INTD_IRQ },
	/* D30: SerialIo and SCS */
	Package () { 0x001EFFFF, 0, 0, LPSS_UART0_IRQ },
	Package () { 0x001EFFFF, 1, 0, eMMC_IRQ },
	Package () { 0x001EFFFF, 2, 0, SDIO_IRQ },
	Package () { 0x001EFFFF, 3, 0, SD_IRQ },
	/* D29: PCI Express Port 9-16 */
	Package () { 0x001DFFFF, 0, 0, PCIE_9_IRQ },
	Package () { 0x001DFFFF, 1, 0, PCIE_10_IRQ },
	Package () { 0x001DFFFF, 2, 0, PCIE_11_IRQ },
	Package () { 0x001DFFFF, 3, 0, PCIE_12_IRQ },
	/* D28: PCI Express Port 1-8 */
	Package () { 0x001CFFFF, 0, 0, PCIE_1_IRQ },
	Package () { 0x001CFFFF, 1, 0, PCIE_2_IRQ },
	Package () { 0x001CFFFF, 2, 0, PCIE_3_IRQ },
	Package () { 0x001CFFFF, 3, 0, PCIE_4_IRQ },
	/* D25: SerialIo */
	Package () { 0x0019FFFF, 0, 0, LPSS_UART2_IRQ },
	Package () { 0x0019FFFF, 1, 0, LPSS_I2C5_IRQ },
	Package () { 0x0019FFFF, 2, 0, LPSS_I2C4_IRQ },
	/* D22: CSME (HECI, IDE-R, KT redirection */
	Package () { 0x0016FFFF, 0, 0, HECI_1_IRQ },
	Package () { 0x0016FFFF, 1, 0, HECI_2_IRQ },
	Package () { 0x0016FFFF, 2, 0, IDER_IRQ },
	Package () { 0x0016FFFF, 3, 0, KT_IRQ },
	/* D23: Sata controller */
	Package () { 0x0017FFFF, 0, 0, SATA_IRQ },
	/* D21: SerialIo */
	Package () { 0x0015FFFF, 0, 0, LPSS_I2C0_IRQ },
	Package () { 0x0015FFFF, 1, 0, LPSS_I2C1_IRQ },
	Package () { 0x0015FFFF, 2, 0, LPSS_I2C2_IRQ },
	Package () { 0x0015FFFF, 3, 0, LPSS_I2C3_IRQ },
	/* D20: xHCI, OTG, Thermal, Camera */
	Package () { 0x0014FFFF, 0, 0, XHCI_IRQ },
	Package () { 0x0014FFFF, 1, 0, OTG_IRQ },
	Package () { 0x0014FFFF, 2, 0, THERMAL_IRQ },
	Package () { 0x0014FFFF, 3, 0, CIO_INTD_IRQ },
	/* D19: Integrated Sensor Hub */
	Package () { 0x0013FFFF, 0, 0, ISH_IRQ },
	/* P.E.G. Root Port D1F0 */
	Package () { 0x0001FFFF, 0, 0, PEG_RP_INTA_IRQ },
	Package () { 0x0001FFFF, 1, 0, PEG_RP_INTB_IRQ },
	Package () { 0x0001FFFF, 2, 0, PEG_RP_INTC_IRQ },
	Package () { 0x0001FFFF, 3, 0, PEG_RP_INTD_IRQ },
	/* SA IGFX Device */
	Package () { 0x0002FFFF, 0, 0, IGFX_IRQ },
	/* SA Thermal Device */
	Package () { 0x0004FFFF, 0, 0, SA_THERMAL_IRQ },
	/* SA SkyCam Device */
	Package () { 0x0005FFFF, 0, 0, SKYCAM_IRQ },
	/* SA GMM Device */
	Package () { 0x0008FFFF, 0, 0, GMM_IRQ },
})

Name (PICN, Package () {
	/* D31: cAVS, SMBus, GbE, Nothpeak */
	Package () { 0x001FFFFF, 0, \_SB.PCI0.LNKA, 0 },
	Package () { 0x001FFFFF, 1, \_SB.PCI0.LNKB, 0 },
	Package () { 0x001FFFFF, 2, \_SB.PCI0.LNKC, 0 },
	Package () { 0x001FFFFF, 3, \_SB.PCI0.LNKD, 0 },
	/* D29: PCI Express Port 9-16 */
	Package () { 0x001DFFFF, 0, \_SB.PCI0.LNKA, 0 },
	Package () { 0x001DFFFF, 1, \_SB.PCI0.LNKB, 0 },
	Package () { 0x001DFFFF, 2, \_SB.PCI0.LNKC, 0 },
	Package () { 0x001DFFFF, 3, \_SB.PCI0.LNKD, 0 },
	/* D28: PCI Express Port 1-8 */
	Package () { 0x001CFFFF, 0, \_SB.PCI0.LNKA, 0 },
	Package () { 0x001CFFFF, 1, \_SB.PCI0.LNKB, 0 },
	Package () { 0x001CFFFF, 2, \_SB.PCI0.LNKC, 0 },
	Package () { 0x001CFFFF, 3, \_SB.PCI0.LNKD, 0 },
	/* D27: PCI Express Port 17-20 */
	Package () { 0x001BFFFF, 0, \_SB.PCI0.LNKA, 0 },
	Package () { 0x001BFFFF, 1, \_SB.PCI0.LNKB, 0 },
	Package () { 0x001BFFFF, 2, \_SB.PCI0.LNKC, 0 },
	Package () { 0x001BFFFF, 3, \_SB.PCI0.LNKD, 0 },
	/* D23 */
	Package () { 0x0017FFFF, 0, \_SB.PCI0.LNKA, 0 },
	/* D22: CSME (HECI, IDE-R, KT redirection */
	Package () { 0x0016FFFF, 0, \_SB.PCI0.LNKA, 0 },
	Package () { 0x0016FFFF, 1, \_SB.PCI0.LNKB, 0 },
	Package () { 0x0016FFFF, 2, \_SB.PCI0.LNKC, 0 },
	Package () { 0x0016FFFF, 3, \_SB.PCI0.LNKD, 0 },
	/* D20: xHCI, OTG, Thermal, Camera */
	Package () { 0x0014FFFF, 0, \_SB.PCI0.LNKA, 0 },
	Package () { 0x0014FFFF, 1, \_SB.PCI0.LNKB, 0 },
	Package () { 0x0014FFFF, 2, \_SB.PCI0.LNKC, 0 },
	Package () { 0x0014FFFF, 3, \_SB.PCI0.LNKD, 0 },
	/* P.E.G. Root Port D1F0 */
	Package () { 0x0001FFFF, 0, \_SB.PCI0.LNKA, 0 },
	Package () { 0x0001FFFF, 1, \_SB.PCI0.LNKB, 0 },
	Package () { 0x0001FFFF, 2, \_SB.PCI0.LNKC, 0 },
	Package () { 0x0001FFFF, 3, \_SB.PCI0.LNKD, 0 },
	/* SA IGFX Device */
	Package () { 0x0002FFFF, 0, \_SB.PCI0.LNKA, 0 },
	/* SA Thermal Device */
	Package () { 0x0004FFFF, 0, \_SB.PCI0.LNKA, 0 },
	/* SA Skycam Device */
	Package () { 0x0005FFFF, 0, \_SB.PCI0.LNKA, 0 },
	/* SA GMM Device */
	Package () { 0x0008FFFF, 0, \_SB.PCI0.LNKA, 0 },
})

Method (_PRT)
{
	If (PICM) {
		Return (^PICP)
	} Else {
		Return (^PICN)
	}
}
