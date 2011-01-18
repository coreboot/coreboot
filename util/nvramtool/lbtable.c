/*****************************************************************************\
 * lbtable.c
 *****************************************************************************
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
#include <sys/mman.h>
#include "common.h"
#include "coreboot_tables.h"
#include "ip_checksum.h"
#include "lbtable.h"
#include "layout.h"
#include "cmos_lowlevel.h"
#include "hexdump.h"

typedef void (*lbtable_print_fn_t) (const struct lb_record * rec);

/* This structure represents an item in the coreboot table that may be
 * displayed using the -l option.
 */
typedef struct {
	uint32_t tag;
	const char *name;
	const char *description;
	const char *nofound_msg;
	lbtable_print_fn_t print_fn;
} lbtable_choice_t;

typedef struct {
	unsigned long start;	/* address of first byte of memory range */
	unsigned long end;	/* address of last byte of memory range */
} mem_range_t;

static const struct lb_header *lbtable_scan(unsigned long start,
					    unsigned long end,
					    int *bad_header_count,
					    int *bad_table_count);
static void process_cmos_table(void);
static void get_cmos_checksum_info(void);
static void try_convert_checksum_layout(cmos_checksum_layout_t * layout);
static void try_add_cmos_table_enum(cmos_enum_t * cmos_enum);
static void try_add_cmos_table_entry(cmos_entry_t * cmos_entry);
static const struct lb_record *find_lbrec(uint32_t tag);
static const char *lbrec_tag_to_str(uint32_t tag);
static const struct cmos_entries *first_cmos_table_entry(void);
static const struct cmos_entries *next_cmos_table_entry(const struct
							cmos_entries *last);
static const struct cmos_enums *first_cmos_table_enum(void);
static const struct cmos_enums *next_cmos_table_enum
    (const struct cmos_enums *last);
static const struct lb_record *first_cmos_rec(uint32_t tag);
static const struct lb_record *next_cmos_rec(const struct lb_record *last,
					     uint32_t tag);
static void memory_print_fn(const struct lb_record *rec);
static void mainboard_print_fn(const struct lb_record *rec);
static void cmos_opt_table_print_fn(const struct lb_record *rec);
static void print_option_record(const struct cmos_entries *cmos_entry);
static void print_enum_record(const struct cmos_enums *cmos_enum);
static void print_defaults_record(const struct cmos_defaults *cmos_defaults);
static void print_unknown_record(const struct lb_record *cmos_item);
static void option_checksum_print_fn(const struct lb_record *rec);
static void string_print_fn(const struct lb_record *rec);
static void uint64_to_hex_string(char str[], uint64_t n);

static const char memory_desc[] =
    "    This shows information about system memory.\n";

static const char mainboard_desc[] =
    "    This shows information about your mainboard.\n";

static const char version_desc[] =
    "    This shows coreboot version information.\n";

static const char extra_version_desc[] =
    "    This shows extra coreboot version information.\n";

static const char build_desc[] = "    This shows coreboot build information.\n";

static const char compile_time_desc[] =
    "    This shows when coreboot was compiled.\n";

static const char compile_by_desc[] = "    This shows who compiled coreboot.\n";

static const char compile_host_desc[] =
    "    This shows the name of the machine that compiled coreboot.\n";

static const char compile_domain_desc[] =
    "    This shows the domain name of the machine that compiled coreboot.\n";

static const char compiler_desc[] =
    "    This shows the name of the compiler used to build coreboot.\n";

static const char linker_desc[] =
    "    This shows the name of the linker used to build coreboot.\n";

static const char assembler_desc[] =
    "    This shows the name of the assembler used to build coreboot.\n";

static const char cmos_opt_table_desc[] =
    "    This does a low-level dump of the CMOS option table.  The table "
    "contains\n"
    "    information about the layout of the values that coreboot stores in\n"
    "    nonvolatile RAM.\n";

static const char option_checksum_desc[] =
    "    This shows the location of the CMOS checksum and the area over which it "
    "is\n" "    calculated.\n";

static const char generic_nofound_msg[] =
    "%s: Item %s not found in coreboot table.\n";

static const char nofound_msg_cmos_opt_table[] =
    "%s: Item %s not found in coreboot table.  Apparently, the "
    "coreboot installed on this system was built without specifying "
    "CONFIG_HAVE_OPTION_TABLE.\n";

static const char nofound_msg_option_checksum[] =
    "%s: Item %s not found in coreboot table. Apparently, you are "
    "using coreboot v1.\n";

int fd;

/* This is the number of items from the coreboot table that may be displayed
 * using the -l option.
 */
#define NUM_LBTABLE_CHOICES 14

/* These represent the various items from the coreboot table that may be
 * displayed using the -l option.
 */
static const lbtable_choice_t lbtable_choices[NUM_LBTABLE_CHOICES] =
    { {LB_TAG_MEMORY, "memory",
       memory_desc, generic_nofound_msg,
       memory_print_fn},
{LB_TAG_MAINBOARD, "mainboard",
 mainboard_desc, generic_nofound_msg,
 mainboard_print_fn},
{LB_TAG_VERSION, "version",
 version_desc, generic_nofound_msg,
 string_print_fn},
{LB_TAG_EXTRA_VERSION, "extra_version",
 extra_version_desc, generic_nofound_msg,
 string_print_fn},
{LB_TAG_BUILD, "build",
 build_desc, generic_nofound_msg,
 string_print_fn},
{LB_TAG_COMPILE_TIME, "compile_time",
 compile_time_desc, generic_nofound_msg,
 string_print_fn},
{LB_TAG_COMPILE_BY, "compile_by",
 compile_by_desc, generic_nofound_msg,
 string_print_fn},
{LB_TAG_COMPILE_HOST, "compile_host",
 compile_host_desc, generic_nofound_msg,
 string_print_fn},
{LB_TAG_COMPILE_DOMAIN, "compile_domain",
 compile_domain_desc, generic_nofound_msg,
 string_print_fn},
{LB_TAG_COMPILER, "compiler",
 compiler_desc, generic_nofound_msg,
 string_print_fn},
{LB_TAG_LINKER, "linker",
 linker_desc, generic_nofound_msg,
 string_print_fn},
{LB_TAG_ASSEMBLER, "assembler",
 assembler_desc, generic_nofound_msg,
 string_print_fn},
{LB_TAG_CMOS_OPTION_TABLE, "cmos_opt_table",
 cmos_opt_table_desc, nofound_msg_cmos_opt_table,
 cmos_opt_table_print_fn},
{LB_TAG_OPTION_CHECKSUM, "option_checksum",
 option_checksum_desc, nofound_msg_option_checksum,
 option_checksum_print_fn}
};

/* The coreboot table resides in low physical memory, which we access using
 * /dev/mem.  These are ranges of physical memory that should be scanned for a
 * coreboot table.
 */

#define NUM_MEM_RANGES 2

static const mem_range_t mem_ranges[NUM_MEM_RANGES] =
    { {0x00000000, 0x00000fff},
{0x000f0000, 0x000fffff}
};

/* This is the number of bytes of physical memory to map, starting at physical
 * address 0.  This value must be large enough to contain all memory ranges
 * specified in mem_ranges above plus the maximum possible size of the
 * coreboot table (since the start of the table could potentially occur at
 * the end of the last memory range).
 */
static const size_t BYTES_TO_MAP = (1024 * 1024);

/* Pointer to low physical memory that we access by calling mmap() on
 * /dev/mem.
 */
static const void *low_phys_mem;
static unsigned long low_phys_base = 0;

/* Pointer to coreboot table. */
static const struct lb_header *lbtable = NULL;

/* The CMOS option table is located within the coreboot table.  It tells us
 * where the CMOS parameters are located in the nonvolatile RAM.
 */
static const struct cmos_option_table *cmos_table = NULL;

static const hexdump_format_t format =
    { 12, 4, "            ", " | ", " ", " | ", '.' };

/****************************************************************************
 * vtophys
 *
 * Convert a virtual address to a physical address.  'vaddr' is a virtual
 * address in the address space of the current process.  It points to
 * somewhere in the chunk of memory that we mapped by calling mmap() on
 * /dev/mem.  This macro converts 'vaddr' to a physical address.
 ****************************************************************************/
#define vtophys(vaddr) (((unsigned long) vaddr) -       \
                        ((unsigned long) low_phys_mem) + low_phys_base)

/****************************************************************************
 * phystov
 *
 * Convert a physical address to a virtual address.  'paddr' is a physical
 * address.  This macro converts 'paddr' to a virtual address in the address
 * space of the current process.  The virtual to physical mapping was set up
 * by calling mmap() on /dev/mem.
 ****************************************************************************/
#define phystov(paddr) (((unsigned long) low_phys_mem) + \
                        ((unsigned long) paddr) - low_phys_base)

/****************************************************************************
 * get_lbtable
 *
 * Find the coreboot table and set global variable lbtable to point to it.
 ****************************************************************************/
void get_lbtable(void)
{
	int i, bad_header_count, bad_table_count, bad_headers, bad_tables;

	if (lbtable != NULL)
		return;

	/* The coreboot table is located in low physical memory, which may be
	 * conveniently accessed by calling mmap() on /dev/mem.
	 */

	if ((fd = open("/dev/mem", O_RDONLY, 0)) < 0) {
		fprintf(stderr, "%s: Can not open /dev/mem for reading: %s\n",
			prog_name, strerror(errno));
		exit(1);
	}

	if ((low_phys_mem =
	     mmap(NULL, BYTES_TO_MAP, PROT_READ, MAP_SHARED, fd, 0))
	    == MAP_FAILED) {
		fprintf(stderr, "%s: Failed to mmap /dev/mem: %s\n", prog_name,
			strerror(errno));
		exit(1);
	}

	bad_header_count = 0;
	bad_table_count = 0;

	for (i = 0; i < NUM_MEM_RANGES; i++) {
		lbtable = lbtable_scan(phystov(mem_ranges[i].start),
				       phystov(mem_ranges[i].end),
				       &bad_headers, &bad_tables);

		if (lbtable != NULL)
			return;	/* success: we found it! */

		bad_header_count += bad_headers;
		bad_table_count += bad_tables;
	}

	fprintf(stderr,
		"%s: coreboot table not found.  coreboot does not appear to\n"
		"        be installed on this system.  Scanning for the table "
		"produced the\n"
		"        following results:\n\n"
		"            %d valid signatures were found with bad header "
		"checksums.\n"
		"            %d valid headers were found with bad table "
		"checksums.\n", prog_name, bad_header_count, bad_table_count);
	exit(1);
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

/****************************************************************************
 * dump_lbtable
 *
 * Do a low-level dump of the coreboot table.
 ****************************************************************************/
void dump_lbtable(void)
{
	const char *p, *data;
	uint32_t bytes_processed;
	const struct lb_record *lbrec;

	p = ((const char *)lbtable) + lbtable->header_bytes;
	printf("Coreboot table at physical address 0x%lx:\n"
	       "    signature:       0x%x (ASCII: %c%c%c%c)\n"
	       "    header_bytes:    0x%x (decimal: %d)\n"
	       "    header_checksum: 0x%x (decimal: %d)\n"
	       "    table_bytes:     0x%x (decimal: %d)\n"
	       "    table_checksum:  0x%x (decimal: %d)\n"
	       "    table_entries:   0x%x (decimal: %d)\n\n",
	       vtophys(lbtable), lbtable->signature32,
	       lbtable->signature[0], lbtable->signature[1],
	       lbtable->signature[2], lbtable->signature[3],
	       lbtable->header_bytes, lbtable->header_bytes,
	       lbtable->header_checksum, lbtable->header_checksum,
	       lbtable->table_bytes, lbtable->table_bytes,
	       lbtable->table_checksum, lbtable->table_checksum,
	       lbtable->table_entries, lbtable->table_entries);

	if ((lbtable->table_bytes == 0) != (lbtable->table_entries == 0)) {
		printf
		    ("Inconsistent values for table_bytes and table_entries!!!\n"
		     "They should be either both 0 or both nonzero.\n");
		return;
	}

	if (lbtable->table_bytes == 0) {
		printf("The coreboot table is empty!!!\n");
		return;
	}

	for (bytes_processed = 0;;) {
		lbrec = (const struct lb_record *)&p[bytes_processed];
		printf("    %s record at physical address 0x%lx:\n"
		       "        tag:  0x%x (decimal: %d)\n"
		       "        size: 0x%x (decimal: %d)\n"
		       "        data:\n",
		       lbrec_tag_to_str(lbrec->tag), vtophys(lbrec), lbrec->tag,
		       lbrec->tag, lbrec->size, lbrec->size);

		data = ((const char *)lbrec) + sizeof(*lbrec);
		hexdump(data, lbrec->size - sizeof(*lbrec), vtophys(data),
			stdout, &format);

		bytes_processed += lbrec->size;

		if (bytes_processed >= lbtable->table_bytes)
			break;

		printf("\n");
	}
}

/****************************************************************************
 * list_lbtable_choices
 *
 * List names and informational blurbs for items from the coreboot table
 * that may be displayed using the -l option.
 ****************************************************************************/
void list_lbtable_choices(void)
{
	int i;

	for (i = 0;;) {
		printf("%s:\n%s",
		       lbtable_choices[i].name, lbtable_choices[i].description);

		if (++i >= NUM_LBTABLE_CHOICES)
			break;

		printf("\n");
	}
}

/****************************************************************************
 * list_lbtable_item
 *
 * Show the coreboot table item specified by 'item'.
 ****************************************************************************/
void list_lbtable_item(const char item[])
{
	int i;
	const struct lb_record *rec;

	for (i = 0; i < NUM_LBTABLE_CHOICES; i++) {
		if (strcmp(item, lbtable_choices[i].name) == 0)
			break;
	}

	if (i == NUM_LBTABLE_CHOICES) {
		fprintf(stderr, "%s: Invalid coreboot table item %s.\n",
			prog_name, item);
		exit(1);
	}

	if ((rec = find_lbrec(lbtable_choices[i].tag)) == NULL) {
		fprintf(stderr, lbtable_choices[i].nofound_msg, prog_name,
			lbtable_choices[i].name);
		exit(1);
	}

	lbtable_choices[i].print_fn(rec);
}

/****************************************************************************
 * lbtable_scan
 *
 * Scan the chunk of memory specified by 'start' and 'end' for a coreboot
 * table.  The first 4 bytes of the table are marked by the signature
 * { 'L', 'B', 'I', 'O' }.  'start' and 'end' indicate the addresses of the
 * first and last bytes of the chunk of memory to be scanned.  For instance,
 * values of 0x10000000 and 0x1000ffff for 'start' and 'end' specify a 64k
 * chunk of memory starting at address 0x10000000.  'start' and 'end' are
 * virtual addresses in the address space of the current process.  They
 * represent a chunk of memory obtained by calling mmap() on /dev/mem.
 *
 * If a coreboot table is found, return a pointer to it.  Otherwise return
 * NULL.  On return, *bad_header_count and *bad_table_count are set as
 * follows:
 *
 *     *bad_header_count:
 *         Indicates the number of times in which a valid signature was found
 *         but the header checksum was invalid.
 *
 *     *bad_table_count:
 *         Indicates the number of times in which a header with a valid
 *         checksum was found but the table checksum was invalid.
 ****************************************************************************/
static const struct lb_header *lbtable_scan(unsigned long start,
					    unsigned long end,
					    int *bad_header_count,
					    int *bad_table_count)
{
	static const char signature[4] = { 'L', 'B', 'I', 'O' };
	const struct lb_header *table;
	const struct lb_forward *forward;
	const uint32_t *p;
	uint32_t sig;

	assert(end >= start);
	memcpy(&sig, signature, sizeof(sig));
	table = NULL;
	*bad_header_count = 0;
	*bad_table_count = 0;

	/* Look for signature.  Table is aligned on 16-byte boundary.  Therefore
	 * only check every fourth 32-bit memory word.  As the loop is coded below,
	 * this function will behave in a reasonable manner for ALL possible values
	 * for 'start' and 'end': even weird boundary cases like 0x00000000 and
	 * 0xffffffff on a 32-bit architecture.
	 */
	for (p = (const uint32_t *)start;
	     (((unsigned long)p) <= end) &&
	     ((end - (unsigned long)p) >= (sizeof(uint32_t) - 1)); p += 4) {
		if (*p != sig)
			continue;

		/* We found a valid signature. */
		table = (const struct lb_header *)p;

		/* validate header checksum */
		if (compute_ip_checksum((void *)table, sizeof(*table))) {
			(*bad_header_count)++;
			continue;
		}

		/* validate table checksum */
		if (table->table_checksum !=
		    compute_ip_checksum(((char *)table) + sizeof(*table),
					table->table_bytes)) {
			(*bad_table_count)++;
			continue;
		}

		/* checksums are ok: we found it! */
		/* But it may just be a forwarding table, so look if there's a forwarder */
		lbtable = table;
		forward = (struct lb_forward *)find_lbrec(LB_TAG_FORWARD);
		lbtable = NULL;

		if (forward) {
			uint64_t new_phys = forward->forward;

			new_phys &= ~(getpagesize() - 1);

			munmap((void *)low_phys_mem, BYTES_TO_MAP);
			if ((low_phys_mem =
			     mmap(NULL, BYTES_TO_MAP, PROT_READ, MAP_SHARED, fd,
				  (off_t) new_phys)) == MAP_FAILED) {
				fprintf(stderr,
					"%s: Failed to mmap /dev/mem: %s\n",
					prog_name, strerror(errno));
				exit(1);
			}
			low_phys_base = new_phys;
			table =
			    lbtable_scan(phystov(low_phys_base),
					 phystov(low_phys_base + BYTES_TO_MAP),
					 bad_header_count, bad_table_count);
		}
		return table;
	}

	return NULL;
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

	checksum = (struct cmos_checksum *)find_lbrec(LB_TAG_OPTION_CHECKSUM);

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
 * find_lbrec
 *
 * Find the record in the coreboot table that matches 'tag'.  Return pointer
 * to record on success or NULL if record not found.
 ****************************************************************************/
static const struct lb_record *find_lbrec(uint32_t tag)
{
	const char *p;
	uint32_t bytes_processed;
	const struct lb_record *lbrec;

	p = ((const char *)lbtable) + lbtable->header_bytes;

	for (bytes_processed = 0;
	     bytes_processed < lbtable->table_bytes;
	     bytes_processed += lbrec->size) {
		lbrec = (const struct lb_record *)&p[bytes_processed];

		if (lbrec->tag == tag)
			return lbrec;
	}

	return NULL;
}

/****************************************************************************
 * lbrec_tag_to_str
 *
 * Return a pointer to the string representation of the given coreboot table
 * tag.
 ****************************************************************************/
static const char *lbrec_tag_to_str(uint32_t tag)
{
	switch (tag) {
	case LB_TAG_UNUSED:
		return "UNUSED";

	case LB_TAG_MEMORY:
		return "MEMORY";

	case LB_TAG_HWRPB:
		return "HWRPB";

	case LB_TAG_MAINBOARD:
		return "MAINBOARD";

	case LB_TAG_VERSION:
		return "VERSION";

	case LB_TAG_EXTRA_VERSION:
		return "EXTRA_VERSION";

	case LB_TAG_BUILD:
		return "BUILD";

	case LB_TAG_COMPILE_TIME:
		return "COMPILE_TIME";

	case LB_TAG_COMPILE_BY:
		return "COMPILE_BY";

	case LB_TAG_COMPILE_HOST:
		return "COMPILE_HOST";

	case LB_TAG_COMPILE_DOMAIN:
		return "COMPILE_DOMAIN";

	case LB_TAG_COMPILER:
		return "COMPILER";

	case LB_TAG_LINKER:
		return "LINKER";

	case LB_TAG_ASSEMBLER:
		return "ASSEMBLER";

	case LB_TAG_SERIAL:
		return "SERIAL";

	case LB_TAG_CONSOLE:
		return "CONSOLE";

	case LB_TAG_FORWARD:
		return "FORWARD";

	case LB_TAG_CMOS_OPTION_TABLE:
		return "CMOS_OPTION_TABLE";

	case LB_TAG_OPTION_CHECKSUM:
		return "OPTION_CHECKSUM";

	default:
		break;
	}

	return "UNKNOWN";
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

/****************************************************************************
 * memory_print_fn
 *
 * Display function for 'memory' item of coreboot table.
 ****************************************************************************/
static void memory_print_fn(const struct lb_record *rec)
{
	char start_str[19], end_str[19], size_str[19];
	const struct lb_memory *p;
	const char *mem_type;
	const struct lb_memory_range *ranges;
	uint64_t size, start, end;
	int i, entries;

	p = (const struct lb_memory *)rec;
	entries = (p->size - sizeof(*p)) / sizeof(p->map[0]);
	ranges = p->map;

	if (entries == 0) {
		printf("No memory ranges were found.\n");
		return;
	}

	for (i = 0;;) {
		switch (ranges[i].type) {
		case LB_MEM_RAM:
			mem_type = "AVAILABLE";
			break;

		case LB_MEM_RESERVED:
			mem_type = "RESERVED";
			break;

		case LB_MEM_TABLE:
			mem_type = "CONFIG_TABLE";
			break;

		default:
			mem_type = "UNKNOWN";
			break;
		}

		size = unpack_lb64(ranges[i].size);
		start = unpack_lb64(ranges[i].start);
		end = start + size - 1;
		uint64_to_hex_string(start_str, start);
		uint64_to_hex_string(end_str, end);
		uint64_to_hex_string(size_str, size);
		printf("%s memory:\n"
		       "    from physical addresses %s to %s\n"
		       "    size is %s bytes (%lld in decimal)\n",
		       mem_type, start_str, end_str, size_str,
		       (unsigned long long)size);

		if (++i >= entries)
			break;

		printf("\n");
	}
}

/****************************************************************************
 * mainboard_print_fn
 *
 * Display function for 'mainboard' item of coreboot table.
 ****************************************************************************/
static void mainboard_print_fn(const struct lb_record *rec)
{
	const struct lb_mainboard *p;

	p = (const struct lb_mainboard *)rec;
	printf("Vendor:      %s\n"
	       "Part number: %s\n",
	       &p->strings[p->vendor_idx], &p->strings[p->part_number_idx]);
}

/****************************************************************************
 * cmos_opt_table_print_fn
 *
 * Display function for 'cmos_opt_table' item of coreboot table.
 ****************************************************************************/
static void cmos_opt_table_print_fn(const struct lb_record *rec)
{
	const struct cmos_option_table *p;
	const struct lb_record *cmos_item;
	uint32_t bytes_processed, bytes_for_entries;
	const char *q;

	p = (const struct cmos_option_table *)rec;
	q = ((const char *)p) + p->header_length;
	bytes_for_entries = p->size - p->header_length;

	printf("CMOS option table at physical address 0x%lx:\n"
	       "    tag:           0x%x (decimal: %d)\n"
	       "    size:          0x%x (decimal: %d)\n"
	       "    header_length: 0x%x (decimal: %d)\n\n",
	       vtophys(p), p->tag, p->tag, p->size, p->size, p->header_length,
	       p->header_length);

	if (p->header_length > p->size) {
		printf
		    ("Header length for CMOS option table is greater than the size "
		     "of the entire table including header!!!\n");
		return;
	}

	if (bytes_for_entries == 0) {
		printf("The CMOS option table is empty!!!\n");
		return;
	}

	for (bytes_processed = 0;;) {
		cmos_item = (const struct lb_record *)&q[bytes_processed];

		switch (cmos_item->tag) {
		case LB_TAG_OPTION:
			print_option_record((const struct cmos_entries *)
					    cmos_item);
			break;

		case LB_TAG_OPTION_ENUM:
			print_enum_record((const struct cmos_enums *)cmos_item);
			break;

		case LB_TAG_OPTION_DEFAULTS:
			print_defaults_record((const struct cmos_defaults *)
					      cmos_item);
			break;

		default:
			print_unknown_record(cmos_item);
			break;
		}

		bytes_processed += cmos_item->size;

		if (bytes_processed >= bytes_for_entries)
			break;

		printf("\n");
	}
}

/****************************************************************************
 * print_option_record
 *
 * Display "option" record from CMOS option table.
 ****************************************************************************/
static void print_option_record(const struct cmos_entries *cmos_entry)
{
	static const size_t S_BUFSIZE = 80;
	char s[S_BUFSIZE];

	switch (cmos_entry->config) {
	case 'e':
		strcpy(s, "ENUM");
		break;

	case 'h':
		strcpy(s, "HEX");
		break;

	case 'r':
		strcpy(s, "RESERVED");
		break;

	default:
		snprintf(s, S_BUFSIZE, "UNKNOWN: value is 0x%x (decimal: %d)",
			 cmos_entry->config, cmos_entry->config);
		break;
	}

	printf("    OPTION record at physical address 0x%lx:\n"
	       "        tag:       0x%x (decimal: %d)\n"
	       "        size:      0x%x (decimal: %d)\n"
	       "        bit:       0x%x (decimal: %d)\n"
	       "        length:    0x%x (decimal: %d)\n"
	       "        config:    %s\n"
	       "        config_id: 0x%x (decimal: %d)\n"
	       "        name:      %s\n",
	       vtophys(cmos_entry), cmos_entry->tag, cmos_entry->tag,
	       cmos_entry->size, cmos_entry->size, cmos_entry->bit,
	       cmos_entry->bit, cmos_entry->length, cmos_entry->length, s,
	       cmos_entry->config_id, cmos_entry->config_id, cmos_entry->name);
}

/****************************************************************************
 * print_enum_record
 *
 * Display "enum" record from CMOS option table.
 ****************************************************************************/
static void print_enum_record(const struct cmos_enums *cmos_enum)
{
	printf("    ENUM record at physical address 0x%lx:\n"
	       "        tag:       0x%x (decimal: %d)\n"
	       "        size:      0x%x (decimal: %d)\n"
	       "        config_id: 0x%x (decimal: %d)\n"
	       "        value:     0x%x (decimal: %d)\n"
	       "        text:      %s\n",
	       vtophys(cmos_enum), cmos_enum->tag, cmos_enum->tag,
	       cmos_enum->size, cmos_enum->size, cmos_enum->config_id,
	       cmos_enum->config_id, cmos_enum->value, cmos_enum->value,
	       cmos_enum->text);
}

/****************************************************************************
 * print_defaults_record
 *
 * Display "defaults" record from CMOS option table.
 ****************************************************************************/
static void print_defaults_record(const struct cmos_defaults *cmos_defaults)
{
	printf("    DEFAULTS record at physical address 0x%lx:\n"
	       "        tag:         0x%x (decimal: %d)\n"
	       "        size:        0x%x (decimal: %d)\n"
	       "        name_length: 0x%x (decimal: %d)\n"
	       "        name:        %s\n"
	       "        default_set:\n",
	       vtophys(cmos_defaults), cmos_defaults->tag, cmos_defaults->tag,
	       cmos_defaults->size, cmos_defaults->size,
	       cmos_defaults->name_length, cmos_defaults->name_length,
	       cmos_defaults->name);
	hexdump(cmos_defaults->default_set, CMOS_IMAGE_BUFFER_SIZE,
		vtophys(cmos_defaults->default_set), stdout, &format);
}

/****************************************************************************
 * print_unknown_record
 *
 * Display record of unknown type from CMOS option table.
 ****************************************************************************/
static void print_unknown_record(const struct lb_record *cmos_item)
{
	const char *data;

	printf("    UNKNOWN record at physical address 0x%lx:\n"
	       "        tag:  0x%x (decimal: %d)\n"
	       "        size: 0x%x (decimal: %d)\n"
	       "        data:\n",
	       vtophys(cmos_item), cmos_item->tag, cmos_item->tag,
	       cmos_item->size, cmos_item->size);
	data = ((const char *)cmos_item) + sizeof(*cmos_item);
	hexdump(data, cmos_item->size - sizeof(*cmos_item), vtophys(data),
		stdout, &format);
}

/****************************************************************************
 * option_checksum_print_fn
 *
 * Display function for 'option_checksum' item of coreboot table.
 ****************************************************************************/
static void option_checksum_print_fn(const struct lb_record *rec)
{
	struct cmos_checksum *p;

	p = (struct cmos_checksum *)rec;
	printf("CMOS checksum from bit %d to bit %d\n"
	       "at position %d is type %s.\n",
	       p->range_start, p->range_end, p->location,
	       (p->type == CHECKSUM_PCBIOS) ? "PC BIOS" : "NONE");
}

/****************************************************************************
 * string_print_fn
 *
 * Display function for a generic item of coreboot table that simply
 * consists of a string.
 ****************************************************************************/
static void string_print_fn(const struct lb_record *rec)
{
	const struct lb_string *p;

	p = (const struct lb_string *)rec;
	printf("%s\n", p->string);
}

/****************************************************************************
 * uint64_to_hex_string
 *
 * Convert the 64-bit integer 'n' to its hexadecimal string representation,
 * storing the result in 's'.  's' must point to a buffer at least 19 bytes
 * long.  The result is displayed with as many leading zeros as needed to
 * make a 16-digit hex number including a 0x prefix (example: the number 1
 * will be displayed as "0x0000000000000001").
 ****************************************************************************/
static void uint64_to_hex_string(char str[], uint64_t n)
{
	int chars_printed;

	str[0] = '0';
	str[1] = 'x';

	/* Print the result right-justified with leading spaces in a
	 * 16-character field. */
	chars_printed = sprintf(&str[2], "%016llx", (unsigned long long)n);
	assert(chars_printed == 16);
}
