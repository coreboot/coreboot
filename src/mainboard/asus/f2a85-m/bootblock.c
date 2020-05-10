/* SPDX-License-Identifier: GPL-2.0-only */

#include <bootblock_common.h>
#include <device/pnp_type.h>
#include <amdblocks/acpimmio.h>
#include <stdint.h>
#include <superio/ite/common/ite.h>
#include <superio/ite/it8728f/it8728f.h>
#include <superio/nuvoton/common/nuvoton.h>
#include <superio/nuvoton/nct6779d/nct6779d.h>

static void sbxxx_enable_48mhzout(void)
{
	/* most likely programming to 48MHz out signal */
	u32 reg32;
	reg32 = misc_read32(0x28);
	reg32 &= 0xffc7ffff;
	reg32 |= 0x00100000;
	misc_write32(0x28, reg32);

	misc_write32(0x40, misc_read32(0x40) & (~0x80u));
}

static void superio_init_m(void)
{
	const pnp_devfn_t uart = PNP_DEV(0x2e, IT8728F_SP1);
	const pnp_devfn_t gpio = PNP_DEV(0x2e, IT8728F_GPIO);

	ite_kill_watchdog(gpio);
	ite_enable_serial(uart, CONFIG_TTYS0_BASE);
	ite_enable_3vsbsw(gpio);
}

static void superio_init_m_pro(void)
{
	const pnp_devfn_t uart = PNP_DEV(0x2e, NCT6779D_SP1);

	nuvoton_enable_serial(uart, CONFIG_TTYS0_BASE);
}

void bootblock_mainboard_early_init(void)
{
	/* enable SIO clock */
	sbxxx_enable_48mhzout();

	if (CONFIG(BOARD_ASUS_F2A85_M_PRO))
		superio_init_m_pro();
	else
		superio_init_m();
}
