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

#include <libpayload.h>
#include <pci/pci.h>
#if CONFIG(LP_STORAGE_AHCI)
# include <storage/ahci.h>
#endif
#include <storage/storage.h>

static storage_dev_t **devices = NULL;
static size_t devices_length = 0;
static size_t dev_count = 0;

int storage_attach_device(storage_dev_t *const dev)
{
	if (dev_count == devices_length) {
		const size_t new_len =
			(0 == devices_length) ? 4 : devices_length << 1;
		storage_dev_t **const new_devices =
			realloc(devices, new_len * sizeof(storage_dev_t *));
		if (!new_devices)
			return -1;
		devices = new_devices;
		memset(devices + devices_length, '\0',
			(new_len - devices_length) * sizeof(storage_dev_t *));
		devices_length = new_len;
	}
	devices[dev_count++] = dev;

	return 0;
}

int storage_device_count(void)
{
	return dev_count;
}

/**
 * Probe for drive with given number
 *
 * Looks for a drive with number dev_num and polls for a medium
 * in the drive if appropriate.
 *
 * @dev_num device number counted from 0
 */
storage_poll_t storage_probe(const size_t dev_num)
{
	if (dev_num >= dev_count)
		return POLL_NO_DEVICE;
	else if (devices[dev_num]->poll)
		return devices[dev_num]->poll(devices[dev_num]);
	else
		return POLL_MEDIUM_PRESENT;
}

/**
 * Read 512-byte blocks
 *
 * Reads count blocks of 512 bytes from block start of drive dev_num
 * into buf.
 *
 * @dev_num device number counted from 0
 * @start number of first block to read from
 * @count number of blocks to read
 * @buf buffer where the read data should be written
 */
ssize_t storage_read_blocks512(const size_t dev_num,
			       const lba_t start, const size_t count,
			       unsigned char *const buf)
{
	if ((dev_num < dev_count) && devices[dev_num]->read_blocks512)
		return devices[dev_num]->read_blocks512(
				devices[dev_num], start, count, buf);
	else
		return -1;
}

/**
 * Initializes storage controllers
 *
 * This function should be called once at startup to bring up supported
 * storage controllers.
 */
void storage_initialize(void)
{
#if CONFIG(LP_PCI)
	struct pci_dev *dev;
	for (dev = lib_sysinfo.pacc.devices; dev; dev = dev->next) {
		switch (dev->device_class) {
#if CONFIG(LP_STORAGE_AHCI)
		case PCI_CLASS_STORAGE_AHCI:
			ahci_initialize(dev);
			break;
#endif
		default:
			break;
		}
	}
#endif
}
