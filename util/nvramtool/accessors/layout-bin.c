/*****************************************************************************\
 * lbtable.c
 *****************************************************************************
 *  Copyright (C) 2012, Vikram Narayanan
 *  	Unified build_opt_tbl and nvramtool
 *  	build_opt_tbl.c
 *  	Copyright (C) 2003 Eric Biederman (ebiederm@xmission.com)
 *  	Copyright (C) 2007-2010 coresystems GmbH
 *
 *  Copyright (C) 2002-2005 The Regents of the University of California.
 *  Produced at the Lawrence Livermore National Laboratory.
 *  Written by Dave Peterson <dsp@llnl.gov> <dave_peterson@pobox.com>
 *  and Stefan Reinauer <stepan@openbios.org>.
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

#include <string.h>
#ifndef __MINGW32__
#include <sys/mman.h>
#endif
#include "common.h"
#include "coreboot_tables.h"
#include "ip_checksum.h"
#include "lbtable.h"
#include "layout.h"
#include "cmos_lowlevel.h"
#include "hexdump.h"
#include "cbfs.h"
#include "layout-text.h"

static void process_cmos_table(void);
static void get_cmos_checksum_info(void);
static void try_convert_checksum_layout(cmos_checksum_layout_t * layout);
static void try_add_cmos_table_enum(cmos_enum_t * cmos_enum);
static void try_add_cmos_table_entry(cmos_entry_t * cmos_entry);
static const struct cmos_entries *first_cmos_table_entry(void);
static const struct cmos_entries *next_cmos_table_entry(const struct
							cmos_entries *last);
static const struct cmos_enums *first_cmos_table_enum(void);
static const struct cmos_enums *next_cmos_table_enum
    (const struct cmos_enums *last);
static const struct lb_record *first_cmos_rec(uint32_t tag);
static const struct lb_record *next_cmos_rec(const struct lb_record *last,
					     uint32_t tag);

/* The CMOS option table is located within the coreboot table.  It tells us
 * where the CMOS parameters are located in the nonvolatile RAM.
 */
static const struct cmos_option_table *cmos_table = NULL;

#define ROUNDUP4(x)	(x += (4 - (x % 4)))

void process_layout(void)
{
	if ((cmos_table) == NULL) {
		fprintf(stderr,
			"%s: CMOS option table not found in coreboot table.  "
			"Apparently, the coreboot installed on this system was "
			"built without specifying CONFIG_HAVE_OPTION_TABLE.\n",
			prog_name);
		exit(1);
	}

	process_cmos_table();
	get_cmos_checksum_info();
}

void get_layout_from_cbfs_file(void)
{
	uint32_t len;
	cmos_table = cbfs_find_file("cmos_layout.bin", CBFS_COMPONENT_CMOS_LAYOUT, &len);
	process_layout();
}

int write_cmos_layout_bin(FILE *f)
{
	const cmos_entry_t *cmos_entry;
	const cmos_enum_t *cmos_enum;
	cmos_checksum_layout_t layout;
	struct cmos_option_table table;
	struct cmos_entries entry;
	struct cmos_enums cenum;
	struct cmos_checksum csum;
	size_t sum = 0;
	int len;

	for (cmos_entry = first_cmos_entry(); cmos_entry != NULL;
			cmos_entry = next_cmos_entry(cmos_entry)) {

		if (cmos_entry == first_cmos_entry()) {
			sum += sizeof(table);
			table.header_length = sizeof(table);
			table.tag = LB_TAG_CMOS_OPTION_TABLE;

			if (fwrite((char *)&table, sizeof(table), 1, f) != 1) {
				perror("Error writing image file");
				goto err;
			}
		}

		memset(&entry, 0, sizeof(entry));
		entry.tag = LB_TAG_OPTION;
		entry.config = cmos_entry->config;
		entry.config_id = (uint32_t)cmos_entry->config_id;
		entry.bit = cmos_entry->bit;
		entry.length = cmos_entry->length;

		if (!is_ident((char *)cmos_entry->name)) {
			fprintf(stderr,
				"Error - Name %s is an invalid identifier\n",
				cmos_entry->name);
			goto err;
		}

		memcpy(entry.name, cmos_entry->name, strlen(cmos_entry->name));
		entry.name[strlen(cmos_entry->name)] = '\0';
		len = strlen(cmos_entry->name) + 1;

		if (len % 4)
			ROUNDUP4(len);

		entry.size = sizeof(entry) - CMOS_MAX_NAME_LENGTH + len;
		sum += entry.size;
		if (fwrite((char *)&entry, entry.size, 1, f) != 1) {
			perror("Error writing image file");
			goto err;
		}
	}

	for (cmos_enum = first_cmos_enum();
			cmos_enum != NULL; cmos_enum = next_cmos_enum(cmos_enum)) {
		memset(&cenum, 0, sizeof(cenum));
		cenum.tag = LB_TAG_OPTION_ENUM;
		memcpy(cenum.text, cmos_enum->text, strlen(cmos_enum->text));
		cenum.text[strlen(cmos_enum->text)] = '\0';
		len = strlen((char *)cenum.text) + 1;

		if (len % 4)
			ROUNDUP4(len);

		cenum.config_id = cmos_enum->config_id;
		cenum.value = cmos_enum->value;
		cenum.size = sizeof(cenum) - CMOS_MAX_TEXT_LENGTH + len;
		sum += cenum.size;
		if (fwrite((char *)&cenum, cenum.size, 1, f) != 1) {
			perror("Error writing image file");
			goto err;
		}
	}

	layout.summed_area_start = cmos_checksum_start;
	layout.summed_area_end = cmos_checksum_end;
	layout.checksum_at = cmos_checksum_index;
	checksum_layout_to_bits(&layout);

	csum.tag = LB_TAG_OPTION_CHECKSUM;
	csum.size = sizeof(csum);
	csum.range_start = layout.summed_area_start;
	csum.range_end = layout.summed_area_end;
	csum.location = layout.checksum_at;
	csum.type = CHECKSUM_PCBIOS;
	sum += csum.size;

	if (fwrite((char *)&csum, csum.size, 1, f) != 1) {
		perror("Error writing image file");
		goto err;
	}

	if (fseek(f, sizeof(table.tag), SEEK_SET) != 0) {
		perror("Error while seeking");
		goto err;
	}

	if (fwrite((char *)&sum, sizeof(table.tag), 1, f) != 1) {
		perror("Error writing image file");
		goto err;
	}
	return sum;

err:
	fclose(f);
	exit(1);
}

void write_cmos_output_bin(const char *binary_filename)
{
	FILE *fp;

	if ((fp = fopen(binary_filename, "wb")) == NULL) {
		fprintf(stderr,
			"%s: Can not open file %s for writing: "
			"%s\n", prog_name, binary_filename, strerror(errno));
		exit(1);
	}
	write_cmos_layout_bin(fp);
	fclose(fp);
}

/****************************************************************************
 * get_layout_from_cmos_table
 *
 * Find the CMOS table which is stored within the coreboot table and set the
 * global variable cmos_table to point to it.
 ****************************************************************************/
void get_layout_from_cmos_table(void)
{
	get_lbtable();
	cmos_table = (const struct cmos_option_table *)
	    find_lbrec(LB_TAG_CMOS_OPTION_TABLE);
	process_layout();
}

/****************************************************************************
 * process_cmos_table
 *
 * Extract layout information from the CMOS option table and store it in our
 * internal repository.
 ****************************************************************************/
static void process_cmos_table(void)
{
	const struct cmos_enums *p;
	const struct cmos_entries *q;
	cmos_enum_t cmos_enum;
	cmos_entry_t cmos_entry;

	/* First add the enums. */
	for (p = first_cmos_table_enum(); p != NULL;
	     p = next_cmos_table_enum(p)) {
		cmos_enum.config_id = p->config_id;
		cmos_enum.value = p->value;
		strncpy(cmos_enum.text, (char *)p->text, CMOS_MAX_TEXT_LENGTH);
		cmos_enum.text[CMOS_MAX_TEXT_LENGTH] = '\0';
		try_add_cmos_table_enum(&cmos_enum);
	}

	/* Now add the entries.  We must add the entries after the enums because
	 * the entries are sanity checked against the enums as they are added.
	 */
	for (q = first_cmos_table_entry(); q != NULL;
	     q = next_cmos_table_entry(q)) {
		cmos_entry.bit = q->bit;
		cmos_entry.length = q->length;

		switch (q->config) {
		case 'e':
			cmos_entry.config = CMOS_ENTRY_ENUM;
			break;

		case 'h':
			cmos_entry.config = CMOS_ENTRY_HEX;
			break;

		case 'r':
			cmos_entry.config = CMOS_ENTRY_RESERVED;
			break;

		case 's':
			cmos_entry.config = CMOS_ENTRY_STRING;
			break;

		default:
			fprintf(stderr,
				"%s: Entry in CMOS option table has unknown config "
				"value.\n", prog_name);
			exit(1);
		}

		cmos_entry.config_id = q->config_id;
		strncpy(cmos_entry.name, (char *)q->name, CMOS_MAX_NAME_LENGTH);
		cmos_entry.name[CMOS_MAX_NAME_LENGTH] = '\0';
		try_add_cmos_table_entry(&cmos_entry);
	}
}

/****************************************************************************
 * get_cmos_checksum_info
 *
 * Get layout information for CMOS checksum.
 ****************************************************************************/
static void get_cmos_checksum_info(void)
{
	const cmos_entry_t *e;
	struct cmos_checksum *checksum;
	cmos_checksum_layout_t layout;
	unsigned index, index2;

	checksum = (struct cmos_checksum *)next_cmos_rec((const struct lb_record *)first_cmos_table_enum(), LB_TAG_OPTION_CHECKSUM);

	if (checksum != NULL) {	/* We are lucky.  The coreboot table hints us to the checksum.
				 * We might have to check the type field here though.
				 */
		layout.summed_area_start = checksum->range_start;
		layout.summed_area_end = checksum->range_end;
		layout.checksum_at = checksum->location;
		try_convert_checksum_layout(&layout);
		cmos_checksum_start = layout.summed_area_start;
		cmos_checksum_end = layout.summed_area_end;
		cmos_checksum_index = layout.checksum_at;
		return;
	}

	if ((e = find_cmos_entry(checksum_param_name)) == NULL)
		return;

	/* If we get here, we are unlucky.  The CMOS option table contains the
	 * location of the CMOS checksum.  However, there is no information
	 * regarding which bytes of the CMOS area the checksum is computed over.
	 * Thus we have to hope our presets will be fine.
	 */

	if (e->bit % 8) {
		fprintf(stderr,
			"%s: Error: CMOS checksum is not byte-aligned.\n",
			prog_name);
		exit(1);
	}

	index = e->bit / 8;
	index2 = index + 1;	/* The CMOS checksum occupies 16 bits. */

	if (verify_cmos_byte_index(index) || verify_cmos_byte_index(index2)) {
		fprintf(stderr,
			"%s: Error: CMOS checksum location out of range.\n",
			prog_name);
		exit(1);
	}

	if (((index >= cmos_checksum_start) && (index <= cmos_checksum_end)) ||
	    (((index2) >= cmos_checksum_start)
	     && ((index2) <= cmos_checksum_end))) {
		fprintf(stderr,
			"%s: Error: CMOS checksum overlaps checksummed area.\n",
			prog_name);
		exit(1);
	}

	cmos_checksum_index = index;
}

/****************************************************************************
 * try_convert_checksum_layout
 *
 * Perform sanity checking on CMOS checksum layout information and attempt to
 * convert information from bit positions to byte positions.  Return OK on
 * success or an error code on failure.
 ****************************************************************************/
static void try_convert_checksum_layout(cmos_checksum_layout_t * layout)
{
	switch (checksum_layout_to_bytes(layout)) {
	case OK:
		return;

	case LAYOUT_SUMMED_AREA_START_NOT_ALIGNED:
		fprintf(stderr,
			"%s: CMOS checksummed area start is not byte-aligned.\n",
			prog_name);
		break;

	case LAYOUT_SUMMED_AREA_END_NOT_ALIGNED:
		fprintf(stderr,
			"%s: CMOS checksummed area end is not byte-aligned.\n",
			prog_name);
		break;

	case LAYOUT_CHECKSUM_LOCATION_NOT_ALIGNED:
		fprintf(stderr,
			"%s: CMOS checksum location is not byte-aligned.\n",
			prog_name);
		break;

	case LAYOUT_INVALID_SUMMED_AREA:
		fprintf(stderr,
			"%s: CMOS checksummed area end must be greater than "
			"CMOS checksummed area start.\n", prog_name);
		break;

	case LAYOUT_CHECKSUM_OVERLAPS_SUMMED_AREA:
		fprintf(stderr,
			"%s: CMOS checksum overlaps checksummed area.\n",
			prog_name);
		break;

	case LAYOUT_SUMMED_AREA_OUT_OF_RANGE:
		fprintf(stderr,
			"%s: CMOS checksummed area out of range.\n", prog_name);
		break;

	case LAYOUT_CHECKSUM_LOCATION_OUT_OF_RANGE:
		fprintf(stderr,
			"%s: CMOS checksum location out of range.\n",
			prog_name);
		break;

	default:
		BUG();
	}

	exit(1);
}

/****************************************************************************
 * try_add_cmos_table_enum
 *
 * Attempt to add a CMOS enum to our internal repository.  Exit with an error
 * message on failure.
 ****************************************************************************/
static void try_add_cmos_table_enum(cmos_enum_t * cmos_enum)
{
	switch (add_cmos_enum(cmos_enum)) {
	case OK:
		return;

	case LAYOUT_DUPLICATE_ENUM:
		fprintf(stderr, "%s: Duplicate enum %s found in CMOS option "
			"table.\n", prog_name, cmos_enum->text);
		break;

	default:
		BUG();
	}

	exit(1);
}

/****************************************************************************
 * try_add_cmos_table_entry
 *
 * Attempt to add a CMOS entry to our internal repository.  Exit with an
 * error message on failure.
 ****************************************************************************/
static void try_add_cmos_table_entry(cmos_entry_t * cmos_entry)
{
	const cmos_entry_t *conflict;

	switch (add_cmos_entry(cmos_entry, &conflict)) {
	case OK:
		return;

	case CMOS_AREA_OUT_OF_RANGE:
		fprintf(stderr,
			"%s: Bad CMOS option layout in CMOS option table entry "
			"%s.\n", prog_name, cmos_entry->name);
		break;

	case CMOS_AREA_TOO_WIDE:
		fprintf(stderr,
			"%s: Area too wide for CMOS option table entry %s.\n",
			prog_name, cmos_entry->name);
		break;

	case LAYOUT_ENTRY_OVERLAP:
		fprintf(stderr,
			"%s: CMOS option table entries %s and %s have overlapping "
			"layouts.\n", prog_name, cmos_entry->name,
			conflict->name);
		break;

	case LAYOUT_ENTRY_BAD_LENGTH:
		/* Silently ignore entries with zero length.  Although this should
		 * never happen in practice, we should handle the case in a
		 * reasonable manner just to be safe.
		 */
		return;

	default:
		BUG();
	}

	exit(1);
}

/****************************************************************************
 * first_cmos_table_entry
 *
 * Return a pointer to the first entry in the CMOS table that represents a
 * CMOS parameter.  Return NULL if CMOS table is empty.
 ****************************************************************************/
static const struct cmos_entries *first_cmos_table_entry(void)
{
	return (const struct cmos_entries *)first_cmos_rec(LB_TAG_OPTION);
}

/****************************************************************************
 * next_cmos_table_entry
 *
 * Return a pointer to the next entry after 'last' in the CMOS table that
 * represents a CMOS parameter.  Return NULL if there are no more parameters.
 ****************************************************************************/
static const struct cmos_entries *next_cmos_table_entry(const struct
							cmos_entries *last)
{
	return (const struct cmos_entries *)
	    next_cmos_rec((const struct lb_record *)last, LB_TAG_OPTION);
}

/****************************************************************************
 * first_cmos_table_enum
 *
 * Return a pointer to the first entry in the CMOS table that represents a
 * possible CMOS parameter value.  Return NULL if the table does not contain
 * any such entries.
 ****************************************************************************/
static const struct cmos_enums *first_cmos_table_enum(void)
{
	return (const struct cmos_enums *)first_cmos_rec(LB_TAG_OPTION_ENUM);
}

/****************************************************************************
 * next_cmos_table_enum
 *
 * Return a pointer to the next entry after 'last' in the CMOS table that
 * represents a possible CMOS parameter value.  Return NULL if there are no
 * more parameter values.
 ****************************************************************************/
static const struct cmos_enums *next_cmos_table_enum
    (const struct cmos_enums *last) {
	return (const struct cmos_enums *)
	    next_cmos_rec((const struct lb_record *)last, LB_TAG_OPTION_ENUM);
}

/****************************************************************************
 * first_cmos_rec
 *
 * Return a pointer to the first entry in the CMOS table whose type matches
 * 'tag'.  Return NULL if CMOS table contains no such entry.
 *
 * Possible values for 'tag' are as follows:
 *
 *     LB_TAG_OPTION:      The entry represents a CMOS parameter.
 *     LB_TAG_OPTION_ENUM: The entry represents a possible value for a CMOS
 *                         parameter of type 'enum'.
 *
 * The CMOS table tells us where in the nonvolatile RAM to look for CMOS
 * parameter values and specifies their types as 'enum', 'hex', or
 * 'reserved'.
 ****************************************************************************/
static const struct lb_record *first_cmos_rec(uint32_t tag)
{
	const char *p;
	uint32_t bytes_processed, bytes_for_entries;
	const struct lb_record *lbrec;

	p = ((const char *)cmos_table) + cmos_table->header_length;
	bytes_for_entries = cmos_table->size - cmos_table->header_length;

	for (bytes_processed = 0;
	     bytes_processed < bytes_for_entries;
	     bytes_processed += lbrec->size) {
		lbrec = (const struct lb_record *)&p[bytes_processed];

		if (lbrec->tag == tag)
			return lbrec;
	}

	return NULL;
}

/****************************************************************************
 * next_cmos_rec
 *
 * Return a pointer to the next entry after 'last' in the CMOS table whose
 * type matches 'tag'.  Return NULL if the table contains no more entries of
 * this type.
 ****************************************************************************/
static const struct lb_record *next_cmos_rec(const struct lb_record *last,
					     uint32_t tag)
{
	const char *p;
	uint32_t bytes_processed, bytes_for_entries, last_offset;
	const struct lb_record *lbrec;

	p = ((const char *)cmos_table) + cmos_table->header_length;
	bytes_for_entries = cmos_table->size - cmos_table->header_length;
	last_offset = ((const char *)last) - p;

	for (bytes_processed = last_offset + last->size;
	     bytes_processed < bytes_for_entries;
	     bytes_processed += lbrec->size) {
		lbrec = (const struct lb_record *)&p[bytes_processed];

		if (lbrec->tag == tag)
			return lbrec;
	}

	return NULL;
}

