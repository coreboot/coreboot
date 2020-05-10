/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef _BAYTRAIL_PCI_DEVS_H_
#define _BAYTRAIL_PCI_DEVS_H_

/* All these devices live on bus 0 with the associated device and function */

/* SoC transaction router */
#define SOC_DEV 0x0
#define SOC_FUNC 0

/* Graphics and Display */
#define GFX_DEV 0x2
#define GFX_FUNC 0

/* SDIO Port */
#define SDIO_DEV 0x11
#define SDIO_FUNC 0

/* SD Port */
#define SD_DEV 0x12
#define SD_FUNC 0

/* SATA */
#define SATA_DEV 0x13
#define SATA_FUNC 0

/* xHCI */
#define XHCI_DEV 0x14
#define XHCI_FUNC 0

/* LPE Audio */
#define LPE_DEV 0x15
#define LPE_FUNC 0

/* MMC Port */
#define MMC_DEV 0x17
#define MMC_FUNC 0

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

/* Trusted Execution Engine */
#define TXE_DEV 0x1a
#define TXE_FUNC 0

/* HD Audio */
#define HDA_DEV 0x1b
#define HDA_FUNC 0

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

/* EHCI */
#define EHCI_DEV 0x1d
#define EHCI_FUNC 0

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

/* Platform Controller Unit */
#define PCU_DEV 0x1f
# define LPC_DEV PCU_DEV
# define LPC_FUNC 0
# define SMBUS_DEV PCU_DEV
# define SMBUS_FUNC 3

#define SOC_DEVID 0x0f00
#define GFX_DEVID 0x0f31
#define SDIO_DEVID 0x0f15
#define SD_DEVID 0x0f16
#define IDE1_DEVID  0x0f20
#define IDE2_DEVID  0x0f21
#define AHCI1_DEVID 0x0f22
#define AHCI2_DEVID 0x0f23
#define XHCI_DEVID 0x0f35
#define LPE_DEVID 0x0f28
#define MMC_DEVID 0x0f50
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
