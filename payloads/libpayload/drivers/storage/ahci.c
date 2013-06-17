/*
 * This file is part of the libpayload project.
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

//#define DEBUG_STATUS

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


static inline int ahci_port_is_active(const hba_port_t *const port)
{
	return (port->sata_status & (HBA_PxSSTS_IPM_MASK | HBA_PxSSTS_DET_MASK))
		== (HBA_PxSSTS_IPM_ACTIVE | HBA_PxSSTS_DET_ESTABLISHED);
}

static int ahci_cmdengine_start(hba_port_t *const port)
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

static int ahci_cmdengine_stop(hba_port_t *const port)
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

/** Do minimal error recovery. */
static int ahci_error_recovery(ahci_dev_t *const dev, const u32 intr_status)
{
	/* Command engine has to be restarted.
	   We don't call ahci_cmdengine_stop() here as it also checks
	   HBA_PxCMD_FR which won't clear on fatal errors. */
	dev->port->cmd_stat &= ~HBA_PxCMD_ST;

	/* Always clear sata_error. */
	ahci_clear_status(dev->port, sata_error);

	/* Perform COMRESET if appropriate. */
	const u32 tfd = dev->port->taskfile_data;
	if ((tfd & (HBA_PxTFD_BSY | HBA_PxTFD_DRQ)) |
			(intr_status & HBA_PxIS_PCS)) {
		const u32 sctl = dev->port->sata_control & ~HBA_PxSCTL_DET_MASK;
		dev->port->sata_control = sctl | HBA_PxSCTL_DET_COMRESET;
		mdelay(1);
		dev->port->sata_control = sctl;
	}

	if (ahci_port_is_active(dev->port))
		/* Start command engine. */
		return ahci_cmdengine_start(dev->port);
	else
		return -1;
}

/** Give a buffer with even address. */
static u8 *ahci_prdbuf_init(ahci_dev_t *const dev,
			    u8 *const user_buf, const size_t len,
			    const int out)
{
	if ((u32)user_buf & 1) {
		printf("ahci: Odd buffer pointer (%p).\n", user_buf);
		if (dev->buf) /* orphaned buffer */
			free((void *)dev->buf - *(dev->buf - 1));
		dev->buf = malloc(len + 2);
		if (!dev->buf)
			return NULL;
		dev->user_buf = user_buf;
		dev->write_back = !out;
		dev->buflen = len;
		if ((u32)dev->buf & 1) {
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
		free((void *)dev->buf - *(dev->buf - 1));
	}
	dev->buf = NULL;
	dev->user_buf = NULL;
	dev->write_back = 0;
	dev->buflen = 0;
}

static ssize_t ahci_cmdslot_exec(ahci_dev_t *const dev)
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

static size_t ahci_cmdslot_prepare(ahci_dev_t *const dev,
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

static ssize_t ahci_ata_read_sectors(ata_dev_t *const ata_dev,
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
#ifdef CONFIG_STORAGE_64BIT_LBA
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
#ifdef CONFIG_STORAGE_64BIT_LBA
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

static ssize_t ahci_packet_read_cmd(atapi_dev_t *const _dev,
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

static int ahci_identify_device(ata_dev_t *const ata_dev, u8 *const buf)
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

static int ahci_dev_init(hba_ctrl_t *const ctrl,
			 hba_port_t *const port,
			 const int portnum)
{
	int ret = 1;

	const int ncs = HBA_CAPS_DECODE_NCS(ctrl->caps);

	/* Allocate command list, one command table and received FIS. */
	cmd_t *const cmdlist = memalign(1024, ncs * sizeof(cmd_t));
	cmdtable_t *const cmdtable = memalign(128, sizeof(cmdtable_t));
	rcvd_fis_t *const rcvd_fis = memalign(256, sizeof(rcvd_fis_t));
	/* Allocate our device structure. */
	ahci_dev_t *const dev = calloc(1, sizeof(ahci_dev_t));
	if (!cmdlist || !cmdtable || !rcvd_fis || !dev)
		goto _cleanup_ret;
	memset((void *)cmdlist, '\0', ncs * sizeof(cmd_t));
	memset((void *)cmdtable, '\0', sizeof(*cmdtable));
	memset((void *)rcvd_fis, '\0', sizeof(*rcvd_fis));

	/* Set command list base and received FIS base. */
	if (ahci_cmdengine_stop(port))
		return 1;
	port->cmdlist_base = virt_to_phys(cmdlist);
	port->frameinfo_base = virt_to_phys(rcvd_fis);
	if (ahci_cmdengine_start(port))
		return 1;
	/* Put port into active state. */
	port->cmd_stat |= HBA_PxCMD_ICC_ACTIVE;

	dev->ctrl = ctrl;
	dev->port = port;
	dev->cmdlist = cmdlist;
	dev->cmdtable = cmdtable;
	dev->rcvd_fis = rcvd_fis;

	/*
	 * Wait for D2H Register FIS with device' signature.
	 * The drive has to spin up here, so wait up to 30s.
	 */
	const int timeout_s = 30; /* Time out after 30s. */
	int timeout = timeout_s * 100;
	while ((port->taskfile_data & HBA_PxTFD_BSY) && timeout--)
		mdelay(10);

	if (port->taskfile_data & HBA_PxTFD_BSY)
		printf("ahci: Timed out after %d seconds "
		       "of waiting for device to spin up.\n", timeout_s);

	/* Initialize device or fall through to clean up. */
	switch (port->signature) {
	case HBA_PxSIG_ATA:
		printf("ahci: ATA drive on port #%d.\n", portnum);
#ifdef CONFIG_STORAGE_ATA
		dev->ata_dev.identify = ahci_identify_device;
		dev->ata_dev.read_sectors = ahci_ata_read_sectors;
		return ata_attach_device(&dev->ata_dev, PORT_TYPE_SATA);
#endif
		break;
	case HBA_PxSIG_ATAPI:
		printf("ahci: ATAPI drive on port #%d.\n", portnum);
#ifdef CONFIG_STORAGE_ATAPI
		dev->atapi_dev.identify = ahci_identify_device;
		dev->atapi_dev.packet_read_cmd = ahci_packet_read_cmd;
		return atapi_attach_device(&dev->atapi_dev, PORT_TYPE_SATA);
#endif
		break;
	default:
		printf("ahci: Unsupported device (signature == 0x%08x) "
				"on port #%d.\n", port->signature, portnum);
		break;
	}
	ret = 2;

_cleanup_ret:
	/* Clean up (not reached for initialized devices). */
	if (dev)
		free(dev);
	if (!ahci_cmdengine_stop(port)) {
		port->cmdlist_base = 0;
		port->frameinfo_base = 0;
		if (rcvd_fis)
			free((void *)rcvd_fis);
		if (cmdtable)
			free((void *)cmdtable);
		if (cmdlist)
			free((void *)cmdlist);
	}
	return ret;
}

static void ahci_port_probe(hba_ctrl_t *const ctrl,
			    hba_port_t *const port,
			    const int portnum)
{
	/* If staggered spin-up is supported, spin-up device. */
	if (ctrl->caps & HBA_CAPS_SSS) {
		port->cmd_stat |= HBA_PxCMD_SUD;
	}

	/* Wait 1s if we just told the device to spin up or
	           if it's the first port. */
	if ((ctrl->caps & HBA_CAPS_SSS) ||
			!(ctrl->ports_impl & ((1 << (portnum - 1)) - 1))) {
		/* Wait for port to become active. */
		int timeout = 100; /* Time out after 100 * 100us == 10ms. */
		while (!ahci_port_is_active(port) && timeout--)
			udelay(100);
	}
	if (!ahci_port_is_active(port))
		return;

	ahci_clear_status(port, sata_error);
	ahci_clear_status(port, intr_status);

	ahci_dev_init(ctrl, port, portnum);
}

#ifdef CONFIG_STORAGE_AHCI_ONLY_TESTED
static u32 working_controllers[] = {
	0x8086 | 0x2929 << 16, /* Mobile ICH9 */
	0x8086 | 0x1e03 << 16, /* Mobile Panther Point PCH */
};
#endif
static void ahci_init_pci(pcidev_t dev)
{
	int i;

	const u16 class = pci_read_config16(dev, 0xa);
	if (class != 0x0106)
		return;
	const u16 vendor = pci_read_config16(dev, 0x00);
	const u16 device = pci_read_config16(dev, 0x02);

#ifdef CONFIG_STORAGE_AHCI_ONLY_TESTED
	const u32 vendor_device = pci_read_config32(dev, 0x0);
	for (i = 0; i < ARRAY_SIZE(working_controllers); ++i)
		if (vendor_device == working_controllers[i])
			break;
	if (i == ARRAY_SIZE(working_controllers)) {
		printf("ahci: Not using untested SATA controller "
			"%02x:%02x.%02x (%04x:%04x).\n", PCI_BUS(dev),
			PCI_SLOT(dev), PCI_FUNC(dev), vendor, device);
		return;
	}
#endif

	printf("ahci: Found SATA controller %02x:%02x.%02x (%04x:%04x).\n",
		PCI_BUS(dev), PCI_SLOT(dev), PCI_FUNC(dev), vendor, device);

	hba_ctrl_t *const ctrl = phys_to_virt(
			pci_read_config32(dev, 0x24) & ~0x3ff);
	hba_port_t *const ports = ctrl->ports;

	/* Reset host controller. */
	ctrl->global_ctrl |= HBA_CTRL_RESET;
	/* Reset has to be finished after 1s. */
	delay(1);
	if (ctrl->global_ctrl & HBA_CTRL_RESET) {
		printf("ahci: ERROR: "
			"Controller reset didn't finish within 1s.\n");
		return;
	}

	/* Set AHCI access mode. */
	ctrl->global_ctrl |= HBA_CTRL_AHCI_EN;

	/* Probe for devices. */
	for (i = 0; i < 32; ++i) {
		if (ctrl->ports_impl & (1 << i))
			ahci_port_probe(ctrl, &ports[i], i + 1);
	}
}

void ahci_initialize(void)
{
	int bus, dev, func;

	for (bus = 0; bus < 256; ++bus) {
		for (dev = 0; dev < 32; ++dev) {
			const u16 class =
				pci_read_config16(PCI_DEV(bus, dev, 0), 0xa);
			if (class != 0xffff) {
				for (func = 0; func < 8; ++func)
					ahci_init_pci(PCI_DEV(bus, dev, func));
			}
		}
	}
}
