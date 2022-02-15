/* SPDX-License-Identifier: GPL-2.0-only */

#include <arch/exception.h>
#include <armv7.h>
#include <cbmem.h>
#include <console/console.h>
#include <program_loading.h>
#include <device/i2c_simple.h>
#include <drivers/maxim/max77686/max77686.h>
#include <soc/clk.h>
#include <soc/cpu.h>
#include <soc/dmc.h>
#include <soc/gpio.h>
#include <soc/i2c.h>
#include <soc/setup.h>
#include <soc/periph.h>
#include <soc/power.h>
#include <soc/trustzone.h>
#include <soc/wakeup.h>
#include <timestamp.h>

#include "exynos5250.h"

static void setup_power(int is_resume)
{
	int error = 0;

	power_init();

	if (is_resume) {
		return;
	}

	/* Initialize I2C bus to configure PMIC. */
	exynos_pinmux_i2c0();
	i2c_init(0, I2C_0_SPEED, 0x00);

	printk(BIOS_DEBUG, "%s: Setting up PMIC...\n", __func__);
	/*
	 * We're using CR1616 coin cell battery that is non-rechargeable
	 * battery. But, BBCHOSTEN bit of the BBAT Charger Register in
	 * MAX77686 is enabled by default for charging coin cell.
	 *
	 * Also, we cannot meet the coin cell reverse current spec. in UL
	 * standard if BBCHOSTEN bit is enabled.
	 *
	 * Disable Coin BATT Charging
	 */
	error = max77686_disable_backup_batt(CONFIG_PMIC_BUS);

	error |= max77686_volsetting(CONFIG_PMIC_BUS, PMIC_BUCK2, VDD_ARM_MV,
						REG_ENABLE, MAX77686_MV);
	error |= max77686_volsetting(CONFIG_PMIC_BUS, PMIC_BUCK3, VDD_INT_UV,
						REG_ENABLE, MAX77686_UV);
	error |= max77686_volsetting(CONFIG_PMIC_BUS, PMIC_BUCK1, VDD_MIF_MV,
						REG_ENABLE, MAX77686_MV);
	error |= max77686_volsetting(CONFIG_PMIC_BUS, PMIC_BUCK4, VDD_G3D_MV,
						REG_ENABLE, MAX77686_MV);
	error |= max77686_volsetting(CONFIG_PMIC_BUS, PMIC_LDO2, VDD_LDO2_MV,
						REG_ENABLE, MAX77686_MV);
	error |= max77686_volsetting(CONFIG_PMIC_BUS, PMIC_LDO3, VDD_LDO3_MV,
						REG_ENABLE, MAX77686_MV);
	error |= max77686_volsetting(CONFIG_PMIC_BUS, PMIC_LDO5, VDD_LDO5_MV,
						REG_ENABLE, MAX77686_MV);
	error |= max77686_volsetting(CONFIG_PMIC_BUS, PMIC_LDO10, VDD_LDO10_MV,
						REG_ENABLE, MAX77686_MV);

	error |= max77686_enable_32khz_cp(CONFIG_PMIC_BUS);

	if (error) {
		printk(BIOS_CRIT, "%s: PMIC error: %#x\n", __func__, error);
		die("Failed to initialize PMIC.\n");
	}
}

static void setup_graphics(void)
{
	exynos_pinmux_dphpd();
}

static void setup_gpio(void)
{
	gpio_direction_input(GPIO_D16); // WP_GPIO
	gpio_set_pull(GPIO_D16, GPIO_PULL_NONE);

	gpio_direction_input(GPIO_Y10); // RECMODE_GPIO
	gpio_set_pull(GPIO_Y10, GPIO_PULL_NONE);

	gpio_direction_input(GPIO_X35); // LID_GPIO
	gpio_set_pull(GPIO_X35, GPIO_PULL_NONE);

	gpio_direction_input(GPIO_X13); // POWER_GPIO
	gpio_set_pull(GPIO_X13, GPIO_PULL_NONE);
}

static void setup_memory(struct mem_timings *mem, int is_resume)
{
	printk(BIOS_SPEW, "man: 0x%x type: 0x%x, div: 0x%x, mhz: %d\n",
	       mem->mem_manuf,
	       mem->mem_type,
	       mem->mpll_mdiv,
	       mem->frequency_mhz);

	if (ddr3_mem_ctrl_init(mem, DMC_INTERLEAVE_SIZE, !is_resume)) {
		die("Failed to initialize memory controller.\n");
	}
}

static struct mem_timings *setup_clock(void)
{
	struct mem_timings *mem = get_mem_timings();
	struct arm_clk_ratios *arm_ratios = get_arm_clk_ratios();
	if (!mem) {
		die("Unable to auto-detect memory timings\n");
	}
	system_clock_init(mem, arm_ratios);
	return mem;
}

void main(void)
{
	struct mem_timings *mem;
	int is_resume = (get_wakeup_state() != IS_NOT_WAKEUP);

	timestamp_init(timestamp_get());
	timestamp_add_now(TS_ROMSTAGE_START);

	/* Clock must be initialized before console_init, otherwise you may need
	 * to re-initialize serial console drivers again. */
	mem = setup_clock();

	console_init();
	exception_init();

	setup_power(is_resume);

	timestamp_add_now(TS_INITRAM_START);

	setup_memory(mem, is_resume);

	timestamp_add_now(TS_INITRAM_END);

	/* This needs to happen on normal boots and on resume. */
	trustzone_init();

	if (is_resume) {
		wakeup();
	}

	setup_gpio();
	setup_graphics();

	/* Set SPI (primary CBFS media) clock to 50MHz and configure pinmux. */
	exynos_pinmux_spi1();
	clock_set_rate(PERIPH_ID_SPI1, 50000000);

	cbmem_initialize_empty();

	run_ramstage();
}
