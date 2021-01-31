/* SPDX-License-Identifier: GPL-2.0-only */

#include <boot/coreboot_tables.h>
#include <device/device.h>
#include <gpio.h>
#include <soc/clock.h>
#include <soc/soc_services.h>
#include <soc/usb.h>
#include <soc/blsp.h>
#include <symbols.h>

#include <vendorcode/google/chromeos/chromeos.h>
#include "mmu.h"

#define USB_ENABLE_GPIO		51

static void setup_usb(void)
{
	usb_clock_config();

	setup_usb_host1();
}

static void mainboard_init(struct device *dev)
{
	/* disable mmu and d-cache before setting up secure world.*/
	dcache_mmu_disable();
	start_tzbsp();
	/* Setup mmu and d-cache again as non secure entries. */
	setup_mmu(DRAM_INITIALIZED);
	setup_usb();

	if (CONFIG(CHROMEOS)) {
		/* Copy WIFI calibration data into CBMEM. */
		cbmem_add_vpd_calibration_data();
	}

	/*
	 * Make sure bootloader can issue sounds The frequency is calculated
	 * as "<frame_rate> * <bit_width> * <channels> * 4", i.e.
	 *
	 * 48000 * 2 * 16 * 4 = 6144000
	 */
	//audio_clock_config(6144000);
}

static void mainboard_enable(struct device *dev)
{
	dev->ops->init = &mainboard_init;
}

struct chip_operations mainboard_ops = {
	.name	= "gale",
	.enable_dev = mainboard_enable,
};

void lb_board(struct lb_header *header)
{
	struct lb_range *dma;

	dma = (struct lb_range *)lb_new_record(header);
	dma->tag = LB_TAG_DMA;
	dma->size = sizeof(*dma);
	dma->range_start = (uintptr_t)_dma_coherent;
	dma->range_size = REGION_SIZE(dma_coherent);

	if (CONFIG(CHROMEOS)) {
		/* Retrieve the switch interface MAC addresses. */
		lb_table_add_macs_from_vpd(header);
	}
}
