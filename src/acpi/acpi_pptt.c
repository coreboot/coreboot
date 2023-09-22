/* SPDX-License-Identifier: GPL-2.0-only */

#include <acpi/acpi.h>
#include <include/assert.h>
#include <console/console.h>

/*
 * A structure to hold a cache pointer
 * and its corresponding reference within
 * the PPTT table.
 */
struct cache_reference {
	struct pptt_cache *cache; // cache pointer
	u32                ref;   // and its reference within PPTT
};

/*
 * A list of 'struct cache_reference', used
 * to keep track of generated caches per topology level.
 */
struct cache_list {
	u32                    n_caches;                                // number of caches in list
	struct cache_reference cache_refs[CONFIG_ACPI_PPTT_MAX_CACHES]; // cache reference list
};

/*
 * Start of the PPTT table. Constant
 * value as soon as we enter acpi_create_pptt_body.
 */
static uintptr_t pptt_start;

/* --- Helper Functions (non exposed) --- */

static inline u32 node_to_reference(const void *node)
{
	/*
	 * References are the offset from the start
	 * of the PPTT table.
	 *
	 *    PPTT
	 * +---------+ <- pptt_start (acpi_pptt_t) <---+
	 * |         |                                 | node - pptt_start
	 * |         |                                 |
	 * +---------+ <- node (cpu or cache) <--------+
	 * |         |
	 * |         |
	 * |         |
	 * +---------+
	 */
	return ((uintptr_t)node - pptt_start);
}

static u32 count_resources(struct pptt_cpu_resources *res)
{
	u32 n_resources = 0;

	while (res != NULL) {
		n_resources += 1;
		res = res->next;
	}

	return n_resources;
}

static u32 cache_list_ref_of(struct cache_list *cache_list, const struct pptt_cache *cache)
{
	/*
	 * Lookup the PPTT reference of 'cache'.
	 * Return 0, if no PPTT structure exists for 'cache'.
	 */

	for (int i = 0; i < cache_list->n_caches; i++) {
		if (cache_list->cache_refs[i].cache == cache)
			return cache_list->cache_refs[i].ref;
	}

	/* no cache reference found */
	return 0;
}

static inline void cache_list_append(struct cache_list *cache_list, struct pptt_cache *cache, const u32 ref)
{
	printk(BIOS_DEBUG, "acpi: pptt: cache=%p ref=%u\n", cache, ref);

	cache_list->cache_refs[cache_list->n_caches].cache = cache;
	cache_list->cache_refs[cache_list->n_caches].ref   = ref;

	cache_list->n_caches += 1;
}

static u32 new_pptt_cache(unsigned long *current, struct pptt_cache *cache, struct cache_list *cache_list)
{
	static u32 unique_cache_id = 1;
	u32 current_reference      = 0;

	if ((current_reference = cache_list_ref_of(cache_list, cache)) != 0)
		return current_reference;

	if (cache_list->n_caches >= CONFIG_ACPI_PPTT_MAX_CACHES) {
		printk(BIOS_WARNING, "acpi: pptt: Too many distinct caches! PPTT incomplete.\n");
		return 0;
	}

	acpi_pptt_cache_node_t *cache_node = (acpi_pptt_cache_node_t *)*current;
	memset(cache_node, 0x0, sizeof(acpi_pptt_cache_node_t));

	cache_node->type   = PPTT_NODE_TYPE_CACHE;
	cache_node->length = sizeof(acpi_pptt_cache_node_t);

	cache_node->flags         = cache->flags.raw;
	cache_node->size          = cache->size;
	cache_node->n_sets        = cache->numsets;
	cache_node->associativity = cache->associativity;

	cache_node->attributes = cache->attributes;
	cache_node->line_size  = cache->line_size;
	cache_node->cache_id   = unique_cache_id++;

	*current += cache_node->length;

	current_reference = node_to_reference(cache_node);
	cache_list_append(cache_list, cache, current_reference);

	if (cache->next_level != NULL)
		cache_node->next_level = new_pptt_cache(current, cache->next_level, cache_list);

	return current_reference;
}

static u32 new_pptt_cpu(unsigned long *current, const struct pptt_topology *cpu, const u32 parent_ref, struct cache_list *cache_list)
{
	acpi_pptt_cpu_node_t *cpu_node = (acpi_pptt_cpu_node_t *)*current;

	const u32 n_resources      = count_resources(cpu->resources);
	const u32 structure_length = sizeof(acpi_pptt_cpu_node_t) + (n_resources * sizeof(u32));

	memset(cpu_node, 0x0, structure_length);

	cpu_node->type         = PPTT_NODE_TYPE_CPU;
	cpu_node->length       = structure_length;
	cpu_node->flags        = cpu->flags.raw;
	cpu_node->processor_id = cpu->processor_id;
	cpu_node->parent       = parent_ref;

	*current += cpu_node->length;

	for (struct pptt_cpu_resources *it = cpu->resources; it != NULL; it = it->next)
		cpu_node->resources[cpu_node->n_resources++] = new_pptt_cache(current, it->cache, cache_list);

	return node_to_reference(cpu_node);
}

static void setup_topology(const struct pptt_topology *node, const u32 parent_ref, unsigned long *current)
{
	struct cache_list cache_list = {
		.cache_refs = { },
		.n_caches   = 0
	};

	while (node != NULL) {
		const u32 cpu_ref = new_pptt_cpu(current, node, parent_ref, &cache_list);
		setup_topology(node->child, cpu_ref, current);

		node = node->sibling;
	}
}

/* --- PPTT generation helper functions (exposed) --- */

void acpi_create_pptt_body(acpi_pptt_t *pptt)
{
	/* set start of pptt table */
	pptt_start = (uintptr_t)pptt;

	/* locate start of pptt body */
	unsigned long current = (unsigned long)(pptt->body);

	/* retrieve processor topology */
	const struct pptt_topology *topology_tree = acpi_get_pptt_topology();

	/* write processor properties topology table to memory */
	setup_topology(topology_tree, 0, &current);

	/* update length field in pptt header */
	pptt->header.length = current - (unsigned long)pptt;
}
