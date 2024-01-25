/* SPDX-License-Identifier: GPL-2.0-or-later */

#ifndef _SOC_SMMRELOCATE_H_
#define _SOC_SMMRELOCATE_H_

#include <cpu/x86/smm.h>

void get_smm_info(uintptr_t *perm_smbase, size_t *perm_smsize,
		  size_t *smm_save_state_size);

void soc_ubox_store_resources(struct smm_pci_resource_info *slots, size_t size);

#endif
