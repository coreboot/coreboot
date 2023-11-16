/* SPDX-License-Identifier: GPL-2.0-only */

#include <stdlib.h>
#include <cpu/cpu.h>
#include <arch/cache.h>
#include <acpi/acpi.h>
#include <console/console.h>

#define CACHE_NODE_FLAGS 0xd7 // everything valid except, write-policy and allocation type
#define CLUSTER_FLAGS    0x11 // physical package, ID invalid, no thread, no leaf, identical impl.
#define CORE_FLAGS       0x0a // no physical package, ID valid, no thread, leaf.
#define CORE_FLAGS_1       0x12 // no physical package, ID valid, no thread, identical i.

/*
 * L2 cache (LLC)
 */
struct pptt_cache l2 = {

	.next_level = NULL
};

/*
 * L1D cache
 */
struct pptt_cache l1d = {
	.next_level = &l2
};

/*
 * L1I cache
 */
struct pptt_cache l1i = {
	//.sibling = &l1d,
	.next_level = &l2
};

/*
 * private resources of a cpu core. Same for
 * each core, thus we can reuse this struture
 * instead of creating it dynamically.
 */
struct pptt_cpu_resources core_resources = {
	.cache = &l1i,
};

struct pptt_topology root_topology = {

	.flags.raw = CLUSTER_FLAGS,
	.resources = NULL,
	.sibling   = NULL,

	.child     = &(struct pptt_topology) {

		.processor_id = 0,
		.flags.raw    = CORE_FLAGS_1,

		.resources = NULL,

		.child     = &(struct pptt_topology) {
			.processor_id = 0,
			.flags.raw    = CORE_FLAGS,

			.resources = &(struct pptt_cpu_resources) {

				.cache = &l1i,

			},

			.sibling   = NULL,
			.child   = NULL,
		},
		.sibling   = NULL, // updated in runtime
	}
};

/* --- Helpers --- */

static u8 cache_attributes(const enum cache_type type)
{
	/*
	 * 'write-policy' and 'allocation type' currently
	 * unsupported. cache flags set accordingly.
	 *
	 * maybe a todo for the future.
	 */

	u8 attr = 0x0;

	if (type == CACHE_INSTRUCTION)
		attr |= (0x1 << 2);
	else if (type == CACHE_UNIFIED)
		attr |= (0x1 << 3);

	return attr;
}

/* --- ACPI hook --- */

struct pptt_topology *acpi_get_pptt_topology(void)
{
	struct cache_info info;

	/* Dump Cache info */
	for (int cache_level = CACHE_L1; cache_level <= CACHE_L7; cache_level++) {
		int cache_type = cpu_get_cache_type(cache_level);
		if (cache_type == NO_CACHE)
			continue;

		if (cache_type == CACHE_SEPARATE) {
			printk(BIOS_DEBUG, "Fetching cache info for: level:%d, type:%d\n",
			       cache_level, cache_type);
			cpu_get_cache_info(cache_level, cache_type, &info);
			printk(BIOS_DEBUG, "Size: %lld, associativity: %lld\n", info.size,
			       info.associativity);

			cache_type = CACHE_INSTRUCTION;

			printk(BIOS_DEBUG, "Fetching cache info for: level:%d, type:%d\n",
			       cache_level, cache_type);
			cpu_get_cache_info(cache_level, cache_type, &info);
			printk(BIOS_DEBUG, "Size: %lld, associativity: %lld\n", info.size,
			       info.associativity);

			cache_type = CACHE_DATA;
		}

		printk(BIOS_DEBUG, "Fetching cache info for: level:%d, type:%d\n", cache_level,
		       cache_type);
		cpu_get_cache_info(cache_level, cache_type, &info);
		printk(BIOS_DEBUG, "Size: %lld, associativity: %lld\n", info.size,
		       info.associativity);
	}

	/* update cache information (L1I) */

	cpu_get_cache_info(CACHE_L1, CACHE_INSTRUCTION, &info);

	l1i.size	  = info.size;
	l1i.associativity = info.associativity;
	l1i.numsets       = info.numsets;
	l1i.line_size     = info.line_bytes;
	l1i.attributes    = cache_attributes(CACHE_INSTRUCTION);
	l1i.flags.raw     = CACHE_NODE_FLAGS | 0xff;

	/* update cache information (L1D) */

	cpu_get_cache_info(CACHE_L1, CACHE_DATA, &info);

	l1d.size	  = info.size;
	l1d.associativity = info.associativity;
	l1d.numsets       = info.numsets;
	l1d.line_size     = info.line_bytes;
	l1d.attributes    = cache_attributes(CACHE_DATA) | (0x2);
	l1d.flags.raw     = CACHE_NODE_FLAGS | 0xff;

	/* update cache information (L2) */

	cpu_get_cache_info(CACHE_L2, CACHE_UNIFIED, &info);

	l2.size	   = info.size;
	l2.associativity  = info.associativity;
	l2.numsets	= info.numsets;
	l2.line_size      = info.line_bytes;
	l2.attributes     = cache_attributes(CACHE_UNIFIED) | (0x2);
	l2.flags.raw      = CACHE_NODE_FLAGS | 0xff;

	/* add secondary CPUs */

	u32 cpu_id = 0;

	struct device *dev   = NULL;
	struct pptt_topology **it = &root_topology.child->sibling;
	struct pptt_topology **sibling;

	while ((dev = dev_find_path(dev, DEVICE_PATH_GICC_V3))) {
		if (cpu_id == 0) {

			cpu_id += 1;
			continue;
		}

		if ((*it = malloc(sizeof(struct pptt_topology))) == NULL) {

			printk(BIOS_ERR, "Could not allocate pptt structure!\n");
			break;
		}

		memset(*it, 0, sizeof(struct pptt_topology));

		(*it)->processor_id = cpu_id;
		(*it)->flags.raw    = CORE_FLAGS_1;
		(*it)->resources    = NULL;

		//sibling = (*it)->sibling;
		sibling = &(*it)->sibling;

		it = &(*it)->child;

		if ((*it = malloc(sizeof(struct pptt_topology))) == NULL) {

			printk(BIOS_ERR, "Could not allocate pptt structure!\n");
			break;
		}

		memset(*it, 0, sizeof(struct pptt_topology));

		(*it)->processor_id = cpu_id;
		(*it)->flags.raw    = CORE_FLAGS;
		(*it)->resources    = &core_resources;

		//it = &(*it)->sibling;
		it = sibling;

		cpu_id += 1;
	}

	return &root_topology;
}
