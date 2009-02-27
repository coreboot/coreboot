#ifndef BOOT_TABLES_H
#define BOOT_TABLES_H

#include <boot/coreboot_tables.h>

void lb_add_memory_range(struct lb_memory *mem,
	uint32_t type, uint64_t start, uint64_t size);

struct lb_memory *write_tables(void);

#endif /* BOOT_TABLES_H */
