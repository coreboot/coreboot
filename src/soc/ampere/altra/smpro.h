/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef SOC_AMPERE_ALTRA_SMPRO_H
#define SOC_AMPERE_ALTRA_SMPRO_H

#include <stdint.h>

/* Access a register of socket 0 through the SMpro mailbox. Return 0 on success. */
int smpro_reg_read(uint64_t addr, uint32_t *val);
int smpro_reg_write(uint64_t addr, uint32_t val);

/*
 * Send a message through an SMpro or PMpro doorbell and wait for the
 * acknowledgment. Return 0 on success.
 */
int ac01_mailbox_write(uintptr_t db, uint32_t data, uint32_t ext0, uint32_t ext1);

/* Let the doorbell raise its interrupt when a message is available */
void ac01_mailbox_unmask_interrupt(uintptr_t db);

#endif
