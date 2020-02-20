/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2018-2020 Intel Corp.
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
#include <device/pci_def.h>
#include <intelblocks/cse.h>
#include <intelblocks/p2sb.h>
#include <intelblocks/pcr.h>
#include <intelblocks/smihandler.h>
#include <soc/soc_chip.h>
#include <soc/pci_devs.h>
#include <soc/pcr_ids.h>
#include <soc/pm.h>

#define CSME0_FBE	0xf
#define CSME0_BAR	0x0
#define CSME0_FID	0xb0

static void pch_disable_heci(void)
{
	struct pcr_sbi_msg msg = {
		.pid = PID_CSME0,
		.offset = 0,
		.opcode = PCR_WRITE,
		.is_posted = false,
		.fast_byte_enable = CSME0_FBE,
		.bar = CSME0_BAR,
		.fid = CSME0_FID
	};
	/* Bit 0: Set to make HECI#1 Function disable */
	uint32_t data32 = 1;
	uint8_t response;
	int status;

	/* unhide p2sb device */
	p2sb_unhide();

	/* Send SBI command to make HECI#1 function disable */
	status = pcr_execute_sideband_msg(&msg, &data32, &response);
	if (status && response)
		printk(BIOS_ERR, "Fail to make CSME function disable\n");

	/* Ensure to Lock SBI interface after this command */
	p2sb_disable_sideband_access();

	/* hide p2sb device */
	p2sb_hide();
}

/*
 * Specific SOC SMI handler during ramstage finalize phase
 *
 * BIOS can't make CSME function disable as is due to POSTBOOT_SAI
 * restriction in place from ICP chipset. Hence create SMI Handler to
 * perform CSME function disabling logic during SMM mode.
 */
void smihandler_soc_at_finalize(void)
{
	const struct soc_intel_icelake_config *config;

	config = config_of_soc();

	if (!config->HeciEnabled && CONFIG(HECI_DISABLE_USING_SMM))
		pch_disable_heci();
}

const smi_handler_t southbridge_smi[SMI_STS_BITS] = {
	[SMI_ON_SLP_EN_STS_BIT] = smihandler_southbridge_sleep,
	[APM_STS_BIT] = smihandler_southbridge_apmc,
	[PM1_STS_BIT] = smihandler_southbridge_pm1,
	[GPE0_STS_BIT] = smihandler_southbridge_gpe0,
	[GPIO_STS_BIT] = smihandler_southbridge_gpi,
	[ESPI_SMI_STS_BIT] = smihandler_southbridge_espi,
	[MCSMI_STS_BIT] = smihandler_southbridge_mc,
	[TCO_STS_BIT] = smihandler_southbridge_tco,
	[PERIODIC_STS_BIT] = smihandler_southbridge_periodic,
	[MONITOR_STS_BIT] = smihandler_southbridge_monitor,
};
