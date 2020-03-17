/*
 * This file is part of the coreboot project.
 *
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#ifndef _QUARK_ROMSTAGE_H_
#define _QUARK_ROMSTAGE_H_

#include <soc/car.h>
#include <soc/reg_access.h>

void clear_smi_and_wake_events(void);
void disable_rom_shadow(void);
void *locate_rmu_file(size_t *rmu_file_len);
void report_platform_info(void);
int set_base_address_and_enable_uart(u8 bus, u8 dev, u8 func, u32 mmio_base);
void pcie_init(void);

#endif /* _QUARK_ROMSTAGE_H_ */
