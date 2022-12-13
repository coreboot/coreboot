/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <soc/acpi.h>
#include <soc/numa.h>
#include <soc/util.h>

unsigned long cxl_fill_srat(unsigned long current)
{
	/*
	 * Create Generic Initiator Affinity structure
	 * and Memory Affinity structure for CXL memory.
	 * In the pds (Proximity Domains structure), Generic Initiator domains
	 * are after processor domains.
	 */
	uint16_t seg = 0;
	uint8_t bus, dev, func;
	uint32_t base, size;
	for (uint8_t i = soc_get_num_cpus(); i < pds.num_pds; i++) {
		bus = pds.pds[i].device_handle >> 20;
		dev = (pds.pds[i].device_handle >> 15) & 0x1f;
		func = (pds.pds[i].device_handle >> 12) & 0x07;
		printk(BIOS_DEBUG,
		       "adding srat GIA ID: %d, seg: 0x%x, bus: 0x%x, dev: 0x%x, func: 0x%x\n",
		       i, seg, bus, dev, func);
		/* flags: 1 (enabled) */
		current += acpi_create_srat_gia_pci((acpi_srat_gia_t *)current, i, seg, bus,
						    dev, func, 1);
		base = pds.pds[i].base << 16;
		size = pds.pds[i].size << 16;
		printk(BIOS_DEBUG,
		       "adding srat MEM affinity domain: %d, base: 0x%x, size: 0x%x\n", i, base,
		       size);
		current +=
			acpi_create_srat_mem((acpi_srat_mem_t *)current, i,
					     pds.pds[i].base << 16, pds.pds[i].size << 16, 1);
	}

	return current;
}

/*
 * The current kernel does not use HMAT table.
 */
unsigned long acpi_fill_hmat(unsigned long current)
{
	uint32_t pd_initiator = 0;
	uint32_t pd_memory = 0;

	/* In CXL2.0, CXL memories attached to different sockets could be ganged
	 * to form a single CXL memory region.
	 * For now, we do not consider this case, and assume socket_bitmap has
	 * only one bit set, eg. a CXL memory region is attached to one socket.
	 */
	uint8_t j;
	for (uint8_t i = soc_get_num_cpus(); i < pds.num_pds; i++) {
		pd_memory = i;
		/* check socket_bitmap which is type uint8_t */
		for (j = 0; j < 8; j++)
			if ((pds.pds[i].socket_bitmap >> j) == 0)
				break;
		pd_initiator = j - 1;
		printk(BIOS_DEBUG, "HMAT: pd_initiator = %d, pd_memory = %d\n", pd_initiator,
		       pd_memory);
		current += acpi_create_hmat_mpda((acpi_hmat_mpda_t *)current, pd_initiator,
						 pd_memory);
	}

	/*
	 * We created only MPDA structure. In future, we could create
	 * SLLBI structure to describe latency/bandwidth info when such info
	 * is available.
	 */

	return current;
}
