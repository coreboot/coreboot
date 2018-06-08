/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2017 Intel Corporation.
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

#ifndef SOC_INTEL_COMMON_BLOCK_SGX_H
#define SOC_INTEL_COMMON_BLOCK_SGX_H

#include <soc/nvs.h>

struct sgx_param {
	uint8_t enable;
};

/*
 * Lock SGX memory.
 * CPU specific code needs to provide the implementation.
 */
void cpu_lock_sgx_memory(void);

/*
 * Configure core PRMRR.
 * PRMRR needs to configured first on all cores and then
 * call sgx_configure() for all cores to init SGX.
 */
void prmrr_core_configure(void);

/*
 * Configure SGX.
 */
void sgx_configure(void *unused);

/* SOC specific API to get SGX params.
 * returns 0, if able to get SGX params; otherwise returns -1 */
int soc_fill_sgx_param(struct sgx_param *sgx_param);

/* Fill GNVS data with SGX status, EPC base and length */
void sgx_fill_gnvs(global_nvs_t *gnvs);

#endif	/* SOC_INTEL_COMMON_BLOCK_SGX_H */
