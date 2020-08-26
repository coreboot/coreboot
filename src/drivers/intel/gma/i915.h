/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef INTEL_I915_H
#define INTEL_I915_H 1

#include <drivers/intel/gma/i915_reg.h>
#include <drivers/intel/gma/gma.h>
#include <edid.h>

/* port types. We stick with the same defines as the kernel */
#define INTEL_OUTPUT_UNUSED 0
#define INTEL_OUTPUT_ANALOG 1
#define INTEL_OUTPUT_DVO 2
#define INTEL_OUTPUT_SDVO 3
#define INTEL_OUTPUT_LVDS 4
#define INTEL_OUTPUT_TVOUT 5
#define INTEL_OUTPUT_HDMI 6
#define INTEL_OUTPUT_DISPLAYPORT 7
#define INTEL_OUTPUT_EDP 8

/* things that are, strangely, not defined anywhere? */
#define PCH_PP_UNLOCK	0xabcd0000
#define WMx_LP_SR_EN	(1UL<<31)
#define PRB0_TAIL	0x02030
#define PRB0_HEAD	0x02034
#define PRB0_START	0x02038
#define PRB0_CTL	0x0203c

enum port {
	PORT_A = 0,
	PORT_B,
	PORT_C,
	PORT_D,
	PORT_E,
	I915_NUM_PORTS
};

enum pipe {
	PIPE_A = 0,
	PIPE_B,
	PIPE_C,
	I915_NUM_PIPES
};

enum transcoder {
	TRANSCODER_A = 0,
	TRANSCODER_B,
	TRANSCODER_C,
	TRANSCODER_EDP = 0xF,
};

enum plane {
	PLANE_A = 0,
	PLANE_B,
	PLANE_C,
};

/* debug enums. These are for printks that, due to their place in the
 * middle of graphics device IO, might change timing.  Use with care
 * or not at all.
 */
enum {
	vio = 2, /* dump every IO */
	vspin = 4, /* print # of times we spun on a register value */
};

/* The mainboard must provide these functions. */
unsigned long io_i915_read32(unsigned long addr);
void io_i915_write32(unsigned long val, unsigned long addr);

void intel_prepare_ddi(void);

int gtt_poll(u32 reg, u32 mask, u32 value);
void gtt_write(u32 reg, u32 data);
u32 gtt_read(u32 reg);

/* vbt.c */
struct device;
void
generate_fake_intel_oprom(const struct i915_gpu_controller_info *conf,
			  struct device *dev, const char *idstr);

#endif
