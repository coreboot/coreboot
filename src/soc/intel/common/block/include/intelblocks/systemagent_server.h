/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef SOC_INTEL_COMMON_BLOCK_SA_SERVER_H
#define SOC_INTEL_COMMON_BLOCK_SA_SERVER_H

#include <device/device.h>
#include <device/pci_type.h>
#include <stdbool.h>
#include <stdint.h>

enum sa_server_reg {
	MMCFG_BASE_REG,
	MMCFG_LIMIT_REG,
	TSEG_BASE_REG,
	TSEG_LIMIT_REG,
	TOCM_REG,
	TOUUD_REG,
	TOLUD_REG,
	MMIO_L_REG,
	VT_BAR_REG,
	DPR_REG,
	NUM_MAP_ENTRIES, /* Must be last. */
};

/**
 * SoC implementation to convert register index to SoC-specific register PCIe configuration
 * space offset.
 */
uint32_t sa_server_soc_reg_to_pci_offset(enum sa_server_reg reg);

struct sa_server_mem_map_descriptor {
	uint32_t reg_offset; /* PCI configuration space offset. */
	bool is_64_bit;      /* If register is 64 bit. */
	bool is_limit;       /* If lower bits should be treated as 1s or 0s but always read as 0s. */
	uint32_t alignment;  /* Alignment of the address in register. Probing as PCI BAR if 0. */
};

uint64_t sa_server_read_map_entry(pci_devfn_t dev,
				  const struct sa_server_mem_map_descriptor *entry);

struct sa_server_mmio_descriptor {
	uint64_t base;
	uint64_t size;
	bool (*get_resource)(struct device *dev, uint64_t *base, uint64_t *size);
	const char *description; /* Name of the register. */
};

uintptr_t sa_server_get_tseg_base(void);

size_t sa_server_get_tseg_size(void);

uint64_t sa_server_get_touud(void);

uintptr_t sa_server_get_tolud(void);

/*
 * API to determine whether system agent is on PCH domain or accelerators domains.
 */
bool sa_server_is_on_pch_domain(const struct device *dev);

/*
 * API to add MMIO resources based on `struct sa_mmio_descriptor` provided by SoC.
 */
void sa_server_add_mmio_resources(
	struct device *dev, int *resource_cnt,
	const struct sa_server_mmio_descriptor *sa_server_fixed_resources, size_t count);

/**
 * SoC call to provide all known fixed memory ranges for system agent.
 *
 * SoC function should provide fixed resource ranges in form of
 * `struct sa_server_mmio_descriptor` along with resource count.
 */
void sa_server_soc_add_fixed_mmio_resources(struct device *dev, int *resource_cnt);

#endif // SOC_INTEL_COMMON_BLOCK_SA_SERVER_H
