/* SPDX-License-Identifier: GPL-2.0-only */

#include <console/console.h>
#include <commonlib/stdlib.h>
#include <device/device.h>
#include <device/pci_ops.h>
#include <device/pci.h>
#include <device/pciexp.h>
#include <soc/numa.h>
#include <soc/soc_util.h>
#include <soc/util.h>
#include <types.h>

void dump_pds(void)
{
	printk(BIOS_DEBUG, "====== Proximity Domain Dump ======\n");
	printk(BIOS_DEBUG, "number of proximity domains: %d\n", pds.num_pds);
	for (uint8_t i = 0; i < pds.num_pds; i++) {
		printk(BIOS_DEBUG, "\tproximity domain %d:\n", i);
		printk(BIOS_DEBUG, "\t\ttype:%d\n", pds.pds[i].pd_type);
		printk(BIOS_DEBUG, "\t\tsocket_bitmap:0x%x\n", pds.pds[i].socket_bitmap);
		printk(BIOS_DEBUG, "\t\tdevice_handle:0x%x\n", pds.pds[i].device_handle);
		printk(BIOS_DEBUG, "\t\tbase(64MB):0x%x\n", pds.pds[i].base);
		printk(BIOS_DEBUG, "\t\tsize(64MB):0x%x\n", pds.pds[i].size);
	}
}

enum cb_err fill_pds(void)
{
	uint8_t num_sockets = soc_get_num_cpus();
	uint8_t num_cxlnodes = get_cxl_node_count();
	const IIO_UDS *hob = get_iio_uds();

	/*
	 * Rules/assumptions:
	 * 1. Each processor has a processor proximity domain regardless whether
	 * a processor has DIMM attached to it or not.
	 * 2. All system memory map elements are either from processor attached memory,
	 * or from CXL memory. Each CXL node info entry has a corresponding entry
	 * in system memory map elements.
	 * 3. Each CXL device may have multiple HDMs (Host-managed Device Memory). Each
	 * HDM has one and only one CXL node info entry. Each CXL node info entry
	 * represents a generic initiator proximity domain.
	 */
	pds.num_pds = num_cxlnodes + num_sockets;
	pds.pds = xmalloc(sizeof(struct proximity_domain) * pds.num_pds);
	if (!pds.pds)
		die("%s %d out of memory.", __FILE__, __LINE__);

	memset(pds.pds, 0, sizeof(struct proximity_domain) * pds.num_pds);

	/* Fill in processor domains */
	uint8_t i, j, socket;
	struct device *dev;
	for (socket = 0, i = 0; i < num_sockets; socket++) {
		if (!soc_cpu_is_enabled(socket))
			continue;
		pds.pds[i].pd_type = PD_TYPE_PROCESSOR;
		pds.pds[i].socket_bitmap = 1 << hob->PlatformData.IIO_resource[socket].SocketID;
		pds.pds[i].distances = malloc(sizeof(uint8_t) * pds.num_pds);
		if (!pds.pds[i].distances)
			die("%s %d out of memory.", __FILE__, __LINE__);
		/* hard code the distances for now, till we know how to calculate them. */
		for (j = 0; j < pds.num_pds; j++) {
			if (j == i)
				pds.pds[i].distances[j] = 0x0a;
			else
				pds.pds[i].distances[j] = 0x0e;
		}
		i++;
	}

	/* If there are no CXL nodes, we are done */
	if (num_cxlnodes == 0)
		return CB_SUCCESS;

	/* There are CXL nodes, fill in generic initiator domain after the processors pds */
	uint8_t skt_id, cxl_id;
	const CXL_NODE_SOCKET *cxl_hob = get_cxl_node();
	for (skt_id = 0, i = num_sockets; skt_id < MAX_SOCKET; skt_id++, i++) {
		for (cxl_id = 0; cxl_id < cxl_hob[skt_id].CxlNodeCount; ++cxl_id) {
			const CXL_NODE_INFO node = cxl_hob[skt_id].CxlNodeInfo[cxl_id];
			pds.pds[i].pd_type = PD_TYPE_GENERIC_INITIATOR;
			pds.pds[i].socket_bitmap = node.SocketBitmap;
			pds.pds[i].base = node.Address;
			pds.pds[i].size = node.Size;
			dev = pcie_find_dsn(node.SerialNumber, node.VendorId, 0);
			pds.pds[i].device_handle = PCI_BDF(dev);
			pds.pds[i].distances = malloc(sizeof(uint8_t) * pds.num_pds);
			if (!pds.pds[i].distances)
				die("%s %d out of memory.", __FILE__, __LINE__);
			/* hard code the distances until we know how to calculate them */
			for (j = 0; j < pds.num_pds; j++) {
				if (j == i)
					pds.pds[i].distances[j] = 0x0a;
				else
					pds.pds[i].distances[j] = 0x0e;
			}
		}
	}

	return CB_SUCCESS;
}

/*
 * Return the total size of memory regions in generic initiator affinity domains.
 * The size is in unit of 64MB.
 */
uint32_t get_generic_initiator_mem_size(void)
{
	uint8_t i;
	uint32_t size = 0;

	for (i = 0; i < pds.num_pds; i++) {
		if (pds.pds[i].pd_type == PD_TYPE_PROCESSOR)
			continue;
		size += pds.pds[i].size;
	}

	return size;
}
