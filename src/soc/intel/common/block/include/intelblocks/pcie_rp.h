/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef SOC_INTEL_COMMON_BLOCK_PCIE_RP_H
#define SOC_INTEL_COMMON_BLOCK_PCIE_RP_H

#include <stdint.h>

/*
 * In schematic PCIe root port numbers are 1-based, but FSP use 0-based indexes for
 * the configuration arrays and so this macro subtracts 1 to convert RP# to array index.
 */
#define PCIE_RP(x)      ((x) - 1)
#define PCH_RP(x)       PCIE_RP(x)
#define CPU_RP(x)       PCIE_RP(x)

enum pcie_rp_flags {
	PCIE_RP_HOTPLUG = (1 << 0),
	PCIE_RP_LTR = (1 << 1),
	/* PCIE RP Advanced Error Report */
	PCIE_RP_AER = (1 << 2),
	/* Clock source is not used by the root port. */
	PCIE_RP_CLK_SRC_UNUSED = (1 << 3),
	/*
	 * Clock request signal requires probing before enabling CLKREQ# based power
	 * management.
	 */
	PCIE_RP_CLK_REQ_DETECT = (1 << 4),
	/* Clock request signal is not used by the root port. */
	PCIE_RP_CLK_REQ_UNUSED = (1 << 5),
};

enum pcie_clk_src_flags {
	PCIE_CLK_FREE_RUNNING = (1 << 0),
	PCIE_CLK_LAN = (1 << 1),
};

/* This enum is for passing into an FSP UPD, typically PcieRpL1Substates */
enum L1_substates_control {
	L1_SS_FSP_DEFAULT,
	L1_SS_DISABLED,
	L1_SS_L1_1,
	L1_SS_L1_2,
};

/* This enum is for passing into an FSP UPD, typically ASPM */
enum ASPM_control {
	ASPM_DEFAULT,
	ASPM_DISABLE,
	ASPM_L0S,
	ASPM_L1,
	ASPM_L0S_L1,
	ASPM_AUTO,
};

/* PCIe Root Ports */
struct pcie_rp_config {
	/* CLKOUT_PCIE_P/N# used by this root port as per schematics. */
	uint8_t clk_src;
	/* SRCCLKREQ# used by this root port as per schematics. */
	uint8_t clk_req;
	enum pcie_rp_flags flags;
	/* PCIe RP L1 substate */
	enum L1_substates_control PcieRpL1Substates;
	/* PCIe RP ASPM */
	enum ASPM_control pcie_rp_aspm;
};

/*
 * The PCIe Root Ports usually come in groups of up to 8 PCI-device
 * functions.
 *
 * `slot` is the PCI device/slot number of such a group.
 * `start` is the initial PCI function number within the group. This is useful
 * in case the root port numbers are not contiguous within the slot.
 * `count` is the number of functions within the group starting with the `start`
 * function number.
 * `lcap_port_base` is the starting index of physical port as described in LCAP
 * register in PCIe config space. coreboot always uses 0 based indexing while
 * referring to the PCIe port but LCAP registers uses 1-based indexing in
 * most of the cases. Remapping logic needs to correctly map LCAP port number
 * (1-based or n-based) to coreboot indexing (0-based).
 */
struct pcie_rp_group {
	unsigned int slot;
	unsigned int start;
	unsigned int count;
	unsigned int lcap_port_base;
};

static inline unsigned int rp_start_fn(const struct pcie_rp_group *group)
{
	return group->start;
}

static inline unsigned int rp_end_fn(const struct pcie_rp_group *group)
{
	return group->start + group->count - 1;
}

/*
 * Update PCI paths of the root ports in the devicetree.
 *
 * Depending on the board layout and physical presence of downstream
 * devices, individual root-port functions can be hidden and reordered.
 * If we have device nodes for root ports in the static `devicetree.cb`,
 * we need to update their PCI paths, so the nodes still control the
 * correct root port. Device nodes for disabled root ports will be
 * unlinked from the bus, to not interfere with PCI enumeration.
 *
 * Call this once, after root ports have been reordered, but before PCI
 * enumeration.
 *
 * `groups` points to a list of groups terminated by an entry with `count == 0`.
 * It is assumed that the first group includes the RPs 1 to the first group's
 * `count` and that adjacent groups follow without gaps in the numbering.
 */
void pcie_rp_update_devicetree(const struct pcie_rp_group *groups);

/*
 * Return mask of PCIe root ports that are enabled by mainboard. Mask is set in
 * the same order as the root ports in pcie_rp_group groups table.
 *
 * Thus, the status of first root port in the groups table is indicated by bit 0
 * in the returned mask, second root port by bit 1 and so on.

 * 1 in the bit position indicates root port is enabled, whereas 0 indicates root
 * port is disabled. This function assumes that the maximum count of root ports
 * in the groups table is <= 32.
 */
uint32_t pcie_rp_enable_mask(const struct pcie_rp_group *groups);

/* Get PCH root port groups */
const struct pcie_rp_group *soc_get_pch_rp_groups(void);

enum pcie_rp_type {
	PCIE_RP_UNKNOWN,
	PCIE_RP_CPU,
	PCIE_RP_PCH,
};

/* For PCIe RTD3 support, each SoC that uses it must implement this function. */
struct device; /* Not necessary to include all of device/device.h */
enum pcie_rp_type soc_get_pcie_rp_type(const struct device *dev);

/* Return the virtual wire index that represents CPU-side PCIe root ports */
int soc_get_cpu_rp_vw_idx(const struct device *dev);

#endif /* SOC_INTEL_COMMON_BLOCK_PCIE_RP_H */
