/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef AMD_TURIN_POC_FABRIC_H
#define AMD_TURIN_POC_FABRIC_H

#include <types.h>

#define TURIN_ROOT_BRIDGES_PER_SOCKET 8

struct device;

/* Ramstage topology, published only after every root has been validated. */
struct turin_root_bridge {
	uint16_t fabric_id;
	uint16_t segment_group;
	uint8_t bus_base;
	uint8_t bus_limit;
	uint8_t socket;
	uint8_t die;
	uint8_t index; /* Index in openSIL's die device map, not a physical RB. */
	uint8_t physical_rb; /* DF IOS instance offset, before NBIO's RBIndex fixup. */
	uint8_t nbio_id;
	bool has_fch;
	bool has_smu;
	bool is_system_fch;
	bool is_system_smu;
};

/* Call after the first openSIL initialization phase and before device enumeration. */
enum cb_err turin_fabric_init(void);

/* domain_id = socket * 8 + local_domain. NULL before init, for absent roots/
 * sockets, or for roots without a decoded PCI config window.
 */
const struct turin_root_bridge *turin_get_root_bridge(unsigned int domain_id);
unsigned int turin_get_socket_count(void);
void turin_pci_domain_scan_bus(struct device *domain);

#endif /* AMD_TURIN_POC_FABRIC_H */
