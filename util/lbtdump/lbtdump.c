#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>
#include <errno.h>
#include <sys/mman.h>
#include "../../src/include/boot/coreboot_tables.h"

void print_lb_records(struct lb_record *rec, struct lb_record *last, unsigned long addr);

unsigned long compute_checksum(void *addr, unsigned long length)
{
	uint8_t *ptr;
	volatile union {
		uint8_t  byte[2];
		uint16_t word;
	} value;
	unsigned long sum;
	unsigned long i;
	/* In the most straight forward way possible,
	 * compute an ip style checksum.
	 */
	sum = 0;
	ptr = addr;
	for(i = 0; i < length; i++) {
		unsigned long value;
		value = ptr[i];
		if (i & 1) {
			value <<= 8;
		}
		/* Add the new value */
		sum += value;
		/* Wrap around the carry */
		if (sum > 0xFFFF) {
			sum = (sum + (sum >> 16)) & 0xFFFF;
		}
	}
	value.byte[0] = sum & 0xff;
	value.byte[1] = (sum >> 8) & 0xff;
	return (~value.word) & 0xFFFF;
}

#define for_each_lbrec(head, rec) \
	for(rec = (struct lb_record *)(((char *)head) + sizeof(*head)); \
		(((char *)rec) < (((char *)head) + sizeof(*head) + head->table_bytes))  && \
		(rec->size >= 1) && \
		((((char *)rec) + rec->size) <= (((char *)head) + sizeof(*head) + head->table_bytes)); \
		rec = (struct lb_record *)(((char *)rec) + rec->size))


static int  count_lb_records(struct lb_header *head)
{
	struct lb_record *rec;
	int count;
	count = 0;
	for_each_lbrec(head, rec) {
		count++;
	}
	return count;
}


struct lb_header *find_lb_table(void *base, unsigned long start, unsigned long end)
{
	unsigned long addr;
	/* For now be stupid.... */
	for(addr = start; addr < end; addr += 16) {
		struct lb_header *head = (struct lb_header *)(((char*)base) + addr);
		struct lb_record *recs = (struct lb_record *)(((char*)base) + addr + sizeof(*head));
		if (memcmp(head->signature, "LBIO", 4) != 0)
			continue;
		fprintf(stdout, "Found candidate at: %08lx-%08lx\n",
			addr, addr + head->table_bytes);
		if (head->header_bytes != sizeof(*head)) {
			fprintf(stderr, "Header bytes of %d are incorrect\n",
				head->header_bytes);
			continue;
		}
		if (count_lb_records(head) != head->table_entries) {
			fprintf(stderr, "bad record count: %d\n",
				head->table_entries);
			continue;
		}
		if (compute_checksum((unsigned char *)head, sizeof(*head)) != 0) {
			fprintf(stderr, "bad header checksum\n");
			continue;
		}
		if (compute_checksum(recs, head->table_bytes)
			!= head->table_checksum) {
			fprintf(stderr, "bad table checksum: %04x\n",
				head->table_checksum);
			continue;
		}
		fprintf(stdout, "Found coreboot table at: %08lx\n", addr);
		return head;

	};
	return 0;
}

void nop_print(struct lb_record *rec, unsigned long addr)
{
	return;
}

void pretty_print_number(FILE *stream, uint64_t num)
{
	unsigned long long value = (unsigned long long) num;

	if (value >      1024ULL*1024*1024*1024*1024*1024) {
		value /= 1024ULL*1024*1024*1024*1024*1024;
		fprintf(stream, "%lldEB", value);
	}
	else if (value > 1024ULL*1024*1024*1024*1024) {
		value /= 1024ULL*1024*1024*1024*1024;
		fprintf(stream, "%lldPB", value);
	}
	else if (value > 1024ULL*1024*1024*1024) {
		value /= 1024ULL*1024*1024*1024;
		fprintf(stream, "%lldTB", value);
	}
	else if (value > 1024ULL*1024*1024) {
		value /= 1024ULL*1024*1024;
		fprintf(stream, "%lldGB", value);
	}
	else if (value > 1024ULL*1024) {
		value /= 1024ULL*1024;
		fprintf(stream, "%lldMB", value);
	}
	else if (value > 1024ULL) {
		value /= 1024ULL;
		fprintf(stream, "%lldKB", value);
	}
	else {
		fprintf(stream, "%lldB", value);
	}
}

void print_memory(struct lb_record *ptr, unsigned long addr)
{
	struct lb_memory *rec = (void *)ptr;
	int entries;
	int i;
	entries = (rec->size -  sizeof(*rec))/sizeof(rec->map[0]);
	for(i = 0; i < entries; i++) {
		char *mem_type;
		uint64_t start;
		uint64_t end;
		start = unpack_lb64(rec->map[i].start);
		end = start + unpack_lb64(rec->map[i].size);
		switch(rec->map[i].type) {
		case 1: mem_type = "ram"; break;
		case 3: mem_type = "acpi"; break;
		case 4: mem_type = "nvs"; break;
		default:
		case 2: mem_type = "reserved"; break;
		}
		printf("0x%08llx - 0x%08llx %s (",
			(unsigned long long)start, (unsigned long long)end, mem_type);
		pretty_print_number(stdout, start);
		printf(" - ");
		pretty_print_number(stdout, end);
		printf(")\n");
	}
}

void print_mainboard(struct lb_record *ptr, unsigned long addr)
{
	struct lb_mainboard *rec;
	int max_size;
	rec = (struct lb_mainboard *)ptr;
	max_size = rec->size - sizeof(*rec);
	printf("vendor: %.*s part number: %.*s\n",
		max_size - rec->vendor_idx,      rec->strings + rec->vendor_idx,
		max_size - rec->part_number_idx, rec->strings + rec->part_number_idx);
}

void print_string(struct lb_record *ptr, unsigned long addr)
{
	struct lb_string *rec;
	int max_size;
	rec = (struct lb_string *)ptr;
	max_size = rec->size - sizeof(*rec);
	printf("%.*s\n", max_size, rec->string);
}

void print_option_table(struct lb_record *ptr, unsigned long addr)
{
	struct lb_record *rec, *last;
	struct cmos_option_table *hdr;
	hdr = (struct cmos_option_table *)ptr;
	rec  = (struct lb_record *)(((char *)hdr) + hdr->header_length);
	last = (struct lb_record *)(((char *)hdr) + hdr->size);
	printf("cmos option header record = type %d, size %d, header length %d\n",
		hdr->tag, hdr->size, hdr->header_length);
	print_lb_records(rec, last, addr + hdr->header_length);
#if 0
	{
		unsigned char *data = (unsigned char *)ptr;
		int i;
		for(i = 0; i < hdr->size; i++) {
			if ((i %10) == 0 ) {
				fprintf(stderr, "\n\t");
			}
			fprintf(stderr, "0x%02x,", data[i]);
		}
	}
#endif

}

void print_option(struct lb_record *ptr, unsigned long addr)
{
	struct cmos_entries *rec;
	rec= (struct cmos_entries *)ptr;
	printf("entry %d, rec len %d, start %d, length %d, conf %d, id %d, %s\n",
		rec->tag, rec->size, rec->bit, rec->length,
		rec->config, rec->config_id, rec->name);
}

void print_option_enumeration(struct lb_record *ptr, unsigned long addr)
{
	struct cmos_enums *rec;
	rec = (struct cmos_enums *)ptr;
	printf("enumeration %d, rec len %d, id %d, value %d, %s\n",
		rec->tag, rec->size, rec->config_id, rec->value,
		rec->text);
}

void print_option_checksum(struct lb_record *ptr, unsigned long addr)
{
	struct cmos_checksum *rec;
	rec = (struct cmos_checksum *)ptr;
	printf("checksum %d, rec len %d, range %d-%d location %d type %d\n",
		rec->tag, rec->size,
		rec->range_start, rec->range_end, rec->location, rec->type);
}

struct {
	uint32_t type;
	char *type_name;
	void (*print)(struct lb_record *rec, unsigned long addr);
} lb_types[] = {
	{ LB_TAG_UNUSED,            "Unused",             nop_print },
	{ LB_TAG_MEMORY,            "Memory",             print_memory },
	{ LB_TAG_HWRPB,             "HWRPB",              nop_print },
	{ LB_TAG_MAINBOARD,         "Mainboard",          print_mainboard },
	{ LB_TAG_VERSION,           "Version",            print_string },
	{ LB_TAG_EXTRA_VERSION,     "Extra Version",      print_string },
	{ LB_TAG_BUILD,             "Build",              print_string },
	{ LB_TAG_COMPILE_TIME,      "Compile Time",       print_string },
	{ LB_TAG_COMPILE_BY,        "Compile By",         print_string },
	{ LB_TAG_COMPILE_HOST,      "Compile Host",       print_string },
	{ LB_TAG_COMPILE_DOMAIN,    "Compile Domain",     print_string },
	{ LB_TAG_COMPILER,          "Compiler",           print_string },
	{ LB_TAG_LINKER,            "Linker",             print_string },
	{ LB_TAG_ASSEMBLER,         "Assembler",          print_string },
	{ LB_TAG_CMOS_OPTION_TABLE, "CMOS option table",  print_option_table },
	{ LB_TAG_OPTION,            "Option",             print_option },
	{ LB_TAG_OPTION_ENUM,       "Option Enumeration", print_option_enumeration },
	{ LB_TAG_OPTION_DEFAULTS,   "Option Defaults",    nop_print },
	{ LB_TAG_OPTION_CHECKSUM,   "Option Checksum",    print_option_checksum },
	{ -1, "Unknown", 0 }
};

static struct lb_record *next_record(struct lb_record *rec)
{
	return (struct lb_record *)(((char *)rec) + rec->size);
}

void print_lb_records(struct lb_record *rec, struct lb_record *last,
	unsigned long addr)
{
	struct lb_record *next;
	int i;
	int count;
	count = 0;

	for(next = next_record(rec); (rec < last) && (next <= last);
		rec = next, addr += rec->size) {
		next = next_record(rec);
		count++;
		for(i = 0; lb_types[i].print != 0; i++) {
			if (lb_types[i].type == rec->tag) {
				break;
			}
		}
		printf("lb_record #%d type %d @ 0x%08lx %s\n",
			count, rec->tag, addr, lb_types[i].type_name);
		if (lb_types[i].print) {
			lb_types[i].print(rec, addr);
		}
	}
}

void print_lb_table(struct lb_header *head, unsigned long addr)
{
	struct lb_record *rec, *last;

	rec  = (struct lb_record *)(((char *)head) + head->header_bytes);
	last = (struct lb_record *)(((char *)rec) + head->table_bytes);

	printf("Coreboot header(%d) checksum: %04x table(%d) checksum: %04x entries: %d\n",
		head->header_bytes, head->header_checksum,
		head->table_bytes, head->table_checksum, head->table_entries);
	print_lb_records(rec, last, addr + head->header_bytes);
}

int main(int argc, char **argv)
{
	unsigned char *low_1MB;
	struct lb_header *lb_table;
	int fd;
	fd = open("/dev/mem", O_RDONLY);
	if (fd < 0) {
		fprintf(stderr, "Can not open /dev/mem\n");
		exit(-1);
	}
	low_1MB = mmap(0, 1024*1024, PROT_READ, MAP_SHARED, fd, 0x00000000);
	if (low_1MB == ((void *) -1)) {
		fprintf(stderr, "Can not mmap /dev/mem at %08lx errno(%d):%s\n",
			0x00000000UL, errno, strerror(errno));
		exit(-2);
	}
	lb_table = 0;
	if (!lb_table)
		lb_table = find_lb_table(low_1MB, 0x00000, 0x1000);
	if (!lb_table)
		lb_table = find_lb_table(low_1MB, 0xf0000, 1024*1024);
	if (lb_table) {
		unsigned long addr;
		addr = ((char *)lb_table) - ((char *)low_1MB);
		print_lb_table(lb_table, addr);
	}
	else {
		printf("lb_table not found\n");
	}
	return 0;
}
