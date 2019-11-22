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
#include <bootmem.h>
#include <cbfs.h>
#include <vboot_check.h>
#include <vboot_common.h>

#define RSA_PUBLICKEY_FILE_NAME "vboot_public_key.bin"

#if CONFIG(VENDORCODE_ELTAN_VBOOT_USE_SHA512)
#define DIGEST_SIZE VB2_SHA512_DIGEST_SIZE
#define HASH_ALG VB2_HASH_SHA512
#else
#define DIGEST_SIZE VB2_SHA256_DIGEST_SIZE
#define HASH_ALG VB2_HASH_SHA256
#endif

int verified_boot_check_manifest(void)
{
	uint8_t *buffer;
	uint8_t sig_buffer[1024]; /* used to build vb21_signature */
	size_t size = 0;
	struct vb2_public_key key;
	struct vb2_workbuf wb;
	struct vb21_signature *vb2_sig_hdr = (struct vb21_signature *)sig_buffer;
	uint8_t wb_buffer[1024];

	buffer = cbfs_boot_map_with_leak(RSA_PUBLICKEY_FILE_NAME, CBFS_TYPE_RAW, &size);
	if (!buffer || !size) {
		printk(BIOS_ERR, "ERROR: Public key not found!\n");
		goto fail;
	}

	if ((size != CONFIG_VENDORCODE_ELTAN_VBOOT_KEY_SIZE) ||
			(buffer != (void *)CONFIG_VENDORCODE_ELTAN_VBOOT_KEY_LOCATION)) {
		printk(BIOS_ERR, "ERROR: Illegal public key!\n");
		goto fail;
	}

	if (vb21_unpack_key(&key, buffer, size)) {
		printk(BIOS_ERR, "ERROR: Invalid public key!\n");
		goto fail;
	}

	cbfs_boot_map_with_leak("oemmanifest.bin", CBFS_TYPE_RAW, &size);
	if (size != (CONFIG_VENDORCODE_ELTAN_OEM_MANIFEST_ITEMS * DIGEST_SIZE) +
			      vb2_rsa_sig_size(VB2_SIG_RSA2048)) {
		printk(BIOS_ERR, "ERROR: Incorrect manifest size!\n");
		goto fail;
	}

	/* prepare work buffer structure */
	wb.buf = (uint8_t *)&wb_buffer;
	wb.size = sizeof(wb_buffer);

	/* Build vb2_sig_hdr buffer */
	vb2_sig_hdr->sig_offset = sizeof(struct vb21_signature) +
				 (CONFIG_VENDORCODE_ELTAN_OEM_MANIFEST_ITEMS * DIGEST_SIZE);
	vb2_sig_hdr->sig_alg = VB2_SIG_RSA2048;
	vb2_sig_hdr->sig_size = vb2_rsa_sig_size(VB2_SIG_RSA2048);
	vb2_sig_hdr->hash_alg = HASH_ALG;
	vb2_sig_hdr->data_size = CONFIG_VENDORCODE_ELTAN_OEM_MANIFEST_ITEMS * DIGEST_SIZE;
	memcpy(&sig_buffer[sizeof(struct vb21_signature)],
	       (uint8_t *)CONFIG_VENDORCODE_ELTAN_OEM_MANIFEST_LOC, size);

	if (vb21_verify_data(&sig_buffer[sizeof(struct vb21_signature)], vb2_sig_hdr->data_size,
			     (struct vb21_signature *)&sig_buffer, &key, &wb)) {
		printk(BIOS_ERR, "ERROR: Signature verification failed for hash table\n");
		goto fail;
	}

	printk(BIOS_INFO, "%s: Successfully verified hash_table signature.\n", __func__);
	return 0;

fail:
	die("HASH table verification failed!\n");
	return -1;
}

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
	TCG_PCR_EVENT2_HDR tcgEventHdr;

	memset(&tcgEventHdr, 0, sizeof(tcgEventHdr));
	tcgEventHdr.pcrIndex = pcr;
	tcgEventHdr.eventType = eventType;
	if (event_msg) {
		status = mboot_hash_extend_log(MBOOT_HASH_PROVIDED, hashData,
					       hashDataLen, &tcgEventHdr,
					       (uint8_t *)event_msg);
		if (status == TPM_SUCCESS)
			printk(BIOS_INFO, "%s: Success! %s measured to pcr %d.\n", __func__,
			       event_msg, pcr);
	}
	return status;
}

static void verified_boot_check_buffer(const char *name, void *start, size_t size,
				       uint32_t hash_index, int32_t pcr)
{
	uint8_t  digest[DIGEST_SIZE];
	int hash_algorithm;
	vb2_error_t status;

	printk(BIOS_DEBUG, "%s: %s HASH verification buffer %p size %d\n", __func__, name,
	       start, (int)size);

	if (start && size) {
		if (CONFIG(VENDORCODE_ELTAN_VBOOT_USE_SHA512))
			hash_algorithm = VB2_HASH_SHA512;
		else
			hash_algorithm = VB2_HASH_SHA256;

		status = cb_sha_little_endian(hash_algorithm, (const uint8_t *)start, size,
					      digest);
		if ((CONFIG(VENDORCODE_ELTAN_VBOOT) && memcmp((void *)(
		    (uint8_t *)CONFIG_VENDORCODE_ELTAN_OEM_MANIFEST_LOC +
		    sizeof(digest) * hash_index), digest, sizeof(digest))) || status) {
			printk(BIOS_DEBUG, "%s: buffer hash\n", __func__);
			hexdump(digest, sizeof(digest));
			printk(BIOS_DEBUG, "%s: manifest hash\n", __func__);
			hexdump((void *)( (uint8_t *)CONFIG_VENDORCODE_ELTAN_OEM_MANIFEST_LOC +
				 sizeof(digest) * hash_index), sizeof(digest));
			printk(BIOS_EMERG, "%s ", name);
			die("HASH verification failed!\n");
		} else {
			if (!ENV_BOOTBLOCK && CONFIG(VENDORCODE_ELTAN_MBOOT)) {
				if (pcr != -1) {
					printk(BIOS_DEBUG, "%s: measuring %s\n", __func__,
					       name);
					if (measure_item(pcr, digest, sizeof(digest),
							 (int8_t *)name, 0))
						printk(BIOS_DEBUG, "%s: measuring failed!\n",
						       __func__);
				}
			}
			if (CONFIG(VENDORCODE_ELTAN_VBOOT))
				printk(BIOS_DEBUG, "%s HASH verification success\n", name);
		}
	} else {
		printk(BIOS_EMERG, "Invalid buffer\n");
		die("HASH verification failed!\n");
	}
}

void verified_boot_check_cbfsfile(const char *name, uint32_t type, uint32_t hash_index,
				  void **buffer, uint32_t *filesize, int32_t pcr)
{
	void *start;
	size_t size;

	start = cbfs_boot_map_with_leak(name, type & ~VERIFIED_BOOT_COPY_BLOCK, &size);
	if (start && size) {
		/* Speed up processing by copying the file content to memory first */
		if (!ENV_ROMSTAGE_OR_BEFORE && (type & VERIFIED_BOOT_COPY_BLOCK)) {

			if ((buffer) && (*buffer) && (*filesize >= size) &&
			    ((uint32_t) start > (uint32_t)(~(CONFIG_CBFS_SIZE-1)))) {

				/* Use the buffer passed in if possible */
				printk(BIOS_DEBUG, "%s: move buffer to memory\n", __func__);
				/* Move the file to memory buffer passed in */
				memcpy(*buffer, start, size);
				start = *buffer;
				printk(BIOS_DEBUG, "%s: done\n", __func__);

			} else if (ENV_RAMSTAGE) {
				/* Try to allocate a buffer from boot_mem */
				void *local_buffer = bootmem_allocate_buffer(size);

				if (local_buffer) {

					/* Use the allocated buffer */
					printk(BIOS_DEBUG, "%s: move file to memory\n",
					       __func__);
					memcpy(local_buffer, start, size);
					start = local_buffer;
					printk(BIOS_DEBUG, "%s: done\n", __func__);
				}
			}
		}
		verified_boot_check_buffer(name, start, size, hash_index, pcr);
	} else {
		printk(BIOS_EMERG, "CBFS Failed to get file content for %s\n", name);
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
 			verified_boot_check_cbfsfile(list[i].name, list[i].data.file.cbfs_type,
					list[i].hash_index, NULL, NULL, list[i].pcr);
			if (list[i].data.file.related_items) {
				printk(BIOS_SPEW, "process related items\n");
				process_verify_list(
				        (verify_item_t *)list[i].data.file.related_items);
			}
			break;
		case VERIFY_BLOCK:
			verified_boot_check_buffer(list[i].name,
						   (void *)list[i].data.block.start,
						   list[i].data.block.size,
						   list[i].hash_index, list[i].pcr);
			break;
		default:
			printk(BIOS_EMERG, "INVALID TYPE IN VERIFY LIST 0x%x\n", list[i].type);
			die("HASH verification failed!\n");
		}
		i++;
	}
}

/*
 * BOOTBLOCK
 */

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

/*
 * ROMSTAGE
 */

void verified_boot_early_check(void)
{
	printk(BIOS_SPEW, "%s: processing early items\n", __func__);

	if (CONFIG(ROMCC_BOOTBLOCK) &&
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
		printk(BIOS_ERR, "Incorrect expansion ROM header signature %04x DONT START\n",
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
							   (void *)rom_header,
							   rom_header->size * 512,
							   list[i].hash_index, list[i].pcr);
				if (list[i].data.oprom.related_items) {
					printk(BIOS_SPEW, "%s: process related items\n",
					       __func__);
					process_verify_list(
					    (verify_item_t *)list[i].data.oprom.related_items);
				}
				printk(BIOS_SPEW, "%s: option rom can be started\n", __func__);
				return 1;
			}
			break;
		default:
			printk(BIOS_EMERG, "%s: INVALID TYPE IN OPTION ROM LIST 0x%x\n",
			       __func__, list[i].type);
			die("HASH verification failed!\n");
		}
		i++;
	}
	printk(BIOS_ERR, "%s: option rom not in list DONT START\n", __func__);
	return 0;
}

int verified_boot_should_run_oprom(struct rom_header *rom_header)
{
	return process_oprom_list(oprom_verify_list, rom_header);
}

int prog_locate_hook(struct prog *prog)
{
	if (ENV_BOOTBLOCK)
		verified_boot_bootblock_check();

	if (ENV_ROMSTAGE) {
		if (prog->type == PROG_REFCODE)
			verified_boot_early_check();

		if (CONFIG(POSTCAR_STAGE) && prog->type == PROG_POSTCAR)
			process_verify_list(postcar_verify_list);

		if (!CONFIG(POSTCAR_STAGE) && prog->type == PROG_RAMSTAGE)
			process_verify_list(ramstage_verify_list);
	}

	if (ENV_POSTCAR && prog->type == PROG_RAMSTAGE)
		process_verify_list(ramstage_verify_list);

	if (ENV_RAMSTAGE && prog->type == PROG_PAYLOAD)
		process_verify_list(payload_verify_list);

	return 0;
}
