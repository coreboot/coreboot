/* SPDX-License-Identifier: GPL-2.0-only */

#include <inttypes.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <getopt.h>
#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <ctype.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <libgen.h>
#include <assert.h>
#include <regex.h>
#include <commonlib/bsd/cbmem_id.h>
#include <commonlib/bsd/ipchksum.h>
#include <commonlib/bsd/tpm_log_defs.h>
#include <commonlib/loglevel.h>
#include <commonlib/timestamp_serialized.h>
#include <commonlib/tpm_log_serialized.h>
#include <commonlib/coreboot_tables.h>

#ifdef __OpenBSD__
#include <sys/param.h>
#include <sys/sysctl.h>
#endif

#if defined(__i386__) || defined(__x86_64__)
#include <x86intrin.h>
#endif

struct mapping {
	void *virt;
	size_t offset;
	size_t virt_size;
	unsigned long long phys;
	size_t size;
};

#define CBMEM_VERSION "1.1"

/* verbose output? */
static int verbose = 0;
#define debug(x...) if(verbose) printf(x)

/* File handle used to access /dev/mem */
static int mem_fd;
static struct mapping lbtable_mapping;

static void die(const char *msg)
{
	if (msg)
		fputs(msg, stderr);
	exit(1);
}

static unsigned long long system_page_size(void)
{
	static unsigned long long page_size;

	if (!page_size)
		page_size = getpagesize();

	return page_size;
}

static inline size_t size_to_mib(size_t sz)
{
	return sz >> 20;
}

/* Return mapping of physical address requested. */
static void *mapping_virt(const struct mapping *mapping)
{
	char *v = mapping->virt;

	if (v == NULL)
		return NULL;

	return v + mapping->offset;
}

/* Returns virtual address on success, NULL on error. mapping is filled in. */
static void *map_memory_with_prot(struct mapping *mapping,
				  unsigned long long phys, size_t sz, int prot)
{
	void *v;
	unsigned long long page_size;

	page_size = system_page_size();

	mapping->virt = NULL;
	mapping->offset = phys % page_size;
	mapping->virt_size = sz + mapping->offset;
	mapping->size = sz;
	mapping->phys = phys;

	if (size_to_mib(mapping->virt_size) == 0) {
		debug("Mapping %zuB of physical memory at 0x%llx (requested 0x%llx).\n",
			mapping->virt_size, phys - mapping->offset, phys);
	} else {
		debug("Mapping %zuMB of physical memory at 0x%llx (requested 0x%llx).\n",
			size_to_mib(mapping->virt_size), phys - mapping->offset,
			phys);
	}

	v = mmap(NULL, mapping->virt_size, prot, MAP_SHARED, mem_fd,
			phys - mapping->offset);

	if (v == MAP_FAILED) {
		debug("Mapping failed %zuB of physical memory at 0x%llx.\n",
			mapping->virt_size, phys - mapping->offset);
		return NULL;
	}

	mapping->virt = v;

	if (mapping->offset != 0)
		debug("  ... padding virtual address with 0x%zx bytes.\n",
			mapping->offset);

	return mapping_virt(mapping);
}

/* Convenience helper for the common case of read-only mappings. */
static const void *map_memory(struct mapping *mapping, unsigned long long phys,
			      size_t sz)
{
	return map_memory_with_prot(mapping, phys, sz, PROT_READ);
}


/* Returns 0 on success, < 0 on error. mapping is cleared if successful. */
static int unmap_memory(struct mapping *mapping)
{
	if (mapping->virt == NULL)
		return -1;

	munmap(mapping->virt, mapping->virt_size);
	mapping->virt = NULL;
	mapping->offset = 0;
	mapping->virt_size = 0;

	return 0;
}

/* Return size of physical address mapping requested. */
static size_t mapping_size(const struct mapping *mapping)
{
	if (mapping->virt == NULL)
		return 0;

	return mapping->size;
}

/*
 * Some architectures map /dev/mem memory in a way that doesn't support
 * unaligned accesses. Most normal libc memcpy()s aren't safe to use in this
 * case, so build our own which makes sure to never do unaligned accesses on
 * *src (*dest is fine since we never map /dev/mem for writing).
 */
static void *aligned_memcpy(void *dest, const void *src, size_t n)
{
	uint8_t *d = dest;
	const volatile uint8_t *s = src;	/* volatile to prevent optimization */

	while ((uintptr_t)s & (sizeof(size_t) - 1)) {
		if (n-- == 0)
			return dest;
		*d++ = *s++;
	}

	while (n >= sizeof(size_t)) {
		*(size_t *)d = *(const volatile size_t *)s;
		d += sizeof(size_t);
		s += sizeof(size_t);
		n -= sizeof(size_t);
	}

	while (n-- > 0)
		*d++ = *s++;

	return dest;
}

/* This is a work-around for a nasty problem introduced by initially having
 * pointer sized entries in the lb_cbmem_ref structures. This caused problems
 * on 64bit x86 systems because coreboot is 32bit on those systems.
 * When the problem was found, it was corrected, but there are a lot of
 * systems out there with a firmware that does not produce the right
 * lb_cbmem_ref structure. Hence we try to autocorrect this issue here.
 */
static struct lb_cbmem_ref parse_cbmem_ref(const struct lb_cbmem_ref *cbmem_ref)
{
	struct lb_cbmem_ref ret;

	aligned_memcpy(&ret, cbmem_ref, sizeof(ret));

	if (cbmem_ref->size < sizeof(*cbmem_ref))
		ret.cbmem_addr = (uint32_t)ret.cbmem_addr;

	debug("      cbmem_addr = %" PRIx64 "\n", ret.cbmem_addr);

	return ret;
}

static uint32_t cbmem_id_to_lb_tag(uint32_t tag)
{
	/* Minimal subset. Expand based on the CBMEM to coreboot table
	   records mapping in lib/coreboot_table.c */
	switch (tag) {
	case CBMEM_ID_TIMESTAMP:
		return LB_TAG_TIMESTAMPS;
	case CBMEM_ID_CONSOLE:
		return LB_TAG_CBMEM_CONSOLE;
	case CBMEM_ID_TPM_CB_LOG:
		return LB_TAG_TPM_CB_LOG;
	}
	return LB_TAG_UNUSED;
}

struct cbmem_console {
	uint32_t size;
	uint32_t cursor;
	uint8_t body[];
} __packed;

#define CBMC_CURSOR_MASK ((1 << 28) - 1)
#define CBMC_OVERFLOW    (1 << 31)


/* Find the first cbmem entry filling in the details. */
static int find_cbmem_entry(uint32_t id, uint64_t *addr, size_t *size)
{
	const uint8_t *table;
	size_t offset = 0;
	const uint32_t legacy_tag = cbmem_id_to_lb_tag(id);
	struct lb_cbmem_ref *ref = NULL;

	table = mapping_virt(&lbtable_mapping);

	if (table == NULL)
		return -1;

	const struct lb_record *lbr = NULL;
	while (offset < mapping_size(&lbtable_mapping)) {
		lbr = (const void *)(table + offset);
		offset += lbr->size;

		/* Store coreboot table entry for later if CBMEM entry does not exist.
		   CBMEM entry stores size including the reserved area, so prefer it,
		   so more potential data and/or space is available. */
		if (legacy_tag != LB_TAG_UNUSED && lbr->tag == legacy_tag)
			ref = (struct lb_cbmem_ref *)lbr;

		if (lbr->tag != LB_TAG_CBMEM_ENTRY)
			continue;

		struct lb_cbmem_entry lbe;
		aligned_memcpy(&lbe, lbr, sizeof(lbe));
		if (lbe.id != id)
			continue;

		*addr = lbe.address;
		*size = lbe.entry_size;
		return 0;
	}

	/* No mapping and/or no potential reference means that
	   the requested entry does not exit. */
	if (legacy_tag == LB_TAG_UNUSED || ref == NULL)
		return -1;

	debug("Found coreboot table record equivalent of CBMEM entry id: %#x, tag: %#x\n", id,
	      legacy_tag);

	const struct lb_cbmem_ref lbc = parse_cbmem_ref(ref);
	size_t header_map_size = 0;

	/* Process legacy coreboot table entries */
	switch (lbc.tag) {
	case LB_TAG_TIMESTAMPS:
		header_map_size = sizeof(struct timestamp_table);
		break;
	case LB_TAG_CBMEM_CONSOLE:
		header_map_size = sizeof(struct cbmem_console);
		break;
	case LB_TAG_TPM_CB_LOG:
		header_map_size = sizeof(struct tpm_cb_log_table);
		break;
	}

	struct mapping entry_mapping;
	const void *entry_header = NULL;

	entry_header = map_memory(&entry_mapping, lbc.cbmem_addr, header_map_size);
	if (!entry_header) {
		fprintf(stderr, "Unable to map header for coreboot table entry id: %#x\n",
			legacy_tag);
		abort();
	}

	*addr = lbc.cbmem_addr;

	switch (legacy_tag) {
	case LB_TAG_TIMESTAMPS: {
		const struct timestamp_table *tst_p = entry_header;
		*size = sizeof(*tst_p) + tst_p->num_entries * sizeof(tst_p->entries[0]);
		break;
	}
	case LB_TAG_CBMEM_CONSOLE: {
		const struct cbmem_console *console_p = entry_header;
		*size = sizeof(*console_p) + console_p->size;
		break;
	}
	case LB_TAG_TPM_CB_LOG: {
		const struct tpm_cb_log_table *tclt_p = entry_header;
		*size = sizeof(*tclt_p) + tclt_p->num_entries * sizeof(tclt_p->entries[0]);
		break;
	}
	}

	unmap_memory(&entry_mapping);

	return 0;
}

/**
 * Returns pointer to allocated buffer and size of the buffer in parameters.
 * Returns zero on success.
 * free() buffer after use.
 */
static int get_cbmem_entry(uint32_t id, uint8_t **buf_out, size_t *size_out)
{
	uint64_t addr;
	size_t size;
	struct mapping cbmem_mapping;

	if (find_cbmem_entry(id, &addr, &size)) {
		debug("CBMEM entry not found. CBMEM id: %#x\n", id);
		return -1;
	}

	const uint8_t *buf = map_memory(&cbmem_mapping, addr, size);
	if (!buf) {
		fprintf(stderr, "Unable to map CBMEM entry id: %#x, size: %zu\n", id, size);
		abort();
	}

	*buf_out = malloc(size);
	if (!*buf_out) {
		unmap_memory(&cbmem_mapping);
		fprintf(stderr,
			"Unable to allocate memory for CBMEM entry id: %#x, size: %zu\n", id,
			size);
		abort();
	}

	aligned_memcpy(*buf_out, buf, size);
	unmap_memory(&cbmem_mapping);
	*size_out = size;
	return 0;
}

/* Return < 0 on error, 0 on success. */
static int parse_cbtable(uint64_t address, size_t table_size)
{
	const uint8_t *buf;
	struct mapping header_mapping;
	size_t req_size;
	size_t i;

	req_size = table_size;
	/* Default to 4 KiB search space. */
	if (req_size == 0)
		req_size = 4 * 1024;

	debug("Looking for coreboot table at %" PRIx64 " %zd bytes.\n", address, req_size);

	buf = map_memory(&header_mapping, address, req_size);

	if (!buf)
		return -1;

	/* look at every 16 bytes */
	for (i = 0; i <= req_size - sizeof(struct lb_header); i += 16) {
		const struct lb_header *lbh;
		struct mapping table_mapping;

		lbh = (const struct lb_header *)&buf[i];
		if (memcmp(lbh->signature, "LBIO", sizeof(lbh->signature)) ||
		    !lbh->header_bytes ||
		    ipchksum(lbh, sizeof(*lbh))) {
			continue;
		}

		/* Map in the whole table to parse. */
		if (!map_memory(&table_mapping, address + i + lbh->header_bytes,
				 lbh->table_bytes)) {
			debug("Couldn't map in table\n");
			continue;
		}

		if (ipchksum(mapping_virt(&table_mapping), lbh->table_bytes) !=
		    lbh->table_checksum) {
			debug("Signature found, but wrong checksum.\n");
			unmap_memory(&table_mapping);
			continue;
		}

		debug("Found at %#" PRIx64 "\n", address + i);

		const struct lb_record *lbr_p;
		const uint8_t *lbtable = mapping_virt(&table_mapping);

		for (size_t offset = 0; offset < lbh->table_bytes; offset += lbr_p->size) {
			lbr_p = (const struct lb_record *)&lbtable[offset];
			debug("  coreboot table entry 0x%02x\n", lbr_p->tag);

			if (lbr_p->tag != LB_TAG_FORWARD)
				continue;

			/* This is a forwarding entry. Repeat the search at the new address. */
			struct lb_forward lbf_p = *(const struct lb_forward *)lbr_p;
			debug("    Found forwarding entry.\n");

			const uint64_t next_addr = lbf_p.forward;
			unmap_memory(&header_mapping);
			unmap_memory(&table_mapping);

			return parse_cbtable(next_addr, 0);
		}

		debug("correct coreboot table found.\n");
		unmap_memory(&header_mapping);
		lbtable_mapping = table_mapping;

		return 0;
	}

	unmap_memory(&header_mapping);

	return -1;
}

static void lb_table_get_entry(uint32_t tag, uint8_t **buf_out, size_t *size_out)
{
	const struct lb_record *lbr_p;
	const uint8_t *lbtable_raw;
	size_t table_size;
	bool tag_found = false;

	if (get_cbmem_entry(CBMEM_ID_CBTABLE, (uint8_t **)&lbtable_raw, &table_size)) {
		fprintf(stderr, "coreboot table not found.\n");
		abort();
	}

	const struct lb_header *lbh = (const struct lb_header *)lbtable_raw;

	for (size_t i = 0; i < lbh->table_bytes; i += lbr_p->size) {
		lbr_p = (const struct lb_record *)(&lbtable_raw[lbh->header_bytes + i]);
		if (lbr_p->tag == tag) {
			tag_found = true;
			break;
		}
	}

	if (!tag_found) {
		fprintf(stderr, "coreboot table entry %#x not found.\n", tag);
		free((void *)lbtable_raw);
		abort();
	}

	debug("coreboot table entry %#x found.\n", tag);

	*buf_out = malloc(lbr_p->size);
	if (!*buf_out) {
		fprintf(stderr,
			"Unable to allocate memory for coreboot table entry %#x, size: %d\n",
			tag, lbr_p->size);
		free((void *)lbtable_raw);
		abort();
	}
	memcpy(*buf_out, lbr_p, lbr_p->size);
	*size_out = lbr_p->size;
	free((void *)lbtable_raw);
}


/**
 * Returns mapping, mapped buffer pointer and its size by parameters.
 * Returns zero on success.
 * unmap_mempry() after use.
 */
static int map_cbmem_entry_rw(uint32_t id, struct mapping *mapping, uint8_t **buf_out,
			       size_t *size_out)
{
	uint64_t addr;
	size_t size;

	if (find_cbmem_entry(id, &addr, &size)) {
		debug("CBMEM entry not found. CBMEM id: %#x\n", id);
		return -1;
	}

	*buf_out = map_memory_with_prot(mapping, addr, size, PROT_READ | PROT_WRITE);
	if (!*buf_out) {
		fprintf(stderr,
			"Unable to map CBMEM entry id: %#x, size: %zu for read-write access.\n",
			id, size);
		abort();
	}

	*size_out = size;
	return 0;
}

#if defined(linux) && (defined(__i386__) || defined(__x86_64__))
/*
 * read CPU frequency from a sysfs file, return an frequency in Megahertz as
 * an int or exit on any error.
 */
static unsigned long arch_tick_frequency(void)
{
	FILE *cpuf;
	char freqs[100];
	int  size;
	char *endp;
	uint64_t rv;

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
	/* cpuinfo_max_freq is in kHz. Convert it to MHz. */
		return rv / 1000;
	fprintf(stderr, "Wrong formatted value ^%s^ read from %s\n",
		freqs, freq_file);
	exit(1);
}
#elif defined(__OpenBSD__) && (defined(__i386__) || defined(__x86_64__))
static unsigned long arch_tick_frequency(void)
{
	int mib[2] = { CTL_HW, HW_CPUSPEED };
	static int value = 0;
	size_t value_len = sizeof(value);

	/* Return 1 MHz when sysctl fails. */
	if ((value == 0) && (sysctl(mib, 2, &value, &value_len, NULL, 0) == -1))
		return 1;

	return value;
}
#else
static unsigned long arch_tick_frequency(void)
{
	/* 1 MHz = 1us. */
	return 1;
}
#endif

static unsigned long tick_freq_mhz;

static void timestamp_set_tick_freq(unsigned long table_tick_freq_mhz)
{
	tick_freq_mhz = table_tick_freq_mhz;

	/* Honor table frequency if present. */
	if (!tick_freq_mhz)
		tick_freq_mhz = arch_tick_frequency();

	if (!tick_freq_mhz) {
		fprintf(stderr, "Cannot determine timestamp tick frequency.\n");
		exit(1);
	}

	debug("Timestamp tick frequency: %ld MHz\n", tick_freq_mhz);
}

static uint64_t arch_convert_raw_ts_entry(uint64_t ts)
{
	return ts / tick_freq_mhz;
}

/*
 * Print an integer in 'normalized' form - with commas separating every three
 * decimal orders.
 */
static void print_norm(uint64_t v)
{
	if (v >= 1000) {
		/* print the higher order sections first */
		print_norm(v / 1000);
		printf(",%3.3u", (uint32_t)(v % 1000));
	} else {
		printf("%u", (uint32_t)(v % 1000));
	}
}

static uint64_t timestamp_get(uint64_t table_tick_freq_mhz)
{
#if defined(__i386__) || defined(__x86_64__)
	uint64_t tsc = __rdtsc();
	struct lb_tsc_info *tsc_info;
	size_t size;

	/* No tick frequency specified means raw TSC values. */
	if (!table_tick_freq_mhz)
		return tsc;

	lb_table_get_entry(LB_TAG_TSC_INFO, (uint8_t **)&tsc_info, &size);

	const uint32_t tsc_freq_khz = tsc_info->freq_khz;

	free(tsc_info);

	if (tsc_freq_khz)
		return tsc * table_tick_freq_mhz * 1000 / tsc_freq_khz;
#else
	(void)table_tick_freq_mhz;
#endif
	die("Don't know how to obtain timestamps on this platform.\n");
	return 0;
}

static const char *timestamp_name(uint32_t id)
{
	for (size_t i = 0; i < ARRAY_SIZE(timestamp_ids); i++) {
		if (timestamp_ids[i].id == id)
			return timestamp_ids[i].name;
	}
	return "<unknown>";
}

static uint32_t timestamp_enum_name_to_id(const char *name)
{
	for (size_t i = 0; i < ARRAY_SIZE(timestamp_ids); i++) {
		if (!strcmp(timestamp_ids[i].enum_name, name))
			return timestamp_ids[i].id;
	}
	return 0;
}

static uint64_t timestamp_print_parseable_entry(uint32_t id, uint64_t stamp,
						uint64_t prev_stamp)
{
	const char *name;
	uint64_t step_time;

	name = timestamp_name(id);

	step_time = arch_convert_raw_ts_entry(stamp - prev_stamp);

	/* ID<tab>absolute time<tab>relative time<tab>description */
	printf("%d\t", id);
	printf("%llu\t", (long long)arch_convert_raw_ts_entry(stamp));
	printf("%llu\t", (long long)step_time);
	printf("%s\n", name);

	return step_time;
}

static uint64_t timestamp_print_entry(uint32_t id, uint64_t stamp, uint64_t prev_stamp)
{
	const char *name;
	uint64_t step_time;

	name = timestamp_name(id);

	printf("%4d:", id);
	printf("%-50s", name);
	print_norm(arch_convert_raw_ts_entry(stamp));
	step_time = arch_convert_raw_ts_entry(stamp - prev_stamp);
	if (prev_stamp) {
		printf(" (");
		print_norm(step_time);
		printf(")");
	}
	printf("\n");

	return step_time;
}

static int compare_timestamp_entries(const void *a, const void *b)
{
	const struct timestamp_entry *tse_a = (struct timestamp_entry *)a;
	const struct timestamp_entry *tse_b = (struct timestamp_entry *)b;

	if (tse_a->entry_stamp > tse_b->entry_stamp)
		return 1;
	else if (tse_a->entry_stamp < tse_b->entry_stamp)
		return -1;

	return 0;
}

static int find_matching_end(struct timestamp_table *sorted_tst_p, uint32_t start, uint32_t end)
{
	uint32_t id = sorted_tst_p->entries[start].entry_id;
	uint32_t possible_match = 0;

	for (uint32_t i = 0; i < ARRAY_SIZE(timestamp_ids); ++i) {
		if (timestamp_ids[i].id == id) {
			possible_match = timestamp_ids[i].id_end;
			break;
		}
	}

	/* No match found or timestamp not defined in IDs table */
	if (!possible_match)
		return -1;

	for (uint32_t i = start + 1; i < end; i++)
		if (sorted_tst_p->entries[i].entry_id == possible_match)
			return i;

	return -1;
}

static const char *get_timestamp_name(const uint32_t id)
{
	for (uint32_t i = 0; i < ARRAY_SIZE(timestamp_ids); i++)
		if (timestamp_ids[i].id == id)
			return timestamp_ids[i].enum_name;

	return "UNKNOWN";
}

struct ts_range_stack {
	const char *name;
	const char *end_name;
	uint32_t end;
};

static void print_with_path(struct ts_range_stack *range_stack, const int stacklvl,
			    const uint64_t stamp, const char *last_part)
{
	for (int i = 1; i <= stacklvl; ++i) {
		printf("%s -> %s", range_stack[i].name, range_stack[i].end_name);
		if (i < stacklvl || last_part)
			putchar(';');
	}
	if (last_part)
		printf("%s", last_part);
	printf(" %llu\n", (long long)arch_convert_raw_ts_entry(stamp));
}

enum timestamps_print_type {
	TIMESTAMPS_PRINT_NONE,
	TIMESTAMPS_PRINT_NORMAL,
	TIMESTAMPS_PRINT_MACHINE_READABLE,
	TIMESTAMPS_PRINT_STACKED,
};

/* dump the timestamp table */
static void dump_timestamps(enum timestamps_print_type output_type)
{
	const struct timestamp_table *tst_p;
	struct timestamp_table *sorted_tst_p;
	size_t size;
	uint64_t prev_stamp = 0;
	uint64_t total_time = 0;

	if (get_cbmem_entry(CBMEM_ID_TIMESTAMP, (uint8_t **)&tst_p, &size)) {
		fprintf(stderr, "Timestamps not found.\n");
		abort();
	}

	timestamp_set_tick_freq(tst_p->tick_freq_mhz);

	if (output_type == TIMESTAMPS_PRINT_NORMAL)
		printf("%d entries total:\n\n", tst_p->num_entries);

	sorted_tst_p = malloc(size + sizeof(struct timestamp_entry));
	if (!sorted_tst_p) {
		free((void *)tst_p);
		die("Failed to allocate memory");
	}
	memcpy(sorted_tst_p, tst_p, size);

	/*
	 * Insert a timestamp to represent the base time (start of coreboot),
	 * in case we have to rebase for negative timestamps below.
	 */
	sorted_tst_p->entries[tst_p->num_entries].entry_id = 0;
	sorted_tst_p->entries[tst_p->num_entries].entry_stamp = 0;
	sorted_tst_p->num_entries += 1;

	qsort(&sorted_tst_p->entries[0], sorted_tst_p->num_entries,
	      sizeof(struct timestamp_entry), compare_timestamp_entries);

	/*
	 * If there are negative timestamp entries, rebase all of the
	 * timestamps to the lowest one in the list.
	 */
	if (sorted_tst_p->entries[0].entry_stamp < 0) {
		sorted_tst_p->base_time = -sorted_tst_p->entries[0].entry_stamp;
		prev_stamp = 0;
	} else {
		prev_stamp = tst_p->base_time;
	}

	struct ts_range_stack range_stack[20];
	range_stack[0].end = sorted_tst_p->num_entries;
	int stacklvl = 0;

	for (uint32_t i = 0; i < sorted_tst_p->num_entries; i++) {
		uint64_t stamp;
		const struct timestamp_entry *tse = &sorted_tst_p->entries[i];

		/* Make all timestamps absolute. */
		stamp = tse->entry_stamp + sorted_tst_p->base_time;
		if (output_type == TIMESTAMPS_PRINT_MACHINE_READABLE) {
			timestamp_print_parseable_entry(tse->entry_id, stamp, prev_stamp);
		} else if (output_type == TIMESTAMPS_PRINT_NORMAL) {
			total_time += timestamp_print_entry(tse->entry_id, stamp, prev_stamp);
		} else if (output_type == TIMESTAMPS_PRINT_STACKED) {
			bool end_of_range = false;
			/* Iterate over stacked entries to pop all ranges, which are closed by
			   current element. For example, assuming two ranges: (TS_A, TS_C),
			   (TS_B, TS_C) it will pop all of them instead of just last one. */
			while (stacklvl > 0 && range_stack[stacklvl].end == i) {
				end_of_range = true;
				stacklvl--;
			}

			int match =
				find_matching_end(sorted_tst_p, i, range_stack[stacklvl].end);
			if (match != -1) {
				const uint64_t match_stamp =
					sorted_tst_p->entries[match].entry_stamp
					+ sorted_tst_p->base_time;
				stacklvl++;
				assert(stacklvl < (int)ARRAY_SIZE(range_stack));
				range_stack[stacklvl].name = get_timestamp_name(tse->entry_id);
				range_stack[stacklvl].end_name = get_timestamp_name(
					sorted_tst_p->entries[match].entry_id);
				range_stack[stacklvl].end = match;
				print_with_path(range_stack, stacklvl, match_stamp - stamp,
						NULL);
			} else if (!end_of_range) {
				print_with_path(range_stack, stacklvl, stamp - prev_stamp,
						get_timestamp_name(tse->entry_id));
			}
			/* else: No match && end_of_range == true */
		}
		prev_stamp = stamp;
	}

	if (output_type == TIMESTAMPS_PRINT_NORMAL) {
		printf("\nTotal Time: ");
		print_norm(total_time);
		printf("\n");
	}

	free(sorted_tst_p);
	free((void *)tst_p);
}

/* add a timestamp entry */
static void timestamp_add_now(uint32_t timestamp_id)
{
	struct timestamp_table *tst_p;
	struct mapping timestamp_mapping;
	size_t tst_size;

	if (map_cbmem_entry_rw(CBMEM_ID_TIMESTAMP, &timestamp_mapping, (uint8_t **)&tst_p,
			       &tst_size)) {
		fprintf(stderr, "Unable to find timestamps.\n");
		abort();
	}

	/*
	 * Note that coreboot sizes the cbmem entry in the table according to
	 * max_entries, so it's OK to just add more entries if there's room.
	 */
	if (tst_p->num_entries >= tst_p->max_entries) {
		die("Not enough space to add timestamp.\n");
	} else {
		int64_t time =
			timestamp_get(tst_p->tick_freq_mhz) - tst_p->base_time;
		tst_p->entries[tst_p->num_entries].entry_id = timestamp_id;
		tst_p->entries[tst_p->num_entries].entry_stamp = time;
		tst_p->num_entries += 1;
	}

	unmap_memory(&timestamp_mapping);
}

static bool can_print(const uint8_t *data, size_t len)
{
	unsigned int i;
	for (i = 0; i < len; i++) {
		if (!isprint(data[i]) && !isspace(data[i])) {
			/* If printable prefix is followed by zeroes, this is a valid string */
			for (; i < len; i++) {
				if (data[i] != 0)
					return false;
			}
			return true;
		}
	}
	return true;
}

static void print_hex_string(const uint8_t *hex, size_t len)
{
	unsigned int i;
	for (i = 0; i < len; i++)
		printf("%02x", hex[i]);
}

static void print_hex_line(const uint8_t *hex, size_t len)
{
	print_hex_string(hex, len);
	printf("\n");
}

static void print_event_type(uint32_t event_type)
{
	unsigned int known_event_count = ARRAY_SIZE(tpm_event_types);
	if (event_type >= known_event_count)
		printf("Unknown (0x%x >= %u)", event_type, known_event_count);
	else
		printf("%s", tpm_event_types[event_type]);
}

static void parse_tpm12_log(const struct tcpa_spec_entry *spec_log)
{
	const uint8_t zero_block[sizeof(struct tcpa_spec_entry)] = {0};

	uintptr_t current;
	uint32_t counter = 0;

	printf("TCPA log:\n");
	printf("\tSpecification: %d.%d%d\n",
	       spec_log->spec_version_major,
	       spec_log->spec_version_minor,
	       spec_log->spec_errata);
	printf("\tPlatform class: %s\n",
	       le32toh(spec_log->platform_class) == 0 ? "PC Client" :
	       le32toh(spec_log->platform_class) == 1 ? "Server" : "Unknown");

	current = (uintptr_t)&spec_log->vendor_info[spec_log->vendor_info_size];
	while (memcmp((const void *)current, (const void *)zero_block, sizeof(zero_block))) {
		uint32_t len;
		struct tcpa_log_entry *log_entry = (void *)current;
		uint32_t event_type = le32toh(log_entry->event_type);

		printf("TCPA log entry %u:\n", ++counter);
		printf("\tPCR: %d\n", le32toh(log_entry->pcr));
		printf("\tEvent type: ");
		print_event_type(event_type);
		printf("\n");
		printf("\tDigest: ");
		print_hex_line(log_entry->digest, SHA1_DIGEST_SIZE);
		current += sizeof(struct tcpa_log_entry);
		len = le32toh(log_entry->event_data_size);
		if (len != 0) {
			current += len;
			printf("\tEvent data: ");
			if (can_print(log_entry->event, len))
				printf("%.*s\n", len, log_entry->event);
			else
				print_hex_line(log_entry->event, len);
		} else {
			printf("\tEvent data not provided\n");
		}
	}
}

static uint32_t print_tpm2_digests(struct tcg_pcr_event2_header *log_entry)
{
	unsigned int i;
	uintptr_t current = (uintptr_t)log_entry->digests;

	for (i = 0; i < le32toh(log_entry->digest_count); i++) {
		struct tpm_hash_algorithm *hash = (struct tpm_hash_algorithm *)current;
		switch (le16toh(hash->hashAlg)) {
		case TPM2_ALG_SHA1:
			printf("\t\t SHA1: ");
			print_hex_line(hash->digest.sha1, SHA1_DIGEST_SIZE);
			current += sizeof(hash->hashAlg) + SHA1_DIGEST_SIZE;
			break;
		case TPM2_ALG_SHA256:
			printf("\t\t SHA256: ");
			print_hex_line(hash->digest.sha256, SHA256_DIGEST_SIZE);
			current += sizeof(hash->hashAlg) + SHA256_DIGEST_SIZE;
			break;
		case TPM2_ALG_SHA384:
			printf("\t\t SHA384: ");
			print_hex_line(hash->digest.sha384, SHA384_DIGEST_SIZE);
			current += sizeof(hash->hashAlg) + SHA384_DIGEST_SIZE;
			break;
		case TPM2_ALG_SHA512:
			printf("\t\t SHA512: ");
			print_hex_line(hash->digest.sha512, SHA512_DIGEST_SIZE);
			current += sizeof(hash->hashAlg) + SHA512_DIGEST_SIZE;
			break;
		case TPM2_ALG_SM3_256:
			printf("\t\t SM3: ");
			print_hex_line(hash->digest.sm3_256, SM3_256_DIGEST_SIZE);
			current += sizeof(hash->hashAlg) + SM3_256_DIGEST_SIZE;
			break;
		default:
			die("Unknown hash algorithm\n");
		}
	}

	return current - (uintptr_t)&log_entry->digest_count;
}

static void parse_tpm2_log(const struct tcg_efi_spec_id_event *tpm2_log)
{
	const uint8_t zero_block[12] = {0}; /* Only PCR index, event type and digest count */

	uintptr_t current;
	uint32_t counter = 0;

	printf("TPM2 log:\n");
	printf("\tSpecification: %d.%d%d\n",
	       tpm2_log->spec_version_major,
	       tpm2_log->spec_version_minor,
	       tpm2_log->spec_errata);
	printf("\tPlatform class: %s\n",
	       le32toh(tpm2_log->platform_class) == 0 ? "PC Client" :
	       le32toh(tpm2_log->platform_class) == 1 ? "Server" : "Unknown");

	/* Start after the first variable-sized part of the header */
	current = (uintptr_t)&tpm2_log->digest_sizes[le32toh(tpm2_log->num_of_algorithms)];
	/* current is at `uint8_t vendor_info_size` here */
	current += 1 + *(uint8_t *)current;

	while (memcmp((const void *)current, (const void *)zero_block, sizeof(zero_block))) {
		uint32_t len;
		struct tcg_pcr_event2_header *log_entry = (void *)current;
		uint32_t event_type = le32toh(log_entry->event_type);

		printf("TPM2 log entry %u:\n", ++counter);
		printf("\tPCR: %d\n", le32toh(log_entry->pcr_index));
		printf("\tEvent type: ");
		print_event_type(event_type);
		printf("\n");

		current = (uintptr_t)&log_entry->digest_count;
		if (le32toh(log_entry->digest_count) > 0) {
			printf("\tDigests:\n");
			current += print_tpm2_digests(log_entry);
		} else {
			printf("\tNo digests in this log entry\n");
			current += sizeof(log_entry->digest_count);
		}
		/* Now event size and event are left to be parsed */
		len = le32toh(*(uint32_t *)current);
		current += sizeof(uint32_t);
		if (len != 0) {
			printf("\tEvent data: %.*s\n", len, (const char *)current);
			current += len;
		} else {
			printf("\tEvent data not provided\n");
		}
	}
}

/* Dump the TPM log table in format defined by specifications */
static void dump_tpm_std_log(void *buf)
{
	const struct tcpa_spec_entry *tspec_entry;
	const struct tcg_efi_spec_id_event *tcg_spec_entry;

	tspec_entry = buf;
	if (!strcmp((const char *)tspec_entry->signature, TCPA_SPEC_ID_EVENT_SIGNATURE)) {
		if (tspec_entry->spec_version_major == 1 &&
		    tspec_entry->spec_version_minor == 2 && tspec_entry->spec_errata >= 1 &&
		    le32toh(tspec_entry->entry.event_type) == EV_NO_ACTION) {
			parse_tpm12_log(tspec_entry);
		} else {
			fprintf(stderr, "Unknown TPM1.2 log specification\n");
		}
		return;
	}

	tcg_spec_entry = buf;
	if (!strcmp((const char *)tcg_spec_entry->signature, TCG_EFI_SPEC_ID_EVENT_SIGNATURE)) {
		if (tcg_spec_entry->spec_version_major == 2 &&
		    tcg_spec_entry->spec_version_minor == 0 &&
		    le32toh(tcg_spec_entry->event_type) == EV_NO_ACTION) {
			parse_tpm2_log(tcg_spec_entry);
		} else {
			fprintf(stderr, "Unknown TPM2 log specification.\n");
		}
		return;
	}

	fprintf(stderr, "Unknown TPM log specification: %.*s\n",
		(int)sizeof(tcg_spec_entry->signature),
		(const char *)tcg_spec_entry->signature);
}

/* dump the TPM CB log table */
static void dump_tpm_cb_log(void)
{
	const struct tpm_cb_log_table *tclt_p;
	size_t tclt_size;

	if (get_cbmem_entry(CBMEM_ID_TPM_CB_LOG, (uint8_t **)&tclt_p, &tclt_size)) {
		fprintf(stderr, "coreboot TPM log not found.\n");
		abort();
	}

	printf("coreboot TPM log:\n\n");

	for (uint16_t i = 0; i < tclt_p->num_entries; i++) {
		const struct tpm_cb_log_entry *tce = &tclt_p->entries[i];

		printf(" PCR-%u ", tce->pcr);
		print_hex_string(tce->digest, tce->digest_length);
		printf(" %s [%s]\n", tce->digest_type, tce->name);
	}

	free((void *)tclt_p);
}

static void dump_tpm_log(void)
{
	uint8_t *buf;
	size_t size;

	if (!get_cbmem_entry(CBMEM_ID_TCPA_TCG_LOG, &buf, &size) ||
	    !get_cbmem_entry(CBMEM_ID_TPM2_TCG_LOG, &buf, &size)) {
		dump_tpm_std_log(buf);
		free(buf);
	} else
		dump_tpm_cb_log();
}

enum console_print_type {
	CONSOLE_PRINT_FULL = 0,
	CONSOLE_PRINT_LAST,
	CONSOLE_PRINT_PREVIOUS,
};

static int parse_loglevel(char *arg, int *print_unknown_logs)
{
	if (arg[0] == '+') {
		*print_unknown_logs = 1;
		arg++;
	} else {
		*print_unknown_logs = 0;
	}

	char *endptr;
	int loglevel = strtol(arg, &endptr, 0);
	if (*endptr == '\0' && loglevel >= BIOS_EMERG && loglevel <= BIOS_LOG_PREFIX_MAX_LEVEL)
		return loglevel;

	/* Only match first 3 characters so `NOTE` and `NOTICE` both match. */
	for (int i = BIOS_EMERG; i <= BIOS_LOG_PREFIX_MAX_LEVEL; i++)
		if (!strncasecmp(arg, bios_log_prefix[i], 3))
			return i;

	*print_unknown_logs = 1;
	return BIOS_NEVER;
}

/* dump the cbmem console */
static void dump_console(enum console_print_type type, int max_loglevel, int print_unknown_logs)
{
	const struct cbmem_console *console_p;
	char *console_c;
	size_t size, cursor, previous;
	size_t console_buf_size;

	if (get_cbmem_entry(CBMEM_ID_CONSOLE, (uint8_t **)&console_p, &console_buf_size)) {
		fprintf(stderr, "CBMEM console not found.\n");
		abort();
	}

	cursor = console_p->cursor & CBMC_CURSOR_MASK;
	if (!(console_p->cursor & CBMC_OVERFLOW) && cursor < console_p->size)
		size = cursor;
	else
		size = console_p->size;

	console_c = malloc(size + 1);
	if (!console_c) {
		fprintf(stderr, "Not enough memory for console.\n");
		free((void *)console_p);
		abort();
	}
	console_c[size] = '\0';

	if (console_p->cursor & CBMC_OVERFLOW) {
		if (cursor >= size) {
			printf("cbmem: ERROR: CBMEM console struct is illegal, "
			       "output may be corrupt or out of order!\n\n");
			cursor = 0;
		}
		memcpy(console_c, console_p->body + cursor, size - cursor);
		memcpy(console_c + size - cursor, console_p->body, cursor);
	} else {
		memcpy(console_c, console_p->body, size);
	}

	/* Slight memory corruption may occur between reboots and give us a few
	   unprintable characters like '\0'. Replace them with '?' on output. */
	for (cursor = 0; cursor < size; cursor++)
		if (!isprint(console_c[cursor]) && !isspace(console_c[cursor])
		    && !BIOS_LOG_IS_MARKER(console_c[cursor]))
			console_c[cursor] = '?';

	/* We detect the reboot cutoff by looking for a bootblock, romstage or
	   ramstage banner, in that order (to account for platforms without
	   CONFIG_BOOTBLOCK_CONSOLE and/or CONFIG_EARLY_CONSOLE). Once we find
	   a banner, store the last two matches for that stage and stop. */
	cursor = previous = 0;
	if (type != CONSOLE_PRINT_FULL) {
#define BANNER_REGEX(stage) \
		"\n\n.?coreboot-[^\n]* " stage " starting.*\\.\\.\\.\n"
#define OVERFLOW_REGEX(stage) "\n.?\\*\\*\\* Pre-CBMEM " stage " console overflow"
		const char *regex[] = { BANNER_REGEX("verstage-before-bootblock"),
					BANNER_REGEX("bootblock"),
					BANNER_REGEX("verstage"),
					OVERFLOW_REGEX("romstage"),
					BANNER_REGEX("romstage"),
					OVERFLOW_REGEX("ramstage"),
					BANNER_REGEX("ramstage") };

		for (size_t i = 0; !cursor && i < ARRAY_SIZE(regex); i++) {
			regex_t re;
			regmatch_t match;
			int res = regcomp(&re, regex[i], REG_EXTENDED | REG_NEWLINE);
			assert(res == 0);

			/* Keep looking for matches so we find the last one. */
			while (!regexec(&re, console_c + cursor, 1, &match, 0)) {
				previous = cursor;
				cursor += match.rm_so + 1;
			}
			regfree(&re);
		}
	}

	if (type == CONSOLE_PRINT_PREVIOUS) {
		console_c[cursor] = '\0';
		cursor = previous;
	}

	char c;
	int suppressed = 0;
	int tty = isatty(fileno(stdout));
	while ((c = console_c[cursor++])) {
		if (BIOS_LOG_IS_MARKER(c)) {
			int lvl = BIOS_LOG_MARKER_TO_LEVEL(c);
			if (lvl > max_loglevel) {
				suppressed = 1;
				continue;
			}
			suppressed = 0;
			if (tty)
				printf(BIOS_LOG_ESCAPE_PATTERN, bios_log_escape[lvl]);
			printf(BIOS_LOG_PREFIX_PATTERN, bios_log_prefix[lvl]);
		} else {
			if (!suppressed)
				putchar(c);
			if (c == '\n') {
				if (tty && !suppressed)
					printf(BIOS_LOG_ESCAPE_RESET);
				suppressed = !print_unknown_logs;
			}
		}
	}
	if (tty)
		printf(BIOS_LOG_ESCAPE_RESET);

	free(console_c);
	free((void *)console_p);
}

/* Hexdump provided buffer using start_address as an offset in the output. */
static void hexdump(const uintptr_t start_address, const uint8_t *buf, const int length)
{
	int i;
	int all_zero = 0;

	for (i = 0; i < length; i += 16) {
		int j;

		all_zero++;
		for (j = 0; j < 16; j++) {
			if (buf[i + j] != 0) {
				all_zero = 0;
				break;
			}
		}

		if (all_zero < 2) {
			printf("%08" PRIxPTR ":", start_address + i);
			for (j = 0; j < 16; j++)
				printf(" %02x", buf[i + j]);
			printf("  ");
			for (j = 0; j < 16; j++)
				printf("%c", isprint(buf[i + j]) ? buf[i + j] : '.');
			printf("\n");
		} else if (all_zero == 2) {
			printf("...\n");
		}
	}
}

static void dump_cbmem_hex(void)
{
	struct mapping cbmem_mapping;
	struct lb_memory *memory;
	const uint8_t *buf = NULL;
	size_t size = 0;

	lb_table_get_entry(LB_TAG_MEMORY, (uint8_t **)&memory, &size);

	if (!memory)
		die("No memory entries available.\n");

	const int entries = (memory->size - sizeof(*memory)) / sizeof(memory->map[0]);
	/* First from the end is CBMEM */
	for (int i = entries - 1; i >= 0; --i) {
		if (memory->map[i].type != LB_MEM_TABLE)
			continue;

		buf = map_memory(&cbmem_mapping, memory->map[i].start, memory->map[i].size);
		if (!buf)
			die("Unable to map CBMEM area memory.\n");

		size = memory->map[i].size;
		break;
	}

	if (!buf)
		die("Unable to find CBMEM area memory entry.\n");

	hexdump(cbmem_mapping.phys, buf, size);

	unmap_memory(&cbmem_mapping);
	free(memory);
}

static void dump_cbmem_raw(unsigned int id)
{
	uint8_t *buf;
	size_t size;

	if (get_cbmem_entry(id, &buf, &size)) {
		fprintf(stderr, "cbmem entry id: %#x not found.\n", id);
		abort();
	}

	fwrite(buf, 1, size, stdout);

	free(buf);
}

struct cbmem_id_to_name {
	uint32_t id;
	const char *name;
};
static const struct cbmem_id_to_name cbmem_ids[] = { CBMEM_ID_TO_NAME_TABLE };

#define MAX_STAGEx 10
static void cbmem_print_entry(int n, uint32_t id, uint64_t base, uint64_t size)
{
	const char *name;
	char stage_x[20];

	name = NULL;
	for (size_t i = 0; i < ARRAY_SIZE(cbmem_ids); i++) {
		if (cbmem_ids[i].id == id) {
			name = cbmem_ids[i].name;
			break;
		}
		if (id >= CBMEM_ID_STAGEx_META &&
			id < CBMEM_ID_STAGEx_META + MAX_STAGEx) {
			snprintf(stage_x, sizeof(stage_x), "STAGE%d META",
				(id - CBMEM_ID_STAGEx_META));
			name = stage_x;
		}
		if (id >= CBMEM_ID_STAGEx_CACHE &&
			id < CBMEM_ID_STAGEx_CACHE + MAX_STAGEx) {
			snprintf(stage_x, sizeof(stage_x), "STAGE%d $  ",
				(id - CBMEM_ID_STAGEx_CACHE));
			name = stage_x;
		}
	}

	printf("%2d. ", n);
	if (name == NULL)
		name = "(unknown)";
	printf("%-20s  %08x", name, id);
	printf("  %08" PRIx64 " ", base);
	printf(" %08" PRIx64 "\n", size);
}

static void dump_cbmem_toc(void)
{
	int i = 0;
	const uint8_t *table = NULL;
	size_t table_size = 0;

	if (get_cbmem_entry(CBMEM_ID_CBTABLE, (uint8_t **)&table, &table_size)) {
		fprintf(stderr, "coreboot table not found.\n");
		abort();
	}

	const struct lb_header *lbh = (const struct lb_header *)table;

	printf("CBMEM table of contents:\n");
	printf("    %-20s  %-8s  %-8s  %-8s\n", "NAME", "ID", "START", "LENGTH");

	const struct lb_record *lbr = NULL;
	for (size_t offset = lbh->header_bytes;
	     offset < lbh->table_bytes + lbh->header_bytes - sizeof(struct lb_cbmem_entry);
	     offset += lbr->size) {
		lbr = (const struct lb_record *)&table[offset];

		if (lbr->tag != LB_TAG_CBMEM_ENTRY)
			continue;

		const struct lb_cbmem_entry *lbe = (const struct lb_cbmem_entry *)lbr;
		cbmem_print_entry(i, lbe->id, lbe->address, lbe->entry_size);
		i++;
	}
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
	uint64_t start;
	uint8_t *coverage;
	size_t size;
	unsigned long phys_offset;
#define phys_to_virt(x) ((void *)(unsigned long)(x) + phys_offset)

	if (get_cbmem_entry(CBMEM_ID_COVERAGE, &coverage, &size)) {
		fprintf(stderr, "No coverage information found\n");
		return;
	}

	// Physical address is needed for pointer translation
	if (find_cbmem_entry(CBMEM_ID_COVERAGE, &start, &size)) {
		fprintf(stderr, "No coverage information found\n");
		return;
	}

	/* Map coverage area */
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
	free(coverage);
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
    "GNU General Public License for more details.\n\n");
}

static void print_usage(const char *name, int exit_code)
{
	printf("usage: %s [-cCltTLxVvh?]\n", name);
	printf("\n"
	     "   -c | --console:                   print cbmem console\n"
	     "   -1 | --oneboot:                   print cbmem console for last boot only\n"
	     "   -2 | --2ndtolast:                 print cbmem console for the boot that came before the last one only\n"
	     "   -B | --loglevel:                  maximum loglevel to print; prefix `+` (e.g. -B +INFO) to also print lines that have no level\n"
	     "   -C | --coverage:                  dump coverage information\n"
	     "   -l | --list:                      print cbmem table of contents\n"
	     "   -x | --hexdump:                   print hexdump of cbmem area\n"
	     "   -r | --rawdump ID:                print rawdump of specific ID (in hex) of cbtable\n"
	     "   -t | --timestamps:                print timestamp information\n"
	     "   -T | --parseable-timestamps:      print parseable timestamps\n"
	     "   -S | --stacked-timestamps:        print stacked timestamps (e.g. for flame graph tools)\n"
	     "   -a | --add-timestamp ID:          append timestamp with ID\n"
	     "   -L | --tcpa-log                   print TPM log\n"
	     "   -V | --verbose:                   verbose (debugging) output\n"
	     "   -v | --version:                   print the version\n"
	     "   -h | --help:                      print this help\n"
	     "\n");
	exit(exit_code);
}

#if defined(__arm__) || defined(__aarch64__)
static void dt_update_cells(const char *name, int *addr_cells_ptr,
			    int *size_cells_ptr)
{
	if (*addr_cells_ptr >= 0 && *size_cells_ptr >= 0)
		return;

	int buffer;
	size_t nlen = strlen(name);
	char *prop = alloca(nlen + sizeof("/#address-cells"));
	strcpy(prop, name);

	if (*addr_cells_ptr < 0) {
		strcpy(prop + nlen, "/#address-cells");
		int fd = open(prop, O_RDONLY);
		if (fd < 0 && errno != ENOENT) {
			perror(prop);
		} else if (fd >= 0) {
			if (read(fd, &buffer, sizeof(int)) < 0)
				perror(prop);
			else
				*addr_cells_ptr = ntohl(buffer);
			close(fd);
		}
	}

	if (*size_cells_ptr < 0) {
		strcpy(prop + nlen, "/#size-cells");
		int fd = open(prop, O_RDONLY);
		if (fd < 0 && errno != ENOENT) {
			perror(prop);
		} else if (fd >= 0) {
			if (read(fd, &buffer, sizeof(int)) < 0)
				perror(prop);
			else
				*size_cells_ptr = ntohl(buffer);
			close(fd);
		}
	}
}

static char *dt_find_compat(const char *parent, const char *compat,
			    int *addr_cells_ptr, int *size_cells_ptr)
{
	char *ret = NULL;
	struct dirent *entry;
	DIR *dir;

	if (!(dir = opendir(parent))) {
		perror(parent);
		return NULL;
	}

	/* Loop through all files in the directory (DT node). */
	while ((entry = readdir(dir))) {
		/* We only care about compatible props or subnodes. */
		if (entry->d_name[0] == '.' || !((entry->d_type & DT_DIR) ||
		    !strcmp(entry->d_name, "compatible")))
			continue;

		/* Assemble the file name (on the stack, for speed). */
		size_t plen = strlen(parent);
		char *name = alloca(plen + strlen(entry->d_name) + 2);

		strcpy(name, parent);
		name[plen] = '/';
		strcpy(name + plen + 1, entry->d_name);

		/* If it's a subnode, recurse. */
		if (entry->d_type & DT_DIR) {
			ret = dt_find_compat(name, compat, addr_cells_ptr,
					     size_cells_ptr);

			/* There is only one matching node to find, abort. */
			if (ret) {
				/* Gather cells values on the way up. */
				dt_update_cells(parent, addr_cells_ptr,
						size_cells_ptr);
				break;
			}
			continue;
		}

		/* If it's a compatible string, see if it's the right one. */
		int fd = open(name, O_RDONLY);
		int clen = strlen(compat);
		char *buffer = alloca(clen + 1);

		if (fd < 0) {
			perror(name);
			continue;
		}

		if (read(fd, buffer, clen + 1) < 0) {
			perror(name);
			close(fd);
			continue;
		}
		close(fd);

		if (!strcmp(compat, buffer)) {
			/* Initialize these to "unset" for the way up. */
			*addr_cells_ptr = *size_cells_ptr = -1;

			/* Can't leave string on the stack or we'll lose it! */
			ret = strdup(parent);
			break;
		}
	}

	closedir(dir);
	return ret;
}
#endif /* defined(__arm__) || defined(__aarch64__) */

int main(int argc, char** argv)
{
	int print_defaults = 1;
	int print_console = 0;
	int print_coverage = 0;
	int print_list = 0;
	int print_hexdump = 0;
	int print_rawdump = 0;
	int print_tcpa_log = 0;
	enum timestamps_print_type timestamp_type = TIMESTAMPS_PRINT_NONE;
	enum console_print_type console_type = CONSOLE_PRINT_FULL;
	unsigned int rawdump_id = 0;
	int max_loglevel = BIOS_NEVER;
	int print_unknown_logs = 1;
	uint32_t timestamp_id = 0;

	int opt, option_index = 0;
	static struct option long_options[] = {
		{"console", 0, 0, 'c'},
		{"oneboot", 0, 0, '1'},
		{"2ndtolast", 0, 0, '2'},
		{"loglevel", required_argument, 0, 'B'},
		{"coverage", 0, 0, 'C'},
		{"list", 0, 0, 'l'},
		{"tcpa-log", 0, 0, 'L'},
		{"timestamps", 0, 0, 't'},
		{"parseable-timestamps", 0, 0, 'T'},
		{"stacked-timestamps", 0, 0, 'S'},
		{"add-timestamp", required_argument, 0, 'a'},
		{"hexdump", 0, 0, 'x'},
		{"rawdump", required_argument, 0, 'r'},
		{"verbose", 0, 0, 'V'},
		{"version", 0, 0, 'v'},
		{"help", 0, 0, 'h'},
		{0, 0, 0, 0}
	};
	while ((opt = getopt_long(argc, argv, "c12B:CltTSa:LxVvh?r:",
				  long_options, &option_index)) != EOF) {
		switch (opt) {
		case 'c':
			print_console = 1;
			print_defaults = 0;
			break;
		case '1':
			print_console = 1;
			console_type = CONSOLE_PRINT_LAST;
			print_defaults = 0;
			break;
		case '2':
			print_console = 1;
			console_type = CONSOLE_PRINT_PREVIOUS;
			print_defaults = 0;
			break;
		case 'B':
			max_loglevel = parse_loglevel(optarg, &print_unknown_logs);
			break;
		case 'C':
			print_coverage = 1;
			print_defaults = 0;
			break;
		case 'l':
			print_list = 1;
			print_defaults = 0;
			break;
		case 'L':
			print_tcpa_log = 1;
			print_defaults = 0;
			break;
		case 'x':
			print_hexdump = 1;
			print_defaults = 0;
			break;
		case 'r':
			print_rawdump = 1;
			print_defaults = 0;
			rawdump_id = strtoul(optarg, NULL, 16);
			break;
		case 't':
			timestamp_type = TIMESTAMPS_PRINT_NORMAL;
			print_defaults = 0;
			break;
		case 'T':
			timestamp_type = TIMESTAMPS_PRINT_MACHINE_READABLE;
			print_defaults = 0;
			break;
		case 'S':
			timestamp_type = TIMESTAMPS_PRINT_STACKED;
			print_defaults = 0;
			break;
		case 'a':
			print_defaults = 0;
			timestamp_id = timestamp_enum_name_to_id(optarg);
			/* Parse numeric value if name is unknown */
			if (timestamp_id == 0)
				timestamp_id = strtoul(optarg, NULL, 0);
			break;
		case 'V':
			verbose = 1;
			break;
		case 'v':
			print_version();
			exit(0);
			break;
		case 'h':
			print_usage(argv[0], 0);
			break;
		case '?':
		default:
			print_usage(argv[0], 1);
			break;
		}
	}

	if (optind < argc) {
		fprintf(stderr, "Error: Extra parameter found.\n");
		print_usage(argv[0], 1);
	}

	mem_fd = open("/dev/mem", timestamp_id ? O_RDWR : O_RDONLY, 0);
	if (mem_fd < 0) {
		fprintf(stderr, "Failed to gain memory access: %s\n",
			strerror(errno));
		return 1;
	}

#if defined(__arm__) || defined(__aarch64__)
	int addr_cells, size_cells;
	char *coreboot_node = dt_find_compat("/proc/device-tree", "coreboot",
					     &addr_cells, &size_cells);

	if (!coreboot_node) {
		fprintf(stderr, "Could not find 'coreboot' compatible node!\n");
		return 1;
	}

	if (addr_cells < 0) {
		fprintf(stderr, "Warning: no #address-cells node in tree!\n");
		addr_cells = 1;
	}

	int nlen = strlen(coreboot_node);
	char *reg = alloca(nlen + sizeof("/reg"));

	strcpy(reg, coreboot_node);
	strcpy(reg + nlen, "/reg");
	free(coreboot_node);

	int fd = open(reg, O_RDONLY);
	if (fd < 0) {
		perror(reg);
		return 1;
	}

	int i;
	size_t size_to_read = addr_cells * 4 + size_cells * 4;
	uint8_t *dtbuffer = alloca(size_to_read);
	if (read(fd, dtbuffer, size_to_read) < 0) {
		perror(reg);
		return 1;
	}
	close(fd);

	/* No variable-length byte swap function anywhere in C... how sad. */
	uint64_t baseaddr = 0;
	for (i = 0; i < addr_cells * 4; i++) {
		baseaddr <<= 8;
		baseaddr |= *dtbuffer;
		dtbuffer++;
	}
	uint64_t cb_table_size = 0;
	for (i = 0; i < size_cells * 4; i++) {
		cb_table_size <<= 8;
		cb_table_size |= *dtbuffer;
		dtbuffer++;
	}

	parse_cbtable(baseaddr, cb_table_size);
#else
	unsigned long long possible_base_addresses[] = { 0, 0xf0000 };

	/* Find and parse coreboot table */
	for (size_t j = 0; j < ARRAY_SIZE(possible_base_addresses); j++) {
		if (!parse_cbtable(possible_base_addresses[j], 0))
			break;
	}
#endif

	if (mapping_virt(&lbtable_mapping) == NULL)
		die("Table not found.\n");

	if (print_console)
		dump_console(console_type, max_loglevel, print_unknown_logs);

	if (print_coverage)
		dump_coverage();

	if (print_list)
		dump_cbmem_toc();

	if (print_hexdump)
		dump_cbmem_hex();

	if (print_rawdump)
		dump_cbmem_raw(rawdump_id);

	if (timestamp_id)
		timestamp_add_now(timestamp_id);

	if (print_defaults)
		timestamp_type = TIMESTAMPS_PRINT_NORMAL;

	if (timestamp_type != TIMESTAMPS_PRINT_NONE)
		dump_timestamps(timestamp_type);

	if (print_tcpa_log)
		dump_tpm_log();

	unmap_memory(&lbtable_mapping);

	close(mem_fd);
	return 0;
}
