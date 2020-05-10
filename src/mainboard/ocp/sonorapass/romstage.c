/* SPDX-License-Identifier: GPL-2.0-only */

#include <soc/romstage.h>

void mainboard_memory_init_params(FSPM_UPD *mupd)
{
	FSP_M_CONFIG *m_cfg = &mupd->FspmConfig;
	void *start = (void *) m_cfg;

	// BoardId
	*((uint8_t *) (start + 140)) = 0x1d;
	// BoardTypeBitmask
	*((uint32_t *) (start + 104)) = 0x11111111;
	// DebugPrintLevel
	*((uint8_t *) (start + 45)) = 8;
	// KtiLinkSpeedMode
	*((uint8_t *) (start + 64)) = 0;
	// mmiolSize
	*((uint32_t *) (start + 88)) = 0;
	// mmiohBase
	*((uint32_t *) (start + 92)) = 0x2000;
	// KtiPrefetchEn
	*((uint8_t *) (start + 53)) = 2;
	// KtiFpgaEnable
	*((uint8_t *) (start + 55)) = 0;
	*((uint8_t *) (start + 56)) = 0;
}
