/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2013 Sage Electronic Engineering, LLC.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
 */

#include <types.h>
#include <string.h>
#include <cpu/x86/stack.h>
#include <console/console.h>
#include <lib.h>
#include "fsp_util.h"

#if CONFIG_ENABLE_FAST_BOOT
#include <device/device.h>
#include "northbridge.h"
#include <cbmem.h>
#include <ip_checksum.h>
#endif

void find_fsp (void);

#ifndef __PRE_RAM__
/* Globals pointers for FSP structures */
void *FspHobListPtr;
FSP_INFO_HEADER *fsp_header_ptr;

void FspNotify (u32 Phase)
{
	FSP_NOTFY_PHASE        NotifyPhaseProc;
	NOTIFY_PHASE_PARAMS    NotifyPhaseParams;
	EFI_STATUS             Status;

	if (fsp_header_ptr == NULL) {
		find_fsp();
		if (fsp_header_ptr == NULL) {
			post_code(0x4F); /* output something in case there is no serial */
			die("Can't find the FSP!\n");
		}
	}

	/* call FSP PEI to Notify PostPciEnumeration */
	NotifyPhaseProc = (FSP_NOTFY_PHASE)(fsp_header_ptr->ImageBase + fsp_header_ptr->NotifyPhaseEntry);
	NotifyPhaseParams.Phase = Phase;
	Status = NotifyPhaseProc (&NotifyPhaseParams);
	if (Status != 0)
		printk(BIOS_ERR,"FSP API NotifyPhase failed for phase 0x%x with status: 0x%x\n", Phase, Status);
}
#endif

#ifdef __PRE_RAM__
/*
 *
 * Call the FSP to do memory init. The FSP doesn't return to this function.
 * The FSP returns to the romstage_main_continue().
 *
 */
const PLATFORM_CONFIG DefaultPlatformConfig = {
	TRUE,  // Hyperthreading
	FALSE, // Turbo Mode
	FALSE,  // Memory Down
#if CONFIG_ENABLE_FAST_BOOT
	TRUE,  // Fast Boot
#else
	FALSE, // Fast Boot
#endif
};

void __attribute__ ((noreturn)) fsp_early_init (FSP_INFO_HEADER *fsp_ptr)
{
	FSP_FSP_INIT FspInitApi;
	FSP_INIT_PARAMS FspInitParams;
	FSP_INIT_RT_BUFFER FspRtBuffer;
	MEM_CONFIG MemoryConfig;
	memset((void*)&MemoryConfig, 0, sizeof(MEM_CONFIG));

	memset((void*)&FspRtBuffer, 0, sizeof(FSP_INIT_RT_BUFFER));
	FspRtBuffer.Common.StackTop = (u32 *)ROMSTAGE_STACK;
	FspRtBuffer.Platform.MemoryConfig = &MemoryConfig;
	FspRtBuffer.PlatformConfiguration.PlatformConfig = &DefaultPlatformConfig;
	FspInitParams.NvsBufferPtr = NULL;

#if CONFIG_ENABLE_FAST_BOOT
	/* find_current_mrc_cache */
	struct mrc_data_container *mrc_cache = NULL;
	if (((mrc_cache = find_current_mrc_cache()) == NULL) || (mrc_cache->mrc_data_size == -1UL)) {
		printk(BIOS_DEBUG, "FSP MRC cache not present.\n");
		FspInitParams.NvsBufferPtr = (void *) NULL;
	} else {
		printk(BIOS_DEBUG, "FSP MRC cache present at %x.\n", (u32)mrc_cache);
		printk(BIOS_SPEW, "Saved MRC data:\n");
		hexdump32(BIOS_SPEW, (void *)mrc_cache->mrc_data, (mrc_cache->mrc_data_size / 4));
		FspInitParams.NvsBufferPtr = (void *) (EFI_HOB_GUID_TYPE *) mrc_cache->mrc_data;
	}
#endif

	/* Not sure why passing BootMode is required - the only valid value is 0 */
	FspRtBuffer.Common.BootMode = 0;
	FspInitParams.RtBufferPtr = (FSP_INIT_RT_BUFFER *)&FspRtBuffer;
	FspInitParams.ContinuationFunc = (CONTINUATION_PROC)romstage_main_continue;
	FspInitApi = (FSP_FSP_INIT)(fsp_ptr->ImageBase + fsp_ptr->FspInitEntry);

	/* call back to romstage.c here to allow structures to be changed */
	romstage_fsp_rt_buffer_callback(&FspRtBuffer);

	FspInitApi(&FspInitParams);

	/* Should never return. Control will continue from ContinuationFunc */
	die("Uh Oh! FspInitApi returned");
}
#endif

void __attribute__((optimize("O0"))) find_fsp ()
{

#ifdef __PRE_RAM__
	volatile register u8 *fsp_ptr asm ("eax");

	/* Entry point for CAR assembly routine */
	__asm__ __volatile__ (
		".global find_fsp\n\t"
		"find_fsp:\n\t"
	);
#else
	u8 *fsp_ptr;
#endif

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

#if 0	/* removed for debugging */
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
			(*(u32 *)(fsp_ptr + FSP_IMAGE_ID_LOC) != CONFIG_FSP_IMAGE_ID_DWORD0)){
		fsp_ptr = (u8 *)ERROR_FSP_SIG_MISMATCH;
	}
	if (((u32)fsp_ptr > 0xff) &&
			(*(u32 *)(fsp_ptr + FSP_IMAGE_ID_LOC + 4) != CONFIG_FSP_IMAGE_ID_DWORD1)) {
		fsp_ptr = (u8 *)ERROR_FSP_SIG_MISMATCH;
	}
#endif /* removed for debugging */

#ifdef __PRE_RAM__
	__asm__ __volatile__ ("ret");
#else
	fsp_header_ptr = (FSP_INFO_HEADER *) fsp_ptr;
	printk(BIOS_SPEW,"fsp_header_ptr: 0x%x\n", (u32)fsp_header_ptr);
	return;
#endif
}

void print_fsp_info(void) {

#ifndef __PRE_RAM__
	if (fsp_header_ptr == NULL)
		find_fsp();

	printk(BIOS_INFO,"FSP Header Version: %d\n", fsp_header_ptr->HeaderRevision);
	printk(BIOS_INFO,"FSP Revision: %d.%d\n",
			(u8)((fsp_header_ptr->ImageRevision >> 8) & 0xff),
			(u8)(fsp_header_ptr->ImageRevision  & 0xff));
#endif
}

#ifndef __PRE_RAM__ /* Only parse HOB data in ramstage */
static void print_hob_mem_attributes(void *Hobptr) {
	EFI_HOB_MEMORY_ALLOCATION *HobMemoryPtr = (EFI_HOB_MEMORY_ALLOCATION *)Hobptr;
	EFI_MEMORY_TYPE Hobmemtype = HobMemoryPtr->AllocDescriptor.MemoryType;
	u64 Hobmemaddr = HobMemoryPtr->AllocDescriptor.MemoryBaseAddress;
	u64 Hobmemlength = HobMemoryPtr->AllocDescriptor.MemoryLength;
	const char * Hobmemtypenames[15];

	Hobmemtypenames[0] = "EfiReservedMemoryType";
	Hobmemtypenames[1] = "EfiLoaderCode";
	Hobmemtypenames[2] = "EfiLoaderData";
	Hobmemtypenames[3] = "EfiBootServicesCode";
	Hobmemtypenames[4] = "EfiBootServicesData";
	Hobmemtypenames[5] = "EfiRuntimeServicesCode";
	Hobmemtypenames[6] = "EfiRuntimeServicesData";
	Hobmemtypenames[7] = "EfiConventionalMemory";
	Hobmemtypenames[8] = "EfiUnusableMemory";
	Hobmemtypenames[9] = "EfiACPIReclaimMemory";
	Hobmemtypenames[10] = "EfiACPIMemoryNVS";
	Hobmemtypenames[11] = "EfiMemoryMappedIO";
	Hobmemtypenames[12] = "EfiMemoryMappedIOPortSpace";
	Hobmemtypenames[13] = "EfiPalCode";
	Hobmemtypenames[14] = "EfiMaxMemoryType";

	printk(BIOS_SPEW, "  Memory type %s (0x%x)\n",
			Hobmemtypenames[(u32)Hobmemtype], (u32) Hobmemtype);
	printk(BIOS_SPEW, "  at location 0x%0lx with length 0x%0lx\n",
			(long unsigned int)Hobmemaddr, (long unsigned int)Hobmemlength);
}

static void print_hob_resource_attributes(void *Hobptr) {
	EFI_HOB_RESOURCE_DESCRIPTOR *HobResourcePtr = (EFI_HOB_RESOURCE_DESCRIPTOR *)Hobptr;
	u32 Hobrestype   = HobResourcePtr->ResourceType;
	u32 Hobresattr   = HobResourcePtr->ResourceAttribute;
	u64 Hobresaddr   = HobResourcePtr->PhysicalStart;
	u64 Hobreslength = HobResourcePtr->ResourceLength;
	const char *Hobrestypestr = NULL;

	// HOB Resource Types
	switch (Hobrestype) {
	case EFI_RESOURCE_SYSTEM_MEMORY:
		Hobrestypestr = "EFI_RESOURCE_SYSTEM_MEMORY"; break;
	case EFI_RESOURCE_MEMORY_MAPPED_IO:
		Hobrestypestr = "EFI_RESOURCE_MEMORY_MAPPED_IO"; break;
	case EFI_RESOURCE_IO:
		Hobrestypestr = "EFI_RESOURCE_IO"; break;
	case EFI_RESOURCE_FIRMWARE_DEVICE:
		Hobrestypestr = "EFI_RESOURCE_FIRMWARE_DEVICE"; break;
	case EFI_RESOURCE_MEMORY_MAPPED_IO_PORT:
		Hobrestypestr = "EFI_RESOURCE_MEMORY_MAPPED_IO_PORT"; break;
	case EFI_RESOURCE_MEMORY_RESERVED:
		Hobrestypestr = "EFI_RESOURCE_MEMORY_RESERVED"; break;
	case EFI_RESOURCE_IO_RESERVED:
		Hobrestypestr = "EFI_RESOURCE_IO_RESERVED"; break;
	case EFI_RESOURCE_MAX_MEMORY_TYPE:
		Hobrestypestr = "EFI_RESOURCE_MAX_MEMORY_TYPE"; break;
	default:
		Hobrestypestr = "EFI_RESOURCE_UNKNOWN"; break;
	}

	printk(BIOS_SPEW, "  Resource %s (0x%0x) has attributes 0x%0x\n",
			Hobrestypestr, Hobrestype, Hobresattr);
	printk(BIOS_SPEW, "  at location 0x%0lx with length 0x%0lx\n",
			(long unsigned int)Hobresaddr, (long unsigned int)Hobreslength);
}

static const char * get_hob_type_string(void *Hobptr) {
	EFI_HOB_GENERIC_HEADER *HobHeaderPtr = (EFI_HOB_GENERIC_HEADER *)Hobptr;
	u16 Hobtype = HobHeaderPtr->HobType;
	const char *Hobtypestring = NULL;

	switch (Hobtype) {
	case EFI_HOB_TYPE_HANDOFF:
		Hobtypestring = "EFI_HOB_TYPE_HANDOFF"; break;
	case EFI_HOB_TYPE_MEMORY_ALLOCATION:
		Hobtypestring = "EFI_HOB_TYPE_MEMORY_ALLOCATION"; break;
	case EFI_HOB_TYPE_RESOURCE_DESCRIPTOR:
		Hobtypestring = "EFI_HOB_TYPE_RESOURCE_DESCRIPTOR"; break;
	case EFI_HOB_TYPE_GUID_EXTENSION:
		Hobtypestring = "EFI_HOB_TYPE_GUID_EXTENSION"; break;
	case EFI_HOB_TYPE_MEMORY_POOL:
		Hobtypestring = "EFI_HOB_TYPE_MEMORY_POOL"; break;
	case EFI_HOB_TYPE_UNUSED:
		Hobtypestring = "EFI_HOB_TYPE_UNUSED"; break;
	case EFI_HOB_TYPE_END_OF_HOB_LIST:
		Hobtypestring = "EFI_HOB_TYPE_END_OF_HOB_LIST"; break;
	default:
		Hobtypestring = "EFI_HOB_TYPE_UNRECOGNIZED"; break;
	}

	return Hobtypestring;
}

/* Print out a structure of all the HOBs
 * that match a certain type:
 * Print all types			(0x0000)
 * EFI_HOB_TYPE_HANDOFF		(0x0001)
 * EFI_HOB_TYPE_MEMORY_ALLOCATION	(0x0002)
 * EFI_HOB_TYPE_RESOURCE_DESCRIPTOR	(0x0003)
 * EFI_HOB_TYPE_GUID_EXTENSION		(0x0004)
 * EFI_HOB_TYPE_MEMORY_POOL		(0x0007)
 * EFI_HOB_TYPE_UNUSED			(0xFFFE)
 * EFI_HOB_TYPE_END_OF_HOB_LIST	(0xFFFF)
 */
void print_hob_type_structure(u16 Hobtype, void *Hoblistptr) {
	u32 *Currenthob;
	u32 *Nexthob = 0;
	u8  Lasthob = 0;
	u32 Currenttype;
	const char *Currenttypestr;

	Currenthob = Hoblistptr;

	/* Print out HOBs of our desired type until
	 * the end of the HOB list
	 */
	printk(BIOS_DEBUG, "\n=== FSP HOB Data Structure ===\n");
	printk(BIOS_DEBUG, "FSP Hoblistptr: 0x%0x\n",
			(u32) Hoblistptr);
	do {
		EFI_HOB_GENERIC_HEADER *CurrentHeaderPtr = (EFI_HOB_GENERIC_HEADER *)Currenthob;
		Currenttype = CurrentHeaderPtr->HobType;  // Get the type of this HOB
		Currenttypestr = get_hob_type_string(Currenthob);

		if (Currenttype == Hobtype || Hobtype == 0x0000) {
			printk(BIOS_DEBUG, "HOB 0x%0x is an %s (type 0x%0x)\n",
					(u32) Currenthob, Currenttypestr, Currenttype);
			switch (Currenttype) {
			case EFI_HOB_TYPE_MEMORY_ALLOCATION:
				print_hob_mem_attributes(Currenthob); break;
			case EFI_HOB_TYPE_RESOURCE_DESCRIPTOR:
				print_hob_resource_attributes(Currenthob); break;
			}
		}

		Lasthob = END_OF_HOB_LIST(Currenthob);	// Check for end of HOB list
		if (!Lasthob) {
			Nexthob = GET_NEXT_HOB(Currenthob);	// Get next HOB pointer
			Currenthob = Nexthob;	// Start on next HOB
		}
	} while (!Lasthob);
	printk(BIOS_DEBUG, "=== End of FSP HOB Data Structure ===\n\n");
}

#if CONFIG_ENABLE_FAST_BOOT
/**
 *  Save the FSP memory HOB (mrc data) to the MRC area in CBMEM
 */
void save_mrc_data(void *hob_start)
{
	u32 *mrc_hob;
	u32 *mrc_hob_data;
	u32 mrc_hob_size;
	struct mrc_data_container *mrc_data;
	int output_len;
	const EFI_GUID mrc_guid = FSP_NON_VOLATILE_STORAGE_HOB_GUID;

	mrc_hob = GetNextGuidHob(&mrc_guid, hob_start);
	if (mrc_hob == NULL) die ("mrc_hob is NULL");

	mrc_hob_data = GET_GUID_HOB_DATA (mrc_hob);
	mrc_hob_size = (u32) GET_HOB_LENGTH(mrc_hob);

	printk(BIOS_DEBUG, "Memory Configure Data Hob at %p (size = 0x%x).\n",
			(void *)mrc_hob_data, mrc_hob_size);

	output_len = ALIGN(mrc_hob_size, 16);

	/* Save the MRC S3/Fastboot/ADR restore data to cbmem */
	mrc_data = cbmem_add (CBMEM_ID_MRCDATA,
			output_len + sizeof(struct mrc_data_container));

	/* Just return if there was a problem with getting CBMEM */
	if (mrc_data == NULL) return;

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

	printk(BIOS_SPEW, "MRC data in CBMEM(includes align and checksum):\n");
	hexdump32(BIOS_SPEW, (void *)mrc_data->mrc_data, (output_len / 4));
}
#endif
#endif	/*__PRE_RAM__*/
