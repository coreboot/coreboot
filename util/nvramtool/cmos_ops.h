/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef CMOS_OPS_H
#define CMOS_OPS_H

#include "common.h"
#include "layout.h"

#define CMOS_OP_BAD_ENUM_VALUE (CMOS_OP_RESULT_START + 0)
#define CMOS_OP_NEGATIVE_INT (CMOS_OP_RESULT_START + 1)
#define CMOS_OP_INVALID_INT (CMOS_OP_RESULT_START + 2)
#define CMOS_OP_RESERVED (CMOS_OP_RESULT_START + 3)
#define CMOS_OP_VALUE_TOO_WIDE (CMOS_OP_RESULT_START + 4)
#define CMOS_OP_NO_MATCHING_ENUM (CMOS_OP_RESULT_START + 5)

int prepare_cmos_read(const cmos_entry_t * e);
int prepare_cmos_write(const cmos_entry_t * e, const char value_str[],
		       unsigned long long *value);
uint16_t cmos_checksum_read(void);
void cmos_checksum_write(uint16_t checksum);
uint16_t cmos_checksum_compute(void);
void cmos_checksum_verify(void);

#endif				/* CMOS_OPS_H */
