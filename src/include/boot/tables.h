#ifndef BOOT_TABLES_H
#define BOOT_TABLES_H

#include <mem.h>
#include <boot/linuxbios_tables.h>

struct lb_memory *write_tables(struct mem_range *mem, unsigned long *processor_map);

#endif /* BOOT_TABLES_H */
