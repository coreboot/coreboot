/* SPDX-License-Identifier: GPL-2.0-or-later */

/*
 * Base addresses for QEMU sbsa-ref machine
 * [hw/arm/sbsa-ref.c, c6f3cbca32bde9ee94d9949aa63e8a7ef2d7bc5b]
 */

#define SBSA_FLASH_BASE           0x00000000
#define SBSA_FLASH_SIZE           0x20000000
#define SBSA_GIC_DIST             0x40060000
#define SBSA_GIC_REDIST           0x40080000
#define SBSA_GWDT_REFRESH         0x50010000
#define SBSA_GWDT_CONTROL         0x50011000
#define SBSA_UART_BASE            0x60000000
#define SBSA_RTC_BASE             0x60010000
#define SBSA_GPIO_BASE            0x60020000
#define SBSA_SECURE_UART_BASE     0x60030000
#define SBSA_SMMU_BASE            0x60050000
#define SBSA_AHCI_BASE            0x60100000
#define SBSA_EHCI_BASE            0x60110000
#define SBSA_SECMEM_BASE          0x20000000
#define SBSA_SECMEM_SIZE          0x20000000
#define SBSA_PCIE_MMIO_BASE       0x80000000
#define SBSA_PCIE_MMIO_LIMIT      0xefffffff
#define SBSA_PCIE_MMIO_SIZE       0x70000000
#define SBSA_PCIE_PIO_BASE        0x7fff0000
#define SBSA_PCIE_ECAM_BASE       0xf0000000
#define SBSA_PCIE_ECAM_LIMIT      0xffffffff
#define SBSA_PCIE_ECAM_SIZE       0x10000000
#define SBSA_PCIE_MMIO_HIGH_BASE  0x100000000
#define SBSA_PCIE_MMIO_HIGH_LIMIT 0xffffffffff
#define SBSA_PCIE_MMIO_HIGH_SIZE  0xff00000000
