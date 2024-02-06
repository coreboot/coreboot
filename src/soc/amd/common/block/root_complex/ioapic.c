/* SPDX-License-Identifier: GPL-2.0-only */

#include <amdblocks/ioapic.h>
#include <amdblocks/root_complex.h>
#include <arch/ioapic.h>
#include <device/device.h>
#include <device/resource.h>
#include <types.h>

void amd_pci_domain_init(struct device *domain)
{
	struct resource *res = probe_resource(domain, IOMMU_IOAPIC_IDX);
	if (!res)
		return;

	register_new_ioapic((uintptr_t)res->base);
}
