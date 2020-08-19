/*
 *
 * Copyright (C) 2012 secunet Security Networks AG
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#include <stdint.h>
#include <stdio.h>
#include <libpayload.h>

#include <storage/ata.h>
#include <storage/atapi.h>

static int atapi_request_sense(atapi_dev_t *const dev)
{
	u8 cdb[12] = { 0, };
	cdb[0] = ATAPI_REQUEST_SENSE;
	cdb[4] = sizeof(dev->sense_data);
	return dev->packet_read_cmd(dev, cdb, sizeof(cdb),
			dev->sense_data, sizeof(dev->sense_data));
}

static ssize_t atapi_packet_read_cmd(atapi_dev_t *const dev,
				     const u8 *const cmd, const size_t cmdlen,
				     u8 *const buf, const size_t buflen)
{
	int retries = 10;
	ssize_t ret;

	do {
		ret = dev->packet_read_cmd(dev, cmd, cmdlen, buf, buflen);
		if (ret >= 0)
			return ret;

		ret = atapi_request_sense(dev);
		if (ret < 0) {
			printf("atapi: Requesting sense failed.\n");
			return -1;
		}

		switch (dev->sense_data[2] & 0xf) {
		case ATAPI_SENSE_UNIT_ATTENTION:
			/* Nothing to do. */
			break;
		case ATAPI_SENSE_NOT_READY:
			switch (dev->sense_data[12]) {
			case ATAPI_ADDITIONAL_SENSE_LOGICAL_UNIT_NOT_READY:
				/* Device wants more time. */
				delay(3);
				break;
			case ATAPI_ADDITIONAL_SENSE_MEDIUM_NOT_PRESENT:
				printf("atapi: No medium present.\n");
				dev->medium_present = 0;
				return -1;
			}
			break;
		default:
			return -1;
		}
	} while (retries--);

	/* No more retries. */
	return -1;
}

static ssize_t atapi_packet_read_sectors(ata_dev_t *const _dev,
					 const lba_t start, size_t count,
					 u8 *const buf)
{
	atapi_dev_t *const dev = (atapi_dev_t *)_dev;

	if (start >= (1ULL << 32)) {
		printf("atapi: Sector is not 32-bit addressable.\n");
		return -1;
	} else if (count >= (64 * 1024)) {
	       printf("ahci: Sector count too high (max. 65535).\n");
	       count = (64 * 1024) - 1;
	}

	u8 cdb[12] = { 0, };
	cdb[0] = ATAPI_READ_10;
	cdb[2] = (start >> 24) & 0xff;
	cdb[3] = (start >> 16) & 0xff;
	cdb[4] = (start >>  8) & 0xff;
	cdb[5] = (start >>  0) & 0xff;
	cdb[7] = (count >>  8) & 0xff;
	cdb[8] = (count >>  0) & 0xff;
	const ssize_t ret = atapi_packet_read_cmd(dev, cdb, sizeof(cdb),
				buf, count << dev->ata_dev.sector_size_shift);
	if (ret < 0)
		return ret;
	else
		return ret >> dev->ata_dev.sector_size_shift;
}

static storage_poll_t atapi_poll(storage_dev_t *const _dev)
{
	atapi_dev_t *const dev = (atapi_dev_t *)_dev;

	u8 cdb[12] = { 0, };
	u8 capacity[8];

	if (dev->medium_present)
		return POLL_MEDIUM_PRESENT;

	cdb[0] = ATAPI_TEST_UNIT_READY;
	const int ret = atapi_packet_read_cmd(dev, cdb, sizeof(cdb), NULL, 0);
	if (!ret) {
		printf("atapi: Found medium.\n");
		dev->medium_present = 1;
	} else if (dev->medium_present) {
		return POLL_ERROR;
	}

	if (dev->medium_present) {
		cdb[0] = ATAPI_START_STOP_UNIT;
		cdb[4] = 0x01; /* Start Disc, read TOC. */
		if (atapi_packet_read_cmd(dev, cdb, sizeof(cdb), NULL, 0))
			goto _error_ret;
		cdb[4] = 0x00;
	}

	cdb[0] = ATAPI_PREVENT_ALLOW_MEDIUM_REMOVAL;
	cdb[2] = 0x02; /* Clear persistent prevent removal bit. */
	atapi_packet_read_cmd(dev, cdb, sizeof(cdb), NULL, 0);
	cdb[2] = 0x00; /* Clear prevent removal bit. */
	atapi_packet_read_cmd(dev, cdb, sizeof(cdb), NULL, 0);

	/* If we don't have a medium, we're done. */
	if (!dev->medium_present)
		return POLL_NO_MEDIUM;

	/* Read capacity and sector size. */
	cdb[0] = ATAPI_READ_CAPACITY;
	if (atapi_packet_read_cmd(dev, cdb, sizeof(cdb),
				  (u8 *)capacity, sizeof(capacity))
			!= sizeof(capacity))
		goto _error_ret;
	const u32 sector_size = (capacity[4] << 24) |
				(capacity[5] << 16) |
				(capacity[6] <<  8) |
				(capacity[7] <<  0);
	if (ata_set_sector_size(&dev->ata_dev, sector_size)) {
		dev->medium_present = 0;
		return POLL_MEDIUM_ERROR;
	}

	return POLL_MEDIUM_PRESENT;

_error_ret:
	dev->medium_present = 0;
	return POLL_ERROR;
}

int atapi_attach_device(atapi_dev_t *const dev, const storage_port_t port_type)
{
	u16 id[256];

	dev->ata_dev.identify_cmd = ATA_IDENTIFY_PACKET_DEVICE;
	if (dev->identify(&dev->ata_dev, (u8 *)id))
		return -1;

	char fw[9], model[41];
	ata_strncpy(fw, id + 23, sizeof(fw));
	ata_strncpy(model, id + 27, sizeof(model));
	printf("atapi: Identified %s [%s]\n", model, fw);

	/* Initialize to sane values. */
	dev->ata_dev.sector_size = 2048;
	dev->ata_dev.sector_size_shift = 11;

	dev->medium_present = 0;

	/* Reuse ata procedures. */
	ata_initialize_storage_ops(&dev->ata_dev);

	dev->ata_dev.read_sectors = atapi_packet_read_sectors;
	dev->ata_dev.storage_dev.port_type = port_type;
	dev->ata_dev.storage_dev.poll = atapi_poll;

	return storage_attach_device(&dev->ata_dev.storage_dev);
}
