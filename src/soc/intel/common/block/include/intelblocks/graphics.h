/* SPDX-License-Identifier: GPL-2.0-only */

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
uintptr_t graphics_soc_write_acpi_opregion(const struct device *device,
		uintptr_t current, struct acpi_rsdp *rsdp);

/* i915 controller info for ACPI backlight controls */
const struct i915_gpu_controller_info *
intel_igd_get_controller_info(const struct device *device);

/* Graphics MMIO register read/write APIs */
uint32_t graphics_gtt_read(unsigned long reg);
void graphics_gtt_write(unsigned long reg, uint32_t data);
void graphics_gtt_rmw(unsigned long reg, uint32_t andmask, uint32_t ormask);
uintptr_t graphics_get_memory_base(void);

#endif	/* SOC_INTEL_COMMON_BLOCK_GRAPHICS_H */
