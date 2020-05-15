/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <cbfs.h>
#include <cbmem.h>
#include <commonlib/fsp.h>
#include <commonlib/stdlib.h>
#include <console/console.h>
#include <fsp/api.h>
#include <fsp/util.h>
#include <program_loading.h>
#include <soc/intel/common/vbt.h>
#include <stage_cache.h>
#include <string.h>
#include <timestamp.h>
#include <types.h>

struct fsp_header fsps_hdr;

static void do_silicon_init(struct fsp_header *hdr)
{
	FSPS_UPD *upd, *supd;
	fsp_silicon_init_fn silicon_init;
	uint32_t status;
	uint8_t postcode;
	const struct cbmem_entry *logo_entry = NULL;

	supd = (FSPS_UPD *) (hdr->cfg_region_offset + hdr->image_base);

	if (supd->FspUpdHeader.Signature != FSPS_UPD_SIGNATURE)
		die_with_post_code(POST_INVALID_VENDOR_BINARY,
			"Invalid FSPS signature\n");

	/* Disallow invalid config regions.  Default settings are likely bad
	 * choices for coreboot, and different sized UPD from what the region
	 * allows is potentially a build problem.
	 */
	if (!hdr->cfg_region_size || hdr->cfg_region_size != sizeof(FSPS_UPD))
		die_with_post_code(POST_INVALID_VENDOR_BINARY,
			"Invalid FSPS UPD region\n");

	upd = xmalloc(hdr->cfg_region_size);

	memcpy(upd, supd, hdr->cfg_region_size);

	/* Give SoC/mainboard a chance to populate entries */
	platform_fsp_silicon_init_params_cb(upd);

	/* Populate logo related entries */
	if (CONFIG(FSP2_0_DISPLAY_LOGO))
		logo_entry = soc_load_logo(upd);

	/* Call SiliconInit */
	silicon_init = (void *) (hdr->image_base +
				 hdr->silicon_init_entry_offset);
	fsp_debug_before_silicon_init(silicon_init, supd, upd);

	timestamp_add_now(TS_FSP_SILICON_INIT_START);
	post_code(POST_FSP_SILICON_INIT);
	status = silicon_init(upd);
	timestamp_add_now(TS_FSP_SILICON_INIT_END);
	post_code(POST_FSP_SILICON_EXIT);

	if (logo_entry)
		cbmem_entry_remove(logo_entry);

	fsp_debug_after_silicon_init(status);

	/* Handle any errors returned by FspSiliconInit */
	fsp_handle_reset(status);
	if (status != FSP_SUCCESS) {
		/* Assume video failure if attempted to initialize graphics */
		if (CONFIG(RUN_FSP_GOP) && vbt_get())
			postcode = POST_VIDEO_FAILURE;
		else
			postcode = POST_HW_INIT_FAILURE; /* else generic */

		printk(BIOS_SPEW, "FspSiliconInit returned 0x%08x\n", status);
		die_with_post_code(postcode,
			"FspSiliconInit returned an error!\n");
	}
}

static int fsps_get_dest(const struct fsp_load_descriptor *fspld, void **dest,
				size_t size, const struct region_device *source)
{
	*dest = cbmem_add(CBMEM_ID_REFCODE, size);

	if (*dest == NULL)
		return -1;

	return 0;
}

void fsps_load(bool s3wake)
{
	struct fsp_load_descriptor fspld = {
		.fsp_prog = PROG_INIT(PROG_REFCODE, CONFIG_FSP_S_CBFS),
		.get_destination = fsps_get_dest,
	};
	struct prog *fsps = &fspld.fsp_prog;
	static int load_done;

	if (load_done)
		return;

	if (s3wake && !CONFIG(NO_STAGE_CACHE)) {
		printk(BIOS_DEBUG, "Loading FSPS from stage_cache\n");
		stage_cache_load_stage(STAGE_REFCODE, fsps);
		if (fsp_validate_component(&fsps_hdr, prog_rdev(fsps)) != CB_SUCCESS)
			die("On resume fsps header is invalid\n");
		load_done = 1;
		return;
	}

	if (fsp_load_component(&fspld, &fsps_hdr) != CB_SUCCESS)
		die("FSP-S failed to load\n");

	stage_cache_add(STAGE_REFCODE, fsps);

	load_done = 1;
}

void fsp_silicon_init(bool s3wake)
{
	fsps_load(s3wake);
	do_silicon_init(&fsps_hdr);
}

/* Load bmp and set FSP parameters, fsp_load_logo can be used */
__weak const struct cbmem_entry *soc_load_logo(FSPS_UPD *supd)
{
	return NULL;
}
