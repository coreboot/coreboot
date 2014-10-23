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
#include <boardid.h>
#include <boot/coreboot_tables.h>
#include <console/console.h>
#include <delay.h>
#include <device/device.h>
#include <gpio.h>
#include <soc/clock.h>
#include <soc/usb.h>
#include <string.h>
#include <symbols.h>

#include <vendorcode/google/chromeos/chromeos.h>

/* convenient shorthand (in MB) */
#define DRAM_START           ((uintptr_t)_dram / MiB)
#define DRAM_SIZE            (CONFIG_DRAM_SIZE_MB)
#define DRAM_END             (DRAM_START + DRAM_SIZE)

/* DMA memory for drivers */
#define DMA_START            ((uintptr_t)_dma_coherent / MiB)
#define DMA_SIZE             (_dma_coherent_size / MiB)

#define USB_ENABLE_GPIO		51

static void setup_usb(void)
{
#if !CONFIG_BOARD_VARIANT_AP148
	gpio_tlmm_config_set(USB_ENABLE_GPIO, FUNC_SEL_GPIO,
			     GPIO_PULL_UP, GPIO_10MA, GPIO_ENABLE);
	gpio_set(USB_ENABLE_GPIO, 1);
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

#define TPM_RESET_GPIO 22
static void setup_tpm(void)
{
	if (board_id() != BOARD_ID_PROTO_0)
		return; /* Only proto0 have TPM reset connected to GPIO22 */

	gpio_tlmm_config_set(TPM_RESET_GPIO, FUNC_SEL_GPIO, GPIO_PULL_UP,
			     GPIO_4MA, GPIO_ENABLE);
	/*
	 * Generate a reset pulse. The spec calls for 80 us minimum, let's
	 * make it twice as long. If the output was driven low originally, the
	 * reset pulse will be even longer.
	 */
	gpio_set(TPM_RESET_GPIO, 0);
	udelay(160);
	gpio_set(TPM_RESET_GPIO, 1);
}

#define SW_RESET_GPIO 26
static void deassert_sw_reset(void)
{
	if (board_id() == BOARD_ID_PROTO_0)
		return;

	/*
	 * only proto0.2 and later care about this. This signal is eventually
	 * driving the ehernet switch reset input, which is active low. But
	 * since this signal gets inverted along the way, the GPIO needs to be
	 * driven low to take the switch out of reset.
	 */
	gpio_tlmm_config_set(SW_RESET_GPIO, FUNC_SEL_GPIO,
			     GPIO_PULL_UP, GPIO_4MA, GPIO_ENABLE);

	gpio_set(SW_RESET_GPIO, 0);
}

static void mainboard_init(device_t dev)
{
	 setup_mmu();
	 setup_usb();
	 deassert_sw_reset();
	 setup_tpm();
	 /* Functionally a 0-cost no-op if NAND is not present */
	 board_nand_init();

#if IS_ENABLED(CONFIG_CHROMEOS)
	/* Copy WIFI calibration data into CBMEM. */
	cbmem_add_vpd_calibration_data();
#endif
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
	dma->range_start = (uintptr_t)_dma_coherent;
	dma->range_size = _dma_coherent_size;

#if IS_ENABLED(CONFIG_CHROMEOS)
	/* Retrieve the switch interface MAC addressses. */
	lb_table_add_macs_from_vpd(header);
#endif
}

static int read_gpio(gpio_t gpio_num)
{
	gpio_tlmm_config_set(gpio_num, GPIO_FUNC_DISABLE,
			     GPIO_NO_PULL, GPIO_2MA, GPIO_DISABLE);
	udelay(10); /* Should be enough to settle. */
	return gpio_get(gpio_num);
}

void fill_lb_gpios(struct lb_gpios *gpios)
{
	struct lb_gpio *gpio;
	const int GPIO_COUNT = 5;

	gpios->size = sizeof(*gpios) + (GPIO_COUNT * sizeof(struct lb_gpio));
	gpios->count = GPIO_COUNT;

	gpio = gpios->gpios;
	fill_lb_gpio(gpio++, 15, ACTIVE_LOW, "developer", read_gpio(15));
	fill_lb_gpio(gpio++, 16, ACTIVE_LOW, "recovery", read_gpio(16));
	fill_lb_gpio(gpio++, 17, ACTIVE_LOW, "write protect", read_gpio(17));
	fill_lb_gpio(gpio++, -1, ACTIVE_LOW, "power", 1);
	fill_lb_gpio(gpio++, -1, ACTIVE_LOW, "lid", 0);
}
