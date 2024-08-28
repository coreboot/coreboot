/* SPDX-License-Identifier: GPL-2.0-only */

#include <arch/stages.h>
#include <delay.h>
#include <soc/clkbuf.h>
#include <soc/emi.h>
#include <soc/i2c.h>
#include <soc/mt6315.h>
#include <soc/mt6359p.h>
#include <soc/mt6360.h>
#include <soc/pcie.h>
#include <soc/pll_common.h>
#include <soc/pmif.h>
#include <soc/rtc.h>
#include <soc/scp.h>

static void raise_little_cpu_freq(void)
{
	mt6359p_buck_set_voltage(MT6359P_SRAM_PROC1, 1000 * 1000);
	mt6359p_buck_set_voltage(MT6359P_CORE, 1000 * 1000);
	udelay(200);
	mt_pll_raise_little_cpu_freq(2000 * MHz);
	mt_pll_raise_cci_freq(1385 * MHz);
}

void platform_romstage_main(void)
{
	mtk_pmif_init();
	mt6359p_init();
	mt6315_init();
	raise_little_cpu_freq();
	mtk_i2c_bus_init(I2C7, I2C_SPEED_FAST);
	if (CONFIG(BOARD_GOOGLE_CHERRY))
		mt6360_init(I2C7);
	clk_buf_init();
	rtc_boot();
	if (CONFIG(PCI))
		mtk_pcie_deassert_perst();
	mtk_dram_init();
	scp_rsi_enable();
}
