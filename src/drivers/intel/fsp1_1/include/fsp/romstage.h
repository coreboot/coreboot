/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2014 Google Inc.
 * Copyright (C) 2015-2016 Intel Corporation
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

#ifndef _COMMON_ROMSTAGE_H_
#define _COMMON_ROMSTAGE_H_

#include <stdint.h>
#include <arch/cpu.h>
#include <memory_info.h>
#include <fsp/car.h>
#include <fsp/util.h>
#include <soc/intel/common/util.h>
#include <soc/intel/common/mma.h>
#include <soc/pei_wrapper.h>
#include <soc/pm.h>		/* chip_power_state */

struct romstage_params {
	uint32_t fsp_version;
	struct chipset_power_state *power_state;
	struct pei_data *pei_data;
	void *chipset_context;
};

/*
 * FSP Boot Flow:
 *   1.  src/cpu/x86/16bit/reset.inc
 *   2.  src/cpu/x86/16bit/entry.inc
 *   3.  other modules
 *   4.  src/drivers/intel/fsp1_1/cache_as_ram.inc
 *   5.  src/drivers/intel/fsp1_1/fsp_util.c/find_fsp
 *   6.  FSP binary/TempRamInit
 *   7.  src/drivers/intel/fsp1_1/cache_as_ram.inc - return
 *   8.  src/soc/intel/common/romstage.c/romstage_main
 *   9   src/soc/.../romstage/.../soc_pre_console_init
 *  10   src/console/console.c/console_init
 *  11   src/soc/.../romstage/.../soc_romstage_init
 *  12.  src/mainboard/.../romstage.c/mainboard_romstage_entry
 *  13.  src/soc/intel/common/romstage.c/romstage_common
 *  14   src/soc/.../romstage/.../soc_pre_raminit
 *  15.  FSP binary/MemoryInit
 *  16.  src/soc/intel/common/romstage.c/romstage_common - return
 *  17.  src/mainboard/.../romstage.c/mainboard_romstage_entry - return
 *  18.  src/soc/intel/common/romstage.c/romstage_main - return
 *  19.  src/soc/intel/common/stack.c/setup_stack_and_mtrrs
 *  20.  src/drivers/intel/fsp1_1/cache_as_ram.inc - return, cleanup
 *       after call to romstage_main
 *  21.  FSP binary/TempRamExit
 *  22.  src/soc/intel/common/romstage.c/romstage_after_car
 *  23.  FSP binary/SiliconInit
 *  24.  src/soc/intel/common/romstage.c/romstage_after_car - return
 *  25.  src/drivers/intel/fsp1_1/fsp_util.c/fsp_notify_boot_state_callback
 *  26.  src/drivers/intel/fsp1_1/fsp_util.c/fsp_notify
 *  27.  FSP binary/FspNotify
 *  28.  src/drivers/intel/fsp1_1/fsp_util.c/fsp_notify_boot_state_callback
 *  29.  src/drivers/intel/fsp1_1/fsp_util.c/fsp_notify
 *  30.  FSP binary/FspNotify
 */

void mainboard_memory_init_params(struct romstage_params *params,
	MEMORY_INIT_UPD *memory_params);
void mainboard_romstage_entry(struct romstage_params *params);
void mainboard_save_dimm_info(struct romstage_params *params);
void mainboard_add_dimm_info(struct romstage_params *params,
			     struct memory_info *mem_info,
			     int channel, int dimm, int index);
void raminit(struct romstage_params *params);
void report_memory_config(void);
void romstage_common(struct romstage_params *params);
asmlinkage void *romstage_main(FSP_INFO_HEADER *fih);
/* Initialize memory margin analysis settings. */
void setup_mma(MEMORY_INIT_UPD *memory_upd);
void *setup_stack_and_mtrrs(void);
void soc_after_ram_init(struct romstage_params *params);
void soc_display_memory_init_params(const MEMORY_INIT_UPD *old,
	MEMORY_INIT_UPD *new);
void soc_memory_init_params(struct romstage_params *params,
			    MEMORY_INIT_UPD *upd);
void soc_pre_ram_init(struct romstage_params *params);
/* Update the SOC specific memory config param for mma. */
void soc_update_memory_params_for_mma(MEMORY_INIT_UPD *memory_cfg,
		struct mma_config_param *mma_cfg);

#endif /* _COMMON_ROMSTAGE_H_ */
