/* SPDX-License-Identifier: GPL-2.0-only */

#include <console/console.h>
#include <commonlib/endian.h>
#include <commonlib/fsp.h>
/*
 * Intel's code does not have a handle on changing global packing state.
 * Therefore, one needs to protect against packing policies that are set
 * globally for a compilation unit just by including a header file.
 */
#pragma pack(push)

/* Default bind FSP 1.1 API to edk2 UEFI 2.4 types. */
#include <vendorcode/intel/edk2/uefi_2.4/uefi_types.h>
#include <vendorcode/intel/fsp/fsp1_1/IntelFspPkg/Include/FspInfoHeader.h>

/* Restore original packing policy. */
#pragma pack(pop)

#include <commonlib/helpers.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

#define FSP_DBG_LVL BIOS_NEVER
#define MASK_24BITS  0x00FFFFFF

/*
 * UEFI defines everything as little endian. However, this piece of code
 * can be integrated in a userland tool. That tool could be on a big endian
 * machine so one needs to access the fields within UEFI structures using
 * endian-aware accesses.
 */

/* Return 0 if equal. Non-zero if not equal. */
static int guid_compare(const EFI_GUID *le_guid, const EFI_GUID *native_guid)
{
	if (read_le32(&le_guid->Data1) != native_guid->Data1)
		return 1;
	if (read_le16(&le_guid->Data2) != native_guid->Data2)
		return 1;
	if (read_le16(&le_guid->Data3) != native_guid->Data3)
		return 1;
	return memcmp(le_guid->Data4, native_guid->Data4,
			ARRAY_SIZE(le_guid->Data4));
}

static const EFI_GUID ffs2_guid = EFI_FIRMWARE_FILE_SYSTEM2_GUID;
static const EFI_GUID fih_guid = FSP_INFO_HEADER_GUID;

struct fsp_patch_table {
	uint32_t signature;
	uint16_t header_length;
	uint8_t header_revision;
	uint8_t reserved;
	uint32_t patch_entry_num;
	uint32_t patch_entries[0];
} __packed;

#define FSPP_SIG 0x50505346

static void *relative_offset(void *base, ssize_t offset)
{
	uintptr_t loc;

	loc = (uintptr_t)base;
	loc += offset;

	return (void *)loc;
}

static size_t csh_size(const EFI_COMMON_SECTION_HEADER *csh)
{
	size_t size;

	/* Unpack the array into a type that can be used. */
	size = 0;
	size |= read_le8(&csh->Size[0]) << 0;
	size |= read_le8(&csh->Size[1]) << 8;
	size |= read_le8(&csh->Size[2]) << 16;

	return size;
}

static size_t file_section_offset(const EFI_FFS_FILE_HEADER *ffsfh)
{
	if (IS_FFS_FILE2(ffsfh))
		return sizeof(EFI_FFS_FILE_HEADER2);
	else
		return sizeof(EFI_FFS_FILE_HEADER);
}

static size_t section_data_offset(const EFI_COMMON_SECTION_HEADER *csh)
{
	if (csh_size(csh) == MASK_24BITS)
		return sizeof(EFI_COMMON_SECTION_HEADER2);
	else
		return sizeof(EFI_COMMON_SECTION_HEADER);
}

static uint32_t *fspp_reloc(void *fsp, size_t fsp_size, uint32_t e)
{
	size_t offset;

	/* Offsets live in bits 23:0. */
	offset = e & MASK_24BITS;

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

static FSP_INFO_HEADER *fsp_get_info_hdr(void *fsp, size_t fih_offset)
{
	EFI_FFS_FILE_HEADER *ffsfh;
	EFI_COMMON_SECTION_HEADER *csh;
	FSP_INFO_HEADER *fih;

	printk(FSP_DBG_LVL, "FSP_INFO_HEADER offset is %zx\n", fih_offset);

	if (fih_offset == 0) {
		printk(BIOS_ERR, "FSP_INFO_HEADER offset is 0.\n");
		return NULL;
	}

	/* FSP_INFO_HEADER is located at first file in FV within first RAW section. */
	ffsfh = relative_offset(fsp, fih_offset);
	fih_offset += file_section_offset(ffsfh);
	csh = relative_offset(fsp, fih_offset);
	fih_offset += section_data_offset(csh);
	fih = relative_offset(fsp, fih_offset);

	if (guid_compare(&ffsfh->Name, &fih_guid)) {
		printk(BIOS_ERR, "Bad FIH GUID.\n");
		return NULL;
	}

	if (read_le8(&csh->Type) != EFI_SECTION_RAW) {
		printk(BIOS_ERR, "FIH file should have raw section: %x\n",
			read_le8(&csh->Type));
		return NULL;
	}

	if (read_le32(&fih->Signature) != FSP_SIG) {
		printk(BIOS_ERR, "Unexpected FIH signature: %08x\n",
			read_le32(&fih->Signature));
		return NULL;
	}

	return fih;
}

static int pe_relocate(uintptr_t new_addr, void *pe, void *fsp, size_t fih_off)
{
	EFI_IMAGE_NT_HEADERS32 *peih;
	EFI_IMAGE_DOS_HEADER *doshdr;
	EFI_IMAGE_OPTIONAL_HEADER32 *ophdr;
	FSP_INFO_HEADER *fih;
	uint32_t  roffset, rsize;
	uint32_t  offset;
	uint8_t *pe_base = pe;
	uint32_t image_base;
	uint32_t img_base_off;
	uint32_t delta;

	doshdr = pe;
	if (read_le16(&doshdr->e_magic) != EFI_IMAGE_DOS_SIGNATURE) {
		printk(BIOS_ERR, "Invalid DOS Header/magic\n");
		return -1;
	}

	peih = relative_offset(pe, doshdr->e_lfanew);

	if (read_le32(&peih->Signature) != EFI_IMAGE_NT_SIGNATURE) {
		printk(BIOS_ERR, "Invalid PE32 header\n");
		return -1;
	}

	ophdr = &peih->OptionalHeader;

	if (read_le16(&ophdr->Magic) != EFI_IMAGE_NT_OPTIONAL_HDR32_MAGIC) {
		printk(BIOS_ERR, "No support for non-PE32 images\n");
		return -1;
	}

	fih = fsp_get_info_hdr(fsp, fih_off);
	if (fih == NULL) {
		printk(BIOS_ERR, "No Image base found for FSP PE32\n");
		return -1;
	}
	image_base = read_le32(&fih->ImageBase);
	printk(FSP_DBG_LVL, "FSP InfoHdr Image Base is %x\n", image_base);

	delta = new_addr - image_base;

	img_base_off = read_le32(&ophdr->ImageBase);
	printk(FSP_DBG_LVL, "lfanew 0x%x, delta-0x%x, FSP Base 0x%x, NT32ImageBase 0x%x, offset 0x%x\n",
			read_le32(&doshdr->e_lfanew),
			delta, image_base, img_base_off,
			(uint32_t)((uint8_t *)&ophdr->ImageBase - pe_base));

	printk(FSP_DBG_LVL, "relocating PE32 image at addr - 0x%lx\n", new_addr);
	rsize = read_le32(&ophdr->DataDirectory[EFI_IMAGE_DIRECTORY_ENTRY_BASERELOC].Size);
	roffset = read_le32(&ophdr->DataDirectory[EFI_IMAGE_DIRECTORY_ENTRY_BASERELOC].VirtualAddress);
	printk(FSP_DBG_LVL, "relocation table at offset-%x,size=%x\n", roffset, rsize);
	// TODO - add support for PE32+ also

	offset = roffset;
	while (offset < (roffset + rsize)) {
		uint32_t vaddr;
		uint32_t rlen, rnum;
		uint16_t *rdata;
		uint32_t i;
		EFI_IMAGE_DATA_DIRECTORY *relocd;

		relocd = (void *)&pe_base[offset];
		offset += sizeof(*relocd);
		// Read relocation type, offset pairs
		rlen = read_le32(&relocd->Size) - sizeof(*relocd);
		rnum = rlen / sizeof(uint16_t);
		vaddr = read_le32(&relocd->VirtualAddress);
		rdata = (uint16_t *)&pe_base[offset];
		printk(FSP_DBG_LVL, "\t%d Relocs for RVA %x\n", rnum, vaddr);

		for (i = 0; i < rnum; i++) {
			uint16_t roff = reloc_offset(rdata[i]);
			uint16_t rtype = reloc_type(rdata[i]);
			uint32_t aoff = vaddr + roff;
			uint32_t val;
			printk(FSP_DBG_LVL, "\t\treloc type %x offset %x aoff %x, base-0x%x\n",
					rtype, roff, aoff, img_base_off);
			switch (rtype) {
			case EFI_IMAGE_REL_BASED_ABSOLUTE:
				continue;
			case EFI_IMAGE_REL_BASED_HIGHLOW:
				val = read_le32(&pe_base[aoff]);
				printk(FSP_DBG_LVL, "Adjusting %p %x -> %x\n",
					&pe_base[aoff], val, val + delta);
				write_le32(&pe_base[aoff], val + delta);
				break;
			case EFI_IMAGE_REL_BASED_DIR64:
				printk(BIOS_ERR, "Unsupported DIR64\n");
				break;
			default:
				printk(BIOS_ERR, "Unsupported relocation type %d\n",
						rtype);
				return -1;
			}
		}
		offset += sizeof(*rdata) * rnum;
	}
	printk(FSP_DBG_LVL, "Adjust Image Base %x->%x\n",
			img_base_off, img_base_off + delta);
	img_base_off += delta;
	write_le32(&ophdr->ImageBase, img_base_off);

	return -1;
}

static int te_relocate(uintptr_t new_addr, void *te)
{
	EFI_TE_IMAGE_HEADER *teih;
	EFI_IMAGE_DATA_DIRECTORY *relocd;
	EFI_IMAGE_BASE_RELOCATION *relocb;
	uintptr_t image_base;
	size_t fixup_offset;
	size_t num_relocs;
	uint16_t *reloc;
	size_t relocd_offset;
	uint8_t *te_base;
	uint32_t adj;

	teih = te;

	if (read_le16(&teih->Signature) != EFI_TE_IMAGE_HEADER_SIGNATURE) {
		printk(BIOS_ERR, "TE Signature mismatch: %x vs %x\n",
			read_le16(&teih->Signature),
			EFI_TE_IMAGE_HEADER_SIGNATURE);
		return -1;
	}

	/*
	 * A TE image is created by converting a PE file. Because of this
	 * the offsets within the headers are off. In order to calculate
	 * the correct relative offsets one needs to subtract fixup_offset
	 * from the encoded offsets. Similarly, the linked address of the
	 * program is found by adding the fixup_offset to the ImageBase.
	 */
	fixup_offset = read_le16(&teih->StrippedSize);
	fixup_offset -= sizeof(EFI_TE_IMAGE_HEADER);
	/* Keep track of a base that is correctly adjusted so that offsets
	 * can be used directly. */
	te_base = te;
	te_base -= fixup_offset;

	image_base = read_le64(&teih->ImageBase);
	adj = new_addr - (image_base + fixup_offset);

	printk(FSP_DBG_LVL, "TE Image %p -> %p adjust value: %x\n",
		(void *)image_base, (void *)new_addr, adj);

	/* Adjust ImageBase for consistency. */
	write_le64(&teih->ImageBase, (uint32_t)(image_base + adj));

	relocd = &teih->DataDirectory[EFI_TE_IMAGE_DIRECTORY_ENTRY_BASERELOC];

	relocd_offset = 0;
	/* Though the field name is VirtualAddress it's actually relative to
	 * the beginning of the image which is linked at ImageBase. */
	relocb = relative_offset(te,
			read_le32(&relocd->VirtualAddress) - fixup_offset);
	while (relocd_offset < read_le32(&relocd->Size)) {
		size_t rva_offset = read_le32(&relocb->VirtualAddress);

		printk(FSP_DBG_LVL, "Relocs for RVA offset %zx\n", rva_offset);
		num_relocs = read_le32(&relocb->SizeOfBlock) - sizeof(*relocb);
		num_relocs /= sizeof(uint16_t);
		reloc = relative_offset(relocb, sizeof(*relocb));

		printk(FSP_DBG_LVL, "Num relocs in block: %zx\n", num_relocs);

		while (num_relocs > 0) {
			uint16_t reloc_val = read_le16(reloc);
			int type = reloc_type(reloc_val);
			size_t offset = reloc_offset(reloc_val);

			printk(FSP_DBG_LVL, "reloc type %x offset %zx\n",
				type, offset);

			if (type == EFI_IMAGE_REL_BASED_HIGHLOW ||
					type == EFI_IMAGE_REL_BASED_DIR64) {
				uint32_t *reloc_addr;
				uint32_t val;

				offset += rva_offset;
				reloc_addr = (void *)&te_base[offset];
				val = read_le32(reloc_addr);

				printk(FSP_DBG_LVL, "Adjusting %p %x -> %x\n",
					reloc_addr, val, val + adj);
				write_le32(reloc_addr, val + adj);
			} else if (type != EFI_IMAGE_REL_BASED_ABSOLUTE) {
				printk(BIOS_ERR, "Unknown reloc type: %x\n",
					type);
				return -1;
			}
			num_relocs--;
			reloc++;
		}

		/* Track consumption of relocation directory contents. */
		relocd_offset += read_le32(&relocb->SizeOfBlock);
		/* Get next relocation block to process. */
		relocb = relative_offset(relocb,
					read_le32(&relocb->SizeOfBlock));
	}

	return 0;
}

static size_t section_data_size(const EFI_COMMON_SECTION_HEADER *csh)
{
	size_t section_size;

	if (csh_size(csh) == MASK_24BITS)
		section_size = read_le32(&SECTION2_SIZE(csh));
	else
		section_size = csh_size(csh);

	return section_size - section_data_offset(csh);
}

static size_t ffs_file_size(const EFI_FFS_FILE_HEADER *ffsfh)
{
	size_t size;

	if (IS_FFS_FILE2(ffsfh)) {
		/*
		 * this cast is needed with UEFI 2.6 headers in order
		 * to read the UINT32 value that FFS_FILE2_SIZE converts
		 * the return into
		 */
		uint32_t file2_size = FFS_FILE2_SIZE(ffsfh);
		size = read_le32(&file2_size);
	} else {
		size = read_le8(&ffsfh->Size[0]) << 0;
		size |= read_le8(&ffsfh->Size[1]) << 8;
		size |= read_le8(&ffsfh->Size[2]) << 16;
	}
	return size;
}

static int relocate_patch_table(void *fsp, size_t size, size_t offset,
				ssize_t adjustment)
{
	struct fsp_patch_table *table;
	size_t num;
	size_t num_entries;

	table = relative_offset(fsp, offset);

	if ((offset + sizeof(*table) > size) ||
	    (read_le16(&table->header_length) + offset) > size) {
		printk(BIOS_ERR, "FSPP not entirely contained in region.\n");
		return -1;
	}

	num_entries = read_le32(&table->patch_entry_num);
	printk(FSP_DBG_LVL, "FSPP relocs: %zx\n", num_entries);

	for (num = 0; num < num_entries; num++) {
		uint32_t *reloc;
		uint32_t reloc_val;

		reloc = fspp_reloc(fsp, size,
				read_le32(&table->patch_entries[num]));

		if (reloc == NULL) {
			printk(BIOS_ERR, "Ignoring FSPP entry: %x\n",
				read_le32(&table->patch_entries[num]));
			continue;
		}

		reloc_val = read_le32(reloc);
		printk(FSP_DBG_LVL, "Adjusting %p %x -> %x\n",
			reloc, reloc_val,
			(unsigned int)(reloc_val + adjustment));

		write_le32(reloc, reloc_val + adjustment);
	}

	return 0;
}

static ssize_t relocate_remaining_items(void *fsp, size_t size,
					uintptr_t new_addr, size_t fih_offset)
{
	EFI_FFS_FILE_HEADER *ffsfh;
	EFI_COMMON_SECTION_HEADER *csh;
	FSP_INFO_HEADER *fih;
	ssize_t adjustment;
	size_t offset;

	printk(FSP_DBG_LVL, "FSP_INFO_HEADER offset is %zx\n", fih_offset);

	if (fih_offset == 0) {
		printk(BIOS_ERR, "FSP_INFO_HEADER offset is 0.\n");
		return -1;
	}

	/* FSP_INFO_HEADER at first file in FV within first RAW section. */
	ffsfh = relative_offset(fsp, fih_offset);
	fih_offset += file_section_offset(ffsfh);
	csh = relative_offset(fsp, fih_offset);
	fih_offset += section_data_offset(csh);
	fih = relative_offset(fsp, fih_offset);

	if (guid_compare(&ffsfh->Name, &fih_guid)) {
		printk(BIOS_ERR, "Bad FIH GUID.\n");
		return -1;
	}

	if (read_le8(&csh->Type) != EFI_SECTION_RAW) {
		printk(BIOS_ERR, "FIH file should have raw section: %x\n",
			read_le8(&csh->Type));
		return -1;
	}

	if (read_le32(&fih->Signature) != FSP_SIG) {
		printk(BIOS_ERR, "Unexpected FIH signature: %08x\n",
			read_le32(&fih->Signature));
	}

	adjustment = (intptr_t)new_addr - read_le32(&fih->ImageBase);

	/* Update ImageBase to reflect FSP's new home. */
	write_le32(&fih->ImageBase, adjustment + read_le32(&fih->ImageBase));
	printk(FSP_DBG_LVL, "Updated FSP InfoHdr Image Base to %x\n",
			read_le32(&fih->ImageBase));

	/* Need to find patch table and adjust each entry. The tables
	 * following FSP_INFO_HEADER have a 32-bit signature and header
	 * length. The patch table is denoted as having a 'FSPP' signature;
	 * the table format doesn't follow the other tables. */
	offset = fih_offset + read_le32(&fih->HeaderLength);
	while (offset + 2 * sizeof(uint32_t) <= size) {
		uint32_t *table_headers;

		table_headers = relative_offset(fsp, offset);

		printk(FSP_DBG_LVL, "Checking offset %zx for 'FSPP'\n",
			offset);

		if (read_le32(&table_headers[0]) != FSPP_SIG) {
			offset += read_le32(&table_headers[1]);
			continue;
		}

		if (relocate_patch_table(fsp, size, offset, adjustment)) {
			printk(BIOS_ERR, "FSPP relocation failed.\n");
			return -1;
		}

		return fih_offset;
	}

	printk(BIOS_ERR, "Could not find the FSP patch table.\n");
	return -1;
}

static ssize_t relocate_fvh(uintptr_t new_addr, void *fsp, size_t fsp_size,
				size_t fvh_offset, size_t *fih_offset)
{
	EFI_FIRMWARE_VOLUME_HEADER *fvh;
	EFI_FFS_FILE_HEADER *ffsfh;
	EFI_COMMON_SECTION_HEADER *csh;
	size_t offset;
	size_t file_offset;
	size_t size;
	size_t fv_length;

	offset = fvh_offset;
	fvh = relative_offset(fsp, offset);

	if (read_le32(&fvh->Signature) != EFI_FVH_SIGNATURE)
		return -1;

	fv_length = read_le64(&fvh->FvLength);

	printk(FSP_DBG_LVL, "FVH length: %zx Offset: %zx Mapping length: %zx\n",
		fv_length, offset, fsp_size);

	if (fv_length + offset > fsp_size)
		return -1;

	/* Parse only this FV. However, the algorithm uses offsets into the
	 * entire FSP region so make size include the starting offset. */
	size = fv_length + offset;

	if (guid_compare(&fvh->FileSystemGuid, &ffs2_guid)) {
		printk(BIOS_ERR, "FVH not an FFS2 type.\n");
		return -1;
	}

	if (read_le16(&fvh->ExtHeaderOffset) != 0) {
		EFI_FIRMWARE_VOLUME_EXT_HEADER *fveh;

		offset += read_le16(&fvh->ExtHeaderOffset);
		fveh = relative_offset(fsp, offset);
		printk(FSP_DBG_LVL, "Extended Header Offset: %zx Size: %zx\n",
			(size_t)read_le16(&fvh->ExtHeaderOffset),
			(size_t)read_le32(&fveh->ExtHeaderSize));
		offset += read_le32(&fveh->ExtHeaderSize);
		/* FFS files are 8 byte aligned after extended header. */
		offset = ALIGN_UP(offset, 8);
	} else {
		offset += read_le16(&fvh->HeaderLength);
	}

	file_offset = offset;
	while (file_offset + sizeof(*ffsfh) < size) {
		offset = file_offset;
		printk(FSP_DBG_LVL, "file offset: %zx\n", file_offset);

		/* First file and section should be FSP info header. */
		if (fih_offset != NULL && *fih_offset == 0)
			*fih_offset = file_offset;

		ffsfh = relative_offset(fsp, file_offset);

		printk(FSP_DBG_LVL, "file type = %x\n", read_le8(&ffsfh->Type));
		printk(FSP_DBG_LVL, "file attribs = %x\n",
			read_le8(&ffsfh->Attributes));

		/* Exit FV relocation when empty space found */
		if (read_le8(&ffsfh->Type) == EFI_FV_FILETYPE_FFS_MAX)
			break;

		/* Next file on 8 byte alignment. */
		file_offset += ffs_file_size(ffsfh);
		file_offset = ALIGN_UP(file_offset, 8);

		/* Padding files have no section information. */
		if (read_le8(&ffsfh->Type) == EFI_FV_FILETYPE_FFS_PAD)
			continue;

		offset += file_section_offset(ffsfh);

		while (offset + sizeof(*csh) < file_offset) {
			size_t data_size;
			size_t data_offset;
			void *section_data;
			size_t section_offset;
			uintptr_t section_addr;

			csh = relative_offset(fsp, offset);

			printk(FSP_DBG_LVL, "section offset: %zx\n", offset);
			printk(FSP_DBG_LVL, "section type: %x\n",
				read_le8(&csh->Type));

			data_size = section_data_size(csh);
			data_offset = section_data_offset(csh);

			if (data_size + data_offset + offset > file_offset) {
				printk(BIOS_ERR, "Section exceeds FV size.\n");
				return -1;
			}

			/*
			 * The entire FSP image can be thought of as one
			 * program with a single link address even though there
			 * are multiple TEs linked separately. The reason is
			 * that each TE is linked for XIP. So in order to
			 * relocate the TE properly we need to form the
			 * relocated address based on the TE offset within
			 * FSP proper.
			 */
			section_offset = offset + data_offset;
			section_addr = new_addr + section_offset;
			section_data = relative_offset(fsp, section_offset);

			if (read_le8(&csh->Type) == EFI_SECTION_TE) {
				printk(FSP_DBG_LVL, "TE image at offset %zx\n",
					section_offset);
				te_relocate(section_addr, section_data);
			} else if (read_le8(&csh->Type) == EFI_SECTION_PE32) {
				printk(FSP_DBG_LVL, "PE32 image at offset %zx\n",
					section_offset);
				pe_relocate(new_addr, section_data, fsp, *fih_offset);
			}

			offset += data_size + data_offset;
			/* Sections are aligned to 4 bytes. */
			offset = ALIGN_UP(offset, 4);
		}
	}

	/* Return amount of buffer parsed: FV size. */
	return fv_length;
}

ssize_t fsp_component_relocate(uintptr_t new_addr, void *fsp, size_t size)
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
			nparsed = relocate_fvh(new_addr, fsp, size, offset,
						&fih_offset);
		else
			nparsed = relocate_fvh(new_addr, fsp, size, offset,
						NULL);

		/* FV should be larger than 0 or failed to parse. */
		if (nparsed <= 0) {
			printk(BIOS_ERR, "FV @ offset %zx relocation failed\n",
				offset);
			return -1;
		}

		offset += nparsed;
	}

	return relocate_remaining_items(fsp, size, new_addr, fih_offset);
}

ssize_t fsp1_1_relocate(uintptr_t new_addr, void *fsp, size_t size)
{
	return fsp_component_relocate(new_addr, fsp, size);
}
