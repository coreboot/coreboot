/* SPDX-License-Identifier: GPL-2.0-only */
/* This file is part of the coreboot project. */

#ifndef INTEL_COMMON_PMCLIB_H
#define INTEL_COMMON_PMCLIB_H

/*
 * Returns 1 if platform was in ACPI S3 power state and CONFIG(HAVE_ACPI_RESUME)
 * is enabled else returns 0.
 */
int southbridge_detect_s3_resume(void);

#endif
