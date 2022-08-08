/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef SABRINA_FSP_CCX_CPPC_DATA_H
#define SABRINA_FSP_CCX_CPPC_DATA_H

#include <types.h>

#define FSP_CCX_CPPC_DATA_VERSION	1

struct fsp_ccx_cppc_data {
	uint8_t version;
	uint8_t unused[3];
	uint32_t ccx_cppc_min_speed;
	uint32_t ccx_cppc_nom_speed;
} __packed;

#endif /* SABRINA_FSP_CCX_CPPC_DATA_H */
