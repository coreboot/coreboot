/*
 *
 * Copyright (C) 2012 secunet Security Networks AG
 * Copyright (C) 2013 Edward O'Callaghan <eocallaghan@alterapraxis.com>
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

#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <libpayload.h>
#include <pci.h>
#include <storage/ata.h>
#include <storage/ahci.h>

#include "ahci_private.h"

ssize_t ahci_ata_read_sectors(ata_dev_t *const ata_dev,
				     const lba_t start, size_t count,
				     u8 *const buf)
{
	ahci_dev_t *const dev = (ahci_dev_t *)ata_dev;

	if (count == 0)
		return 0;

	if (ata_dev->read_cmd == ATA_READ_DMA) {
		if (start >= (1 << 28)) {
		       printf("ahci: Sector is not 28-bit addressable.\n");
		       return -1;
		} else if (count > 256) {
		       printf("ahci: Sector count too high (max. 256).\n");
		       count = 256;
		}
#if CONFIG(LP_STORAGE_64BIT_LBA)
	} else if (ata_dev->read_cmd == ATA_READ_DMA_EXT) {
		if (start >= (1ULL << 48)) {
			printf("ahci: Sector is not 48-bit addressable.\n");
			return -1;
		} else if (count > (64 * 1024)) {
		       printf("ahci: Sector count too high (max. 65536).\n");
		       count = 64 * 1024;
		}
#endif
	} else {
		printf("ahci: Unsupported ATA read command (0x%x).\n",
			ata_dev->read_cmd);
		return -1;
	}

	const size_t bytes = count << ata_dev->sector_size_shift;
	const size_t bytes_feasible = ahci_cmdslot_prepare(dev, buf, bytes, 0);
	const size_t sectors = bytes_feasible >> ata_dev->sector_size_shift;

	dev->cmdtable->fis[ 0] = FIS_HOST_TO_DEVICE;
	dev->cmdtable->fis[ 1] = FIS_H2D_CMD;
	dev->cmdtable->fis[ 2] = ata_dev->read_cmd;
	dev->cmdtable->fis[ 4] = (start >>  0) & 0xff;
	dev->cmdtable->fis[ 5] = (start >>  8) & 0xff;
	dev->cmdtable->fis[ 6] = (start >> 16) & 0xff;
	dev->cmdtable->fis[ 7] = FIS_H2D_DEV_LBA;
	dev->cmdtable->fis[ 8] = (start >> 24) & 0xff;
#if CONFIG(LP_STORAGE_64BIT_LBA)
	if (ata_dev->read_cmd == ATA_READ_DMA_EXT) {
		dev->cmdtable->fis[ 9] = (start >> 32) & 0xff;
		dev->cmdtable->fis[10] = (start >> 40) & 0xff;
	}
#endif
	dev->cmdtable->fis[12] = (sectors >>  0) & 0xff;
	dev->cmdtable->fis[13] = (sectors >>  8) & 0xff;

	if (ahci_cmdslot_exec(dev) < 0)
		return -1;
	else
		return dev->cmdlist->prd_bytes >> ata_dev->sector_size_shift;
}
