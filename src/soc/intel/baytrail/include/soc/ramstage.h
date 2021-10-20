/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef _BAYTRAIL_RAMSTAGE_H_
#define _BAYTRAIL_RAMSTAGE_H_

#include <device/device.h>
#include <soc/intel/baytrail/chip.h>

/* The baytrail_init_pre_device() function is called prior to device
 * initialization, but it's after console and cbmem has been reinitialized. */
void baytrail_init_pre_device(struct soc_intel_baytrail_config *config);
void southcluster_enable_dev(struct device *dev);
/* The baytrail_run_reference_code() function is provided by either
 * native refcode or the ChromeBook refcode wrapper */
void baytrail_run_reference_code(void);
void baytrail_init_scc(void);
void scc_enable_acpi_mode(struct device *dev, int iosf_reg, int nvs_index);

extern struct pci_operations soc_pci_ops;

#endif /* _BAYTRAIL_RAMSTAGE_H_ */
