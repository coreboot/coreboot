/* SPDX-License-Identifier: GPL-2.0-only */

#include <device/pci_ops.h>
#include <soc/crashlog.h>
#include <soc/pci_devs.h>

static uint32_t read_msm_config_reg(uint32_t func, uint32_t reg)
{
	uint32_t pcie_offset = PCI_DEV(MSM_BUS, MSM_DEV, func);
	return pci_s_read_config32(pcie_offset, reg);
}

static void write_msm_config_reg(uint32_t func, uint32_t reg, uint32_t value)
{
	uint32_t pcie_offset = PCI_DEV(MSM_BUS, MSM_DEV, func);
	pci_s_write_config32(pcie_offset, reg, value);
}

void disable_cpu_crashlog(void)
{
	uint32_t ctl;
	ctl = read_msm_config_reg(MSM_FUN, CRASHLOG_CTL);
	ctl |= CRASHLOG_CTL_DIS;
	write_msm_config_reg(MSM_FUN, CRASHLOG_CTL, ctl);
	ctl = read_msm_config_reg(MSM_FUN_PMON, BIOS_CRASHLOG_CTL);
	ctl |= CRASHLOG_CTL_DIS;
	write_msm_config_reg(MSM_FUN_PMON, BIOS_CRASHLOG_CTL, ctl);
}
