/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef SOC_MEDIATEK_PCIE_COMMON_H
#define SOC_MEDIATEK_PCIE_COMMON_H

#include <device/device.h>
#include <types.h>

struct mtk_pcie_mmio_res {
	uint32_t cpu_addr;
	uint32_t pci_addr;
	uint32_t size;
	unsigned long type;
};

struct mtk_pcie_config {
	uintptr_t base;		/* MAC physical address */
	const struct mtk_pcie_mmio_res mmio_res_io;
	const struct mtk_pcie_mmio_res mmio_res_mem;
};

void mtk_pcie_domain_read_resources(struct device *dev);
void mtk_pcie_domain_set_resources(struct device *dev);
void mtk_pcie_domain_enable(struct device *dev);

#endif
