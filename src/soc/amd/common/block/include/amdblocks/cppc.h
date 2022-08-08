/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef AMD_CEZANNE_CPPC_H
#define AMD_CEZANNE_CPPC_H

#include <types.h>
#include <acpi/acpigen.h>

struct cppc_config;
void generate_cppc_entries(unsigned int core_id);
enum cb_err get_ccx_cppc_min_frequency(uint32_t *freq);
enum cb_err get_ccx_cppc_nom_frequency(uint32_t *freq);

#endif /* AMD_CEZANNE_CPPC_H */
