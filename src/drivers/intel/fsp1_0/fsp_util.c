/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2013-2014 Sage Electronic Engineering, LLC.
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

#include <types.h>
#include <string.h>
#include <console/console.h>
#include <bootstate.h>
#include <cbmem.h>
#include "fsp_util.h"
#include <lib.h> // hexdump
#include <ip_checksum.h>
#include <timestamp.h>

#ifndef __PRE_RAM__
/* Globals pointers for FSP structures */
void *FspHobListPtr = NULL;
FSP_INFO_HEADER *fsp_header_ptr = NULL;

void FspNotify (u32 Phase)
{
	FSP_NOTFY_PHASE        NotifyPhaseProc;
	NOTIFY_PHASE_PARAMS    NotifyPhaseParams;
	EFI_STATUS             Status;

	if (fsp_header_ptr == NULL) {
		fsp_header_ptr = (void *)find_fsp();
		if ((u32)fsp_header_ptr < 0xff) {
			post_code(0x4F); /* output something in case there is no serial */
			die("Can't find the FSP!\n");
		}
	}

	/* call FSP PEI to Notify PostPciEnumeration */
	NotifyPhaseProc = (FSP_NOTFY_PHASE)(fsp_header_ptr->ImageBase +
		fsp_header_ptr->NotifyPhaseEntry);
	NotifyPhaseParams.Phase = Phase;

	timestamp_add_now(Phase == EnumInitPhaseReadyToBoot ?
		TS_FSP_BEFORE_FINALIZE : TS_FSP_BEFORE_ENUMERATE);

	Status = NotifyPhaseProc (&NotifyPhaseParams);

	timestamp_add_now(Phase == EnumInitPhaseReadyToBoot ?
		TS_FSP_AFTER_FINALIZE : TS_FSP_AFTER_ENUMERATE);

	if (Status != 0)
		printk(BIOS_ERR,"FSP API NotifyPhase failed for phase 0x%x with status: 0x%x\n", Phase, Status);
}
#endif /* #ifndef __PRE_RAM__ */

#ifdef __PRE_RAM__

/*
 * Call the FSP to do memory init. The FSP doesn't return to this function.
 * The FSP returns to the romstage_main_continue().
 */
void __attribute__((noreturn)) fsp_early_init (FSP_INFO_HEADER *fsp_ptr)
{
	FSP_FSP_INIT FspInitApi;
	FSP_INIT_PARAMS FspInitParams;
	FSP_INIT_RT_BUFFER FspRtBuffer;
#if IS_ENABLED(CONFIG_FSP_USES_UPD)
	UPD_DATA_REGION fsp_upd_data;
#endif

	memset((void *)&FspRtBuffer, 0, sizeof(FSP_INIT_RT_BUFFER));
	FspRtBuffer.Common.StackTop = (u32 *)CONFIG_RAMTOP;
	FspInitParams.NvsBufferPtr = NULL;

#if IS_ENABLED(CONFIG_FSP_USES_UPD)
	FspRtBuffer.Common.UpdDataRgnPtr = &fsp_upd_data;
#endif
	FspInitParams.RtBufferPtr = (FSP_INIT_RT_BUFFER *)&FspRtBuffer;
	FspInitParams.ContinuationFunc = (CONTINUATION_PROC)ChipsetFspReturnPoint;
	FspInitApi = (FSP_FSP_INIT)(fsp_ptr->ImageBase + fsp_ptr->FspInitEntry);

	/* Call the chipset code to fill in the chipset specific structures */
	chipset_fsp_early_init(&FspInitParams, fsp_ptr);

	/* Call back to romstage for board specific changes */
	romstage_fsp_rt_buffer_callback(&FspRtBuffer);

	post_code(POST_FSP_MEMORY_INIT);
	FspInitApi(&FspInitParams);

	/* Should never return. Control will continue from ContinuationFunc */
	die("Uh Oh! FspInitApi returned");
}
#endif	/* __PRE_RAM__ */

volatile u8 *find_fsp()
{

#ifdef __PRE_RAM__
	volatile register u8 *fsp_ptr asm ("eax");

	/* Entry point for CAR assembly routine */
	__asm__ __volatile__ (
		".global find_fsp_bypass_prologue\n\t"
		"find_fsp_bypass_prologue:\n\t"
	);
#else
	volatile u8 *fsp_ptr;
#endif	/* __PRE_RAM__ */

	/* The FSP is stored in CBFS */
	fsp_ptr = (u8 *) CONFIG_FSP_LOC;

	/* Check the FV signature, _FVH */
	if (((EFI_FIRMWARE_VOLUME_HEADER *)fsp_ptr)->Signature == 0x4856465F) {
		/* Go to the end of the FV header and align the address. */
		fsp_ptr += ((EFI_FIRMWARE_VOLUME_HEADER *)fsp_ptr)->ExtHeaderOffset;
		fsp_ptr += ((EFI_FIRMWARE_VOLUME_EXT_HEADER *)fsp_ptr)->ExtHeaderSize;
		fsp_ptr = (u8 *)(((u32)fsp_ptr + 7) & 0xFFFFFFF8);
	} else {
		fsp_ptr = (u8*)ERROR_NO_FV_SIG;
	}

	/* Check the FFS GUID */
	if (((u32)fsp_ptr > 0xff) &&
		(((u32 *)&(((EFI_FFS_FILE_HEADER *)fsp_ptr)->Name))[0] == 0x912740BE) &&
		(((u32 *)&(((EFI_FFS_FILE_HEADER *)fsp_ptr)->Name))[1] == 0x47342284) &&
		(((u32 *)&(((EFI_FFS_FILE_HEADER *)fsp_ptr)->Name))[2] == 0xB08471B9) &&
		(((u32 *)&(((EFI_FFS_FILE_HEADER *)fsp_ptr)->Name))[3] == 0x0C3F3527)) {
		/* Add the FFS Header size to the base to find the Raw section Header */
		fsp_ptr += sizeof(EFI_FFS_FILE_HEADER);
	} else {
		fsp_ptr = (u8 *)ERROR_NO_FFS_GUID;
	}

	if (((u32)fsp_ptr > 0xff) &&
			((EFI_RAW_SECTION *)fsp_ptr)->Type == EFI_SECTION_RAW) {
		/* Add the Raw Header size to the base to find the FSP INFO Header */
		fsp_ptr += sizeof(EFI_RAW_SECTION);
	} else {
		fsp_ptr = (u8 *)ERROR_NO_INFO_HEADER;
	}

	/* Verify that the FSP is set to the base address we're expecting.*/
	if (((u32)fsp_ptr > 0xff) &&
			(*(u32*)(fsp_ptr + FSP_IMAGE_BASE_LOC) != CONFIG_FSP_LOC)) {
		fsp_ptr = (u8 *)ERROR_IMAGEBASE_MISMATCH;
	}

	/* Verify the FSP Signature */
	if (((u32)fsp_ptr > 0xff) &&
			(*(u32*)(fsp_ptr + FSP_IMAGE_SIG_LOC) != FSP_SIG)){
		fsp_ptr = (u8 *)ERROR_INFO_HEAD_SIG_MISMATCH;
	}

	/* Verify the FSP ID */
	if (((u32)fsp_ptr > 0xff) &&
		((*(u32 *)(fsp_ptr + FSP_IMAGE_ID_LOC) != FSP_IMAGE_ID_DWORD0) ||
		 (*(u32 *)(fsp_ptr + (FSP_IMAGE_ID_LOC + 4)) != FSP_IMAGE_ID_DWORD1))) {
		fsp_ptr = (u8 *)ERROR_FSP_SIG_MISMATCH;
	}

	return (fsp_ptr);
}

/** finds the saved temporary memory information in the FSP HOB list
 *
 * @param hob_list_ptr pointer to the start of the hob list
 * @return pointer to saved CAR MEM or NULL if not found.
 */
void *find_saved_temp_mem(void *hob_list_ptr)
{
	EFI_GUID temp_hob_guid = FSP_BOOTLOADER_TEMPORARY_MEMORY_HOB_GUID;
	EFI_HOB_GUID_TYPE *saved_mem_hob =
			(EFI_HOB_GUID_TYPE *) find_hob_by_guid(
			hob_list_ptr, &temp_hob_guid);

	if (saved_mem_hob == NULL)
		return NULL;

	return (void *) ((char *) saved_mem_hob + sizeof(EFI_HOB_GUID_TYPE));
}

#ifndef FSP_RESERVE_MEMORY_SIZE
/** @brief locates the HOB containing the location of the fsp reserved mem area
 *
 * @param hob_list_ptr pointer to the start of the hob list
 * @return pointer to the start of the FSP reserved memory or NULL if not found.
 */
void *find_fsp_reserved_mem(void *hob_list_ptr)
{
	EFI_GUID fsp_reserved_guid = FSP_HOB_RESOURCE_OWNER_FSP_GUID;
	EFI_HOB_RESOURCE_DESCRIPTOR *fsp_reserved_mem =
			(EFI_HOB_RESOURCE_DESCRIPTOR *) find_hob_by_guid(
			hob_list_ptr, &fsp_reserved_guid);

	if (fsp_reserved_mem == NULL)
		return NULL;

	return  (void *)((uintptr_t)fsp_reserved_mem->PhysicalStart);
}
#endif /* FSP_RESERVE_MEMORY_SIZE */

#ifndef __PRE_RAM__ /* Only parse HOB data in ramstage */

void print_fsp_info(void) {

	if (fsp_header_ptr == NULL)
		fsp_header_ptr = (void *)find_fsp();

	if ((u32)fsp_header_ptr < 0xff) {
		post_code(0x4F); /* post code in case there is no serial */
		die("Can't find the FSP!\n");
	}

	if (FspHobListPtr == NULL) {
		FspHobListPtr = (void *)*((u32 *)
				 cbmem_find(CBMEM_ID_HOB_POINTER));
	}

	printk(BIOS_SPEW,"fsp_header_ptr: %p\n", fsp_header_ptr);
	printk(BIOS_INFO,"FSP Header Version: %d\n", fsp_header_ptr->HeaderRevision);
	printk(BIOS_INFO,"FSP Revision: %d.%d\n",
			(u8)((fsp_header_ptr->ImageRevision >> 8) & 0xff),
			(u8)(fsp_header_ptr->ImageRevision  & 0xff));
}


#if IS_ENABLED(CONFIG_ENABLE_MRC_CACHE)
/**
 *  Save the FSP memory HOB (mrc data) to the MRC area in CBMEM
 */
int save_mrc_data(void *hob_start)
{
	u32 *mrc_hob;
	u32 *mrc_hob_data;
	u32 mrc_hob_size;
	struct mrc_data_container *mrc_data;
	int output_len;
	const EFI_GUID mrc_guid = FSP_NON_VOLATILE_STORAGE_HOB_GUID;

	mrc_hob = GetNextGuidHob(&mrc_guid, hob_start);
	if (mrc_hob == NULL){
		printk(BIOS_DEBUG, "Memory Configure Data Hob is not present\n");
		return(0);
	}

	mrc_hob_data = GET_GUID_HOB_DATA (mrc_hob);
	mrc_hob_size = (u32) GET_HOB_LENGTH(mrc_hob);

	printk(BIOS_DEBUG, "Memory Configure Data Hob at %p (size = 0x%x).\n",
			(void *)mrc_hob_data, mrc_hob_size);

	output_len = ALIGN(mrc_hob_size, 16);

	/* Save the MRC S3/fast boot/ADR restore data to cbmem */
	mrc_data = cbmem_add (CBMEM_ID_MRCDATA,
			output_len + sizeof(struct mrc_data_container));

	/* Just return if there was a problem with getting CBMEM */
	if (mrc_data == NULL) {
		printk(BIOS_WARNING, "CBMEM was not available to save the fast boot cache data.\n");
		return 0;
	}

	printk(BIOS_DEBUG, "Copy FSP MRC DATA to HOB (source addr %p, dest addr %p, %u bytes)\n",
			(void *)mrc_hob_data, mrc_data, output_len);

	mrc_data->mrc_signature = MRC_DATA_SIGNATURE;
	mrc_data->mrc_data_size = output_len;
	mrc_data->reserved = 0;
	memcpy(mrc_data->mrc_data, (const void *)mrc_hob_data, mrc_hob_size);

	/* Zero the unused space in aligned buffer. */
	if (output_len > mrc_hob_size)
		memset((mrc_data->mrc_data + mrc_hob_size), 0,
				output_len - mrc_hob_size);

	mrc_data->mrc_checksum = compute_ip_checksum(mrc_data->mrc_data,
			mrc_data->mrc_data_size);

	printk(BIOS_SPEW, "Fast boot data (includes align and checksum):\n");
	hexdump32(BIOS_SPEW, (void *)mrc_data->mrc_data, output_len / 4);
	return (1);
}
#endif /* CONFIG_ENABLE_MRC_CACHE */

static void find_fsp_hob_update_mrc(void *unused)
{
	/* Set the global HOB list pointer */
	FspHobListPtr = (void *)*((u32 *) cbmem_find(CBMEM_ID_HOB_POINTER));

	if (!FspHobListPtr){
		printk(BIOS_ERR, "ERROR: Could not find FSP HOB pointer in CBFS!\n");
	} else {
		/* 0x0000: Print all types */
		print_hob_type_structure(0x000, FspHobListPtr);

	#if IS_ENABLED(CONFIG_ENABLE_MRC_CACHE)
		if (save_mrc_data(FspHobListPtr))
			update_mrc_cache(NULL);
		else
			printk(BIOS_DEBUG,"Not updating MRC data in flash.\n");
	#endif
	}
}

/** @brief Notify FSP for PostPciEnumeration
 *
 * @param unused
 */
static void fsp_after_pci_enum(void *unused)
{
	/* This call needs to be done before resource allocation. */
	printk(BIOS_DEBUG, "FspNotify(EnumInitPhaseAfterPciEnumeration)\n");
	post_code(POST_FSP_NOTIFY_BEFORE_ENUMERATE);
	FspNotify(EnumInitPhaseAfterPciEnumeration);
	printk(BIOS_DEBUG,
	       "Returned from FspNotify(EnumInitPhaseAfterPciEnumeration)\n");
}

/** @brief Notify FSP for ReadyToBoot
 *
 * @param unused
 */
static void fsp_finalize(void *unused)
{
	printk(BIOS_DEBUG, "FspNotify(EnumInitPhaseReadyToBoot)\n");
	print_fsp_info();
	post_code(POST_FSP_NOTIFY_BEFORE_FINALIZE);
	FspNotify(EnumInitPhaseReadyToBoot);
	printk(BIOS_DEBUG, "Returned from FspNotify(EnumInitPhaseReadyToBoot)\n");
}

/* Set up for the ramstage FSP calls */
BOOT_STATE_INIT_ENTRY(BS_DEV_ENUMERATE, BS_ON_EXIT, fsp_after_pci_enum, NULL);
BOOT_STATE_INIT_ENTRY(BS_PAYLOAD_BOOT, BS_ON_ENTRY, fsp_finalize, NULL);

/* Update the MRC/fast boot cache as part of the late table writing stage */
BOOT_STATE_INIT_ENTRY(BS_WRITE_TABLES, BS_ON_ENTRY,
			find_fsp_hob_update_mrc, NULL);
#endif	/* #ifndef __PRE_RAM__ */
