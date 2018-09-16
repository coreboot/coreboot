/*
 * This file is part of the coreboot project.
 *
 * Copyright 2017 Intel Corporation.
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

#ifndef SOC_INTEL_COMMON_BLOCK_GRAPHICS_H
#define SOC_INTEL_COMMON_BLOCK_GRAPHICS_H

#include <device/device.h>

/*
 * SoC overrides
 *
 * All new SoC must implement below functionality.
 */

/*
 * Perform Graphics Initialization in ramstage
 * Input:
 * struct device *dev: device structure
 */
void graphics_soc_init(struct device *dev);

/*
 * Write ASL entry for Graphics opregion
 * Input:
 * struct device *device: device structure
 * current: start address of graphics opregion
 * rsdp: pointer to RSDT (and XSDT) structure
 *
 * Output:
 * End address of graphics opregion so that the called
 * can use the same for future calls to write_acpi_tables
 */
uintptr_t graphics_soc_write_acpi_opregion(struct device *device,
		uintptr_t current, struct acpi_rsdp *rsdp);

/* Graphics MMIO register read/write APIs */
uint32_t graphics_gtt_read(unsigned long reg);
void graphics_gtt_write(unsigned long reg, uint32_t data);
void graphics_gtt_rmw(unsigned long reg, uint32_t andmask, uint32_t ormask);
uintptr_t graphics_get_memory_base(void);

#endif	/* SOC_INTEL_COMMON_BLOCK_GRAPHICS_H */
