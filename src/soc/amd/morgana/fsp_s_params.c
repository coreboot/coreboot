/* SPDX-License-Identifier: GPL-2.0-only */

/* TODO: Update for Morgana */

#include <acpi/acpi.h>
#include <amdblocks/apob_cache.h>
#include <device/pci.h>
#include <fsp/api.h>
#include <program_loading.h>

static void fsp_assign_vbios_upds(FSP_S_CONFIG *scfg)
{
	scfg->vbios_buffer = CONFIG(RUN_FSP_GOP) ? PCI_VGA_RAM_IMAGE_START : 0;
}

void platform_fsp_silicon_init_params_cb(FSPS_UPD *supd)
{
	FSP_S_CONFIG *scfg = &supd->FspsConfig;

	fsp_assign_vbios_upds(scfg);

	/*
	 * At this point FSP-S has been loaded into RAM. If we were to start loading the APOB
	 * before FSP-S was loaded, we would introduce contention onto the SPI bus and
	 * slow down the FSP-S read from SPI. Since FSP-S takes a while to execute and performs
	 * no SPI operations, we can read the APOB while FSP-S executes.
	 */
	start_apob_cache_read();
	/*
	 * We enqueue the payload to be loaded after the APOB. This might cause a bit of
	 * bus contention when loading uCode and OPROMs, but since those calls happen at
	 * different points in the boot state machine it's a little harder to sequence all the
	 * async loading correctly. So in order to keep the complexity down, we enqueue the
	 * payload preload here. The end goal will be to add uCode and OPROM preloading
	 * before the payload so that the sequencing is correct.
	 *
	 * While FSP-S is executing, it's not currently possible to enqueue other transactions
	 * because FSP-S doesn't call `thread_yield()`. So the payload will start loading
	 * right after FSP-S completes.
	 */
	if (!acpi_is_wakeup_s3())
		payload_preload();
}
