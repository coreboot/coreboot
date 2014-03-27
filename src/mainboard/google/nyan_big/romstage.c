/*
 * This file is part of the coreboot project.
 *
 * Copyright 2014 Google Inc.
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

#include <arch/cache.h>
#include <arch/cpu.h>
#include <arch/exception.h>
#include <arch/io.h>
#include <arch/stages.h>
#include <device/device.h>
#include <cbfs.h>
#include <cbmem.h>
#include <console/console.h>
#include "sdram_configs.h"
#include <soc/nvidia/tegra/i2c.h>
#include <soc/nvidia/tegra124/chip.h>
#include <soc/nvidia/tegra124/clk_rst.h>
#include <soc/nvidia/tegra124/sdram.h>
#include <soc/addressmap.h>
#include <soc/clock.h>
#include <soc/display.h>
#include <timestamp.h>

static struct clk_rst_ctlr *clk_rst = (void *)TEGRA_CLK_RST_BASE;

enum {
	L2CTLR_ECC_PARITY = 0x1 << 21,
	L2CTLR_TAG_RAM_LATENCY_MASK = 0x7 << 6,
	L2CTLR_TAG_RAM_LATENCY_CYCLES_3 = 2 << 6,
	L2CTLR_DATA_RAM_LATENCY_MASK = 0x7 << 0,
	L2CTLR_DATA_RAM_LATENCY_CYCLES_3  = 2 << 0
};

enum {
	L2ACTLR_FORCE_L2_LOGIC_CLOCK_ENABLE_ACTIVE = 0x1 << 27,
	L2ACTLR_ENABLE_HAZARD_DETECT_TIMEOUT = 0x1 << 7,
	L2ACTLR_DISABLE_CLEAN_EVICT_PUSH_EXTERNAL = 0x1 << 3
};

/* Configures L2 Control Register to use 3 cycles for DATA/TAG RAM latency. */
static void configure_l2ctlr(void)
{
   uint32_t val;

   val = read_l2ctlr();
   val &= ~(L2CTLR_DATA_RAM_LATENCY_MASK | L2CTLR_TAG_RAM_LATENCY_MASK);
   val |= (L2CTLR_DATA_RAM_LATENCY_CYCLES_3 | L2CTLR_TAG_RAM_LATENCY_CYCLES_3 |
	   L2CTLR_ECC_PARITY);
   write_l2ctlr(val);
}

/* Configures L2 Auxiliary Control Register for Cortex A15. */
static void configure_l2actlr(void)
{
   uint32_t val;

   val = read_l2actlr();
   val |= (L2ACTLR_DISABLE_CLEAN_EVICT_PUSH_EXTERNAL |
	   L2ACTLR_ENABLE_HAZARD_DETECT_TIMEOUT |
	   L2ACTLR_FORCE_L2_LOGIC_CLOCK_ENABLE_ACTIVE);
   write_l2actlr(val);
}

static void setup_pinmux(void)
{
	// Write protect.
	gpio_input_pullup(GPIO(R1));
	// Recovery mode.
	gpio_input_pullup(GPIO(Q7));
	// Lid switch.
	gpio_input_pullup(GPIO(R4));
	// Power switch.
	gpio_input_pullup(GPIO(Q0));
	// Developer mode.
	gpio_input_pullup(GPIO(Q6));
	// EC in RW.
	gpio_input_pullup(GPIO(U4));

	// SOC and TPM reset GPIO, active low.
	gpio_output(GPIO(I5), 1);

	// SPI1 MOSI
	pinmux_set_config(PINMUX_ULPI_CLK_INDEX, PINMUX_ULPI_CLK_FUNC_SPI1 |
						 PINMUX_PULL_NONE |
						 PINMUX_INPUT_ENABLE);
	// SPI1 MISO
	pinmux_set_config(PINMUX_ULPI_DIR_INDEX, PINMUX_ULPI_DIR_FUNC_SPI1 |
						 PINMUX_PULL_NONE |
						 PINMUX_INPUT_ENABLE);
	// SPI1 SCLK
	pinmux_set_config(PINMUX_ULPI_NXT_INDEX, PINMUX_ULPI_NXT_FUNC_SPI1 |
						 PINMUX_PULL_NONE |
						 PINMUX_INPUT_ENABLE);
	// SPI1 CS0
	pinmux_set_config(PINMUX_ULPI_STP_INDEX, PINMUX_ULPI_STP_FUNC_SPI1 |
						 PINMUX_PULL_NONE |
						 PINMUX_INPUT_ENABLE);

	// I2C3 (cam) clock.
	pinmux_set_config(PINMUX_CAM_I2C_SCL_INDEX,
			  PINMUX_CAM_I2C_SCL_FUNC_I2C3 | PINMUX_INPUT_ENABLE);
	// I2C3 (cam) data.
	pinmux_set_config(PINMUX_CAM_I2C_SDA_INDEX,
			  PINMUX_CAM_I2C_SDA_FUNC_I2C3 | PINMUX_INPUT_ENABLE);

	// switch unused pin to GPIO
	gpio_set_mode(GPIO(X3), GPIO_MODE_GPIO);
	gpio_set_mode(GPIO(X4), GPIO_MODE_GPIO);
	gpio_set_mode(GPIO(X5), GPIO_MODE_GPIO);
	gpio_set_mode(GPIO(X6), GPIO_MODE_GPIO);
	gpio_set_mode(GPIO(X7), GPIO_MODE_GPIO);
	gpio_set_mode(GPIO(W3), GPIO_MODE_GPIO);
}

static void configure_ec_spi_bus(void)
{
	clock_configure_source(sbc1, PLLP, 5000);
}

static void configure_tpm_i2c_bus(void)
{
	clock_configure_i2c_scl_freq(i2c3, PLLP, 400);

	i2c_init(2);
}

static void __attribute__((noinline)) romstage(void)
{
#if CONFIG_COLLECT_TIMESTAMPS
	uint64_t romstage_start_time = timestamp_get();
#endif

	configure_l2ctlr();
	configure_l2actlr();

	console_init();
	exception_init();

	sdram_init(get_sdram_config());

	/* used for MMU and CBMEM setup, in MB */
	u32 dram_start = (CONFIG_SYS_SDRAM_BASE >> 20);
	u32 dram_end = sdram_max_addressable_mb();	/* plus one... */
	u32 dram_size = dram_end - dram_start;

	mmu_init();
	/* Device memory below DRAM is uncached. */
	mmu_config_range(0, dram_start, DCACHE_OFF);
	/* SRAM is cached. Round the size up to 2MB, the LPAE page size. */
	mmu_config_range(0x40000000 >> 20, 2, DCACHE_WRITEBACK);
	/* DRAM is cached. */
	mmu_config_range(dram_start, dram_size, DCACHE_WRITEBACK);
	/* A window for DMA is uncached. */
	mmu_config_range(CONFIG_DRAM_DMA_START >> 20,
			 CONFIG_DRAM_DMA_SIZE >> 20, DCACHE_OFF);
	/* The space above DRAM is uncached. */
	if (dram_end < 4096)
		mmu_config_range(dram_end, 4096 - dram_end, DCACHE_OFF);
	mmu_disable_range(0, 1);
	dcache_mmu_enable();

	/* For quality of the user experience, it's important to get
	 * the video going ASAP. Because there are long delays in some
	 * of the powerup steps, we do some very early setup here in
	 * romstage. The only thing setup_display does is manage
	 * 4 GPIOs, under control of the config struct members.
	 * In general, it is safe to enable panel power, and disable
	 * anything related to the backlight. If we get something wrong,
	 * we can easily fix it in ramstage by further GPIO manipulation,
	 * so we feel it is ok to do some setting at this point.
	 */

	const struct device *soc = dev_find_slot(DEVICE_PATH_CPU_CLUSTER, 0);
	printk(BIOS_SPEW, "s%s: soc is %p\n", __func__, soc);
	if (soc && soc->chip_info) {
		const struct soc_nvidia_tegra124_config *config =
			soc->chip_info;
		setup_display((struct soc_nvidia_tegra124_config *)config);
	}

	cbmem_initialize_empty();

#if CONFIG_COLLECT_TIMESTAMPS
	timestamp_init(0);
	timestamp_add(TS_START_ROMSTAGE, romstage_start_time);
	timestamp_add(TS_START_COPYRAM, timestamp_get());
#endif

	// Enable additional peripherals we need for ROM stage.
	clock_enable_clear_reset(0, CLK_H_SBC1, CLK_U_I2C3, 0, 0, 0);

	setup_pinmux();

	configure_ec_spi_bus();
	configure_tpm_i2c_bus();

	void *entry = cbfs_load_stage(CBFS_DEFAULT_MEDIA,
				      "fallback/coreboot_ram");
#if CONFIG_COLLECT_TIMESTAMPS
	timestamp_add(TS_END_COPYRAM, timestamp_get());
#endif
	stage_exit(entry);
}

/* Stub to force arm_init_caches to the top, before any stack/memory accesses */
void main(void)
{
	asm volatile ("bl arm_init_caches"
		      ::: "r0","r1","r2","r3","r4","r5","ip");
	romstage();
}
