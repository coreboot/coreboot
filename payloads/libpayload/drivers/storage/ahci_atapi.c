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

ssize_t ahci_packet_read_cmd(atapi_dev_t *const _dev,
				    const u8 *const cmd, const size_t cmdlen,
				    u8 *const buf, const size_t buflen)
{
	ahci_dev_t *const dev = (ahci_dev_t *)_dev;

	if ((cmdlen != 12) && (cmdlen != 16)) {
		printf("ahci: Only 12- and 16-byte packet commands allowed.\n");
		return -1;
	}

	const size_t len = ahci_cmdslot_prepare(dev, buf, buflen, 0);
	u16 byte_limit = MIN(len, 63 * 1024); /* like Linux */
	if (byte_limit & 1) ++byte_limit; /* even limit */

	dev->cmdlist[0].cmd |= CMD_ATAPI;
	dev->cmdtable->fis[0] = FIS_HOST_TO_DEVICE;
	dev->cmdtable->fis[1] = FIS_H2D_CMD;
	dev->cmdtable->fis[2] = ATA_PACKET;
	dev->cmdtable->fis[5] = byte_limit & 0xff;
	dev->cmdtable->fis[6] = byte_limit >> 8;
	memcpy((void *)dev->cmdtable->atapi_cmd, cmd, cmdlen);

	return ahci_cmdslot_exec(dev);
}
