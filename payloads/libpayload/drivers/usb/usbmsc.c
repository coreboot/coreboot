/*
 * This file is part of the libpayload project.
 *
 * Copyright (C) 2008 coresystems GmbH
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

#include <endian.h>
#include <usb/usb.h>
#include <usb/usbmsc.h>
#include <usb/usbdisk.h>

enum {
	msc_subclass_rbc = 0x1,
	msc_subclass_mmc2 = 0x2,
	msc_subclass_qic157 = 0x3,
	msc_subclass_ufi = 0x4,
	msc_subclass_sff8070i = 0x5,
	msc_subclass_scsitrans = 0x6
};

static const char *msc_subclass_strings[7] = {
	"(none)",
	"RBC",
	"MMC-2",
	"QIC-157",
	"UFI",
	"SFF-8070i",
	"SCSI transparent"
};
enum {
	msc_proto_cbi_wcomp = 0x0,
	msc_proto_cbi_wocomp = 0x1,
	msc_proto_bulk_only = 0x50
};
static const char *msc_protocol_strings[0x51] = {
	"Control/Bulk/Interrupt protocol (with command completion interrupt)",
	"Control/Bulk/Interrupt protocol (with no command completion interrupt)",
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	"Bulk-Only Transport"
};


static void
usb_msc_destroy (usbdev_t *dev)
{
	if (dev->data) {
		if (MSC_INST (dev)->usbdisk_created && usbdisk_remove)
			usbdisk_remove (dev);
		free (dev->data);
	}
	dev->data = 0;
}

static void
usb_msc_poll (usbdev_t *dev)
{
}

const int DEV_RESET = 0xff;
const int GET_MAX_LUN = 0xfe;

const unsigned int cbw_signature = 0x43425355;
const unsigned int csw_signature = 0x53425355;

typedef struct {
	unsigned int dCBWSignature;
	unsigned int dCBWTag;
	unsigned int dCBWDataTransferLength;
	unsigned char bmCBWFlags;
	unsigned long bCBWLUN:4;
	unsigned long:4;
	unsigned long bCBWCBLength:5;
	unsigned long:3;
	unsigned char CBWCB[31 - 15];
} __attribute__ ((packed)) cbw_t;

typedef struct {
	unsigned int dCSWSignature;
	unsigned int dCSWTag;
	unsigned int dCSWDataResidue;
	unsigned char bCSWStatus;
} __attribute__ ((packed)) csw_t;

enum {
	/*
	 * MSC commands can be
	 *   successful,
	 *   fail with proper response or
	 *   fail totally, which results in detaching of the usb device.
	 * In the latter case the caller has to make sure, that he won't
	 * use the device any more.
	 */
	MSC_COMMAND_OK = 0, MSC_COMMAND_FAIL, MSC_COMMAND_DETACHED
};

static int
request_sense (usbdev_t *dev);

static int
reset_transport (usbdev_t *dev)
{
	dev_req_t dr;
	memset (&dr, 0, sizeof (dr));
	dr.bmRequestType = 0;
	dr.data_dir = host_to_device;
#ifndef QEMU
	dr.req_type = class_type;
	dr.req_recp = iface_recp;
#endif
	dr.bRequest = DEV_RESET;
	dr.wValue = 0;
	dr.wIndex = 0;
	dr.wLength = 0;

	/* if any of these fails, detach device, as we are lost */
	if (dev->controller->control (dev, OUT, sizeof (dr), &dr, 0, 0) ||
			clear_stall (MSC_INST (dev)->bulk_in) ||
			clear_stall (MSC_INST (dev)->bulk_out)) {
		usb_debug ("Detaching unresponsive device.\n");
		usb_detach_device (dev->controller, dev->address);
		return MSC_COMMAND_DETACHED;
	}
	/* return fail as we are only called in case of failure */
	return MSC_COMMAND_FAIL;
}

/* device may stall this command, so beware! */
static int
get_max_luns (usbdev_t *dev)
{
	unsigned char luns = 75;
	dev_req_t dr;
	dr.bmRequestType = 0;
	dr.data_dir = device_to_host;
#ifndef QEMU
	dr.req_type = class_type;
	dr.req_recp = iface_recp;
#endif
	dr.bRequest = GET_MAX_LUN;
	dr.wValue = 0;
	dr.wIndex = 0;
	dr.wLength = 1;
	if (dev->controller->control (dev, IN, sizeof (dr), &dr, 1, &luns)) {
		luns = 0;	// assume only 1 lun if req fails
	}
	return luns;
}

unsigned int tag;
unsigned char lun = 0;

static void
wrap_cbw (cbw_t *cbw, int datalen, cbw_direction dir, const u8 *cmd,
	  int cmdlen)
{
	memset (cbw, 0, sizeof (cbw_t));

	cbw->dCBWSignature = cbw_signature;
	cbw->dCBWTag = ++tag;
	cbw->bCBWLUN = lun;	// static value per device

	cbw->dCBWDataTransferLength = datalen;
	cbw->bmCBWFlags = dir;
	memcpy (cbw->CBWCB, cmd, sizeof (cbw->CBWCB));
	cbw->bCBWCBLength = cmdlen;
}

static int
get_csw (endpoint_t *ep, csw_t *csw)
{
	if (ep->dev->controller->bulk (ep, sizeof (csw_t), (u8 *) csw, 1)) {
		clear_stall (ep);
		if (ep->dev->controller->bulk
				(ep, sizeof (csw_t), (u8 *) csw, 1)) {
			return reset_transport (ep->dev);
		}
	}
	if (csw->dCSWTag != tag) {
		return reset_transport (ep->dev);
	}
	return MSC_COMMAND_OK;
}

static int
execute_command (usbdev_t *dev, cbw_direction dir, const u8 *cb, int cblen,
		 u8 *buf, int buflen, int residue_ok)
{
	cbw_t cbw;
	csw_t csw;

	int always_succeed = 0;
	if ((cb[0] == 0x1b) && (cb[4] == 1)) {	//start command, always succeed
		always_succeed = 1;
	}
	wrap_cbw (&cbw, buflen, dir, cb, cblen);
	if (dev->controller->
	    bulk (MSC_INST (dev)->bulk_out, sizeof (cbw), (u8 *) &cbw, 0)) {
		return reset_transport (dev);
	}
	if (buflen > 0) {
		if (dir == cbw_direction_data_in) {
			if (dev->controller->
			    bulk (MSC_INST (dev)->bulk_in, buflen, buf, 0))
				clear_stall (MSC_INST (dev)->bulk_in);
		} else {
			if (dev->controller->
			    bulk (MSC_INST (dev)->bulk_out, buflen, buf, 0))
				clear_stall (MSC_INST (dev)->bulk_out);
		}
	}
	int ret = get_csw (MSC_INST (dev)->bulk_in, &csw);
	if (ret) {
		return ret;
	} else if (always_succeed == 1) {
		/* return success, regardless of message */
		return MSC_COMMAND_OK;
	} else if (csw.bCSWStatus == 2) {
		/* phase error, reset transport */
		return reset_transport (dev);
	} else if (csw.bCSWStatus == 0) {
		if ((csw.dCSWDataResidue == 0) || residue_ok)
			/* no error, exit */
			return MSC_COMMAND_OK;
		else
			/* missed some bytes */
			return MSC_COMMAND_FAIL;
	} else {
		if (cb[0] == 0x03)
			/* requesting sense failed, that's bad */
			return MSC_COMMAND_FAIL;
		/* error "check condition" or reserved error */
		ret = request_sense (dev);
		/* return fail or the status of request_sense if it's worse */
		return ret ? ret : MSC_COMMAND_FAIL;
	}
}

typedef struct {
	unsigned char command;	//0
	unsigned char res1;	//1
	unsigned int block;	//2-5
	unsigned char res2;	//6
	unsigned short numblocks;	//7-8
	unsigned char res3;	//9 - the block is 10 bytes long
} __attribute__ ((packed)) cmdblock_t;

typedef struct {
	unsigned char command;	//0
	unsigned char res1;	//1
	unsigned char res2;	//2
	unsigned char res3;	//3
	unsigned char lun;	//4
	unsigned char res4;	//5
} __attribute__ ((packed)) cmdblock6_t;

/**
 * Like readwrite_blocks, but for soft-sectors of 512b size. Converts the
 * start and count from 512b units.
 * Start and count must be aligned so that they match the native
 * sector size.
 *
 * @param dev device to access
 * @param start first sector to access
 * @param n number of sectors to access
 * @param dir direction of access: cbw_direction_data_in == read, cbw_direction_data_out == write
 * @param buf buffer to read into or write from. Must be at least n*512 bytes
 * @return 0 on success, 1 on failure
 */
int
readwrite_blocks_512 (usbdev_t *dev, int start, int n,
	cbw_direction dir, u8 *buf)
{
	int blocksize_divider = MSC_INST(dev)->blocksize / 512;
	return readwrite_blocks (dev, start / blocksize_divider,
		n / blocksize_divider, dir, buf);
}

/**
 * Reads or writes a number of sequential blocks on a USB storage device.
 * As it uses the READ(10) SCSI-2 command, it's limited to storage devices
 * of at most 2TB. It assumes sectors of 512 bytes.
 *
 * @param dev device to access
 * @param start first sector to access
 * @param n number of sectors to access
 * @param dir direction of access: cbw_direction_data_in == read, cbw_direction_data_out == write
 * @param buf buffer to read into or write from. Must be at least n*sectorsize bytes
 * @return 0 on success, 1 on failure
 */
int
readwrite_blocks (usbdev_t *dev, int start, int n, cbw_direction dir, u8 *buf)
{
	cmdblock_t cb;
	memset (&cb, 0, sizeof (cb));
	if (dir == cbw_direction_data_in) {
		// read
		cb.command = 0x28;
	} else {
		// write
		cb.command = 0x2a;
	}
	cb.block = htonl (start);
	cb.numblocks = htonw (n);

	return execute_command (dev, dir, (u8 *) &cb, sizeof (cb), buf,
				n * MSC_INST(dev)->blocksize, 0)
		!= MSC_COMMAND_OK ? 1 : 0;
}

/* Only request it, we don't interpret it.
   On certain errors, that's necessary to get devices out of
   a special state called "Contingent Allegiance Condition" */
static int
request_sense (usbdev_t *dev)
{
	u8 buf[19];
	cmdblock6_t cb;
	memset (&cb, 0, sizeof (cb));
	cb.command = 0x3;

	return execute_command (dev, cbw_direction_data_in, (u8 *) &cb,
				sizeof (cb), buf, 19, 1);
}

static int
test_unit_ready (usbdev_t *dev)
{
	cmdblock6_t cb;
	memset (&cb, 0, sizeof (cb));	// full initialization for T-U-R
	return execute_command (dev, cbw_direction_data_out, (u8 *) &cb,
				sizeof (cb), 0, 0, 0);
}

static int
spin_up (usbdev_t *dev)
{
	cmdblock6_t cb;
	memset (&cb, 0, sizeof (cb));
	cb.command = 0x1b;
	cb.lun = 1;
	return execute_command (dev, cbw_direction_data_out, (u8 *) &cb,
				sizeof (cb), 0, 0, 0);
}

static int
read_capacity (usbdev_t *dev)
{
	cmdblock_t cb;
	memset (&cb, 0, sizeof (cb));
	cb.command = 0x25;	// read capacity
	u32 buf[2];

	usb_debug ("Reading capacity of mass storage device.\n");
	int count = 0, ret;
	while (count++ < 20) {
		switch (ret = execute_command
				(dev, cbw_direction_data_in, (u8 *) &cb,
				 sizeof (cb), (u8 *)buf, 8, 0)) {
		case MSC_COMMAND_OK:
			break;
		case MSC_COMMAND_FAIL:
			continue;
		default: /* if it's worse return */
			return ret;
		}
		break;
	}
	if (count >= 20) {
		// still not successful, assume 2tb in 512byte sectors, which is just the same garbage as any other number, but probably more usable.
		usb_debug ("  assuming 2 TB with 512-byte sectors as READ CAPACITY didn't answer.\n");
		MSC_INST (dev)->numblocks = 0xffffffff;
		MSC_INST (dev)->blocksize = 512;
	} else {
		MSC_INST (dev)->numblocks = ntohl(buf[0]) + 1;
		MSC_INST (dev)->blocksize = ntohl(buf[1]);
	}
	usb_debug ("  %d %d-byte sectors (%d MB)\n", MSC_INST (dev)->numblocks,
		MSC_INST (dev)->blocksize,
		/* round down high block counts to avoid integer overflow */
		MSC_INST (dev)->numblocks > 1000000
			? (MSC_INST (dev)->numblocks / 1000) * MSC_INST (dev)->blocksize / 1000 :
		MSC_INST (dev)->numblocks * MSC_INST (dev)->blocksize / 1000 / 1000);
	return MSC_COMMAND_OK;
}

void
usb_msc_init (usbdev_t *dev)
{
	int i, timeout;

	/* init .data before setting .destroy */
	dev->data = NULL;

	dev->destroy = usb_msc_destroy;
	dev->poll = usb_msc_poll;

	configuration_descriptor_t *cd =
		(configuration_descriptor_t *) dev->configuration;
	interface_descriptor_t *interface =
		(interface_descriptor_t *) (((char *) cd) + cd->bLength);

	usb_debug ("  it uses %s command set\n",
		msc_subclass_strings[interface->bInterfaceSubClass]);
	usb_debug ("  it uses %s protocol\n",
		msc_protocol_strings[interface->bInterfaceProtocol]);


	if (interface->bInterfaceProtocol != 0x50) {
		usb_debug ("  Protocol not supported.\n");
		return;
	}

	if ((interface->bInterfaceSubClass != 2) &&	// ATAPI 8020
		(interface->bInterfaceSubClass != 5) &&	// ATAPI 8070
		(interface->bInterfaceSubClass != 6)) {	// SCSI
		/* Other protocols, such as ATAPI don't seem to be very popular. looks like ATAPI would be really easy to add, if necessary. */
		usb_debug ("  Interface SubClass not supported.\n");
		return;
	}

	dev->data = malloc (sizeof (usbmsc_inst_t));
	if (!dev->data)
		fatal("Not enough memory for USB MSC device.\n");

	MSC_INST (dev)->protocol = interface->bInterfaceSubClass;
	MSC_INST (dev)->bulk_in = 0;
	MSC_INST (dev)->bulk_out = 0;
	MSC_INST (dev)->usbdisk_created = 0;

	for (i = 1; i <= dev->num_endp; i++) {
		if (dev->endpoints[i].endpoint == 0)
			continue;
		if (dev->endpoints[i].type != BULK)
			continue;
		if ((dev->endpoints[i].direction == IN)
		    && (MSC_INST (dev)->bulk_in == 0))
			MSC_INST (dev)->bulk_in = &dev->endpoints[i];
		if ((dev->endpoints[i].direction == OUT)
		    && (MSC_INST (dev)->bulk_out == 0))
			MSC_INST (dev)->bulk_out = &dev->endpoints[i];
	}

	if (MSC_INST (dev)->bulk_in == 0) {
		usb_debug("couldn't find bulk-in endpoint");
		return;
	}
	if (MSC_INST (dev)->bulk_out == 0) {
		usb_debug("couldn't find bulk-out endpoint");
		return;
	}
	usb_debug ("  using endpoint %x as in, %x as out\n",
		MSC_INST (dev)->bulk_in->endpoint,
		MSC_INST (dev)->bulk_out->endpoint);

	usb_debug ("  has %d luns\n", get_max_luns (dev) + 1);

	usb_debug ("  Waiting for device to become ready...");
	timeout = 30 * 10; /* SCSI/ATA specs say we have to wait up to 30s. Ugh */
	while (timeout--) {
		switch (test_unit_ready (dev)) {
		case MSC_COMMAND_OK:
			break;
		case MSC_COMMAND_FAIL:
			mdelay (100);
			if (!(timeout % 10))
				usb_debug (".");
			continue;
		default: /* if it's worse return */
			return;
		}
		break;
	}
	if (timeout < 0) {
		usb_debug ("timeout. Device not ready. Still trying...\n");
	} else {
		usb_debug ("ok.\n");
	}

	usb_debug ("  spin up");
	for (i = 0; i < 30; i++) {
		usb_debug (".");
		switch (spin_up (dev)) {
		case MSC_COMMAND_OK:
			usb_debug (" OK.");
			break;
		case MSC_COMMAND_FAIL:
			mdelay (100);
			continue;
		default: /* if it's worse return */
			return;
		}
		break;
	}
	usb_debug ("\n");

	if ((read_capacity (dev) == MSC_COMMAND_OK) && usbdisk_create) {
		usbdisk_create (dev);
		MSC_INST (dev)->usbdisk_created = 1;
	}
}
