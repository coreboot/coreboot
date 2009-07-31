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

#include <libpayload-config.h>
#include <usb/usb.h>

hci_t *usb_hcs = 0;

hci_t *
new_controller (void)
{
	hci_t *controller = malloc (sizeof (hci_t));

	if (controller) {
		/* atomic */
		controller->next = usb_hcs;
		usb_hcs = controller;
		/* atomic end */
	}

	return controller;
}

void
detach_controller (hci_t *controller)
{
	if (controller == NULL)
		return;
	if (usb_hcs == controller) {
		usb_hcs = controller->next;
	} else {
		hci_t *it = usb_hcs;
		while (it != NULL) {
			if (it->next == controller) {
				it->next = controller->next;
				return;
			}
		}
	}
}

/**
 * Polls all hubs on all USB controllers, to find out about device changes
 */
void
usb_poll ()
{
	if (usb_hcs == 0)
		return;
	hci_t *controller = usb_hcs;
	while (controller != 0) {
		int i;
		for (i = 0; i < 128; i++) {
			if (controller->devices[i] != 0) {
				controller->devices[i]->poll (controller->
							     devices[i]);
			}
		}
		controller = controller->next;
	}
}

void
init_device_entry (hci_t *controller, int i)
{
	if (controller->devices[i] != 0)
		printf("warning: device %d reassigned?\n", i);
	controller->devices[i] = malloc(sizeof(usbdev_t));
	controller->devices[i]->controller = controller;
	controller->devices[i]->address = -1;
	controller->devices[i]->hub = -1;
	controller->devices[i]->port = -1;
	controller->devices[i]->init = usb_nop_init;
	controller->devices[i]->init (controller->devices[i]);
}

void
set_feature (usbdev_t *dev, int endp, int feature, int rtype)
{
	dev_req_t dr;

	dr.bmRequestType = rtype;
	dr.data_dir = host_to_device;
	dr.bRequest = SET_FEATURE;
	dr.wValue = feature;
	dr.wIndex = endp;
	dr.wLength = 0;
	dev->controller->control (dev, OUT, sizeof (dr), &dr, 0, 0);
}

void
get_status (usbdev_t *dev, int intf, int rtype, int len, void *data)
{
	dev_req_t dr;

	dr.bmRequestType = rtype;
	dr.data_dir = device_to_host;
	dr.bRequest = GET_STATUS;
	dr.wValue = 0;
	dr.wIndex = intf;
	dr.wLength = len;
	dev->controller->control (dev, IN, sizeof (dr), &dr, len, data);
}

u8 *
get_descriptor (usbdev_t *dev, unsigned char bmRequestType, int descType,
		int descIdx, int langID)
{
	u8 buf[8];
	u8 *result;
	dev_req_t dr;
	int size;

	dr.bmRequestType = bmRequestType;
	dr.data_dir = device_to_host;	// always like this for descriptors
	dr.bRequest = GET_DESCRIPTOR;
	dr.wValue = (descType << 8) | descIdx;
	dr.wIndex = langID;
	dr.wLength = 8;
	if (dev->controller->control (dev, IN, sizeof (dr), &dr, 8, buf)) {
		printf ("getting descriptor size (type %x) failed\n",
			descType);
	}

	if (descType == 1) {
		device_descriptor_t *dd = (device_descriptor_t *) buf;
		printf ("maxPacketSize0: %x\n", dd->bMaxPacketSize0);
		if (dd->bMaxPacketSize0 != 0)
			dev->endpoints[0].maxpacketsize = dd->bMaxPacketSize0;
	}

	/* special case for configuration descriptors: they carry all their
	   subsequent descriptors with them, and keep the entire size at a
	   different location */
	size = buf[0];
	if (buf[1] == 2) {
		int realsize = ((unsigned short *) (buf + 2))[0];
		size = realsize;
	}
	result = malloc (size);
	memset (result, 0, size);
	dr.wLength = size;
	if (dev->controller->
	    control (dev, IN, sizeof (dr), &dr, size, result)) {
		printf ("getting descriptor (type %x, size %x) failed\n",
			descType, size);
	}

	return result;
}

void
set_configuration (usbdev_t *dev)
{
	dev_req_t dr;

	dr.bmRequestType = 0;
	dr.bRequest = SET_CONFIGURATION;
	dr.wValue = dev->configuration[5];
	dr.wIndex = 0;
	dr.wLength = 0;
	dev->controller->control (dev, OUT, sizeof (dr), &dr, 0, 0);
}

int
clear_stall (endpoint_t *ep)
{
	usbdev_t *dev = ep->dev;
	int endp = ep->endpoint;
	dev_req_t dr;

	dr.bmRequestType = 0;
	if (endp != 0) {
		dr.req_recp = endp_recp;
	}
	dr.bRequest = CLEAR_FEATURE;
	dr.wValue = ENDPOINT_HALT;
	dr.wIndex = endp;
	dr.wLength = 0;
	dev->controller->control (dev, OUT, sizeof (dr), &dr, 0, 0);
	return 0;
}

/* returns free address or -1 */
static int
get_free_address (hci_t *controller)
{
	int i;
	for (i = 1; i < 128; i++) {
		if (controller->devices[i] == 0)
			return i;
	}
	printf ("no free address found\n");
	return -1;		// no free address
}

int
set_address (hci_t *controller, int lowspeed)
{
	int adr = get_free_address (controller);	// address to set
	dev_req_t dr;
	configuration_descriptor_t *cd;
	device_descriptor_t *dd;

	memset (&dr, 0, sizeof (dr));
	dr.data_dir = host_to_device;
	dr.req_type = standard_type;
	dr.req_recp = dev_recp;
	dr.bRequest = SET_ADDRESS;
	dr.wValue = adr;
	dr.wIndex = 0;
	dr.wLength = 0;

	init_device_entry(controller, adr);
	usbdev_t *dev = controller->devices[adr];
	// dummy values for registering the address
	dev->address = 0;
	dev->lowspeed = lowspeed;
	dev->endpoints[0].dev = dev;
	dev->endpoints[0].endpoint = 0;
	dev->endpoints[0].maxpacketsize = 8;
	dev->endpoints[0].toggle = 0;
	dev->endpoints[0].direction = SETUP;
	mdelay (50);
	if (dev->controller->control (dev, OUT, sizeof (dr), &dr, 0, 0)) {
		printf ("set_address failed\n");
		return -1;
	}
	mdelay (50);
	dev->address = adr;
	dev->descriptor =
		get_descriptor (dev,
				gen_bmRequestType (device_to_host,
						   standard_type, dev_recp),
				1, 0, 0);
	dd = (device_descriptor_t *) dev->descriptor;
	printf ("device version: %x.%x\n", dd->bcdUSB >> 8,
		dd->bcdUSB & 0xff);
	printf ("device has %x configurations\n", dd->bNumConfigurations);
	if (dd->bNumConfigurations == 0) {
		/* device isn't usable */
		printf ("no usable configuration!\n");
		dev->address = 0;
		return -1;
	}
	dev->configuration =
		get_descriptor (dev,
				gen_bmRequestType (device_to_host,
						   standard_type, dev_recp),
				2, 0, 0);
	cd = (configuration_descriptor_t *) dev->configuration;
	set_configuration (dev);
	interface_descriptor_t *interface =
		(interface_descriptor_t *) (((char *) cd) + cd->bLength);
	{
		int i;
		int num = cd->bNumInterfaces;
		interface_descriptor_t *current = interface;
		printf ("device has %x interfaces\n", num);
		if (num>1)
			printf ("NOTICE: This driver defaults to using the first interface.\n"
				"This might be the wrong choice and lead to limited functionality\n"
				"of the device. Please report such a case to coreboot@coreboot.org\n"
				"as you might be the first.\n");
		/* we limit to the first interface, as there was no need to
		   implement something else for the time being. If you need
		   it, see the SetInterface and GetInterface functions in
		   the USB specification, and adapt appropriately. */
		num = (num > 1) ? 1 : num;
		for (i = 0; i < num; i++) {
			int j;
			printf (" #%x has %x endpoints, interface %x:%x, protocol %x\n", current->bInterfaceNumber, current->bNumEndpoints, current->bInterfaceClass, current->bInterfaceSubClass, current->bInterfaceProtocol);
			endpoint_descriptor_t *endp =
				(endpoint_descriptor_t *) (((char *) current)
							   +
							   current->bLength);
			if (interface->bInterfaceClass == 0x3)
				endp = (endpoint_descriptor_t *) (((char *) endp) + ((char *) endp)[0]);	// ignore HID descriptor
			memset (dev->endpoints, 0, sizeof (dev->endpoints));
			dev->num_endp = 1;	// 0 always exists
			dev->endpoints[0].dev = dev;
			dev->endpoints[0].maxpacketsize = dd->bMaxPacketSize0;
			dev->endpoints[0].direction = SETUP;
			dev->endpoints[0].type = CONTROL;
			for (j = 1; j <= current->bNumEndpoints; j++) {
				static const char *transfertypes[4] =
					{ "control", "isochronous", "bulk",
					"interrupt"
				};
				printf ("   #%x: Endpoint %x (%s), max packet size %x, type %s\n", j, endp->bEndpointAddress & 0x7f, ((endp->bEndpointAddress & 0x80) != 0) ? "in" : "out", endp->wMaxPacketSize, transfertypes[endp->bmAttributes]);
				endpoint_t *ep =
					&dev->endpoints[dev->num_endp++];
				ep->dev = dev;
				ep->endpoint = endp->bEndpointAddress;
				ep->toggle = 0;
				ep->maxpacketsize = endp->wMaxPacketSize;
				ep->direction =
					((endp->bEndpointAddress & 0x80) ==
					 0) ? OUT : IN;
				ep->type = endp->bmAttributes;
				endp = (endpoint_descriptor_t
					*) (((char *) endp) + endp->bLength);
			}
			current = (interface_descriptor_t *) endp;
		}
	}
	int class = dd->bDeviceClass;
	if (class == 0)
		class = interface->bInterfaceClass;

	enum { hid_device = 0x3, msc_device = 0x8, hub_device = 0x9 };

	printf ("device of class %x found\n", class);
	if (class == hub_device) {
		printf ("hub found\n");
#ifdef CONFIG_USB_HUB
		controller->devices[adr]->init = usb_hub_init;
#else
		printf ("support not compiled in\n");
#endif
	}
	if (class == hid_device) {
		printf ("HID found\n");
#ifdef CONFIG_USB_HID
		controller->devices[adr]->init = usb_hid_init;
#else
		printf ("support not compiled in\n");
#endif
	}
	if (class == msc_device) {
		printf ("MSC found\n");
#ifdef CONFIG_USB_MSC
		controller->devices[adr]->init = usb_msc_init;
#else
		printf ("support not compiled in\n");
#endif
	}
	return adr;
}

void
usb_detach_device(hci_t *controller, int devno)
{
	controller->devices[devno]->destroy (controller->devices[devno]);
	free(controller->devices[devno]);
	controller->devices[devno] = 0;
}

int
usb_attach_device(hci_t *controller, int hubaddress, int port, int lowspeed)
{
	printf ("%sspeed device\n", (lowspeed == 1) ? "low" : "full");
	int newdev = set_address (controller, lowspeed);
	if (newdev == -1)
		return -1;
	usbdev_t *newdev_t = controller->devices[newdev];

	newdev_t->address = newdev;
	newdev_t->hub = hubaddress;
	newdev_t->port = port;
	// determine responsible driver - current done in set_address
	newdev_t->init (newdev_t);
	return newdev;
}

void
usb_fatal (const char *message)
{
	printf(message);
	for (;;) ;
}
