/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef _SOC_SNOWRIDGE_RAMSTAGE_H_
#define _SOC_SNOWRIDGE_RAMSTAGE_H_

#include <device/device.h>
#include <device/pci.h>
#include <southbridge/intel/common/acpi_pirq_gen.h>

void pciexp_pf_read_resources(struct device *dev);
void pciexp_vf_read_resources(struct device *dev);

enum pci_pin pciexp_get_rp_irq_pin(struct device *bridge, enum pci_pin pin);

#endif // _SOC_SNOWRIDGE_RAMSTAGE_H_
