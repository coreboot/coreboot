/* SPDX-License-Identifier: GPL-2.0-or-later */

#ifndef SOUTHBRIDGE_INTEL_LYNXPOINT_HSIO_H
#define SOUTHBRIDGE_INTEL_LYNXPOINT_HSIO_H

#include <southbridge/intel/lynxpoint/iobp.h>
#include <types.h>

struct hsio_table_row {
	uint32_t addr;
	uint32_t and;
	uint32_t or;
};

static inline void hsio_update(const uint32_t addr, const uint32_t and, const uint32_t or)
{
	pch_iobp_update(addr, and, or);
}

static inline void hsio_update_row(const struct hsio_table_row row)
{
	hsio_update(row.addr, row.and, row.or);
}

void hsio_xhci_shared_update(const uint32_t addr, const uint32_t and, const uint32_t or);
void hsio_sata_shared_update(const uint32_t addr, const uint32_t and, const uint32_t or);

static inline void hsio_sata_shared_update_row(const struct hsio_table_row row)
{
	hsio_sata_shared_update(row.addr, row.and, row.or);
}

static inline void hsio_xhci_shared_update_row(const struct hsio_table_row row)
{
	hsio_xhci_shared_update(row.addr, row.and, row.or);
}

void program_hsio_sata_lpt_h_cx(const bool is_mobile);
void program_hsio_xhci_lpt_h_cx(void);
void program_hsio_igbe_lpt_h_cx(void);

void program_hsio_sata_lpt_lp_bx(const bool is_mobile);
void program_hsio_xhci_lpt_lp_bx(void);
void program_hsio_igbe_lpt_lp_bx(void);

#endif
