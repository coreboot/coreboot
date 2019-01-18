/* SPDX-License-Identifier: GPL-2.0-only */

#include <amdblocks/acpimmio.h>
#include <bootblock_common.h>
#include <device/pnp_type.h>
#include <superio/ite/common/ite.h>
#include <superio/ite/it8728f/it8728f.h>

static void sbxxx_enable_48mhzout(void)
{
	/* Set auxiliary output clock frequency on OSCOUT2 pin to be 48MHz */
	u32 reg32;
	reg32 = misc_read32(0x28);
	reg32 &= ~(7 << 19);
	reg32 |=  (2 << 19);
	misc_write32(0x28, reg32);

	/* Enable Auxiliary OSCOUT2 */
	misc_write32(0x40, misc_read32(0x40) & ~(1 << 7));
}

static void superio_init_m(void)
{
	const pnp_devfn_t uart = PNP_DEV(0x2e, IT8728F_SP1);
	const pnp_devfn_t gpio = PNP_DEV(0x2e, IT8728F_GPIO);

	ite_kill_watchdog(gpio);
	ite_enable_serial(uart, CONFIG_TTYS0_BASE);
	ite_enable_3vsbsw(gpio);
}

void bootblock_mainboard_early_init(void)
{
	/* enable SIO clock */
	sbxxx_enable_48mhzout();

	superio_init_m();
}
