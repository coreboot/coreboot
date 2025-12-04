/* SPDX-License-Identifier: GPL-2.0-only */

#include <device/pci.h>
#include <soc/pci_devs.h>
#include <static.h>
#include <tdp.h>

enum soc_intel_pantherlake_cpu_tdps soc_get_cpu_tdp(void)
{
	if (CONFIG(MAINBOARD_HAS_CHROMEOS)) {
		const unsigned int variable_tdp_mch_ids[] = {
			PCI_DID_INTEL_PTL_U_ID_1,
			PCI_DID_INTEL_PTL_U_ID_2,
			PCI_DID_INTEL_PTL_U_ID_3
		};
		uint16_t mch_id = pci_read_config16(_PCI_DEV(ROOT, 0), PCI_DEVICE_ID);

		for (size_t i = 0; i < ARRAY_SIZE(variable_tdp_mch_ids); i++)
			if (variable_tdp_mch_ids[i] == mch_id)
				return TDP_15W;
	}

	const struct soc_intel_pantherlake_config *config = config_of_soc();
	return config->tdp || get_cpu_tdp();
}
