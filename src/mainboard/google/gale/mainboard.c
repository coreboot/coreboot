/*
 * This file is part of the coreboot project.
 *
 * Copyright (c) 2015, The Linux Foundation. All rights reserved.
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
 */

#include <boardid.h>
#include <boot/coreboot_tables.h>
#include <delay.h>
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

#define TPM_RESET_GPIO		19
void ipq_setup_tpm(void)
{
	if (!IS_ENABLED(CONFIG_I2C_TPM))
		return;

	gpio_tlmm_config_set(TPM_RESET_GPIO, FUNC_SEL_GPIO,
			     GPIO_PULL_UP, GPIO_6MA, 1);
	gpio_set(TPM_RESET_GPIO, 0);
	udelay(100);
	gpio_set(TPM_RESET_GPIO, 1);

	/*
	 * ----- Per the SLB 9615XQ1.2 spec -----
	 *
	 * 4.7.1 Reset Timing
	 *
	 * The TPM_ACCESS_x.tpmEstablishment bit has the correct value
	 * and the TPM_ACCESS_x.tpmRegValidSts bit is typically set
	 * within 8ms after RESET# is deasserted.
	 *
	 * The TPM is ready to receive a command after less than 30 ms.
	 *
	 * --------------------------------------
	 *
	 * I'm assuming this means "wait for 30ms"
	 *
	 * If we don't wait here, subsequent QUP I2C accesses
	 * to the TPM either fail or timeout.
	 */
	mdelay(30);
}

static void mainboard_init(device_t dev)
{
	/* disable mmu and d-cache before setting up secure world.*/
	dcache_mmu_disable();
	start_tzbsp();
	/* Setup mmu and d-cache again as non secure entries. */
	setup_mmu(DRAM_INITIALIZED);
	setup_usb();
	ipq_setup_tpm();

	if (IS_ENABLED(CONFIG_CHROMEOS)) {
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

static void mainboard_enable(device_t dev)
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
	dma->tag = LB_TAB_DMA;
	dma->size = sizeof(*dma);
	dma->range_start = (uintptr_t)_dma_coherent;
	dma->range_size = _dma_coherent_size;

	if (IS_ENABLED(CONFIG_CHROMEOS)) {
		/* Retrieve the switch interface MAC addressses. */
		lb_table_add_macs_from_vpd(header);
	}
}
