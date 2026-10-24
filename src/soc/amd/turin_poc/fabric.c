/* SPDX-License-Identifier: GPL-2.0-only */

#include <amdblocks/root_complex.h>
#include <amdblocks/data_fabric.h>
#include <console/console.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_type.h>
#include <soc/fabric.h>
#include <string.h>
#include <xPrfFabric.h>

#define MAX_ROOT_BRIDGES (CONFIG_MAX_SOCKET * TURIN_ROOT_BRIDGES_PER_SOCKET)

/*
 * Keep the primary FCH first, followed by the existing socket-local domain
 * order. xPrfFabric's physical root number is a DF IOS instance offset, before
 * the NBIO package fixup renumbers RBIndex. DF0..7 map to NBIO0,5,1,4,2,7,3,6.
 * Each SMN base follows the NBIO number, not openSIL's enumeration index.
 * openSIL indices and system fabric IDs are discovered independently.
 */
static const struct {
	uint8_t physical_rb;
	uint32_t misc_smn_base;
} domain_order[] = {
	{ 4, SMN_IOHC_MISC_BASE_13D1 }, /* NBIO2: FCH/SMU */
	{ 5, SMN_IOHC_MISC_BASE_1D71 }, /* NBIO7 */
	{ 6, SMN_IOHC_MISC_BASE_13E1 }, /* NBIO3 */
	{ 7, SMN_IOHC_MISC_BASE_1D61 }, /* NBIO6 */
	{ 3, SMN_IOHC_MISC_BASE_1D41 }, /* NBIO4 */
	{ 2, SMN_IOHC_MISC_BASE_13C1 }, /* NBIO1 */
	{ 1, SMN_IOHC_MISC_BASE_1D51 }, /* NBIO5 */
	{ 0, SMN_IOHC_MISC_BASE_13B1 },
};

_Static_assert(ARRAY_SIZE(domain_order) == TURIN_ROOT_BRIDGES_PER_SOCKET,
	       "Every Turin root bridge needs a domain mapping");
_Static_assert(CONFIG_MAX_SOCKET >= 1 && CONFIG_MAX_SOCKET <= 2,
	       "Turin supports one or two sockets");

static struct turin_root_bridge roots[MAX_ROOT_BRIDGES];
static struct domain_iohc_info iohc[MAX_ROOT_BRIDGES];
static bool root_seen[MAX_ROOT_BRIDGES];
static unsigned int domain_count;
static unsigned int socket_count;

const struct turin_root_bridge *turin_get_root_bridge(unsigned int domain_id)
{
	return domain_id < domain_count && iohc[domain_id].misc_smn_base ?
		&roots[domain_id] : NULL;
}

unsigned int turin_get_socket_count(void)
{
	return socket_count;
}

const struct domain_iohc_info *get_iohc_info(size_t *count)
{
	*count = domain_count;
	return domain_count ? iohc : NULL;
}

static unsigned int local_domain(uint32_t physical_rb)
{
	for (unsigned int i = 0; i < ARRAY_SIZE(domain_order); i++) {
		if (domain_order[i].physical_rb == physical_rb)
			return i;
	}
	return ARRAY_SIZE(domain_order);
}

static bool matches_location(const XPRF_ROOT_BRIDGE_LOCATION *location,
			     unsigned int socket, unsigned int index)
{
	return location->Socket == socket && location->Die == 0 && location->Index == index;
}

static bool valid_bus_range(uint32_t base, uint32_t limit)
{
	/* The public API returns segment * 256 + bus, including for the limit.
	 * SilPass alone does not imply a decoded range: absent bases are UINT32_MAX.
	 * PCI_DEV() encodes only four segment bits; reject before it can truncate.
	 */
	return base != UINT32_MAX && limit != UINT32_MAX && base <= limit &&
		(base >> 8) == (limit >> 8) && (base >> 8) < 16 &&
		(base >> 8) < PCI_SEGMENT_GROUP_COUNT &&
		(base & PCI_BUS_NUMBER_MASK) < PCI_BUSES_PER_SEGMENT_GROUP &&
		(limit & PCI_BUS_NUMBER_MASK) < PCI_BUSES_PER_SEGMENT_GROUP;
}

static enum cb_err discover_socket(unsigned int socket,
				   const XPRF_ROOT_BRIDGE_LOCATION *fch,
				   const XPRF_ROOT_BRIDGE_LOCATION *smu,
				   unsigned int *discovered_count)
{
	uint32_t dies = 0, socket_roots = 0, die_roots = 0;
	SIL_STATUS status = xPrfFabricGetProcessorInfo(socket, &dies, &socket_roots);

	/* Turin has one I/O die and up to eight physical root bridges per socket. */
	if (status != SilPass || dies != 1 || socket_roots > TURIN_ROOT_BRIDGES_PER_SOCKET) {
		printk(BIOS_ERR, "Turin socket %u: topology status %u, dies %u, roots %u\n",
		       socket, status, dies, socket_roots);
		return CB_ERR;
	}
	status = xPrfFabricGetDieInfo(socket, 0, &die_roots, NULL, NULL);
	if (status != SilPass || die_roots != socket_roots) {
		printk(BIOS_ERR, "Turin socket %u: die topology status %u, roots %u\n",
		       socket, status, die_roots);
		return CB_ERR;
	}

	for (unsigned int index = 0; index < die_roots; index++) {
		uint32_t fabric_id = UINT32_MAX, base = UINT32_MAX, limit = UINT32_MAX;
		uint32_t physical_rb = UINT32_MAX;
		bool has_fch = false, has_smu = false;

		status = xPrfFabricGetRootBridgeInfo(socket, 0, index, &fabric_id, &base,
						   &limit, &physical_rb, &has_fch, &has_smu);
		const unsigned int local = local_domain(physical_rb);
		if (status != SilPass || local >= ARRAY_SIZE(domain_order) ||
		    fabric_id > 0xfff || (base != UINT32_MAX && !valid_bus_range(base, limit))) {
			printk(BIOS_ERR, "Turin socket %u root %u: invalid status %u, physical "
			       "%u, fabric %#x, bus range %#x-%#x\n", socket, index, status,
			       physical_rb, fabric_id, base, limit);
			return CB_ERR;
		}
		const unsigned int domain = socket * TURIN_ROOT_BRIDGES_PER_SOCKET + local;
		if (root_seen[domain]) {
			printk(BIOS_ERR, "Turin socket %u: duplicate physical root %u\n",
			       socket, physical_rb);
			return CB_ERR;
		}
		root_seen[domain] = true;

		roots[domain] = (struct turin_root_bridge) {
			.fabric_id = fabric_id,
			.segment_group = base >> 8,
			.bus_base = base & PCI_BUS_NUMBER_MASK,
			.bus_limit = limit & PCI_BUS_NUMBER_MASK,
			.socket = socket,
			.die = 0,
			.index = index,
			.physical_rb = physical_rb,
			.has_fch = has_fch,
			.has_smu = has_smu,
			.is_system_fch = matches_location(fch, socket, index),
			.is_system_smu = matches_location(smu, socket, index),
		};
		/* No config window is a legitimate result for an unused/fused root.
		 * Retain its identity for consistency checks, but never expose an
		 * ECAM/SMN endpoint or scan that domain. Primary roots are checked below.
		 */
		if (base == UINT32_MAX) {
			printk(BIOS_DEBUG, "Turin domain %u: root has no PCI config window\n", domain);
			continue;
		}
		iohc[domain] = (struct domain_iohc_info) {
			.fabric_id = fabric_id,
			.misc_smn_base = domain_order[local].misc_smn_base,
			.smn_dev = PCI_DEV(base, 0, 0),
			.socket = socket,
		};
	}
	*discovered_count += die_roots;
	return CB_SUCCESS;
}

static enum cb_err discover_fabric(void)
{
	uint32_t sockets = 0, dies = 0, total_roots = 0;
	XPRF_ROOT_BRIDGE_LOCATION fch = { UINT32_MAX, UINT32_MAX, UINT32_MAX };
	XPRF_ROOT_BRIDGE_LOCATION smu = fch;

	/* Do not expose a partial discovery or retain stale data after a retry. */
	domain_count = 0;
	socket_count = 0;
	memset(roots, 0, sizeof(roots));
	memset(iohc, 0, sizeof(iohc));
	memset(root_seen, 0, sizeof(root_seen));

	SIL_STATUS status = xPrfFabricGetSystemInfo(&sockets, &dies, &total_roots, &fch, &smu);
	printk(BIOS_DEBUG, "Turin openSIL FCH: S%u die %u root %u; SMU: S%u die %u root %u\n",
	       fch.Socket, fch.Die, fch.Index, smu.Socket, smu.Die, smu.Index);
	if (status != SilPass || !sockets || sockets > CONFIG_MAX_SOCKET || dies != sockets ||
	    !total_roots || total_roots > sockets * TURIN_ROOT_BRIDGES_PER_SOCKET) {
		printk(BIOS_ERR, "Turin system: topology status %u, sockets %u, dies %u, roots %u; "
		       "configured socket limit %u\n", status, sockets, dies, total_roots,
		       CONFIG_MAX_SOCKET);
		return CB_ERR;
	}

	unsigned int discovered_count = 0;
	for (unsigned int socket = 0; socket < sockets; socket++) {
		if (discover_socket(socket, &fch, &smu, &discovered_count) != CB_SUCCESS)
			return CB_ERR;
	}
	if (discovered_count != total_roots) {
		printk(BIOS_ERR, "Turin: reported %u roots but discovered %u\n",
		       total_roots, discovered_count);
		return CB_ERR;
	}

	const unsigned int domains = sockets * TURIN_ROOT_BRIDGES_PER_SOCKET;
	unsigned int system_smu_count = 0;
	for (unsigned int i = 0; i < domains; i++) {
		const struct turin_root_bridge *root = &roots[i];
		if (!root_seen[i])
			continue;
		if (root->is_system_smu && (!root->has_smu || !iohc[i].misc_smn_base)) {
			printk(BIOS_ERR, "Turin domain %u: invalid or undecoded system SMU\n", i);
			return CB_ERR;
		}
		system_smu_count += root->is_system_smu;
		for (unsigned int j = 0; j < i; j++) {
			const struct turin_root_bridge *other = &roots[j];
			if (!root_seen[j])
				continue;
			if (root->fabric_id == other->fabric_id ||
			    (iohc[i].misc_smn_base && iohc[j].misc_smn_base &&
			     root->segment_group == other->segment_group &&
			     root->bus_base <= other->bus_limit && other->bus_base <= root->bus_limit)) {
				printk(BIOS_ERR, "Turin domains %u and %u: duplicate fabric ID or "
				       "overlapping PCI bus ranges\n", i, j);
				return CB_ERR;
			}
		}
	}

	/* Static FCH devices and the default PSP mailbox belong to S0, segment 0,
	 * bus 0. Reject a different primary rather than silently addressing it.
	 */
	if (!iohc[0].misc_smn_base || !roots[0].is_system_fch || !roots[0].has_fch ||
	    roots[0].segment_group != 0 ||
	    roots[0].bus_base != 0 || system_smu_count != 1) {
		printk(BIOS_ERR, "Turin: unsupported system FCH/SMU location\n");
		printk(BIOS_ERR, "Turin domain 0: DF root %u physical %u PCI %04x:%02x "
		       "decoded %u has_fch %u system_fch %u; system SMU count %u\n",
		       roots[0].index, roots[0].physical_rb, roots[0].segment_group,
		       roots[0].bus_base, !!iohc[0].misc_smn_base, roots[0].has_fch,
		       roots[0].is_system_fch, system_smu_count);
		return CB_ERR;
	}

	domain_count = domains;
	socket_count = sockets;
	for (unsigned int i = 0; i < domain_count; i++) {
		const struct turin_root_bridge *root = &roots[i];
		if (!iohc[i].misc_smn_base)
			continue;
		printk(BIOS_DEBUG, "Turin domain %u: S%u die %u root %u physical %u fabric %#x "
		       "PCI %04x:%02x-%02x SMN %#x\n", i, root->socket, root->die,
		       root->index, root->physical_rb, root->fabric_id, root->segment_group,
		       root->bus_base, root->bus_limit, iohc[i].misc_smn_base);
	}
	return CB_SUCCESS;
}

enum cb_err turin_fabric_init(void)
{
	if (discover_fabric() == CB_SUCCESS)
		return CB_SUCCESS;

	/* Preserve access to bus-zero FCH devices if openSIL discovery failed.
	 * Only the primary root's fixed fabric/SMN identity is assumed. Its PCI
	 * decode is still checked by the existing DF register reader below.
	 */
	memset(roots, 0, sizeof(roots));
	memset(iohc, 0, sizeof(iohc));
	domain_count = 1;
	socket_count = 0;
	iohc[0] = (struct domain_iohc_info) {
		.fabric_id = 0x24,
		.misc_smn_base = SMN_IOHC_MISC_BASE_13D1,
		.smn_dev = PCI_DEV(0, 0, 0),
	};
	roots[0] = (struct turin_root_bridge) {
		.fabric_id = 0x24,
		.physical_rb = 4,
		.nbio_id = 2,
		.has_fch = true,
		.is_system_fch = true,
	};
	printk(BIOS_WARNING, "Turin: using primary FCH root after discovery failure\n");
	return CB_ERR;
}

void turin_pci_domain_scan_bus(struct device *domain)
{
	const struct turin_root_bridge *root = turin_get_root_bridge(dev_get_domain_id(domain));
	if (!root) {
		printk(BIOS_ERR, "Turin domain %u has no discovered root bridge\n",
		       dev_get_domain_id(domain));
		domain->enabled = 0;
		return;
	}
	/* Failed discovery leaves only the primary FCH root and no socket count.
	 * Validate its hardware bus decode before scanning this fallback root.
	 */
	if (!socket_count) {
		uint8_t segment, bus, limit;
		if (data_fabric_get_pci_bus_numbers(domain, &segment, &bus, &limit) != CB_SUCCESS ||
		    segment != 0 || bus != 0 || !valid_bus_range(bus, limit)) {
			printk(BIOS_ERR, "Turin: primary FCH PCI decode is unavailable\n");
			domain->enabled = 0;
			return;
		}
		roots[0].bus_limit = limit;
	}

	domain->downstream->secondary = root->bus_base;
	domain->downstream->subordinate = root->bus_base;
	domain->downstream->max_subordinate = root->bus_limit;
	domain->downstream->segment_group = root->segment_group;
	pci_host_bridge_scan_bus(domain);
}
