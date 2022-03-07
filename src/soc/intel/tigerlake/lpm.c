/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <arch/cpu.h>
#include <device/device.h>
#include <device/pci_ops.h>
#include <device/pci_ids.h>
#include <cpu/intel/cpu_ids.h>
#include <soc/lpm.h>
#include <soc/pci_devs.h>
#include <soc/soc_chip.h>
#include <types.h>

/* Function returns true if the platform is TGL-UP3 */
static bool platform_is_up3(void)
{
	const struct device *dev = pcidev_path_on_root(SA_DEVFN_ROOT);
	u32 cpu_id = cpu_get_cpuid();
	uint16_t mchid = pci_read_config16(dev, PCI_DEVICE_ID);

	if ((cpu_id != CPUID_TIGERLAKE_A0) && (cpu_id != CPUID_TIGERLAKE_B0))
		return false;

	return ((mchid == PCI_DID_INTEL_TGL_ID_U_2_2) ||
		(mchid == PCI_DID_INTEL_TGL_ID_U_4_2));
}

int get_supported_lpm_mask(struct soc_intel_tigerlake_config *config)
{
	int disable_mask;

	/* Disable any sub-states requested by mainboard */
	disable_mask = config->LpmStateDisableMask;

	/* UP3 does not support S0i2.2/S0i3.3/S0i3.4 */
	if (platform_is_up3())
		disable_mask |= LPM_S0i3_3 | LPM_S0i3_4 | LPM_S0i2_2;

	/* If external bypass is not used, S0i3 isn't recommended. */
	if (config->external_bypass == false)
		disable_mask |= LPM_S0i3_0 | LPM_S0i3_1 | LPM_S0i3_2 | LPM_S0i3_3 | LPM_S0i3_4;

	/* If external clock gating is not implemented, S0i3.4 isn't recommended. */
	if (config->external_clk_gated == false)
		disable_mask |= LPM_S0i3_4;

	/*
	 * If external phy gating is not implemented,
	 * S0i3.3/S0i3.4/S0i2.2 are not recommended.
	 */
	if (config->external_phy_gated == false)
		disable_mask |= LPM_S0i3_3 | LPM_S0i3_4 | LPM_S0i2_2;

	/* If CNVi or ISH is used, S0i3.2/S0i3.3/S0i3.4 cannot be achieved. */
	if (is_devfn_enabled(PCH_DEVFN_CNVI_WIFI) || is_devfn_enabled(PCH_DEVFN_ISH))
		disable_mask |= LPM_S0i3_2 | LPM_S0i3_3 | LPM_S0i3_4;

	return LPM_S0iX_ALL & ~disable_mask;
}
