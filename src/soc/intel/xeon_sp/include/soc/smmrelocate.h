/* SPDX-License-Identifier: GPL-2.0-or-later */

#ifndef _SOC_SMMRELOCATE_H_
#define _SOC_SMMRELOCATE_H_

#include <cpu/x86/smm.h>

void soc_ubox_store_resources(struct smm_pci_resource_info *slots, size_t size);

#endif
