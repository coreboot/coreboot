/*
 * This file is part of the coreboot project.
 *
 * Copyright 2015 Google Inc
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
 * Foundation, Inc.
 */

#include <console/console.h>
#include <cbmem.h>
#include <fsp_util.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <uefi_types.h>

#define FSP_DBG_LVL BIOS_NEVER

static const EFI_GUID ffs2_guid = EFI_FIRMWARE_FILE_SYSTEM2_GUID;
static const EFI_GUID fih_guid = FSP_INFO_HEADER_GUID;

struct fsp_patch_table {
	uint32_t signature;
	uint16_t header_length;
	uint8_t header_revision;
	uint8_t reserved;
	uint32_t patch_entry_num;
	uint32_t patch_entries[0];
} __attribute__((packed));

#define FSPP_SIG 0x50505346

static void *relative_offset(void *base, ssize_t offset)
{
	uintptr_t loc;

	loc = (uintptr_t)base;
	loc += offset;

	return (void *)loc;
}

static uint32_t *fspp_reloc(void *fsp, size_t fsp_size, uint32_t e)
{
	size_t offset;

	/* Offsets live in bits 23:0. */
	offset = e & 0xffffff;

	/* If bit 31 is set then the offset is considered a negative value
	 * relative to the end of the image using 16MiB as the offset's
	 * reference. */
	if (e & (1 << 31))
		offset = fsp_size - (16 * MiB - offset);

	/* Determine if offset falls within fsp_size for a 32 bit relocation. */
	if (offset > fsp_size - sizeof(uint32_t))
		return NULL;

	return relative_offset(fsp, offset);
}

static int reloc_type(uint16_t reloc_entry)
{
	/* Reloc type in upper 4 bits */
	return reloc_entry >> 12;
}

static size_t reloc_offset(uint16_t reloc_entry)
{
	/* Offsets are in low 12 bits. */
	return reloc_entry & ((1 << 12) - 1);
}

static int te_relocate_in_place(void *te, size_t size)
{
	EFI_TE_IMAGE_HEADER *teih;
	EFI_IMAGE_DATA_DIRECTORY *relocd;
	EFI_IMAGE_BASE_RELOCATION *relocb;
	size_t fixup_offset;
	size_t num_relocs;
	uint16_t *reloc;
	size_t relocd_offset;
	uint8_t *te_base;
	uint32_t adj;

	teih = te;

	if (teih->Signature != EFI_TE_IMAGE_HEADER_SIGNATURE) {
		printk(BIOS_ERR, "TE Signature mismatch: %x vs %x\n",
			teih->Signature, EFI_TE_IMAGE_HEADER_SIGNATURE);
		return -1;
	}

	/*
	 * A TE image is created by converting a PE file. Because of this
	 * the offsets within the headers are off. In order to calculate
	 * the correct releative offets one needs to subtract fixup_offset
	 * from the encoded offets.  Similarly, the linked address of the
	 * program is found by adding the fixup_offset to the ImageBase.
	 */
	fixup_offset = teih->StrippedSize - sizeof(EFI_TE_IMAGE_HEADER);
	/* Keep track of a base that is correctly adjusted so that offsets
	 * can be used directly. */
	te_base = te;
	te_base -= fixup_offset;

	adj = (uintptr_t)te - (teih->ImageBase + fixup_offset);

	printk(FSP_DBG_LVL, "TE Image %p -> %p adjust value: %x\n",
		(void *)(uintptr_t)(teih->ImageBase + fixup_offset),
		te, adj);

	/* Adjust ImageBase for consistency. */
	teih->ImageBase = (uint32_t)(teih->ImageBase + adj);

	relocd = &teih->DataDirectory[EFI_TE_IMAGE_DIRECTORY_ENTRY_BASERELOC];

	relocd_offset = 0;
	/* Though the field name is VirtualAddress it's actually relative to
	 * the beginning of the image which is linked at ImageBase. */
	relocb = relative_offset(te, relocd->VirtualAddress - fixup_offset);
	while (relocd_offset < relocd->Size) {
		size_t rva_offset = relocb->VirtualAddress;

		printk(FSP_DBG_LVL, "Relocs for RVA offset %zx\n", rva_offset);
		num_relocs = relocb->SizeOfBlock - sizeof(*relocb);
		num_relocs /= sizeof(uint16_t);
		reloc = relative_offset(relocb, sizeof(*relocb));

		printk(FSP_DBG_LVL, "Num relocs in block: %zx\n", num_relocs);

		while (num_relocs > 0) {
			int type = reloc_type(*reloc);
			size_t offset = reloc_offset(*reloc);

			printk(FSP_DBG_LVL, "reloc type %x offset %zx\n",
				type, offset);

			if (type == EFI_IMAGE_REL_BASED_HIGHLOW) {
				uint32_t *reloc_addr;

				offset += rva_offset;
				reloc_addr = (void *)&te_base[offset];

				printk(FSP_DBG_LVL, "Adjusting %p %x -> %x\n",
					reloc_addr, *reloc_addr,
					*reloc_addr + adj);
				*reloc_addr += adj;
			} else if (type != EFI_IMAGE_REL_BASED_ABSOLUTE) {
				printk(BIOS_ERR, "Unknown reloc type: %x\n",
					type);
				return -1;
			}
			num_relocs--;
			reloc++;
		}

		/* Track consumption of relocation directory contents. */
		relocd_offset += relocb->SizeOfBlock;
		/* Get next relocation block to process. */
		relocb = relative_offset(relocb, relocb->SizeOfBlock);
	}

	return 0;
}

static size_t csh_size(const EFI_COMMON_SECTION_HEADER *csh)
{
	size_t size;

	/* Unpack the array into a type that can be used. */
	size = 0;
	size |= csh->Size[0] << 0;
	size |= csh->Size[1] << 8;
	size |= csh->Size[2] << 16;

	return size;
}

static size_t section_data_offset(const EFI_COMMON_SECTION_HEADER *csh)
{
	if (csh_size(csh) == 0x00ffffff)
		return sizeof(EFI_COMMON_SECTION_HEADER2);
	else
		return sizeof(EFI_COMMON_SECTION_HEADER);
}

static size_t section_data_size(const EFI_COMMON_SECTION_HEADER *csh)
{
	size_t section_size;

	if (csh_size(csh) == 0x00ffffff)
		section_size = SECTION2_SIZE(csh);
	else
		section_size = csh_size(csh);

	return section_size - section_data_offset(csh);
}

static size_t file_section_offset(const EFI_FFS_FILE_HEADER *ffsfh)
{
	if (IS_FFS_FILE2(ffsfh))
		return sizeof(EFI_FFS_FILE_HEADER2);
	else
		return sizeof(EFI_FFS_FILE_HEADER);
}

static size_t ffs_file_size(const EFI_FFS_FILE_HEADER *ffsfh)
{
	size_t size;

	if (IS_FFS_FILE2(ffsfh))
		size = FFS_FILE2_SIZE(ffsfh);
	else {
		size = ffsfh->Size[0] << 0;
		size |= ffsfh->Size[1] << 8;
		size |= ffsfh->Size[2] << 16;
	}
	return size;
}

static int relocate_patch_table(void *fsp, size_t size, size_t offset,
				ssize_t adjustment)
{
	struct fsp_patch_table *table;
	uint32_t num;

	table = relative_offset(fsp, offset);

	if ((offset + sizeof(*table) > size) ||
	    (table->header_length + offset) > size) {
		printk(BIOS_ERR, "FSPP not entirely contained in region.\n");
		return -1;
	}

	printk(FSP_DBG_LVL, "FSPP relocs: %x\n", table->patch_entry_num);

	for (num = 0; num < table->patch_entry_num; num++) {
		uint32_t *reloc;

		reloc = fspp_reloc(fsp, size, table->patch_entries[num]);

		if (reloc == NULL) {
			printk(BIOS_ERR, "Ignoring FSPP entry: %x\n",
				table->patch_entries[num]);
			continue;
		}

		printk(FSP_DBG_LVL, "Adjusting %p %x -> %x\n",
			reloc, *reloc, (unsigned int)(*reloc + adjustment));

		*reloc += adjustment;
	}

	return 0;
}

static void *relocate_remaining_items(void *fsp, size_t size, size_t fih_offset)
{
	EFI_FFS_FILE_HEADER *ffsfh;
	EFI_COMMON_SECTION_HEADER *csh;
	FSP_INFO_HEADER *fih;
	ssize_t adjustment;
	size_t offset;

	printk(FSP_DBG_LVL, "FSP_INFO_HEADER offset is %zx\n", fih_offset);

	if (fih_offset == 0) {
		printk(BIOS_ERR, "FSP_INFO_HEADER offset is 0.\n");
		return NULL;
	}

	/* FSP_INFO_HEADER at first file in FV within first RAW section. */
	ffsfh = relative_offset(fsp, fih_offset);
	fih_offset += file_section_offset(ffsfh);
	csh = relative_offset(fsp, fih_offset);
	fih_offset += section_data_offset(csh);
	fih = relative_offset(fsp, fih_offset);

	if (memcmp(&ffsfh->Name, &fih_guid, sizeof(fih_guid))) {
		printk(BIOS_ERR, "Bad FIH GUID.\n");
		return NULL;
	}

	if (csh->Type != EFI_SECTION_RAW) {
		printk(BIOS_ERR, "FIH file should have raw section: %x\n",
			csh->Type);
		return NULL;
	}

	if (fih->Signature != FSP_SIG) {
		printk(BIOS_ERR, "Unexpected FIH signature: %08x\n",
			fih->Signature);
		return NULL;
	}

	adjustment = (intptr_t)fsp - fih->ImageBase;

	/* Update ImageBase to reflect FSP's new home. */
	fih->ImageBase += adjustment;

	/* Need to find patch table and adjust each entry. The tables
	 * following FSP_INFO_HEADER have a 32-bit signature and header
	 * length. The patch table is denoted as having a 'FSPP' signature;
	 * the table format doesn't follow the other tables. */
	offset = fih_offset + fih->HeaderLength;
	while (offset + 2 * sizeof(uint32_t) <= size) {
		uint32_t *table_headers;

		table_headers = relative_offset(fsp, offset);

		printk(FSP_DBG_LVL, "Checking offset %zx for 'FSPP'\n",
			offset);

		if (table_headers[0] != FSPP_SIG) {
			offset += table_headers[1];
			continue;
		}

		if (relocate_patch_table(fsp, size, offset, adjustment)) {
			printk(BIOS_ERR, "FSPP relocation failed.\n");
			return NULL;
		}

		return fih;
	}

	printk(BIOS_ERR, "Could not find the FSP patch table.\n");
	return NULL;
}

static ssize_t relocate_fvh(void *fsp, size_t fsp_size, size_t fvh_offset,
				size_t *fih_offset)
{
	EFI_FIRMWARE_VOLUME_HEADER *fvh;
	EFI_FFS_FILE_HEADER *ffsfh;
	EFI_COMMON_SECTION_HEADER *csh;
	size_t offset;
	size_t file_offset;
	size_t size;

	offset = fvh_offset;
	fvh = relative_offset(fsp, offset);

	if (fvh->Signature != EFI_FVH_SIGNATURE)
		return -1;

	printk(FSP_DBG_LVL, "FVH length: %zx Offset: %zx Mapping length: %zx\n",
		(size_t)fvh->FvLength, offset, fsp_size);

	if (fvh->FvLength + offset > fsp_size)
		return -1;

	/* Parse only this FV. However, the algorithm uses offsets into the
	 * entire FSP region so make size include the starting offset. */
	size = fvh->FvLength + offset;

	if (memcmp(&fvh->FileSystemGuid, &ffs2_guid, sizeof(ffs2_guid))) {
		printk(BIOS_ERR, "FVH not an FFS2 type.\n");
		return -1;
	}

	if (fvh->ExtHeaderOffset != 0) {
		EFI_FIRMWARE_VOLUME_EXT_HEADER *fveh;

		offset += fvh->ExtHeaderOffset;
		fveh = relative_offset(fsp, offset);
		printk(FSP_DBG_LVL, "Extended Header Offset: %zx Size: %zx\n",
			(size_t)fvh->ExtHeaderOffset,
			(size_t)fveh->ExtHeaderSize);
		offset += fveh->ExtHeaderSize;
		/* FFS files are 8 byte aligned after extended header. */
		offset = ALIGN_UP(offset, 8);
	} else {
		offset += fvh->HeaderLength;
	}

	file_offset = offset;
	while (file_offset + sizeof(*ffsfh) < size) {
		offset = file_offset;
		printk(FSP_DBG_LVL, "file offset: %zx\n", file_offset);

		/* First file and section should be FSP info header. */
		if (fih_offset != NULL && *fih_offset == 0)
			*fih_offset = file_offset;

		ffsfh = relative_offset(fsp, file_offset);

		printk(FSP_DBG_LVL, "file type = %x\n", ffsfh->Type);
		printk(FSP_DBG_LVL, "file attribs = %x\n", ffsfh->Attributes);

		/* Exit FV relocation when empty space found */
		if (ffsfh->Type == EFI_FV_FILETYPE_FFS_MAX)
			break;

		/* Next file on 8 byte alignment. */
		file_offset += ffs_file_size(ffsfh);
		file_offset = ALIGN_UP(file_offset, 8);

		/* Padding files have no section information. */
		if (ffsfh->Type == EFI_FV_FILETYPE_FFS_PAD)
			continue;

		offset += file_section_offset(ffsfh);

		while (offset + sizeof(*csh) < file_offset) {
			size_t data_size;
			size_t data_offset;

			csh = relative_offset(fsp, offset);

			printk(FSP_DBG_LVL, "section offset: %zx\n", offset);
			printk(FSP_DBG_LVL, "section type: %x\n", csh->Type);

			data_size = section_data_size(csh);
			data_offset = section_data_offset(csh);

			if (data_size + data_offset + offset > file_offset) {
				printk(BIOS_ERR, "Section exceeds FV size.\n");
				return -1;
			}

			if (csh->Type == EFI_SECTION_TE) {
				void *te;
				size_t te_offset = offset + data_offset;

				printk(FSP_DBG_LVL, "TE image at offset %zx\n",
					te_offset);
				te = relative_offset(fsp, te_offset);
				te_relocate_in_place(te, data_size);
			}

			offset += data_size + data_offset;
			/* Sections are aligned to 4 bytes. */
			offset = ALIGN_UP(offset, 4);
		}
	}

	/* Return amount of buffer parsed: FV size. */
	return fvh->FvLength;
}

static FSP_INFO_HEADER *fsp_relocate_in_place(void *fsp, size_t size)
{
	size_t offset;
	size_t fih_offset;

	offset = 0;
	fih_offset = 0;
	while (offset < size) {
		ssize_t nparsed;

		/* Relocate each FV within the FSP region. The FSP_INFO_HEADER
		 * should only be located in the first FV. */
		if (offset == 0)
			nparsed = relocate_fvh(fsp, size, offset, &fih_offset);
		else
			nparsed = relocate_fvh(fsp, size, offset, NULL);

		/* FV should be larger than 0 or failed to parse. */
		if (nparsed <= 0) {
			printk(BIOS_ERR, "FV @ offset %zx relocation failed\n",
				offset);
			return NULL;
		}

		offset += nparsed;
	}

	return relocate_remaining_items(fsp, size, fih_offset);
}

FSP_INFO_HEADER *fsp_relocate(void *fsp_src, size_t size)
{
	void *new_loc;

	new_loc = cbmem_add(CBMEM_ID_REFCODE, size);
	if (new_loc == NULL) {
		printk(BIOS_ERR, "Unable to load FSP into memory.\n");
		return NULL;
	}
	memcpy(new_loc, fsp_src, size);
	return fsp_relocate_in_place(new_loc, size);
}
