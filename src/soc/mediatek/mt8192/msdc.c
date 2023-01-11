/* SPDX-License-Identifier: GPL-2.0-only */

#include <device/mmio.h>
#include <gpio.h>
#include <soc/addressmap.h>
#include <soc/regulator.h>
#include <soc/msdc.h>

#define MSDC0_BASE	0x11f60000
#define MSDC0_TOP_BASE	0x11f50000

#define MSDC0_DRV_MASK	0x3fffffff
#define MSDC1_DRV_MASK	0x3ffff000
#define MSDC0_DRV_VALUE	0x24924924
#define MSDC1_DRV_VALUE	0x1b6db000

#define MSDC1_GPIO_MODE0_BASE	0x10005360
#define MSDC1_GPIO_MODE0_MASK	0x77777000
#define MSDC1_GPIO_MODE0_VALUE	0x11111000

#define MSDC1_GPIO_MODE1_BASE	0x10005370
#define MSDC1_GPIO_MODE1_MASK	0x7
#define MSDC1_GPIO_MODE1_VALUE	0x1

void mtk_msdc_configure_emmc(bool is_early_init)
{
	void *gpio_base = (void *)IOCFG_TL_BASE;
	int i;

	const gpio_t emmc_pu_pin[] = {
		GPIO(MSDC0_DAT0), GPIO(MSDC0_DAT1),
		GPIO(MSDC0_DAT2), GPIO(MSDC0_DAT3),
		GPIO(MSDC0_DAT4), GPIO(MSDC0_DAT5),
		GPIO(MSDC0_DAT6), GPIO(MSDC0_DAT7),
		GPIO(MSDC0_CMD), GPIO(MSDC0_RSTB),
	};

	const gpio_t emmc_pd_pin[] = {
		GPIO(MSDC0_DSL), GPIO(MSDC0_CLK),
	};

	for (i = 0; i < ARRAY_SIZE(emmc_pu_pin); i++)
		gpio_set_pull(emmc_pu_pin[i], GPIO_PULL_ENABLE, GPIO_PULL_UP);

	for (i = 0; i < ARRAY_SIZE(emmc_pd_pin); i++)
		gpio_set_pull(emmc_pd_pin[i], GPIO_PULL_ENABLE, GPIO_PULL_DOWN);

	/* set eMMC cmd/dat/clk/ds/rstb pins driving to 10mA */
	clrsetbits32(gpio_base, MSDC0_DRV_MASK, MSDC0_DRV_VALUE);

	if (is_early_init)
		mtk_emmc_early_init((void *)MSDC0_BASE, (void *)MSDC0_TOP_BASE);
}

void mtk_msdc_configure_sdcard(void)
{
	void *gpio_base = (void *)IOCFG_RM_BASE;
	void *gpio_mode0_base = (void *)MSDC1_GPIO_MODE0_BASE;
	void *gpio_mode1_base = (void *)MSDC1_GPIO_MODE1_BASE;
	int i;

	const gpio_t sdcard_pu_pin[] = {
		GPIO(MSDC1_DAT0), GPIO(MSDC1_DAT1),
		GPIO(MSDC1_DAT2), GPIO(MSDC1_DAT3),
		GPIO(MSDC1_CMD),
	};

	const gpio_t sdcard_pd_pin[] = {
		GPIO(MSDC1_CLK),
	};

	for (i = 0; i < ARRAY_SIZE(sdcard_pu_pin); i++)
		gpio_set_pull(sdcard_pu_pin[i], GPIO_PULL_ENABLE, GPIO_PULL_UP);

	for (i = 0; i < ARRAY_SIZE(sdcard_pd_pin); i++)
		gpio_set_pull(sdcard_pd_pin[i], GPIO_PULL_ENABLE, GPIO_PULL_DOWN);

	/* set sdcard cmd/dat/clk pins driving to 8mA */
	clrsetbits32(gpio_base, MSDC1_DRV_MASK, MSDC1_DRV_VALUE);

	/* set sdcard dat2/dat0/dat3/cmd/clk pins to msdc1 mode */
	clrsetbits32(gpio_mode0_base, MSDC1_GPIO_MODE0_MASK, MSDC1_GPIO_MODE0_VALUE);

	/* set sdcard dat1 pin to msdc1 mode */
	clrsetbits32(gpio_mode1_base, MSDC1_GPIO_MODE1_MASK, MSDC1_GPIO_MODE1_VALUE);

	mainboard_enable_regulator(MTK_REGULATOR_VCC, true);
	mainboard_enable_regulator(MTK_REGULATOR_VCCQ, true);
}
