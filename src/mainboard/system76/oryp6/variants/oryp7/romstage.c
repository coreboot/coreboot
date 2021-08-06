/* SPDX-License-Identifier: GPL-2.0-only */

#include <variant/romstage.h>

void variant_configure_fspm(FSPM_UPD *memupd)
{
	// Typically, we set SaOcSupport to allow overclocking RAM,
	// but oryp7 saw a high fail rate when using 3200 MHz DIMMs.
	// So disable OC so modules run at the standard 2933 MHz.
	memupd->FspmConfig.SaOcSupport = 0;
}
