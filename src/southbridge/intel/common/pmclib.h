/*
 * This file is part of the coreboot project.
 *
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; version 2 of
 * the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#ifndef INTEL_COMMON_PMCLIB_H
#define INTEL_COMMON_PMCLIB_H

/*
 * Returns 1 if platform was in ACPI S3 power state and CONFIG(HAVE_ACPI_RESUME)
 * is enabled else returns 0.
 */
int southbridge_detect_s3_resume(void);

#endif
