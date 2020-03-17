/*
 * This file is part of the coreboot project.
 *
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

#include <device/mmio.h>
#include <stdint.h>
#include <types.h>
#include <amdblocks/acpimmio_map.h>

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

/* For older discrete FCHs */
void enable_acpimmio_decode_pm24(void);

/* For newer integrated FCHs */
void enable_acpimmio_decode_pm04(void);

/* Access PM registers using IO cycles */
uint8_t pm_io_read8(uint8_t reg);
uint16_t pm_io_read16(uint8_t reg);
uint32_t pm_io_read32(uint8_t reg);
void pm_io_write8(uint8_t reg, uint8_t value);
void pm_io_write16(uint8_t reg, uint16_t value);
void pm_io_write32(uint8_t reg, uint32_t value);

static inline uint8_t sm_pci_read8(uint8_t reg)
{
	return read8((void *)(ACPIMMIO_SM_PCI_BASE + reg));
}

static inline uint16_t sm_pci_read16(uint8_t reg)
{
	return read16((void *)(ACPIMMIO_SM_PCI_BASE + reg));
}

static inline uint32_t sm_pci_read32(uint8_t reg)
{
	return read32((void *)(ACPIMMIO_SM_PCI_BASE + reg));
}

static inline void sm_pci_write8(uint8_t reg, uint8_t value)
{
	write8((void *)(ACPIMMIO_SM_PCI_BASE + reg), value);
}

static inline void sm_pci_write16(uint8_t reg, uint16_t value)
{
	write16((void *)(ACPIMMIO_SM_PCI_BASE + reg), value);
}

static inline void sm_pci_write32(uint8_t reg, uint32_t value)
{
	write32((void *)(ACPIMMIO_SM_PCI_BASE + reg), value);
}

static inline uint8_t smi_read8(uint8_t reg)
{
	return read8((void *)(ACPIMMIO_SMI_BASE + reg));
}

static inline uint16_t smi_read16(uint8_t reg)
{
	return read16((void *)(ACPIMMIO_SMI_BASE + reg));
}

static inline uint32_t smi_read32(uint8_t reg)
{
	return read32((void *)(ACPIMMIO_SMI_BASE + reg));
}

static inline void smi_write8(uint8_t reg, uint8_t value)
{
	write8((void *)(ACPIMMIO_SMI_BASE + reg), value);
}

static inline void smi_write16(uint8_t reg, uint16_t value)
{
	write16((void *)(ACPIMMIO_SMI_BASE + reg), value);
}

static inline void smi_write32(uint8_t reg, uint32_t value)
{
	write32((void *)(ACPIMMIO_SMI_BASE + reg), value);
}

static inline uint8_t pm_read8(uint8_t reg)
{
	return read8((void *)(ACPIMMIO_PMIO_BASE + reg));
}

static inline uint16_t pm_read16(uint8_t reg)
{
	return read16((void *)(ACPIMMIO_PMIO_BASE + reg));
}

static inline uint32_t pm_read32(uint8_t reg)
{
	return read32((void *)(ACPIMMIO_PMIO_BASE + reg));
}

static inline void pm_write8(uint8_t reg, uint8_t value)
{
	write8((void *)(ACPIMMIO_PMIO_BASE + reg), value);
}

static inline void pm_write16(uint8_t reg, uint16_t value)
{
	write16((void *)(ACPIMMIO_PMIO_BASE + reg), value);
}

static inline void pm_write32(uint8_t reg, uint32_t value)
{
	write32((void *)(ACPIMMIO_PMIO_BASE + reg), value);
}

static inline uint8_t pm2_read8(uint8_t reg)
{
	return read8((void *)(ACPIMMIO_PMIO2_BASE + reg));
}

static inline uint16_t pm2_read16(uint8_t reg)
{
	return read16((void *)(ACPIMMIO_PMIO2_BASE + reg));
}

static inline uint32_t pm2_read32(uint8_t reg)
{
	return read32((void *)(ACPIMMIO_PMIO2_BASE + reg));
}

static inline void pm2_write8(uint8_t reg, uint8_t value)
{
	write8((void *)(ACPIMMIO_PMIO2_BASE + reg), value);
}

static inline void pm2_write16(uint8_t reg, uint16_t value)
{
	write16((void *)(ACPIMMIO_PMIO2_BASE + reg), value);
}

static inline void pm2_write32(uint8_t reg, uint32_t value)
{
	write32((void *)(ACPIMMIO_PMIO2_BASE + reg), value);
}

static inline uint8_t acpi_read8(uint8_t reg)
{
	return read8((void *)(ACPIMMIO_ACPI_BASE + reg));
}

static inline uint16_t acpi_read16(uint8_t reg)
{
	return read16((void *)(ACPIMMIO_ACPI_BASE + reg));
}

static inline uint32_t acpi_read32(uint8_t reg)
{
	return read32((void *)(ACPIMMIO_ACPI_BASE + reg));
}

static inline void acpi_write8(uint8_t reg, uint8_t value)
{
	write8((void *)(ACPIMMIO_ACPI_BASE + reg), value);
}

static inline void acpi_write16(uint8_t reg, uint16_t value)
{
	write16((void *)(ACPIMMIO_ACPI_BASE + reg), value);
}

static inline void acpi_write32(uint8_t reg, uint32_t value)
{
	write32((void *)(ACPIMMIO_ACPI_BASE + reg), value);
}

static inline uint8_t asf_read8(uint8_t reg)
{
	return read8((void *)(ACPIMMIO_ASF_BASE + reg));
}

static inline uint16_t asf_read16(uint8_t reg)
{
	return read16((void *)(ACPIMMIO_ASF_BASE + reg));
}

static inline void asf_write8(uint8_t reg, uint8_t value)
{
	write8((void *)(ACPIMMIO_ASF_BASE + reg), value);
}

static inline void asf_write16(uint8_t reg, uint16_t value)
{
	write16((void *)(ACPIMMIO_ASF_BASE + reg), value);
}

static inline uint8_t smbus_read8(uint8_t reg)
{
	return read8((void *)(ACPIMMIO_SMBUS_BASE + reg));
}

static inline uint16_t smbus_read16(uint8_t reg)
{
	return read16((void *)(ACPIMMIO_SMBUS_BASE + reg));
}

static inline void smbus_write8(uint8_t reg, uint8_t value)
{
	write8((void *)(ACPIMMIO_SMBUS_BASE + reg), value);
}

static inline void smbus_write16(uint8_t reg, uint16_t value)
{
	write16((void *)(ACPIMMIO_SMBUS_BASE + reg), value);
}

static inline uint8_t iomux_read8(uint8_t reg)
{
	return read8((void *)(ACPIMMIO_IOMUX_BASE + reg));
}

static inline uint16_t iomux_read16(uint8_t reg)
{
	return read16((void *)(ACPIMMIO_IOMUX_BASE + reg));
}

static inline uint32_t iomux_read32(uint8_t reg)
{
	return read32((void *)(ACPIMMIO_IOMUX_BASE + reg));
}

static inline void iomux_write8(uint8_t reg, uint8_t value)
{
	write8((void *)(ACPIMMIO_IOMUX_BASE + reg), value);
}

static inline void iomux_write16(uint8_t reg, uint16_t value)
{
	write16((void *)(ACPIMMIO_IOMUX_BASE + reg), value);
}

static inline void iomux_write32(uint8_t reg, uint32_t value)
{
	write32((void *)(ACPIMMIO_IOMUX_BASE + reg), value);
}

static inline uint8_t misc_read8(uint8_t reg)
{
	return read8((void *)(ACPIMMIO_MISC_BASE + reg));
}

static inline uint16_t misc_read16(uint8_t reg)
{
	return read16((void *)(ACPIMMIO_MISC_BASE + reg));
}

static inline uint32_t misc_read32(uint8_t reg)
{
	return read32((void *)(ACPIMMIO_MISC_BASE + reg));
}

static inline void misc_write8(uint8_t reg, uint8_t value)
{
	write8((void *)(ACPIMMIO_MISC_BASE + reg), value);
}

static inline void misc_write16(uint8_t reg, uint16_t value)
{
	write16((void *)(ACPIMMIO_MISC_BASE + reg), value);
}

static inline void misc_write32(uint8_t reg, uint32_t value)
{
	write32((void *)(ACPIMMIO_MISC_BASE + reg), value);
}

/* Old GPIO configuration registers */
static inline uint8_t gpio_100_read8(uint8_t reg)
{
	return read8((void *)(ACPIMMIO_GPIO_BASE_100 + reg));
}

static inline uint16_t gpio_100_read16(uint8_t reg)
{
	return read16((void *)(ACPIMMIO_GPIO_BASE_100 + reg));
}

static inline uint32_t gpio_100_read32(uint8_t reg)
{
	return read32((void *)(ACPIMMIO_GPIO_BASE_100 + reg));
}

static inline void gpio_100_write8(uint8_t reg, uint8_t value)
{
	write8((void *)(ACPIMMIO_GPIO_BASE_100 + reg), value);
}

static inline void gpio_100_write16(uint8_t reg, uint16_t value)
{
	write16((void *)(ACPIMMIO_GPIO_BASE_100 + reg), value);
}

static inline void gpio_100_write32(uint8_t reg, uint32_t value)
{
	write32((void *)(ACPIMMIO_GPIO_BASE_100 + reg), value);
}

/* New GPIO banks configuration registers */
/* GPIO bank 0 */
static inline uint8_t gpio0_read8(uint8_t reg)
{
	return read8((void *)(ACPIMMIO_GPIO0_BASE + reg));
}

static inline uint16_t gpio0_read16(uint8_t reg)
{
	return read16((void *)(ACPIMMIO_GPIO0_BASE + reg));
}

static inline uint32_t gpio0_read32(uint8_t reg)
{
	return read32((void *)(ACPIMMIO_GPIO0_BASE + reg));
}

static inline void gpio0_write8(uint8_t reg, uint8_t value)
{
	write8((void *)(ACPIMMIO_GPIO0_BASE + reg), value);
}

static inline void gpio0_write16(uint8_t reg, uint16_t value)
{
	write16((void *)(ACPIMMIO_GPIO0_BASE + reg), value);
}

static inline void gpio0_write32(uint8_t reg, uint32_t value)
{
	write32((void *)(ACPIMMIO_GPIO0_BASE + reg), value);
}

/* GPIO bank 1 */
static inline uint8_t gpio1_read8(uint8_t reg)
{
	return read8((void *)(ACPIMMIO_GPIO1_BASE + reg));
}

static inline uint16_t gpio1_read16(uint8_t reg)
{
	return read16((void *)(ACPIMMIO_GPIO1_BASE + reg));
}

static inline uint32_t gpio1_read32(uint8_t reg)
{
	return read32((void *)(ACPIMMIO_GPIO1_BASE + reg));
}

static inline void gpio1_write8(uint8_t reg, uint8_t value)
{
	write8((void *)(ACPIMMIO_GPIO1_BASE + reg), value);
}

static inline void gpio1_write16(uint8_t reg, uint16_t value)
{
	write16((void *)(ACPIMMIO_GPIO1_BASE + reg), value);
}

static inline void gpio1_write32(uint8_t reg, uint32_t value)
{
	write32((void *)(ACPIMMIO_GPIO1_BASE + reg), value);
}

/* GPIO bank 2 */
static inline uint8_t gpio2_read8(uint8_t reg)
{
	return read8((void *)(ACPIMMIO_GPIO2_BASE + reg));
}

static inline uint16_t gpio2_read16(uint8_t reg)
{
	return read16((void *)(ACPIMMIO_GPIO2_BASE + reg));
}

static inline uint32_t gpio2_read32(uint8_t reg)
{
	return read32((void *)(ACPIMMIO_GPIO2_BASE + reg));
}

static inline void gpio2_write8(uint8_t reg, uint8_t value)
{
	write8((void *)(ACPIMMIO_GPIO2_BASE + reg), value);
}

static inline void gpio2_write16(uint8_t reg, uint16_t value)
{
	write16((void *)(ACPIMMIO_GPIO2_BASE + reg), value);
}

static inline void gpio2_write32(uint8_t reg, uint32_t value)
{
	write32((void *)(ACPIMMIO_GPIO2_BASE + reg), value);
}

static inline uint8_t xhci_pm_read8(uint8_t reg)
{
	return read8((void *)(ACPIMMIO_XHCIPM_BASE + reg));
}

static inline uint16_t xhci_pm_read16(uint8_t reg)
{
	return read16((void *)(ACPIMMIO_XHCIPM_BASE + reg));
}

static inline uint32_t xhci_pm_read32(uint8_t reg)
{
	return read32((void *)(ACPIMMIO_XHCIPM_BASE + reg));
}

static inline void xhci_pm_write8(uint8_t reg, uint8_t value)
{
	write8((void *)(ACPIMMIO_XHCIPM_BASE + reg), value);
}

static inline void xhci_pm_write16(uint8_t reg, uint16_t value)
{
	write16((void *)(ACPIMMIO_XHCIPM_BASE + reg), value);
}

static inline void xhci_pm_write32(uint8_t reg, uint32_t value)
{
	write32((void *)(ACPIMMIO_XHCIPM_BASE + reg), value);
}

static inline uint8_t aoac_read8(uint8_t reg)
{
	return read8((void *)(ACPIMMIO_AOAC_BASE + reg));
}

static inline void aoac_write8(uint8_t reg, uint8_t value)
{
	write8((void *)(ACPIMMIO_AOAC_BASE + reg), value);
}

#endif /* __AMDBLOCKS_ACPIMMIO_H__ */
