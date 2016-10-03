#ifndef NORTHBRIDGE_INTEL_I945_CHIP_H
#define NORTHBRIDGE_INTEL_I945_CHIP_H

#include <drivers/intel/gma/i915.h>

struct northbridge_intel_i945_config {
	u32 gpu_hotplug;
	u32 gpu_backlight;
	int gpu_lvds_use_spread_spectrum_clock;
	struct i915_gpu_controller_info gfx;
	int pci_mmio_size;
};

#endif /* NORTHBRIDGE_INTEL_I945_CHIP_H */
