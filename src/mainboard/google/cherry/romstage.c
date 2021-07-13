/* SPDX-License-Identifier: GPL-2.0-only */

#include <arch/stages.h>
#include <soc/clkbuf.h>
#include <soc/emi.h>
#include <soc/i2c.h>
#include <soc/mt6315.h>
#include <soc/mt6359p.h>
#include <soc/mt6360.h>
#include <soc/pmif.h>
#include <soc/rtc.h>
#include <soc/scp.h>

#define I2C_BUS 7

void platform_romstage_main(void)
{
	mtk_pmif_init();
	mt6359p_init();
	mt6315_init();
	mtk_i2c_bus_init(I2C_BUS);
	if (CONFIG(BOARD_GOOGLE_CHERRY))
		mt6360_init(I2C_BUS);
	clk_buf_init();
	rtc_boot();
	mtk_dram_init();
	scp_rsi_enable();
}
