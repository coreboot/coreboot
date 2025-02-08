/* SPDX-License-Identifier: GPL-2.0-only */

#include <option.h>
#include <device/pci_ops.h>
#include <device/pci_def.h>
#include <northbridge/intel/sandybridge/sandybridge.h>
#include <ec/lenovo/pmh7/pmh7.h>
#include <types.h>

void mainboard_early_init(bool s3resume)
{
	u8 enable_peg = get_uint_option("enable_dual_graphics", 0);

	bool power_en = pmh7_dgpu_power_state();

	if (enable_peg != power_en)
		pmh7_dgpu_power_enable(!power_en);

	if (!enable_peg) {
		// Hide disabled dGPU device
		pci_and_config32(HOST_BRIDGE, DEVEN, ~DEVEN_PEG10);
	}
}
