/*
 * This file is part of the coreboot project.
 *
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

#ifndef _SOC_JSL_IRQ_H_
#define _SOC_JSL_IRQ_H_

#define GPIO_IRQ14 14
#define GPIO_IRQ15 15

#define PCH_IRQ10 10
#define PCH_IRQ11 11

/* LPSS Devices */
#define LPSS_I2C0_IRQ 16
#define LPSS_I2C1_IRQ 17
#define LPSS_I2C2_IRQ 18
#define LPSS_I2C3_IRQ 19
#define LPSS_I2C4_IRQ 32
#define LPSS_I2C5_IRQ 33
#define LPSS_SPI0_IRQ 22
#define LPSS_SPI1_IRQ 23
#define LPSS_SPI2_IRQ 24
#define LPSS_UART0_IRQ 20
#define LPSS_UART1_IRQ 21
#define LPSS_UART2_IRQ 34

/* PCI D:31 F:x */
#define cAVS_INTA_IRQ 16
#define SMBUS_INTA_IRQ 16
#define SMBUS_INTB_IRQ 17
#define GbE_INTA_IRQ 16
#define GbE_INTC_IRQ 18
#define TRACE_HUB_INTA_IRQ 16
#define TRACE_HUB_INTD_IRQ 19

/* PCI D:28 F:x */
#define PCIE_1_IRQ 16
#define PCIE_2_IRQ 17
#define PCIE_3_IRQ 18
#define PCIE_4_IRQ 19
#define PCIE_5_IRQ 16
#define PCIE_6_IRQ 17
#define PCIE_7_IRQ 18
#define PCIE_8_IRQ 19

/* PCI D:26 F:x */
#define eMMC_IRQ 16

/* PCI D:23 F:x */
#define SATA_IRQ 16

/* PCI D:22 F:x */
#define HECI_1_IRQ 16
#define HECI_2_IRQ 17
#define HECI_3_IRQ 16
#define HECI_4_IRQ 19
#define IDER_IRQ 18
#define KT_IRQ 19

/* PCI D:20 F:x */
#define XHCI_IRQ 16
#define OTG_IRQ 17
#define CNViWIFI_IRQ 16
#define SD_IRQ 19
#define PMC_SRAM_IRQ 18

/* PCI D:18 F:x */
#define UFS_IRQ 16

#define IGFX_IRQ 16
#define SA_THERMAL_IRQ 16
#define IPU_IRQ 16
#define GNA_IRQ 16

#endif /* _JSL_IRQ_H_ */
