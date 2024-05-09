/* SPDX-License-Identifier: GPL-2.0-only */

/*
 * This header file defines data structures and operations related
 * to NUMA proximity domains.
 */
#ifndef NUMA_H
#define NUMA_H

#include <soc/soc_util.h>
#include <types.h>

#define XEONSP_INVALID_PD_INDEX	UINT32_MAX

enum proximity_domain_type {
	PD_TYPE_PROCESSOR,
	/*
	 * The Generic Initiator concept is used in ACPI spec. A typical
	 * Generic Initiator domain is a CXL memory device.
	 */
	PD_TYPE_GENERIC_INITIATOR,
	PD_TYPE_MAX
};

/*
 * This proximity domain structure records all data related to
 * a proximity doamin needed for following purpose:
 * a. Device resource allocation. IIO stack involving CXL device
 *    needs to have different resource allocation method.
 * b. e820 table setup. For example, CXL memory region may need to
 *    be set as soft reserved, since it is specific purpose memory.
 * c. ACPI NUMA tables (SRAT, SLIT, HMAT).
 */
struct proximity_domain {
	enum proximity_domain_type pd_type;
	/*
	 * For processor domain, this holds the socket #.
	 * For generic initiator domain, this indicates to which socket the
	 * device is attached to. CXL 2.0 allows interleaving within and between
	 * sockets, so we need a bitmap.
	 */
	uint8_t socket_bitmap;
	/* Relative distances (memory latency) from all domains */
	uint8_t *distances;
	/*
	 * Below fields are set to 0 for processor domains.
	 */
	struct device *dev;
	uint32_t base; /* Memory region base address in the unit of 64MB */
	uint32_t size; /* Memory region size in the unit of 64MB */
};

struct proximity_domains {
	uint8_t num_pds;
	struct proximity_domain *pds;
};

extern struct proximity_domains pds;

void setup_pds(void);

/*
 * Return the total size of memory regions in generic initiator affinity
 * domains.  The size is in unit of 64MB.
 */
uint32_t get_generic_initiator_mem_size(void);

uint32_t memory_to_pd(const struct SystemMemoryMapElement *mem);
uint32_t device_to_pd(const struct device *dev);

#endif /* NUMA_H */
