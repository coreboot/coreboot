/*
 * This file is part of the coreboot project.
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

#ifndef _CPU_INTEL_COMMON_H
#define _CPU_INTEL_COMMON_H

void set_vmx(void);

/*
 * Init CPPC block with MSRs for Intel Enhanced Speed Step Technology.
 * Version 2 is suggested--this function's implementation of version 3
 * may have room for improvment.
 */
struct cppc_config;
void cpu_init_cppc_config(struct cppc_config *config, u32 version);

#endif
