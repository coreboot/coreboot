/* SPDX-License-Identifier: GPL-2.0-only */

// reference: fu740-c000 manual chapter 5: Memory Map
// Table 15: (TODO: subject for common code: none of these changed compared to fu540)
#define FU740_ROM        0x00001000
#define FU740_DTIM       0x01000000
#define FU740_CLINT      0x02000000
#define FU740_L2LIM      0x08000000
#define FU740_PRCI       0x10000000
#define FU740_UART0      0x10010000
#define FU740_UART1      0x10011000
#define FU740_QSPI0      0x10040000
#define FU740_QSPI1      0x10041000
#define FU740_QSPI2      0x10050000 // in unmatched board schematics it's called SPI0
#define FU740_GPIO       0x10060000
#define FU740_OTP        0x10070000
#define FU740_PINCTRL    0x10080000
#define FU740_QSPI0FLASH 0x20000000
#define FU740_QSPI1FLASH 0x30000000
#define FU740_DRAM       0x80000000

#define PCIE_MGMT   0x100D0000
#define PCIE_CONFIG 0x000DF0000000
#define PCIE_DBI    0x000E00000000

#define FU740_I2C_0 0x10030000
#define FU740_I2C_1 0x10031000

// Reset Vector - 4
#define FU740_ROM1 0x00001000
// After reset vector it will jump directly to this address if ZSBL is used (ZSBL code)
#define FU740_ROM2 0x00010000
#define FU740_MSEL FU740_ROM // mode select is always at start of ROM

// naming changed a bit between FU540 and FU740 manuals
// Ethernet MAC -> Ethernet
// Ethernet Management -> GEMGXL MGMT
#define SIFIVE_ETHERNET_MAC  0x10090000
#define SIFIVE_ETHERNET_MGMT 0x100A0000

#define FU740_UART(i) (FU740_UART0 + 0x1000 * i)
