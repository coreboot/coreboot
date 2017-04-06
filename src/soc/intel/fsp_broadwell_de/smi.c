/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2008-2009 coresystems GmbH
 * Copyright (C) 2014 Google Inc.
 * Copyright (C) 2017 Siemens AG
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

#include <console/console.h>
#include <arch/io.h>
#include <soc/iomap.h>
#include <soc/lpc.h>
#include <soc/smm.h>

void southbridge_smm_clear_state(void)
{
	u32 smi_en;

	printk(BIOS_DEBUG, "Initializing Southbridge SMI...");
	printk(BIOS_SPEW, " ... pmbase = 0x%04x\n", ACPI_BASE_ADDRESS);

	smi_en = inl(ACPI_BASE_ADDRESS + SMI_EN);
	if (smi_en & APMC_EN) {
		printk(BIOS_INFO, "SMI# handler already enabled?\n");
		return;
	}

	printk(BIOS_DEBUG, "\n");

	/* Dump and clear status registers */
	clear_smi_status();
	clear_pm1_status();
}

void southbridge_smm_enable_smi(void)
{
	printk(BIOS_DEBUG, "Enabling SMIs.\n");

	/* Clear all possible set SMI status bits
	   before enabling SMIs. */
	southbridge_clear_smi_status();

	/* Enable SMI generation:
	   - on SERIRQ-SMI (is always enabled) */
	enable_smi(EOS | GBL_SMI_EN);
}

void southbridge_trigger_smi(void)
{
	/*
	 * There are several methods of raising a controlled SMI# via
	 * software, among them:
	 *  - Writes to io 0xb2 (APMC)
	 *  - Writes to the Local Apic ICR with Delivery mode SMI.
	 *
	 * Using the local apic is a bit more tricky. According to
	 * AMD Family 11 Processor BKDG no destination shorthand must be
	 * used.
	 * The whole SMM initialization is quite a bit hardware specific, so
	 * I'm not too worried about the better of the methods at the moment
	 */

	/* Raise an SMI interrupt */
	printk(BIOS_SPEW, "  ... raise SMI#\n");
	outb(0x00, 0xb2);
}

void southbridge_clear_smi_status(void)
{
	/* Clear SMI status */
	clear_smi_status();

	/* Clear PM1 status */
	clear_pm1_status();

	/* Set EOS bit so other SMIs can occur. */
	enable_smi(EOS);
}
