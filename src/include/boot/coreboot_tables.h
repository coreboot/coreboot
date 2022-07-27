/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef COREBOOT_TABLES_H
#define COREBOOT_TABLES_H

#include <commonlib/coreboot_tables.h>
#include <stddef.h>
#include <stdint.h>

/* function prototypes for building the coreboot table */

/*
 * Write forwarding table of target address at entry address returning size
 * of table written.
 */
size_t write_coreboot_forwarding_table(uintptr_t entry, uintptr_t target);

void fill_lb_gpios(struct lb_gpios *gpios);
void lb_add_gpios(struct lb_gpios *gpios, const struct lb_gpio *gpio_table,
		  size_t count);

void uart_fill_lb(void *data);
void lb_add_serial(struct lb_serial *serial, void *data);
void lb_add_console(uint16_t consoletype, void *data);

enum cb_err lb_fill_pcie(struct lb_pcie *pcie);

/* Define this in mainboard.c to add board-specific table entries. */
void lb_board(struct lb_header *header);

/* Define this function to fill in the frame buffer returning 0 on success and
   < 0 on error. */
int fill_lb_framebuffer(struct lb_framebuffer *framebuffer);

/* Allow arch to add records. */
void lb_arch_add_records(struct lb_header *header);

/*
 * Function to retrieve MAC address(es) from the VPD and store them in the
 * coreboot table.
 */
void lb_table_add_macs_from_vpd(struct lb_header *header);

void lb_table_add_serialno_from_vpd(struct lb_header *header);

struct lb_record *lb_new_record(struct lb_header *header);

/* Add VBOOT VBNV offsets. */
void lb_table_add_vbnv_cmos(struct lb_header *header);

#endif /* COREBOOT_TABLES_H */
