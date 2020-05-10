/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef LBTABLE_H
#define LBTABLE_H

#include "common.h"

void get_lbtable(void);
void get_layout_from_cmos_table(void);
void get_layout_from_cbfs_file(void);
void dump_lbtable(void);
void list_lbtable_choices(void);
void list_lbtable_item(const char item[]);
const struct lb_record *find_lbrec(uint32_t tag);

void process_layout(void);
#endif				/* LBTABLE_H */
