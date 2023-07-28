/* SPDX-License-Identifier: GPL-2.0-only */

#define __SIMPLE_DEVICE__

#include <stdlib.h>
#include <console/console.h>
#include <device/device.h>
#include <device/mmio.h>
#include <device/pci_def.h>
#include <device/pci_ops.h>
#include <fmap.h>
#include <intelblocks/cse.h>
#include <intelblocks/systemagent.h>
#include <intelblocks/vtd.h>
#include <security/vboot/misc.h>
#include <soc/hsphy.h>
#include <soc/iomap.h>
#include <soc/pci_devs.h>
#include <vb2_api.h>
#include <lib.h>

#define HASHALG_SHA1		0x00000001
#define HASHALG_SHA256		0x00000002
#define HASHALG_SHA384		0x00000003
#define HASHALG_SHA512		0x00000004

#define MAX_HASH_SIZE		VB2_SHA512_DIGEST_SIZE
#define GET_IP_FIRMWARE_CMD	0x21
#define HSPHY_PAYLOAD_SIZE	(32*KiB)

#define CPU_PID_PCIE_PHYX16_BROADCAST	0x55

struct hsphy_cache {
	uint32_t hsphy_size;
	uint8_t hash_algo;
	uint8_t digest[MAX_HASH_SIZE];
	uint8_t hsphy_fw[];
} __packed;

struct ip_push_model {
	uint16_t count;
	uint16_t address;
	uint32_t data[];
} __packed;

static int heci_get_hsphy_payload(void *buf, uint32_t *buf_size, uint8_t *hash_buf,
				  uint8_t *hash_alg, uint32_t *status)
{
	size_t reply_size;

	struct heci_ip_load_request {
		struct mkhi_hdr hdr;
		uint32_t version;
		uint32_t operation;
		uint32_t dram_base_low;
		uint32_t dram_base_high;
		uint32_t memory_size;
		uint32_t reserved;
	} __packed msg = {
		.hdr = {
			.group_id = MKHI_GROUP_ID_BUP_COMMON,
			.command = GET_IP_FIRMWARE_CMD,
		},
		.version = 1,
		.operation = 1,
		.dram_base_low = (uintptr_t)buf,
		.dram_base_high = 0,
		.memory_size = *buf_size,
		.reserved = 0,
	};

	struct heci_ip_load_response {
		struct mkhi_hdr hdr;
		uint32_t payload_size;
		uint32_t reserved[2];
		uint32_t status;
		uint8_t hash_type;
		uint8_t hash[MAX_HASH_SIZE];
	} __packed reply;

	if (!buf || !buf_size || !hash_buf || !hash_alg) {
		printk(BIOS_ERR, "%s: Invalid parameters\n", __func__);
		return -1;
	}

	reply_size = sizeof(reply);
	memset(&reply, 0, reply_size);

	printk(BIOS_DEBUG, "HECI: Sending Get IP firmware command\n");

	if (heci_send_receive(&msg, sizeof(msg), &reply, &reply_size, HECI_MKHI_ADDR)) {
		printk(BIOS_ERR, "HECI: Get IP firmware failed\n");
		return -1;
	}

	if (reply.hdr.result) {
		printk(BIOS_ERR, "HECI: Get IP firmware response invalid\n");
		*status = reply.status;
		printk(BIOS_DEBUG, "HECI response:\n");
		hexdump(&reply, sizeof(reply));
		return -1;
	}

	*buf_size = reply.payload_size;
	*hash_alg = reply.hash_type;
	*status = reply.status;
	memcpy(hash_buf, reply.hash, MAX_HASH_SIZE);

	printk(BIOS_DEBUG, "HECI: Get IP firmware success. Response:\n");
	printk(BIOS_DEBUG, "  Payload size = 0x%x\n", *buf_size);
	printk(BIOS_DEBUG, "  Hash type used for signing payload = 0x%x\n", *hash_alg);

	return 0;
}

static bool verify_hsphy_hash(void *buf, uint32_t buf_size, uint8_t *hash_buf, uint8_t hash_alg)
{
	struct vb2_hash hash;

	switch (hash_alg) {
	case HASHALG_SHA256:
		hash.algo = VB2_HASH_SHA256;
		break;
	case HASHALG_SHA384:
		hash.algo = VB2_HASH_SHA384;
		break;
	case HASHALG_SHA512:
		hash.algo = VB2_HASH_SHA512;
		break;
	case HASHALG_SHA1:
	default:
		printk(BIOS_ERR, "Hash alg %d not supported, trying SHA384\n", hash_alg);
		hash.algo = VB2_HASH_SHA384;
		break;
	}

	memcpy(hash.raw, hash_buf, vb2_digest_size(hash.algo));

	if (vb2_hash_verify(vboot_hwcrypto_allowed(), buf, buf_size, &hash) != VB2_SUCCESS)
		return false;

	return true;
}

static void upload_hsphy_to_cpu_pcie(void *buf, uint32_t buf_size)
{
	uint16_t i = 0, j;
	struct ip_push_model *push_model = (struct ip_push_model *)buf;

	while (i < buf_size) {
		i += sizeof(*push_model);

		if ((push_model->address == 0) && (push_model->count == 0))
			break; // End of file

		for (j = 0; j < push_model->count; j++) {
			REGBAR32(CPU_PID_PCIE_PHYX16_BROADCAST,
				 push_model->address) = push_model->data[j];
			i += sizeof(uint32_t);
		}

		push_model = (struct ip_push_model *)(buf + i);
	}
}

static bool hsphy_cache_valid(struct hsphy_cache *hsphy_fw_cache)
{
	if (!hsphy_fw_cache) {
		printk(BIOS_WARNING, "Failed to mmap HSPHY cache\n");
		return false;
	}

	if (hsphy_fw_cache->hsphy_size == 0 ||
	    hsphy_fw_cache->hsphy_size > HSPHY_PAYLOAD_SIZE ||
	    hsphy_fw_cache->hash_algo <= HASHALG_SHA1 ||
	    hsphy_fw_cache->hash_algo > HASHALG_SHA512)
		return false;

	if (!verify_hsphy_hash(hsphy_fw_cache->hsphy_fw, hsphy_fw_cache->hsphy_size,
			       hsphy_fw_cache->digest, hsphy_fw_cache->hash_algo))
		return false;

	return true;
}

static bool load_hsphy_from_cache(void)
{
	struct region_device rdev;
	struct hsphy_cache *hsphy_fw_cache;

	if (fmap_locate_area_as_rdev("HSPHY_FW", &rdev) < 0) {
		printk(BIOS_ERR, "HSPHY: Cannot find HSPHY_FW region\n");
		return false;
	}

	hsphy_fw_cache = (struct hsphy_cache *)rdev_mmap_full(&rdev);

	if (!hsphy_cache_valid(hsphy_fw_cache)) {
		printk(BIOS_ERR, "HSPHY: HSPHY cache invalid\n");
		if (hsphy_fw_cache)
			rdev_munmap(&rdev, hsphy_fw_cache);
		return false;
	}

	printk(BIOS_INFO, "Loading HSPHY FW from cache\n");
	upload_hsphy_to_cpu_pcie(hsphy_fw_cache->hsphy_fw, hsphy_fw_cache->hsphy_size);

	rdev_munmap(&rdev, hsphy_fw_cache);

	return true;
}

static void cache_hsphy_fw_in_flash(void *buf, uint32_t buf_size, uint8_t *hash_buf,
				   uint8_t hash_alg)
{
	struct region_device rdev;
	struct hsphy_cache *hsphy_fw_cache;
	size_t ret;

	if (!buf || buf_size == 0 || buf_size > (HSPHY_PAYLOAD_SIZE - sizeof(*hsphy_fw_cache))
	    || !hash_buf || hash_alg <= HASHALG_SHA1 || hash_alg > HASHALG_SHA512) {
		printk(BIOS_ERR, "Invalid parameters, HSPHY will not be cached in flash.\n");
		return;
	}

	/* Locate the area as RO rdev, otherwise mmap will fail */
	if (fmap_locate_area_as_rdev("HSPHY_FW", &rdev) < 0) {
		printk(BIOS_ERR, "HSPHY: Could not find HSPHY_FW region\n");
		printk(BIOS_ERR, "HSPHY will not be cached in flash\n");
		return;
	}

	hsphy_fw_cache = (struct hsphy_cache *)rdev_mmap_full(&rdev);

	if (hsphy_cache_valid(hsphy_fw_cache)) {
		/* If the cache is valid, check the buffer against the cache hash */
		if (verify_hsphy_hash(buf, buf_size, hsphy_fw_cache->digest,
				      hsphy_fw_cache->hash_algo)) {
			printk(BIOS_INFO, "HSPHY: cache does not need update\n");
			rdev_munmap(&rdev, hsphy_fw_cache);
			return;
		} else {
			printk(BIOS_INFO, "HSPHY: cache needs update\n");
		}
	} else {
		printk(BIOS_INFO, "HSPHY: cache invalid, updating\n");
	}

	if (region_device_sz(&rdev) < (buf_size + sizeof(*hsphy_fw_cache))) {
		printk(BIOS_ERR, "HSPHY: HSPHY_FW region too small: %zx < %zx\n",
		       region_device_sz(&rdev), buf_size + sizeof(*hsphy_fw_cache));
		printk(BIOS_ERR, "HSPHY will not be cached in flash\n");
		rdev_munmap(&rdev, hsphy_fw_cache);
		return;
	}

	rdev_munmap(&rdev, hsphy_fw_cache);
	hsphy_fw_cache = malloc(sizeof(*hsphy_fw_cache));

	if (!hsphy_fw_cache) {
		printk(BIOS_ERR, "HSPHY: Could not allocate memory for HSPHY cache buffer\n");
		printk(BIOS_ERR, "HSPHY will not be cached in flash\n");
		return;
	}

	hsphy_fw_cache->hsphy_size = buf_size;
	hsphy_fw_cache->hash_algo = hash_alg;

	switch (hash_alg) {
	case HASHALG_SHA256:
		hash_alg = VB2_HASH_SHA256;
		break;
	case HASHALG_SHA384:
		hash_alg = VB2_HASH_SHA384;
		break;
	case HASHALG_SHA512:
		hash_alg = VB2_HASH_SHA512;
		break;
	}

	memset(hsphy_fw_cache->digest, 0, sizeof(hsphy_fw_cache->digest));
	memcpy(hsphy_fw_cache->digest, hash_buf, vb2_digest_size(hash_alg));

	/* Now that we want to write to flash, locate the area as RW rdev */
	if (fmap_locate_area_as_rdev_rw("HSPHY_FW", &rdev) < 0) {
		printk(BIOS_ERR, "HSPHY: Could not find HSPHY_FW region\n");
		printk(BIOS_ERR, "HSPHY will not be cached in flash\n");
		free(hsphy_fw_cache);
		return;
	}

	if (rdev_eraseat(&rdev, 0, region_device_sz(&rdev)) < 0) {
		printk(BIOS_ERR, "Failed to erase HSPHY cache region\n");
		free(hsphy_fw_cache);
		return;
	}

	ret = rdev_writeat(&rdev, hsphy_fw_cache, 0, sizeof(*hsphy_fw_cache));
	if (ret != sizeof(*hsphy_fw_cache)) {
		printk(BIOS_ERR, "Failed to write HSPHY cache metadata\n");
		free(hsphy_fw_cache);
		return;
	}

	ret = rdev_writeat(&rdev, buf, sizeof(*hsphy_fw_cache), buf_size);
	if (ret != buf_size) {
		printk(BIOS_ERR, "Failed to write HSPHY FW to cache\n");
		free(hsphy_fw_cache);
		return;
	}

	printk(BIOS_INFO, "HSPHY cached to flash successfully\n");

	free(hsphy_fw_cache);
}

static void *allocate_hsphy_buf(void)
{
	void *hsphy_buf;
	size_t dma_buf_size;

	if (CONFIG(ENABLE_EARLY_DMA_PROTECTION)) {
		hsphy_buf = vtd_get_dma_buffer(&dma_buf_size);
		if (!hsphy_buf || dma_buf_size < HSPHY_PAYLOAD_SIZE) {
			printk(BIOS_ERR, "DMA protection enabled but DMA buffer does not"
					 " exist or is too small\n");
			return NULL;
		}

		/* Rather impossible scenario, but check alignment anyways */
		if (!IS_ALIGNED((uintptr_t)hsphy_buf, 4 * KiB) &&
		    (HSPHY_PAYLOAD_SIZE + 4 * KiB) <= dma_buf_size)
			hsphy_buf = (void *)ALIGN_UP((uintptr_t)hsphy_buf, 4 * KiB);
	} else {
		/* Align the buffer to page size, otherwise the HECI command will fail */
		hsphy_buf = memalign(4 * KiB, HSPHY_PAYLOAD_SIZE);

		if (!hsphy_buf) {
			printk(BIOS_ERR, "Failed to allocate memory for HSPHY blob\n");
			return NULL;
		}
	}

	return hsphy_buf;
}

void load_and_init_hsphy(void)
{
	void *hsphy_buf;
	uint8_t hsphy_hash[MAX_HASH_SIZE] = { 0 };
	uint8_t hash_type;
	uint32_t buf_size = HSPHY_PAYLOAD_SIZE;
	pci_devfn_t dev = PCH_DEV_CSE;
	const uint16_t pci_cmd_bme_mem = PCI_COMMAND_MASTER | PCI_COMMAND_MEMORY;
	uint32_t status;

	if (!is_devfn_enabled(SA_DEVFN_CPU_PCIE1_0) &&
	    !is_devfn_enabled(SA_DEVFN_CPU_PCIE1_1)) {
		printk(BIOS_DEBUG, "All HSPHY ports disabled, skipping HSPHY loading\n");
		return;
	}

	/*
	 * Try to get HSPHY payload from CSME first, so we can always keep our
	 * HSPHY cache up to date. If we cannot allocate the buffer for it, the
	 * cache is our last resort.
	 */
	hsphy_buf = allocate_hsphy_buf();
	if (!hsphy_buf) {
		printk(BIOS_ERR, "Could not allocate memory for HSPHY blob\n");
		if (CONFIG(INCLUDE_HSPHY_IN_FMAP)) {
			printk(BIOS_INFO, "Trying to load HSPHY FW from cache\n");
			if (load_hsphy_from_cache()) {
				printk(BIOS_INFO, "Successfully loaded HSPHY FW from cache\n");
				return;
			}
			printk(BIOS_ERR, "Failed to load HSPHY FW from cache\n");
		}
		printk(BIOS_ERR, "Aborting HSPHY FW loading, PCIe Gen5 won't work.\n");
		return;
	}

	memset(hsphy_buf, 0, HSPHY_PAYLOAD_SIZE);

	/*
	 * If CSME is not present, try cached HSPHY FW. We still want to use
	 * CSME just in case CSME is updated along with HSPHY FW, so that we
	 * can update our cache if needed.
	 */
	if (!is_cse_enabled()) {
		if (CONFIG(INCLUDE_HSPHY_IN_FMAP)) {
			printk(BIOS_INFO, "Trying to load HSPHY FW from cache"
					  " because CSME is not enabled or not visible\n");
			if (load_hsphy_from_cache()) {
				printk(BIOS_INFO, "Successfully loaded HSPHY FW from cache\n");
				return;
			}
			printk(BIOS_ERR, "Failed to load HSPHY FW from cache\n");
		}
		printk(BIOS_ERR, "%s: CSME not enabled or not visible, but required\n",
		       __func__);
		printk(BIOS_ERR, "Aborting HSPHY FW loading, PCIe Gen5 won't work.\n");
		if (!CONFIG(ENABLE_EARLY_DMA_PROTECTION))
			free(hsphy_buf);
		return;
	}

	/* Ensure BAR, BME and memory space are enabled */
	if ((pci_read_config16(dev, PCI_COMMAND) & pci_cmd_bme_mem) != pci_cmd_bme_mem)
		pci_or_config16(dev, PCI_COMMAND, pci_cmd_bme_mem);


	if (pci_read_config32(dev, PCI_BASE_ADDRESS_0) == 0) {
		pci_and_config16(dev, PCI_COMMAND, ~pci_cmd_bme_mem);
		pci_write_config32(dev, PCI_BASE_ADDRESS_0, HECI1_BASE_ADDRESS);
		pci_or_config16(dev, PCI_COMMAND, pci_cmd_bme_mem);
	}

	/* Try to get HSPHY payload from CSME and cache it if possible. */
	if (!heci_get_hsphy_payload(hsphy_buf, &buf_size, hsphy_hash, &hash_type, &status)) {
		if (verify_hsphy_hash(hsphy_buf, buf_size, hsphy_hash, hash_type)) {
			upload_hsphy_to_cpu_pcie(hsphy_buf, buf_size);
			if (CONFIG(INCLUDE_HSPHY_IN_FMAP))
				cache_hsphy_fw_in_flash(hsphy_buf, buf_size, hsphy_hash,
							hash_type);

			if (!CONFIG(ENABLE_EARLY_DMA_PROTECTION))
				free(hsphy_buf);
			return;
		} else {
			printk(BIOS_ERR, "Failed to verify HSPHY FW hash.\n");
		}
	} else {
		printk(BIOS_ERR, "Failed to get HSPHY FW over HECI.\n");
	}

	if (!CONFIG(ENABLE_EARLY_DMA_PROTECTION))
		free(hsphy_buf);

	/* We failed to get HSPHY payload from CSME, cache is our last chance. */
	if (CONFIG(INCLUDE_HSPHY_IN_FMAP) && load_hsphy_from_cache()) {
		printk(BIOS_INFO, "Successfully loaded HSPHY FW from cache\n");
		return;
	}

	printk(BIOS_ERR, "Failed to load HSPHY FW, PCIe Gen5 won't work.\n");
}
