/* SPDX-License-Identifier: GPL-2.0-only */

#define __SIMPLE_DEVICE__

#include <commonlib/bsd/helpers.h>
#include <device/pci_ops.h>
#include <intelblocks/systemagent_server.h>
#include <soc/pci_devs.h>
#include <soc/systemagent.h>

uintptr_t sa_server_get_tseg_base(void)
{
	struct sa_server_mem_map_descriptor tseg_descriptor = {
		.reg_offset = sa_server_soc_reg_to_pci_offset(TSEG_BASE_REG),
	};

	return sa_server_read_map_entry(SA_DEV_ROOT, &tseg_descriptor);
}

size_t sa_server_get_tseg_size(void)
{
	struct sa_server_mem_map_descriptor tseg_descriptor = {
		.reg_offset = sa_server_soc_reg_to_pci_offset(TSEG_LIMIT_REG),
		.is_limit = true,
	};

	return sa_server_read_map_entry(SA_DEV_ROOT, &tseg_descriptor) -
	       sa_server_get_tseg_base();
}
