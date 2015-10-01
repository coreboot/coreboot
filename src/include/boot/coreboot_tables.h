#ifndef COREBOOT_TABLES_H
#define COREBOOT_TABLES_H

#include <commonlib/coreboot_tables.h>
/* function prototypes for building the coreboot table */

unsigned long write_coreboot_table(
	unsigned long low_table_start, unsigned long low_table_end,
	unsigned long rom_table_start, unsigned long rom_table_end);

void fill_lb_gpios(struct lb_gpios *gpios);
void fill_lb_gpio(struct lb_gpio *gpio, int num,
			 int polarity, const char *name, int value);

void uart_fill_lb(void *data);
void lb_add_serial(struct lb_serial *serial, void *data);
void lb_add_console(uint16_t consoletype, void *data);

/* Define this in mainboard.c to add board-specific table entries. */
void lb_board(struct lb_header *header);

/* Define this in soc or fsp driver to add specific table entries. */
void lb_framebuffer(struct lb_header *header);

/*
 * Function to retrieve MAC address(es) from the VPD and store them in the
 * coreboot table.
 */
void lb_table_add_macs_from_vpd(struct lb_header *header);

void lb_table_add_serialno_from_vpd(struct lb_header *header);

struct lb_record *lb_new_record(struct lb_header *header);

#endif /* COREBOOT_TABLES_H */
