/* SPDX-License-Identifier: GPL-2.0-or-later */

#ifndef EC_USB_PD_FW__UTILS_H__
#define EC_USB_PD_FW__UTILS_H__

#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>

struct mem_range {
	uint8_t *start;
	size_t length;
};
uint8_t crc8_itu(const uint8_t *data, int len);

struct mem_range map_file(const char path[], bool rw);

void unmap_file(struct mem_range store);

#endif // EC_USB_PD_FW__UTILS_H__
