#ifndef COREBOOT_TABLE_H
#define COREBOOT_TABLE_H

#include <boot/coreboot_tables.h>

/* This file holds function prototypes for building the coreboot table. */
unsigned long write_coreboot_table(
	unsigned long low_table_start, unsigned long low_table_end,
	unsigned long rom_table_start, unsigned long rom_table_end);

void lb_memory_range(struct lb_memory *mem,
	uint32_t type, uint64_t start, uint64_t size);

/* Routines to extract part so the coreboot table or information
 * from the coreboot table.
 */
struct lb_memory *get_lb_mem(void);

/* defined by mainboard.c if the mainboard requires extra resources */
int add_mainboard_resources(struct lb_memory *mem);
int add_northbridge_resources(struct lb_memory *mem);

#endif /* COREBOOT_TABLE_H */
