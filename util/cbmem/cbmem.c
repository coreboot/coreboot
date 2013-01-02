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

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <getopt.h>

#include "stdlib.h"
#include "boot/coreboot_tables.h"

typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

#include "cbmem.h"
#include "timestamp.h"

#define CBMEM_VERSION "1.0"

/* File descriptor used to access /dev/mem */
static FILE* fd;

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
 * Starting at 'offset' read 'size' bytes from the previously opened /dev/mem
 * into the 'buffer'.
 *
 * Return zero on success or exit on any error.
 */
static int readmem(void* buffer, u32 offset,  int size)
{
	if (fseek(fd, offset, SEEK_SET)) {
		fprintf(stderr, "fseek failed(%d) for offset %d\n",
			errno, offset);
		exit(1);
	}
	if (fread(buffer, 1, size, fd) != size) {
		fprintf(stderr, "failed (%d) to read %d bytes at 0x%x\n",
			errno, size, offset);
		exit(1);
	}
	return 0;
}

/*
 * Try finding the timestamp table starting from the passed in memory offset.
 * Could be called recursively in case a forwarding entry is found.
 *
 * Returns pointer to a memory buffer containg the timestamp table or zero if
 * none found.
 */
static const struct timestamp_table *find_tstamps(u64 address)
{
	int i;

	/* look at every 16 bytes within 4K of the base */
	for (i = 0; i < 0x1000; i += 0x10) {
		void *buf;
		struct lb_header lbh;
		struct lb_record* lbr_p;
		int j;

		readmem(&lbh, address + i, sizeof(lbh));
		if (memcmp(lbh.signature, "LBIO", sizeof(lbh.signature)) ||
		    !lbh.header_bytes ||
		    ipchcksum(&lbh, sizeof(lbh)))
			continue;

		/* good lb_header is found, try reading the table */
		buf = malloc(lbh.table_bytes);
		if (!buf) {
			fprintf(stderr, "failed to allocate %d bytes\n",
				lbh.table_bytes);
			exit(1);
		}

		readmem(buf, address + i + lbh.header_bytes, lbh.table_bytes);
		if (ipchcksum(buf, lbh.table_bytes) !=
		    lbh.table_checksum) {
			/* False positive or table corrupted... */
			free(buf);
			continue;
		}

		for (j = 0; j < lbh.table_bytes; j += lbr_p->size) {
			/* look for the timestamp table */
			lbr_p = (struct lb_record*) ((char *)buf + j);
			switch (lbr_p->tag) {
			case LB_TAG_TIMESTAMPS: {
				struct lb_cbmem_ref *cbmr_p =
					(struct lb_cbmem_ref *) lbr_p;
				int new_size;
				struct timestamp_table *tst_p;
				u32 stamp_addr = (u32)
					((uintptr_t)(cbmr_p->cbmem_addr));

				readmem(buf, stamp_addr,
					sizeof(struct timestamp_table));
				tst_p = (struct timestamp_table *) buf;
				new_size = sizeof(struct timestamp_table) +
					tst_p->num_entries *
					sizeof(struct timestamp_entry);
				buf = realloc(buf, new_size);
				if (!buf) {
					fprintf(stderr,
						"failed to reallocate %d bytes\n",
						new_size);
					exit(1);
				}
				readmem(buf, stamp_addr, new_size);
				return buf;
			}
			case LB_TAG_FORWARD: {
				/*
				 * This is a forwarding entry - repeat the
				 * search at the new address.
				 */
				struct lb_forward *lbf_p =
					(struct lb_forward *) lbr_p;

				free(buf);
				return find_tstamps(lbf_p->forward);
			}
			default:
				break;
			}

		}
	}
	return 0;
}

/*
 * read CPU frequency from a sysfs file, return an frequency in Kilohertz as
 * an int or exit on any error.
 */
static u64 get_cpu_freq_KHz()
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
		fprintf(stderr, "Could not open %s\n", freq_file);
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
static void dump_timestamps(const struct timestamp_table *tst_p)
{
	int i;
	u64 cpu_freq_MHz = get_cpu_freq_KHz() / 1000;

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
	     "   -v | --version:                   print the version\n"
	     "   -h | --help:                      print this help\n"
	     "\n");
	exit(1);
}

int main(int argc, char** argv)
{
	int j;
	static const int possible_base_addresses[] = { 0, 0xf0000 };

	int opt, option_index = 0;
	static struct option long_options[] = {
		{"version", 0, 0, 'v'},
		{"help", 0, 0, 'h'},
		{0, 0, 0, 0}
	};
	while ((opt = getopt_long(argc, argv, "vh?",
				  long_options, &option_index)) != EOF) {
		switch (opt) {
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

	fd = fopen("/dev/mem", "r");
	if (!fd) {
		printf("failed to gain memory access\n");
		return 1;
	}

	for (j = 0; j < ARRAY_SIZE(possible_base_addresses); j++) {
		const struct timestamp_table * tst_p =
			find_tstamps(possible_base_addresses[j]);

		if (tst_p) {
			dump_timestamps(tst_p);
			free((void*)tst_p);
			break;
		}
	}

	fclose(fd);
	return 0;
}
