/*
 * This file is part of the coreboot project.
 *
 * Copyright 2013 Google Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */

#include <types.h>
#include <stdlib.h>

#include <armv7.h>
#include <cbfs.h>

#include <arch/cache.h>
#include <cpu/samsung/exynos5420/i2c.h>
#include <cpu/samsung/exynos5420/clk.h>
#include <cpu/samsung/exynos5420/cpu.h>
#include <cpu/samsung/exynos5420/dmc.h>
#include <cpu/samsung/exynos5420/gpio.h>
#include <cpu/samsung/exynos5420/setup.h>
#include <cpu/samsung/exynos5420/periph.h>
#include <cpu/samsung/exynos5420/power.h>
#include <cpu/samsung/exynos5420/wakeup.h>
#include <console/console.h>
#include <arch/stages.h>

#include <drivers/maxim/max77802/max77802.h>
#include <device/i2c.h>

#include "exynos5420.h"

#define MMC0_GPIO_PIN	(58)

struct pmic_write
{
	int or_orig; // Whether to or in the original value.
	uint8_t reg; // Register to write.
	uint8_t val; // Value to write.
};

/*
 * Use read-modify-write for MAX77802 control registers and clobber the
 * output voltage setting (BUCK?DVS?) registers.
 */
struct pmic_write pmic_writes[] =
{
	{ 1, MAX77802_REG_PMIC_32KHZ, MAX77802_32KHCP_EN },
	{ 0, MAX77802_REG_PMIC_BUCK1DVS1, MAX77802_BUCK1DVS1_1V },
	{ 1, MAX77802_REG_PMIC_BUCK1CTRL, MAX77802_BUCK_TYPE1_ON |
					  MAX77802_BUCK_TYPE1_IGNORE_PWRREQ },
	{ 0, MAX77802_REG_PMIC_BUCK2DVS1, MAX77802_BUCK2DVS1_1V },
	{ 1, MAX77802_REG_PMIC_BUCK2CTRL1, MAX77802_BUCK_TYPE2_ON |
					   MAX77802_BUCK_TYPE2_IGNORE_PWRREQ },
	{ 0, MAX77802_REG_PMIC_BUCK3DVS1, MAX77802_BUCK3DVS1_1V },
	{ 1, MAX77802_REG_PMIC_BUCK3CTRL1, MAX77802_BUCK_TYPE2_ON |
					   MAX77802_BUCK_TYPE2_IGNORE_PWRREQ },
	{ 0, MAX77802_REG_PMIC_BUCK4DVS1, MAX77802_BUCK4DVS1_1V },
	{ 1, MAX77802_REG_PMIC_BUCK4CTRL1, MAX77802_BUCK_TYPE2_ON |
					   MAX77802_BUCK_TYPE2_IGNORE_PWRREQ },
	{ 0, MAX77802_REG_PMIC_BUCK6DVS1, MAX77802_BUCK6DVS1_1V },
	{ 1, MAX77802_REG_PMIC_BUCK6CTRL, MAX77802_BUCK_TYPE1_ON |
					   MAX77802_BUCK_TYPE1_IGNORE_PWRREQ },
	{ 1, MAX77802_REG_PMIC_LDO35CTRL1, MAX77802_LDO35CTRL1_1_2V },
};

static void setup_power(int is_resume)
{
	int error = 0;
	int i;

	power_init();

	if (is_resume) {
		return;
	}

	/* Initialize I2C bus to configure PMIC. */
	exynos_pinmux_i2c4();
	i2c_init(4, I2C_4_SPEED, 0x00);

	printk(BIOS_DEBUG, "%s: Setting up PMIC...\n", __func__);

	for (i = 0; i < ARRAY_SIZE(pmic_writes); i++) {
		uint8_t data = 0;
		uint8_t reg = pmic_writes[i].reg;

		if (pmic_writes[i].or_orig)
			error |= i2c_read(4, MAX77802_I2C_ADDR,
					  reg, sizeof(reg),
					  &data, sizeof(data));
		data |= pmic_writes[i].val;
		error |= i2c_write(4, MAX77802_I2C_ADDR,
				   reg, sizeof(reg),
				   &data, sizeof(data));
	}

	if (error)
		die("Failed to intialize PMIC.\n");
}

static void setup_storage(void)
{
	/* MMC0: Fixed, 8 bit mode, connected with GPIO. */
	if (clock_set_mshci(PERIPH_ID_SDMMC0))
		printk(BIOS_CRIT, "%s: Failed to set MMC0 clock.\n", __func__);
	if (gpio_direction_output(MMC0_GPIO_PIN, 1)) {
		printk(BIOS_CRIT, "%s: Unable to power on MMC0.\n", __func__);
	}
	gpio_set_pull(MMC0_GPIO_PIN, GPIO_PULL_NONE);
	gpio_set_drv(MMC0_GPIO_PIN, GPIO_DRV_4X);
	exynos_pinmux_sdmmc0();

	/* MMC2: Removable, 4 bit mode, no GPIO. */
	clock_set_mshci(PERIPH_ID_SDMMC2);
	exynos_pinmux_sdmmc2();
}

static void setup_ec(void)
{
	/* SPI2 (EC) is slower and needs to work in half-duplex mode with
	 * single byte bus width. */
	clock_set_rate(PERIPH_ID_SPI2, 500000);
	exynos_pinmux_spi2();
}

static void setup_gpio(void)
{
	gpio_direction_input(GPIO_X30); // WP_GPIO
	gpio_set_pull(GPIO_X30, GPIO_PULL_NONE);

	gpio_direction_input(GPIO_X07); // RECMODE_GPIO
	gpio_set_pull(GPIO_X07, GPIO_PULL_NONE);

	gpio_direction_input(GPIO_X34); // LID_GPIO
	gpio_set_pull(GPIO_X34, GPIO_PULL_NONE);

	gpio_direction_input(GPIO_X12); // POWER_GPIO
	gpio_set_pull(GPIO_X12, GPIO_PULL_NONE);
}

static void setup_memory(struct mem_timings *mem, int is_resume)
{
	printk(BIOS_SPEW, "manufacturer: 0x%x type: 0x%x, div: 0x%x, mhz: %d\n",
	       mem->mem_manuf,
	       mem->mem_type,
	       mem->mpll_mdiv,
	       mem->frequency_mhz);

	/* FIXME Currently memory initialization with mem_reset on normal boot
	 * will cause resume to fail (even if we don't do mem_reset on resume),
	 * and the workaround is to temporarily always enable "is_resume".
	 * This should be removed when the root cause of resume issue is found.
	 */
	is_resume = 1;

	if (ddr3_mem_ctrl_init(mem, DMC_INTERLEAVE_SIZE, !is_resume)) {
		die("Failed to initialize memory controller.\n");
	}
}

#define PRIMITIVE_MEM_TEST 0
#if PRIMITIVE_MEM_TEST
static unsigned long primitive_mem_test(void)
{
	unsigned long *l = (void *)0x40000000;
	int bad = 0;
	unsigned long i;
	for(i = 0; i < 256*1048576; i++){
		if (! (i%1048576))
			printk(BIOS_SPEW, "%lu ...", i);
		l[i] = 0xffffffff - i;
	}

	for(i = 0; i < 256*1048576; i++){
		if (! (i%1048576))
			printk(BIOS_SPEW, "%lu ...", i);
		if (l[i] != (0xffffffff - i)){
			printk(BIOS_SPEW, "%p: want %08lx got %08lx\n", l, l[i], 0xffffffff - i);
			bad++;
		}
	}

	printk(BIOS_SPEW, "%d errors\n", bad);

	return bad;
}
#else
#define primitive_mem_test()
#endif

#define SIMPLE_SPI_TEST 0
#if SIMPLE_SPI_TEST
/* here is a simple SPI debug test, known to fid trouble */
static void simple_spi_test(void)
{
	struct cbfs_media default_media, *media;
	int i, amt = 4 * MiB, errors = 0;
	//u32 *data = (void *)0x40000000;
	u32 data[1024];
	u32 in;

	amt = sizeof(data);
	media = &default_media;
	if (init_default_cbfs_media(media) != 0) {
		printk(BIOS_SPEW, "Failed to initialize default media.\n");
		return;
	}


	media->open(media);
	if (media->read(media, data, (size_t) 0, amt) < amt){
		printk(BIOS_SPEW, "simple_spi_test fails\n");
		return;
	}


	for(i = 0; i < amt; i += 4){
		if (media->read(media, &in, (size_t) i, 4) < 1){
			printk(BIOS_SPEW, "simple_spi_test fails at %d\n", i);
			return;
		}
		if (data[i/4] != in){
		  errors++;
			printk(BIOS_SPEW, "BAD at %d(%p):\nRAM %08lx\nSPI %08lx\n",
			       i, &data[i/4], (unsigned long)data[i/4], (unsigned long)in);
			/* reread it to see which is wrong. */
			if (media->read(media, &in, (size_t) i, 4) < 1){
				printk(BIOS_SPEW, "simple_spi_test fails at %d\n", i);
				return;
			}
			printk(BIOS_SPEW, "RTRY at %d(%p):\nRAM %08lx\nSPI %08lx\n",
			       i, &data[i/4], (unsigned long)data[i/4], (unsigned long)in);
		}

	}
	printk(BIOS_SPEW, "%d errors\n", errors);
}
#else
#define simple_spi_test()
#endif

void main(void)
{

	extern struct mem_timings mem_timings;
	void *entry;
	int is_resume = (get_wakeup_state() != IS_NOT_WAKEUP);

	/* Clock must be initialized before console_init, otherwise you may need
	 * to re-initialize serial console drivers again. */
	system_clock_init();

	console_init();

	setup_power(is_resume);
	setup_memory(&mem_timings, is_resume);

	primitive_mem_test();

	if (is_resume) {
		wakeup();
	}

	setup_storage();
	setup_gpio();
	setup_ec();

	simple_spi_test();
	/* Set SPI (primary CBFS media) clock to 50MHz. */
	/* if this is uncommented SPI will not work correctly. */
	clock_set_rate(PERIPH_ID_SPI1, 50000000);
	simple_spi_test();
	entry = cbfs_load_stage(CBFS_DEFAULT_MEDIA, "fallback/coreboot_ram");
	simple_spi_test();
	stage_exit(entry);
}
