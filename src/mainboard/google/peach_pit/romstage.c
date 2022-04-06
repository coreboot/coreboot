/* SPDX-License-Identifier: GPL-2.0-only */

#include <arch/exception.h>
#include <armv7.h>
#include <boot_device.h>
#include <cbmem.h>
#include <commonlib/region.h>
#include <console/console.h>
#include <device/i2c_simple.h>
#include <drivers/maxim/max77802/max77802.h>
#include <program_loading.h>
#include <romstage_common.h>
#include <soc/clk.h>
#include <soc/cpu.h>
#include <soc/dmc.h>
#include <soc/gpio.h>
#include <soc/i2c.h>
#include <soc/periph.h>
#include <soc/power.h>
#include <soc/setup.h>
#include <soc/trustzone.h>
#include <soc/wakeup.h>
#include <timestamp.h>
#include <types.h>

#define PMIC_I2C_BUS	4

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
	{ 0, MAX77802_REG_PMIC_BUCK2DVS1, MAX77802_BUCK2DVS1_1_2625V },
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
	/* Disable Boost(bypass) OUTPUT */
	{ 0, MAX77802_REG_PMIC_BOOSTCTRL, MAX77802_BOOSTCTRL_OFF},
};

static int setup_power(int is_resume)
{
	int error = 0;
	int i;

	power_init();

	if (is_resume) {
		return 0;
	}

	/* Initialize I2C bus to configure PMIC. */
	exynos_pinmux_i2c4();
	i2c_init(PMIC_I2C_BUS, 1000000, 0x00); /* 1MHz */

	for (i = 0; i < ARRAY_SIZE(pmic_writes); i++) {
		uint8_t data = 0;
		uint8_t reg = pmic_writes[i].reg;

		if (pmic_writes[i].or_orig)
			error |= i2c_readb(4, MAX77802_I2C_ADDR, reg, &data);

		data |= pmic_writes[i].val;
		error |= i2c_writeb(4, MAX77802_I2C_ADDR, reg, data);
	}

	return error;
}

static void setup_ec(void)
{
	/* SPI2 (EC) is slower and needs to work in half-duplex mode with
	 * single byte bus width. */
	clock_set_rate(PERIPH_ID_SPI2, 5000000);
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
	for (i = 0; i < 256*1048576; i++){
		if (! (i%1048576))
			printk(BIOS_SPEW, "%lu ...", i);
		l[i] = 0xffffffff - i;
	}

	for (i = 0; i < 256*1048576; i++){
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
	const struct region_device *boot_dev;
	int i, amt = 4 * MiB, errors = 0;
	//u32 *data = (void *)0x40000000;
	u32 data[1024];
	u32 in;

	boot_device_init();
	boot_dev = boot_device_ro();
	amt = sizeof(data);
	if (boot_dev == NULL) {
		printk(BIOS_SPEW, "Failed to initialize default media.\n");
		return;
	}

	if (rdev_readat(boot_dev, data, 0, amt) < amt) {
		printk(BIOS_SPEW, "simple_spi_test fails\n");
		return;
	}

	for (i = 0; i < amt; i += 4){
		if (rdev_readat(boot_dev, &in, i, 4) < 4) {
			printk(BIOS_SPEW, "simple_spi_test fails at %d\n", i);
			return;
		}
		if (data[i/4] != in){
		  errors++;
			printk(BIOS_SPEW, "BAD at %d(%p):\nRAM %08lx\nSPI %08lx\n",
			       i, &data[i/4], (unsigned long)data[i/4], (unsigned long)in);
			/* reread it to see which is wrong. */
			if (rdev_readat(boot_dev, &in, i, 4) < 4) {
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
	timestamp_init(timestamp_get());
	timestamp_add_now(TS_ROMSTAGE_START);

	/*
	 * From the clocks comment below it looks like serial console won't
	 * work in the bootblock so keep in the romstage_main flow even with
	 * !CONFIG SEPARATE_ROMSTAGE.
	 */
	romstage_main();
}

void __noreturn romstage_main(void)
{

	extern struct mem_timings mem_timings;
	int is_resume = (get_wakeup_state() != IS_NOT_WAKEUP);
	int power_init_failed;

	exynos5420_config_smp();
	power_init_failed = setup_power(is_resume);

	/* Clock must be initialized before console_init, otherwise you may need
	 * to re-initialize serial console drivers again. */
	system_clock_init();

	exynos_pinmux_uart3();
	console_init();
	exception_init();

	if (power_init_failed)
		die("Failed to initialize power.\n");

	/* re-initialize PMIC I2C channel after (re-)setting system clocks */
	i2c_init(PMIC_I2C_BUS, 1000000, 0x00); /* 1MHz */

	timestamp_add_now(TS_INITRAM_START);

	setup_memory(&mem_timings, is_resume);

	timestamp_add_now(TS_INITRAM_END);

	primitive_mem_test();

	trustzone_init();

	if (is_resume) {
		wakeup();
	}

	setup_gpio();
	setup_ec();

	simple_spi_test();
	/* Set SPI (primary CBFS media) clock to 50MHz. */
	/* if this is uncommented SPI will not work correctly. */
	clock_set_rate(PERIPH_ID_SPI1, 50000000);
	exynos_pinmux_spi1();
	simple_spi_test();

	cbmem_initialize_empty();

	simple_spi_test();

	run_ramstage();
}
