/*****************************************************************************\
 * layout.c
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

#include "common.h"
#include "layout.h"
#include "cmos_lowlevel.h"

typedef struct cmos_entry_item_t cmos_entry_item_t;

struct cmos_entry_item_t {
	cmos_entry_t item;
	cmos_entry_item_t *next;
};

typedef struct cmos_enum_item_t cmos_enum_item_t;

struct cmos_enum_item_t {
	cmos_enum_t item;
	cmos_enum_item_t *next;
};

static void default_cmos_layout_get_fn(void);
static int areas_overlap(unsigned area_0_start, unsigned area_0_length,
			 unsigned area_1_start, unsigned area_1_length);
static int entries_overlap(const cmos_entry_t * p, const cmos_entry_t * q);
static const cmos_enum_item_t *find_first_cmos_enum_id(unsigned config_id);

const char checksum_param_name[] = "check_sum";

/* Newer versions of coreboot store the 3 pieces of information below in the
 * coreboot table so we don't have to rely on hardcoded values.
 */

/* This is the offset from the start of CMOS of the first byte that the
 * checksum is calculated over.
 */
#define CMOS_CHECKSUM_START 49

/* This is the offset from the start of CMOS of the last byte that the
 * checksum is calculated over.
 */
#define CMOS_CHECKSUM_END 125

/* This is the offset from the start of CMOS where the coreboot checksum is
 * stored.
 */
#define CMOS_CHECKSUM_INDEX 126

/* index of first byte of checksummed area */
unsigned cmos_checksum_start = CMOS_CHECKSUM_START;

/* index of last byte of checksummed area */
unsigned cmos_checksum_end = CMOS_CHECKSUM_END;

/* index of first byte of CMOS checksum (a big-endian 16-bit value) */
unsigned cmos_checksum_index = CMOS_CHECKSUM_INDEX;

/* List is sorted in ascending order according to 'bit' field in
 * cmos_entry_t.
 */
static cmos_entry_item_t *cmos_entry_list = NULL;

/* List is sorted in ascending order: first by 'config_id' and then by
 * 'value'.
 */
static cmos_enum_item_t *cmos_enum_list = NULL;

static cmos_layout_get_fn_t cmos_layout_get_fn = default_cmos_layout_get_fn;

/****************************************************************************
 * entries_overlap
 *
 * Return 1 if cmos entries 'p' and 'q' overlap.  Else return 0.
 ****************************************************************************/
static inline int entries_overlap(const cmos_entry_t * p,
				  const cmos_entry_t * q)
{
	return areas_overlap(p->bit, p->length, q->bit, q->length);
}

/****************************************************************************
 * cmos_entry_to_const_item
 *
 * Return a pointer to the cmos_entry_item_t that 'p' is embedded within.
 ****************************************************************************/
static inline const cmos_entry_item_t *cmos_entry_to_const_item
    (const cmos_entry_t * p) {
	static const cmos_entry_t *pos = &((cmos_entry_item_t *) 0)->item;
	unsigned long offset, address;

	offset = (unsigned long)pos;
	address = ((unsigned long)p) - offset;
	return (const cmos_entry_item_t *)address;
}

/****************************************************************************
 * cmos_enum_to_const_item
 *
 * Return a pointer to the cmos_enum_item_t that 'p' is embedded within.
 ****************************************************************************/
static inline const cmos_enum_item_t *cmos_enum_to_const_item
    (const cmos_enum_t * p) {
	static const cmos_enum_t *pos = &((cmos_enum_item_t *) 0)->item;
	unsigned long offset, address;

	offset = (unsigned long)pos;
	address = ((unsigned long)p) - offset;
	return (const cmos_enum_item_t *)address;
}

/****************************************************************************
 * register_cmos_layout_get_fn
 *
 * Set 'fn' as the function that will be called to retrieve CMOS layout
 * information.
 ****************************************************************************/
void register_cmos_layout_get_fn(cmos_layout_get_fn_t fn)
{
	cmos_layout_get_fn = fn;
}

/****************************************************************************
 * get_cmos_layout
 *
 * Retrieve CMOS layout information and store it in our internal repository.
 ****************************************************************************/
void get_cmos_layout(void)
{
	cmos_layout_get_fn();
}

/****************************************************************************
 * add_cmos_entry
 *
 * Attempt to add CMOS entry 'e' to our internal repository of layout
 * information.  Return OK on success or an error code on failure.  If
 * operation fails because 'e' overlaps an existing CMOS entry, '*conflict'
 * will be set to point to the overlapping entry.
 ****************************************************************************/
int add_cmos_entry(const cmos_entry_t * e, const cmos_entry_t ** conflict)
{
	cmos_entry_item_t *item, *prev, *new_entry;

	*conflict = NULL;

	if (e->length < 1)
		return LAYOUT_ENTRY_BAD_LENGTH;

	if ((new_entry =
	     (cmos_entry_item_t *) malloc(sizeof(*new_entry))) == NULL)
		out_of_memory();

	new_entry->item = *e;

	if (cmos_entry_list == NULL) {
		new_entry->next = NULL;
		cmos_entry_list = new_entry;
		return OK;
	}

	/* Find place in list to insert new entry.  List is sorted in ascending
	 * order.
	 */
	for (item = cmos_entry_list, prev = NULL;
	     (item != NULL) && (item->item.bit < e->bit);
	     prev = item, item = item->next) ;

	if (prev == NULL) {
		if (entries_overlap(e, &cmos_entry_list->item)) {
			*conflict = &cmos_entry_list->item;
			goto fail;
		}

		new_entry->next = cmos_entry_list;
		cmos_entry_list = new_entry;
		return OK;
	}

	if (entries_overlap(&prev->item, e)) {
		*conflict = &prev->item;
		goto fail;
	}

	if ((item != NULL) && entries_overlap(e, &item->item)) {
		*conflict = &item->item;
		goto fail;
	}

	new_entry->next = item;
	prev->next = new_entry;
	return OK;

      fail:
	free(new_entry);
	return LAYOUT_ENTRY_OVERLAP;
}

/****************************************************************************
 * find_cmos_entry
 *
 * Search for a CMOS entry whose name is 'name'.  Return pointer to matching
 * entry or NULL if entry not found.
 ****************************************************************************/
const cmos_entry_t *find_cmos_entry(const char name[])
{
	cmos_entry_item_t *item;

	for (item = cmos_entry_list; item != NULL; item = item->next) {
		if (!strcmp(item->item.name, name))
			return &item->item;
	}

	return NULL;
}

/****************************************************************************
 * first_cmos_entry
 *
 * Return a pointer to the first CMOS entry in our list or NULL if list is
 * empty.
 ****************************************************************************/
const cmos_entry_t *first_cmos_entry(void)
{
	return (cmos_entry_list == NULL) ? NULL : &cmos_entry_list->item;
}

/****************************************************************************
 * next_cmos_entry
 *
 * Return a pointer to next entry in list after 'last' or NULL if no more
 * entries.
 ****************************************************************************/
const cmos_entry_t *next_cmos_entry(const cmos_entry_t * last)
{
	const cmos_entry_item_t *last_item, *next_item;

	last_item = cmos_entry_to_const_item(last);
	next_item = last_item->next;
	return (next_item == NULL) ? NULL : &next_item->item;
}

/****************************************************************************
 * add_cmos_enum
 *
 * Attempt to add CMOS enum 'e' to our internal repository of layout
 * information.  Return OK on success or an error code on failure.
 ****************************************************************************/
int add_cmos_enum(const cmos_enum_t * e)
{
	cmos_enum_item_t *item, *prev, *new_enum;

	if ((new_enum = (cmos_enum_item_t *) malloc(sizeof(*new_enum))) == NULL)
		out_of_memory();

	new_enum->item = *e;

	if (cmos_enum_list == NULL) {
		new_enum->next = NULL;
		cmos_enum_list = new_enum;
		return OK;
	}

	/* The list of enums is sorted in ascending order, first by
	 * 'config_id' and then by 'value'.  Look for the first enum
	 * whose 'config_id' field matches 'e'.
	 */
	for (item = cmos_enum_list, prev = NULL;
	     (item != NULL) && (item->item.config_id < e->config_id);
	     prev = item, item = item->next) ;

	if (item == NULL) {
		new_enum->next = NULL;
		prev->next = new_enum;
		return OK;
	}

	if (item->item.config_id > e->config_id) {
		new_enum->next = item;

		if (prev == NULL)
			cmos_enum_list = new_enum;
		else
			prev->next = new_enum;

		return OK;
	}

	/* List already contains at least one enum whose 'config_id'
	 * matches 'e'.  Now find proper place to insert 'e' based on
	 * 'value'.
	 */
	while (item->item.value < e->value) {
		prev = item;
		item = item->next;

		if ((item == NULL) || (item->item.config_id != e->config_id)) {
			new_enum->next = item;
			prev->next = new_enum;
			return OK;
		}
	}

	if (item->item.value == e->value) {
		free(new_enum);
		return LAYOUT_DUPLICATE_ENUM;
	}

	new_enum->next = item;

	if (prev == NULL)
		cmos_enum_list = new_enum;
	else
		prev->next = new_enum;

	return OK;
}

/****************************************************************************
 * find_cmos_enum
 *
 * Search for an enum that matches 'config_id' and 'value'.  If found, return
 * a pointer to the mathcing enum.  Else return NULL.
 ****************************************************************************/
const cmos_enum_t *find_cmos_enum(unsigned config_id, unsigned long long value)
{
	const cmos_enum_item_t *item;

	if ((item = find_first_cmos_enum_id(config_id)) == NULL)
		return NULL;

	while (item->item.value < value) {
		item = item->next;

		if ((item == NULL) || (item->item.config_id != config_id))
			return NULL;
	}

	return (item->item.value == value) ? &item->item : NULL;
}

/****************************************************************************
 * first_cmos_enum
 *
 * Return a pointer to the first CMOS enum in our list or NULL if list is
 * empty.
 ****************************************************************************/
const cmos_enum_t *first_cmos_enum(void)
{
	return (cmos_enum_list == NULL) ? NULL : &cmos_enum_list->item;
}

/****************************************************************************
 * next_cmos_enum
 *
 * Return a pointer to next enum in list after 'last' or NULL if no more
 * enums.
 ****************************************************************************/
const cmos_enum_t *next_cmos_enum(const cmos_enum_t * last)
{
	const cmos_enum_item_t *last_item, *next_item;

	last_item = cmos_enum_to_const_item(last);
	next_item = last_item->next;
	return (next_item == NULL) ? NULL : &next_item->item;
}

/****************************************************************************
 * first_cmos_enum_id
 *
 * Return a pointer to the first CMOS enum in our list that matches
 * 'config_id' or NULL if there are no matching enums.
 ****************************************************************************/
const cmos_enum_t *first_cmos_enum_id(unsigned config_id)
{
	const cmos_enum_item_t *item;

	item = find_first_cmos_enum_id(config_id);
	return (item == NULL) ? NULL : &item->item;
}

/****************************************************************************
 * next_cmos_enum_id
 *
 * Return a pointer to next enum in list after 'last' that matches the
 * 'config_id' field of 'last' or NULL if there are no more matching enums.
 ****************************************************************************/
const cmos_enum_t *next_cmos_enum_id(const cmos_enum_t * last)
{
	const cmos_enum_item_t *item;

	item = cmos_enum_to_const_item(last)->next;
	return ((item == NULL) || (item->item.config_id != last->config_id)) ?
	    NULL : &item->item;
}

/****************************************************************************
 * is_checksum_name
 *
 * Return 1 if 'name' matches the name of the parameter representing the CMOS
 * checksum.  Else return 0.
 ****************************************************************************/
int is_checksum_name(const char name[])
{
	return !strcmp(name, checksum_param_name);
}

/****************************************************************************
 * checksum_layout_to_bytes
 *
 * On entry, '*layout' contains checksum-related layout information expressed
 * in bits.  Perform sanity checking on the information and convert it from
 * bit positions to byte positions.  Return OK on success or an error code if
 * a sanity check fails.
 ****************************************************************************/
int checksum_layout_to_bytes(cmos_checksum_layout_t * layout)
{
	unsigned start, end, index;

	start = layout->summed_area_start;
	end = layout->summed_area_end;
	index = layout->checksum_at;

	if (start % 8)
		return LAYOUT_SUMMED_AREA_START_NOT_ALIGNED;

	if ((end % 8) != 7)
		return LAYOUT_SUMMED_AREA_END_NOT_ALIGNED;

	if (index % 8)
		return LAYOUT_CHECKSUM_LOCATION_NOT_ALIGNED;

	if (end <= start)
		return LAYOUT_INVALID_SUMMED_AREA;

	/* Convert bit positions to byte positions. */
	start /= 8;
	end /= 8;		/* equivalent to "end = ((end - 7) / 8)" */
	index /= 8;

	if (verify_cmos_byte_index(start) || verify_cmos_byte_index(end))
		return LAYOUT_SUMMED_AREA_OUT_OF_RANGE;

	if (verify_cmos_byte_index(index))
		return LAYOUT_CHECKSUM_LOCATION_OUT_OF_RANGE;

	/* checksum occupies 16 bits */
	if (areas_overlap(start, end - start + 1, index, index + 1))
		return LAYOUT_CHECKSUM_OVERLAPS_SUMMED_AREA;

	layout->summed_area_start = start;
	layout->summed_area_end = end;
	layout->checksum_at = index;
	return OK;
}

/****************************************************************************
 * checksum_layout_to_bits
 *
 * On entry, '*layout' contains checksum-related layout information expressed
 * in bytes.  Convert this information to bit positions.
 ****************************************************************************/
void checksum_layout_to_bits(cmos_checksum_layout_t * layout)
{
	layout->summed_area_start *= 8;
	layout->summed_area_end = (layout->summed_area_end * 8) + 7;
	layout->checksum_at *= 8;
}

/****************************************************************************
 * default_cmos_layout_get_fn
 *
 * If this function is ever called, it means that an appropriate callback for
 * obtaining CMOS layout information was not set before attempting to
 * retrieve layout information.
 ****************************************************************************/
static void default_cmos_layout_get_fn(void)
{
	BUG();
}

/****************************************************************************
 * areas_overlap
 *
 * Return 1 if the two given areas overlap.  Else return 0.
 ****************************************************************************/
static int areas_overlap(unsigned area_0_start, unsigned area_0_length,
			 unsigned area_1_start, unsigned area_1_length)
{
	unsigned area_0_end, area_1_end;

	area_0_end = area_0_start + area_0_length - 1;
	area_1_end = area_1_start + area_1_length - 1;
	return ((area_1_start <= area_0_end) && (area_0_start <= area_1_end));
}

/****************************************************************************
 * find_first_cmos_enum_id
 *
 * Return a pointer to the first item in our list of enums that matches
 * 'config_id'.  Return NULL if there is no matching enum.
 ****************************************************************************/
static const cmos_enum_item_t *find_first_cmos_enum_id(unsigned config_id)
{
	cmos_enum_item_t *item;

	for (item = cmos_enum_list;
	     (item != NULL) && (item->item.config_id < config_id);
	     item = item->next) ;

	return ((item == NULL) || (item->item.config_id > config_id)) ?
	    NULL : item;
}
