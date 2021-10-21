/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef _DENVERTON_NS_SOC_RAMSTAGE_H_
#define _DENVERTON_NS_SOC_RAMSTAGE_H_

#include <device/device.h>
#include <fsp/api.h>
#include <fsp/util.h>
#include <memory_info.h>

void mainboard_silicon_init_params(FSPS_UPD *params);
void southcluster_enable_dev(struct device *dev);
void mainboard_add_dimm_info(struct memory_info *mem_info, int channel,
			     int dimm, int index);

extern struct pci_operations soc_pci_ops;

#endif
