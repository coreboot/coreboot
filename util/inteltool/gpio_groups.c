/*
 * inteltool - dump all registers on an Intel CPU + chipset based system.
 *
 * Copyright (C) 2017 secunet Security Networks AG
 * Copyright (C) 2019 YADRO
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

#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdint.h>
#include <assert.h>
#include <inttypes.h>
#include "inteltool.h"
#include "pcr.h"

#include "gpio_names/apollolake.h"
#include "gpio_names/cannonlake.h"
#include "gpio_names/icelake.h"
#include "gpio_names/lewisburg.h"
#include "gpio_names/sunrise.h"

#define SBBAR_SIZE	(16 * MiB)
#define PCR_PORT_SIZE	(64 * KiB)

static const char *const denverton_group_north_all_names[] = {
	"NORTH_ALL_GBE0_SDP0",
	"NORTH_ALL_GBE1_SDP0",
	"NORTH_ALL_GBE0_SDP1",
	"NORTH_ALL_GBE1_SDP1",
	"NORTH_ALL_GBE0_SDP2",
	"NORTH_ALL_GBE1_SDP2",
	"NORTH_ALL_GBE0_SDP3",
	"NORTH_ALL_GBE1_SDP3",
	"NORTH_ALL_GBE2_LED0",
	"NORTH_ALL_GBE2_LED1",
	"NORTH_ALL_GBE0_I2C_CLK",
	"NORTH_ALL_GBE0_I2C_DATA",
	"NORTH_ALL_GBE1_I2C_CLK",
	"NORTH_ALL_GBE1_I2C_DATA",
	"NORTH_ALL_NCSI_RXD0",
	"NORTH_ALL_NCSI_CLK_IN",
	"NORTH_ALL_NCSI_RXD1",
	"NORTH_ALL_NCSI_CRS_DV",
	"NORTH_ALL_NCSI_ARB_IN",
	"NORTH_ALL_NCSI_TX_EN",
	"NORTH_ALL_NCSI_TXD0",
	"NORTH_ALL_NCSI_TXD1",
	"NORTH_ALL_NCSI_ARB_OUT",
	"NORTH_ALL_GBE0_LED0",
	"NORTH_ALL_GBE0_LED1",
	"NORTH_ALL_GBE1_LED0",
	"NORTH_ALL_GBE1_LED1",
	"NORTH_ALL_GPIO_0",
	"NORTH_ALL_PCIE_CLKREQ0_N",
	"NORTH_ALL_PCIE_CLKREQ1_N",
	"NORTH_ALL_PCIE_CLKREQ2_N",
	"NORTH_ALL_PCIE_CLKREQ3_N",
	"NORTH_ALL_PCIE_CLKREQ4_N",
	"NORTH_ALL_GPIO_1",
	"NORTH_ALL_GPIO_2",
	"NORTH_ALL_SVID_ALERT_N",
	"NORTH_ALL_SVID_DATA",
	"NORTH_ALL_SVID_CLK",
	"NORTH_ALL_THERMTRIP_N",
	"NORTH_ALL_PROCHOT_N",
	"NORTH_ALL_MEMHOT_N",
};

static const struct gpio_group denverton_group_north_all = {
	.display	= "------- GPIO Group North All -------",
	.pad_count	= ARRAY_SIZE(denverton_group_north_all_names) / 1,
	.func_count	= 1,
	.pad_names	= denverton_group_north_all_names,
};

static const struct gpio_group *const denverton_community_north_groups[] = {
	&denverton_group_north_all,
};

static const struct gpio_community denverton_community_north = {
	.name		= "------- GPIO Community 0 -------",
	.pcr_port_id	= 0xc2,
	.group_count	= ARRAY_SIZE(denverton_community_north_groups),
	.groups		= denverton_community_north_groups,
};

static const char *const denverton_group_south_dfx_names[] = {
	"SOUTH_DFX_DFX_PORT_CLK0",
	"SOUTH_DFX_DFX_PORT_CLK1",
	"SOUTH_DFX_DFX_PORT0",
	"SOUTH_DFX_DFX_PORT1",
	"SOUTH_DFX_DFX_PORT2",
	"SOUTH_DFX_DFX_PORT3",
	"SOUTH_DFX_DFX_PORT4",
	"SOUTH_DFX_DFX_PORT5",
	"SOUTH_DFX_DFX_PORT6",
	"SOUTH_DFX_DFX_PORT7",
	"SOUTH_DFX_DFX_PORT8",
	"SOUTH_DFX_DFX_PORT9",
	"SOUTH_DFX_DFX_PORT10",
	"SOUTH_DFX_DFX_PORT11",
	"SOUTH_DFX_DFX_PORT12",
	"SOUTH_DFX_DFX_PORT13",
	"SOUTH_DFX_DFX_PORT14",
	"SOUTH_DFX_DFX_PORT15",
};

static const struct gpio_group denverton_group_south_dfx = {
	.display	= "------- GPIO Group South DFX -------",
	.pad_count	= ARRAY_SIZE(denverton_group_south_dfx_names) / 1,
	.func_count	= 1,
	.pad_names	= denverton_group_south_dfx_names,
};

static const char *const denverton_group_south_group0_names[] = {
	"SOUTH_GROUP0_GPIO_12",
	"SOUTH_GROUP0_SMB5_GBE_ALRT_N",
	"SOUTH_GROUP0_PCIE_CLKREQ5_N",
	"SOUTH_GROUP0_PCIE_CLKREQ6_N",
	"SOUTH_GROUP0_PCIE_CLKREQ7_N",
	"SOUTH_GROUP0_UART0_RXD",
	"SOUTH_GROUP0_UART0_TXD",
	"SOUTH_GROUP0_SMB5_GBE_CLK",
	"SOUTH_GROUP0_SMB5_GBE_DATA",
	"SOUTH_GROUP0_ERROR2_N",
	"SOUTH_GROUP0_ERROR1_N",
	"SOUTH_GROUP0_ERROR0_N",
	"SOUTH_GROUP0_IERR_N",
	"SOUTH_GROUP0_MCERR_N",
	"SOUTH_GROUP0_SMB0_LEG_CLK",
	"SOUTH_GROUP0_SMB0_LEG_DATA",
	"SOUTH_GROUP0_SMB0_LEG_ALRT_N",
	"SOUTH_GROUP0_SMB1_HOST_DATA",
	"SOUTH_GROUP0_SMB1_HOST_CLK",
	"SOUTH_GROUP0_SMB2_PECI_DATA",
	"SOUTH_GROUP0_SMB2_PECI_CLK",
	"SOUTH_GROUP0_SMB4_CSME0_DATA",
	"SOUTH_GROUP0_SMB4_CSME0_CLK",
	"SOUTH_GROUP0_SMB4_CSME0_ALRT_N",
	"SOUTH_GROUP0_USB_OC0_N",
	"SOUTH_GROUP0_FLEX_CLK_SE0",
	"SOUTH_GROUP0_FLEX_CLK_SE1",
	"SOUTH_GROUP0_GPIO_4",
	"SOUTH_GROUP0_GPIO_5",
	"SOUTH_GROUP0_GPIO_6",
	"SOUTH_GROUP0_GPIO_7",
	"SOUTH_GROUP0_SATA0_LED_N",
	"SOUTH_GROUP0_SATA1_LED_N",
	"SOUTH_GROUP0_SATA_PDETECT0",
	"SOUTH_GROUP0_SATA_PDETECT1",
	"SOUTH_GROUP0_SATA0_SDOUT",
	"SOUTH_GROUP0_SATA1_SDOUT",
	"SOUTH_GROUP0_UART1_RXD",
	"SOUTH_GROUP0_UART1_TXD",
	"SOUTH_GROUP0_GPIO_8",
	"SOUTH_GROUP0_GPIO_9",
	"SOUTH_GROUP0_TCK",
	"SOUTH_GROUP0_TRST_N",
	"SOUTH_GROUP0_TMS",
	"SOUTH_GROUP0_TDI",
	"SOUTH_GROUP0_TDO",
	"SOUTH_GROUP0_CX_PRDY_N",
	"SOUTH_GROUP0_CX_PREQ_N",
	"SOUTH_GROUP0_CTBTRIGINOUT",
	"SOUTH_GROUP0_CTBTRIGOUT",
	"SOUTH_GROUP0_DFX_SPARE2",
	"SOUTH_GROUP0_DFX_SPARE3",
	"SOUTH_GROUP0_DFX_SPARE4",
};

static const struct gpio_group denverton_group_south_group0 = {
	.display	= "------- GPIO Group South Group0 -------",
	.pad_count	= ARRAY_SIZE(denverton_group_south_group0_names) / 1,
	.func_count	= 1,
	.pad_names	= denverton_group_south_group0_names,
};

static const char *const denverton_group_south_group1_names[] = {
	"SOUTH_GROUP1_SUSPWRDNACK",
	"SOUTH_GROUP1_PMU_SUSCLK",
	"SOUTH_GROUP1_ADR_TRIGGER",
	"SOUTH_GROUP1_PMU_SLP_S45_N",
	"SOUTH_GROUP1_PMU_SLP_S3_N",
	"SOUTH_GROUP1_PMU_WAKE_N",
	"SOUTH_GROUP1_PMU_PWRBTN_N",
	"SOUTH_GROUP1_PMU_RESETBUTTON_N",
	"SOUTH_GROUP1_PMU_PLTRST_N",
	"SOUTH_GROUP1_SUS_STAT_N",
	"SOUTH_GROUP1_SLP_S0IX_N",
	"SOUTH_GROUP1_SPI_CS0_N",
	"SOUTH_GROUP1_SPI_CS1_N",
	"SOUTH_GROUP1_SPI_MOSI_IO0",
	"SOUTH_GROUP1_SPI_MISO_IO1",
	"SOUTH_GROUP1_SPI_IO2",
	"SOUTH_GROUP1_SPI_IO3",
	"SOUTH_GROUP1_SPI_CLK",
	"SOUTH_GROUP1_SPI_CLK_LOOPBK",
	"SOUTH_GROUP1_ESPI_IO0",
	"SOUTH_GROUP1_ESPI_IO1",
	"SOUTH_GROUP1_ESPI_IO2",
	"SOUTH_GROUP1_ESPI_IO3",
	"SOUTH_GROUP1_ESPI_CS0_N",
	"SOUTH_GROUP1_ESPI_CLK",
	"SOUTH_GROUP1_ESPI_RST_N",
	"SOUTH_GROUP1_ESPI_ALRT0_N",
	"SOUTH_GROUP1_GPIO_10",
	"SOUTH_GROUP1_GPIO_11",
	"SOUTH_GROUP1_ESPI_CLK_LOOPBK",
	"SOUTH_GROUP1_EMMC_CMD",
	"SOUTH_GROUP1_EMMC_STROBE",
	"SOUTH_GROUP1_EMMC_CLK",
	"SOUTH_GROUP1_EMMC_D0",
	"SOUTH_GROUP1_EMMC_D1",
	"SOUTH_GROUP1_EMMC_D2",
	"SOUTH_GROUP1_EMMC_D3",
	"SOUTH_GROUP1_EMMC_D4",
	"SOUTH_GROUP1_EMMC_D5",
	"SOUTH_GROUP1_EMMC_D6",
	"SOUTH_GROUP1_EMMC_D7",
	"SOUTH_GROUP1_GPIO_3",
};

static const struct gpio_group denverton_group_south_group1 = {
	.display	= "------- GPIO Group South Group1 -------",
	.pad_count	= ARRAY_SIZE(denverton_group_south_group1_names) / 1,
	.func_count	= 1,
	.pad_names	= denverton_group_south_group1_names,
};

static const struct gpio_group *const denverton_community_south_groups[] = {
	&denverton_group_south_dfx,
	&denverton_group_south_group0,
	&denverton_group_south_group1,
};

static const struct gpio_community denverton_community_south = {
	.name		= "------- GPIO Community 1 -------",
	.pcr_port_id	= 0xc5,
	.group_count	= ARRAY_SIZE(denverton_community_south_groups),
	.groups		= denverton_community_south_groups,
};

static const struct gpio_community *const denverton_communities[] = {
	&denverton_community_north, &denverton_community_south,
};

static const char *decode_pad_mode(const struct gpio_group *const group,
				   const size_t pad, const uint32_t dw0)
{
	const size_t pad_mode = dw0 >> 10 & 7;
	const char *const pad_name =
		group->pad_names[pad * group->func_count + pad_mode];

	if (!pad_mode)
		return pad_name[0] == '*' ? "*GPIO" : "GPIO";
	else if (pad_mode < group->func_count)
		return group->pad_names[pad * group->func_count + pad_mode];
	else
		return "RESERVED";
}

static void print_gpio_group(const uint8_t pid, size_t pad_cfg,
			     const struct gpio_group *const group,
			     size_t pad_stepping)
{
	size_t p;

	printf("%s\n", group->display);

	for (p = 0; p < group->pad_count; ++p, pad_cfg += pad_stepping) {
		const uint32_t dw0 = read_pcr32(pid, pad_cfg);
		const uint32_t dw1 = read_pcr32(pid, pad_cfg + 4);
		const char *const pad_name =
			group->pad_names[p * group->func_count];

		printf("0x%04zx: 0x%016"PRIx64" %-12s %-20s\n", pad_cfg,
		       (uint64_t)dw1 << 32 | dw0,
		       pad_name[0] == '*' ? &pad_name[1] : pad_name,
		       decode_pad_mode(group, p, dw0));
	}
}

static void print_gpio_community(const struct gpio_community *const community,
				 size_t pad_stepping)
{
	size_t group, pad_count;
	size_t pad_cfg; /* offset in bytes under this communities PCR port */

	printf("%s\n\nPCR Port ID: 0x%06zx\n\n",
	       community->name, (size_t)community->pcr_port_id << 16);

	for (group = 0, pad_count = 0; group < community->group_count; ++group)
		pad_count += community->groups[group]->pad_count;
	assert(pad_count * 8 <= PCR_PORT_SIZE - 0x10);

	pad_cfg = read_pcr32(community->pcr_port_id, 0x0c);
	if (pad_cfg + pad_count * 8 > PCR_PORT_SIZE) {
		fprintf(stderr, "Bad Pad Base Address: 0x%08zx\n", pad_cfg);
		return;
	}

	for (group = 0; group < community->group_count; ++group) {
		print_gpio_group(community->pcr_port_id,
				 pad_cfg, community->groups[group],
				 pad_stepping);
		pad_cfg += community->groups[group]->pad_count * pad_stepping;
	}
}

void print_gpio_groups(struct pci_dev *const sb)
{
	size_t community_count;
	const struct gpio_community *const *communities;
	size_t pad_stepping = 8;

	switch (sb->device_id) {
	case PCI_DEVICE_ID_INTEL_H110:
	case PCI_DEVICE_ID_INTEL_H170:
	case PCI_DEVICE_ID_INTEL_Z170:
	case PCI_DEVICE_ID_INTEL_Q170:
	case PCI_DEVICE_ID_INTEL_Q150:
	case PCI_DEVICE_ID_INTEL_B150:
	case PCI_DEVICE_ID_INTEL_C236:
	case PCI_DEVICE_ID_INTEL_C232:
	case PCI_DEVICE_ID_INTEL_QM170:
	case PCI_DEVICE_ID_INTEL_HM170:
	case PCI_DEVICE_ID_INTEL_CM236:
		community_count = ARRAY_SIZE(sunrise_communities);
		communities = sunrise_communities;
		pcr_init(sb);
		break;
	case PCI_DEVICE_ID_INTEL_SUNRISEPOINT_LP_PRE:
	case PCI_DEVICE_ID_INTEL_SUNRISEPOINT_LP_U_BASE_SKL:
	case PCI_DEVICE_ID_INTEL_SUNRISEPOINT_LP_Y_PREM_SKL:
	case PCI_DEVICE_ID_INTEL_SUNRISEPOINT_LP_U_PREM_SKL:
	case PCI_DEVICE_ID_INTEL_SUNRISEPOINT_LP_U_BASE_KBL:
	case PCI_DEVICE_ID_INTEL_SUNRISEPOINT_LP_U_PREM_KBL:
	case PCI_DEVICE_ID_INTEL_SUNRISEPOINT_LP_Y_PREM_KBL:
	case PCI_DEVICE_ID_INTEL_SUNRISEPOINT_LP_U_IHDCP_BASE:
	case PCI_DEVICE_ID_INTEL_SUNRISEPOINT_LP_U_IHDCP_PREM:
	case PCI_DEVICE_ID_INTEL_SUNRISEPOINT_LP_Y_IHDCP_PREM:
		community_count = ARRAY_SIZE(sunrise_lp_communities);
		communities = sunrise_lp_communities;
		pcr_init(sb);
		break;
	case PCI_DEVICE_ID_INTEL_C621:
	case PCI_DEVICE_ID_INTEL_C622:
	case PCI_DEVICE_ID_INTEL_C624:
	case PCI_DEVICE_ID_INTEL_C625:
	case PCI_DEVICE_ID_INTEL_C626:
	case PCI_DEVICE_ID_INTEL_C627:
	case PCI_DEVICE_ID_INTEL_C628:
	case PCI_DEVICE_ID_INTEL_C629:
	case PCI_DEVICE_ID_INTEL_C624_SUPER:
	case PCI_DEVICE_ID_INTEL_C627_SUPER_1:
	case PCI_DEVICE_ID_INTEL_C621_SUPER:
	case PCI_DEVICE_ID_INTEL_C627_SUPER_2:
	case PCI_DEVICE_ID_INTEL_C628_SUPER:
		community_count = ARRAY_SIZE(lewisburg_communities);
		communities = lewisburg_communities;
		pcr_init(sb);
		break;
	case PCI_DEVICE_ID_INTEL_DNV_LPC:
		community_count = ARRAY_SIZE(denverton_communities);
		communities = denverton_communities;
		pcr_init(sb);
		break;
	case PCI_DEVICE_ID_INTEL_APL_LPC:
		community_count = ARRAY_SIZE(apl_communities);
		communities = apl_communities;
		pcr_init(sb);
		break;
	case PCI_DEVICE_ID_INTEL_H310:
	case PCI_DEVICE_ID_INTEL_H370:
	case PCI_DEVICE_ID_INTEL_Z390:
	case PCI_DEVICE_ID_INTEL_Q370:
	case PCI_DEVICE_ID_INTEL_B360:
	case PCI_DEVICE_ID_INTEL_C246:
	case PCI_DEVICE_ID_INTEL_C242:
	case PCI_DEVICE_ID_INTEL_QM370:
	case PCI_DEVICE_ID_INTEL_HM370:
	case PCI_DEVICE_ID_INTEL_CM246:
		community_count = ARRAY_SIZE(cannonlake_pch_h_communities);
		communities = cannonlake_pch_h_communities;
		pad_stepping = 16;
		pcr_init(sb);
		break;
	case PCI_DEVICE_ID_INTEL_ICELAKE_LP_U:
		community_count = ARRAY_SIZE(icelake_pch_h_communities);
		communities = icelake_pch_h_communities;
		pad_stepping = 16;
		pcr_init(sb);
		break;
	default:
		return;
	}

	printf("\n============= GPIOS =============\n\n");

	for (; community_count; --community_count)
		print_gpio_community(*communities++, pad_stepping);
}
