/* SPDX-License-Identifier: GPL-2.0-or-later */

#ifndef _SOC_RAMSTAGE_H_
#define _SOC_RAMSTAGE_H_

#include <device/device.h>
#include <fsp/api.h>
#include <fsp/util.h>
#include <memory_info.h>

void xeon_sp_init_cpus(struct device *dev);
void mainboard_silicon_init_params(FSPS_UPD *params);

extern struct pci_operations soc_pci_ops;

#endif
