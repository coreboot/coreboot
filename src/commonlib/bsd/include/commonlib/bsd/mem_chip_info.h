/* SPDX-License-Identifier: BSD-3-Clause */

#ifndef _COMMONLIB_BSD_MEM_CHIP_INFO_H_
#define _COMMONLIB_BSD_MEM_CHIP_INFO_H_

#include <stddef.h>

enum mem_chip_type {
	MEM_CHIP_DDR3 = 0x30,
	MEM_CHIP_LPDDR3 = 0x38,
	MEM_CHIP_DDR4 = 0x40,
	MEM_CHIP_LPDDR4 = 0x48,
	MEM_CHIP_LPDDR4X = 0x49,
};

struct mem_chip_info {
	uint8_t type;			/* enum mem_chip_type */
	uint8_t num_channels;
	uint8_t reserved[6];
	struct mem_chip_channel {
		uint64_t density;	/* number in _bytes_, not Megabytes! */
		uint8_t io_width;	/* should be `8`, `16`, `32` or `64` */
		uint8_t manufacturer_id; /* raw value from MR5 */
		uint8_t revision_id[2];	/* raw values from MR6 and MR7 */
		uint8_t reserved[4];
		uint8_t serial_id[8];	/* LPDDR5 only, MR47 - MR54 */
	} channel[0];
};

static inline size_t mem_chip_info_size(struct mem_chip_info *info)
{
	return sizeof(*info) + sizeof(info->channel[0]) * info->num_channels;
};

#endif /* _COMMONLIB_BSD_MEM_CHIP_INFO_H_ */
