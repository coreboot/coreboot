/* SPDX-License-Identifier: BSD-2-Clause */

#ifndef _STM_PLATFORM_RESOURCE_H_
#define _STM_PLATFORM_RESOURCE_H_

#define MASK0 0
#define MASK64 0xFFFFFFFFFFFFFFFFull

// LPC

#define LPC_DEVICE 31
#define LPC_FUNCTION 0
#define R_ACPI_PM_BASE 0x40
#define ACPI_PM_BASE_MASK 0xFFF8

/*
 *  Add resources to BIOS resource database.
 */
void add_resources_cmd(void);
#endif
