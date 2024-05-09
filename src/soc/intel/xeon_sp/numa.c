/* SPDX-License-Identifier: GPL-2.0-only */

#include <console/console.h>
#include <device/device.h>
#include <device/pci_ops.h>
#include <device/pci.h>
#include <device/pciexp.h>
#include <soc/chip_common.h>
#include <soc/numa.h>
#include <soc/soc_util.h>
#include <soc/util.h>
#include <types.h>

static void dump_pds(void)
{
	printk(BIOS_DEBUG, "====== Proximity Domain Dump ======\n");
	printk(BIOS_DEBUG, "number of proximity domains: %d\n", pds.num_pds);
	for (uint8_t i = 0; i < pds.num_pds; i++) {
		printk(BIOS_DEBUG, "\tproximity domain %d:\n", i);
		printk(BIOS_DEBUG, "\t\ttype:%d\n", pds.pds[i].pd_type);
		printk(BIOS_DEBUG, "\t\tsocket_bitmap:0x%x\n", pds.pds[i].socket_bitmap);
		printk(BIOS_DEBUG, "\t\tdevice:%s\n", pds.pds[i].dev ? dev_path(pds.pds[i].dev) : "");
		printk(BIOS_DEBUG, "\t\tbase(64MB):0x%x\n", pds.pds[i].base);
		printk(BIOS_DEBUG, "\t\tsize(64MB):0x%x\n", pds.pds[i].size);
	}
}

static void fill_pds(void)
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
	uint8_t i = 0;
	for (uint8_t socket = 0; socket < num_sockets; socket++) {
		if (!soc_cpu_is_enabled(socket))
			continue;
		pds.pds[i].pd_type = PD_TYPE_PROCESSOR;
		pds.pds[i].socket_bitmap = 1 << hob->PlatformData.IIO_resource[socket].SocketID;
		pds.pds[i].distances = malloc(sizeof(uint8_t) * pds.num_pds);
		if (!pds.pds[i].distances)
			die("%s %d out of memory.", __FILE__, __LINE__);
		i++;
	}

	/* If there are no CXL nodes, we are done */
	if (num_cxlnodes == 0)
		return;

#if CONFIG(SOC_INTEL_HAS_CXL)
	/* There are CXL nodes, fill in generic initiator domain after the processors pds */
	const CXL_NODE_SOCKET *cxl_hob = get_cxl_node();
	for (uint8_t skt_id = 0; skt_id < MAX_SOCKET; skt_id++) {
		for (uint8_t cxl_id = 0; cxl_id < cxl_hob[skt_id].CxlNodeCount; ++cxl_id) {
			const CXL_NODE_INFO node = cxl_hob[skt_id].CxlNodeInfo[cxl_id];
			pds.pds[i].pd_type = PD_TYPE_GENERIC_INITIATOR;
			pds.pds[i].socket_bitmap = node.SocketBitmap;
			pds.pds[i].base = node.Address;
			pds.pds[i].size = node.Size;
			struct device *dev = pcie_find_dsn(node.SerialNumber, node.VendorId, 0);
			pds.pds[i].dev = dev;
			pds.pds[i].distances = malloc(sizeof(uint8_t) * pds.num_pds);
			if (!pds.pds[i].distances)
				die("%s %d out of memory.", __FILE__, __LINE__);
			i++;
		}
	}
#endif
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

static uint32_t socket_to_pd(uint8_t socket)
{
	for (uint8_t i = 0; i < pds.num_pds; i++) {
		if (pds.pds[i].pd_type != PD_TYPE_PROCESSOR)
			continue;
		if (pds.pds[i].socket_bitmap == (1 << socket))
			return i;
	}

	printk(BIOS_ERR, "%s: could not find proximity domain for socket %d.\n",
		__func__, socket);

	return XEONSP_INVALID_PD_INDEX;
}

uint32_t device_to_pd(const struct device *dev)
{
	/* first to see if the dev is bound to specific pd */
	for (int i = 0; i < pds.num_pds; i++)
		if (pds.pds[i].dev == dev)
			return i;

	if (dev->path.type == DEVICE_PATH_APIC)
		return socket_to_pd(dev->path.apic.package_id);

	if ((dev->path.type == DEVICE_PATH_DOMAIN) ||
		(dev->path.type == DEVICE_PATH_PCI))
		return socket_to_pd(iio_pci_domain_socket_from_dev(dev));

	printk(BIOS_ERR, "%s: could not find proximity domain for device %s.\n",
		__func__, dev_path(dev));

	return XEONSP_INVALID_PD_INDEX;
}

uint32_t memory_to_pd(const struct SystemMemoryMapElement *mem)
{
	return socket_to_pd(mem->SocketId);
}

#define PD_DISTANCE_SELF                0x0A
#define PD_DISTANCE_SAME_SOCKET         0x0C
#define PD_DISTANCE_CROSS_SOCKET        0x14
#define PD_DISTANCE_MAX                 0xFF
#define PD_DISTANCE_IO_EXTRA            0x01

static void fill_pd_distances(void)
{
	for (int i = 0; i < pds.num_pds; i++) {
		for (int j = 0; j < pds.num_pds; j++) {
			if (i == j) {
				pds.pds[i].distances[j] = PD_DISTANCE_SELF;
				continue;
			}

			if (pds.pds[i].socket_bitmap == pds.pds[j].socket_bitmap)
				pds.pds[i].distances[j] = PD_DISTANCE_SAME_SOCKET;
			else
				pds.pds[i].distances[j] = PD_DISTANCE_CROSS_SOCKET;

			if (pds.pds[i].pd_type == PD_TYPE_GENERIC_INITIATOR)
				pds.pds[i].distances[j] += PD_DISTANCE_IO_EXTRA;

			if (pds.pds[j].pd_type == PD_TYPE_GENERIC_INITIATOR)
				pds.pds[i].distances[j] += PD_DISTANCE_IO_EXTRA;
		}
	}
}

void setup_pds(void)
{
	fill_pds();
	fill_pd_distances();
	dump_pds();
}
