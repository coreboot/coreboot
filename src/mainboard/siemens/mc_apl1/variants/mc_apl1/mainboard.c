/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2018 Siemens AG
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

#include <bootstate.h>
#include <console/console.h>
#include <device/pci_ids.h>
#include <device/pci_ops.h>
#include <gpio.h>
#include <hwilib.h>
#include <intelblocks/lpc_lib.h>
#include <intelblocks/pcr.h>
#include <soc/pcr_ids.h>
#include <timer.h>
#include <timestamp.h>
#include <baseboard/variants.h>
#include <variant/ptn3460.h>

#define TX_DWORD3	0xa8c

void variant_mainboard_final(void)
{
	int status;
	struct device *dev = NULL;

	/*
	 * Set up the DP2LVDS converter.
	 * ptn3460_init() may only be executed after i2c bus init.
	 */
	status = ptn3460_init("hwinfo.hex");
	if (status)
		printk(BIOS_ERR, "LCD: Set up PTN with status 0x%x\n", status);
	else
		printk(BIOS_INFO, "LCD: Set up PTN was successful.\n");

	/*
	 * PIR6 register mapping for PCIe root ports
	 * INTA#->PIRQB#, INTB#->PIRQC#, INTC#->PIRQD#, INTD#-> PIRQA#
	 */
	pcr_write16(PID_ITSS, 0x314c, 0x0321);

	/* Disable clock outputs 1-5 (CLKOUT) for XIO2001 PCIe to PCI Bridge. */
	dev = dev_find_device(PCI_VENDOR_ID_TI, PCI_DEVICE_ID_TI_XIO2001, 0);
	if (dev)
		pci_write_config8(dev, 0xd8, 0x3e);

	/* Enable CLKRUN_EN for power gating LPC */
	lpc_enable_pci_clk_cntl();

	/*
	 * Enable LPC PCE (Power Control Enable) by setting IOSF-SB port 0xD2
	 * offset 0x341D bit3 and bit0.
	 * Enable LPC CCE (Clock Control Enable) by setting IOSF-SB port 0xD2
	 * offset 0x341C bit [3:0].
	 */
	pcr_or32(PID_LPC, PCR_LPC_PRC, (PCR_LPC_CCE_EN | PCR_LPC_PCE_EN));

	/*
	 * Correct the SATA transmit signal via the High Speed I/O Transmit
	 * Control Register 3.
	 * Bit [23:16] set the output voltage swing for TX line.
	 * The value 0x4a sets the swing level to 0.58 V.
	 */
	pcr_rmw32(PID_MODPHY, TX_DWORD3, (0x00 << 16), (0x4a << 16));
}

static void wait_for_legacy_dev(void *unused)
{
	uint32_t legacy_delay, us_since_boot;
	struct stopwatch sw;

	/* Open main hwinfo block. */
	if (hwilib_find_blocks("hwinfo.hex") != CB_SUCCESS)
		return;

	/* Get legacy delay parameter from hwinfo. */
	if (hwilib_get_field(LegacyDelay, (uint8_t *) &legacy_delay,
			      sizeof(legacy_delay)) != sizeof(legacy_delay))
		return;

	us_since_boot = get_us_since_boot();
	/* No need to wait if the time since boot is already long enough.*/
	if (us_since_boot > legacy_delay)
		return;
	stopwatch_init_msecs_expire(&sw, (legacy_delay - us_since_boot) / 1000);
	printk(BIOS_NOTICE, "Wait remaining %d of %d us for legacy devices...",
			legacy_delay - us_since_boot, legacy_delay);
	stopwatch_wait_until_expired(&sw);
	printk(BIOS_NOTICE, "done!\n");
}

static void finalize_boot(void *unused)
{
	/* Set coreboot ready LED. */
	gpio_output(CNV_RGI_DT, 1);
}

BOOT_STATE_INIT_ENTRY(BS_DEV_ENUMERATE, BS_ON_ENTRY, wait_for_legacy_dev, NULL);
BOOT_STATE_INIT_ENTRY(BS_PAYLOAD_BOOT, BS_ON_ENTRY, finalize_boot, NULL);
