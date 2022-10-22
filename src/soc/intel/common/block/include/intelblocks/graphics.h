/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef SOC_INTEL_COMMON_BLOCK_GRAPHICS_H
#define SOC_INTEL_COMMON_BLOCK_GRAPHICS_H

#include <device/device.h>
#include <stdint.h>

/*
 * SoC overrides
 *
 * All new SoC must implement below functionality.
 */

/*
 * Perform Graphics/Panel Initialization in ramstage
 * Input:
 * struct device *dev: device structure
 */
void graphics_soc_panel_init(struct device *dev);

/* i915 controller info for ACPI backlight controls */
const struct i915_gpu_controller_info *
intel_igd_get_controller_info(const struct device *device);

/* Graphics MMIO register read/write APIs */
uint32_t graphics_gtt_read(unsigned long reg);
void graphics_gtt_write(unsigned long reg, uint32_t data);
void graphics_gtt_rmw(unsigned long reg, uint32_t andmask, uint32_t ormask);
uintptr_t graphics_get_framebuffer_address(void);

#endif	/* SOC_INTEL_COMMON_BLOCK_GRAPHICS_H */
