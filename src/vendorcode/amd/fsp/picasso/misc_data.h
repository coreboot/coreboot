/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef __PI_PICASSO_MISC_DATA_H__
#define __PI_PICASSO_MISC_DATA_H__

#define PICASSO_MISC_DATA_VERSION	1

struct picasso_misc_data {
	uint8_t version;
	uint8_t unused[3];
	uint32_t silicon_id;
} __packed;

#endif /* __PI_PICASSO_MISC_DATA_H__ */
