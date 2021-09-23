/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <acpi/acpi_device.h>
#include <baseboard/variants.h>
#include <console/console.h>
#include <device/pci_ids.h>
#include <device/pci_ops.h>
#include <soc/gpio_soc_defs.h>
#include <soc/pci_devs.h>
#include <soc/soc_chip.h>
#include <drivers/intel/dptf/chip.h>
#include "board_id.h"

const struct cpu_power_limits limits[] = {
	/* SKU_ID, pl1_min, pl1_max, pl2_min, pl2_max */
	/* PL2 values are for performance configuration */
	{ PCI_DEVICE_ID_INTEL_ADL_P_ID_7, 3000, 15000,  55000,  55000 },
	{ PCI_DEVICE_ID_INTEL_ADL_P_ID_5, 4000, 28000,  64000,  64000 },
	{ PCI_DEVICE_ID_INTEL_ADL_P_ID_3, 5000, 45000, 115000, 115000 },
};

WEAK_DEV_PTR(dptf_policy);
void variant_update_power_limits(void)
{
	const struct device *policy_dev = DEV_PTR(dptf_policy);
	if (!policy_dev)
		return;

	struct drivers_intel_dptf_config *config = config_of(policy_dev);

	uint16_t mchid = pci_s_read_config16(PCI_DEV(0, 0, 0), PCI_DEVICE_ID);

	for (size_t i = 0; i < ARRAY_SIZE(limits); i++) {
		if (mchid == limits[i].mchid) {
			struct dptf_power_limits *settings = &config->controls.power_limits;
			settings->pl1.min_power = limits[i].pl1_min_power;
			settings->pl1.max_power = limits[i].pl1_max_power;
			settings->pl2.min_power = limits[i].pl2_min_power;
			settings->pl2.max_power = limits[i].pl2_max_power;
			printk(BIOS_INFO, "Overriding DPTF power limits PL1 (%u, %u) PL2 (%u, %u)\n",
					limits[i].pl1_min_power,
					limits[i].pl1_max_power,
					limits[i].pl2_min_power,
					limits[i].pl2_max_power);
		}
	}
}

static const struct typec_aux_bias_pads pad_config = { GPP_E23, GPP_E22 };

static const struct board_id_iom_port_config {
	int board_id;
	enum typec_port_index port;
} port_config[] = {
	{ ADL_P_LP4_1,	TYPE_C_PORT_2 },
	{ ADL_P_LP4_2,	TYPE_C_PORT_2 },
	{ ADL_P_DDR4_1,	TYPE_C_PORT_2 },
	{ ADL_P_DDR4_2,	TYPE_C_PORT_2 },
	{ ADL_P_LP5_1,	TYPE_C_PORT_2 },
	{ ADL_P_LP5_2,	TYPE_C_PORT_2 },
	{ ADL_M_LP4,	TYPE_C_PORT_1 },
	{ ADL_M_LP5,	TYPE_C_PORT_0 },
};

static void variant_update_typec_init_config(void)
{
	/* Skip filling aux bias gpio pads for Windows SKUs */
	if (!(CONFIG(BOARD_INTEL_ADLRVP_P_EXT_EC) || CONFIG(BOARD_INTEL_ADLRVP_M_EXT_EC)))
		return;

	config_t *config = config_of_soc();
	int board_id = get_board_id();
	for (int i = 0; i < ARRAY_SIZE(port_config); i++) {
		if (board_id != port_config[i].board_id)
			continue;

		memcpy(&config->typec_aux_bias_pads[port_config[i].port], &pad_config,
			sizeof(pad_config));
	}
}

void variant_devtree_update(void)
{
	variant_update_power_limits();
	variant_update_typec_init_config();
}
