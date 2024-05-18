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

#ifdef DEBUG_STATUS
static inline u32 _ahci_clear_status(volatile u32 *const reg,
				     const char *const r,
				     const char *const f)
{
	const u32 bits = *reg;
	if (bits)
		*reg = bits;
	printf("ahci: %s: %s == 0x%08x\n", f, r, bits);

	return bits;
}
#define ahci_clear_status(p, r) _ahci_clear_status(&(p)->r, #r, __func__)
#else
static inline u32 _ahci_clear_status(volatile u32 *const reg)
{
	const u32 bits = *reg;
	if (bits)
		*reg = bits;
	return bits;
}
#define ahci_clear_status(p, r) _ahci_clear_status(&(p)->r)
#endif

/** Give a buffer with even address. */
static u8 *ahci_prdbuf_init(ahci_dev_t *const dev,
			    u8 *const user_buf, const size_t len,
			    const int out)
{
	if ((uintptr_t)user_buf & 1) {
		printf("ahci: Odd buffer pointer (%p).\n", user_buf);
		if (dev->buf) /* orphaned buffer */
			free(dev->buf - *(dev->buf - 1));
		dev->buf = malloc(len + 2);
		if (!dev->buf)
			return NULL;
		dev->user_buf = user_buf;
		dev->write_back = !out;
		dev->buflen = len;
		if ((uintptr_t)dev->buf & 1) {
			dev->buf[0] = 1;
			dev->buf += 1;
		} else {
			dev->buf[0] = 1;
			dev->buf[1] = 2;
			dev->buf += 2;
		}
		if (out)
			memcpy(dev->buf, user_buf, len);
		return dev->buf;
	} else {
		return user_buf;
	}
}

static void ahci_prdbuf_finalize(ahci_dev_t *const dev)
{
	if (dev->buf) {
		if (dev->write_back)
			memcpy(dev->user_buf, dev->buf, dev->buflen);
		free(dev->buf - *(dev->buf - 1));
	}
	dev->buf = NULL;
	dev->user_buf = NULL;
	dev->write_back = 0;
	dev->buflen = 0;
}

int ahci_cmdengine_start(hba_port_t *const port)
{
	/* CR has to be clear before starting the command engine.
	   This shouldn't take too long, but we should time out nevertheless. */
	int timeout = 1000; /* Time out after 1000 * 1us == 1ms. */
	while ((port->cmd_stat & HBA_PxCMD_CR) && timeout--)
		udelay(1);
	if (timeout < 0) {
		printf("ahci: Timeout during start of command engine.\n");
		return 1;
	}

	port->cmd_stat |= HBA_PxCMD_FRE;
	port->cmd_stat |= HBA_PxCMD_ST;
	return 0;
}

int ahci_cmdengine_stop(hba_port_t *const port)
{
	port->cmd_stat &= ~HBA_PxCMD_ST;

	/* Wait for the controller to clear CR.
	   This shouldn't take too long, but we should time out nevertheless. */
	int timeout = 1000; /* Time out after 1000 * 1us == 1ms. */
	while ((port->cmd_stat & HBA_PxCMD_CR) && timeout--)
		udelay(1);
	if (timeout < 0) {
		printf("ahci: Timeout during stopping of command engine.\n");
		return 1;
	}

	port->cmd_stat &= ~HBA_PxCMD_FRE;

	/* Wait for the controller to clear FR.
	   This shouldn't take too long, but we should time out nevertheless. */
	timeout = 1000; /* Time out after 1000 * 1us == 1ms. */
	while ((port->cmd_stat & HBA_PxCMD_FR) && timeout--)
		udelay(1);
	if (timeout < 0) {
		printf("ahci: Timeout during stopping of command engine.\n");
		return 1;
	}

	return 0;
}

ssize_t ahci_cmdslot_exec(ahci_dev_t *const dev)
{
	const int slotnum = 0; /* We always use the first slot. */

	if (!(dev->port->cmd_stat & HBA_PxCMD_CR))
		return -1;

	/* Trigger command execution. */
	dev->port->cmd_issue |= (1 << slotnum);

	/* Wait for the controller to finish command execution. */
	int timeout = 50000; /* Time out after 50000 * 100us == 5s. */
	while ((dev->port->cmd_issue & (1 << slotnum)) &&
			!(dev->port->intr_status & HBA_PxIS_TFES) &&
			timeout--)
		udelay(100);
	if (timeout < 0) {
		printf("ahci: Timeout during command execution.\n");
		return -1;
	}

	ahci_prdbuf_finalize(dev);

	const u32 intr_status = ahci_clear_status(dev->port, intr_status);
	if (intr_status & (HBA_PxIS_FATAL | HBA_PxIS_PCS)) {
		ahci_error_recovery(dev, intr_status);
		return -1;
	} else {
		return dev->cmdlist[slotnum].prd_bytes;
	}
}

size_t ahci_cmdslot_prepare(ahci_dev_t *const dev,
				   u8 *const user_buf, size_t buf_len,
				   const int out)
{
	const int slotnum = 0; /* We always use the first slot. */

	size_t read_count = 0;

	memset((void *)&dev->cmdlist[slotnum],
			'\0', sizeof(dev->cmdlist[slotnum]));
	memset((void *)dev->cmdtable,
			'\0', sizeof(*dev->cmdtable));
	dev->cmdlist[slotnum].cmd = CMD_CFL(FIS_H2D_FIS_LEN);
	dev->cmdlist[slotnum].cmdtable_base = virt_to_phys(dev->cmdtable);

	if (buf_len > 0) {
		size_t prdt_len;
		u8 *buf;
		int i;

		prdt_len = ((buf_len - 1) >> BYTES_PER_PRD_SHIFT) + 1;
		const size_t max_prdt_len = ARRAY_SIZE(dev->cmdtable->prdt);
		if (prdt_len > max_prdt_len) {
			prdt_len = max_prdt_len;
			buf_len = prdt_len << BYTES_PER_PRD_SHIFT;
		}

		dev->cmdlist[slotnum].prdt_length = prdt_len;
		read_count = buf_len;

		buf = ahci_prdbuf_init(dev, user_buf, buf_len, out);
		if (!buf)
			return 0;
		for (i = 0; i < prdt_len; ++i) {
			const size_t bytes =
				(buf_len < BYTES_PER_PRD)
				? buf_len : BYTES_PER_PRD;
			dev->cmdtable->prdt[i].data_base = virt_to_phys(buf);
			dev->cmdtable->prdt[i].flags = PRD_TABLE_BYTES(bytes);
			buf_len -= bytes;
			buf += bytes;
		}
	}

	return read_count;
}

int ahci_identify_device(ata_dev_t *const ata_dev, u8 *const buf)
{
	ahci_dev_t *const dev = (ahci_dev_t *)ata_dev;

	ahci_cmdslot_prepare(dev, buf, 512, 0);

	dev->cmdtable->fis[0] = FIS_HOST_TO_DEVICE;
	dev->cmdtable->fis[1] = FIS_H2D_CMD;
	dev->cmdtable->fis[2] = ata_dev->identify_cmd;

	if ((ahci_cmdslot_exec(dev) < 0) || (dev->cmdlist->prd_bytes != 512))
		return -1;
	else
		return 0;
}
