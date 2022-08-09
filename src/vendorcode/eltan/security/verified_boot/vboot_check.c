/* SPDX-License-Identifier: GPL-2.0-only */

#include <boot_device.h>
#include <bootmem.h>
#include <bootmode.h>
#include <cbfs.h>
#include <fmap_config.h>
#include <vboot_check.h>
#include <vboot_common.h>
#include <vb2_internals_please_do_not_use.h>

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
	struct vb2_context *ctx;
	struct vb2_kernel_preamble *pre;
	static struct vb2_shared_data *sd;
	size_t size;
	uint8_t wb_buffer[3000];

	if (vb2api_init(&wb_buffer, sizeof(wb_buffer), &ctx)) {
		goto fail;
	}

	sd = vb2_get_sd(ctx);

	buffer = cbfs_map(RSA_PUBLICKEY_FILE_NAME, &size);
	if (!buffer || !size) {
		printk(BIOS_ERR, "Public key not found!\n");
		goto fail;
	}

	if ((size != CONFIG_VENDORCODE_ELTAN_VBOOT_KEY_SIZE) ||
	    (buffer != (void *)CONFIG_VENDORCODE_ELTAN_VBOOT_KEY_LOCATION)) {
		printk(BIOS_ERR, "Illegal public key!\n");
		goto fail;
	}

	/*
	 * Check if all items will fit into workbuffer:
	 * vb2_shared data, Public Key, Preamble data
	 */
	if ((sd->workbuf_used + size + sizeof(struct vb2_kernel_preamble) +
	    ((CONFIG_VENDORCODE_ELTAN_OEM_MANIFEST_ITEMS * DIGEST_SIZE) + (2048/8))) >
	    sizeof(wb_buffer)) {
		printk(BIOS_ERR, "Work buffer too small\n");
		goto fail;
	}

	/* Add public key */
	sd->data_key_offset = sd->workbuf_used;
	sd->data_key_size = size;
	sd->workbuf_used += sd->data_key_size;
	memcpy((void *)((void *)sd + (long)sd->data_key_offset), (uint8_t *)buffer, size);

	/* Fill preamble area */
	sd->preamble_size = sizeof(struct vb2_kernel_preamble);
	sd->preamble_offset = sd->data_key_offset + sd->data_key_size;
	sd->workbuf_used += sd->preamble_size;
	pre = (struct vb2_kernel_preamble *)((void *)sd + (long)sd->preamble_offset);

	pre->flags = VB2_FIRMWARE_PREAMBLE_DISALLOW_HWCRYPTO;

	/* Fill body_signature (vb2_structure). RSA2048 key is used */
	cbfs_map("oemmanifest.bin", &size);
	if (size != ((CONFIG_VENDORCODE_ELTAN_OEM_MANIFEST_ITEMS * DIGEST_SIZE) + (2048/8))) {
		printk(BIOS_ERR, "Incorrect manifest size!\n");
		goto fail;
	}
	pre->body_signature.data_size = CONFIG_VENDORCODE_ELTAN_OEM_MANIFEST_ITEMS *
					DIGEST_SIZE;
	pre->body_signature.sig_offset = sizeof(struct vb2_signature) +
					 pre->body_signature.data_size;
	pre->body_signature.sig_size = size - pre->body_signature.data_size;
	sd->workbuf_used += size;
	memcpy((void *)((void *)&pre->body_signature + (long)sizeof(struct vb2_signature)),
	       (uint8_t *)CONFIG_VENDORCODE_ELTAN_OEM_MANIFEST_LOC, size);


	if (vb2api_verify_kernel_data(ctx, (void *)CONFIG_VENDORCODE_ELTAN_OEM_MANIFEST_LOC,
				      pre->body_signature.data_size))
		goto fail;

	printk(BIOS_INFO, "%s: Successfully verified hash_table signature.\n", __func__);
	return 0;

fail:
	die("ERROR: HASH table verification failed!\n");
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
	vb2_error_t status;

	printk(BIOS_DEBUG, "%s: %s HASH verification buffer %p size %d\n", __func__, name,
	       start, (int)size);

	if (start && size) {
		struct vb2_hash tmp_hash;

		status = vb2_hash_calculate(false, start, size, HASH_ALG, &tmp_hash);
		if (!status)
			memcpy(digest, tmp_hash.raw, DIGEST_SIZE);

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

#if FMAP_SECTION_COREBOOT_START < (0xffffffff - CONFIG_ROM_SIZE + 1)
#define COREBOOT_CBFS_START (0xffffffff - CONFIG_ROM_SIZE + 1 + FMAP_SECTION_COREBOOT_START)
#else
#define COREBOOT_CBFS_START FMAP_SECTION_COREBOOT_START
#endif

void verified_boot_check_cbfsfile(const char *name, uint32_t type, uint32_t hash_index,
				  void **buffer, uint32_t *filesize, int32_t pcr)
{
	void *start;
	size_t size;

	start = cbfs_map(name, &size);
	if (start && size) {
		/* Speed up processing by copying the file content to memory first */
		if (!ENV_ROMSTAGE_OR_BEFORE && (type & VERIFIED_BOOT_COPY_BLOCK)) {

			if ((buffer) && (*buffer) && (*filesize >= size) &&
			    ((uint32_t) start > COREBOOT_CBFS_START)) {

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

	if (CONFIG(VENDORCODE_ELTAN_MBOOT)) {
		printk(BIOS_DEBUG, "mb_measure returned 0x%x\n",
		mb_measure(platform_is_resuming()));
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
	static int initialized;

	if (ENV_BOOTBLOCK)
		verified_boot_bootblock_check();

	if (ENV_ROMSTAGE) {
		if (!initialized && ((prog->type == PROG_REFCODE) ||
		    (prog->type == PROG_POSTCAR))) {
			verified_boot_early_check();
			initialized = 1;
		}

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
