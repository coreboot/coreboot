/* SPDX-License-Identifier: GPL-2.0-only */

#include <commonlib/endian.h>
#include <string.h>

#include "cbfs.h"
#include "cbfs_sections.h"
#include "elfparsing.h"

#define	MBN_V7 7

struct mbnv7_hashtbl_seg {
	uint32_t reserved;
	uint32_t version;
	uint32_t common_metadata_size;
	uint32_t qti_metadata_size;
	uint32_t oem_metadata_size;
	uint32_t hash_table_size;
	uint32_t qti_signature_size;
	uint32_t qti_certificate_chain_size;
	uint32_t oem_signature_size;
	uint32_t oem_certificate_chain_size;
};

enum hash_table_algo {
	QUALCOMM_HASH_TABLE_ALGO_SHA384 = 3,
};

struct common_metadata {
	uint32_t major;
	uint32_t minor;
	uint32_t software_id;
	uint32_t secondary_software_id;
	uint32_t hash_table_algorithm;
	uint32_t measurement_register_target;
};

static void *qualcomm_find_hash_mbnv7(struct buffer *elf, size_t bb_offset,
				      struct vb2_hash *real_hash)
{
	void *hashtable = NULL, *bb_hash = NULL;
	struct common_metadata *cmn = NULL;
	struct mbnv7_hashtbl_seg *mbn;
	struct parsed_elf pelf;
	int bb_segment = -1;
	Elf64_Phdr *ph;

	while (true) {
		size_t tmp;

		if (!buffer_check_magic(elf, ELFMAG, 4) ||
		     parse_elf(elf, &pelf, ELF_PARSE_PHDR))
			return NULL;	/* Not an ELF -- guess not a Qualcomm MBN after all? */

		ph = &pelf.phdr[pelf.ehdr.e_phnum - 1];

		tmp = ph->p_offset + ph->p_filesz;

		if (buffer_offset(elf) < bb_offset &&
		    bb_offset < (buffer_offset(elf) + tmp)) /* found BB */
			break;

		parsed_elf_destroy(&pelf);

		buffer_seek(elf, tmp);
	}

	/* Qualcomm stores an array of SHA-384 hashes in a special ELF segment,
	   one for each segment in order. */
	for (int i = 0; i < pelf.ehdr.e_phnum; i++) {
		ph = &pelf.phdr[i];
		mbn = buffer_get(elf) + ph->p_offset;

		if (((ph->p_flags & PF_QC_SG_MASK) == PF_QC_SG_HASH) &&
		    (mbn->version == MBN_V7)) {
			cmn = buffer_get(elf) + ph->p_offset +
			      sizeof(*mbn);

			hashtable = buffer_get(elf) + ph->p_offset +
				    sizeof(*mbn) + mbn->common_metadata_size +
				    mbn->qti_metadata_size +
				    mbn->oem_metadata_size;
		} else if (ph->p_type == PT_LOAD && ph->p_flags == (PF_X | PF_W | PF_R)) {
			bb_segment = i; /* Found the bootblock segment -- store its index. */
		}
	}

	if (!hashtable || !cmn)
		goto destroy_elf;

	/* MBNv7 supports only SHA384, ensure that. */
	if (cmn->hash_table_algorithm != QUALCOMM_HASH_TABLE_ALGO_SHA384)
		goto destroy_elf;

	bb_hash = hashtable + bb_segment * VB2_SHA384_DIGEST_SIZE;

	/* Pass out the actual hash of the current bootblock segment in |real_hash|. */
	if (vb2_hash_calculate(false, buffer_get(elf) + pelf.phdr[bb_segment].p_offset,
			       pelf.phdr[bb_segment].p_filesz, VB2_HASH_SHA384, real_hash)) {
		ERROR("fixups: vboot digest error\n");
		goto destroy_elf;
	} /* Return pointer to where the bootblock hash needs to go in Qualcomm's table. */

destroy_elf:
	parsed_elf_destroy(&pelf);
	return bb_hash;
}

/*
 * NOTE: This currently only implements support for MBN version 6 (as used by sc7180). Support
 * for other MBN versions could probably be added but may require more parsing to tell them
 * apart, and minor modifications (e.g. different hash algorithm). Add later as needed.
 */
static void *qualcomm_find_hash(struct buffer *in, size_t bb_offset, struct vb2_hash *real_hash)
{
	Elf64_Phdr *ph;
	struct buffer elf;
	buffer_clone(&elf, in);

	/* When buffer_size(&elf) becomes this small, we know we've searched through 32KiB (or
	   the whole bootblock) without finding anything, so we know we can stop looking. */
	size_t search_end_size = MIN(0, buffer_size(in) - 32 * KiB);

	/* To identify a Qualcomm image, first we find the GPT header... */
	while (buffer_size(&elf) > search_end_size &&
	       !buffer_check_magic(&elf, "EFI PART", 8))
		buffer_seek(&elf, 512);

	/* ...then shortly afterwards there's an ELF header... */
	while (buffer_size(&elf) > search_end_size &&
	       !buffer_check_magic(&elf, ELFMAG, 4))
		buffer_seek(&elf, 512);

	if (buffer_size(&elf) <= search_end_size)
		return NULL;	/* Doesn't seem to be a Qualcomm image. */

	struct parsed_elf pelf;
	if (parse_elf(&elf, &pelf, ELF_PARSE_PHDR))
		return NULL;	/* Not an ELF -- guess not a Qualcomm MBN after all? */

	/*
	 * Check if it is a multi ELF or single ELF. If bb_offset is beyond the
	 * final segment of first ELF, it is a multi ELF.
	 */
	ph = &pelf.phdr[pelf.ehdr.e_phnum - 1];
	if (bb_offset > (ph->p_offset + ph->p_filesz)) { /* MBNv7 multi ELF */
		parsed_elf_destroy(&pelf);
		return qualcomm_find_hash_mbnv7(&elf, bb_offset, real_hash);
	}

	/* Qualcomm stores an array of SHA-384 hashes in a special ELF segment. One special one
	   to start with, and then one for each segment in order. */
	void *bb_hash = NULL;
	void *hashtable = NULL;
	int i;
	int bb_segment = -1;
	for (i = 0; i < pelf.ehdr.e_phnum; i++) {
		ph = &pelf.phdr[i];
		if ((ph->p_flags & PF_QC_SG_MASK) == PF_QC_SG_HASH) {
			if ((int)ph->p_filesz !=
			    (pelf.ehdr.e_phnum + 1) * VB2_SHA384_DIGEST_SIZE) {
				ERROR("fixups: Qualcomm hash segment has wrong size!\n");
				goto destroy_elf;
			} /* Found the table with the hashes -- store its address. */
			hashtable = buffer_get(&elf) + ph->p_offset;
		} else if (bb_segment < 0 && ph->p_offset + ph->p_filesz < buffer_size(&elf) &&
			   buffer_offset(&elf) + ph->p_offset <= bb_offset &&
			   buffer_offset(&elf) + ph->p_offset + ph->p_filesz > bb_offset) {
			bb_segment = i;	/* Found the bootblock segment -- store its index. */
		}
	}
	if (!hashtable)	/* ELF but no special QC hash segment -- guess not QC after all? */
		goto destroy_elf;
	if (bb_segment < 0) {	/* Can assume it's QC if we found the special segment. */
		ERROR("fixups: Cannot find bootblock code in Qualcomm MBN!\n");
		goto destroy_elf;
	}

	/* Pass out the actual hash of the current bootblock segment in |real_hash|. */
	if (vb2_hash_calculate(false, buffer_get(&elf) + pelf.phdr[bb_segment].p_offset,
			       pelf.phdr[bb_segment].p_filesz, VB2_HASH_SHA384, real_hash)) {
		ERROR("fixups: vboot digest error\n");
		goto destroy_elf;
	} /* Return pointer to where the bootblock hash needs to go in Qualcomm's table. */
	bb_hash = hashtable + (bb_segment + 1) * VB2_SHA384_DIGEST_SIZE;

destroy_elf:
	parsed_elf_destroy(&pelf);
	return bb_hash;
}

static bool qualcomm_probe(struct buffer *buffer, size_t offset)
{
	struct vb2_hash real_hash;
	void *table_hash = qualcomm_find_hash(buffer, offset, &real_hash);
	if (!table_hash)
		return false;

	if (memcmp(real_hash.raw, table_hash, VB2_SHA384_DIGEST_SIZE)) {
		ERROR("fixups: Identified Qualcomm MBN, but existing hash doesn't match!\n");
		return false;
	}

	return true;
}

static int qualcomm_fixup(struct buffer *buffer, size_t offset)
{
	struct vb2_hash real_hash;
	void *table_hash = qualcomm_find_hash(buffer, offset, &real_hash);
	if (!table_hash) {
		ERROR("fixups: Cannot find Qualcomm MBN headers anymore!\n");
		return -1;
	}

	memcpy(table_hash, real_hash.raw, VB2_SHA384_DIGEST_SIZE);
	INFO("fixups: Updated Qualcomm MBN header bootblock hash.\n");
	return 0;
}

/*
 * MediaTek bootblock.bin layout (see util/mediatek/gen-bl-img.py):
 *	header		2048 bytes
 *	gfh info	176 bytes, where bytes 32-35 (in little endian) is the
 *			total size excluding the header (gfh info + data + hash)
 *	data		`data_size` bytes
 *	hash		32 bytes, SHA256 of "gfh info + data"
 *	padding
 */
#define MEDIATEK_BOOTBLOCK_HEADER_SIZE	2048
#define MEDIATEK_BOOTBLOCK_GFH_SIZE	176
static void *mediatek_find_hash(struct buffer *bootblock, struct vb2_hash *real_hash)
{
	struct buffer buffer;
	size_t data_size;
	const char emmc_magic[] = "EMMC_BOOT";
	const char sf_magic[] = "SF_BOOT";
	const char brlyt_magic[] = "BRLYT";
	const size_t brlyt_offset = 512;

	buffer_clone(&buffer, bootblock);

	/* Doesn't seem to be MediaTek image */
	if (buffer_size(&buffer) <
	    MEDIATEK_BOOTBLOCK_HEADER_SIZE + MEDIATEK_BOOTBLOCK_GFH_SIZE)
		return NULL;

	/* Check header magic */
	if (!buffer_check_magic(&buffer, emmc_magic, strlen(emmc_magic)) &&
	    !buffer_check_magic(&buffer, sf_magic, strlen(sf_magic)))
		return NULL;

	/* Check "BRLYT" */
	buffer_seek(&buffer, brlyt_offset);
	if (!buffer_check_magic(&buffer, brlyt_magic, strlen(brlyt_magic)))
		return NULL;

	buffer_seek(&buffer, MEDIATEK_BOOTBLOCK_HEADER_SIZE - brlyt_offset);
	data_size = read_le32(buffer_get(&buffer) + 32);
	if (data_size <= MEDIATEK_BOOTBLOCK_GFH_SIZE + VB2_SHA256_DIGEST_SIZE) {
		ERROR("fixups: MediaTek: data size too small: %zu\n", data_size);
		return NULL;
	}
	data_size -= MEDIATEK_BOOTBLOCK_GFH_SIZE + VB2_SHA256_DIGEST_SIZE;

	if (buffer_size(&buffer) <
	    MEDIATEK_BOOTBLOCK_GFH_SIZE + data_size + VB2_SHA256_DIGEST_SIZE) {
		ERROR("fixups: MediaTek: not enough data: %zu\n", buffer_size(&buffer));
		return NULL;
	}

	if (vb2_hash_calculate(false, buffer_get(&buffer),
			       MEDIATEK_BOOTBLOCK_GFH_SIZE + data_size,
			       VB2_HASH_SHA256, real_hash)) {
		ERROR("fixups: MediaTek: vboot digest error\n");
		return NULL;
	}

	buffer_seek(&buffer, MEDIATEK_BOOTBLOCK_GFH_SIZE + data_size);
	return buffer_get(&buffer);
}

static bool mediatek_probe(struct buffer *buffer)
{
	struct vb2_hash real_hash;
	void *hash = mediatek_find_hash(buffer, &real_hash);
	if (!hash)
		return false;

	if (memcmp(real_hash.raw, hash, VB2_SHA256_DIGEST_SIZE)) {
		ERROR("fixups: Found MediaTek bootblock, but existing hash doesn't match!\n");
		return false;
	}

	return true;
}

static int mediatek_fixup(struct buffer *buffer, unused size_t offset)
{
	struct vb2_hash real_hash;
	void *hash = mediatek_find_hash(buffer, &real_hash);
	if (!hash) {
		ERROR("fixups: Cannot find MediaTek header anymore!\n");
		return -1;
	}

	memcpy(hash, real_hash.raw, VB2_SHA256_DIGEST_SIZE);
	INFO("fixups: Updated MediaTek bootblock hash.\n");
	return 0;
}

platform_fixup_func platform_fixups_probe(struct buffer *buffer, size_t offset,
					  const char *region_name)
{
	if (!strcmp(region_name, SECTION_NAME_BOOTBLOCK)) {
		if (qualcomm_probe(buffer, offset))
			return qualcomm_fixup;
		else if (mediatek_probe(buffer))
			return mediatek_fixup;
	} else if (!strcmp(region_name, SECTION_NAME_PRIMARY_CBFS)) {
		/* TODO: add fixups for primary CBFS bootblock platforms, if needed */
	} else {
		ERROR("%s called for unexpected FMAP region %s!\n", __func__, region_name);
	}

	return NULL;
}
