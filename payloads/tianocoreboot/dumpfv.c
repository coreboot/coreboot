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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <efi.h>

#undef VERBOSE
#define INVENTORY

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
	printf("  length: 0x%016llx\n", (unsigned long long)fvh->FvLength);
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

#ifdef INVENTORY
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
#endif

int main(int argc, char **argv)
{
	int i;
	void *tiano;
	EFI_FIRMWARE_VOLUME_HEADER *fvh = NULL;
	EFI_PEI_HOB_POINTERS hoblist;
	EFI_COMMON_SECTION_HEADER *dxecore = NULL;

	printf("Firmware Volume Dump\n");
	printf("Copyright (C) 2013 Google Inc. All rights reserved.\n\n");

	char *filename = argv[1];

	/* Find UEFI firmware volume in CBFS */
	int file = open(filename, O_RDONLY);
	if (file == -1) {
		printf("Could not open %s: %s\n", filename, strerror(errno));
		exit(1);
	}

	struct stat buf;
	fstat(file, &buf);

	tiano = malloc(buf.st_size);

	read(file, tiano, buf.st_size);

	void *current = tiano;

	while (current < tiano + buf.st_size) {
		/* Verify UEFI firmware volume consistency */
		fvh = (EFI_FIRMWARE_VOLUME_HEADER *)current;
		if (fvh->Signature != 0x4856465f) {
			printf("Not an UEFI firmware volume at %lx.\n",
				(unsigned long)current - (unsigned long)tiano);
			exit(1);
		}

		/* Dump UEFI firmware volume header */
		dump_uefi_firmware_volume_header(fvh);

		/* Dump UEFI firmware file headers */
		for (i = fvh->HeaderLength; i < fvh->FvLength;) {
			int size;
			EFI_FFS_FILE_HEADER *ffs;

			ffs = (EFI_FFS_FILE_HEADER *)(current + i);

			size = ffs->Size[0] | (ffs->Size[1] << 8) | (ffs->Size[2] << 16);
			if (size == 0xffffff)
				break;
#ifdef INVENTORY
			printf("%08x - ", i);
			dump_uefi_ffs_file_header(ffs);
#endif

			if (ffs->Type == EFI_FV_FILETYPE_DXE_CORE) {
				dxecore = (EFI_COMMON_SECTION_HEADER *)&ffs[1];
#ifndef INVENTORY
				break;
#endif
			}

			i = ALIGN(i + size, 8);
		}

		current += fvh->FvLength;
#ifdef INVENTORY
		printf("\n");
#endif
	}
	free(tiano);
}

