/* SPDX-License-Identifier: GPL-2.0-or-later */

#ifndef HASWELL_VCU_MAILBOX_H
#define HASWELL_VCU_MAILBOX_H

#include <stdint.h>

uint32_t vcu_read_csr(uint32_t addr);
void vcu_write_csr(uint32_t addr, uint32_t data);
void vcu_update_csr(uint32_t addr, uint32_t andvalue, uint32_t orvalue);

uint32_t vcu_read_mmio(uint32_t addr);
void vcu_write_mmio(uint32_t addr, uint32_t data);
void vcu_update_mmio(uint32_t addr, uint32_t andvalue, uint32_t orvalue);

#endif /* HASWELL_VCU_MAILBOX_H */
