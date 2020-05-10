/*****************************************************************************\
 * cmos_ops.h
 *****************************************************************************
 *  Please also read the file DISCLAIMER which is included in this software
 *  distribution.
 *
 *  This program is free software; you can redistribute it and/or modify it
 *  under the terms of the GNU General Public License (as published by the
 *  Free Software Foundation) version 2, dated June 1991.
 *
 *  This program is distributed in the hope that it will be useful, but
 *  WITHOUT ANY WARRANTY; without even the IMPLIED WARRANTY OF
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the terms and
 *  conditions of the GNU General Public License for more details.
\*****************************************************************************/

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
