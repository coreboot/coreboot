/* SPDX-License-Identifier: GPL-2.0-only */
/* This file is part of the coreboot project. */

#ifndef _EC_GOOGLE_CHROMEEC_SMM_H
#define _EC_GOOGLE_CHROMEEC_SMM_H

#include <stdint.h>

/* Process all events from the EC when EC triggered an SMI#. */
void chromeec_smi_process_events(void);

/*
 * Set wake masks according to sleep type, clear SCI and SMI masks,
 * and clear any pending events.
 */
void chromeec_smi_sleep(int slp_type, uint64_t s3_mask, uint64_t s5_mask);

/*
 * Set device event masks according to sleep type,
 * and clear any pending device events.
 */
void chromeec_smi_device_event_sleep(int slp_type, uint64_t s3_mask,
				     uint64_t s5_mask);

/*
 * Provided the APMC command do the following while clearing pending events.
 * APM_CNT_ACPI_ENABLE: clear SMI mask. set SCI mask.
 * APM_CNT_ACPI_DISABLE: clear SCI mask. set SMI mask.
 */
void chromeec_smi_apmc(int apmc, uint64_t sci_mask, uint64_t smi_mask);

#endif /* _EC_GOOGLE_CHROMEEC_SMM_H */
