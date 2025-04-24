/* SPDX-License-Identifier: GPL-2.0-only */

#include <inttypes.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <getopt.h>
#include <errno.h>
#include <fcntl.h>
#include <ctype.h>
#include <arpa/inet.h>
#include <sys/stat.h>
#include <libgen.h>
#include <assert.h>
#include <regex.h>
#include <commonlib/bsd/cbmem_id.h>
#include <commonlib/bsd/helpers.h>
#include <commonlib/bsd/tpm_log_defs.h>
#include <commonlib/loglevel.h>
#include <commonlib/timestamp_serialized.h>
#include <commonlib/tpm_log_serialized.h>
#include <commonlib/coreboot_tables.h>

#if defined(__i386__) || defined(__x86_64__)
#include <x86intrin.h>
#endif

#include "cbmem_util.h"

#define CBMEM_VERSION "1.1"

/* Global verbosity level for debug() macro. */
int cbmem_util_verbose;

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

#if defined(__i386__) || defined(__x86_64__)
static void cbmem_get_lb_table_entry(uint32_t tag, uint8_t **buf_out, size_t *size_out)
{
	const struct lb_record *lbr_p;
	const uint8_t *lbtable_raw;
	bool tag_found = false;

	if (!cbmem_devmem_get_cbmem_entry(CBMEM_ID_CBTABLE, (uint8_t **)&lbtable_raw, NULL,
					  NULL))
		die("coreboot table not found.\n");

	const struct lb_header *lbh = (const struct lb_header *)lbtable_raw;

	for (size_t i = 0; i < lbh->table_bytes - sizeof(struct lb_record); i += lbr_p->size) {
		lbr_p = (const struct lb_record *)(&lbtable_raw[lbh->header_bytes + i]);
		if (lbr_p->tag == tag) {
			tag_found = true;
			break;
		}
	}

	if (!tag_found) {
		free((void *)lbtable_raw);
		die("coreboot table entry %#x not found.\n", tag);
	}

	debug("coreboot table entry %#x found.\n", tag);

	*buf_out = malloc(lbr_p->size);
	if (!*buf_out) {
		free((void *)lbtable_raw);
		die("Unable to allocate memory for coreboot table entry %#x, size: %d\n", tag,
		    lbr_p->size);
	}
	memcpy(*buf_out, lbr_p, lbr_p->size);
	*size_out = lbr_p->size;
	free((void *)lbtable_raw);
}
#endif /* defined(__i386__) || defined(__x86_64__) */

static uint64_t timestamp_get(uint64_t table_tick_freq_mhz)
{
#if defined(__i386__) || defined(__x86_64__)
	uint64_t tsc = __rdtsc();
	struct lb_tsc_info *tsc_info;
	size_t size;

	/* No tick frequency specified means raw TSC values. */
	if (!table_tick_freq_mhz)
		return tsc;

	cbmem_get_lb_table_entry(LB_TAG_TSC_INFO, (uint8_t **)&tsc_info, &size);

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

	if (!cbmem_devmem_get_cbmem_entry(CBMEM_ID_TIMESTAMP, (uint8_t **)&tst_p, &size, NULL))
		die("Timestamps not found.\n");

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
	size_t tst_size;

	if (!cbmem_devmem_get_cbmem_entry(CBMEM_ID_TIMESTAMP, (uint8_t **)&tst_p, &tst_size, NULL))
		die("Unable to find timestamps.\n");

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

	if (!cbmem_devmem_write_cbmem_entry(CBMEM_ID_TIMESTAMP, (uint8_t *)tst_p, tst_size))
		die("Unable to write timestamps.\n");
	free((uint8_t *)tst_p);
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

	if (!cbmem_devmem_get_cbmem_entry(CBMEM_ID_TPM_CB_LOG, (uint8_t **)&tclt_p, NULL, NULL))
		die("coreboot TPM log not found.\n");

	printf("coreboot TPM log:\n\n");

	for (uint16_t i = 0; i < tclt_p->num_entries; i++) {
		const struct tpm_cb_log_entry *tce = &tclt_p->entries[i];

		printf(" PCR-%u ", tce->pcr);
		print_hex_string(tce->digest, tce->digest_length);
		printf(" %s [%s]\n", tce->digest_type, tce->name);
	}

	free((uint8_t *)tclt_p);
}

static void dump_tpm_log(void)
{
	uint8_t *buf;

	if (cbmem_devmem_get_cbmem_entry(CBMEM_ID_TCPA_TCG_LOG, &buf, NULL, NULL) ||
	    cbmem_devmem_get_cbmem_entry(CBMEM_ID_TPM2_TCG_LOG, &buf, NULL, NULL)) {
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

	if (!cbmem_devmem_get_cbmem_entry(CBMEM_ID_CONSOLE, (uint8_t **)&console_p, NULL, NULL))
		die("CBMEM console not found.\n");

	cursor = console_p->cursor & CBMC_CURSOR_MASK;
	if (!(console_p->cursor & CBMC_OVERFLOW) && cursor < console_p->size)
		size = cursor;
	else
		size = console_p->size;

	console_c = malloc(size + 1);
	if (!console_c) {
		free((uint8_t *)console_p);
		die("Not enough memory for console.\n");
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
	free((uint8_t *)console_p);
}

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

static bool hexdump_handler(const uint32_t id, const uint64_t physical_address, const uint8_t *buf,
		 const size_t size, void *data)
{
	(void)id;
	(void)data;
	hexdump(physical_address, buf, size);
	return false;
}

static void dump_cbmem_hex(void)
{
	cbmem_devmem_foreach_cbmem_entry(hexdump_handler, NULL, true);
}

static void dump_cbmem_raw(unsigned int id)
{
	uint8_t *buf;
	size_t size;

	if (!cbmem_devmem_get_cbmem_entry(id, &buf, &size, NULL))
		die("cbmem entry id: %#x not found.\n", id);

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

static bool toc_handler(const uint32_t id, const uint64_t physical_address, const uint8_t *buf,
		 const size_t size, void *data)
{
	(void)buf;
	int *i = data;

	cbmem_print_entry(*i, id, physical_address, size);

	(*i)++;

	return false;
}

static void dump_cbmem_toc(void)
{
	int i = 0;

	printf("CBMEM table of contents:\n");
	printf("    %-20s  %-8s  %-8s  %-8s\n", "NAME", "ID", "START", "LENGTH");

	cbmem_devmem_foreach_cbmem_entry(toc_handler, &i, false);
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
	unsigned long phys_offset;
#define phys_to_virt(x) ((void *)(unsigned long)(x) + phys_offset)

	if (!cbmem_devmem_get_cbmem_entry(CBMEM_ID_COVERAGE, &coverage, NULL, &start))
		die("No coverage information found\n");

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
			cbmem_util_verbose++;
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

	if (!cbmem_devmem_init(timestamp_id != 0))
		die("Unable to initialize /dev/mem access to coreboot tables and CBMEM.\n");

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

	cbmem_devmem_terminate();

	return 0;
}
