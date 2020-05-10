/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef INPUT_FILE_H
#define INPUT_FILE_H

#include "common.h"
#include "layout.h"

typedef struct cmos_write_t cmos_write_t;

/* This represents a pending CMOS write operation.  When changing
 * multiple CMOS parameter values, we first represent the changes as a
 * list of pending write operations.  This allows us to sanity check all
 * write operations before any of them are performed.
 */
struct cmos_write_t {
	unsigned bit;
	unsigned length;
	cmos_entry_config_t config;
	unsigned long long value;
	cmos_write_t *next;
};

cmos_write_t *process_input_file(FILE * f);
void do_cmos_writes(cmos_write_t * list);

extern const char assignment_regex[];

#endif				/* INPUT_FILE_H */
