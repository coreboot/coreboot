/* SPDX-License-Identifier: GPL-2.0-only */

#include <device/mmio.h>
#include <intelblocks/pmc.h>
#include <soc/iomap.h>
#include <soc/pm.h>

static void ioe_pmc_read_resources(struct device *dev)
{
	/* Add the fixed MMIO resource */
	mmio_range(dev, PWRMBASE, IOE_PWRM_BASE_ADDRESS, IOE_PWRM_BASE_SIZE);
}

static void ioe_pmc_init(struct device *dev)
{
	if (!CONFIG(USE_PM_ACPI_TIMER))
		setbits8(ioe_pmc_mmio_regs() + PCH_PWRM_ACPI_TMR_CTL, ACPI_TIM_DIS);
}

struct device_operations ioe_pmc_ops = {
	.read_resources = ioe_pmc_read_resources,
	.set_resources  = noop_set_resources,
	.init   = ioe_pmc_init,
};
