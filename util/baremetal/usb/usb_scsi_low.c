/*******************************************************************************
 *
 *
 *	Copyright 2003 Steven James <pyro@linuxlabs.com> and
 *	LinuxLabs http://www.linuxlabs.com
 *
 *	This program is free software; you can redistribute it and/or modify
 *	it under the terms of the GNU General Public License as published by
 *	the Free Software Foundation; either version 2 of the License, or
 *	(at your option) any later version.
 *
 *	This program is distributed in the hope that it will be useful,
 *	but WITHOUT ANY WARRANTY; without even the implied warranty of
 *	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *	GNU General Public License for more details.
 *
 *	You should have received a copy of the GNU General Public License
 *	along with this program; if not, write to the Free Software
 *	Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 ******************************************************************************/

#include "debug.h"
#include "usb_scsi_low.h"

//#include <scsi/scsi.h>

#define SG_DXFER_FROM_DEV -3
#define SG_DXFER_TO_DEV -2

#define REQUEST_SENSE 0x03

#define CBW_SIG 43425355

typedef struct usb_cbw {
	unsigned int 	signature;
	unsigned int 	tag;
	unsigned int 	transfer_len;	// this is exclusive of cbw and csw

	uchar		res1:7;
	uchar 		direction:1;	// 1 = device to host (read)
	
	uchar 		lun:4;
	uchar		res:4;
	
	uchar		cbw_len:5;	// the length of the SCSI command
	uchar		res3:3;

	uchar		scsi_cmd[16];
} __attribute__ ((packed)) usb_cbw_t;

#define CSW_SIG 53425355

typedef struct usb_csw {
	unsigned int	signature;
	unsigned int	tag;
	unsigned int	residue;
	uchar		status;
} __attribute__ ((packed)) usb_csw_t;


int scsi_command( uchar device, unsigned char *cmd, int cmd_len, int direction, unsigned char *data, int data_len, char *sense_data, int sense_len)
{
	usb_cbw_t cbw;
	usb_csw_t csw;
	int ret;
	
	memset(&cbw,0,sizeof(usb_cbw_t));
	memset(&csw,0,sizeof(usb_csw_t));

	cbw.signature = CBW_SIG;
	cbw.tag = 777;

	memcpy(cbw.scsi_cmd, cmd, cmd_len);
	cbw.cbw_len = cmd_len;

	if(direction == SG_DXFER_FROM_DEV)
		cbw.direction=1;

	cbw.transfer_len = data_len;

	ret = bulk_transfer(device, 2, sizeof(cbw), (uchar *) &cbw);
	if(ret<0)
		DPRINTF("ERROR:Bulk write:\n");

	if(data_len) 
		if(cbw.direction) {
//			DPRINTF("scsi_command reading %u bytes\n", data_len);
			ret = bulk_transfer(device, 0x81, data_len, data);
//			DPRINTF("scsi_command read %u bytes\n", ret);
			if(ret<0 || ret <data_len)
				DPRINTF("ERROR:Bulk read data ret = %d\n", ret);
		} else {
//			DPRINTF("scsi_command writing %u bytes\n", data_len);
			ret = bulk_transfer(device, 0x2, data_len, data);
//			DPRINTF("scsi_command wrote %u bytes\n", ret);
			if(ret<0)
				DPRINTF("ERROR:Bulk write data\n");
		}

//	DPRINTF("scsi_command fetching csw\n");
	ret = bulk_transfer(device, 0x81, sizeof(csw), (uchar *) &csw);
//	DPRINTF("scsi_command csw is %d bytes\n", ret);
	if(ret<0 || ret < sizeof(csw)) {
		DPRINTF("ERROR: Bulk read CSW ret = %d\n", ret);
		return(-1);
	}

	if(csw.status) {
		DPRINTF("CSW: residue = %08x, status = %02x\n", csw.residue, csw.status);
		DPRINTF("Getting sense data\n");
		request_sense( device, sense_data, sense_len);
		return(-csw.status);
	}

	return(data_len - csw.residue);
}

int request_sense( uchar device, char *sense_data, int len)
{
	usb_cbw_t cbw;
	usb_csw_t csw;
	int ret;

	memset(&cbw,0,sizeof(usb_cbw_t));
	memset(&csw,0,sizeof(usb_csw_t));

	cbw.signature = CBW_SIG;
	cbw.tag = 666;

	cbw.scsi_cmd[0] = REQUEST_SENSE;
	cbw.scsi_cmd[4] = len;
	cbw.cbw_len = 6;
	cbw.direction=1;
	cbw.transfer_len = len;

	ret = bulk_transfer(device, 2, sizeof(usb_cbw_t), (uchar *) &cbw);
	if(ret<0 || ret < sizeof(usb_cbw_t))
		DPRINTF("ERROR: sense Bulk write ret = %d\n", ret);


	ret = bulk_transfer(device, 0x81, len, sense_data);
	if(ret<0 || ret < len)
		DPRINTF("ERROR: sense Bulk read data ret = %d\n", ret);

	ret = bulk_transfer(device, 0x81, sizeof(usb_csw_t), (uchar *) &csw);
	if(ret<0 || ret < sizeof(usb_csw_t))
		DPRINTF("ERROR: sense Bulk read CSW ret = %d\n", ret);

	return(-csw.status);
}
