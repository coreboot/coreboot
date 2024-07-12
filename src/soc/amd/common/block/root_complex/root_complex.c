/* SPDX-License-Identifier: GPL-2.0-only */

#include <amdblocks/root_complex.h>
#include <console/console.h>
#include <device/device.h>
#include <types.h>

static const struct domain_iohc_info *get_domain_iohc_info(struct device *domain)
{
	const unsigned int domain_id = domain->path.domain.domain;
	const struct domain_iohc_info *iohc;
	size_t iohc_count;

	iohc = get_iohc_info(&iohc_count);

	if (domain_id < iohc_count) {
		return &iohc[domain_id];
	} else {
		printk(BIOS_ERR, "Invalid domain 0x%x with no corresponding IOHC device.\n",
		       domain_id);
		return NULL;
	}
}

uint32_t get_iohc_misc_smn_base(struct device *domain)
{
	const struct domain_iohc_info *iohc_info = get_domain_iohc_info(domain);

	if (iohc_info)
		return iohc_info->misc_smn_base;
	else
		return 0;
}

signed int get_iohc_fabric_id(struct device *domain)
{
	const struct domain_iohc_info *iohc_info = get_domain_iohc_info(domain);

	if (iohc_info)
		return iohc_info->fabric_id;
	else
		return -1;
}
