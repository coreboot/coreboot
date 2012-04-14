/*****************************************************************************\
 * layout.h
 *****************************************************************************
 *  Copyright (C) 2002-2005 The Regents of the University of California.
 *  Produced at the Lawrence Livermore National Laboratory.
 *  Written by Dave Peterson <dsp@llnl.gov> <dave_peterson@pobox.com>.
 *  UCRL-CODE-2003-012
 *  All rights reserved.
 *
 *  This file is part of nvramtool, a utility for reading/writing coreboot
 *  parameters and displaying information from the coreboot table.
 *  For details, see http://coreboot.org/nvramtool.
 *
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
 *
 *  You should have received a copy of the GNU General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA.
\*****************************************************************************/

#ifndef LAYOUT_H
#define LAYOUT_H

#include "common.h"
#include "coreboot_tables.h"

#define LAYOUT_ENTRY_OVERLAP (LAYOUT_RESULT_START + 0)
#define LAYOUT_ENTRY_BAD_LENGTH (LAYOUT_RESULT_START + 1)
#define LAYOUT_DUPLICATE_ENUM (LAYOUT_RESULT_START + 2)
#define LAYOUT_SUMMED_AREA_START_NOT_ALIGNED (LAYOUT_RESULT_START + 3)
#define LAYOUT_SUMMED_AREA_END_NOT_ALIGNED (LAYOUT_RESULT_START + 4)
#define LAYOUT_CHECKSUM_LOCATION_NOT_ALIGNED (LAYOUT_RESULT_START + 5)
#define LAYOUT_INVALID_SUMMED_AREA (LAYOUT_RESULT_START + 6)
#define LAYOUT_CHECKSUM_OVERLAPS_SUMMED_AREA (LAYOUT_RESULT_START + 7)
#define LAYOUT_SUMMED_AREA_OUT_OF_RANGE (LAYOUT_RESULT_START + 8)
#define LAYOUT_CHECKSUM_LOCATION_OUT_OF_RANGE (LAYOUT_RESULT_START + 9)

typedef enum {
	CMOS_ENTRY_ENUM = 'e',
	CMOS_ENTRY_HEX = 'h',
	CMOS_ENTRY_STRING = 's',
	CMOS_ENTRY_RESERVED = 'r',
} cmos_entry_config_t;

/* This represents a CMOS parameter. */
typedef struct {
	unsigned bit;
	unsigned length;
	cmos_entry_config_t config;
	unsigned config_id;
	char name[CMOS_MAX_NAME_LENGTH + 1];
} cmos_entry_t;

/* This represents a possible value for a CMOS parameter of type
 * CMOS_ENTRY_ENUM.
 */
typedef struct {
	unsigned config_id;
	unsigned long long value;
	char text[CMOS_MAX_TEXT_LENGTH + 1];
} cmos_enum_t;

/* This represents the location of the CMOS checksum and the area over
 * which it is computed.  Depending on the context, the values may be
 * represented as either bit positions or byte positions.
 */
typedef struct {
	unsigned summed_area_start;	/* first checksummed location */
	unsigned summed_area_end;	/* last checksummed location */
	unsigned checksum_at;	/* location of checksum */
} cmos_checksum_layout_t;

extern const char checksum_param_name[];

extern unsigned cmos_checksum_start;

extern unsigned cmos_checksum_end;

extern unsigned cmos_checksum_index;

typedef void (*cmos_layout_get_fn_t) (void);

void register_cmos_layout_get_fn(cmos_layout_get_fn_t fn);
void get_cmos_layout(void);
int add_cmos_entry(const cmos_entry_t * e, const cmos_entry_t ** conflict);
const cmos_entry_t *find_cmos_entry(const char name[]);
const cmos_entry_t *first_cmos_entry(void);
const cmos_entry_t *next_cmos_entry(const cmos_entry_t * last);
int add_cmos_enum(const cmos_enum_t * e);
const cmos_enum_t *find_cmos_enum(unsigned config_id, unsigned long long value);
const cmos_enum_t *first_cmos_enum(void);
const cmos_enum_t *next_cmos_enum(const cmos_enum_t * last);
const cmos_enum_t *first_cmos_enum_id(unsigned config_id);
const cmos_enum_t *next_cmos_enum_id(const cmos_enum_t * last);
int is_checksum_name(const char name[]);
int checksum_layout_to_bytes(cmos_checksum_layout_t * layout);
void checksum_layout_to_bits(cmos_checksum_layout_t * layout);

#endif				/* LAYOUT_H */
