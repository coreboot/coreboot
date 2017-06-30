/*
 * This file is part of the coreboot project.
 *
 * Copyright 2016 Google Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#ifndef _EC_GOOGLE_CHROMEEC_SMM_H
#define _EC_GOOGLE_CHROMEEC_SMM_H

#include <stdint.h>

/* Process all events from the EC when EC triggered an SMI#. */
void chromeec_smi_process_events(void);

/*
 * Set wake masks according to sleep type, clear SCI and SMI masks,
 * and clear any pending events.
 */
void chromeec_smi_sleep(int slp_type, uint32_t s3_mask, uint32_t s5_mask);

/*
 * Set device event masks according to sleep type,
 * and clear any pending device events.
 */
void chromeec_smi_device_event_sleep(int slp_type, uint32_t s3_mask,
				     uint32_t s5_mask);

/*
 * Provided the APMC command do the following while clearing pending events.
 * APM_CNT_ACPI_ENABLE: clear SMI mask. set SCI mask.
 * APM_CNT_ACPI_DISABLE: clear SCI mask. set SMI mask.
 */
void chromeec_smi_apmc(int apmc, uint32_t sci_mask, uint32_t smi_mask);

#endif /* _EC_GOOGLE_CHROMEEC_SMM_H */
