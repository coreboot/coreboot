/* SPDX-License-Identifier: GPL-2.0-only */

#include <fsp/api.h>
#include <FspmUpd.h>
#include <soc/romstage.h>

/*
* Configure GPIO depend on platform
*/
static void mainboard_config_gpios(FSPM_UPD *mupd)
{
	/* To be implemented */
}

static void mainboard_config_iio(FSPM_UPD *mupd)
{
	/* To be implemented */
}

void mainboard_memory_init_params(FSPM_UPD *mupd)
{
	mainboard_config_gpios(mupd);
	mainboard_config_iio(mupd);
}
