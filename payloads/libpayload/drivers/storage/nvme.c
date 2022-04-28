// SPDX-License-Identifier: BSD-3-Clause
/*
 * Libpayload NVMe device driver
 * Copyright (C) 2019 secunet Security Networks AG
 */

#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <pci.h>
#include <pci/pci.h>
#include <libpayload.h>
#include <storage/storage.h>
#include <storage/nvme.h>

#define NVME_CC_EN	(1 <<  0)
#define NVME_CC_CSS	(0 <<  4)
#define NVME_CC_MPS	(0 <<  7)
#define NVME_CC_AMS	(0 << 11)
#define NVME_CC_SHN	(0 << 14)
#define NVME_CC_IOSQES	(6 << 16)
#define NVME_CC_IOCQES	(4 << 20)

#define NVME_QUEUE_SIZE 2
#define NVME_SQ_ENTRY_SIZE 64
#define NVME_CQ_ENTRY_SIZE 16

struct nvme_dev {
	storage_dev_t storage_dev;

	pcidev_t pci_dev;
	void *config;
	struct {
		void *base;
		uint32_t *bell;
		uint16_t idx; // bool pos 0 or 1
		uint16_t round; // bool round 0 or 1+0xd
	} queue[4];

	uint64_t *prp_list;
};


struct nvme_s_queue_entry {
	uint32_t dw[16];
};

struct nvme_c_queue_entry {
	uint32_t dw[4];
};

enum nvme_queue {
	NVME_ADMIN_QUEUE = 0,
	ads = 0,
	adc = 1,
	NVME_IO_QUEUE = 2,
	ios = 2,
	ioc = 3,
};

static storage_poll_t nvme_poll(struct storage_dev *dev)
{
	return POLL_MEDIUM_PRESENT;
}

static int nvme_cmd(
		struct nvme_dev *nvme, enum nvme_queue q, const struct nvme_s_queue_entry *cmd)
{
	int sq = q, cq = q+1;

	void *s_entry = nvme->queue[sq].base + (nvme->queue[sq].idx * NVME_SQ_ENTRY_SIZE);
	memcpy(s_entry, cmd, NVME_SQ_ENTRY_SIZE);
	nvme->queue[sq].idx = (nvme->queue[sq].idx + 1) & (NVME_QUEUE_SIZE - 1);
	write32(nvme->queue[sq].bell, nvme->queue[sq].idx);

	struct nvme_c_queue_entry *c_entry = nvme->queue[cq].base +
		(nvme->queue[cq].idx * NVME_CQ_ENTRY_SIZE);
	while (((read32(&c_entry->dw[3]) >> 16) & 0x1) == nvme->queue[cq].round)
		;
	nvme->queue[cq].idx = (nvme->queue[cq].idx + 1) & (NVME_QUEUE_SIZE - 1);
	write32(nvme->queue[cq].bell, nvme->queue[cq].idx);
	if (nvme->queue[cq].idx == 0)
		nvme->queue[cq].round = (nvme->queue[cq].round + 1) & 1;
	return c_entry->dw[3] >> 17;
}

static int delete_io_submission_queue(struct nvme_dev *nvme)
{
	const struct nvme_s_queue_entry e = {
		.dw[0]  = 0,
		.dw[10] = ios,
	};

	int res = nvme_cmd(nvme, NVME_ADMIN_QUEUE, &e);

	free(nvme->queue[ios].base);
	nvme->queue[ios].base = NULL;
	nvme->queue[ios].bell = NULL;
	nvme->queue[ios].idx  = 0;
	return res;
}

static int delete_io_completion_queue(struct nvme_dev *nvme)
{
	const struct nvme_s_queue_entry e = {
		.dw[0]  = 1,
		.dw[10] = ioc,
	};

	int res = nvme_cmd(nvme, NVME_ADMIN_QUEUE, &e);
	free(nvme->queue[ioc].base);

	nvme->queue[ioc].base  = NULL;
	nvme->queue[ioc].bell  = NULL;
	nvme->queue[ioc].idx   = 0;
	nvme->queue[ioc].round = 0;
	return res;
}

static int delete_admin_queues(struct nvme_dev *nvme)
{
	if (nvme->queue[ios].base || nvme->queue[ioc].base)
		printf("NVMe ERROR: IO queues still active.\n");

	free(nvme->queue[ads].base);
	nvme->queue[ads].base = NULL;
	nvme->queue[ads].bell = NULL;
	nvme->queue[ads].idx  = 0;

	free(nvme->queue[adc].base);
	nvme->queue[adc].base = NULL;
	nvme->queue[adc].bell = NULL;
	nvme->queue[adc].idx  = 0;
	nvme->queue[adc].round = 0;

	return 0;
}

static void nvme_detach_device(struct storage_dev *dev)
{
	struct nvme_dev *nvme = (struct nvme_dev *)dev;

	if (delete_io_submission_queue(nvme))
		printf("NVMe ERROR: Failed to delete io submission queue\n");
	if (delete_io_completion_queue(nvme))
		printf("NVME ERROR: Failed to delete io completion queue\n");
	if (delete_admin_queues(nvme))
		printf("NVME ERROR: Failed to delete admin queues\n");

	write32(nvme->config + 0x1c, 0);

	int status, timeout = (read64(nvme->config) >> 24 & 0xff) * 500;
	do {
		status = read32(nvme->config + 0x1c) & 0x3;
		if (status == 0x2) {
			printf("NVMe ERROR: Failed to disable controller. FATAL ERROR\n");
			break;
		}
		if (timeout < 0) {
			printf("NVMe ERROR: Failed to disable controller. Timeout.\n");
			break;
		}
		timeout -= 10;
		mdelay(10);
	} while (status != 0x0);

	uint16_t command = pci_read_config16(nvme->pci_dev, PCI_COMMAND);
	pci_write_config16(nvme->pci_dev, PCI_COMMAND, command & ~PCI_COMMAND_MASTER);

	free(nvme->prp_list);
}

static int nvme_read(struct nvme_dev *nvme, unsigned char *buffer, uint64_t base, uint16_t count)
{
	if (count == 0 || count > 512)
		return -1;

	struct nvme_s_queue_entry e = {
		.dw[0] = 0x02,
		.dw[1] = 0x1,
		.dw[6] = virt_to_phys(buffer),
		.dw[10] = base,
		.dw[11] = base >> 32,
		.dw[12] = count - 1,
	};

	const unsigned int start_page = (uintptr_t)buffer >> 12;
	const unsigned int end_page = ((uintptr_t)buffer + count * 512 - 1) >> 12;
	if (end_page == start_page) {
		/* No page crossing, PRP2 is reserved */
	} else if (end_page == start_page + 1) {
		/* Crossing exactly one page boundary, PRP2 is second page */
		e.dw[8] = virt_to_phys(buffer + 0x1000) & ~0xfff;
	} else {
		/* Use a single page as PRP list, PRP2 points to the list */
		unsigned int i;
		for (i = 0; i < end_page - start_page; ++i) {
			buffer += 0x1000;
			nvme->prp_list[i] = virt_to_phys(buffer) & ~0xfff;
		}
		e.dw[8] = virt_to_phys(nvme->prp_list);
	}

	return nvme_cmd(nvme, ios, &e);
}

static ssize_t nvme_read_blocks512(
		struct storage_dev *const dev,
		const lba_t start, const size_t count, unsigned char *const buf)
{
	unsigned int off = 0;
	while (off < count) {
		const unsigned int blocks = MIN(count - off, 512);
		if (nvme_read((struct nvme_dev *)dev, buf + (off * 512), start + off, blocks))
			return off;
		off += blocks;
	}
	return count;
}

static int create_io_submission_queue(struct nvme_dev *nvme)
{
	void *sq_buffer = memalign(0x1000, NVME_SQ_ENTRY_SIZE * NVME_QUEUE_SIZE);
	if (!sq_buffer) {
		printf("NVMe ERROR: Failed to allocate memory for io submission queue.\n");
		return -1;
	}
	memset(sq_buffer, 0, NVME_SQ_ENTRY_SIZE * NVME_QUEUE_SIZE);

	struct nvme_s_queue_entry e = {
		.dw[0]  = 0x01,
		.dw[6]  = virt_to_phys(sq_buffer),
		.dw[10] = ((NVME_QUEUE_SIZE - 1) << 16) | ios >> 1,
		.dw[11] = (1 << 16) | 1,
	};

	int res = nvme_cmd(nvme, NVME_ADMIN_QUEUE, &e);
	if (res) {
		printf("NVMe ERROR: nvme_cmd returned with %i.\n", res);
		free(sq_buffer);
		return res;
	}

	uint8_t cap_dstrd = (read64(nvme->config) >> 32) & 0xf;
	nvme->queue[ios].base = sq_buffer;
	nvme->queue[ios].bell = nvme->config + 0x1000 + (ios * (4 << cap_dstrd));
	nvme->queue[ios].idx = 0;
	return 0;
}

static int create_io_completion_queue(struct nvme_dev *nvme)
{
	void *const cq_buffer = memalign(0x1000, NVME_CQ_ENTRY_SIZE * NVME_QUEUE_SIZE);
	if (!cq_buffer) {
		printf("NVMe ERROR: Failed to allocate memory for io completion queue.\n");
		return -1;
	}
	memset(cq_buffer, 0, NVME_CQ_ENTRY_SIZE * NVME_QUEUE_SIZE);

	const struct nvme_s_queue_entry e = {
		.dw[0]  = 0x05,
		.dw[6]  = virt_to_phys(cq_buffer),
		.dw[10] = ((NVME_QUEUE_SIZE - 1) << 16) | ioc >> 1,
		.dw[11] = 1,
	};

	int res = nvme_cmd(nvme, NVME_ADMIN_QUEUE, &e);
	if (res) {
		printf("NVMe ERROR: nvme_cmd returned with %i.\n", res);
		free(cq_buffer);
		return res;
	}

	uint8_t cap_dstrd = (read64(nvme->config) >> 32) & 0xf;
	nvme->queue[ioc].base  = cq_buffer;
	nvme->queue[ioc].bell  = nvme->config + 0x1000 + (ioc * (4 << cap_dstrd));
	nvme->queue[ioc].idx   = 0;
	nvme->queue[ioc].round = 0;

	return 0;
}

static int create_admin_queues(struct nvme_dev *nvme)
{
	uint8_t cap_dstrd = (read64(nvme->config) >> 32) & 0xf;
	write32(nvme->config + 0x24, (NVME_QUEUE_SIZE - 1) << 16 | (NVME_QUEUE_SIZE - 1));

	void *sq_buffer = memalign(0x1000, NVME_SQ_ENTRY_SIZE * NVME_QUEUE_SIZE);
	if (!sq_buffer) {
		printf("NVMe ERROR: Failed to allocated memory for admin submission queue\n");
		return -1;
	}
	memset(sq_buffer, 0, NVME_SQ_ENTRY_SIZE * NVME_QUEUE_SIZE);
	write64(nvme->config + 0x28, virt_to_phys(sq_buffer));

	nvme->queue[ads].base = sq_buffer;
	nvme->queue[ads].bell = nvme->config + 0x1000 + (ads * (4 << cap_dstrd));
	nvme->queue[ads].idx = 0;

	void *cq_buffer = memalign(0x1000, NVME_CQ_ENTRY_SIZE * NVME_QUEUE_SIZE);
	if (!cq_buffer) {
		printf("NVMe ERROR: Failed to allocate memory for admin completion queue\n");
		free(cq_buffer);
		return -1;
	}
	memset(cq_buffer, 0, NVME_CQ_ENTRY_SIZE * NVME_QUEUE_SIZE);
	write64(nvme->config + 0x30, virt_to_phys(cq_buffer));

	nvme->queue[adc].base = cq_buffer;
	nvme->queue[adc].bell = nvme->config + 0x1000 + (adc * (4 << cap_dstrd));
	nvme->queue[adc].idx = 0;
	nvme->queue[adc].round = 0;

	return 0;
}

static void nvme_init(pcidev_t dev)
{
	printf("NVMe init (Device %02x:%02x.%02x)\n",
			PCI_BUS(dev), PCI_SLOT(dev), PCI_FUNC(dev));

	void *pci_bar0 = phys_to_virt(pci_read_config32(dev, 0x10) & ~0x3ff);

	if (!(((read64(pci_bar0) >> 37) & 0xff) == 0x01)) {
		printf("NVMe ERROR: PCIe device does not support the NVMe command set\n");
		return;
	}
	struct nvme_dev *nvme = malloc(sizeof(*nvme));
	if (!nvme) {
		printf("NVMe ERROR: Failed to allocate buffer for nvme driver struct\n");
		return;
	}
	nvme->storage_dev.port_type		= PORT_TYPE_NVME;
	nvme->storage_dev.poll			= nvme_poll;
	nvme->storage_dev.read_blocks512	= nvme_read_blocks512;
	nvme->storage_dev.write_blocks512	= NULL;
	nvme->storage_dev.detach_device		= nvme_detach_device;
	nvme->pci_dev				= dev;
	nvme->config				= pci_bar0;
	nvme->prp_list				= memalign(0x1000, 0x1000);

	if (!nvme->prp_list) {
		printf("NVMe ERROR: Failed to allocate buffer for PRP list\n");
		goto _free_abort;
	}

	const uint32_t cc = NVME_CC_EN | NVME_CC_CSS | NVME_CC_MPS | NVME_CC_AMS | NVME_CC_SHN
			| NVME_CC_IOSQES | NVME_CC_IOCQES;

	write32(nvme->config + 0x1c, 0);

	int status, timeout = (read64(nvme->config) >> 24 & 0xff) * 500;
	do {
		status = read32(nvme->config + 0x1c) & 0x3;
		if (status == 0x2) {
			printf("NVMe ERROR: Failed to disable controller. FATAL ERROR\n");
			goto _free_abort;
		}
		if (timeout < 0) {
			printf("NVMe ERROR: Failed to disable controller. Timeout.\n");
			goto _free_abort;
		}
		timeout -= 10;
		mdelay(10);
	} while (status != 0x0);
	if (create_admin_queues(nvme))
		goto _free_abort;
	write32(nvme->config + 0x14, cc);

	timeout = (read64(nvme->config) >> 24 & 0xff) * 500;
	do {
		status = read32(nvme->config + 0x1c) & 0x3;
		if (status == 0x2)
			goto _delete_admin_abort;
		if (timeout < 0)
			goto _delete_admin_abort;
		timeout -= 10;
		mdelay(10);
	} while (status != 0x1);

	uint16_t command = pci_read_config16(dev, PCI_COMMAND);
	pci_write_config16(dev, PCI_COMMAND, command | PCI_COMMAND_MASTER);
	if (create_io_completion_queue(nvme))
		goto _delete_admin_abort;
	if (create_io_submission_queue(nvme))
		goto _delete_completion_abort;
	storage_attach_device((storage_dev_t *)nvme);
	printf("NVMe init done.\n");
	return;

_delete_completion_abort:
	delete_io_completion_queue(nvme);
_delete_admin_abort:
	delete_admin_queues(nvme);
_free_abort:
	free(nvme->prp_list);
	free(nvme);
	printf("NVMe init failed.\n");
}

void nvme_initialize(struct pci_dev *dev)
{
	nvme_init(PCI_DEV(dev->bus, dev->dev, dev->func));
}
