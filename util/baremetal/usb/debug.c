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

#include "uhci.h"
#include "debug.h"

#include <asm/io.h>

#ifdef __KERNEL__
#include <linux/string.h>
#else
#include <string.h>
#endif

void dump_link( link_pointer_t *link, char *prefix)
{
	DPRINTF("%saddr: %p\n", prefix, MEM_ADDR(link->link) );
	DPRINTF("%s raw addr: %p\n", prefix, (link->link) <<4 );
	DPRINTF("%sterminate: %u\n", prefix, link->terminate);
	DPRINTF("%squeue: %u\n", prefix, link->queue);
	DPRINTF("%sdepth: %u\n", prefix, link->depth);
}

void dump_frame_list( link_pointer_t *addr, char *prefix)
{
	int i;

	DPRINTF("%sFRAMELIST:\n",prefix);

	for(i=0;i<10; i++) {
		dump_link(addr+i, prefix);
		if(addr[i].queue)
			dump_queue_head( MEM_ADDR(addr[i].link), "");
		else
			dump_td( MEM_ADDR(addr[i].link), "");
	}
}

void dump_hex(uchar *data, int len, char *prefix)
{
	int i=0;

	while(i<len) {
		if(!i%16)
			DPRINTF("\n%s %04x: ", prefix, i);
		else
			DPRINTF(": ");

		DPRINTF("%02x", data[i++]);
	}

	DPRINTF("\n");
}

void dump_uhci(unsigned short port)
{
	unsigned long value;

	DPRINTF("UHCI at %04x\n", port);
#ifdef __KERNEL__
	value = inw(port);
	DPRINTF("Command: %04x\n", value);

	value = inw(port+2);
	DPRINTF("USBSTS: %04x\n", value);

	value = inw(port+4);
	DPRINTF("USBINTR: %04x\n", value);

	value = inw(port+6);
	DPRINTF("FRNUM: %04x\n", value);

	value = inl(port+8);
	DPRINTF("FLBASEADD: %08x\n", value);

	value = inb(port+0x0c);
	DPRINTF("SOFMOD: %02x\n", value);

	value = inw(port+0x10);
	DPRINTF("PORTSTS1: %04x\n", value);

	value = inw(port+0x12);
	DPRINTF("PORTSTS2: %04x\n", value);
	
#endif

}

void dump_td( td_t *td, char *prefix)
{
	char newpre[64];

	newpre[0]='\t';
	strcpy(newpre+1, prefix);

	DPRINTF("%sTD(%p):\n", prefix, td);

	switch(td->packet_type) {
		case SETUP_TOKEN:
			DPRINTF("%stype: SETUP\n", prefix);
			break;
		case OUT_TOKEN:
			DPRINTF("%stype: OUT\n", prefix);
			break;
		case IN_TOKEN:
			DPRINTF("%stype: IN\n", prefix);
			break;
		default:
			DPRINTF("%stype: INVALID (%02x)\n", prefix, td->packet_type);
			break;
	}

	DPRINTF("%sretries: %u\n", prefix, td->retrys);

	if(td->isochronous)
		DPRINTF("%sisochronous\n", prefix);

	if(td->interrupt)
		DPRINTF("%sIOC\n", prefix);

	if(td->lowspeed)
		DPRINTF("%slowspeed\n", prefix);

	if(td->detect_short)
		DPRINTF("%sDETECT_SHORT\n", prefix);

	DPRINTF("%sactive: %u\n", prefix, td->active);
	DPRINTF("%sdevice_addr: %02x\n", prefix, td->device_addr);
	DPRINTF("%sendpoint: %1x\n", prefix, td->endpoint);
	DPRINTF("%sdata_toggle: %1u\n", prefix, td->data_toggle);
	DPRINTF("%smax_transfer: %u\n", prefix, td->max_transfer);
	DPRINTF("%sactual: %u\n", prefix, td->actual);
	DPRINTF("%slink:\n", prefix);

	if(td->stall)
		DPRINTF("%sSTALL\n", prefix);

	if(td->bitstuff)
		DPRINTF("%sBITSTUFF ERROR\n", prefix);

	if(td->crc)
		DPRINTF("%sCRC ERROR\n", prefix);

	if(td->nak)
		DPRINTF("%sNAK ERROR\n", prefix);

	if(td->babble)
		DPRINTF("%sBABBLE ERROR\n", prefix);

	if(td->buffer_error)
		DPRINTF("%sBUFFER ERROR\n", prefix);

	if(MEM_ADDR(td->link.link) == td) {
		DPRINTF("link loops back to me!\n");
		return;
	}

	dump_link(&(td->link), newpre);
	if(!td->link.terminate) {
		if(td->link.queue)
			dump_queue_head(MEM_ADDR(td->link.link), prefix );
		else
			dump_td(MEM_ADDR(td->link.link), prefix);
	}
}

void dump_queue_head( queue_head_t *qh, char *prefix)
{
	char newpre[64];

	newpre[0] = '\t';
	strcpy(newpre+1, prefix);

	DPRINTF("%sQUEUE HEAD(%p):\n", prefix, qh);
	DPRINTF("%sdepth:\n", prefix);
	dump_link( &(qh->depth), newpre);

	if(!qh->depth.terminate)
		if(qh->depth.queue)
			dump_queue_head(MEM_ADDR(qh->depth.link), newpre);
		else
			dump_td( MEM_ADDR(qh->depth.link), newpre);
		

	DPRINTF("%sbredth:\n", prefix);
	dump_link( &(qh->bredth), newpre);
	if(!qh->bredth.terminate)
		if(qh->bredth.queue)
			dump_queue_head(MEM_ADDR(qh->bredth.link), newpre);
		else
			dump_td( MEM_ADDR(qh->bredth.link), newpre);
}

void dump_transaction( transaction_t *trans, char *prefix)
{
	char newpre[64];

	newpre[0] = '\t';
	strcpy(newpre+1, prefix);


	DPRINTF("%s TRANSACTION(%p):\n", prefix, trans);
	dump_queue_head( trans->qh, newpre);

	DPRINTF("%s TDs:\n", prefix);
	dump_td( trans->td_list, newpre);

	DPRINTF("\n");
	if(trans->next)
		dump_transaction(trans->next, prefix);
}

void dump_usbdev( usbdev_t *dev, char *prefix)
{
	char newpre[64];
	int i;

	newpre[0] = '\t';
	strcpy(newpre+1, prefix);

	DPRINTF("%saddress: %u\n", prefix, dev->address);
	DPRINTF("%sclass: %u\n", prefix, dev->class);
	DPRINTF("%ssubclass: %u\n", prefix, dev->subclass);
	DPRINTF("%sbulk_in: %u\n", prefix, dev->bulk_in);
	DPRINTF("%sbulk_out: %u\n", prefix, dev->bulk_out);
	DPRINTF("%sinterrupt: %u\n", prefix, dev->interrupt);

	DPRINTF("%sep toggle:\n", prefix);
		for(i=0;i<MAX_EP;i++)
			DPRINTF("%s%u\n", newpre, dev->toggle[i]);

	DPRINTF("%sep max_packet:\n", prefix);
		for(i=0;i<MAX_EP;i++)
			DPRINTF("%s%u\n", newpre, dev->max_packet[i]);
}

void dump_all_usbdev(char *prefix)
{
	int i;

	for(i=0;i<MAX_USB_DEV;i++) {
		if(usb_device[i].address) {
			DPRINTF("%sDEVICE: %u\n", prefix, i);
			dump_usbdev( usb_device +i, prefix);
		}
	}
}

void dump_device_descriptor( device_descriptor_t *des, char *prefix)
{
	DPRINTF("%sbLength: %02x\n", prefix, des->bLength);
	DPRINTF("%stype: %02x\n", prefix, des->type);
	DPRINTF("%sbcdVersion: %1u%1u\n", prefix, des->bcdVersion[1], des->bcdVersion[0]);
	DPRINTF("%sClass: %02x\n",prefix, des->Class);
	DPRINTF("%sSubClass: %02x\n",prefix, des->SubClass);
	DPRINTF("%sprotocol: %02x\n",prefix, des->protocol);
	DPRINTF("%smax_packet: %u\n",prefix, des->max_packet);
	DPRINTF("%sidVendor: %04x\n", prefix, des->idVendor);
	DPRINTF("%sidProduct: %04x\n", prefix, des->idProduct);
	DPRINTF("%sbcdDeviceVersion: %u%u\n", prefix, des->bcdDevice[1], des->bcdDevice[0]);
	DPRINTF("%siManufacturor: %02x\n", prefix, des->iManufacturor);
	DPRINTF("%siProduct: %02x\n", prefix, des->iProduct);
	DPRINTF("%siSerial: %02x\n", prefix, des->iSerial);
	DPRINTF("%sbNumConfig: %02x\n", prefix, des->bNumConfig);

}

void dump_interface_descriptor( interface_descriptor_t *iface, char *prefix)
{

	DPRINTF("%sbLength: %02x\n", prefix, iface->bLength);
	DPRINTF("%stype: %02x\n", prefix, iface->type);
	DPRINTF("%sbInterfaceNumber: %02x\n", prefix, iface->bInterfaceNumber);
	DPRINTF("%sbAlternateSetting: %02x\n", prefix, iface->bAlternateSetting);
	DPRINTF("%sbNumEndpoints: %02x\n", prefix, iface->bNumEndpoints);
	DPRINTF("%sbInterfaceClass: %02x\n", prefix, iface->bInterfaceClass);
	DPRINTF("%sbInterfaceSubClass: %02x\n", prefix, iface->bInterfaceSubClass);
	DPRINTF("%sbInterfaceProtocol: %02x\n", prefix, iface->bInterfaceProtocol);
	DPRINTF("%siInterface: %02x\n", prefix, iface->iInterface);
}

void dump_endpoint_descriptor( endpoint_descriptor_t *ep, char *prefix)
{

	DPRINTF("%sbLength: %02x\n", prefix, ep->bLength);
	DPRINTF("%stype: %02x\n", prefix, ep->type);
	DPRINTF("%sbEndpointAddress: %02x\n", prefix, ep->bEndpointAddress);
	DPRINTF("%sbmAttributes: %02x\n", prefix, ep->bmAttributes);
	DPRINTF("%swMaxPacketSize: %02x\n", prefix, ep->wMaxPacketSize);
	DPRINTF("%sbInterval: %02x\n", prefix, ep->bInterval);
}

void dump_config_descriptor( uchar *des, char *prefix)	// YES uchar *
{
	config_descriptor_t *config;
	interface_descriptor_t *iface;
	endpoint_descriptor_t *ep;
	char newpre[64];
	int i;

	memset(newpre,0,sizeof(newpre));
	newpre[0] = '\t';
	strcpy(newpre+1, prefix);

	config = (config_descriptor_t *) des;
	iface = (interface_descriptor_t *) (des + config->bLength);
	ep = (endpoint_descriptor_t *) (des + config->bLength + iface->bLength);
	
	// now, the config itself
	DPRINTF("%sbLength: %02x\n", prefix, config->bLength);
	DPRINTF("%stype: %02x\n", prefix, config->type);
	DPRINTF("%swTotalLength: %04x\n", prefix, config->wTotalLength);
	DPRINTF("%sbNumInterfaces: %02x\n", prefix, config->bNumInterfaces);
	DPRINTF("%sbConfigurationValue: %02x\n", prefix, config->bConfigurationValue);
	DPRINTF("%siConfiguration: %02x\n", prefix, config->iConfiguration);
	DPRINTF("%sbmAttributes: %02x\n", prefix, config->bmAttributes);

	DPRINTF("%sbMaxPower: %02x\n", prefix, config->bMaxPower);

	DPRINTF("\n%sInterface(%p):\n", prefix, iface);
	dump_interface_descriptor(iface, newpre);

	newpre[1] = '\t';
	strcpy(newpre+2, prefix);

	for(i=0; i<iface->bNumEndpoints; i++) {
		DPRINTF("\n%sEndpoint (%p):\n", newpre+1, ep+i);
		dump_endpoint_descriptor( ep+i, newpre);
	}
}

// Some control message bmRequestType defines
#define CTRL_DEVICE 0
#define CONTROL_INTERFACE 1
#define CONTROL_ENDPOINT 2
#define CONTROL_OTHER 3
#define CONTROL_RECIPIENT_MASK 0x1f

#define CONTROL_TYPE_STD 0
#define CONTROL_TYPE_CLASS 0x20
#define CONTROL_CLASS_VENDOR 0x40
#define CONTROL_CLASS_MASK 0x60

#define CONTROL_OUT 0
#define CONTROL_IN 0x80
#define CONTROL_DIR_MASK 0x80

// bRequest values
#define GET_STATUS 0
#define CLEAR_FEATURE 1
#define SET_FEATURE 3
#define SET_ADDRESS 5

#define GET_DESCRIPTOR 6
#define SET_DESCRIPTOR 7

#define GET_CONFIGURATION 8
#define SET_CONFIGURATION 9

#define GET_INTERFACE 10
#define SET_INTERFACE 11

#define SYNC_FRAME 12

// descriptor types
#define DEVICE_DESC 1
#define CONFIGURATION_DESC 2
#define STRING_DESC 3
#define INTERFACE_DESC 4
#define ENDPOINT_DESC 5
#define OTHERSPEED_DESC 7
#define POWER_DESC 8

// features
#define FEATURE_HALT 0
void dump_ctrlmsg( ctrl_msg_t *msg, char *prefix)
{
	DPRINTF("%sbmRequestType: %02x\n", prefix, msg->bmRequestType);
	DPRINTF("%sbRequest: %02x\n", prefix, msg->bRequest);
	DPRINTF("%swValue: %04x\n", prefix, msg->wValue);
	DPRINTF("%swIndex: %04x\n", prefix, msg->wIndex);
	DPRINTF("%swLength: %04x\n", prefix, msg->wLength);
}

