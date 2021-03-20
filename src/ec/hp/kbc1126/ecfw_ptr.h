/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef _EC_HP_KBC1126_PTR_H
#define _EC_HP_KBC1126_PTR_H

#include <stdint.h>

struct __attribute__((__packed__)) ecfw_addr {
	/* 8-byte offset of firmware blob in big endian */
	uint16_t off;
	/* bitwise inverse of "off", for error checking */
	uint16_t inv;
};

struct __attribute__((__packed__)) ecfw_ptr {
	struct ecfw_addr fw1;
	struct ecfw_addr fw2;
};

#endif
