/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef _COMMON_ROMSTAGE_H_
#define _COMMON_ROMSTAGE_H_

#include <stddef.h>
#include <stdint.h>
#include <memory_info.h>
#include <fsp/car.h>
#include <fsp/util.h>
#include <soc/intel/common/mma.h>
#include <soc/pm.h>

struct romstage_params {
	uint32_t fsp_version;
	struct chipset_power_state *power_state;
	void *chipset_context;

	/* Fast boot and S3 resume MRC data */
	size_t saved_data_size;
	const void *saved_data;
	bool disable_saved_data;

	/* New save data from MRC */
	size_t data_to_save_size;
	const void *data_to_save;
};

void mainboard_memory_init_params(struct romstage_params *params,
	MEMORY_INIT_UPD *memory_params);
void mainboard_pre_raminit(struct romstage_params *params);
void mainboard_save_dimm_info(struct romstage_params *params);
void mainboard_add_dimm_info(struct romstage_params *params,
			     struct memory_info *mem_info,
			     int channel, int dimm, int index);
void raminit(struct romstage_params *params);
/* Initialize memory margin analysis settings. */
void setup_mma(MEMORY_INIT_UPD *memory_upd);
void soc_after_ram_init(struct romstage_params *params);
void soc_display_memory_init_params(const MEMORY_INIT_UPD *old,
	MEMORY_INIT_UPD *new);
void soc_memory_init_params(struct romstage_params *params,
			    MEMORY_INIT_UPD *upd);
void soc_pre_ram_init(struct romstage_params *params);
/* Update the SOC specific memory config param for mma. */
void soc_update_memory_params_for_mma(MEMORY_INIT_UPD *memory_cfg,
		struct mma_config_param *mma_cfg);
void mainboard_after_memory_init(void);

#endif /* _COMMON_ROMSTAGE_H_ */
