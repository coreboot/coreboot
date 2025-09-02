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

enum cb_err fill_lb_serial(struct lb_serial *serial);
void lb_add_console(uint16_t consoletype, void *data);

enum cb_err fill_lb_pcie(struct lb_pcie *pcie);

/* Define this in mainboard.c to add board-specific table entries. */
void lb_board(struct lb_header *header);

/* Adds LB_TAG_EFI_FW_INFO table entry. */
void lb_efi_fw_info(struct lb_header *header);

/* Adds LB_TAG_CAPSULE table entries. */
void lb_efi_capsules(struct lb_header *header);

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

/* Add Boot Mode information */
void lb_add_boot_mode(struct lb_header *header);

/* Define this in mainboard.c to add board specific CFR entries */
void mb_cfr_setup_menu(struct lb_cfr *cfr_root);

#endif /* COREBOOT_TABLES_H */
