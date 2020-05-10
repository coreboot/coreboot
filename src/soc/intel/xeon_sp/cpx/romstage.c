/* SPDX-License-Identifier: GPL-2.0-only */

#include <arch/romstage.h>
#include <fsp/api.h>
#include <soc/romstage.h>
#include "chip.h"

void platform_fsp_memory_init_params_cb(FSPM_UPD *mupd, uint32_t version)
{
	FSP_M_CONFIG *m_cfg = &mupd->FspmConfig;
	FSPM_ARCH_UPD *arch_upd = &mupd->FspmArchUpd;

	(void)m_cfg;
	/*
	 * Currently FSP for CPX does not implement user-provided StackBase/Size
	 * properly. When KTI link needs to be trained, inter-socket communication
	 * library needs quite a bit of memory for its heap usage. However, location
	 * is hardcoded so this workaround is needed.
	 */
	if (CONFIG_MAX_SOCKET > 1) {
		arch_upd->StackBase = (void *) 0xfe930000;
		arch_upd->StackSize = 0x70000;
	}

	mainboard_memory_init_params(mupd);
}
