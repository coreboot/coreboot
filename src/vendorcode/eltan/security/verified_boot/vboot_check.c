/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2016 Intel Corp.
 * Copyright (C) 2017-2019 Eltan B.V.
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
#include <boot_device.h>
#include <cbfs.h>
#include <vboot_check.h>
#include <vboot_common.h>
#include "fmap_config.h"

#define RSA_PUBLICKEY_FILE_NAME "vboot_public_key.bin"

#if CONFIG(VENDORCODE_ELTAN_VBOOT_USE_SHA512)
#define DIGEST_SIZE VB2_SHA512_DIGEST_SIZE
#else
#define DIGEST_SIZE VB2_SHA256_DIGEST_SIZE
#endif

int verified_boot_check_manifest(void)
{
	struct vb2_public_key key;
	const struct vb2_workbuf wb;
	uint8_t *buffer;
	uint8_t digest[DIGEST_SIZE];
	uint8_t *signature = NULL;
	size_t size = 0;
	int hash_algorithm;
	int status;

	cbfs_boot_map_with_leak("oemmanifest.bin", CBFS_TYPE_RAW, &size);

	if (size != (CONFIG_VENDORCODE_ELTAN_OEM_MANIFEST_ITEMS *
	    	     DIGEST_SIZE) + 256) {
		printk(BIOS_ERR, "ERROR: Incorrect manifest size!\n");
		goto fail;
	}

	buffer = cbfs_boot_map_with_leak(RSA_PUBLICKEY_FILE_NAME,
			CBFS_TYPE_RAW, &size);

	size = DIGEST_SIZE;
	if (!vb2_unpack_key_data(&key, buffer, size)) {
		printk(BIOS_ERR, "ERROR: Unable to create RSA Public Key !\n");
		goto fail;
	}

	if (CONFIG(VENDORCODE_ELTAN_VBOOT_USE_SHA512)) {
		key.hash_alg = VB2_HASH_SHA512;
		hash_algorithm = VB2_HASH_SHA512;
	} else {
		key.sig_alg = VB2_HASH_SHA256;
		hash_algorithm = VB2_HASH_SHA256;
	}

	/* Create a big endian digest */
	status = cb_sha_endian(hash_algorithm,
		  (const uint8_t *)CONFIG_VENDORCODE_ELTAN_OEM_MANIFEST_LOC,
		  CONFIG_VENDORCODE_ELTAN_OEM_MANIFEST_ITEMS * DIGEST_SIZE,
		  digest, BIG_ENDIAN_ALGORITHM);
	if (status)
		goto fail;

	signature = (uint8_t *)CONFIG_VENDORCODE_ELTAN_OEM_MANIFEST_LOC +
		    CONFIG_VENDORCODE_ELTAN_OEM_MANIFEST_ITEMS * DIGEST_SIZE;

	if (!vb2_rsa_verify_digest(&key, signature, digest, &wb)) {
		printk(BIOS_ERR, "ERROR: Signature verification failed for"
			"hash table !!\n");
		goto fail;
	}

	printk(BIOS_DEBUG, "%s: Successfully verified hash_table signature.\n",
		__func__);
	return 0;

fail:
	die("HASH table verification failed!\n");
	return -1;
}

static int vendor_secure_locate(struct cbfs_props *props)
{
	struct cbfs_header header;
	const struct region_device *bdev;
	int32_t rel_offset;
	size_t offset;

	bdev = boot_device_ro();

	if (bdev == NULL)
		return -1;

	size_t fmap_top = ___FMAP__COREBOOT_BASE + ___FMAP__COREBOOT_SIZE;

	/* Find location of header using signed 32-bit offset from
	 * end of CBFS region. */
	offset = fmap_top - sizeof(int32_t);
	if (rdev_readat(bdev, &rel_offset, offset, sizeof(int32_t)) < 0)
		return -1;

	offset = fmap_top + rel_offset;
	if (rdev_readat(bdev, &header, offset, sizeof(header)) < 0)
		return -1;

	header.magic = ntohl(header.magic);
	header.romsize = ntohl(header.romsize);
	header.offset = ntohl(header.offset);

	if (header.magic != CBFS_HEADER_MAGIC)
		return -1;

	props->offset = header.offset;
	props->size = header.romsize;
	props->size -= props->offset;

	printk(BIOS_SPEW, "CBFS @ %zx size %zx\n", props->offset, props->size);

	return 0;
}

#ifndef __BOOTBLOCK__

/*
 *
 * measure_item
 *
 * extends the defined pcr using the hash calculated by the verified boot
 * routines.
 *
 * @param[in] pcr		PCR to extend
 * @param[in] *hashData		Pointer to the hash data
 * @param[in] hashDataLen	Length of the hash data
 * @param[in] *event_msg	Message to log or display
 * @param[in] eventType		Event type to use when logging

 * @retval TPM_SUCCESS		Operation completed successfully.
 * @retval TPM_E_IOERROR	Unexpected device behavior.
 */
static int measure_item(uint32_t pcr, uint8_t *hashData, uint32_t hashDataLen,
		int8_t *event_msg, TCG_EVENTTYPE eventType)
{
	int status = TPM_SUCCESS;
	EFI_TCG2_EVENT_ALGORITHM_BITMAP ActivePcrs;
	TCG_PCR_EVENT2_HDR tcgEventHdr;

	ActivePcrs = tpm2_get_active_pcrs();

	memset(&tcgEventHdr, 0, sizeof(tcgEventHdr));
	tcgEventHdr.pcrIndex = pcr;
	tcgEventHdr.eventType = eventType;
	if (event_msg) {
		status = mboot_hash_extend_log(ActivePcrs, MBOOT_HASH_PROVIDED,
			hashData, hashDataLen, &tcgEventHdr,
			(uint8_t *)event_msg, 0);
		if (status == TPM_SUCCESS) {
			printk(BIOS_DEBUG, "%s: Success! %s measured to pcr"
				"%d.\n", __func__, event_msg, pcr);
		} else {
			printk(BIOS_DEBUG, "%s: Fail! %s can't be measured. "
				"ABORTING!!!\n", __func__, event_msg);
			return status;
		}
	}
	return status;
}
#endif

static void verified_boot_check_buffer(const char *name, void *start,
		size_t size, uint32_t hash_index, int32_t pcr)
{
	uint8_t  digest[DIGEST_SIZE];
	int hash_algorithm;
	int status;
	printk(BIOS_DEBUG, "%s: %s HASH verification buffer %p size %d\n",
		__func__, name, start, (int) size);

	if (start && size) {
		if (CONFIG(VENDORCODE_ELTAN_VBOOT_USE_SHA512))
			hash_algorithm = VB2_HASH_SHA512;
		else
			hash_algorithm = VB2_HASH_SHA256;

		status = cb_sha_endian(hash_algorithm, (const uint8_t *)start,
				       size, digest, LITTLE_ENDIAN_ALGORITHM);

		if ((CONFIG(VENDORCODE_ELTAN_VBOOT) && memcmp((void *)(
		    (uint8_t *)CONFIG_VENDORCODE_ELTAN_OEM_MANIFEST_LOC +
		    sizeof(digest) * hash_index), digest, sizeof(digest))) ||
		    status) {
			printk(BIOS_DEBUG, "%s: buffer hash\n", __func__);
			hexdump(digest, sizeof(digest));
			printk(BIOS_DEBUG, "%s: manifest hash\n", __func__);
			hexdump((void *)(
			   (uint8_t *)CONFIG_VENDORCODE_ELTAN_OEM_MANIFEST_LOC +
			   sizeof(digest) * hash_index), sizeof(digest));
			printk(BIOS_EMERG, "%s ", name);
			die("HASH verification failed!\n");
		} else {
#ifndef __BOOTBLOCK__
			if (CONFIG(VENDORCODE_ELTAN_MBOOT)) {
				if (pcr != -1) {
					printk(BIOS_DEBUG, "%s: measuring %s\n",
						__func__, name);
					status = measure_item(pcr, digest,
							sizeof(digest),
							(int8_t *)name, 0);
				}
			}
#endif
			if (CONFIG(VENDORCODE_ELTAN_VBOOT))
				printk(BIOS_DEBUG, "%s HASH verification "
				       "success\n", name);
		}
	} else {
		printk(BIOS_EMERG, "Invalid buffer ");
		die("HASH verification failed!\n");
	}
}

void verified_boot_check_cbfsfile(const char *name, uint32_t type,
		uint32_t hash_index, void **buffer, uint32_t *filesize,
		int32_t pcr)
{
	void *start;
	size_t size;

	start = cbfs_boot_map_with_leak(name, type & ~VERIFIED_BOOT_COPY_BLOCK,
			&size);
	if (start && size) {
		/*
		 * Speed up processing by copying the file content to memory
		 * first
		 */
#ifndef __PRE_RAM__
		if ((type & VERIFIED_BOOT_COPY_BLOCK) && (buffer) &&
		    (*buffer) &&
		    ((uint32_t) start > (uint32_t)(~(CONFIG_CBFS_SIZE-1)))) {
				printk(BIOS_DEBUG, "%s: move buffer to "
					"memory\n", __func__);
			/* Move the file to a memory bufferof which we know it
			 * doesn't harm
			 */
			memcpy(*buffer, start, size);
			start = *buffer;
			printk(BIOS_DEBUG, "%s: done\n", __func__);
		}
#endif // __PRE_RAM__
		verified_boot_check_buffer(name, start, size, hash_index, pcr);
	} else {
		printk(BIOS_EMERG, "CBFS Failed to get file content for %s\n",
				name);
		die("HASH verification failed!\n");
	}
	if (buffer)
		*buffer = start;
	if (filesize)
		*filesize = size;
}

void process_verify_list(const verify_item_t list[])
{
	int i = 0;

	while (list[i].type != VERIFY_TERMINATOR) {
		switch (list[i].type) {
		case VERIFY_FILE:
			verified_boot_check_cbfsfile(list[i].name,
					list[i].data.file.cbfs_type,
					list[i].hash_index, NULL, NULL,
					list[i].pcr);
			if (list[i].data.file.related_items) {
				printk(BIOS_SPEW, "process related items\n");
				process_verify_list((verify_item_t *)
					list[i].data.file.related_items);
			}
			break;
		case VERIFY_BLOCK:
			verified_boot_check_buffer(list[i].name,
				(void *) list[i].data.block.start,
				list[i].data.block.size,
				list[i].hash_index, list[i].pcr);
			break;
		default:
			printk(BIOS_EMERG, "INVALID TYPE IN VERIFY"
				"LIST 0x%x\n", list[i].type);
			die("HASH verification failed!\n");
		}
		i++;
	}
}
#ifdef __BOOTBLOCK__
/*
 * BOOTBLOCK
 */

extern verify_item_t bootblock_verify_list[];

void verified_boot_bootblock_check(void)
{
	printk(BIOS_SPEW, "%s: processing bootblock items\n", __func__);

	if (CONFIG(VENDORCODE_ELTAN_VBOOT_SIGNED_MANIFEST)) {
		printk(BIOS_SPEW, "%s: check the manifest\n", __func__);
		if (verified_boot_check_manifest() != 0)
			die("invalid manifest");
	}
	printk(BIOS_SPEW, "%s: process bootblock verify list\n", __func__);
	process_verify_list(bootblock_verify_list);
}

static void vendor_secure_prepare(void)
{
	printk(BIOS_SPEW, "%s: bootblock\n", __func__);
	verified_boot_bootblock_check();
}
#endif //__BOOTBLOCK__

#ifdef __ROMSTAGE__
/*
 * ROMSTAGE
 */

extern verify_item_t romstage_verify_list[];

void verified_boot_early_check(void)
{
	printk(BIOS_SPEW, "%s: processing early items\n", __func__);

	if (!CONFIG(C_ENVIRONMENT_BOOTBLOCK) &&
	    CONFIG(VENDORCODE_ELTAN_VBOOT_SIGNED_MANIFEST)) {
		printk(BIOS_SPEW, "%s: check the manifest\n", __func__);
		if (verified_boot_check_manifest() != 0)
			die("invalid manifest");
	}

	if (CONFIG(VENDORCODE_ELTAN_MBOOT)) {
		printk(BIOS_DEBUG, "mb_measure returned 0x%x\n",
		mb_measure(vboot_platform_is_resuming()));
	}

	printk(BIOS_SPEW, "%s: process early verify list\n", __func__);
	process_verify_list(romstage_verify_list);
}

static int prepare_romstage = 0;

static void vendor_secure_prepare(void)
{
	printk(BIOS_SPEW, "%s: romstage\n", __func__);
	if (!prepare_romstage) {
		verified_boot_early_check();
		prepare_romstage = 1;
	}
}
#endif //__ROMSTAGE__

#ifdef __POSTCAR__
/*
 * POSTCAR
 */

extern verify_item_t postcar_verify_list[];

static void vendor_secure_prepare(void)
{
	printk(BIOS_SPEW, "%s: postcar\n", __func__);
	process_verify_list(postcar_verify_list);
}
#endif //__POSTCAR__

#ifdef __RAMSTAGE__
/*
 * RAM STAGE
 */

static int process_oprom_list(const verify_item_t list[],
		struct rom_header *rom_header)
{
	int i = 0;
	struct pci_data *rom_data;
	uint32_t viddevid = 0;

	if (le32_to_cpu(rom_header->signature) != PCI_ROM_HDR) {
		printk(BIOS_ERR, "Incorrect expansion ROM header "
			"signature %04x DONT START\n",
			le32_to_cpu(rom_header->signature));
		return 0;
	}

	rom_data = (((void *)rom_header) + le32_to_cpu(rom_header->data));

	viddevid |= (rom_data->vendor << 16);
	viddevid |= rom_data->device;

	while (list[i].type != VERIFY_TERMINATOR) {
		switch (list[i].type) {
		case VERIFY_OPROM:
			if (viddevid == list[i].data.oprom.viddev) {
				verified_boot_check_buffer(list[i].name,
					(void *) rom_header,
					rom_header->size * 512,
					list[i].hash_index, list[i].pcr);
				if (list[i].data.oprom.related_items) {
					printk(BIOS_SPEW, "%s: process"
						" related items\n", __func__);
					process_verify_list((verify_item_t *)list[i].data.oprom.related_items);
				}
				printk(BIOS_SPEW, "%s: option rom can be"
					" started\n", __func__);
				return 1;
			}
			break;
		default:
			printk(BIOS_EMERG, "%s: INVALID TYPE IN OPTION ROM LIST"
				"0x%x\n", __func__, list[i].type);
			die("HASH verification failed!\n");
		}
		i++;
	}
	printk(BIOS_ERR, "%s: option rom not in list DONT START\n", __func__);
	return 0;
}

extern verify_item_t payload_verify_list[];

extern verify_item_t oprom_verify_list[];

int verified_boot_should_run_oprom(struct rom_header *rom_header)
{
	return process_oprom_list(oprom_verify_list, rom_header);
}

static void vendor_secure_prepare(void)
{
	printk(BIOS_SPEW, "%s: ramstage\n", __func__);
	process_verify_list(payload_verify_list);
}
#endif //__RAMSTAGE__

const struct cbfs_locator cbfs_master_header_locator = {
	.name = "Vendorcode Header Locator",
	.prepare = vendor_secure_prepare,
	.locate = vendor_secure_locate
};
