/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef OPENSIL_STUB_MPIO_CHIP_H
#define OPENSIL_STUB_MPIO_CHIP_H

#include <types.h>

enum mpio_engine_type {
	IFTYPE_UNUSED,
	IFTYPE_PCIE,
	IFTYPE_SATA,
};

/* Sync with PCIE_HOTPLUG_TYPE */
enum mpio_hotplug {
	HOTPLUG_DISABLED = 0,
	HOTPLUG_BASIC,
	HOTPLUG_SERVER_EXPRESS,
	HOTPLUG_ENHANCED,
	HOTPLUG_INBOARD,
	HOTPLUG_SERVER_ENT_SSD,
	HOTPLUG_UBM,
	HOTPLUG_OCP,
};

enum pcie_link_speed {
	GEN_MAX = 0,	/* Maximum supported */
	GEN1,
	GEN2,
	GEN3,
	GEN4,
	GEN5,
};

/* Sync with PCIE_ASPM_TYPE */
enum pcie_aspm {
	ASPM_DISABLED = 0,
	ASPM_L0s,
	ASPM_L1,
	ASPM_L0sL1,
};


/* CLKREQ for PCIe type descriptors */
enum pcie_clk_req {
	CLK_DISABLE = 0x00,
	CLK_REQ0,
	CLK_REQ1,
	CLK_REQ2,
	CLK_REQ3,
	CLK_REQ4,
	CLK_REQ5,
	CLK_REQ6,
};

enum pcie_slot_power_limit_scale {
	SLOT_POWER_LIMIT_DIVISOR_1 = 0,		/* Scale factor 1 */
	SLOT_POWER_LIMIT_DIVISOR_10 = 1,	/* Scale factor 0.1 */
	SLOT_POWER_LIMIT_DIVISOR_100 = 2,	/* Scale factor 0.01 */
	SLOT_POWER_LIMIT_DIVISOR_1000 = 3,	/* Scale factor 0.001 */
};

struct vendorcode_amd_opensil_chip_mpio_config {
	enum mpio_engine_type type;
	uint8_t start_lane;
	uint8_t end_lane;
	uint8_t gpio_group;
	enum mpio_hotplug hotplug;
	enum pcie_link_speed speed_capability;
	enum pcie_aspm aspm;
	bool aspm_l1_1;
	bool aspm_l1_2;
	enum pcie_clk_req clk_req;
	bool clock_pm;
	uint8_t slot_power_limit;
	enum pcie_slot_power_limit_scale slot_power_limit_scale;
	bool bmc;
};

#endif /* OPENSIL_STUB_MPIO_CHIP_H */
