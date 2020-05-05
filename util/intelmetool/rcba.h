/* SPDX-License-Identifier: GPL-2.0-or-later */

int write_rcba32(uint32_t addr, uint32_t val);
int read_rcba32(uint32_t addr, uint32_t *val);
u32 get_rcba_phys(void);
