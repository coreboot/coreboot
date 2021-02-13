/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef AMD_BLOCK_DATA_FABRIC_H
#define AMD_BLOCK_DATA_FABRIC_H

#include <stdint.h>

#define BROADCAST_FABRIC_ID		0xff

uint32_t data_fabric_read32(uint8_t function, uint16_t reg, uint8_t instance_id);

#endif /* AMD_BLOCK_DATA_FABRIC_H */
