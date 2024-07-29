/* SPDX-License-Identifier: GPL-2.0-only */

#include <fsp/api.h>
#include <intelblocks/aspm.h>
#include <intelblocks/pcie_rp.h>
#include <option.h>
#include <soc/soc_chip.h>
#include <soc/pcie.h>

/*
 * The PCIe RP ASPM and PCIe L1 Substate UPDs follow the PCI Express Base
 * Specification 1.1. The UPDs and their default values are consistent
 * from Skylake through Meteor Lake. However, the default for CPU ports
 * differs from PCH ports. Use auto and maximum unless overwritten
 * to make the behaviour consistent.
 *
 * +-------------------+--------------------------+-----------+-----------+
 * | Setting           | Option                   | PCH Ports | CPU Ports |
 * |-------------------|--------------------------|-----------|-----------|
 * | PcieRpEnableCpm   | Disabled                 | [Default] | [Default] |
 * |                   | Enabled                  |           |           |
 * |-------------------|--------------------------|-----------|-----------|
 * | PcieRpAspm        | PchPcieAspmDisabled      |           |           |
 * |                   | PchPcieAspmL0s           |           |           |
 * |                   | PchPcieAspmL1            |           |           |
 * |                   | PchPcieAspmL0sL1         |           | [Default] |
 * |                   | PchPcieAspmAutoConfig    | [Default] |           |
 * |                   | PchPcieAspmMax           |           |           |
 * |-------------------|--------------------------|-----------|-----------|
 * | PcieRpL1Substates | Disabled                 |           |           |
 * |                   | PchPcieL1SubstatesL1_1   |           |           |
 * |                   | PchPcieL1SubstatesL1_1_2 |           | [Default] |
 * |                   | PchPcieL1SubstatesMax    | [Default] |           |
 * |-------------------|--------------------------|-----------|-----------|
 * | PchPcieRpPcieSpeed| PchPcieRpPcieSpeedAuto   | [Default] |           |
 * |                   | PchPcieRpPcieSpeedGen1   |           |           |
 * |                   | PchPcieRpPcieSpeedGen2   |           |           |
 * |                   | PchPcieRpPcieSpeedGen3   |           |           |
 * |                   | PchPcieRpPcieSpeedGen4   |           |           |
 * +-------------------+--------------------------+-----------+-----------+
 */

static unsigned int aspm_control_to_upd(enum ASPM_control aspm_control)
{
	/* Disable without Kconfig selected */
	if (!CONFIG(PCIEXP_ASPM))
		return UPD_INDEX(ASPM_DISABLE);

	/* Use auto unless overwritten */
	if (!aspm_control)
		return UPD_INDEX(ASPM_AUTO);

	return UPD_INDEX(aspm_control);
}

static unsigned int l1ss_control_to_upd(enum L1_substates_control l1_substates_control)
{
	/* Disable without Kconfig selected */
	if (!CONFIG(PCIEXP_ASPM))
		return UPD_INDEX(L1_SS_DISABLED);

	/* Don't enable UPD if Kconfig not set */
	if (!CONFIG(PCIEXP_L1_SUB_STATE))
		return UPD_INDEX(L1_SS_DISABLED);

	/* L1 Substate should be disabled in compliance mode */
	if (CONFIG(SOC_INTEL_COMPLIANCE_TEST_MODE))
		return UPD_INDEX(L1_SS_DISABLED);

	/* Use maximum unless overwritten */
	if (!l1_substates_control)
		return UPD_INDEX(L1_SS_L1_2);

	return UPD_INDEX(l1_substates_control);
}

static unsigned int pcie_speed_control_to_upd(enum PCIE_SPEED_control pcie_speed_control)
{
	/* Use auto unless overwritten */
	if (!pcie_speed_control)
		return UPD_INDEX(SPEED_AUTO);

	return UPD_INDEX(pcie_speed_control);
}

void configure_pch_rp_power_management(FSP_S_CONFIG *s_cfg,
					      const struct pcie_rp_config *rp_cfg,
					      unsigned int index)
{
	s_cfg->PcieRpEnableCpm[index] =
		get_uint_option("pciexp_clk_pm", CONFIG(PCIEXP_CLK_PM));
	s_cfg->PcieRpAspm[index] =
		aspm_control_to_upd(get_uint_option("pciexp_aspm", rp_cfg->pcie_rp_aspm));
	s_cfg->PcieRpL1Substates[index] =
		l1ss_control_to_upd(get_uint_option("pciexp_l1ss", rp_cfg->PcieRpL1Substates));
	s_cfg->PcieRpPcieSpeed[index] =
		pcie_speed_control_to_upd(get_uint_option("pciexp_speed", rp_cfg->pcie_rp_pcie_speed));
}

#if CONFIG(HAS_INTEL_CPU_ROOT_PORTS)
/*
 * Starting with Alder Lake, UPDs for Clock Power Management were
 * introduced for the CPU root ports.
 *
 * CpuPcieClockGating:
 *	Disabled
 *	Enabled		[Default]
 *
 * CpuPciePowerGating
 *	Disabled
 *	Enabled		[Default]
 *
 */
void configure_cpu_rp_power_management(FSP_S_CONFIG *s_cfg,
					      const struct pcie_rp_config *rp_cfg,
					      unsigned int index)
{
	bool pciexp_clk_pm = get_uint_option("pciexp_clk_pm", CONFIG(PCIEXP_CLK_PM));
	s_cfg->CpuPcieRpEnableCpm[index] = pciexp_clk_pm;
	s_cfg->CpuPcieClockGating[index] = pciexp_clk_pm;
	s_cfg->CpuPciePowerGating[index] = pciexp_clk_pm;
	s_cfg->CpuPcieRpAspm[index] =
		aspm_control_to_upd(get_uint_option("pciexp_aspm", rp_cfg->pcie_rp_aspm));
	s_cfg->CpuPcieRpL1Substates[index] =
		l1ss_control_to_upd(get_uint_option("pciexp_l1ss", rp_cfg->PcieRpL1Substates));
}

#endif	// CONFIG(HAS_INTEL_CPU_ROOT_PORTS)
