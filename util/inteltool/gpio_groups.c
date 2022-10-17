/* inteltool - dump all registers on an Intel CPU + chipset based system */
/* SPDX-License-Identifier: GPL-2.0-only */

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
#include "gpio_names/cannonlake_lp.h"
#include "gpio_names/denverton.h"
#include "gpio_names/geminilake.h"
#include "gpio_names/icelake.h"
#include "gpio_names/lewisburg.h"
#include "gpio_names/sunrise.h"
#include "gpio_names/tigerlake.h"
#include "gpio_names/alderlake_h.h"
#include "gpio_names/alderlake_p.h"
#include "gpio_names/elkhartlake.h"

#define SBBAR_SIZE	(16 * MiB)
#define PCR_PORT_SIZE	(64 * KiB)

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

const struct gpio_community *const *get_gpio_communities(struct pci_dev *const sb,
						size_t* community_count,
						size_t* pad_stepping)
{
	*pad_stepping = 8;

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
	case PCI_DEVICE_ID_INTEL_H270:
	case PCI_DEVICE_ID_INTEL_Z270:
	case PCI_DEVICE_ID_INTEL_Q270:
	case PCI_DEVICE_ID_INTEL_Q250:
	case PCI_DEVICE_ID_INTEL_B250:
	case PCI_DEVICE_ID_INTEL_Z370:
	case PCI_DEVICE_ID_INTEL_H310C:
	case PCI_DEVICE_ID_INTEL_X299:
		*community_count = ARRAY_SIZE(sunrise_communities);
		return sunrise_communities;
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
		*community_count = ARRAY_SIZE(sunrise_lp_communities);
		return sunrise_lp_communities;
	case PCI_DEVICE_ID_INTEL_C621:
	case PCI_DEVICE_ID_INTEL_C622:
	case PCI_DEVICE_ID_INTEL_C624:
	case PCI_DEVICE_ID_INTEL_C625:
	case PCI_DEVICE_ID_INTEL_C626:
	case PCI_DEVICE_ID_INTEL_C627:
	case PCI_DEVICE_ID_INTEL_C628:
	case PCI_DEVICE_ID_INTEL_C629:
	case PCI_DEVICE_ID_INTEL_C621A:
	case PCI_DEVICE_ID_INTEL_C627A:
	case PCI_DEVICE_ID_INTEL_C629A:
	case PCI_DEVICE_ID_INTEL_C624_SUPER:
	case PCI_DEVICE_ID_INTEL_C627_SUPER_1:
	case PCI_DEVICE_ID_INTEL_C621_SUPER:
	case PCI_DEVICE_ID_INTEL_C627_SUPER_2:
	case PCI_DEVICE_ID_INTEL_C628_SUPER:
	case PCI_DEVICE_ID_INTEL_C621A_SUPER:
	case PCI_DEVICE_ID_INTEL_C627A_SUPER:
	case PCI_DEVICE_ID_INTEL_C629A_SUPER:
		*community_count = ARRAY_SIZE(lewisburg_communities);
		return lewisburg_communities;
	case PCI_DEVICE_ID_INTEL_DNV_LPC:
		*community_count = ARRAY_SIZE(denverton_communities);
		return denverton_communities;
	case PCI_DEVICE_ID_INTEL_APL_LPC:
		*community_count = ARRAY_SIZE(apl_communities);
		return apl_communities;
	case PCI_DEVICE_ID_INTEL_GLK_LPC:
		*community_count = ARRAY_SIZE(glk_communities);
		*pad_stepping = 16;
		return glk_communities;
	case PCI_DEVICE_ID_INTEL_CANNONPOINT_LP_U_PREM:
	case PCI_DEVICE_ID_INTEL_COMETPOINT_LP_U_PREM:
	case PCI_DEVICE_ID_INTEL_COMETPOINT_LP_U_BASE:
		*community_count = ARRAY_SIZE(cannonlake_pch_lp_communities);
		*pad_stepping = 16;
		return cannonlake_pch_lp_communities;
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
		*community_count = ARRAY_SIZE(cannonlake_pch_h_communities);
		*pad_stepping = 16;
		return cannonlake_pch_h_communities;
	case PCI_DEVICE_ID_INTEL_ICELAKE_LP_U:
		*community_count = ARRAY_SIZE(icelake_pch_h_communities);
		*pad_stepping = 16;
		return icelake_pch_h_communities;
	case PCI_DEVICE_ID_INTEL_TIGERPOINT_U_SUPER:
	case PCI_DEVICE_ID_INTEL_TIGERPOINT_U_PREM:
	case PCI_DEVICE_ID_INTEL_TIGERPOINT_U_BASE:
	case PCI_DEVICE_ID_INTEL_TIGERPOINT_Y_SUPER:
	case PCI_DEVICE_ID_INTEL_TIGERPOINT_Y_PREM:
		*community_count = ARRAY_SIZE(tigerlake_pch_lp_communities);
		*pad_stepping = 16;
		return tigerlake_pch_lp_communities;
	case PCI_DEVICE_ID_INTEL_Q570:
	case PCI_DEVICE_ID_INTEL_Z590:
	case PCI_DEVICE_ID_INTEL_H570:
	case PCI_DEVICE_ID_INTEL_B560:
	case PCI_DEVICE_ID_INTEL_H510:
	case PCI_DEVICE_ID_INTEL_WM590:
	case PCI_DEVICE_ID_INTEL_QM580:
	case PCI_DEVICE_ID_INTEL_HM570:
	case PCI_DEVICE_ID_INTEL_C252:
	case PCI_DEVICE_ID_INTEL_C256:
	case PCI_DEVICE_ID_INTEL_W580:
		*community_count = ARRAY_SIZE(tigerlake_pch_h_communities);
		*pad_stepping = 16;
		return tigerlake_pch_h_communities;
	case PCI_DEVICE_ID_INTEL_H610:
	case PCI_DEVICE_ID_INTEL_B660:
	case PCI_DEVICE_ID_INTEL_H670:
	case PCI_DEVICE_ID_INTEL_Q670:
	case PCI_DEVICE_ID_INTEL_Z690:
	case PCI_DEVICE_ID_INTEL_W680:
	case PCI_DEVICE_ID_INTEL_W685:
	case PCI_DEVICE_ID_INTEL_WM690:
	case PCI_DEVICE_ID_INTEL_HM670:
	case PCI_DEVICE_ID_INTEL_WM790:
	case PCI_DEVICE_ID_INTEL_HM770:
		*community_count = ARRAY_SIZE(alderlake_pch_h_communities);
		*pad_stepping = 16;
		return alderlake_pch_h_communities;
	case PCI_DEVICE_ID_INTEL_ADL_P:
	case PCI_DEVICE_ID_INTEL_ADL_M:
	case PCI_DEVICE_ID_INTEL_RPL_P:
		*community_count = ARRAY_SIZE(alderlake_pch_p_communities);
		*pad_stepping = 16;
		return alderlake_pch_p_communities;
	case PCI_DEVICE_ID_INTEL_EHL:
		*community_count = ARRAY_SIZE(elkhartlake_pch_communities);
		*pad_stepping = 16;
		return elkhartlake_pch_communities;

	default:
		return NULL;
	}
}

void print_gpio_groups(struct pci_dev *const sb)
{
	size_t community_count;
	const struct gpio_community *const *communities;
	size_t pad_stepping;

	communities = get_gpio_communities(sb, &community_count, &pad_stepping);

	if (!communities)
		return;

	pcr_init(sb);

	printf("\n============= GPIOS =============\n\n");

	for (; community_count; --community_count)
		print_gpio_community(*communities++, pad_stepping);
}
