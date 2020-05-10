/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef _BAYTRAIL_RAMSTAGE_H_
#define _BAYTRAIL_RAMSTAGE_H_

#include <device/device.h>
#include <soc/intel/baytrail/chip.h>

/* The baytrail_init_pre_device() function is called prior to device
 * initialization, but it's after console and cbmem has been reinitialized. */
void baytrail_init_pre_device(struct soc_intel_baytrail_config *config);
void baytrail_init_cpus(struct device *dev);
void southcluster_enable_dev(struct device *dev);
#if CONFIG(HAVE_REFCODE_BLOB)
void baytrail_run_reference_code(void);
#else
static inline void baytrail_run_reference_code(void) {}
#endif
void baytrail_init_scc(void);
void scc_enable_acpi_mode(struct device *dev, int iosf_reg, int nvs_index);

extern struct pci_operations soc_pci_ops;

#endif /* _BAYTRAIL_RAMSTAGE_H_ */
