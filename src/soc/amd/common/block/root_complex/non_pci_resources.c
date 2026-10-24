/* SPDX-License-Identifier: GPL-2.0-only */

#include <amdblocks/root_complex.h>
#include <amdblocks/smn.h>
#include <console/console.h>
#include <device/device.h>
#include <types.h>

void read_non_pci_resources(struct device *domain, unsigned long *idx)
{
	const struct domain_iohc_info *iohc = get_domain_iohc_info(domain);
	const struct non_pci_mmio_reg *regs;
	size_t reg_count;

	if (!iohc) {
		printk(BIOS_ERR, "No IOHC resource endpoint for domain %u\n",
		       dev_get_domain_id(domain));
		return;
	}

	regs = get_iohc_non_pci_mmio_regs(&reg_count);

	for (size_t i = 0; i < reg_count; i++) {
		const uint64_t reg64 = smn_read64_at(iohc->smn_dev,
						    iohc->misc_smn_base + regs[i].iohc_misc_offset);
		/* only report enabled non-PCI MMIO regions */
		if (!(reg64 & IOHC_MMIO_EN))
			continue;

		const unsigned long res_idx = regs[i].res_idx == NON_PCI_RES_IDX_AUTO ?
						(*idx)++ : regs[i].res_idx;
		const uint64_t base = reg64 & regs[i].mask;
		mmio_range(domain, res_idx, base, regs[i].size);
	}
}
