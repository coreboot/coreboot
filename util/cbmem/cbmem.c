/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2012 The ChromiumOS Authors.  All rights reserved.
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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <getopt.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>

#define ARRAY_SIZE(a) (sizeof(a) / sizeof((a)[0]))
#define MAP_BYTES (1024*1024)

#include "boot/coreboot_tables.h"

typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

#include "cbmem.h"
#include "timestamp.h"

#define CBMEM_VERSION "1.0"

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
	int page = getpagesize();

	/* Mapped memory must be aligned to page size */
	p = physical & ~(page - 1);

	debug("Mapping 1MB of physical memory at %zx.\n", p);

	v = mmap(NULL, MAP_BYTES, PROT_READ, MAP_SHARED, fd, p);

	if (v == MAP_FAILED) {
		fprintf(stderr, "Failed to mmap /dev/mem: %s\n",
			strerror(errno));
		exit(1);
	}

	/* Remember what we actually mapped ... */
	mapped_virtual = v;

	/* ... but return address to the physical memory that was requested */
	v += physical & (page-1);

	return v;
}

static void unmap_memory(void)
{
	debug("Unmapping 1MB of virtual memory at %p.\n", mapped_virtual);
	munmap(mapped_virtual, MAP_BYTES);
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

static int parse_cbtable(u64 address)
{
	int i, found = 0;
	void *buf;

	debug("Looking for coreboot table at %lx\n", address);
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
			case LB_TAG_TIMESTAMPS: {
				debug("Found timestamp table\n");
				timestamps = *(struct lb_cbmem_ref *) lbr_p;
				continue;
			}
			case LB_TAG_CBMEM_CONSOLE: {
				debug("Found cbmem console\n");
				console = *(struct lb_cbmem_ref *) lbr_p;
				continue;
			}
			case LB_TAG_FORWARD: {
				/*
				 * This is a forwarding entry - repeat the
				 * search at the new address.
				 */
				struct lb_forward lbf_p =
					*(struct lb_forward *) lbr_p;
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
	printf("%s", (char *)console_p + 8);

	unmap_memory();
}


void print_version(void)
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

void print_usage(const char *name)
{
	printf("usage: %s [-vh?]\n", name);
	printf("\n"
	     "   -c | --console:                   verbose (debugging) output\n"
	     "   -t | --timestamps:                verbose (debugging) output\n"
	     "   -V | --verbose:                   verbose (debugging) output\n"
	     "   -v | --version:                   print the version\n"
	     "   -h | --help:                      print this help\n"
	     "\n");
	exit(1);
}

int main(int argc, char** argv)
{
	int j;
	static const int possible_base_addresses[] = { 0, 0xf0000 };

	int print_defaults = 1;
	int print_console = 0;
	int print_timestamps = 0;

	int opt, option_index = 0;
	static struct option long_options[] = {
		{"console", 0, 0, 'c'},
		{"timestamps", 0, 0, 't'},
		{"verbose", 0, 0, 'V'},
		{"version", 0, 0, 'v'},
		{"help", 0, 0, 'h'},
		{0, 0, 0, 0}
	};
	while ((opt = getopt_long(argc, argv, "ctVvh?",
				  long_options, &option_index)) != EOF) {
		switch (opt) {
		case 'c':
			print_console = 1;
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

	/* Find and parse coreboot table */
	for (j = 0; j < ARRAY_SIZE(possible_base_addresses); j++) {
		if (parse_cbtable(possible_base_addresses[j]))
			break;
	}

	if (print_console)
		dump_console();

	if (print_defaults || print_timestamps)
		dump_timestamps();

	close(fd);
	return 0;
}
