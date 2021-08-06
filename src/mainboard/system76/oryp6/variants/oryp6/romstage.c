/* SPDX-License-Identifier: GPL-2.0-only */

#include <variant/romstage.h>

void variant_configure_fspm(FSPM_UPD *memupd)
{
	// Allow memory speeds higher than 2933 MT/s
	memupd->FspmConfig.SaOcSupport = 1;
}
