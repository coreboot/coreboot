#include <stdio.h>
#include <stdint.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>
#include <errno.h>
#include <sys/mman.h>
#include "../../src/include/boot/linuxbios_tables.h"

unsigned long compute_checksum(void *addr, unsigned long length)
{
	unsigned short *ptr;
	unsigned long sum;
	unsigned long len;
	/* Assumes len is a multiple of two, and addr is 2 byte aligned. */
	/* compute an ip style checksum */
	sum = 0;
	len = length >> 1;
	ptr = addr;
	while (len--) {
		sum += *(ptr++);
		if (sum > 0xFFFF)
			sum -= 0xFFFF;
	}
	return (~sum) & 0xFFFF;
	
}

int count_lb_records(void *start, unsigned long length)
{
	struct lb_record *rec;
	void *end;
	int count;
	count = 0;
	end = ((char *)start) + length;
	for(rec = start; ((void *)rec < end) &&
		    (rec->size <= (end - (void *)rec)); 
	    rec = (void *)(((char *)rec) + rec->size)) {
		count++;
	}
	return count;
}

void *find_lb_table(void *start, void *end)
{
	unsigned char *ptr;
	/* For now be stupid.... */
	for(ptr = start; (void *)ptr < end; ptr += 16) {
		struct lb_header *head = (void *)ptr;
		if ((head->signature[0] == 'L') && 
			(head->signature[1] == 'B') &&
			(head->signature[2] == 'I') &&
			(head->signature[3] == 'O') &&
			(head->header_bytes == sizeof(*head)) &&
			(compute_checksum(head, sizeof(*head)) == 0) &&
			(compute_checksum(ptr + sizeof(*head), head->table_bytes) ==
				head->table_checksum) &&
			(count_lb_records(ptr + sizeof(*head), head->table_bytes) ==
				head->table_entries)
			) {
			return ptr;
		}
	};
	return 0;
}

void nop_print(struct lb_record *rec)
{
	return;
}

void pretty_print_number(FILE *stream, uint64_t value)
{
	if (value > 1024ULL*1024*1024*1024*1024) {
		value /= 1024ULL*1024*1024*1024*1024;
		fprintf(stream, "%lldEB", value);
	}
	else if (value > 1024ULL*1024*1024*1024) {
		value /= 1024ULL*1024*1024*1024;
		fprintf(stream, "%lldPB", value);
	}
	else if (value > 1024ULL*1024*1024) {
		value /= 1024ULL*1024*1024;
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

void print_memory(struct lb_record *ptr)
{
	struct lb_memory *rec = (void *)ptr;
	int entries;
	int i;
	entries = (rec->size -  sizeof(*rec))/sizeof(rec->map[0]);
	for(i = 0; i < entries; i++) {
		char *mem_type;
		uint64_t start;
		uint64_t end;
		start = rec->map[i].start;
		end = start + rec->map[i].size;
		switch(rec->map[i].type) {
		case 1: mem_type = "ram"; break;
		case 3: mem_type = "acpi"; break;
		case 4: mem_type = "nvs"; break;
		default:
		case 2: mem_type = "reserved"; break;
		}
		printf("0x%08llx - 0x%08llx %s (",
			start, end, mem_type);
		pretty_print_number(stdout, start);
		printf(" - ");
		pretty_print_number(stdout, end);		
		printf(")\n");
	}
}
struct {
	uint32_t type;
	char *type_name;
	void (*print)(struct lb_record *rec);
} lb_types[] = {
	{ 0, "Unused", nop_print },
	{ 1, "Memory", print_memory },
	{ 2, "HWRPB", nop_print },
	{ -1, "Unknown", 0 }
};

void print_lb_records(struct lb_header *head)
{
	struct lb_record *rec;
	void *start, *end;
	int i;
	int count;
	count = 0;
	start = ((char *)head) + head->header_bytes;
	end = ((char *)start) + head->table_bytes;

	printf("LinuxBIOS header\n");
	for(rec = start; 
	    ((void *)rec < end) && (rec->size <= (end - (void *)rec)); 
		rec = (void *)(((char *)rec) + rec->size)) {
		count++;
		for(i = 0; lb_types[i].print != 0; i++) {
			if (lb_types[i].type == rec->tag) {
				break;
			}
		}
		printf("lb_record #%d type %d %s\n",
			count, rec->tag, lb_types[i].type_name);
		if (lb_types[i].print) {
			lb_types[i].print(rec);
		}
	}
}

int main(int argc, char **argv) 
{
	unsigned char *low_1MB;
	void *lb_table;
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
	lb_table = find_lb_table(low_1MB + 0xf0000, low_1MB + 1024*1024);
	if (lb_table) {
		printf("lb_table = 0x%08x\n", 
			lb_table - (void *)low_1MB);
		print_lb_records(lb_table);
	}
	else {
		printf("lb_table not found\n");
	}
	return 0;
}
