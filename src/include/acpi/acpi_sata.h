/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef __ACPI_SATA_H__
#define __ACPI_SATA_H__

#include <stdint.h>

void generate_sata_ssdt_ports(const char *scope, uint32_t enable_map);

#endif
