/* SPDX-License-Identifier: BSD-3-Clause */

#ifndef _COMMONLIB_BSD_ELOG_H_
#define _COMMONLIB_BSD_ELOG_H_

#include <inttypes.h>

#include <commonlib/bsd/cb_err.h>

/* ELOG header */
struct elog_header {
	uint32_t magic;
	uint8_t version;
	uint8_t header_size;
	uint8_t reserved[2];
} __packed;

/* ELOG related constants */
#define ELOG_SIGNATURE			0x474f4c45  /* 'ELOG' */
#define ELOG_VERSION			1

/* SMBIOS event log header */
struct event_header {
	uint8_t type;
	uint8_t length;
	uint8_t year;
	uint8_t month;
	uint8_t day;
	uint8_t hour;
	uint8_t minute;
	uint8_t second;
} __packed;

/* SMBIOS Type 15 related constants */
#define ELOG_HEADER_TYPE_OEM		0x88

enum cb_err elog_verify_header(const struct elog_header *header);

#endif /* _COMMONLIB_BSD_ELOG_H_ */
