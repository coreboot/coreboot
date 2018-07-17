/*
 * This file is part of the coreboot project.
 *
 * Copyright 2018-present  Facebook, Inc.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; version 2 of
 * the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */
#ifndef SRC_SOC_CAVIUM_COMMON_INCLUDE_SOC_BOOTBLOCK_H_
#define SRC_SOC_CAVIUM_COMMON_INCLUDE_SOC_BOOTBLOCK_H_

void bootblock_mainboard_early_init(void);
void bootblock_soc_early_init(void);
void bootblock_soc_init(void);
void bootblock_mainboard_init(void);

void bootblock_main(const uint64_t reg_x0,
		    const uint64_t reg_x1,
		    const uint64_t reg_pc);


#endif /* SRC_SOC_CAVIUM_COMMON_INCLUDE_SOC_BOOTBLOCK_H_ */
