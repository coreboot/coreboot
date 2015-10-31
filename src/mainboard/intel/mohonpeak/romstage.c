/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2007-2010 coresystems GmbH
 * Copyright (C) 2011 The ChromiumOS Authors.  All rights reserved.
 * Copyright (C) 2013-2014 Sage Electronic Engineering, LLC.
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

#include <arch/io.h>
#include <device/pci_def.h>
#include <device/pnp_def.h>
#include <cpu/x86/lapic.h>
#include <drivers/intel/fsp1_0/fsp_util.h>
#include <southbridge/intel/fsp_rangeley/soc.h>
#include <southbridge/intel/fsp_rangeley/gpio.h>
#include <southbridge/intel/fsp_rangeley/romstage.h>
#include <arch/cpu.h>
#include "gpio.h"

static void interrupt_routing_config(void)
{
	u8 *ilb_base = (u8 *)(pci_read_config32(SOC_LPC_DEV, IBASE) & ~0xf);

	/*
	* Initialize Interrupt Routings for each device in ilb_base_address.
	* IR01 map to PCIe device 0x01 ... IR31 to device 0x1F.
	* PIRQ_A maps to IRQ 16 ... PIRQ_H maps tp IRQ 23.
	* This should match devicetree and the ACPI IRQ routing/
	*/
	write32(ilb_base + ILB_ACTL, 0x0000);  /* ACTL bit 2:0 SCIS IRQ9 */
	write16(ilb_base + ILB_IR01, 0x3210);  /* IR01h IR(ABCD) - PIRQ(ABCD) */
	write16(ilb_base + ILB_IR02, 0x3210);  /* IR02h IR(ABCD) - PIRQ(ABCD) */
	write16(ilb_base + ILB_IR03, 0x7654);  /* IR03h IR(ABCD) - PIRQ(EFGH) */
	write16(ilb_base + ILB_IR04, 0x7654);  /* IR04h IR(ABCD) - PIRQ(EFGH) */
	write16(ilb_base + ILB_IR20, 0x7654);  /* IR14h IR(ABCD) - PIRQ(EFGH) */
	write16(ilb_base + ILB_IR22, 0x0007);  /* IR16h IR(A) - PIRQ(H) */
	write16(ilb_base + ILB_IR23, 0x0003);  /* IR17h IR(A) - PIRQ(D) */
	write16(ilb_base + ILB_IR24, 0x0003);  /* IR18h IR(A) - PIRQ(D) */
	write16(ilb_base + ILB_IR31, 0x0020);  /* IR1Fh IR(B) - PIRQ(C) */
}

/**
 * /brief mainboard call for setup that needs to be done before fsp init
 *
 */
void early_mainboard_romstage_entry(void)
{
	setup_soc_gpios(&gpio_map);
}

/**
 * /brief mainboard call for setup that needs to be done after fsp init
 *
 */
void late_mainboard_romstage_entry(void)
{
	interrupt_routing_config();
}

/**
 * Get function disables - most of these will be done automatically
 * @param mask pointer to the function-disable bitfield
 */
void get_func_disables(uint32_t *mask)
{

}

void romstage_fsp_rt_buffer_callback(FSP_INIT_RT_BUFFER *FspRtBuffer)
{
	/* No overrides needed */
	return;
}
