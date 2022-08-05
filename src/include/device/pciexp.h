/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef DEVICE_PCIEXP_H
#define DEVICE_PCIEXP_H
/* (c) 2005 Linux Networx GPL see COPYING for details */

enum aspm_type {
	PCIE_ASPM_NONE = 0,
	PCIE_ASPM_L0S  = 1,
	PCIE_ASPM_L1   = 2,
	PCIE_ASPM_BOTH = 3,
};

#define ASPM_LTR_L12_THRESHOLD_VALUE_OFFSET 16
#define ASPM_LTR_L12_THRESHOLD_VALUE_MASK (0x3ff << ASPM_LTR_L12_THRESHOLD_VALUE_OFFSET)
#define ASPM_LTR_L12_THRESHOLD_SCALE_OFFSET 29
#define ASPM_LTR_L12_THRESHOLD_SCALE_MASK (0x7 << ASPM_LTR_L12_THRESHOLD_SCALE_OFFSET)

/* Latency tolerance reporting, max non-snoop latency value 3.14ms */
#define PCIE_LTR_MAX_NO_SNOOP_LATENCY_3146US 0x1003
/* Latency tolerance reporting, max snoop latency value 3.14ms */
#define PCIE_LTR_MAX_SNOOP_LATENCY_3146US 0x1003

void pciexp_scan_bus(struct bus *bus, unsigned int min_devfn,
			     unsigned int max_devfn);

void pciexp_scan_bridge(struct device *dev);

extern struct device_operations default_pciexp_ops_bus;

void pciexp_hotplug_scan_bridge(struct device *dev);

extern struct device_operations default_pciexp_hotplug_ops_bus;

unsigned int pciexp_find_extended_cap(const struct device *dev, unsigned int cap,
				      unsigned int offset);

static inline bool pciexp_is_downstream_port(int type)
{
	return type == PCI_EXP_TYPE_ROOT_PORT ||
	       type == PCI_EXP_TYPE_DOWNSTREAM ||
	       type == PCI_EXP_TYPE_PCIE_BRIDGE;
}

bool pciexp_get_ltr_max_latencies(struct device *dev, u16 *max_snoop, u16 *max_nosnoop);

#endif /* DEVICE_PCIEXP_H */
