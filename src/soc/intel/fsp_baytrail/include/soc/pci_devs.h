/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2013 Google Inc.
 * Copyright (C) 2013 Sage Electronic Engineering, LLC.
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

#ifndef _BAYTRAIL_PCI_DEVS_H_
#define _BAYTRAIL_PCI_DEVS_H_

#include <device/pci_def.h>

#define BUS0 0

/* All these devices live on bus 0 with the associated device and function */

/* SoC transaction router */
#define SOC_DEV 0x0
#define SOC_FUNC 0
# define SOC_DEVFN_SOC PCI_DEVFN(SOC_DEV,SOC_FUNC)


/* Graphics and Display */
#define GFX_DEV 0x2
#define GFX_FUNC 0
# define SOC_DEVFN_GFX PCI_DEVFN(GFX_DEV,GFX_FUNC)

/* MIPI */
#define MIPI_DEV 0x3
#define MIPI_FUNC 0
# define SOC_DEVFN_MIPI PCI_DEVFN(MIPI_DEV,MIPI_FUNC)


/* SDIO Port */
#define EMMC_DEV 0x10
#define EMMC_FUNC 0
# define SOC_DEVFN_EMMC PCI_DEVFN(EMMC_DEV,EMMC_FUNC)

/* SDIO Port */
#define SDIO_DEV 0x11
#define SDIO_FUNC 0
# define SOC_DEVFN_SDIO PCI_DEVFN(SDIO_DEV,SDIO_FUNC)

/* SD Port */
#define SD_DEV 0x12
#define SD_FUNC 0
# define SOC_DEVFN_SD PCI_DEVFN(SD_DEV,SD_FUNC)

/* SATA */
#define SATA_DEV 0x13
#define SATA_FUNC 0
# define SOC_DEVFN_SATA PCI_DEVFN(SATA_DEV,SATA_FUNC)

/* xHCI */
#define XHCI_DEV 0x14
#define XHCI_FUNC 0
# define XHCI_FUS_REG	0xE0
# define XHCI_FUNC_DISABLE	(1 << 0)
# define XHCI_USB2PR_REG 0xD0
# define SOC_DEVFN_XHCI PCI_DEVFN(XHCI_DEV,XHCI_FUNC)

/* LPE Audio */
#define LPE_DEV 0x15
#define LPE_FUNC 0
# define SOC_DEVFN_LPE PCI_DEVFN(LPE_DEV,LPE_FUNC)

/* OTG */
#define OTG_DEV 0x16
#define OTG_FUNC 0
# define SOC_DEVFN_OTG PCI_DEVFN(LPE_DEV,LPE_FUNC)

/* MMC Port */
#define MMC45_DEV 0x17
#define MMC45_FUNC 0
# define SOC_DEVFN_MMC45 PCI_DEVFN(MMC45_DEV,MMC45_FUNC)

/* Serial IO 1 */
#define SIO1_DEV 0x18
# define SIO_DMA1_DEV SIO1_DEV
# define SIO_DMA1_FUNC 0
# define I2C1_DEV SIO1_DEV
# define I2C1_FUNC 1
# define I2C2_DEV SIO1_DEV
# define I2C2_FUNC 2
# define I2C3_DEV SIO1_DEV
# define I2C3_FUNC 3
# define I2C4_DEV SIO1_DEV
# define I2C4_FUNC 4
# define I2C5_DEV SIO1_DEV
# define I2C5_FUNC 5
# define I2C6_DEV SIO1_DEV
# define I2C6_FUNC 6
# define I2C7_DEV SIO1_DEV
# define I2C7_FUNC 7
# define SOC_DEVFN_SIO_DMA1 PCI_DEVFN(SIO_DMA1_DEV,SIO_DMA1_FUNC)
# define SOC_DEVFN_I2C1 PCI_DEVFN(I2C1_DEV,I2C1_FUNC)
# define SOC_DEVFN_I2C2 PCI_DEVFN(I2C2_DEV,I2C2_FUNC)
# define SOC_DEVFN_I2C3 PCI_DEVFN(I2C3_DEV,I2C3_FUNC)
# define SOC_DEVFN_I2C4 PCI_DEVFN(I2C4_DEV,I2C4_FUNC)
# define SOC_DEVFN_I2C5 PCI_DEVFN(I2C5_DEV,I2C5_FUNC)
# define SOC_DEVFN_I2C6 PCI_DEVFN(I2C6_DEV,I2C6_FUNC)
# define SOC_DEVFN_I2C7 PCI_DEVFN(I2C7_DEV,I2C7_FUNC)

#define PCH_DEV_SLOT_I2C1 I2C1_DEV

/* Trusted Execution Engine */
#define TXE_DEV 0x1a
#define TXE_FUNC 0
# define SOC_DEVFN_TXE PCI_DEVFN(TXE_DEV,TXE_FUNC)

/* HD Audio */
#define HDA_DEV 0x1b
#define HDA_FUNC 0
# define SOC_DEVFN_HDA PCI_DEVFN(HDA_DEV,HDA_FUNC)

/* PCIe Ports */
#define PCIE_DEV 0x1c
# define PCIE_PORT1_DEV PCIE_DEV
# define PCIE_PORT1_FUNC 0
# define PCIE_PORT2_DEV PCIE_DEV
# define PCIE_PORT2_FUNC 1
# define PCIE_PORT3_DEV PCIE_DEV
# define PCIE_PORT3_FUNC 2
# define PCIE_PORT4_DEV PCIE_DEV
# define PCIE_PORT4_FUNC 3
# define SOC_DEVFN_PCIE_PORT1 PCI_DEVFN(PCIE_DEV,PCIE_PORT1_FUNC)
# define SOC_DEVFN_PCIE_PORT2 PCI_DEVFN(PCIE_DEV,PCIE_PORT2_FUNC)
# define SOC_DEVFN_PCIE_PORT3 PCI_DEVFN(PCIE_DEV,PCIE_PORT3_FUNC)
# define SOC_DEVFN_PCIE_PORT4 PCI_DEVFN(PCIE_DEV,PCIE_PORT4_FUNC)

/* EHCI */
#define EHCI_DEV 0x1d
#define EHCI_FUNC 0
# define SOC_DEVFN_EHCI PCI_DEVFN(EHCI_DEV,EHCI_FUNC)

/* Serial IO 2 */
#define SIO2_DEV 0x1e
# define SIO_DMA2_DEV SIO2_DEV
# define SIO_DMA2_FUNC 0
# define PWM1_DEV SIO2_DEV
# define PWM1_FUNC 1
# define PWM2_DEV SIO2_DEV
# define PWM2_FUNC 2
# define HSUART1_DEV SIO2_DEV
# define HSUART1_FUNC 3
# define HSUART2_DEV SIO2_DEV
# define HSUART2_FUNC 4
# define SPI_DEV SIO2_DEV
# define SPI_FUNC 5
# define SOC_DEVFN_SIO_DMA2 PCI_DEVFN(SIO_DMA2_DEV,SIO_DMA2_FUNC)
# define SOC_DEVFN_PWM1 PCI_DEVFN(PWM1_DEV,PWM1_FUNC)
# define SOC_DEVFN_PWM2 PCI_DEVFN(PWM2_DEV,PWM2_FUNC)
# define SOC_DEVFN_HSUART1 PCI_DEVFN(HSUART1_DEV,HSUART1_FUNC)
# define SOC_DEVFN_HSUART2 PCI_DEVFN(HSUART2_DEV,HSUART2_FUNC)
# define SOC_DEVFN_SPI PCI_DEVFN(SPI_DEV,SPI_FUNC)


/* Platform Controller Unit */
#define PCU_DEV 0x1f
# define LPC_DEV PCU_DEV
# define LPC_FUNC 0
# define PCH_DEVFN_LPC 	PCI_DEVFN(LPC_DEV,LPC_FUNC)
# define LPC_BDF		PCI_DEV(0, LPC_DEV, LPC_FUNC)

# define SMBUS_DEV PCU_DEV
# define SMBUS_FUNC 3
# define SOC_DEVFN_SMBUS PCI_DEVFN(SMBUS_DEV,SMBUS_FUNC)

#define SOC_DEVID 0x0f00
#define GFX_DEVID 0x0f31
#define MIPI_DEVID 0x0f38
#define EMMC_DEVID 0x0f14
#define SDIO_DEVID 0x0f15
#define SD_DEVID 0x0f16
#define IDE1_DEVID 0x0f20
#define IDE2_DEVID 0x0f21
#define AHCI1_DEVID 0x0f22
#define AHCI2_DEVID 0x0f23
#define XHCI_DEVID 0x0f35
#define LPE_DEVID 0x0f28
#define OTG_DEVID 0x0f37
#define MMC45_DEVID 0x0f50
#define SIO_DMA1_DEVID 0x0f40
#define I2C1_DEVID 0x0f41
#define I2C2_DEVID 0x0f42
#define I2C3_DEVID 0x0f43
#define I2C4_DEVID 0x0f44
#define I2C5_DEVID 0x0f45
#define I2C6_DEVID 0x0f46
#define I2C7_DEVID 0x0f47
#define TXE_DEVID 0x0f18
#define HDA_DEVID 0x0f04
#define PCIE_PORT1_DEVID 0x0f48
#define PCIE_PORT2_DEVID 0x0f4a
#define PCIE_PORT3_DEVID 0x0f4c
#define PCIE_PORT4_DEVID 0x0f4e
#define EHCI_DEVID 0x0f34
#define SIO_DMA2_DEVID 0x0f06
#define PWM1_DEVID 0x0f08
#define PWM2_DEVID 0x0f09
#define HSUART1_DEVID 0x0f0a
#define HSUART2_DEVID 0x0f0c
#define SPI_DEVID 0xf0e
#define LPC_DEVID 0x0f1c
#define SMBUS_DEVID 0x0f12

#endif /* _BAYTRAIL_PCI_DEVS_H_ */
