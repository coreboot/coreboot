/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef OPENSIL_GENOA_POC_MPIO_CHIP_H
#define OPENSIL_GENOA_POC_MPIO_CHIP_H

#include <stdint.h>

/*
 * GENOA MPIO mapping
 * P0 -> [0-15]
 * G0 -> [16-31]
 * P1 -> [32-47]
 * G1 -> [48-63]
 * P2 -> [64-79]
 * G2 -> [80-95]
 * P3 -> [96-111]
 * G3 -> [112-127]
 * P4 -> [128-131]
 * P5 -> [132-136]
 */

enum mpio_type {
	IFTYPE_UNUSED,
	IFTYPE_PCIE,
	IFTYPE_SATA,
};

/* Sync with PCIE_HOTPLUG_TYPE */
enum mpio_hotplug {
	HotplugDisabled,                             ///< Hotplug disable
	Basic,                                       ///< Basic Hotplug
	ServerExpress,                               ///< Server Hotplug Express Module
	Enhanced,                                    ///< Enhanced
	Inboard,                                     ///< Inboard
	ServerEntSSD,                                ///< Server Hotplug Enterprise SSD
	UBM,                                         ///< UBM Backplane
	OCP,                                         ///< OCP NIC 3.0
};

enum pcie_link_speed {
	MaxSupported,
	Gen1,
	Gen2,
	Gen3,
	Gen4,
	Gen5,
};

/* Sync with PCIE_ASPM_TYPE */
enum pcie_aspm {
	aspm_disabled,
	L0s,
	L1,
	L0sL1,
};

struct drivers_amd_opensil_mpio_config {
	enum mpio_type type;
	uint8_t start_lane;
	uint8_t end_lane;
	uint8_t gpio_group;
	enum mpio_hotplug hotplug;
	enum pcie_link_speed speed;
	enum pcie_aspm aspm;
	uint8_t aspm_l1_1 : 1;
	uint8_t aspm_l1_2 : 1;
	uint8_t clock_pm : 1;
	uint8_t bmc : 1;
};

#endif /* OPENSIL_GENOA_POC_MPIO_CHIP_H */
