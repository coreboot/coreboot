#include <printk.h>
#include <subr.h>
#include <boot/elf.h>
#include <rom/read_bytes.h>
#include <boot/linuxbios_tables.h>
#include <ip_checksum.h>
#include <stdlib.h>

static unsigned long count_lb_records(void *start, unsigned long length)
{
	struct lb_record *rec;
	void *end;
	unsigned long count;
	count = 0;
	end = ((char *)start) + length;
	for(rec = start; ((void *)rec < end) &&
		((signed long)rec->size <= (end - (void *)rec)); 
		rec = (void *)(((char *)rec) + rec->size)) {
		count++;
	}
	return count;
}

static int find_lb_table(struct lb_header **result, void *start, void *end)
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
			(compute_ip_checksum(head, sizeof(*head)) == 0) &&
			(compute_ip_checksum(ptr + sizeof(*head), head->table_bytes) ==
				head->table_checksum) &&
			(count_lb_records(ptr + sizeof(*head), head->table_bytes) ==
				head->table_entries)
			) {
			*result = (struct lb_header *)ptr;
			return 1;
		}
	}
	return 0;
}

static struct lb_memory *lookup_meminfo(void)
{
	struct lb_header *head;
	struct lb_record *rec;
	void *start, *end;
	
	if (!find_lb_table(&head, (void *)0x00000, (void *)0x1000)) {
		if (!find_lb_table(&head, (void *)0xf0000, (void*)0x100000)) {
			printk_err("Cannot find linuxbios table...\n");
			while(1);
		}
	}
	start = ((unsigned char *)head) + sizeof(*head);
	end = ((char *)start) + head->table_bytes;
	for(rec = start; ((void *)rec < end) &&
		((long)rec->size <= (end - (void *)rec));
		rec = (void *)(((char *)rec) + rec->size)) {
		switch(rec->tag) {
		case LB_TAG_MEMORY:
			return (struct lb_memory *)rec;
		}
	}
	printk_err("Cannot find memory range table\n");
	while(1);
	
}
void standalonemain(void)
{
	int i;
	int max;
	malloc_mark_t place;
	struct lb_memory *mem;

	/* displayinit MUST PRECEDE ALL PRINTK! */
	displayinit();
	
	printk_info("LBBL\n");
	max = estreams - streams;
	mem = lookup_meminfo();
	printk_info("%d boot devices present\n", max);
	for(i = 0; i < max; i++) {
		int result = 1;
		malloc_mark(&place);
		printk_info("Trying to boot from %d\n", i);
		result = elfboot(&streams[i], mem);
		malloc_release(&place);
		if (result) {
			/* displayinit MUST PRECEDE ALL PRINTK! */
			displayinit();
		}
	}
	printk_err("All boot devices failed\n");
}
