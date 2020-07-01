/* SPDX-License-Identifier: GPL-2.0-or-later */

#ifndef AMD_BLOCK_ACPIMMIO_MAP_H
#define AMD_BLOCK_ACPIMMIO_MAP_H

/*
 * The following AcpiMmio register block mapping represents definitions
 * that have been documented in AMD publications.  All blocks aren't
 * implemented in all products, so the caller should be careful not to
 * inadvertently access a non-existent block.  The definitions within
 * each block are also subject to change across products.  Please refer
 * to the appropriate RRG, the BKDG, or PPR for the product.
 *
 * The base address is configurable in older products, but defaults to
 * 0xfed80000.  The address is fixed at 0xfed80000 in newer products.
 *
 * +---------------------------------------------------------------------------+
 * |0x000    SMBus PCI space                                                   |
 * |           * Dual-mapped to PCI configuration header of D14F0              |
 * +---------------------------------------------------------------------------+
 * |0x100    GPIO configuration registers                                      |
 * |           * old style, never implemented with newer style                 |
 * |           * discrete controller hubs and Family 16h Models 00h-0Fh.       |
 * +---------------------------------------------------------------------------+
 * |0x200    SMI configuration registers                                       |
 * +---------------------------------------------------------------------------+
 * |0x300    Power Management registers                                        |
 * |           * Dual-mapped via IO Index/Data 0xcd6/0xcd7 (byte access only)  |
 * +---------------------------------------------------------------------------+
 * |0x400    Power Management 2 registers                                      |
 * +---------------------------------------------------------------------------+
 * |0x500    BIOS RAM                                                          |
 * |           * General-purpose storage in S3 domain                          |
 * |           * Byte access only                                              |
 * +---------------------------------------------------------------------------+
 * |0x600    CMOS RAM                                                          |
 * |           * Dual-mapped to storage at Alt RTC Index/Data (0x72/0x73)      |
 * |           * Byte access only                                              |
 * +---------------------------------------------------------------------------+
 * |0x700    CMOS                                                              |
 * |           * Dual-mapped to storage at RTC Index/Data (0x70/0x71)          |
 * |           * Byte access only                                              |
 * +---------------------------------------------------------------------------+
 * |0x800    Standard ACPI registers                                           |
 * |           * Dual-mapped to I/O ACPI registers                             |
 * +---------------------------------------------------------------------------+
 * |0x900    ASF controller registers                                          |
 * |           * Dual-mapped to I/O ASF controller registers                   |
 * +---------------------------------------------------------------------------+
 * |0xa00    SMBus controller registers                                        |
 * |           * Dual-mapped to I/O SMBus controller registers                 |
 * +---------------------------------------------------------------------------+
 * |0xb00    WDT registers                                                     |
 * |           * Dual-mapped to WDT registers, typ. enabled at 0xfeb00000      |
 * +---------------------------------------------------------------------------+
 * |0xc00    HPET registers                                                    |
 * |           * Dual-mapped to HPET registers, typ. enabled at 0xfed00000     |
 * +---------------------------------------------------------------------------+
 * |0xd00    MUX configuration registers for GPIO signals                      |
 * +---------------------------------------------------------------------------+
 * |0xe00    Miscellaneous registers                                           |
 * +---------------------------------------------------------------------------+
 * |0x1000   Serial debug bus                                                  |
 * +---------------------------------------------------------------------------+
 * |0x1400   DP-VGA                                                            |
 * +---------------------------------------------------------------------------+
 * |0x1500   GPIO configuration registers bank 0                               |
 * |           * new style, never implemented with older style                 |
 * |           * SoCs starting with Family 16h Models 30h-3Fh                  |
 * +---------------------------------------------------------------------------+
 * |0x1600   GPIO configuration registers bank 1 (following bank 0)            |
 * +---------------------------------------------------------------------------+
 * |0x1700   GPIO configuration registers bank 2 (following bank 1)            |
 * +---------------------------------------------------------------------------+
 * |0x1c00   xHCI Power Management registers                                   |
 * +---------------------------------------------------------------------------+
 * |0x1d00   Wake device (AC DC timer)                                         |
 * +---------------------------------------------------------------------------+
 * |0x1e00   Always On Always Connect registers                                |
 * +---------------------------------------------------------------------------+
 */

/*
 * MMIO register blocks are at fixed offsets from 0xfed80000 and are enabled
 * in PMx24[0] (older implementations) and PMx04[1] (newer implementations).
 * PM registers are also accessible via IO CD6/CD7.
 *
 * All products do not support all blocks below, however AMD has avoided
 * redefining addresses and consumes new ranges as necessary.
 *
 * Definitions within each block are not guaranteed to remain consistent
 * across family/model products.
 */

#define AMD_SB_ACPI_MMIO_ADDR		0xfed80000

#ifdef __ACPI__

/* ASL fails on additions. */
#define ACPIMMIO_MISC_BASE		0xfed80e00
#define ACPIMMIO_GPIO0_BASE		0xfed81500
#define ACPIMMIO_AOAC_BASE		0xfed81e00

#else

#define ACPIMMIO_SM_PCI_BANK		0x0000
#define ACPIMMIO_GPIO_100_BANK		0x0100
#define ACPIMMIO_SMI_BANK		0x0200
#define ACPIMMIO_PMIO_BANK		0x0300
#define ACPIMMIO_PMIO2_BANK		0x0400
#define ACPIMMIO_BIOSRAM_BANK		0x0500
#define ACPIMMIO_CMOSRAM_BANK		0x0600
#define ACPIMMIO_CMOS_BANK		0x0700
#define ACPIMMIO_ACPI_BANK		0x0800
#define ACPIMMIO_ASF_BANK		0x0900
#define ACPIMMIO_SMBUS_BANK		0x0a00
#define ACPIMMIO_WDT_BANK		0x0b00
#define ACPIMMIO_HPET_BANK		0x0c00
#define ACPIMMIO_IOMUX_BANK		0x0d00
#define ACPIMMIO_MISC_BANK		0x0e00
#define ACPIMMIO_DPVGA_BANK		0x1400
#define ACPIMMIO_GPIO0_BANK		0x1500
#define ACPIMMIO_GPIO1_BANK		0x1600
#define ACPIMMIO_GPIO2_BANK		0x1700
#define ACPIMMIO_XHCIPM_BANK		0x1c00
#define ACPIMMIO_ACDCTMR_BANK		0x1d00
#define ACPIMMIO_AOAC_BANK		0x1e00

#endif

#endif /* AMD_BLOCK_ACPIMMIO_MAP_H */
