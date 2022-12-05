/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <bootmode.h>
#include <bootstate.h>
#include <delay.h>
#include <device/device.h>
#include <device/mmio.h>
#include <device/pci.h>
#include <drivers/intel/gma/i915_reg.h>
#include <soc/pci_devs.h>

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
 * Bugzzy uses panel-built-in touch screen, it needs to set panel power and
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
