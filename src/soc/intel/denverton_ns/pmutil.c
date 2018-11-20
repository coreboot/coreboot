/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2013 Google Inc.
 * Copyright (C) 2014 - 2017 Intel Corporation.
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
 */

#include <stdint.h>
#include <arch/io.h>
#include <console/console.h>

#include <intelblocks/pmclib.h>
#include <soc/iomap.h>
#include <soc/soc_util.h>
#include <soc/pm.h>

const char *const *soc_smi_sts_array(size_t *a)
{
	static const char *const smi_sts_bits[] = {
		[2] = "BIOS",
		[4] = "SLP_SMI",
		[5] = "APM",
		[6] = "SWSMI_TMR",
		[8] = "PM1",
		[9] = "GPE0",
		[10] = "GPE1",
		[11] = "MC_SMI",
		[12] = "DEVMON",
		[13] = "TCO",
		[14] = "PERIODIC",
		[15] = "SERIRQ",
		[16] = "SMBUS_SMI",
		[17] = "LEGACY_USB2",
		[18] = "INTEL_USB2",
		[19] = "PATCH",
		[20] = "PCI_EXP_SMI",
		[21] = "MONITOR",
		[26] = "SPI",
		[27] = "GPIO_UNLOCK",
		[31] = "LEGACY_USB3",
	};

	*a = ARRAY_SIZE(smi_sts_bits);
	return smi_sts_bits;
}

const char *const *soc_tco_sts_array(size_t *a)
{
	static const char *const tco_sts_bits[] = {
		[0] = "NMI2SMI",     [1] = "OS_TCO_SMI",
		[2] = "TCO_INIT",    [3] = "TIMEOUT",
		[7] = "NEWCENTURY ", [8] = "BIOSWR ",
		[9] = "CPUSCI ",     [10] = "CPUSMI ",
		[12] = "CPUSERR ",   [16] = "INTRD_DET ",
		[17] = "SECOND_TO",  [20] = "SMLINK_SLV_SMI",
	};

	*a = ARRAY_SIZE(tco_sts_bits);
	return tco_sts_bits;
}

uint32_t soc_reset_tco_status(void)
{
	uint16_t tcobase = get_tcobase();
	uint32_t tco_sts = inl((uint16_t)(tcobase + TCO1_STS));
	uint32_t tco_en = inl((uint16_t)(tcobase + TCO1_CNT));

	outl(tco_sts, (uint16_t)(tcobase + TCO1_STS));
	return tco_sts & tco_en;
}

const char *const *soc_std_gpe_sts_array(size_t *a)
{
	static const char *const gpe_sts_bits[] = {
		[0] = "GPIO_0", [1] = "GPIO_1",
		[2] = "GPIO_2", [3] = "GPIO_3",
		[4] = "GPIO_4", [5] = "GPIO_5",
		[6] = "GPIO_6", [7] = "GPIO_7",
		[8] = "GPIO_8", [9] = "GPIO_9",
		[10] = "GPIO_10", [11] = "GPIO_11",
		[12] = "GPIO_12", [13] = "GPIO_13",
		[14] = "GPIO_14", [15] = "GPIO_15",
		[16] = "GPIO_16", [17] = "GPIO_17",
		[18] = "GPIO_18", [19] = "GPIO_19",
		[20] = "GPIO_20", [21] = "GPIO_21",
		[22] = "GPIO_22", [23] = "GPIO_23",
		[24] = "GPIO_24", [25] = "GPIO_25",
		[26] = "GPIO_26", [27] = "GPIO_27",
		[28] = "GPIO_28", [29] = "GPIO_29",
		[30] = "GPIO_30", [31] = "GPIO_31",
	};

	*a = ARRAY_SIZE(gpe_sts_bits);
	return gpe_sts_bits;
}

void clear_pmc_status(void) { /* TODO */ }
