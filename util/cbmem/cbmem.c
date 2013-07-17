/*
 * This file is part of the coreboot project.
 *
 * Copyright 2012 Google Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */

#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <inttypes.h>
#include <getopt.h>
#include <errno.h>
#include <fcntl.h>
#include <ctype.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <libgen.h>
#include <assert.h>

#define ARRAY_SIZE(a) (sizeof(a) / sizeof((a)[0]))
#define MAP_BYTES (1024*1024)

#include "boot/coreboot_tables.h"

typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

#include "cbmem.h"
#include "timestamp.h"

#define CBMEM_VERSION "1.1"

/* verbose output? */
static int verbose = 0;
#define debug(x...) if(verbose) printf(x)

/* File handle used to access /dev/mem */
static int fd;

/*
 * calculate ip checksum (16 bit quantities) on a passed in buffer. In case
 * the buffer length is odd last byte is excluded from the calculation
 */
static u16 ipchcksum(const void *addr, unsigned size)
{
	const u16 *p = addr;
	unsigned i, n = size / 2; /* don't expect odd sized blocks */
	u32 sum = 0;

	for (i = 0; i < n; i++)
		sum += p[i];

	sum = (sum >> 16) + (sum & 0xffff);
	sum += (sum >> 16);
	sum = ~sum & 0xffff;
	return (u16) sum;
}

/*
 * Functions to map / unmap physical memory into virtual address space. These
 * functions always maps 1MB at a time and can only map one area at once.
 */
static void *mapped_virtual;
static void *map_memory(u64 physical)
{
	void *v;
	off_t p;
	u64 page = getpagesize();
	int padding;

	/* Mapped memory must be aligned to page size */
	p = physical & ~(page - 1);

	debug("Mapping 1MB of physical memory at 0x%jx.\n", (intmax_t)p);

	v = mmap(NULL, MAP_BYTES, PROT_READ, MAP_SHARED, fd, p);

	if (v == MAP_FAILED) {
		fprintf(stderr, "Failed to mmap /dev/mem: %s\n",
			strerror(errno));
		exit(1);
	}

	/* Remember what we actually mapped ... */
	mapped_virtual = v;

	/* ... but return address to the physical memory that was requested */
	padding = physical & (page-1);
	if (padding)
		debug("  ... padding virtual address with 0x%x bytes.\n",
			padding);
	v += padding;

	return v;
}

static void unmap_memory(void)
{
	if (mapped_virtual == NULL) {
		fprintf(stderr, "Error unmapping memory\n");
		return;
	}
	debug("Unmapping 1MB of virtual memory at %p.\n", mapped_virtual);
	munmap(mapped_virtual, MAP_BYTES);
	mapped_virtual = NULL;
}

/*
 * Try finding the timestamp table and coreboot cbmem console starting from the
 * passed in memory offset.  Could be called recursively in case a forwarding
 * entry is found.
 *
 * Returns pointer to a memory buffer containg the timestamp table or zero if
 * none found.
 */

static struct lb_cbmem_ref timestamps;
static struct lb_cbmem_ref console;
static struct lb_memory_range cbmem;

/* This is a work-around for a nasty problem introduced by initially having
 * pointer sized entries in the lb_cbmem_ref structures. This caused problems
 * on 64bit x86 systems because coreboot is 32bit on those systems.
 * When the problem was found, it was corrected, but there are a lot of
 * systems out there with a firmware that does not produce the right
 * lb_cbmem_ref structure. Hence we try to autocorrect this issue here.
 */
static struct lb_cbmem_ref parse_cbmem_ref(struct lb_cbmem_ref *cbmem_ref)
{
	struct lb_cbmem_ref ret;

	ret = *cbmem_ref;

	if (cbmem_ref->size < sizeof(*cbmem_ref))
		ret.cbmem_addr = (uint32_t)ret.cbmem_addr;

	debug("      cbmem_addr = %" PRIx64 "\n", ret.cbmem_addr);

	return ret;
}

static int parse_cbtable(u64 address)
{
	int i, found = 0;
	void *buf;

	debug("Looking for coreboot table at %" PRIx64 "\n", address);
	buf = map_memory(address);

	/* look at every 16 bytes within 4K of the base */

	for (i = 0; i < 0x1000; i += 0x10) {
		struct lb_header *lbh;
		struct lb_record* lbr_p;
		void *lbtable;
		int j;

		lbh = (struct lb_header *)(buf + i);
		if (memcmp(lbh->signature, "LBIO", sizeof(lbh->signature)) ||
		    !lbh->header_bytes ||
		    ipchcksum(lbh, sizeof(*lbh))) {
			continue;
		}
		lbtable = buf + i + lbh->header_bytes;

		if (ipchcksum(lbtable, lbh->table_bytes) !=
		    lbh->table_checksum) {
			debug("Signature found, but wrong checksum.\n");
			continue;
		}

		found = 1;
		debug("Found!\n");

		for (j = 0; j < lbh->table_bytes; j += lbr_p->size) {
			/* look for the timestamp table */
			lbr_p = (struct lb_record*) ((char *)lbtable + j);
			debug("  coreboot table entry 0x%02x\n", lbr_p->tag);
			switch (lbr_p->tag) {
			case LB_TAG_MEMORY: {
				int i = 0;
				debug("    Found memory map.\n");
				struct lb_memory *memory =
						(struct lb_memory *)lbr_p;
				while ((char *)&memory->map[i] < ((char *)lbtable
							    + lbr_p->size)) {
					if (memory->map[i].type == LB_MEM_TABLE) {
						debug("      LB_MEM_TABLE found.\n");
						/* The last one found is CBMEM */
						cbmem = memory->map[i];
					}
					i++;
				}
				continue;
			}
			case LB_TAG_TIMESTAMPS: {
				debug("    Found timestamp table.\n");
				timestamps = parse_cbmem_ref((struct lb_cbmem_ref *) lbr_p);
				continue;
			}
			case LB_TAG_CBMEM_CONSOLE: {
				debug("    Found cbmem console.\n");
				console = parse_cbmem_ref((struct lb_cbmem_ref *) lbr_p);
				continue;
			}
			case LB_TAG_FORWARD: {
				/*
				 * This is a forwarding entry - repeat the
				 * search at the new address.
				 */
				struct lb_forward lbf_p =
					*(struct lb_forward *) lbr_p;
				debug("    Found forwarding entry.\n");
				unmap_memory();
				return parse_cbtable(lbf_p.forward);
			}
			default:
				break;
			}

		}
	}
	unmap_memory();

	return found;
}

/*
 * read CPU frequency from a sysfs file, return an frequency in Kilohertz as
 * an int or exit on any error.
 */
static u64 get_cpu_freq_KHz(void)
{
	FILE *cpuf;
	char freqs[100];
	int  size;
	char *endp;
	u64 rv;

	const char* freq_file =
		"/sys/devices/system/cpu/cpu0/cpufreq/cpuinfo_max_freq";

	cpuf = fopen(freq_file, "r");
	if (!cpuf) {
		fprintf(stderr, "Could not open %s: %s\n",
			freq_file, strerror(errno));
		exit(1);
	}

	memset(freqs, 0, sizeof(freqs));
	size = fread(freqs, 1, sizeof(freqs), cpuf);
	if (!size || (size == sizeof(freqs))) {
		fprintf(stderr, "Wrong number of bytes(%d) read from %s\n",
			size, freq_file);
		exit(1);
	}
	fclose(cpuf);
	rv = strtoull(freqs, &endp, 10);

	if (*endp == '\0' || *endp == '\n')
		return rv;
	fprintf(stderr, "Wrong formatted value ^%s^ read from %s\n",
		freqs, freq_file);
	exit(1);
}

/*
 * Print an integer in 'normalized' form - with commas separating every three
 * decimal orders. The 'comma' parameter indicates if a comma is needed after
 * the value is printed.
 */
static void print_norm(u64 v, int comma)
{
	int first_triple = 1;

	if (v > 1000) {
		/* print the higher order sections first */
		print_norm(v / 1000, 1);
		first_triple = 0;
	}
	if (first_triple)
		printf("%d", (u32)(v % 1000));
	else
		printf("%3.3d", (u32)(v % 1000));
	if (comma)
		printf(",");
}

/* dump the timestamp table */
static void dump_timestamps(void)
{
	int i;
	u64 cpu_freq_MHz = get_cpu_freq_KHz() / 1000;
	struct timestamp_table *tst_p;

	if (timestamps.tag != LB_TAG_TIMESTAMPS) {
		fprintf(stderr, "No timestamps found in coreboot table.\n");
		return;
	}

	tst_p = (struct timestamp_table *)
			map_memory((unsigned long)timestamps.cbmem_addr);

	printf("%d entries total:\n\n", tst_p->num_entries);
	for (i = 0; i < tst_p->num_entries; i++) {
		const struct timestamp_entry *tse_p = tst_p->entries + i;

		printf("%4d:", tse_p->entry_id);
		print_norm(tse_p->entry_stamp / cpu_freq_MHz, 0);
		if (i) {
			printf(" (");
			print_norm((tse_p->entry_stamp -
				    tse_p[-1].entry_stamp) /
				   cpu_freq_MHz, 0);
			printf(")");
		}
		printf("\n");
	}

	unmap_memory();
}

/* dump the cbmem console */
static void dump_console(void)
{
	void *console_p;
	char *console_c;
	uint32_t size;
	uint32_t cursor;

	if (console.tag != LB_TAG_CBMEM_CONSOLE) {
		fprintf(stderr, "No console found in coreboot table.\n");
		return;
	}

	console_p = map_memory((unsigned long)console.cbmem_addr);
	/* The in-memory format of the console area is:
	 *  u32  size
	 *  u32  cursor
	 *  char console[size]
	 * Hence we have to add 8 to get to the actual console string.
	 */
	size = *(uint32_t *)console_p;
	cursor = *(uint32_t *) (console_p + 4);
	/* Cursor continues to go on even after no more data fits in
	 * the buffer but the data is dropped in this case.
	 */
	if (size > cursor)
		size = cursor;
	console_c = malloc(size + 1);
	if (!console_c) {
		fprintf(stderr, "Not enough memory for console.\n");
		exit(1);
	}

	memcpy(console_c, console_p + 8, size);
	console_c[size] = 0;

	printf("%s\n", console_c);
	if (size < cursor)
		printf("%d %s lost\n", cursor - size,
			(cursor - size) == 1 ? "byte":"bytes");

	free(console_c);

	unmap_memory();
}

static void hexdump(unsigned long memory, int length)
{
	int i;
	uint8_t *m;
	int all_zero = 0;

	m = map_memory((intptr_t)memory);

	if (length > MAP_BYTES) {
		printf("Truncating hex dump from %d to %d bytes\n\n",
			length, MAP_BYTES);
		length = MAP_BYTES;
	}

	for (i = 0; i < length; i += 16) {
		int j;

		all_zero++;
		for (j = 0; j < 16; j++) {
			if(m[i+j] != 0) {
				all_zero = 0;
				break;
			}
		}

		if (all_zero < 2) {
			printf("%08lx:", memory + i);
			for (j = 0; j < 16; j++)
				printf(" %02x", m[i+j]);
			printf("  ");
			for (j = 0; j < 16; j++)
				printf("%c", isprint(m[i+j]) ? m[i+j] : '.');
			printf("\n");
		} else if (all_zero == 2) {
			printf("...\n");
		}
	}

	unmap_memory();
}

static void dump_cbmem_hex(void)
{
	if (cbmem.type != LB_MEM_TABLE) {
		fprintf(stderr, "No coreboot CBMEM area found!\n");
		return;
	}

	hexdump(unpack_lb64(cbmem.start), unpack_lb64(cbmem.size));
}

/* The root region is at least DYN_CBMEM_ALIGN_SIZE . */
#define DYN_CBMEM_ALIGN_SIZE (4096)
#define ROOT_MIN_SIZE DYN_CBMEM_ALIGN_SIZE
#define CBMEM_POINTER_MAGIC 0xc0389479
#define CBMEM_ENTRY_MAGIC ~(CBMEM_POINTER_MAGIC)

struct cbmem_root_pointer {
	uint32_t magic;
	uint32_t root;
} __attribute__((packed));

struct dynamic_cbmem_entry {
	uint32_t magic;
	uint32_t start;
	uint32_t size;
	uint32_t id;
} __attribute__((packed));

struct cbmem_root {
	uint32_t max_entries;
	uint32_t num_entries;
	uint32_t locked;
	uint32_t size;
	struct dynamic_cbmem_entry entries[0];
} __attribute__((packed));

#define CBMEM_MAGIC 0x434f5245
#define MAX_CBMEM_ENTRIES 16

struct cbmem_entry {
	uint32_t magic;
	uint32_t id;
	uint64_t base;
	uint64_t size;
} __attribute__((packed));

static const struct cbmem_id_to_name {
	u32 id;
	const char *name;
} cbmem_ids[] = {
	{ CBMEM_ID_FREESPACE,		"FREE SPACE " },
	{ CBMEM_ID_GDT,			"GDT        " },
	{ CBMEM_ID_ACPI,		"ACPI       " },
	{ CBMEM_ID_CBTABLE,		"COREBOOT   " },
	{ CBMEM_ID_PIRQ,		"IRQ TABLE  " },
	{ CBMEM_ID_MPTABLE,		"SMP TABLE  " },
	{ CBMEM_ID_RESUME,		"ACPI RESUME" },
	{ CBMEM_ID_RESUME_SCRATCH,	"ACPISCRATCH" },
	{ CBMEM_ID_ACPI_GNVS,		"ACPI GNVS  " },
	{ CBMEM_ID_ACPI_GNVS_PTR,	"GNVS PTR   " },
	{ CBMEM_ID_SMBIOS,		"SMBIOS     " },
	{ CBMEM_ID_TIMESTAMP,		"TIME STAMP " },
	{ CBMEM_ID_MRCDATA,		"MRC DATA   " },
	{ CBMEM_ID_CONSOLE,		"CONSOLE    " },
	{ CBMEM_ID_ELOG,		"ELOG       " },
	{ CBMEM_ID_COVERAGE,		"COVERAGE   " },
	{ CBMEM_ID_ROMSTAGE_INFO,	"ROMSTAGE   " },
	{ CBMEM_ID_ROMSTAGE_RAM_STACK,	"ROMSTG STCK" },
	{ CBMEM_ID_RAMSTAGE,		"RAMSTAGE   " },
	{ CBMEM_ID_RAMSTAGE_CACHE,	"RAMSTAGE $ " },
	{ CBMEM_ID_ROOT,		"CBMEM ROOT " },
	{ CBMEM_ID_VBOOT_HANDOFF,	"VBOOT      " },
	{ CBMEM_ID_CAR_GLOBALS,		"CAR GLOBALS" },
};

void cbmem_print_entry(int n, uint32_t id, uint64_t base, uint64_t size)
{
	int i;
	const char *name;

	name = NULL;
	for (i = 0; i < ARRAY_SIZE(cbmem_ids); i++) {
		if (cbmem_ids[i].id == id) {
			name = cbmem_ids[i].name;
			break;
		}
	}

	printf("%2d. ", n);
	if (name == NULL)
		printf("%08x ", id);
	else
		printf("%s", name);
	printf("  %08" PRIx64 " ", base);
	printf("  %08" PRIx64 "\n", size);
}

static void dump_static_cbmem_toc(struct cbmem_entry *entries)
{
	int i;

	printf("CBMEM table of contents:\n");
	printf("    ID           START      LENGTH\n");

	for (i=0; i<MAX_CBMEM_ENTRIES; i++) {
		if (entries[i].magic != CBMEM_MAGIC)
			break;
		cbmem_print_entry(i, entries[i].id,
				entries[i].base, entries[i].size);
	}
}

static void dump_dynamic_cbmem_toc(struct cbmem_root *root)
{
	int i;
	debug("CBMEM: max_entries=%d num_entries=%d locked=0x%x, size=%d\n\n",
		root->max_entries, root->num_entries, root->locked, root->size);

	printf("CBMEM table of contents:\n");
	printf("    ID           START      LENGTH\n");

	for (i = 0; i < root->num_entries; i++) {
		if(root->entries[i].magic != CBMEM_ENTRY_MAGIC)
			break;
		cbmem_print_entry(i, root->entries[i].id,
			root->entries[i].start, root->entries[i].size);
	}
}

static void dump_cbmem_toc(void)
{
	uint64_t start;
	void *cbmem_area;
	struct cbmem_entry *entries;

	if (cbmem.type != LB_MEM_TABLE) {
		fprintf(stderr, "No coreboot CBMEM area found!\n");
		return;
	}

	start = unpack_lb64(cbmem.start);

	cbmem_area = map_memory(start);
	entries = (struct cbmem_entry *)cbmem_area;

	if (entries[0].magic == CBMEM_MAGIC) {
		dump_static_cbmem_toc(entries);
	} else {
		uint64_t rootptr;

		rootptr = unpack_lb64(cbmem.start) + unpack_lb64(cbmem.size);
		rootptr &= ~(DYN_CBMEM_ALIGN_SIZE - 1);
		rootptr -= sizeof(struct cbmem_root_pointer);
		unmap_memory();
		struct cbmem_root_pointer *r =
			(struct cbmem_root_pointer *)map_memory(rootptr);
		if (r->magic == CBMEM_POINTER_MAGIC) {
			struct cbmem_root *root;
			uint64_t rootaddr = r->root;
			unmap_memory();
			/* Note that this only works because our default mmap
			 * size is 1MiB which happens to be larger than the
			 * root entry size which is default to be 4KiB.
			 */
			root = (struct cbmem_root *)map_memory(rootaddr);
			dump_dynamic_cbmem_toc(root);
		} else
			fprintf(stderr, "No valid coreboot CBMEM root pointer found.\n");
	}

	unmap_memory();
}

#define COVERAGE_MAGIC 0x584d4153
struct file {
	uint32_t magic;
	uint32_t next;
	uint32_t filename;
	uint32_t data;
	int offset;
	int len;
};

static int mkpath(char *path, mode_t mode)
{
	assert (path && *path);
	char *p;
	for (p = strchr(path+1, '/'); p; p = strchr(p + 1, '/')) {
		*p = '\0';
		if (mkdir(path, mode) == -1) {
			if (errno != EEXIST) {
				*p = '/';
				return -1;
			}
		}
		*p = '/';
	}
	return 0;
}

static void dump_coverage(void)
{
	int i, found = 0;
	uint64_t start;
	struct cbmem_entry *entries;
	void *coverage;
	unsigned long phys_offset;
#define phys_to_virt(x) ((void *)(unsigned long)(x) + phys_offset)

	if (cbmem.type != LB_MEM_TABLE) {
		fprintf(stderr, "No coreboot table area found!\n");
		return;
	}

	start = unpack_lb64(cbmem.start);

	entries = (struct cbmem_entry *)map_memory(start);

	for (i=0; i<MAX_CBMEM_ENTRIES; i++) {
		if (entries[i].magic != CBMEM_MAGIC)
			break;
		if (entries[i].id == CBMEM_ID_COVERAGE) {
			found = 1;
			break;
		}
	}

	if (!found) {
		unmap_memory();
		fprintf(stderr, "No coverage information found in"
			" CBMEM area.\n");
		return;
	}

	start = entries[i].base;
	unmap_memory();
	/* Map coverage area */
	coverage = map_memory(start);
	phys_offset = (unsigned long)coverage - (unsigned long)start;

	printf("Dumping coverage data...\n");

	struct file *file = (struct file *)coverage;
	while (file && file->magic == COVERAGE_MAGIC) {
		FILE *f;
		char *filename;

		debug(" -> %s\n", (char *)phys_to_virt(file->filename));
		filename = strdup((char *)phys_to_virt(file->filename));
		if (mkpath(filename, 0755) == -1) {
			perror("Directory for coverage data could "
				"not be created");
			exit(1);
		}
		f = fopen(filename, "wb");
		if (!f) {
			printf("Could not open %s: %s\n",
				filename, strerror(errno));
			exit(1);
		}
		if (fwrite((void *)phys_to_virt(file->data),
						file->len, 1, f) != 1) {
			printf("Could not write to %s: %s\n",
				filename, strerror(errno));
			exit(1);
		}
		fclose(f);
		free(filename);

		if (file->next)
			file = (struct file *)phys_to_virt(file->next);
		else
			file = NULL;
	}
	unmap_memory();
}

static void print_version(void)
{
	printf("cbmem v%s -- ", CBMEM_VERSION);
	printf("Copyright (C) 2012 The ChromiumOS Authors.  All rights reserved.\n\n");
	printf(
    "This program is free software: you can redistribute it and/or modify\n"
    "it under the terms of the GNU General Public License as published by\n"
    "the Free Software Foundation, version 2 of the License.\n\n"
    "This program is distributed in the hope that it will be useful,\n"
    "but WITHOUT ANY WARRANTY; without even the implied warranty of\n"
    "MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the\n"
    "GNU General Public License for more details.\n\n"
    "You should have received a copy of the GNU General Public License\n"
    "along with this program.  If not, see <http://www.gnu.org/licenses/>.\n\n");
}

static void print_usage(const char *name)
{
	printf("usage: %s [-cCltxVvh?]\n", name);
	printf("\n"
	     "   -c | --console:                   print cbmem console\n"
	     "   -C | --coverage:                  dump coverage information\n"
	     "   -l | --list:                      print cbmem table of contents\n"
	     "   -x | --hexdump:                   print hexdump of cbmem area\n"
	     "   -t | --timestamps:                print timestamp information\n"
	     "   -V | --verbose:                   verbose (debugging) output\n"
	     "   -v | --version:                   print the version\n"
	     "   -h | --help:                      print this help\n"
	     "\n");
	exit(1);
}

int main(int argc, char** argv)
{
	int print_defaults = 1;
	int print_console = 0;
	int print_coverage = 0;
	int print_list = 0;
	int print_hexdump = 0;
	int print_timestamps = 0;

	int opt, option_index = 0;
	static struct option long_options[] = {
		{"console", 0, 0, 'c'},
		{"coverage", 0, 0, 'C'},
		{"list", 0, 0, 'l'},
		{"timestamps", 0, 0, 't'},
		{"hexdump", 0, 0, 'x'},
		{"verbose", 0, 0, 'V'},
		{"version", 0, 0, 'v'},
		{"help", 0, 0, 'h'},
		{0, 0, 0, 0}
	};
	while ((opt = getopt_long(argc, argv, "cCltxVvh?",
				  long_options, &option_index)) != EOF) {
		switch (opt) {
		case 'c':
			print_console = 1;
			print_defaults = 0;
			break;
		case 'C':
			print_coverage = 1;
			print_defaults = 0;
			break;
		case 'l':
			print_list = 1;
			print_defaults = 0;
			break;
		case 'x':
			print_hexdump = 1;
			print_defaults = 0;
			break;
		case 't':
			print_timestamps = 1;
			print_defaults = 0;
			break;
		case 'V':
			verbose = 1;
			break;
		case 'v':
			print_version();
			exit(0);
			break;
		case 'h':
		case '?':
		default:
			print_usage(argv[0]);
			exit(0);
			break;
		}
	}

	fd = open("/dev/mem", O_RDONLY, 0);
	if (fd < 0) {
		fprintf(stderr, "Failed to gain memory access: %s\n",
			strerror(errno));
		return 1;
	}

#ifdef __arm__
	int dt_fd;
	uint32_t cbtable_base;

	dt_fd = open("/proc/device-tree/firmware/coreboot/coreboot-table",
			O_RDONLY, 0);
	if (dt_fd < 0) {
		fprintf(stderr, "Failed to open device tree node: %s\n",
			strerror(errno));
		return 1;
	}

	if (read(dt_fd, &cbtable_base, 4) != 4) {
		fprintf(stderr, "Failed to read device tree node: %s\n",
			strerror(errno));
		return 1;
	}
	close(dt_fd);

	parse_cbtable(ntohl(cbtable_base));
#else
	int j;
	static const int possible_base_addresses[] = { 0, 0xf0000 };

	/* Find and parse coreboot table */
	for (j = 0; j < ARRAY_SIZE(possible_base_addresses); j++) {
		if (parse_cbtable(possible_base_addresses[j]))
			break;
	}
#endif

	if (print_console)
		dump_console();

	if (print_coverage)
		dump_coverage();

	if (print_list)
		dump_cbmem_toc();

	if (print_hexdump)
		dump_cbmem_hex();

	if (print_defaults || print_timestamps)
		dump_timestamps();

	close(fd);
	return 0;
}
