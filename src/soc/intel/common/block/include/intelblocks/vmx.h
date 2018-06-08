/*
 * This file is part of the coreboot project.
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

#ifndef SOC_INTEL_COMMON_BLOCK_VMX_H
#define SOC_INTEL_COMMON_BLOCK_VMX_H

struct vmx_param {
	uint8_t enable;
};

/*
 * Configure VMX.
 */
void vmx_configure(void *unused);

/* SOC specific API to get VMX params.
 * returns 0, if able to get VMX params; otherwise returns -1 */
int soc_fill_vmx_param(struct vmx_param *vmx_param);

#endif	/* SOC_INTEL_COMMON_BLOCK_VMX_H */
