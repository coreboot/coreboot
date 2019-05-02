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

void enable_acpimmio_decode(void);
uint8_t pm_io_read8(uint8_t reg);
uint16_t pm_io_read16(uint8_t reg);
uint32_t pm_io_read32(uint8_t reg);
void pm_io_write8(uint8_t reg, uint8_t value);
void pm_io_write16(uint8_t reg, uint16_t value);
void pm_io_write32(uint8_t reg, uint32_t value);
uint8_t smi_read8(uint8_t offset);
uint16_t smi_read16(uint8_t offset);
uint32_t smi_read32(uint8_t offset);
void smi_write8(uint8_t offset, uint8_t value);
void smi_write16(uint8_t offset, uint16_t value);
void smi_write32(uint8_t offset, uint32_t value);
uint8_t pm_read8(uint8_t reg);
uint16_t pm_read16(uint8_t reg);
uint32_t pm_read32(uint8_t reg);
void pm_write8(uint8_t reg, uint8_t value);
void pm_write16(uint8_t reg, uint16_t value);
void pm_write32(uint8_t reg, uint32_t value);
uint8_t pm2_read8(uint8_t reg);
uint16_t pm2_read16(uint8_t reg);
uint32_t pm2_read32(uint8_t reg);
void pm2_write8(uint8_t reg, uint8_t value);
void pm2_write16(uint8_t reg, uint16_t value);
void pm2_write32(uint8_t reg, uint32_t value);
uint8_t biosram_read8(uint8_t offset);
void biosram_write8(uint8_t offset, uint8_t value);
uint16_t biosram_read16(uint8_t offset);
uint32_t biosram_read32(uint8_t offset);
void biosram_write16(uint8_t offset, uint16_t value);
void biosram_write32(uint8_t offset, uint32_t value);
uint8_t acpi_read8(uint8_t reg);
uint16_t acpi_read16(uint8_t reg);
uint32_t acpi_read32(uint8_t reg);
void acpi_write8(uint8_t reg, uint8_t value);
void acpi_write16(uint8_t reg, uint16_t value);
void acpi_write32(uint8_t reg, uint32_t value);
uint8_t asf_read8(uint8_t reg);
uint16_t asf_read16(uint8_t reg);
uint32_t asf_read32(uint8_t reg);
void asf_write8(uint8_t reg, uint8_t value);
void asf_write16(uint8_t reg, uint16_t value);
void asf_write32(uint8_t reg, uint32_t value);
uint8_t smbus_read8(uint8_t reg);
uint16_t smbus_read16(uint8_t reg);
void smbus_write8(uint8_t reg, uint8_t value);
void smbus_write16(uint8_t reg, uint16_t value);
uint8_t wdt_read8(uint8_t reg);
uint16_t wdt_read16(uint8_t reg);
uint32_t wdt_read32(uint8_t reg);
void wdt_write8(uint8_t reg, uint8_t value);
void wdt_write16(uint8_t reg, uint16_t value);
void wdt_write32(uint8_t reg, uint32_t value);
uint8_t hpet_read8(uint8_t reg);
uint16_t hpet_read16(uint8_t reg);
uint32_t hpet_read32(uint8_t reg);
void hpet_write8(uint8_t reg, uint8_t value);
void hpet_write16(uint8_t reg, uint16_t value);
void hpet_write32(uint8_t reg, uint32_t value);
uint8_t iomux_read8(uint8_t reg);
uint16_t iomux_read16(uint8_t reg);
uint32_t iomux_read32(uint8_t reg);
void iomux_write8(uint8_t reg, uint8_t value);
void iomux_write16(uint8_t reg, uint16_t value);
void iomux_write32(uint8_t reg, uint32_t value);
uint8_t misc_read8(uint8_t reg);
uint16_t misc_read16(uint8_t reg);
uint32_t misc_read32(uint8_t reg);
void misc_write8(uint8_t reg, uint8_t value);
void misc_write16(uint8_t reg, uint16_t value);
void misc_write32(uint8_t reg, uint32_t value);
uint8_t xhci_pm_read8(uint8_t reg);
uint16_t xhci_pm_read16(uint8_t reg);
uint32_t xhci_pm_read32(uint8_t reg);
void xhci_pm_write8(uint8_t reg, uint8_t value);
void xhci_pm_write16(uint8_t reg, uint16_t value);
void xhci_pm_write32(uint8_t reg, uint32_t value);
uint8_t aoac_read8(uint8_t reg);
void aoac_write8(uint8_t reg, uint8_t value);

#endif /* __AMDBLOCKS_ACPIMMIO_H__ */
