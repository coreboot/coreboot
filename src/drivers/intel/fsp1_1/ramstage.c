/* SPDX-License-Identifier: GPL-2.0-only */

#include <bootmode.h>
#include <bootsplash.h>
#include <acpi/acpi.h>
#include <console/console.h>
#include <fsp/ramstage.h>
#include <fsp/util.h>
#include <framebuffer_info.h>
#include <lib.h>
#include <stage_cache.h>
#include <string.h>
#include <timestamp.h>

static void display_hob_info(FSP_INFO_HEADER *fsp_info_header)
{
	const EFI_GUID graphics_info_guid = EFI_PEI_GRAPHICS_INFO_HOB_GUID;
	void *hob_list_ptr = get_hob_list();

	/* Verify the HOBs */
	if (hob_list_ptr == NULL) {
		printk(BIOS_ERR, "HOB pointer is NULL!\n");
		return;
	}

	if (CONFIG(DISPLAY_HOBS))
		print_hob_type_structure(0, hob_list_ptr);

	/*
	 * Verify that FSP is generating the required HOBs:
	 *	7.1: FSP_BOOTLOADER_TEMP_MEMORY_HOB only produced for FSP 1.0
	 *	7.2: FSP_RESERVED_MEMORY_RESOURCE_HOB verified by raminit
	 *	7.3: FSP_NON_VOLATILE_STORAGE_HOB verified by raminit
	 *	7.4: FSP_BOOTLOADER_TOLUM_HOB verified by raminit
	 *	7.5: EFI_PEI_GRAPHICS_INFO_HOB verified below,
	 *	     if the ImageAttribute bit is set
	 *	FSP_SMBIOS_MEMORY_INFO HOB verified by raminit
	 */
	if ((fsp_info_header->ImageAttribute & GRAPHICS_SUPPORT_BIT) &&
		!get_guid_hob(&graphics_info_guid, hob_list_ptr) &&
		CONFIG(DISPLAY_HOBS)) {
		printk(BIOS_ERR, "7.5: EFI_PEI_GRAPHICS_INFO_HOB missing!\n");
		printk(BIOS_ERR, "Missing one or more required FSP HOBs!\n");
	}
}

static void fsp_run_silicon_init(FSP_INFO_HEADER *fsp_info_header)
{
	FSP_SILICON_INIT fsp_silicon_init;
	SILICON_INIT_UPD *original_params;
	SILICON_INIT_UPD silicon_init_params;
	EFI_STATUS status;
	UPD_DATA_REGION *upd_ptr;
	VPD_DATA_REGION *vpd_ptr;

	/* Display the FSP header */
	if (fsp_info_header == NULL) {
		printk(BIOS_ERR, "FSP_INFO_HEADER not set!\n");
		return;
	}
	print_fsp_info(fsp_info_header);

	/* Initialize the UPD values */
	vpd_ptr = (VPD_DATA_REGION *)(fsp_info_header->CfgRegionOffset +
					fsp_info_header->ImageBase);
	printk(BIOS_DEBUG, "%p: VPD Data\n", vpd_ptr);
	upd_ptr = (UPD_DATA_REGION *)(vpd_ptr->PcdUpdRegionOffset +
					fsp_info_header->ImageBase);
	printk(BIOS_DEBUG, "%p: UPD Data\n", upd_ptr);
	original_params = (void *)((u8 *)upd_ptr +
		upd_ptr->SiliconInitUpdOffset);
	memcpy(&silicon_init_params, original_params,
		sizeof(silicon_init_params));
	soc_silicon_init_params(&silicon_init_params);

	/* Locate VBT and pass to FSP GOP */
	if (CONFIG(RUN_FSP_GOP))
		load_vbt(&silicon_init_params);
	mainboard_silicon_init_params(&silicon_init_params);

	if (CONFIG(BMP_LOGO))
		bmp_load_logo(&silicon_init_params.PcdLogoPtr,
			      &silicon_init_params.PcdLogoSize);

	/* Display the UPD data */
	if (CONFIG(DISPLAY_UPD_DATA))
		soc_display_silicon_init_params(original_params,
			&silicon_init_params);

	/* Perform silicon initialization after RAM is configured */
	printk(BIOS_DEBUG, "Calling FspSiliconInit\n");
	fsp_silicon_init = (FSP_SILICON_INIT)(fsp_info_header->ImageBase
		+ fsp_info_header->FspSiliconInitEntryOffset);
	timestamp_add_now(TS_FSP_SILICON_INIT_START);
	printk(BIOS_DEBUG, "Calling FspSiliconInit(%p) at %p\n",
		&silicon_init_params, fsp_silicon_init);
	post_code(POST_FSP_SILICON_INIT);
	status = fsp_silicon_init(&silicon_init_params);
	timestamp_add_now(TS_FSP_SILICON_INIT_END);
	printk(BIOS_DEBUG, "FspSiliconInit returned 0x%08x\n", status);

	/* The logo_entry can be freed up now as it is not required any longer */
	if (CONFIG(BMP_LOGO))
		bmp_release_logo();

	/* Mark graphics init done after SiliconInit if VBT was provided */
#if CONFIG(RUN_FSP_GOP)
	/* GraphicsConfigPtr doesn't exist in Quark X1000's FSP, so this needs
	 * to be #if'd out instead of using if (). */
	if (silicon_init_params.GraphicsConfigPtr)
		gfx_set_init_done(1);
#endif

	if (CONFIG(RUN_FSP_GOP)) {
		const EFI_GUID vbt_guid = EFI_PEI_GRAPHICS_INFO_HOB_GUID;
		u32 *vbt_hob;

		void *hob_list_ptr = get_hob_list();
		vbt_hob = get_guid_hob(&vbt_guid, hob_list_ptr);
		if (vbt_hob == NULL) {
			printk(BIOS_ERR, "FSP_ERR: Graphics Data HOB is not present\n");
		} else {
			EFI_PEI_GRAPHICS_INFO_HOB *gop;

			printk(BIOS_DEBUG, "FSP_DEBUG: Graphics Data HOB present\n");
			gop = GET_GUID_HOB_DATA(vbt_hob);

			fb_add_framebuffer_info(gop->FrameBufferBase,
						gop->GraphicsMode.HorizontalResolution,
						gop->GraphicsMode.VerticalResolution,
						gop->GraphicsMode.PixelsPerScanLine * 4,
						32);
		}
	}

	display_hob_info(fsp_info_header);
}

static void fsp_load(void)
{
	struct prog fsp = PROG_INIT(PROG_REFCODE, "fsp.bin");

	if (resume_from_stage_cache()) {
		stage_cache_load_stage(STAGE_REFCODE, &fsp);
	} else {
		fsp_relocate(&fsp);

		if (prog_entry(&fsp))
			stage_cache_add(STAGE_REFCODE, &fsp);
	}

	/* FSP_INFO_HEADER is set as the program entry. */
	fsp_update_fih(prog_entry(&fsp));
}

void intel_silicon_init(void)
{
	fsp_load();
	fsp_run_silicon_init(fsp_get_fih());
}

/* Initialize the UPD parameters for SiliconInit */
__weak void mainboard_silicon_init_params(
	SILICON_INIT_UPD *params)
{
}
