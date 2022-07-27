/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef BOOT_TABLES_H
#define BOOT_TABLES_H

#include <stdint.h>

/*
 * Write architecture specific tables as well as the common
 * coreboot table.
 * Returns a pointer to the table or NULL on error.
 */
void *write_tables(void);

/*
 * Allow per-architecture table writes called from write_tables(). The
 * coreboot_table parameter provides a reference to where the coreboot
 * table will be written. The parameter is to allow architectures to
 * provide a forwarding table to real coreboot table.
 */
void arch_write_tables(uintptr_t coreboot_table);

#endif /* BOOT_TABLES_H */
