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

#include <arch/endian.h>
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
	if (usbdisk_remove)
		usbdisk_remove (dev);
	free (dev->data);
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
} __attribute__ ((packed))
     cbw_t;

     typedef struct {
	     unsigned int dCSWSignature;
	     unsigned int dCSWTag;
	     unsigned int dCSWDataResidue;
	     unsigned char bCSWStatus;
     } __attribute__ ((packed))
     csw_t;

     static void
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
	dev->controller->control (dev, OUT, sizeof (dr), &dr, 0, 0);
	clear_stall (MSC_INST (dev)->bulk_in);
	clear_stall (MSC_INST (dev)->bulk_out);
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

int tag;
int lun = 0;

static void
wrap_cbw (cbw_t *cbw, int datalen, cbw_direction dir, const u8 *cmd,
	  int cmdlen)
{
	memset (cbw, 0, sizeof (cbw_t));

	cbw->dCBWSignature = cbw_signature;
	cbw->dCBWTag = tag++;
	cbw->bCBWLUN = lun;	// static value per device

	cbw->dCBWDataTransferLength = datalen;
	cbw->bmCBWFlags = dir;
	memcpy (cbw->CBWCB, cmd, sizeof (cbw->CBWCB));
	cbw->bCBWCBLength = cmdlen;
}

static void
get_csw (endpoint_t *ep, csw_t *csw)
{
	ep->dev->controller->bulk (ep, sizeof (csw_t), (u8 *) csw, 1);
}

static int
execute_command (usbdev_t *dev, cbw_direction dir, const u8 *cb, int cblen,
		 u8 *buf, int buflen)
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
		clear_stall (MSC_INST (dev)->bulk_out);
		return 1;
	}
	mdelay (10);
	if (dir == cbw_direction_data_in) {
		if (dev->controller->
		    bulk (MSC_INST (dev)->bulk_in, buflen, buf, 0)) {
			clear_stall (MSC_INST (dev)->bulk_in);
			return 1;
		}
	} else {
		if (dev->controller->
		    bulk (MSC_INST (dev)->bulk_out, buflen, buf, 0)) {
			clear_stall (MSC_INST (dev)->bulk_out);
			return 1;
		}
	}
	get_csw (MSC_INST (dev)->bulk_in, &csw);
	if (always_succeed == 1) {
		// return success, regardless of message
		return 0;
	}
	if (csw.bCSWStatus == 2) {
		// phase error, reset transport
		reset_transport (dev);
		return 1;
	}
	if (csw.bCSWStatus == 0) {
		// no error, exit
		return 0;
	}
	// error "check condition" or reserved error
	return 1;
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
 * Reads or writes a number of sequential blocks on a USB storage device.
 * As it uses the READ(10) SCSI-2 command, it's limited to storage devices
 * of at most 2TB. It assumes sectors of 512 bytes.
 *
 * @param dev device to access
 * @param start first sector to access
 * @param n number of sectors to access
 * @param dir direction of access: cbw_direction_data_in == read, cbw_direction_data_out == write
 * @param buf buffer to read into or write from. Must be at least n*512 bytes
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
	cb.block = ntohl (start);
	cb.numblocks = ntohw (n);
	return execute_command (dev, dir, (u8 *) &cb, sizeof (cb), buf,
				n * 512);
}

static int
test_unit_ready (usbdev_t *dev)
{
	cmdblock6_t cb;
	memset (&cb, 0, sizeof (cb));	// full initialization for T-U-R
	return execute_command (dev, cbw_direction_data_out, (u8 *) &cb,
				sizeof (cb), 0, 0);
}

static int
spin_up (usbdev_t *dev)
{
	cmdblock6_t cb;
	memset (&cb, 0, sizeof (cb));
	cb.command = 0x1b;
	cb.lun = 1;
	return execute_command (dev, cbw_direction_data_out, (u8 *) &cb,
				sizeof (cb), 0, 0);
}

static void
read_capacity (usbdev_t *dev)
{
	cmdblock_t cb;
	memset (&cb, 0, sizeof (cb));
	cb.command = 0x25;	// read capacity
	u8 buf[8];
	int count = 0;
	while ((count++ < 20)
	       &&
	       (execute_command
		(dev, cbw_direction_data_in, (u8 *) &cb, sizeof (cb), buf,
		 8) == 1));
	if (count >= 20) {
		// still not successful, assume 2tb in 512byte sectors, which is just the same garbage as any other number, but probably reasonable.
		printf ("assuming 2TB in 512byte sectors as READ CAPACITY didn't answer.\n");
		MSC_INST (dev)->numblocks = 0xffffffff;
		MSC_INST (dev)->blocksize = 512;
	} else {
		MSC_INST (dev)->numblocks = ntohl (*(u32 *) buf) + 1;
		MSC_INST (dev)->blocksize = ntohl (*(u32 *) (buf + 4));
	}
	printf ("  has %d blocks sized %db\n", MSC_INST (dev)->numblocks,
		MSC_INST (dev)->blocksize);
}

void
usb_msc_init (usbdev_t *dev)
{
	int i, timeout;

	dev->destroy = usb_msc_destroy;
	dev->poll = usb_msc_poll;

	configuration_descriptor_t *cd =
		(configuration_descriptor_t *) dev->configuration;
	interface_descriptor_t *interface =
		(interface_descriptor_t *) (((char *) cd) + cd->bLength);

	printf ("  it uses %s command set\n",
		msc_subclass_strings[interface->bInterfaceSubClass]);
	printf ("  it uses %s protocol\n",
		msc_protocol_strings[interface->bInterfaceProtocol]);

	if ((interface->bInterfaceProtocol != 0x50)
	    || (interface->bInterfaceSubClass != 6)) {
		/* Other protocols, such as ATAPI don't seem to be very popular. looks like ATAPI would be really easy to add, if necessary. */
		printf ("  Only SCSI over Bulk is supported.\n");
		return;
	}

	dev->data = malloc (sizeof (usbmsc_inst_t));
	MSC_INST (dev)->bulk_in = 0;
	MSC_INST (dev)->bulk_out = 0;

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

	if (MSC_INST (dev)->bulk_in == 0)
		fatal ("couldn't find bulk-in endpoint");
	if (MSC_INST (dev)->bulk_out == 0)
		fatal ("couldn't find bulk-out endpoint");
	printf ("  using endpoint %x as in, %x as out\n",
		MSC_INST (dev)->bulk_in->endpoint,
		MSC_INST (dev)->bulk_out->endpoint);

	printf ("  has %d luns\n", get_max_luns (dev) + 1);

	printf ("  Waiting for device to become ready... ");
	timeout = 10;
	while (test_unit_ready (dev) && --timeout) {
		mdelay (100);
		printf (".");
	}
	if (test_unit_ready (dev)) {
		printf ("timeout. Device not ready. Still trying...\n");
	} else {
		printf ("ok.\n");
	}

	printf ("  spin up");
	for (i = 0; i < 30; i++) {
		printf (".");
		if (!spin_up (dev)) {
			printf (" OK.");
			break;
		}
		mdelay (100);
	}
	printf ("\n");

	read_capacity (dev);
	if (usbdisk_create)
		usbdisk_create (dev);
}
