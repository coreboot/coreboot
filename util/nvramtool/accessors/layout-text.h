/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef LAYOUT_FILE_H
#define LAYOUT_FILE_H

#include "common.h"
#include "coreboot_tables.h"

void set_layout_filename(const char filename[]);
void get_layout_from_file(void);
void write_cmos_layout(FILE * f);
void write_cmos_output_bin(const char *binary_filename);
void write_cmos_layout_header(const char *header_filename);
extern int is_ident(char *str);

#endif				/* LAYOUT_FILE_H */
