/*
 * This file is part of the coreboot project.
 *
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 */

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
