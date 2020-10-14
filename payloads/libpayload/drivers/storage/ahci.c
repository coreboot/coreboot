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

//#define DEBUG_STATUS

#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <libpayload.h>
#include <pci.h>
#include <pci/pci.h>
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

/** Do minimal error recovery. */
int ahci_error_recovery(ahci_dev_t *const dev, const u32 intr_status)
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

static int ahci_dev_init(hba_ctrl_t *const ctrl,
			 hba_port_t *const port,
			 const int portnum)
{
	int ret = 1;

	const int ncs = HBA_CAPS_DECODE_NCS(ctrl->caps);

	if (ahci_cmdengine_stop(port))
		return 1;

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
	port->cmdlist_base = virt_to_phys(cmdlist);
	port->frameinfo_base = virt_to_phys(rcvd_fis);
	if (ahci_cmdengine_start(port))
		goto _cleanup_ret;
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
#if CONFIG(LP_STORAGE_ATA)
		dev->ata_dev.identify = ahci_identify_device;
		dev->ata_dev.read_sectors = ahci_ata_read_sectors;
		return ata_attach_device(&dev->ata_dev, PORT_TYPE_SATA);
#endif
		break;
	case HBA_PxSIG_ATAPI:
		printf("ahci: ATAPI drive on port #%d.\n", portnum);
#if CONFIG(LP_STORAGE_ATAPI)
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
	/* Only free if stopping succeeds, since otherwise the controller may
	   still use the resources for DMA. */
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

#if CONFIG(LP_STORAGE_AHCI_ONLY_TESTED)
static u32 working_controllers[] = {
	0x8086 | 0x2929 << 16, /* Mobile ICH9 */
	0x8086 | 0x1c03 << 16, /* Mobile Cougar Point PCH */
	0x8086 | 0x1e03 << 16, /* Mobile Panther Point PCH */
	0x8086 | 0xa102 << 16, /* Desktop / Mobile-Wks  Sunrise Point PCH */
	0x8086 | 0x5ae3 << 16, /* Apollo Lake */
};
#endif

void ahci_initialize(struct pci_dev *dev)
{
	int i;

#if CONFIG(LP_STORAGE_AHCI_ONLY_TESTED)
	const u32 vendor_device = dev->vendor_id | dev->device_id << 16;
	for (i = 0; i < ARRAY_SIZE(working_controllers); ++i)
		if (vendor_device == working_controllers[i])
			break;
	if (i == ARRAY_SIZE(working_controllers)) {
		printf("ahci: Not using untested SATA controller "
			"%02x:%02x.%02x (%04x:%04x).\n", dev->bus,
			dev->dev, dev->func, dev->vendor_id, dev->device_id);
		return;
	}
#endif

	printf("ahci: Found SATA controller %02x:%02x.%02x (%04x:%04x).\n",
		dev->bus, dev->dev, dev->func, dev->vendor_id, dev->device_id);

	hba_ctrl_t *const ctrl = phys_to_virt(pci_read_long(dev, 0x24) & ~0x3ff);
	hba_port_t *const ports = ctrl->ports;

	/* Reset host controller. */
	ctrl->global_ctrl |= HBA_CTRL_RESET;
	/* Reset has to be finished after 1s. */
	int timeout = 10 * 1000; /* Time out after 10,000 * 100us == 1s. */
	while (ctrl->global_ctrl & HBA_CTRL_RESET && timeout--)
		udelay(100);
	if (ctrl->global_ctrl & HBA_CTRL_RESET) {
		printf("ahci: ERROR: "
			"Controller reset didn't finish within 1s.\n");
		return;
	}

	/* Set AHCI access mode. */
	ctrl->global_ctrl |= HBA_CTRL_AHCI_EN;

	/* Enable bus mastering. */
	const u16 command = pci_read_word(dev, PCI_COMMAND);
	pci_write_word(dev, PCI_COMMAND, command | PCI_COMMAND_MASTER);

	/* Probe for devices. */
	for (i = 0; i < 32; ++i) {
		if (ctrl->ports_impl & (1 << i))
			ahci_port_probe(ctrl, &ports[i], i + 1);
	}
}
