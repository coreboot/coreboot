/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef _SOC_NVS_H_
#define _SOC_NVS_H_

#include <stdint.h>

/* TODO - this requires xeon sp, server board support */
/* NOTE: We do not use intelblocks/nvs.h since it includes
   mostly client specific attributes */

/* TODO: This is not aligned with the ACPI asl code */
struct __packed global_nvs {
	uint8_t  pcnt;        /* 0x00 - Processor Count */
	uint32_t cbmc;	      /* 0x01 - coreboot memconsole */
	uint8_t uior;
	uint8_t rsvd3[250];
};

#endif /* _SOC_NVS_H_ */
