/*
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

//#define USB_DEBUG
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
usb_msc_create_disk(usbdev_t *dev)
{
	if (usbdisk_create) {
		usbdisk_create(dev);
		MSC_INST(dev)->usbdisk_created = 1;
	}
}

static void
usb_msc_remove_disk(usbdev_t *dev)
{
	if (MSC_INST(dev)->usbdisk_created && usbdisk_remove) {
		usbdisk_remove(dev);
		MSC_INST(dev)->usbdisk_created = 0;
	}
}

static void
usb_msc_destroy(usbdev_t *dev)
{
	if (dev->data) {
		usb_msc_remove_disk(dev);
		free(dev->data);
	}
	dev->data = 0;
}

const int DEV_RESET = 0xff;
const int GET_MAX_LUN = 0xfe;
/* Many USB3 devices do not work with large transfer requests.
 * Limit the request size to 64KB chunks to ensure maximum compatibility. */
const int MAX_CHUNK_BYTES = 1024 * 64;

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
} __packed cbw_t;

typedef struct {
	unsigned int dCSWSignature;
	unsigned int dCSWTag;
	unsigned int dCSWDataResidue;
	unsigned char bCSWStatus;
} __packed csw_t;

enum {
	/*
	 * MSC commands can be
	 *   successful,
	 *   fail with proper response or
	 *   fail totally, which results in detaching of the USB device
	 *   and immediate cleanup of the usbdev_t structure.
	 * In the latter case the caller has to make sure, that he won't
	 * use the device any more.
	 */
	MSC_COMMAND_OK = 0, MSC_COMMAND_FAIL, MSC_COMMAND_DETACHED
};

static int
request_sense(usbdev_t *dev);
static int
request_sense_no_media(usbdev_t *dev);
static void
usb_msc_poll(usbdev_t *dev);

static int
reset_transport(usbdev_t *dev)
{
	dev_req_t dr;
	memset(&dr, 0, sizeof(dr));
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

	if (MSC_INST(dev)->quirks & USB_MSC_QUIRK_NO_RESET)
		return MSC_COMMAND_FAIL;

	/* if any of these fails, detach device, as we are lost */
	if (dev->controller->control(dev, OUT, sizeof(dr), &dr, 0, 0) < 0 ||
			clear_stall(MSC_INST(dev)->bulk_in) ||
			clear_stall(MSC_INST(dev)->bulk_out)) {
		usb_debug("Detaching unresponsive device.\n");
		usb_detach_device(dev->controller, dev->address);
		return MSC_COMMAND_DETACHED;
	}
	/* return fail as we are only called in case of failure */
	return MSC_COMMAND_FAIL;
}

/* device may stall this command, so beware! */
static void
initialize_luns(usbdev_t *dev)
{
	usbmsc_inst_t *msc = MSC_INST(dev);
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
	if (MSC_INST(dev)->quirks & USB_MSC_QUIRK_NO_LUNS ||
	    dev->controller->control(dev, IN, sizeof(dr), &dr,
			sizeof(msc->num_luns), &msc->num_luns) < 0)
		msc->num_luns = 0;	/* assume only 1 lun if req fails */
	msc->num_luns++;	/* Get Max LUN returns number of last LUN */
	msc->lun = 0;
}

unsigned int tag;

static void
wrap_cbw(cbw_t *cbw, int datalen, cbw_direction dir, const u8 *cmd,
	  int cmdlen, u8 lun)
{
	memset(cbw, 0, sizeof(cbw_t));

	/* commands are typically shorter, but we don't want overflows */
	if (cmdlen > sizeof(cbw->CBWCB)) {
		cmdlen = sizeof(cbw->CBWCB);
	}

	cbw->dCBWSignature = cbw_signature;
	cbw->dCBWTag = ++tag;
	cbw->bCBWLUN = lun;

	cbw->dCBWDataTransferLength = datalen;
	cbw->bmCBWFlags = dir;
	memcpy(cbw->CBWCB, cmd, cmdlen);
	cbw->bCBWCBLength = cmdlen;
}

static int
get_csw(endpoint_t *ep, csw_t *csw)
{
	hci_t *ctrlr = ep->dev->controller;
	int ret = ctrlr->bulk(ep, sizeof(csw_t), (u8 *) csw, 1);

	/* Some broken sticks send a zero-length packet at the end of their data
	   transfer which would show up here. Skip it to get the actual CSW. */
	if (ret == 0)
		ret = ctrlr->bulk(ep, sizeof(csw_t), (u8 *) csw, 1);

	if (ret < 0) {
		clear_stall(ep);
		ret = ctrlr->bulk(ep, sizeof(csw_t), (u8 *) csw, 1);
		if (ret < 0)
			return reset_transport(ep->dev);
	}
	if (ret != sizeof(csw_t) || csw->dCSWTag != tag ||
	    csw->dCSWSignature != csw_signature) {
		usb_debug("MSC: received malformed CSW\n");
		return reset_transport(ep->dev);
	}
	return MSC_COMMAND_OK;
}

static int
execute_command(usbdev_t *dev, cbw_direction dir, const u8 *cb, int cblen,
		 u8 *buf, int buflen, int residue_ok)
{
	cbw_t cbw;
	csw_t csw;

	int always_succeed = 0;
	if ((cb[0] == 0x1b) && (cb[4] == 1)) {	//start command, always succeed
		always_succeed = 1;
	}
	wrap_cbw(&cbw, buflen, dir, cb, cblen, MSC_INST(dev)->lun);
	if (dev->controller->
	    bulk(MSC_INST(dev)->bulk_out, sizeof(cbw), (u8 *) &cbw, 0) < 0) {
		return reset_transport(dev);
	}
	if (buflen > 0) {
		if (dir == cbw_direction_data_in) {
			if (dev->controller->
			    bulk(MSC_INST(dev)->bulk_in, buflen, buf, 0) < 0)
				clear_stall(MSC_INST(dev)->bulk_in);
		} else {
			if (dev->controller->
			    bulk(MSC_INST(dev)->bulk_out, buflen, buf, 0) < 0)
				clear_stall(MSC_INST(dev)->bulk_out);
		}
	}
	int ret = get_csw(MSC_INST(dev)->bulk_in, &csw);
	if (ret) {
		return ret;
	} else if (always_succeed == 1) {
		/* return success, regardless of message */
		return MSC_COMMAND_OK;
	} else if (csw.bCSWStatus == 2) {
		/* phase error, reset transport */
		return reset_transport(dev);
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
		else if (cb[0] == 0)
			/* If command was TEST UNIT READY determine if the
			 * device is of removable type indicating no media
			 * found. */
			return request_sense_no_media(dev);
		/* error "check condition" or reserved error */
		ret = request_sense(dev);
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
	unsigned char control;	//9 - the block is 10 bytes long
} __packed cmdblock_t;

typedef struct {
	unsigned char command;	//0
	unsigned char res1;	//1
	unsigned char res2;	//2
	unsigned char res3;	//3
	union {			//4
		struct {
			unsigned long start:1;  // for START STOP UNIT
			unsigned long:7;
		};
		unsigned char length;		// for REQUEST SENSE
	};
	unsigned char control;	//5
} __packed cmdblock6_t;

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
readwrite_blocks_512(usbdev_t *dev, int start, int n,
	cbw_direction dir, u8 *buf)
{
	int blocksize_divider = MSC_INST(dev)->blocksize / 512;
	return readwrite_blocks(dev, start / blocksize_divider,
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
static int
readwrite_chunk(usbdev_t *dev, int start, int n, cbw_direction dir, u8 *buf)
{
	cmdblock_t cb;
	memset(&cb, 0, sizeof(cb));
	if (dir == cbw_direction_data_in) {
		// read
		cb.command = 0x28;
	} else {
		// write
		cb.command = 0x2a;
	}
	cb.block = htonl(start);
	cb.numblocks = htonw(n);

	return execute_command(dev, dir, (u8 *) &cb, sizeof(cb), buf,
				n * MSC_INST(dev)->blocksize, 0)
		!= MSC_COMMAND_OK ? 1 : 0;
}

/**
 * Reads or writes a number of sequential blocks on a USB storage device
 * that is split into MAX_CHUNK_BYTES size requests.
 *
 * As it uses the READ(10) SCSI-2 command, it's limited to storage devices
 * of at most 2TB. It assumes sectors of 512 bytes.
 *
 * @param dev device to access
 * @param start first sector to access
 * @param n number of sectors to access
 * @param dir direction of access: cbw_direction_data_in == read,
 *                                 cbw_direction_data_out == write
 * @param buf buffer to read into or write from.
 *            Must be at least n*sectorsize bytes
 * @return 0 on success, 1 on failure
 */
int
readwrite_blocks(usbdev_t *dev, int start, int n, cbw_direction dir, u8 *buf)
{
	int chunk_size = MAX_CHUNK_BYTES / MSC_INST(dev)->blocksize;
	int chunk;

	/* Read as many full chunks as needed. */
	for (chunk = 0; chunk < (n / chunk_size); chunk++) {
		if (readwrite_chunk(dev, start + (chunk * chunk_size),
				     chunk_size, dir,
				     buf + (chunk * MAX_CHUNK_BYTES))
		    != MSC_COMMAND_OK)
			return 1;
	}

	/* Read any remaining partial chunk at the end. */
	if (n % chunk_size) {
		if (readwrite_chunk(dev, start + (chunk * chunk_size),
				     n % chunk_size, dir,
				     buf + (chunk * MAX_CHUNK_BYTES))
		    != MSC_COMMAND_OK)
			return 1;
	}

	return 0;
}

/* Only request it, we don't interpret it.
   On certain errors, that's necessary to get devices out of
   a special state called "Contingent Allegiance Condition" */
static int
request_sense(usbdev_t *dev)
{
	u8 buf[19];
	cmdblock6_t cb;
	memset(&cb, 0, sizeof(cb));
	cb.command = 0x3;
	cb.length = sizeof(buf);

	return execute_command(dev, cbw_direction_data_in, (u8 *) &cb,
				sizeof(cb), buf, sizeof(buf), 1);
}

static int request_sense_no_media(usbdev_t *dev)
{
	u8 buf[19];
	int ret;
	cmdblock6_t cb;
	memset(&cb, 0, sizeof(cb));
	cb.command = 0x3;
	cb.length = sizeof(buf);

	ret = execute_command(dev, cbw_direction_data_in, (u8 *) &cb,
				sizeof(cb), buf, sizeof(buf), 1);

	if (ret)
		return ret;

	/* Check if sense key is set to NOT READY. */
	if ((buf[2] & 0xf) != 2)
		return MSC_COMMAND_FAIL;

	/* Check if additional sense code is 0x3a. */
	if (buf[12] != 0x3a)
		return MSC_COMMAND_FAIL;

	/* No media is present. Return MSC_COMMAND_OK while marking the disk
	 * not ready. */
	usb_debug("Empty media found.\n");
	MSC_INST(dev)->ready = USB_MSC_NOT_READY;
	return MSC_COMMAND_OK;
}

static int
test_unit_ready(usbdev_t *dev)
{
	cmdblock6_t cb;
	memset(&cb, 0, sizeof(cb));	// full initialization for T-U-R
	return execute_command(dev, cbw_direction_data_out, (u8 *) &cb,
				sizeof(cb), 0, 0, 0);
}

static int
spin_up(usbdev_t *dev)
{
	cmdblock6_t cb;
	memset(&cb, 0, sizeof(cb));
	cb.command = 0x1b;
	cb.start = 1;
	return execute_command(dev, cbw_direction_data_out, (u8 *) &cb,
				sizeof(cb), 0, 0, 0);
}

static int
read_capacity(usbdev_t *dev)
{
	cmdblock_t cb;
	memset(&cb, 0, sizeof(cb));
	cb.command = 0x25;	// read capacity
	u32 buf[2];

	usb_debug("Reading capacity of mass storage device.\n");
	int count = 0, ret;
	while (count++ < 20) {
		switch (ret = execute_command
				(dev, cbw_direction_data_in, (u8 *) &cb,
				 sizeof(cb), (u8 *)buf, 8, 0)) {
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
		usb_debug("  assuming 2 TB with 512-byte sectors as READ CAPACITY didn't answer.\n");
		MSC_INST(dev)->numblocks = 0xffffffff;
		MSC_INST(dev)->blocksize = 512;
	} else {
		MSC_INST(dev)->numblocks = ntohl(buf[0]) + 1;
		MSC_INST(dev)->blocksize = ntohl(buf[1]);
	}
	usb_debug("  %d %d-byte sectors (%d MB)\n", MSC_INST(dev)->numblocks,
		MSC_INST(dev)->blocksize,
		/* round down high block counts to avoid integer overflow */
		MSC_INST(dev)->numblocks > 1000000
			? (MSC_INST(dev)->numblocks / 1000) * MSC_INST(dev)->blocksize / 1000 :
		MSC_INST(dev)->numblocks * MSC_INST(dev)->blocksize / 1000 / 1000);
	return MSC_COMMAND_OK;
}

static int
usb_msc_test_unit_ready(usbdev_t *dev)
{
	int i;
	time_t start_time_secs;
	struct timeval tv;
	/* SCSI/ATA specs say we have to wait up to 30s, but most devices
	 * are ready much sooner. Use a 5 sec timeout to better accommodate
	 * devices which fail to respond. */
	const int timeout_secs = 5;

	usb_debug("  Waiting for device to become ready...");

	/* Initially mark the device ready. */
	MSC_INST(dev)->ready = USB_MSC_READY;
	gettimeofday(&tv, NULL);
	start_time_secs = tv.tv_sec;

	while (tv.tv_sec - start_time_secs < timeout_secs) {
		switch (test_unit_ready(dev)) {
		case MSC_COMMAND_OK:
			break;
		case MSC_COMMAND_FAIL:
			mdelay(100);
			usb_debug(".");
			gettimeofday(&tv, NULL);
			continue;
		default:
			/* Device detached, return immediately */
			return USB_MSC_DETACHED;
		}
		break;
	}
	if (!(tv.tv_sec - start_time_secs < timeout_secs)) {
		usb_debug("timeout. Device not ready.\n");
		MSC_INST(dev)->ready = USB_MSC_NOT_READY;
	}

	/* Don't bother spinning up the storage device if the device is not
	 * ready. This can happen when empty card readers are present.
	 * Polling will pick it back up if readiness changes. */
	if (!MSC_INST(dev)->ready)
		return MSC_INST(dev)->ready;

	usb_debug("ok.\n");

	usb_debug("  spin up");
	for (i = 0; i < 30; i++) {
		usb_debug(".");
		switch (spin_up(dev)) {
		case MSC_COMMAND_OK:
			usb_debug(" OK.");
			break;
		case MSC_COMMAND_FAIL:
			mdelay(100);
			continue;
		default:
			/* Device detached, return immediately */
			return USB_MSC_DETACHED;
		}
		break;
	}
	usb_debug("\n");

	if (read_capacity(dev) == MSC_COMMAND_DETACHED)
		return USB_MSC_DETACHED;

	return MSC_INST(dev)->ready;
}

void
usb_msc_init(usbdev_t *dev)
{
	configuration_descriptor_t *cd =
		(configuration_descriptor_t *) dev->configuration;
	interface_descriptor_t *interface =
		(interface_descriptor_t *) (((char *) cd) + cd->bLength);

	usb_debug("  it uses %s command set\n",
		msc_subclass_strings[interface->bInterfaceSubClass]);
	usb_debug("  it uses %s protocol\n",
		msc_protocol_strings[interface->bInterfaceProtocol]);

	if (interface->bInterfaceProtocol != 0x50) {
		usb_debug("  Protocol not supported.\n");
		usb_detach_device(dev->controller, dev->address);
		return;
	}

	if ((interface->bInterfaceSubClass != 2) &&	// ATAPI 8020
		(interface->bInterfaceSubClass != 5) &&	// ATAPI 8070
		(interface->bInterfaceSubClass != 6)) {	// SCSI
		/* Other protocols, such as ATAPI don't seem to be very popular. looks like ATAPI would be really easy to add, if necessary. */
		usb_debug("  Interface SubClass not supported.\n");
		usb_detach_device(dev->controller, dev->address);
		return;
	}

	usb_msc_force_init(dev, 0);
}

void usb_msc_force_init(usbdev_t *dev, u32 quirks)
{
	int i;

	/* init .data before setting .destroy */
	dev->data = NULL;

	dev->destroy = usb_msc_destroy;
	dev->poll = usb_msc_poll;

	dev->data = malloc(sizeof(usbmsc_inst_t));
	if (!dev->data)
		fatal("Not enough memory for USB MSC device.\n");

	MSC_INST(dev)->bulk_in = 0;
	MSC_INST(dev)->bulk_out = 0;
	MSC_INST(dev)->usbdisk_created = 0;
	MSC_INST(dev)->quirks = quirks;

	for (i = 1; i <= dev->num_endp; i++) {
		if (dev->endpoints[i].endpoint == 0)
			continue;
		if (dev->endpoints[i].type != BULK)
			continue;
		if ((dev->endpoints[i].direction == IN)
		    && (MSC_INST(dev)->bulk_in == 0))
			MSC_INST(dev)->bulk_in = &dev->endpoints[i];
		if ((dev->endpoints[i].direction == OUT)
		    && (MSC_INST(dev)->bulk_out == 0))
			MSC_INST(dev)->bulk_out = &dev->endpoints[i];
	}

	if (MSC_INST(dev)->bulk_in == 0) {
		usb_debug("couldn't find bulk-in endpoint.\n");
		usb_detach_device(dev->controller, dev->address);
		return;
	}
	if (MSC_INST(dev)->bulk_out == 0) {
		usb_debug("couldn't find bulk-out endpoint.\n");
		usb_detach_device(dev->controller, dev->address);
		return;
	}
	usb_debug("  using endpoint %x as in, %x as out\n",
		MSC_INST(dev)->bulk_in->endpoint,
		MSC_INST(dev)->bulk_out->endpoint);

	/* Some sticks need a little more time to get ready after SET_CONFIG. */
	udelay(50);

	initialize_luns(dev);
	usb_debug("  has %d luns\n", MSC_INST(dev)->num_luns);

	/* Test if unit is ready (nothing to do if it isn't). */
	if (usb_msc_test_unit_ready(dev) != USB_MSC_READY)
		return;

	/* Create the disk. */
	usb_msc_create_disk(dev);
}

static void
usb_msc_poll(usbdev_t *dev)
{
	usbmsc_inst_t *msc = MSC_INST(dev);
	int prev_ready = msc->ready;

	if (usb_msc_test_unit_ready(dev) == USB_MSC_DETACHED)
		return;

	if (!prev_ready && msc->ready) {
		usb_debug("USB msc: not ready -> ready (lun %d)\n", msc->lun);
		usb_msc_create_disk(dev);
	} else if (prev_ready && !msc->ready) {
		usb_debug("USB msc: ready -> not ready (lun %d)\n", msc->lun);
		usb_msc_remove_disk(dev);
	} else if (!prev_ready && !msc->ready) {
		u8 new_lun = (msc->lun + 1) % msc->num_luns;
		usb_debug("USB msc: not ready (lun %d) -> lun %d\n", msc->lun,
			  new_lun);
		msc->lun = new_lun;
	}
}
