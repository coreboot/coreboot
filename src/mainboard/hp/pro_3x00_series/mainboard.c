/* SPDX-License-Identifier: GPL-2.0-only */

#include <cpu/intel/model_206ax/model_206ax.h>
#include <cpu/x86/msr.h>
#include <device/device.h>
#include <static_devices.h>
#include <superio/ite/it8772f/chip.h>

static u8 cpu_get_temp_offset(void)
{
	msr_t msr = rdmsr(MSR_TEMPERATURE_TARGET);
	u8 tj_max = (msr.lo >> 16) & 0xFF;
	u8 tcc_offset = (msr.lo >> 24) & 0xf;

	msr = rdmsr(MSR_PLATFORM_INFO);
	if (msr.lo & (1 << 30)) {
		return tj_max - tcc_offset;
	} else {
		return tj_max;
	}
}

static void mainboard_enable(struct device *dev)
{
	/* Set PECI TMPIN 3 offset to TJ_MAX temperature */
	struct superio_ite_it8772f_config *const config = __pnp_002e_04->chip_info;
	if (config)
		config->ec.tmpin[2].offset = cpu_get_temp_offset();
}

struct chip_operations mainboard_ops = {
	.enable_dev = mainboard_enable,
};
