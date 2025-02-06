/* SPDX-License-Identifier: GPL-2.0-only */

/* TODO: Check if this is still correct */

#include <acpi/acpi.h>
#include <amdblocks/apob_cache.h>
#include <amdblocks/vbios_cache.h>
#include <amdblocks/vbt.h>
#include <bootmode.h>
#include <bootsplash.h>
#include <console/console.h>
#include <device/pci.h>
#include <fsp/api.h>
#include <program_loading.h>

static void fsp_assign_vbios_upds(FSP_S_CONFIG *scfg)
{
	if (CONFIG(USE_SELECTIVE_GOP_INIT) && vbios_cache_is_valid() &&
			!display_init_required()) {
		scfg->vbios_buffer = 0;
		printk(BIOS_SPEW, "%s: using VBIOS cache; skipping GOP driver.\n", __func__);
		return;
	}
	/*
	 * The VBIOS contains the ATOMBIOS tables that will be modified as
	 * part of FSP GOP init. We can delay loading of the VBIOS until
	 * before FSP notify AFTER_PCI_ENUM.
	 */
	printk(BIOS_SPEW, "%s: not using VBIOS cache; running GOP driver.\n", __func__);
	scfg->vbios_buffer = (uintptr_t)vbt_get();
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

void soc_load_logo(FSPS_UPD *supd)
{
	size_t logo_size;
	supd->FspsConfig.logo_bmp_buffer = (uint32_t)(uintptr_t)bmp_load_logo(&logo_size);
}
