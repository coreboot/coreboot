/* SPDX-License-Identifier: GPL-2.0-only */
/* This file is part of the coreboot project. */

#ifndef _SOC_NVS_H_
#define _SOC_NVS_H_

#include <stdint.h>

/* TODO - this requires xeon sp, server board support */
/* NOTE: We do not use intelblocks/nvs.h since it includes
   mostly client specific attributes */
typedef struct global_nvs_t {
	uint8_t  pcnt;        /* 0x00 - Processor Count */
	uint32_t cbmc;	      /* 0x01 - coreboot memconsole */
	uint8_t rsvd3[251];
} __packed global_nvs_t;

#endif /* _SOC_NVS_H_ */
