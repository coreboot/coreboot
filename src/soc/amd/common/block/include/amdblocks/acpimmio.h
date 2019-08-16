/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2019 Advanced Micro Devices, Inc.
 * Copyright (C) 2014 Alexandru Gagniuc <mr.nuke.me@gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License, or (at your
 * option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#ifndef __AMDBLOCKS_ACPIMMIO_H__
#define __AMDBLOCKS_ACPIMMIO_H__

#include <stdint.h>
/* iomap.h must indicate if the device uses a block, optional if unused. */
#include <soc/iomap.h>
#ifndef SUPPORTS_ACPIMMIO_SM_PCI_BASE
  #define SUPPORTS_ACPIMMIO_SM_PCI_BASE		0
#endif
#ifndef SUPPORTS_ACPIMMIO_SMI_BASE
  #define SUPPORTS_ACPIMMIO_SMI_BASE		0
#endif
#ifndef SUPPORTS_ACPIMMIO_PMIO_BASE
  #define SUPPORTS_ACPIMMIO_PMIO_BASE		0
#endif
#ifndef SUPPORTS_ACPIMMIO_PMIO2_BASE
  #define SUPPORTS_ACPIMMIO_PMIO2_BASE		0
#endif
#ifndef SUPPORTS_ACPIMMIO_BIOSRAM_BASE
  #define SUPPORTS_ACPIMMIO_BIOSRAM_BASE	0
#endif
#ifndef SUPPORTS_ACPIMMIO_CMOSRAM_BASE
  #define SUPPORTS_ACPIMMIO_CMOSRAM_BASE	0
#endif
#ifndef SUPPORTS_ACPIMMIO_CMOS_BASE
  #define SUPPORTS_ACPIMMIO_CMOS_BASE		0
#endif
#ifndef SUPPORTS_ACPIMMIO_ACPI_BASE
  #define SUPPORTS_ACPIMMIO_ACPI_BASE		0
#endif
#ifndef SUPPORTS_ACPIMMIO_ASF_BASE
  #define SUPPORTS_ACPIMMIO_ASF_BASE		0
#endif
#ifndef SUPPORTS_ACPIMMIO_SMBUS_BASE
  #define SUPPORTS_ACPIMMIO_SMBUS_BASE		0
#endif
#ifndef SUPPORTS_ACPIMMIO_WDT_BASE
  #define SUPPORTS_ACPIMMIO_WDT_BASE		0
#endif
#ifndef SUPPORTS_ACPIMMIO_HPET_BASE
  #define SUPPORTS_ACPIMMIO_HPET_BASE		0
#endif
#ifndef SUPPORTS_ACPIMMIO_IOMUX_BASE
  #define SUPPORTS_ACPIMMIO_IOMUX_BASE		0
#endif
#ifndef SUPPORTS_ACPIMMIO_MISC_BASE
  #define SUPPORTS_ACPIMMIO_MISC_BASE		0
#endif
#ifndef SUPPORTS_ACPIMMIO_DPVGA_BASE
  #define SUPPORTS_ACPIMMIO_DPVGA_BASE		0
#endif
#ifndef SUPPORTS_ACPIMMIO_GPIO0_BASE
  #define SUPPORTS_ACPIMMIO_GPIO0_BASE		0
#endif
#ifndef SUPPORTS_ACPIMMIO_GPIO1_BASE
  #define SUPPORTS_ACPIMMIO_GPIO1_BASE		0
#endif
#ifndef SUPPORTS_ACPIMMIO_GPIO2_BASE
  #define SUPPORTS_ACPIMMIO_GPIO2_BASE		0
#endif
#ifndef SUPPORTS_ACPIMMIO_XHCIPM_BASE
  #define SUPPORTS_ACPIMMIO_XHCIPM_BASE		0
#endif
#ifndef SUPPORTS_ACPIMMIO_ACDCTMR_BASE
  #define SUPPORTS_ACPIMMIO_ACDCTMR_BASE	0
#endif
#ifndef SUPPORTS_ACPIMMIO_AOAC_BASE
  #define SUPPORTS_ACPIMMIO_AOAC_BASE		0
#endif

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
 * +---------------------------------------------------------------------------+
 * |0x1600   GPIO configuration registers bank 1                               |
 * |           * new style, never implemented with older style                 |
 * +---------------------------------------------------------------------------+
 * |0x1700   GPIO configuration registers bank 2                               |
 * |           * new style, never implemented with older style                 |
 * +---------------------------------------------------------------------------+
 * |0x1c00   xHCI Power Management registers                                   |
 * +---------------------------------------------------------------------------+
 * |0x1d00   Wake device (AC DC timer)                                         |
 * +---------------------------------------------------------------------------+
 * |0x1e00   Always On Always Connect registers                                |
 * +---------------------------------------------------------------------------+
 */

/* Enable the AcpiMmio range at 0xfed80000 */
void enable_acpimmio_decode(void);

/* Access SMBus PCI registers at 0xfed80000 */
uint8_t sm_pci_read8(uint8_t reg);
uint16_t sm_pci_read16(uint8_t reg);
uint32_t sm_pci_read32(uint8_t reg);
void sm_pci_write8(uint8_t reg, uint8_t value);
void sm_pci_write16(uint8_t reg, uint16_t value);
void sm_pci_write32(uint8_t reg, uint32_t value);

/* Access PM registers using IO cycles */
uint8_t pm_io_read8(uint8_t reg);
uint16_t pm_io_read16(uint8_t reg);
uint32_t pm_io_read32(uint8_t reg);
void pm_io_write8(uint8_t reg, uint8_t value);
void pm_io_write16(uint8_t reg, uint16_t value);
void pm_io_write32(uint8_t reg, uint32_t value);

/* Access SMI registers at 0xfed80100 */
uint8_t smi_read8(uint8_t reg);
uint16_t smi_read16(uint8_t reg);
uint32_t smi_read32(uint8_t reg);
void smi_write8(uint8_t reg, uint8_t value);
void smi_write16(uint8_t reg, uint16_t value);
void smi_write32(uint8_t reg, uint32_t value);

/* Access Power Management registers at 0xfed80300 */
uint8_t pm_read8(uint8_t reg);
uint16_t pm_read16(uint8_t reg);
uint32_t pm_read32(uint8_t reg);
void pm_write8(uint8_t reg, uint8_t value);
void pm_write16(uint8_t reg, uint16_t value);
void pm_write32(uint8_t reg, uint32_t value);

/* Access Power Management 2 registers at 0xfed80400 */
uint8_t pm2_read8(uint8_t reg);
uint16_t pm2_read16(uint8_t reg);
uint32_t pm2_read32(uint8_t reg);
void pm2_write8(uint8_t reg, uint8_t value);
void pm2_write16(uint8_t reg, uint16_t value);
void pm2_write32(uint8_t reg, uint32_t value);

/* Access BIOS RAM storage at 0xfed80500 */
uint8_t biosram_read8(uint8_t reg);
uint16_t biosram_read16(uint8_t reg);
uint32_t biosram_read32(uint8_t reg);
void biosram_write8(uint8_t reg, uint8_t value);
void biosram_write16(uint8_t reg, uint16_t value);
void biosram_write32(uint8_t reg, uint32_t value);

/* Access ACPI registers at 0xfed80800 */
uint8_t acpi_read8(uint8_t reg);
uint16_t acpi_read16(uint8_t reg);
uint32_t acpi_read32(uint8_t reg);
void acpi_write8(uint8_t reg, uint8_t value);
void acpi_write16(uint8_t reg, uint16_t value);
void acpi_write32(uint8_t reg, uint32_t value);

/* Access ASF controller registers at 0xfed80900 */
uint8_t asf_read8(uint8_t reg);
uint16_t asf_read16(uint8_t reg);
void asf_write8(uint8_t reg, uint8_t value);
void asf_write16(uint8_t reg, uint16_t value);

/* Access SMBus controller registers at 0xfed80a00 */
uint8_t smbus_read8(uint8_t reg);
uint16_t smbus_read16(uint8_t reg);
void smbus_write8(uint8_t reg, uint8_t value);
void smbus_write16(uint8_t reg, uint16_t value);

/* Access WDT registers at 0xfed80b00 */
uint8_t wdt_read8(uint8_t reg);
uint16_t wdt_read16(uint8_t reg);
uint32_t wdt_read32(uint8_t reg);
void wdt_write8(uint8_t reg, uint8_t value);
void wdt_write16(uint8_t reg, uint16_t value);
void wdt_write32(uint8_t reg, uint32_t value);

/* Access HPET registers at 0xfed80c00 */
uint8_t hpet_read8(uint8_t reg);
uint16_t hpet_read16(uint8_t reg);
uint32_t hpet_read32(uint8_t reg);
void hpet_write8(uint8_t reg, uint8_t value);
void hpet_write16(uint8_t reg, uint16_t value);
void hpet_write32(uint8_t reg, uint32_t value);

/* Access GPIO MUX registers at 0xfed80d00 */
uint8_t iomux_read8(uint8_t reg);
uint16_t iomux_read16(uint8_t reg);
uint32_t iomux_read32(uint8_t reg);
void iomux_write8(uint8_t reg, uint8_t value);
void iomux_write16(uint8_t reg, uint16_t value);
void iomux_write32(uint8_t reg, uint32_t value);

/* Access Miscellaneous registers at 0xfed80e00 */
uint8_t misc_read8(uint8_t reg);
uint16_t misc_read16(uint8_t reg);
uint32_t misc_read32(uint8_t reg);
void misc_write8(uint8_t reg, uint8_t value);
void misc_write16(uint8_t reg, uint16_t value);
void misc_write32(uint8_t reg, uint32_t value);

/* Access xHCI Power Management registers at 0xfed81c00 */
uint8_t xhci_pm_read8(uint8_t reg);
uint16_t xhci_pm_read16(uint8_t reg);
uint32_t xhci_pm_read32(uint8_t reg);
void xhci_pm_write8(uint8_t reg, uint8_t value);
void xhci_pm_write16(uint8_t reg, uint16_t value);
void xhci_pm_write32(uint8_t reg, uint32_t value);

/* Access Always On Always Connect registers at 0xfed81e00 */
uint8_t aoac_read8(uint8_t reg);
void aoac_write8(uint8_t reg, uint8_t value);

#endif /* __AMDBLOCKS_ACPIMMIO_H__ */
