#include <mem.h>
#include <ip_checksum.h>
#include <boot/linuxbios_tables.h>
#include <boot/linuxbios_table.h>
#include <printk.h>
#include <string.h>
#include <version.h>


struct lb_header *lb_table_init(unsigned long addr)
{
	struct lb_header *header;

	/* 16 byte align the address */
	addr += 15;
	addr &= ~15;

	header = (void *)addr;
	header->signature[0] = 'L';
	header->signature[1] = 'B';
	header->signature[2] = 'I';
	header->signature[3] = 'O';
	header->header_bytes = sizeof(*header);
	header->header_checksum = 0;
	header->table_bytes = 0;
	header->table_checksum = 0;
	header->table_entries = 0;
	return header;
}

struct lb_record *lb_first_record(struct lb_header *header)
{
	struct lb_record *rec;
	rec = (void *)(((char *)header) + sizeof(*header));
	return rec;
}

struct lb_record *lb_last_record(struct lb_header *header)
{
	struct lb_record *rec;
	rec = (void *)(((char *)header) + sizeof(*header) + header->table_bytes);
	return rec;
}

struct lb_record *lb_next_record(struct lb_record *rec)
{
	rec = (void *)(((char *)rec) + rec->size);	
	return rec;
}

struct lb_record *lb_new_record(struct lb_header *header)
{
	struct lb_record *rec;
	rec = lb_last_record(header);
	if (header->table_entries) {
		header->table_bytes += rec->size;
	}
	rec = lb_last_record(header);
	header->table_entries++;
	rec->tag = LB_TAG_UNUSED;
	rec->size = sizeof(*rec);
	return rec;
}


struct lb_memory *lb_memory(struct lb_header *header)
{
	struct lb_record *rec;
	struct lb_memory *mem;
	rec = lb_new_record(header);
	mem = (struct lb_memory *)rec;
	mem->tag = LB_TAG_MEMORY;
	mem->size = sizeof(*mem);
	return mem;
}

struct lb_mainboard *lb_mainboard(struct lb_header *header)
{
	struct lb_record *rec;
	struct lb_mainboard *mainboard;
	rec = lb_new_record(header);
	mainboard = (struct lb_mainboard *)rec;
	mainboard->tag = LB_TAG_MAINBOARD;

	mainboard->size = (sizeof(*mainboard) +
		strlen(mainboard_vendor) + 1 + 
		strlen(mainboard_part_number) + 1 +
		3) & ~3;

	mainboard->vendor_idx = 0;
	mainboard->part_number_idx = strlen(mainboard_vendor) + 1;

	memcpy(mainboard->strings + mainboard->vendor_idx,
		mainboard_vendor,      strlen(mainboard_vendor) + 1);
	memcpy(mainboard->strings + mainboard->part_number_idx,
		mainboard_part_number, strlen(mainboard_part_number) + 1);

	return mainboard;
}

void lb_strings(struct lb_header *header)
{
	static const struct {
		uint32_t tag;
		const uint8_t *string;
	} strings[] = {
		{ LB_TAG_VERSION,        linuxbios_version,        },
		{ LB_TAG_EXTRA_VERSION,  linuxbios_extra_version,  },
		{ LB_TAG_BUILD,          linuxbios_build,          },
		{ LB_TAG_COMPILE_TIME,   linuxbios_compile_time,   },
		{ LB_TAG_COMPILE_BY,     linuxbios_compile_by,     },
		{ LB_TAG_COMPILE_HOST,   linuxbios_compile_host,   },
		{ LB_TAG_COMPILE_DOMAIN, linuxbios_compile_domain, },
		{ LB_TAG_COMPILER,       linuxbios_compiler,       },
		{ LB_TAG_LINKER,         linuxbios_linker,         },
		{ LB_TAG_ASSEMBLER,      linuxbios_assembler,      },
	};
	int i;
	for(i = 0; i < sizeof(strings)/sizeof(strings[0]); i++) {
		struct lb_record *rec;
		struct lb_string *str;
		size_t len;
		rec = lb_new_record(header);
		str = (struct lb_string *)rec;
		len = strlen(strings[i].string);
		str->tag = strings[i].tag;
		str->size = (sizeof(*rec) + len + 1 + 3) & ~3;
		memcpy(str->string, strings[i].string, len+1);
	}

}

/* Some version of gcc have problems with 64 bit types so
 * take an unsigned long instead of a uint64_t for now.
 */
void lb_memory_range(struct lb_memory *mem,
	uint32_t type, unsigned long start, unsigned long size)
{
	int entries;
	entries = (mem->size - sizeof(*mem))/sizeof(mem->map[0]);
	mem->map[entries].start = start;
	mem->map[entries].size = size;
	mem->map[entries].type = type;
	mem->size += sizeof(mem->map[0]);
}

static void lb_memory_rangek(struct lb_memory *mem,
	uint32_t type, unsigned long startk, unsigned long endk)
{
	int entries;
	entries = (mem->size - sizeof(*mem))/sizeof(mem->map[0]);
	mem->map[entries].start = startk;
	mem->map[entries].start <<= 10;
	mem->map[entries].size = endk - startk;
	mem->map[entries].size <<= 10;
	mem->map[entries].type = type;
	mem->size += sizeof(mem->map[0]);
}

static void lb_reserve_table_memory(struct lb_header *head)
{
	struct lb_record *last_rec;
	struct lb_memory *mem;
	uint64_t start;
	uint64_t end;
	int i, entries;

	last_rec = lb_last_record(head);
	mem = get_lb_mem();
	start = (unsigned long)head;
	end = (unsigned long)last_rec;
	entries = (mem->size - sizeof(*mem))/sizeof(mem->map[0]);
	/* Resize the right two memory areas so this table is in
	 * a reserved area of memory.  Everything has been carefully
	 * setup so that is all we need to do.
	 */
	for(i = 0; i < entries; i++ ) {
		uint64_t map_start = mem->map[i].start;
		uint64_t map_end = map_start + mem->map[i].size;
		/* Does this area need to be expanded? */
		if (map_end == start) {
			mem->map[i].size = end - map_start;
		}
		/* Does this area need to be contracted? */
		else if (map_start == start) {
			mem->map[i].start = end;
			mem->map[i].size = map_end - end;
		}
	}
}


unsigned long lb_table_fini(struct lb_header *head)
{
	struct lb_record *rec, *first_rec;
	rec = lb_last_record(head);
	if (head->table_entries) {
		head->table_bytes += rec->size;
	}
	lb_reserve_table_memory(head);
	first_rec = lb_first_record(head);
	head->table_checksum = compute_ip_checksum(first_rec, head->table_bytes);
	head->header_checksum = 0;
	head->header_checksum = compute_ip_checksum(head, sizeof(*head));
	printk_debug("Wrote linuxbios table at: %p - %p  checksum %lx\n",
		head, rec, head->table_checksum);
	return (unsigned long)rec;
}


/* Routines to extract part so the linuxBIOS table or 
 * information from the linuxBIOS table after we have written it.
 * Currently get_lb_mem relies on a global we can change the
 * implementaiton.
 */
static struct lb_memory *mem_ranges = 0;
struct lb_memory *get_lb_mem(void)
{
	return mem_ranges;
}

unsigned long write_linuxbios_table( 
	unsigned long *processor_map, 
	struct mem_range *ram,
	unsigned long low_table_start, unsigned long low_table_end,
	unsigned long rom_table_startk, unsigned long rom_table_endk)
{
	unsigned long table_size;
	struct mem_range *ramp;
	struct lb_header *head;
	struct lb_memory *mem;
#if HAVE_OPTION_TABLE == 1
	struct lb_record *rec_dest, *rec_src;
#endif	

	head = lb_table_init(low_table_end);
	low_table_end = (unsigned long)head;
#if HAVE_OPTION_TABLE == 1
	/* Write the option config table... */
	rec_dest = lb_new_record(head);
	rec_src = (struct lb_record *)&option_table;
	memcpy(rec_dest,  rec_src, rec_src->size);
#endif	
	mem = lb_memory(head);
	mem_ranges = mem;
	/* I assume there is always ram at address 0 */
	/* Reserve our tables in low memory */
	table_size = (low_table_end - low_table_start);
	lb_memory_range(mem, LB_MEM_TABLE, 0, table_size);
	lb_memory_range(mem, LB_MEM_RAM, table_size, (ram[0].sizek << 10) - table_size);
	/* Reserving pci memory mapped  space will keep the kernel from booting seeing
	 * any pci resources.
	 */
	for(ramp = &ram[1]; ramp->sizek; ramp++) {
		unsigned long startk, endk;
		startk = ramp->basek;
		endk = startk + ramp->sizek;
		if ((startk < rom_table_startk) && (endk > rom_table_startk)) {
			lb_memory_rangek(mem, LB_MEM_RAM, startk, rom_table_startk);
			startk = rom_table_startk;
		}
		if ((startk == rom_table_startk) && (endk > startk)) {
			unsigned long tend;
			tend = rom_table_endk;
			if (tend > endk) {
				tend = endk;
			}
			lb_memory_rangek(mem, LB_MEM_TABLE, rom_table_startk, tend);
			startk = tend;
		}
		if (endk > startk) {
			lb_memory_rangek(mem, LB_MEM_RAM, startk, endk);
		}
	}

	/* Record our motheboard */
	lb_mainboard(head);
	/* Record our various random string information */
	lb_strings(head);

	low_table_end = lb_table_fini(head);

	/* Remember where my valid memory ranges are */
	return low_table_end;
}
