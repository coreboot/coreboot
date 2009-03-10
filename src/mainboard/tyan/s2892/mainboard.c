#include <device/device.h>
#include <console/console.h>
#include <boot/tables.h>
#include "chip.h"

/* in arch/i386/boot/tables.c */
extern uint64_t high_tables_base, high_tables_size;

int add_mainboard_resources(struct lb_memory *mem)
{
#if HAVE_HIGH_TABLES == 1
	printk_debug("Adding high table area\n");
	lb_add_memory_range(mem, LB_MEM_TABLE,
		high_tables_base, high_tables_size);
#endif
	return 0;
}


#if CONFIG_CHIP_NAME == 1
struct chip_operations mainboard_ops = {
	CHIP_NAME("Tyan S2892 Mainboard")
};
#endif

