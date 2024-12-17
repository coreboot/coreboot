/* SPDX-License-Identifier: GPL-2.0-only OR MIT */

#include <console/console.h>
#include <device/mmio.h>
#include <soc/mtk_pwrsel.h>

static void cpu_pwrsel_init(void)
{
	write32p(MCUSYS_BASE + OFFSET_PWRSEL, VAL_PWRSEL);
	write32p(MCUSYS_BASE + OFFSET_PWRSEL_AUTO_MODE_CFG, VAL_PWRSEL_AUTO_MODE);
}

static void gpu_pwrsel_init(void)
{
	write32p(MFG_VCORE_AO_RPC_PWRSEL_CONFIG, GENMASK(14, 0));
}

void pwrsel_init(void)
{
	cpu_pwrsel_init();
	gpu_pwrsel_init();

	/* PWR_SEL must be 0x0 */
	printk(BIOS_DEBUG, "PWR_SEL = %#x\n", read32p(MCUSYS_BASE + OFFSET_PWRSEL));
	/* PWRSEL_CONFIG must be 0x7fff */
	printk(BIOS_DEBUG, "PWRSEL_CONFIG = %#x\n", read32p(MFG_VCORE_AO_RPC_PWRSEL_CONFIG));
}
