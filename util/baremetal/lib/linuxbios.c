
//#include <linuxbios_tables.h>
#include <printk.h>
#include <boot/linuxbios_table.h>
#include <stdlib.h>

static int lb_failsafe = 1;

#undef DEBUG_LINUXBIOS


#define for_each_lbrec(head, rec) \
	for(rec = (struct lb_record *)(((char *)head) + sizeof(*head)); \
		(((char *)rec) < (((char *)head) + sizeof(*head) + head->table_bytes))  && \
		(rec->size >= 1) && \
		((((char *)rec) + rec->size) <= (((char *)head) + sizeof(*head) + head->table_bytes)); \
		rec = (struct lb_record *)(((char *)rec) + rec->size)) 
		

#define for_each_crec(tbl, rec) \
	for(rec = (struct lb_record *)(((char *)tbl) + tbl->header_length); \
		(((char *)rec) < (((char *)tbl) + tbl->size))  && \
		(rec->size >= 1) && \
		((((char *)rec) + rec->size) <= (((char *)tbl) + tbl->size)); \
		rec = (struct lb_record *)(((char *)rec) + rec->size)) 
		


#if 0
static void read_lb_memory(
	struct meminfo *info, struct lb_memory *mem)
{
	int i;
	int entries;
	entries = (mem->size - sizeof(*mem))/sizeof(mem->map[0]);
	for(i = 0; (i < entries); i++) {
		if (info->map_count < E820MAX) {
			info->map[info->map_count].addr = mem->map[i].start;
			info->map[info->map_count].size = mem->map[i].size;
			info->map[info->map_count].type = mem->map[i].type;
			info->map_count++;
		}
		switch(mem->map[i].type) {
		case LB_MEM_RAM:
		{
			unsigned long long end;
			unsigned long mem_k;
			end = mem->map[i].start + mem->map[i].size;
#if defined(DEBUG_LINUXBIOS)
			printf("lb: %X%X - %X%X (ram)\n",
				(unsigned long)(mem->map[i].start >>32), 
				(unsigned long)(mem->map[i].start & 0xFFFFFFFF), 
				(unsigned long)(end >> 32), 
				(unsigned long)(end & 0xFFFFFFFF));
#endif /* DEBUG_LINUXBIOS */
			end >>= 10;
			mem_k = end;
			if (end & 0xFFFFFFFF00000000ULL) {
				mem_k = 0xFFFFFFFF;
			}
			set_base_mem_k(info, mem_k);
			set_high_mem_k(info, mem_k);
			break;
		}
		case LB_MEM_RESERVED:
		default:
#if defined(DEBUG_LINUXBIOS)
		{
			unsigned long long end;
			end = mem->map[i].start + mem->map[i].size;
			printf("lb: %X%X - %X%X (reserved)\n",
				(unsigned long)(mem->map[i].start >>32), 
				(unsigned long)(mem->map[i].start & 0xFFFFFFFF), 
				(unsigned long)(end >> 32), 
				(unsigned long)(end & 0xFFFFFFFF));
		}
#endif /* DEBUG_LINUXBIOS */
			break;
		}
	}
}

static unsigned cmos_read(unsigned offset, unsigned int size)
{
	unsigned addr, old_addr;
	unsigned value;
	
	addr = offset/8;

	old_addr = inb(0x70);
	outb(addr | (old_addr &0x80), 0x70);
	value = inb(0x71);
	outb(old_addr, 0x70);

	value >>= offset & 0x7;
	value &= ((1 << size) - 1);
	
	return value;
}

static unsigned cmos_read_checksum(void)
{
	unsigned sum = 
		(cmos_read(lb_checksum.location, 8) << 8) |
		cmos_read(lb_checksum.location +8, 8);
	return sum & 0xffff;
}

static int cmos_valid(void)
{
	unsigned i;
	unsigned sum, old_sum;
	sum = 0;
	if ((lb_checksum.tag != LB_TAG_OPTION_CHECKSUM) || 
		(lb_checksum.type != CHECKSUM_PCBIOS) ||
		(lb_checksum.size != sizeof(lb_checksum))) {
		return 0;
	}
	for(i = lb_checksum.range_start; i <= lb_checksum.range_end; i+= 8) {
		sum += cmos_read(i, 8);
	}
	sum = (~sum)&0x0ffff;
	old_sum = cmos_read_checksum();
	return sum == old_sum;
}

static void cmos_write(unsigned offset, unsigned int size, unsigned setting)
{
	unsigned addr, old_addr;
	unsigned value, mask, shift;
	unsigned sum;
	
	addr = offset/8;
	
	shift = offset & 0x7;
	mask = ((1 << size) - 1) << shift;
	setting = (setting << shift) & mask;

	old_addr = inb(0x70);
	sum = cmos_read_checksum();
	sum = (~sum) & 0xffff;

	outb(addr | (old_addr &0x80), 0x70);
	value = inb(0x71);
	sum -= value;
	value &= ~mask;
	value |= setting;
	sum += value;
	outb(value, 0x71);

	sum = (~sum) & 0x0ffff;
	outb((lb_checksum.location/8) | (old_addr & 0x80), 0x70);
	outb((sum >> 8) & 0xff, 0x71);
	outb(((lb_checksum.location +8)/8) | (old_addr & 0x80), 0x70);
	outb(sum & 0xff, 0x71);

	outb(old_addr, 0x70);

	return;
}

#endif 

struct lb_memory *mem = NULL;

static void read_linuxbios_values( struct lb_header *head)
{
	/* Read linuxbios tables... */
	struct lb_record *rec;
	for_each_lbrec(head, rec) {
		switch(rec->tag) {
		case LB_TAG_MEMORY:
		{
			mem = (struct lb_memory *) rec;
//			read_lb_memory(info, mem); 
			break;
		}
		case LB_TAG_CMOS_OPTION_TABLE:
		{
			break;
		}
		default: 
			break;
		};
	}
}



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

static int find_lb_table(void *start, void *end, struct lb_header **result)
{
	unsigned char *ptr;
	/* For now be stupid.... */
	for(ptr = start; (void *)ptr < end; ptr += 16) {
		struct lb_header *head = (struct lb_header *)ptr;
		if (	(head->signature[0] != 'L') || 
			(head->signature[1] != 'B') ||
			(head->signature[2] != 'I') ||
			(head->signature[3] != 'O')) {
			continue;
		}
		if (head->header_bytes != sizeof(*head))
			continue;
#if defined(DEBUG_LINUXBIOS)
		printf("Found canidate at: %X\n", (unsigned long)head);
#endif
		if (compute_ip_checksum((uint16_t *)head, sizeof(*head)) != 0) 
			continue;
#if defined(DEBUG_LINUXBIOS)
		printf("header checksum o.k.\n");
#endif
		if (compute_ip_checksum((uint16_t *)(ptr + sizeof(*head)), head->table_bytes) !=
			head->table_checksum) {
			continue;
		}
#if defined(DEBUG_LINUXBIOS)
		printf("table checksum o.k.\n");
#endif
		if (count_lb_records(ptr + sizeof(*head), head->table_bytes) !=
			head->table_entries) {
			continue;
		}
#if defined(DEBUG_LINUXBIOS)
		printf("record count o.k.\n");
#endif
		*result = head;
		return 1;
	};
	return 0;
}

struct lb_memory *get_lbmem(void)
{
	struct lb_header *lb_table;
	int found;
#if defined(DEBUG_LINUXBIOS)
	printf("\nSearching for linuxbios tables...\n");
#endif /* DEBUG_LINUXBIOS */
	found = 0;

	/* This code is specific to linuxBIOS but could
	 * concievably be extended to work under a normal bios.
	 * but size is important...
	 */
	if (!found) {
		found = find_lb_table((void *)0x00000, (void *) 0x01000, &lb_table);
	}
	if (!found) {
		found = find_lb_table( (void *) 0xf0000, (void *) 0x100000, &lb_table);
	}
	if (found) {
		printk("Found LinuxBIOS table at: %X\n", (unsigned long)lb_table);
//		read_linuxbios_values(&meminfo, lb_table);
	}

	if(!found)
		return(0);

	read_linuxbios_values(lb_table);
	return(mem);
	
}

#if 0
unsigned long get_boot_order(unsigned long order)
{
	int i;
	int checksum_valid;
	checksum_valid = cmos_valid();
	for(i = 0; i < MAX_BOOT_ENTRIES; i++) {
		unsigned long boot;
		boot = order >> (i*BOOT_BITS) & BOOT_MASK;
		if (!lb_failsafe && checksum_valid && (lb_boot[i].bit > 0)) {
			boot = cmos_read(lb_boot[i].bit, lb_boot[i].length);
			if ((boot & BOOT_TYPE_MASK) >= BOOT_NOTHING) {
				boot = BOOT_NOTHING;
			} else {
				/* Set the failsafe bit on all of 
				 * the boot entries... 
				 */
				cmos_write(lb_boot[i].bit, lb_boot[i].length,
					boot | BOOT_FAILSAFE);
			}
		}
		order &= ~(BOOT_MASK << (i * BOOT_BITS));
		order |= (boot << (i*BOOT_BITS));
	}
	return order;
}
#endif
