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

#include <usb/usb.h>

enum { hid_subclass_none = 0, hid_subclass_boot = 1 };
enum { hid_proto_boot = 0, hid_proto_report = 1 };
enum { hid_boot_proto_none = 0, hid_boot_proto_keyboard =
		1, hid_boot_proto_mouse = 2
};
static const char *boot_protos[3] = { "(none)", "keyboard", "mouse" };
enum { GET_REPORT = 0x1, GET_IDLE = 0x2, GET_PROTOCOL = 0x3, SET_REPORT =
		0x9, SET_IDLE = 0xa, SET_PROTOCOL = 0xb
};

static void
usb_hid_destroy (usbdev_t *dev)
{
}

int keypress;
char keymap[256] = {
	-1, -1, -1, -1, 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k',
	'l',
	'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z',
	'1', '2',
	'3', '4', '5', '6', '7', '8', '9', '0', '\n', TERM_ESC,
	TERM_BACKSPACE, TERM_TAB, ' ', '-', '=', '[',
	']', '\\', -1, ';', '\'', '`', ',', '.', '/', -1, -1, -1, -1, -1, -1,
	-1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, TERM_HOME, TERM_PPAGE, -1,
	TERM_END, TERM_NPAGE, TERM_RIGHT,
	TERM_LEFT, TERM_DOWN, TERM_UP, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
};


static void
usb_hid_poll (usbdev_t *dev)
{
	char buf[8];
	static int toggle = 0;
	// hardcode to endpoint 1, 8 bytes
	dev->controller->packet (dev, 1, IN, toggle, 8, buf);
	toggle ^= 1;
	// FIXME: manage buf[0]=special keys, too
	keypress = keymap[buf[2]];
	if ((keypress == -1) && (buf[2] != 0)) {
		printf ("%x %x %x %x %x %x %x %x\n", buf[0], buf[1], buf[2],
			buf[3], buf[4], buf[5], buf[6], buf[7]);
	}
}

int (*oldhook) (void);

int
hookfunc (void)
{
	int key;
	if (oldhook != 0)
		key = oldhook ();
	if (key == -1)
		key = keypress;
	return key;
}

void
usb_hid_init (usbdev_t *dev)
{

	configuration_descriptor_t *cd = dev->configuration;
	interface_descriptor_t *interface = ((char *) cd) + cd->bLength;

	if (interface->bInterfaceSubClass == hid_subclass_boot) {
		printf ("  supports boot interface..\n");
		printf ("  it's a %s\n",
			boot_protos[interface->bInterfaceProtocol]);
		if (interface->bInterfaceProtocol == hid_boot_proto_keyboard) {
			printf ("  activating...\n");
			dev_req_t dr;
			// set_protocol(hid_proto_boot)
			dr.data_dir = host_to_device;
			dr.req_type = class_type;
			dr.req_recp = iface_recp;
			dr.bRequest = SET_PROTOCOL;
			dr.wValue = hid_proto_boot;
			dr.wIndex = interface->bInterfaceNumber;
			dr.wLength = 0;
			dev->controller->control (dev, OUT,
						  sizeof (dev_req_t), &dr, 0,
						  0);

			// only add here, because we only support boot-keyboard HID devices
			// FIXME: make this a real console input driver instead, once the API is there
			dev->destroy = usb_hid_destroy;
			dev->poll = usb_hid_poll;
			oldhook = getkey_hook;
			getkey_hook = hookfunc;
		}
	}
}
