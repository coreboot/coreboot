/* SPDX-License-Identifier: GPL-2.0-or-later */

#define __SIMPLE_DEVICE__

#include <soc/ramstage.h>
#include <device/pci_def.h>
#include <device/pci_ops.h>
#include <device/mmio.h>

#define DPHY_0_CLOCK_LANE_TIMING	0x162180
#define RESET_DPHY_0_CLOCK_LANE_TIMING	0

static uint32_t igd_mem_base(void)
{
	uint32_t igd_bar = pci_read_config32(SA_DEVFN_IGD, PCI_BASE_ADDRESS_0);

	/* Check if the controller is disabled or not present */
	if (igd_bar == 0 || igd_bar == 0xffffffff)
		return 0;

	return (igd_bar & ~PCI_BASE_ADDRESS_MEM_ATTR_MASK);
}

static void igd_set_mem_base(uint32_t base)
{
	uint16_t igd_cmd;

	pci_write_config32(SA_DEVFN_IGD, PCI_BASE_ADDRESS_0, base);
	igd_cmd = pci_read_config16(SA_DEVFN_IGD, PCI_COMMAND);
	pci_write_config16(SA_DEVFN_IGD, PCI_COMMAND, igd_cmd | PCI_COMMAND_MEMORY);
}

static void reset_display_dphy_clock(void)
{
	uint32_t igd_base = igd_mem_base();
	bool need_temp_bar = false;

	if (!igd_base) {
		need_temp_bar = true;
		igd_base = CONFIG_GFX_GMA_DEFAULT_MMIO;
		igd_set_mem_base(igd_base);
	}

	write32p(igd_base + DPHY_0_CLOCK_LANE_TIMING, RESET_DPHY_0_CLOCK_LANE_TIMING);

	if (need_temp_bar)
		igd_set_mem_base(0);
}

void mainboard_silicon_init_params(FSP_S_CONFIG *params)
{
	/* DPHY CLK timing is not reset if the boot reason is warm boot. Hence to make
	   the function consistent relies on software forcing the DPHY CLK timing to
	   the HW default */
	reset_display_dphy_clock();
}
