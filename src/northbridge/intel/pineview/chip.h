#include <drivers/intel/gma/i915.h>

struct northbridge_intel_pineview_config {
	u32 gpu_hotplug;
	u32 gpu_backlight;
	int gpu_lvds_use_spread_spectrum_clock;
	struct i915_gpu_controller_info gfx;
};
