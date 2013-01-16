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
#include <coff.h>

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
#ifdef VERBOSE
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
#endif
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


void *load_dxe_core(void *pe, void *target)
{
	dos_header_t *dos_hdr = (dos_header_t *)pe;

	printf("Loading DXE core at %p\n", pe);

	if (*(uint16_t *)pe != 0x5a4d) {
		printf("DXE core not a PE binary.\n");
		return NULL;
	}

#ifdef VERBOSE
	printf("e_lfanew = 0x%08x\n", dos_hdr->e_lfanew);
#endif

	coff_header_t *coff_hdr = (coff_header_t *)(pe + dos_hdr->e_lfanew);
#ifdef VERBOSE
	printf("Machine: %x\n", coff_hdr->Machine);
	printf("NumberOfSections: %x\n", coff_hdr->NumberOfSections);
	printf("TimeDateStamp: %x\n", coff_hdr->TimeDateStamp);
	printf("PointerToSymbolTable: %x\n", coff_hdr->PointerToSymbolTable);
	printf("NumberOfSymbols: %x\n", coff_hdr->NumberOfSymbols);
	printf("SizeOfOptionalHeader: %x\n", coff_hdr->SizeOfOptionalHeader);
	printf("Characteristics: %x\n", coff_hdr->Characteristics);
#endif
	if (coff_hdr->Machine != 0x14c) {
		printf("Only x86 supported right now.\n");
		return NULL;
	}

	/* Right after the coff header */
	pe_opt_header_t *pe_hdr = (pe_opt_header_t *)(&coff_hdr[1]);
	if (pe_hdr->signature != 267) {
		printf("No valid PE opt header\n");
		return NULL;
	}

#ifdef VERBOSE
	printf("\n");
	printf("MajorLinkerVersion: %x\n", pe_hdr->MajorLinkerVersion);
	printf("MinorLinkerVersion: %x\n", pe_hdr->MinorLinkerVersion);
	printf("SizeOfCode: %x\n", pe_hdr->SizeOfCode);
	printf("SizeOfInitializedData: %x\n", pe_hdr->SizeOfInitializedData);
	printf("SizeOfUninitializedData: %x\n", pe_hdr->SizeOfUninitializedData);
	printf("AddressOfEntryPoint: %x\n", pe_hdr->AddressOfEntryPoint);
	printf("BaseOfCode: %x\n", pe_hdr->BaseOfCode);
	printf("BaseOfData: %x\n", pe_hdr->BaseOfData);
	printf("ImageBase: %x\n", pe_hdr->ImageBase);
	printf("SectionAlignment: %x\n", pe_hdr->SectionAlignment);
	printf("FileAlignment: %x\n", pe_hdr->FileAlignment);
	printf("MajorOSVersion: %x\n", pe_hdr->MajorOSVersion);
	printf("MinorOSVersion: %x\n", pe_hdr->MinorOSVersion);
	printf("MajorImageVersion: %x\n", pe_hdr->MajorImageVersion);
	printf("MinorImageVersion: %x\n", pe_hdr->MinorImageVersion);
	printf("MajorSubsystemVersion: %x\n", pe_hdr->MajorSubsystemVersion);
	printf("MinorSubsystemVersion: %x\n", pe_hdr->MinorSubsystemVersion);
	printf("Reserved: %x\n", pe_hdr->Reserved);
	printf("SizeOfImage: %x\n", pe_hdr->SizeOfImage);
	printf("SizeOfHeaders: %x\n", pe_hdr->SizeOfHeaders);
	printf("Checksum: %x\n", pe_hdr->Checksum);
	printf("Subsystem: %x\n", pe_hdr->Subsystem);
	printf("DLLCharacteristics: %x\n", pe_hdr->DLLCharacteristics);
	printf("SizeOfStackReserve: %x\n", pe_hdr->SizeOfStackReserve);
	printf("SizeOfStackCommit: %x\n", pe_hdr->SizeOfStackCommit);
	printf("SizeOfHeapReserve: %x\n", pe_hdr->SizeOfHeapReserve);
	printf("SizeOfHeapCommit: %x\n", pe_hdr->SizeOfHeapCommit);
	printf("LoaderFlags: %x\n", pe_hdr->LoaderFlags);
	printf("NumberOfRvaAndSizes: %x\n", pe_hdr->NumberOfRvaAndSizes);
#endif

	if(pe_hdr->Subsystem != 0xb) {
		printf("Not an EFI binary.\n");
		return NULL;
	}

	int i;
#ifdef VERBOSE
	for (i = 0; i < pe_hdr->NumberOfRvaAndSizes; i++) {
		if (!pe_hdr->DataDirectory[i].Size)
			continue;
		printf("Data Directory %d\n", i+1);
		printf("  VirtualAddress %x\n", pe_hdr->DataDirectory[i].VirtualAddress);
		printf("  Size %x\n", pe_hdr->DataDirectory[i].Size);
	}
#endif

	pe_section_t *sections = (pe_section_t *)(&pe_hdr[1]);

	for (i = 0; i < coff_hdr->NumberOfSections; i++) {
		int j;
		printf("section %d: ", i);
		for (j = 0; j < 8; j++)
			printf("%c", sections[i].SectionName[j] ? sections[i].SectionName[j] : ' ');

		printf("  size=%08x rva=%08x in file=%08x/%08x flags=%08x\n",
			sections[i].Size, sections[i].RVA, sections[i].PhysicalSizeOnDisk,
				sections[i].PhysicalLocationOnDisk, sections[i].SectionFlags);

		if (!strncmp((char *)sections[i].SectionName, ".text", 6)) {
			// .text section
			//  size=157a0 rva=240 size on disk=157a0 location on disk=240 flags=60000020
			memcpy(target + sections[i].RVA, pe + sections[i].PhysicalLocationOnDisk,
				sections[i].PhysicalSizeOnDisk);
		} else
		if (!strncmp((char *)sections[i].SectionName, ".data", 6)) {
			// .data section
			//  size=6820 rva=159e0 size on disk=6820 location on disk=159e0 flags=c0000040
			memcpy(target + sections[i].RVA, pe + sections[i].PhysicalLocationOnDisk,
				sections[i].PhysicalSizeOnDisk);
		} else
		if (!strncmp((char *)sections[i].SectionName, ".reloc", 7)) {
			// .reloc section
			// section 2: .reloc
			//   size=1080 rva=1c200 size on disk=1080 location on disk=1c200 flags=42000040
			relocation_t *reloc = (relocation_t *)
				(pe + sections[i].PhysicalLocationOnDisk);
			while (reloc && reloc->SizeOfBlock) {
				printf("Relocation Block Virtual %08x Size %08x\n",
					 reloc->VirtualAddress, reloc->SizeOfBlock);
				for (i = sizeof(relocation_t); i < reloc->SizeOfBlock; i+= 2) {
					uint16_t r = *(uint16_t *)((void *)reloc + i);
					switch (r>>12) {
					case 3:
						printf("  HIGHLOW  %08x\n",
							reloc->VirtualAddress + (r & 0xfff));
						break;
					case 0:
						printf("  ABSOLUTE %08x\n", r & 0xfff);
						break;
					default:
						printf("Unknown relocation type %x\n", r);
						return NULL;
					}
				}

				reloc = (relocation_t *)(((void *)reloc) + reloc->SizeOfBlock);
			}
		} else
		if (!strncmp((char *)sections[i].SectionName, ".debug", 7)) {
			// debug section, ignored.
		} else {
			printf("section type ");
			for (j = 0; j < 8; j++)
				if (sections[i].SectionName[j])
					printf("%c", sections[i].SectionName[j]);
			printf(" unknown. ignored.\n");
		}
	}

	return (target + pe_hdr->AddressOfEntryPoint);
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
	EFI_COMMON_SECTION_HEADER *dxecore = NULL;

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
			printf("%08x - ", i);
			dump_uefi_ffs_file_header(ffs);

			if (ffs->Type == EFI_FV_FILETYPE_DXE_CORE) {
				dxecore = (EFI_COMMON_SECTION_HEADER *)&ffs[1];
				// break;
			}

			i = ALIGN(i + size, 8);
		}

		tiano += fvh->FvLength;
		printf("\n");
	}

	if (!dxecore) {
		printf("No DXE core found.\n");
		halt();
	}

	printf("Found DXE core at %p\n", dxecore);
	int size = dxecore->Size[0] | (dxecore->Size[1] << 8) | (dxecore->Size[2] << 16);
	printf("  size = %d, type = %x\n", size, dxecore->Type);

	void *pe = (void *)&dxecore[1];

	void (*entry)(void);
	entry = load_dxe_core(pe, (void *)0x200000);

	if (!entry) {
		printf("Could not load DXE code.\n");
		halt();
	}

	printf("Jumping to DXE core at %p\n", entry);
	entry();

	printf("The end.\n");
	halt();

	return 0;
}

PAYLOAD_INFO(name, "TianoCoreBoot");
PAYLOAD_INFO(listname, "Tiano Core");
PAYLOAD_INFO(desc, "Tiano Core Loader");
