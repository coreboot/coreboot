/* SPDX-License-Identifier: GPL-2.0-or-later */

#define __SIMPLE_DEVICE__

#include <bootmode.h>
#include <bootstate.h>
#include <delay.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_def.h>
#include <device/pci_ops.h>
#include <device/mmio.h>
#include <drivers/intel/gma/i915_reg.h>
#include <soc/pci_devs.h>
#include <soc/ramstage.h>

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

static void panel_power_on(uintptr_t igd_bar)
{
	setbits32((void *)(igd_bar + PCH_PP_CONTROL), PANEL_POWER_ON);
}

static void panel_reset_assert(uintptr_t igd_bar)
{
	clrsetbits32((void *)(igd_bar + PCH_GPIOB),
		GPIO_CLOCK_VAL_OUT,
		GPIO_CLOCK_DIR_MASK | GPIO_CLOCK_DIR_OUT | GPIO_CLOCK_VAL_MASK);
}

static void panel_reset_deassert(uintptr_t igd_bar)
{
	const uint32_t data32 = GPIO_CLOCK_VAL_OUT |
		GPIO_CLOCK_DIR_MASK | GPIO_CLOCK_DIR_OUT | GPIO_CLOCK_VAL_MASK;
	setbits32((void *)(igd_bar + PCH_GPIOB), data32);
}

/*
 * Meliks uses panel-built-in touch screen, it needs to set panel power and
 * reset signal to high for touch screen to work.
 * On user mode, coreboot doesn't initialize graphics since there is no screen
 * display before OS. We would add this WA to initialize required signals on
 * user mode.
 */
static void wa_init_display_signal(void *unused)
{
	struct device *igd_dev = pcidev_path_on_root(SA_DEVFN_IGD);
	uintptr_t igd_bar;

	if (display_init_required() || !igd_dev)
		return;

	igd_bar = find_resource(igd_dev, PCI_BASE_ADDRESS_0)->base;
	if (!igd_bar)
		return;

	panel_power_on(igd_bar);
	mdelay(20);
	panel_reset_deassert(igd_bar);
	mdelay(2);
	panel_reset_assert(igd_bar);
	mdelay(2);
	panel_reset_deassert(igd_bar);
}

BOOT_STATE_INIT_ENTRY(BS_POST_DEVICE, BS_ON_ENTRY, wa_init_display_signal, NULL);
