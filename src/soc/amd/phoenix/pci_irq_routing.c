/* SPDX-License-Identifier: GPL-2.0-only */

#include <amdblocks/amd_pci_util.h>
#include <console/console.h>
#include <types.h>

const struct pci_routing_info *get_pci_routing_table(size_t *entries)
{
	/* TODO: still needs to be implemented for the non-FSP case */
	printk(BIOS_NOTICE, "%s stub: returning empty IRQ routing table\n", __func__);

	*entries = 0;
	return NULL;
}
