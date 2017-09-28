/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2017 Advanced Micro Devices, Inc.
 * Copyright (C) 2014 Alexandru Gagniuc <mr.nuke.me@gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License, or (at your
 * option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <console/console.h>
#include <cpu/x86/smm.h>
#include <delay.h>
#include <soc/smi.h>
#include <soc/southbridge.h>

static void sb_apmc_smi_handler(void)
{
	u32 reg32;
	const uint8_t cmd = inb(pm_acpi_smi_cmd_port());

	switch (cmd) {
	case APM_CNT_ACPI_ENABLE:
		reg32 = inl(ACPI_PM1_CNT_BLK);
		reg32 |= (1 << 0);	/* SCI_EN */
		outl(reg32, ACPI_PM1_CNT_BLK);
		break;
	case APM_CNT_ACPI_DISABLE:
		reg32 = inl(ACPI_PM1_CNT_BLK);
		reg32 &= ~(1 << 0);	/* clear SCI_EN */
		outl(ACPI_PM1_CNT_BLK, reg32);
		break;
	}

	mainboard_smi_apmc(cmd);
}

int southbridge_io_trap_handler(int smif)
{
	return 0;
}

/*
 * Table of functions supported in the SMI handler.  Note that SMI source setup
 * in southbridge.c is unrelated to this list.
 */
struct smi_sources_t smi_sources[] = {
	{ .type = SMITYPE_SMI_CMD_PORT, .handler = sb_apmc_smi_handler },
};

static void process_smi_sci(void)
{
	const uint32_t status = smi_read32(SMI_SCI_STATUS);

	/* Clear events to prevent re-entering SMI if event isn't handled */
	smi_write32(SMI_SCI_STATUS, status);
}

static void *get_source_handler(int source)
{
	int i;

	for (i = 0 ; i < ARRAY_SIZE(smi_sources) ; i++)
		if (smi_sources[i].type == source)
			return smi_sources[i].handler;

	return NULL;
}

static void process_smi_sources(uint32_t reg)
{
	const uint32_t status = smi_read32(reg);
	int bit_zero = 32 / sizeof(uint32_t) * (reg - SMI_REG_SMISTS0);
	void (*source_handler)(void);
	int i;

	for (i = 0 ; i < 32 ; i++) {
		if (status & (1 << i)) {
			source_handler = get_source_handler(i + bit_zero);
			if (source_handler)
				source_handler();
		}
	}

	if (reg == SMI_REG_SMISTS0)
		if (status & GEVENT_MASK)
			/* Gevent[23:0] are assumed to be mainboard-specific */
			mainboard_smi_gpi(status & GEVENT_MASK);

	/* Clear all events in this register */
	smi_write32(reg, status);
}

void southbridge_smi_handler(void)
{
	const uint16_t smi_src = smi_read16(SMI_REG_POINTER);

	if (smi_src & SMI_STATUS_SRC_SCI)
		process_smi_sci();
	if (smi_src & SMI_STATUS_SRC_0)
		process_smi_sources(SMI_REG_SMISTS0);
	if (smi_src & SMI_STATUS_SRC_1)
		process_smi_sources(SMI_REG_SMISTS1);
	if (smi_src & SMI_STATUS_SRC_2)
		process_smi_sources(SMI_REG_SMISTS2);
	if (smi_src & SMI_STATUS_SRC_3)
		process_smi_sources(SMI_REG_SMISTS3);
	if (smi_src & SMI_STATUS_SRC_4)
		process_smi_sources(SMI_REG_SMISTS4);
}

void southbridge_smi_set_eos(void)
{
	uint32_t reg = smi_read32(SMI_REG_SMITRIG0);
	reg |= SMITRG0_EOS;
	smi_write32(SMI_REG_SMITRIG0, reg);
}
