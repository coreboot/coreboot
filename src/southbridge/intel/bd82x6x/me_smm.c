/* SPDX-License-Identifier: GPL-2.0-only */

#include <console/console.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <device/pci_ops.h>
#include <stdint.h>

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

void intel_me_finalize_smm(void)
{
	union me_hfs hfs;

	update_mei_base_address();

	/* S3 path will have hidden this device already */
	if (!is_mei_base_address_valid())
		return;

	/* Make sure ME is in a mode that expects EOP */
	hfs.raw = pci_read_config32(PCH_ME_DEV, PCI_ME_HFS);

	/* Abort and leave device alone if not normal mode */
	if (hfs.fpt_bad ||
	    hfs.working_state != ME_HFS_CWS_NORMAL ||
	    hfs.operation_mode != ME_HFS_MODE_NORMAL)
		return;

	/* Try to send EOP command so ME stops accepting other commands */
	const u16 did = pci_read_config16(PCH_ME_DEV, PCI_DEVICE_ID);
	switch (did) {
	case 0x1c3a:
		me7_mkhi_end_of_post();
		break;
	case 0x1e3a:
		me8_mkhi_end_of_post();
		break;
	default:
		printk(BIOS_ERR, "No finalize handler for ME %04x.\n", did);
	}

	/* Make sure IO is disabled */
	pci_and_config16(PCH_ME_DEV, PCI_COMMAND,
			 ~(PCI_COMMAND_MASTER | PCI_COMMAND_MEMORY | PCI_COMMAND_IO));

	/* Hide the PCI device */
	RCBA32_OR(FD2, PCH_DISABLE_MEI1);
}
