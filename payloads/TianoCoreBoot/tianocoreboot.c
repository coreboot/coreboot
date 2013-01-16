/*
 * This file is part of the TianoCoreBoot project.
 *
 * Copyright (C) 2013 Google Inc.
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

#include <libpayload.h>
#include <endian.h>
#include <cbfs.h>
#include <efi.h>

static void print_guid(EFI_GUID *guid)
{
	printf("%08x-%04x-%04x-%02x%02x-%02x%02x%02x%02x%02x%02x",
		guid->Data1, guid->Data2, guid->Data3,
		guid->Data4[0], guid->Data4[1], guid->Data4[2], guid->Data4[3],
		guid->Data4[4], guid->Data4[5], guid->Data4[6], guid->Data4[7]);
}

static void dump_uefi_firmware_volume_header(EFI_FIRMWARE_VOLUME_HEADER *fvh)
{
	printf("Found UEFI firmware volume.\n");
	printf("  GUID: ");
	print_guid(&(fvh->FileSystemGuid));
	printf("\n");
	printf("  length: 0x%016llx\n", fvh->FvLength);
	printf("  signature: 0x%08x\n", fvh->Signature);
	printf("  attributes: 0x%08x\n", fvh->Attributes);
	printf("  header length: 0x%04x\n", fvh->HeaderLength);
	printf("  checksum: 0x%04x\n", fvh->Checksum);
	printf("  revision: 0x%02x\n", fvh->Revision);
	printf("  block map:\n");
	int i = 0;
	EFI_FV_BLOCK_MAP_ENTRY *fbm = &(fvh->FvBlockMap[0]);
	while (fbm[i].NumBlocks || fbm[i].BlockLength) {
		printf("    %2d. numblocks = 0x%08x length = 0x%08x\n",
			i+1, fbm[i].NumBlocks, fbm[i].BlockLength);
		i++;
	}
	printf("\n");
}

static void dump_uefi_ffs_file_header(EFI_FFS_FILE_HEADER *file)
{
	int size;

#ifdef VERBOSE
	printf("Found FFS file:\n  GUID: ");
#endif
	print_guid(&(file->Name));
#ifdef VERBOSE
	printf("\n  integrity check: %02x %02x\n",
		file->IntegrityCheck.Checksum.Header,
		file->IntegrityCheck.Checksum.File);
	printf("  file type: ");
#else
	printf(" ");
#endif
	switch (file->Type) {
	case EFI_FV_FILETYPE_RAW: printf("raw"); break;
	case EFI_FV_FILETYPE_FREEFORM: printf("free form"); break;
	case EFI_FV_FILETYPE_SECURITY_CORE: printf("security core"); break;
	case EFI_FV_FILETYPE_PEI_CORE: printf("PEIM core"); break;
	case EFI_FV_FILETYPE_DXE_CORE: printf("DXE core"); break;
	case EFI_FV_FILETYPE_PEIM: printf("PEIM"); break;
	case EFI_FV_FILETYPE_DRIVER: printf("driver"); break;
	case EFI_FV_FILETYPE_COMBINED_PEIM_DRIVER: printf("combined PEIM driver"); break;
	case EFI_FV_FILETYPE_APPLICATION: printf("application"); break;
	case EFI_FV_FILETYPE_FIRMWARE_VOLUME_IMAGE: printf("firmware volume image"); break;
	case EFI_FV_FILETYPE_FFS_PAD: printf("FFS pad"); break;
	default: printf("unknown");
	}
#ifdef VERBOSE
	printf("\n");
	printf("  attributes: 0x%02x\n", file->Attributes);
#endif
	size = file->Size[0] | (file->Size[1] << 8) | (file->Size[2] << 16);
#ifdef VERBOSE
	printf("  size: 0x%06x\n", size);
	printf("  state: 0x%02x\n", file->State);
#else
	printf(" (%d bytes)\n", size);
#endif
}

#if 0
EFI_HOB_HANDOFF_INFO_TABLE HandoffInformationTable = {
	{ EFI_HOB_TYPE_HANDOFF, sizeof(EFI_HOB_HANDOFF_INFO_TABLE), 0 },
	EFI_HOB_HANDOFF_TABLE_VERSION,
	BOOT_WITH_FULL_CONFIGURATION,
	0 /* EfiMemoryTop */,
	0 /* EfiMemoryBottom */,
	0 /* EfiFreeMemoryTop */,
	0 /* EfiFreeMemoryBottom */,
	0 /* EfiEndOfHobList */
};

EFI_HOB_FIRMWARE_VOLUME FirmwareVolume = {
	{ EFI_HOB_TYPE_FV, sizeof(EFI_HOB_FIRMWARE_VOLUME), 0 },
	0 /* BaseAddress */,
	0 /* Length */
};

/* 1..n */
EFI_HOB_RESOURCE_DESCRIPTOR ResourceDescriptor = {
	{ EFI_HOB_TYPE_RESOURCE_DESCRIPTOR, sizeof(EFI_HOB_RESOURCE_DESCRIPTOR), 0 },
	/* */, // owner EFI_GUID
	EFI_RESOURCE_SYSTEM_MEMORY,
	EFI_RESOURCE_ATTRIBUTE_PRESENT | EFI_RESOURCE_ATTRIBUTE_INITIALIZED | EFI_RESOURCE_ATTRIBUTE_TESTED,
	0, /* PhysicalStart */
	0 /* ResourceLength */
};

EFI_HOB_MEMORY_ALLOCATION_MODULE MemoryAllocationModule = {
	{ EFI_HOB_TYPE_MEMORY_ALLOCATION, sizeof(EFI_HOB_MEMORY_ALLOCATION_MODULE), 0 },
	{ GUID, MemoryBaseAddress, MemoryLength, EfiConventionalMemory,  {0,0,0,0} },
	GUID ModuleName,
        ADDR EntryPoint
};

EFI_HOB_CPU Cpu = {
	{ EFI_HOB_TYPE_CPU, sizeof(EFI_HOB_CPU), 0 },
	32, /* SizeOfMemorySpace, u8 */
	16, /* SizeOfIoSpace */
	{ 0,0,0,0,0,0 }
};

EFI_HOB_GENERIC_HEADER End =
	{ EFI_HOB_TYPE_END_OF_HOB_LIST, sizeof(EFI_HOB_GENERIC_HEADER), 0 };

#endif

int main(void)
{
	int i;
	struct cbfs_file *file;
	void *tiano;

	printf("\nTiano Core Loader v1.0\n");
	printf("Copyright (C) 2013 Google Inc. All rights reserved.\n\n");

	/* Find UEFI firmware volume in CBFS */
	file = cbfs_find("fallback/tianocore.fd");
	if (!file) {
		printf("Could not find fallback/tianocore.fd in CBFS.\n");
		halt();
	}

	tiano = CBFS_SUBHEADER(file);
	while (tiano < (void *)CBFS_SUBHEADER(file) + ntohl(file->len)) {


	/* Verify UEFI firmware volume consistency */
	EFI_FIRMWARE_VOLUME_HEADER *fvh = (EFI_FIRMWARE_VOLUME_HEADER *)tiano;
	if (fvh->Signature != 0x4856465f) {
		printf("Not an UEFI firmware volume.\n");
		halt();
	}

	/* Dump UEFI firmware volume header */
	dump_uefi_firmware_volume_header(fvh);

	/* Dump UEFI firmware file headers */
	for (i = fvh->HeaderLength; i < fvh->FvLength;) {
		int size;
		EFI_FFS_FILE_HEADER *ffs;

		ffs = (EFI_FFS_FILE_HEADER *)(tiano + i);

		size = ffs->Size[0] | (ffs->Size[1] << 8) | (ffs->Size[2] << 16);
		if (size == 0xffffff)
			break;
		printf("%08x   ", i);
		dump_uefi_ffs_file_header(ffs);
		i = ALIGN(i + size, 8);
	}

	tiano += fvh->FvLength;
	printf("\n\n");
	}
#if 0
	void (*entry)(void);
	entry = tiano;
	entry();
#endif

	printf("The end.\n");
	halt();

	return 0;
}

PAYLOAD_INFO(name, "TianoCoreBoot");
PAYLOAD_INFO(listname, "Tiano Core");
PAYLOAD_INFO(desc, "Tiano Core Loader");
