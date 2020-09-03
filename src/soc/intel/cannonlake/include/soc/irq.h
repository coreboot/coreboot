/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef _SOC_IRQ_H_
#define _SOC_IRQ_H_

#define GPIO_IRQ14 14
#define GPIO_IRQ15 15

#define PCH_IRQ10 10
#define PCH_IRQ11 11

#define SCI_IRQ9 9
#define SCI_IRQ10 10
#define SCI_IRQ11 11
#define SCI_IRQ20 20
#define SCI_IRQ21 21
#define SCI_IRQ22 22
#define SCI_IRQ23 23

#define TCO_IRQ9 9
#define TCO_IRQ10 10
#define TCO_IRQ11 11
#define TCO_IRQ20 20
#define TCO_IRQ21 21
#define TCO_IRQ22 22
#define TCO_IRQ23 23

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
#define SDIO_IRQ 22

#define cAVS_INTA_IRQ 16
#define SMBUS_INTA_IRQ 16
#define SMBUS_INTB_IRQ 17
#define GbE_INTA_IRQ 16
#define GbE_INTC_IRQ 18
#define TRACE_HUB_INTA_IRQ 16
#define TRACE_HUB_INTD_IRQ 19

#define eMMC_IRQ 16
#define SD_IRQ 19

#define PCIE_1_IRQ 16
#define PCIE_2_IRQ 17
#define PCIE_3_IRQ 18
#define PCIE_4_IRQ 19
#define PCIE_5_IRQ 16
#define PCIE_6_IRQ 17
#define PCIE_7_IRQ 18
#define PCIE_8_IRQ 19
#define PCIE_9_IRQ 16
#define PCIE_10_IRQ 17
#define PCIE_11_IRQ 18
#define PCIE_12_IRQ 19
#define PCIE_14_IRQ 16
#define PCIE_15_IRQ 17
#define PCIE_16_IRQ 18
#define PCIE_17_IRQ 19
#define PCIE_18_IRQ 16
#define PCIE_19_IRQ 17
#define PCIE_20_IRQ 18
#define PCIE_21_IRQ 19

#define SATA_IRQ 16

#define HECI_1_IRQ 16
#define HECI_2_IRQ 17
#define IDER_IRQ 18
#define KT_IRQ 19
#define HECI_3_IRQ 16

#define XHCI_IRQ 16
#define OTG_IRQ 17
#define PMC_SRAM_IRQ 18
#define THERMAL_IRQ 16
#define CNViWIFI_IRQ 19
#define UFS_IRQ 16
#define CIO_INTA_IRQ 16
#define CIO_INTD_IRQ 19
#define ISH_IRQ 20

#define PEG_RP_INTA_IRQ 16
#define PEG_RP_INTB_IRQ 17
#define PEG_RP_INTC_IRQ 18
#define PEG_RP_INTD_IRQ 19

#define IGFX_IRQ 16
#define SA_THERMAL_IRQ 16
#define IPU_IRQ 16
#define GNA_IRQ 16
#endif /* _SOC_IRQ_H_ */
