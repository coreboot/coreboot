/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2014-2016 Intel Corporation
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <arch/acpi.h>
#include <cbmem.h>
#include <compiler.h>
#include <console/console.h>
#include <fsp/memmap.h>
#include <fsp/romstage.h>
#include <fsp/util.h>
#include <lib.h> /* hexdump */
#include <reset.h>
#include <string.h>
#include <timestamp.h>
#include <security/vboot/vboot_common.h>

void raminit(struct romstage_params *params)
{
	const EFI_GUID bootldr_tolum_guid = FSP_BOOTLOADER_TOLUM_HOB_GUID;
	EFI_HOB_RESOURCE_DESCRIPTOR *cbmem_root;
	FSP_INFO_HEADER *fsp_header;
	EFI_HOB_RESOURCE_DESCRIPTOR *fsp_memory;
	FSP_MEMORY_INIT fsp_memory_init;
	FSP_MEMORY_INIT_PARAMS fsp_memory_init_params;
	const EFI_GUID fsp_reserved_guid =
		FSP_RESERVED_MEMORY_RESOURCE_HOB_GUID;
	void *fsp_reserved_memory_area;
	FSP_INIT_RT_COMMON_BUFFER fsp_rt_common_buffer;
	void *hob_list_ptr;
	FSP_SMBIOS_MEMORY_INFO *memory_info_hob;
	const EFI_GUID memory_info_hob_guid = FSP_SMBIOS_MEMORY_INFO_GUID;
	MEMORY_INIT_UPD memory_init_params;
	const EFI_GUID mrc_guid = FSP_NON_VOLATILE_STORAGE_HOB_GUID;
	u32 *mrc_hob;
	u32 fsp_reserved_bytes;
	MEMORY_INIT_UPD *original_params;
	struct pei_data *pei_ptr;
	EFI_STATUS status;
	VPD_DATA_REGION *vpd_ptr;
	UPD_DATA_REGION *upd_ptr;
	int fsp_verification_failure = 0;
#if IS_ENABLED(CONFIG_DISPLAY_HOBS)
	unsigned long int data;
	EFI_PEI_HOB_POINTERS hob_ptr;
#endif

	/*
	 * Find and copy the UPD region to the stack so the platform can modify
	 * the settings if needed.  Modifications to the UPD buffer are done in
	 * the platform callback code.  The platform callback code is also
	 * responsible for assigning the UpdDataRngPtr to this buffer if any
	 * updates are made.  The default state is to leave the UpdDataRngPtr
	 * set to NULL.  This indicates that the FSP code will use the UPD
	 * region in the FSP binary.
	 */
	post_code(0x34);
	fsp_header = params->chipset_context;
	vpd_ptr = (VPD_DATA_REGION *)(fsp_header->CfgRegionOffset +
					fsp_header->ImageBase);
	printk(BIOS_DEBUG, "VPD Data: 0x%p\n", vpd_ptr);
	upd_ptr = (UPD_DATA_REGION *)(vpd_ptr->PcdUpdRegionOffset +
					fsp_header->ImageBase);
	printk(BIOS_DEBUG, "UPD Data: 0x%p\n", upd_ptr);
	original_params = (void *)((u8 *)upd_ptr +
		upd_ptr->MemoryInitUpdOffset);
	memcpy(&memory_init_params, original_params,
		sizeof(memory_init_params));

	/* Zero fill RT Buffer data and start populating fields. */
	memset(&fsp_rt_common_buffer, 0, sizeof(fsp_rt_common_buffer));
	pei_ptr = params->pei_data;
	if (pei_ptr->boot_mode == ACPI_S3) {
		fsp_rt_common_buffer.BootMode = BOOT_ON_S3_RESUME;
	} else if (pei_ptr->saved_data != NULL) {
		fsp_rt_common_buffer.BootMode =
			BOOT_ASSUMING_NO_CONFIGURATION_CHANGES;
	} else {
		fsp_rt_common_buffer.BootMode = BOOT_WITH_FULL_CONFIGURATION;
	}
	fsp_rt_common_buffer.UpdDataRgnPtr = &memory_init_params;
	fsp_rt_common_buffer.BootLoaderTolumSize = cbmem_overhead_size();

	/* Get any board specific changes */
	fsp_memory_init_params.NvsBufferPtr = (void *)pei_ptr->saved_data;
	fsp_memory_init_params.RtBufferPtr = &fsp_rt_common_buffer;
	fsp_memory_init_params.HobListPtr = &hob_list_ptr;

	/* Update the UPD data */
	soc_memory_init_params(params, &memory_init_params);
	mainboard_memory_init_params(params, &memory_init_params);

	if (IS_ENABLED(CONFIG_MMA))
		setup_mma(&memory_init_params);

	post_code(0x36);

	/* Display the UPD data */
	if (IS_ENABLED(CONFIG_DISPLAY_UPD_DATA))
		soc_display_memory_init_params(original_params,
			&memory_init_params);

	/* Call FspMemoryInit to initialize RAM */
	fsp_memory_init = (FSP_MEMORY_INIT)(fsp_header->ImageBase
		+ fsp_header->FspMemoryInitEntryOffset);
	printk(BIOS_DEBUG, "Calling FspMemoryInit: 0x%p\n", fsp_memory_init);
	printk(BIOS_SPEW, "    0x%p: NvsBufferPtr\n",
		fsp_memory_init_params.NvsBufferPtr);
	printk(BIOS_SPEW, "    0x%p: RtBufferPtr\n",
		fsp_memory_init_params.RtBufferPtr);
	printk(BIOS_SPEW, "    0x%p: HobListPtr\n",
		fsp_memory_init_params.HobListPtr);

	timestamp_add_now(TS_FSP_MEMORY_INIT_START);
	post_code(POST_FSP_MEMORY_INIT);
	status = fsp_memory_init(&fsp_memory_init_params);
	post_code(0x37);
	timestamp_add_now(TS_FSP_MEMORY_INIT_END);

	printk(BIOS_DEBUG, "FspMemoryInit returned 0x%08x\n", status);
	if (status != EFI_SUCCESS)
		die("ERROR - FspMemoryInit failed to initialize memory!\n");

	/* Locate the FSP reserved memory area */
	fsp_reserved_bytes = 0;
	fsp_memory = get_next_resource_hob(&fsp_reserved_guid, hob_list_ptr);
	if (fsp_memory == NULL) {
		fsp_verification_failure = 1;
		printk(BIOS_DEBUG,
			"7.2: FSP_RESERVED_MEMORY_RESOURCE_HOB missing!\n");
	} else {
		fsp_reserved_bytes = fsp_memory->ResourceLength;
		printk(BIOS_DEBUG, "Reserving 0x%016lx bytes for FSP\n",
			(unsigned long int)fsp_reserved_bytes);
	}

	/* Display SMM area */
#if IS_ENABLED(CONFIG_HAVE_SMI_HANDLER)
	char *smm_base;
	size_t smm_size;

	smm_region((void **)&smm_base, &smm_size);
	printk(BIOS_DEBUG, "0x%08x: smm_size\n", (unsigned int)smm_size);
	printk(BIOS_DEBUG, "0x%p: smm_base\n", smm_base);
#endif

	/* Migrate CAR data */
	printk(BIOS_DEBUG, "0x%p: cbmem_top\n", cbmem_top());
	if (pei_ptr->boot_mode != ACPI_S3) {
		cbmem_initialize_empty_id_size(CBMEM_ID_FSP_RESERVED_MEMORY,
			fsp_reserved_bytes);
	} else if (cbmem_initialize_id_size(CBMEM_ID_FSP_RESERVED_MEMORY,
		fsp_reserved_bytes)) {
#if IS_ENABLED(CONFIG_HAVE_ACPI_RESUME)
		printk(BIOS_DEBUG, "Failed to recover CBMEM in S3 resume.\n");
		/* Failed S3 resume, reset to come up cleanly */
		hard_reset();
#endif
	}

	/* Save the FSP runtime parameters. */
	fsp_set_runtime(fsp_header, hob_list_ptr);

	/* Lookup the FSP_BOOTLOADER_TOLUM_HOB */
	cbmem_root = get_next_resource_hob(&bootldr_tolum_guid, hob_list_ptr);
	if (cbmem_root == NULL) {
		fsp_verification_failure = 1;
		printk(BIOS_ERR, "7.4: FSP_BOOTLOADER_TOLUM_HOB missing!\n");
		printk(BIOS_ERR, "BootLoaderTolumSize: 0x%08x bytes\n",
			fsp_rt_common_buffer.BootLoaderTolumSize);
	}

	/* Locate the FSP_SMBIOS_MEMORY_INFO HOB */
	memory_info_hob = get_next_guid_hob(&memory_info_hob_guid,
		hob_list_ptr);
	if (memory_info_hob == NULL) {
		printk(BIOS_ERR, "FSP_SMBIOS_MEMORY_INFO HOB missing!\n");
		fsp_verification_failure = 1;
	} else {
		printk(BIOS_DEBUG,
			"FSP_SMBIOS_MEMORY_INFO HOB: 0x%p\n",
			memory_info_hob);
	}

#if IS_ENABLED(CONFIG_DISPLAY_HOBS)
	if (hob_list_ptr == NULL)
		die("ERROR - HOB pointer is NULL!\n");

	/*
	 * Verify that FSP is generating the required HOBs:
	 *	7.1: FSP_BOOTLOADER_TEMP_MEMORY_HOB only produced for FSP 1.0
	 *	7.2: FSP_RESERVED_MEMORY_RESOURCE_HOB verified above
	 *	7.3: FSP_NON_VOLATILE_STORAGE_HOB verified below
	 *	7.4: FSP_BOOTLOADER_TOLUM_HOB verified above
	 *	7.5: EFI_PEI_GRAPHICS_INFO_HOB produced by SiliconInit
	 *	FSP_SMBIOS_MEMORY_INFO HOB verified above
	 */
	if (cbmem_root != NULL) {
		printk(BIOS_DEBUG,
			"7.4: FSP_BOOTLOADER_TOLUM_HOB: 0x%p\n",
			cbmem_root);
		data = cbmem_root->PhysicalStart;
		printk(BIOS_DEBUG, "    0x%016lx: PhysicalStart\n", data);
		data = cbmem_root->ResourceLength;
		printk(BIOS_DEBUG, "    0x%016lx: ResourceLength\n", data);
	}
	hob_ptr.Raw = get_next_guid_hob(&mrc_guid, hob_list_ptr);
	if (hob_ptr.Raw == NULL) {
		printk(BIOS_ERR, "7.3: FSP_NON_VOLATILE_STORAGE_HOB missing!\n");
		fsp_verification_failure =
			(params->pei_data->saved_data == NULL) ? 1 : 0;
	} else {
		printk(BIOS_DEBUG,
			"7.3: FSP_NON_VOLATILE_STORAGE_HOB: 0x%p\n",
			hob_ptr.Raw);
	}
	if (fsp_memory != NULL) {
		printk(BIOS_DEBUG,
			"7.2: FSP_RESERVED_MEMORY_RESOURCE_HOB: 0x%p\n",
			fsp_memory);
		data = fsp_memory->PhysicalStart;
		printk(BIOS_DEBUG, "    0x%016lx: PhysicalStart\n", data);
		data = fsp_memory->ResourceLength;
		printk(BIOS_DEBUG, "    0x%016lx: ResourceLength\n", data);
	}

	/* Verify all the HOBs are present */
	if (fsp_verification_failure)
		printk(BIOS_DEBUG,
			"ERROR - Missing one or more required FSP HOBs!\n");

	/* Display the HOBs */
	print_hob_type_structure(0, hob_list_ptr);
#endif

	/* Get the address of the CBMEM region for the FSP reserved memory */
	fsp_reserved_memory_area = cbmem_find(CBMEM_ID_FSP_RESERVED_MEMORY);
	printk(BIOS_DEBUG, "0x%p: fsp_reserved_memory_area\n",
		fsp_reserved_memory_area);

	/* Verify the order of CBMEM root and FSP memory */
	if ((fsp_memory != NULL) && (cbmem_root != NULL) &&
		(cbmem_root->PhysicalStart <= fsp_memory->PhysicalStart)) {
		fsp_verification_failure = 1;
		printk(BIOS_DEBUG,
			"ERROR - FSP reserved memory above CBMEM root!\n");
	}

	/* Verify that the FSP memory was properly reserved */
	if ((fsp_memory != NULL) && ((fsp_reserved_memory_area == NULL) ||
		(fsp_memory->PhysicalStart !=
			(unsigned int)fsp_reserved_memory_area))) {
		fsp_verification_failure = 1;
		printk(BIOS_DEBUG, "ERROR - Reserving FSP memory area!\n");
#if IS_ENABLED(CONFIG_HAVE_SMI_HANDLER)
		if (cbmem_root != NULL) {
			size_t delta_bytes = (unsigned int)smm_base
				- cbmem_root->PhysicalStart
				- cbmem_root->ResourceLength;
			printk(BIOS_DEBUG,
				"0x%08x: Chipset reserved bytes reported by FSP\n",
				(unsigned int)delta_bytes);
			die("Please verify the chipset reserved size\n");
		}
#endif
	}

	/* Verify the FSP 1.1 HOB interface */
	if (fsp_verification_failure)
		die("ERROR - coreboot's requirements not met by FSP binary!\n");

	/* Display the memory configuration */
	report_memory_config();

	/* Locate the memory configuration data to speed up the next reboot */
	mrc_hob = get_next_guid_hob(&mrc_guid, hob_list_ptr);
	if (mrc_hob == NULL)
		printk(BIOS_DEBUG,
			"Memory Configuration Data Hob not present\n");
	else if (!vboot_recovery_mode_enabled()) {
		/* Do not save MRC data in recovery path */
		pei_ptr->data_to_save = GET_GUID_HOB_DATA(mrc_hob);
		pei_ptr->data_to_save_size = ALIGN(
			((u32)GET_HOB_LENGTH(mrc_hob)), 16);
	}
}

/* Initialize the UPD parameters for MemoryInit */
__weak void mainboard_memory_init_params(
	struct romstage_params *params,
	MEMORY_INIT_UPD *upd_ptr)
{
	printk(BIOS_DEBUG, "WEAK: %s/%s called\n", __FILE__, __func__);
}

/* Display the UPD parameters for MemoryInit */
__weak void soc_display_memory_init_params(
	const MEMORY_INIT_UPD *old, MEMORY_INIT_UPD *new)
{
	printk(BIOS_SPEW, "UPD values for MemoryInit:\n");
	hexdump32(BIOS_SPEW, new, sizeof(*new));
}

/* Initialize the UPD parameters for MemoryInit */
__weak void soc_memory_init_params(
	struct romstage_params *params,
	MEMORY_INIT_UPD *upd)
{
	printk(BIOS_DEBUG, "WEAK: %s/%s called\n", __FILE__, __func__);
}
