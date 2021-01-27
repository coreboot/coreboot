/* SPDX-License-Identifier: GPL-2.0-only */

#include <acpi/acpi.h>
#include <device/mmio.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ops.h>
#include <console/console.h>
#include <device/pci_ids.h>
#include <device/pci_def.h>
#include <string.h>
#include <delay.h>

#include "me.h"
#include "pch.h"

/* Send END OF POST message to the ME */
static int me8_mkhi_end_of_post(void)
{
	struct mkhi_header mkhi = {
		.group_id	= MKHI_GROUP_ID_GEN,
		.command	= MKHI_END_OF_POST,
	};
	struct mei_header mei = {
		.is_complete	= 1,
		.host_address	= MEI_HOST_ADDRESS,
		.client_address	= MEI_ADDRESS_MKHI,
		.length		= sizeof(mkhi),
	};

	u32 eop_ack;

	/* Send request and wait for response */
	printk(BIOS_NOTICE, "ME: %s\n", __func__);
	if (mei_sendrecv(&mei, &mkhi, NULL, &eop_ack, sizeof(eop_ack)) < 0) {
		printk(BIOS_ERR, "ME: END OF POST message failed\n");
		return -1;
	}

	printk(BIOS_INFO, "ME: END OF POST message successful (%d)\n", eop_ack);
	return 0;
}

void intel_me8_finalize_smm(void)
{
	struct me_hfs hfs;
	u32 reg32;

	update_mei_base_address();

	/* S3 path will have hidden this device already */
	if (!is_mei_base_address_valid())
		return;

	/* Make sure ME is in a mode that expects EOP */
	reg32 = pci_read_config32(PCH_ME_DEV, PCI_ME_HFS);
	memcpy(&hfs, &reg32, sizeof(u32));

	/* Abort and leave device alone if not normal mode */
	if (hfs.fpt_bad ||
	    hfs.working_state != ME_HFS_CWS_NORMAL ||
	    hfs.operation_mode != ME_HFS_MODE_NORMAL)
		return;

	/* Try to send EOP command so ME stops accepting other commands */
	me8_mkhi_end_of_post();

	/* Make sure IO is disabled */
	pci_and_config16(PCH_ME_DEV, PCI_COMMAND,
			 ~(PCI_COMMAND_MASTER | PCI_COMMAND_MEMORY | PCI_COMMAND_IO));

	/* Hide the PCI device */
	RCBA32_OR(FD2, PCH_DISABLE_MEI1);
}

/* Send END OF POST message to the ME */
static int me7_mkhi_end_of_post(void)
{
	struct mkhi_header mkhi = {
		.group_id	= MKHI_GROUP_ID_GEN,
		.command	= MKHI_END_OF_POST,
	};
	struct mei_header mei = {
		.is_complete	= 1,
		.host_address	= MEI_HOST_ADDRESS,
		.client_address	= MEI_ADDRESS_MKHI,
		.length		= sizeof(mkhi),
	};

	/* Send request and wait for response */
	if (mei_sendrecv(&mei, &mkhi, NULL, NULL, 0) < 0) {
		printk(BIOS_ERR, "ME: END OF POST message failed\n");
		return -1;
	}

	printk(BIOS_INFO, "ME: END OF POST message successful\n");
	return 0;
}

static void intel_me7_finalize_smm(void)
{
	struct me_hfs hfs;
	u32 reg32;

	update_mei_base_address();

	/* S3 path will have hidden this device already */
	if (!is_mei_base_address_valid())
		return;

	/* Make sure ME is in a mode that expects EOP */
	reg32 = pci_read_config32(PCH_ME_DEV, PCI_ME_HFS);
	memcpy(&hfs, &reg32, sizeof(u32));

	/* Abort and leave device alone if not normal mode */
	if (hfs.fpt_bad ||
	    hfs.working_state != ME_HFS_CWS_NORMAL ||
	    hfs.operation_mode != ME_HFS_MODE_NORMAL)
		return;

	/* Try to send EOP command so ME stops accepting other commands */
	me7_mkhi_end_of_post();

	/* Make sure IO is disabled */
	pci_and_config16(PCH_ME_DEV, PCI_COMMAND,
			 ~(PCI_COMMAND_MASTER | PCI_COMMAND_MEMORY | PCI_COMMAND_IO));

	/* Hide the PCI device */
	RCBA32_OR(FD2, PCH_DISABLE_MEI1);
}

void intel_me_finalize_smm(void)
{
	u16 did = pci_read_config16(PCH_ME_DEV, PCI_DEVICE_ID);
	switch (did) {
	case 0x1c3a:
		intel_me7_finalize_smm();
		break;
	case 0x1e3a:
		intel_me8_finalize_smm();
		break;
	default:
		printk(BIOS_ERR, "No finalize handler for ME %04x.\n", did);
	}
}
