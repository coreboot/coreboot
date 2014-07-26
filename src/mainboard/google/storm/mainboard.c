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
#include <boot/coreboot_tables.h>
#include <device/device.h>
#include <soc/qualcomm/ipq806x/include/clock.h>
#include <soc/qualcomm/ipq806x/include/gpio.h>
#include <soc/qualcomm/ipq806x/include/usb.h>

/* convenient shorthand (in MB) */
#define DRAM_START           (CONFIG_SYS_SDRAM_BASE / MiB)
#define DRAM_SIZE            (CONFIG_DRAM_SIZE_MB)
#define DRAM_END             (DRAM_START + DRAM_SIZE)

/* DMA memory for drivers */
#define DMA_START            (CONFIG_DRAM_DMA_START / MiB)
#define DMA_SIZE             (CONFIG_DRAM_DMA_SIZE / MiB)

#define USB_ENABLE_GPIO		51

static void setup_usb(void)
{
#if !CONFIG_BOARD_VARIANT_AP148
	gpio_tlmm_config_set(USB_ENABLE_GPIO, FUNC_SEL_GPIO,
			     GPIO_PULL_UP, GPIO_10MA, GPIO_ENABLE);
	gpio_set_out_value(USB_ENABLE_GPIO, 1);
#endif
	usb_clock_config();

	setup_usb_host1();
}

static void setup_mmu(void)
{
	dcache_mmu_disable();

	/* Map Device memory. */
	mmu_config_range(0, DRAM_START, DCACHE_OFF);
	/* Disable Page 0 for trapping NULL pointer references. */
	mmu_disable_range(0, 1);
	/* Map DRAM memory */
	mmu_config_range(DRAM_START, DRAM_SIZE, DCACHE_WRITEBACK);
	/* Map DMA memory */
	mmu_config_range(DMA_START, DMA_SIZE, DCACHE_OFF);

	mmu_disable_range(DRAM_END, 4096 - DRAM_END);

	mmu_init();

	dcache_mmu_enable();
}

static void mainboard_init(device_t dev)
{
	 setup_mmu();
	 setup_usb();
}

static void mainboard_enable(device_t dev)
{
	dev->ops->init = &mainboard_init;
}

struct chip_operations mainboard_ops = {
	.name	= "storm",
	.enable_dev = mainboard_enable,
};

void lb_board(struct lb_header *header)
{
	struct lb_range *dma;

	dma = (struct lb_range *)lb_new_record(header);
	dma->tag = LB_TAB_DMA;
	dma->size = sizeof(*dma);
	dma->range_start = CONFIG_DRAM_DMA_START;
	dma->range_size = CONFIG_DRAM_DMA_SIZE;
}
