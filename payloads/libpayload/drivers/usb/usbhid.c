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
#include <curses.h>

enum { hid_subclass_none = 0, hid_subclass_boot = 1 };
typedef enum { hid_proto_boot = 0, hid_proto_report = 1 } hid_proto;
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
	free (dev->data);
}

typedef struct {
	void* queue;
} usbhid_inst_t;

#define HID_INST(dev) ((usbhid_inst_t*)(dev)->data)

/* buffer is global to all keyboard drivers */
int count;
short keybuffer[16];

int keypress;
short keymap[256] = {
	-1, -1, -1, -1, 'a', 'b', 'c', 'd',
	'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l',

	'm', 'n', 'o', 'p', 'q', 'r', 's', 't',
	'u', 'v', 'w', 'x', 'y', 'z', '1', '2',

	'3', '4', '5', '6', '7', '8', '9', '0',
	'\n', '\e', '\b', '\t', ' ', '-', '=', '[',

	']', '\\', -1, ';', '\'', '`', ',', '.',
	'/', -1, KEY_F(1), KEY_F(2), KEY_F(3), KEY_F(4), KEY_F(5), KEY_F(6),

	KEY_F(7), KEY_F(8), KEY_F(9), KEY_F(10), KEY_F(11), KEY_F(12), -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1,
/* 50 */
	-1, -1, -1, -1, -1, '*', '-', '+',
	-1, KEY_END, KEY_DOWN, KEY_NPAGE, KEY_LEFT, -1, KEY_RIGHT, KEY_HOME,

	KEY_UP, KEY_PPAGE, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1,

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
	u8* buf;
	while ((buf=dev->controller->poll_intr_queue (HID_INST(dev)->queue))) {
		// FIXME: manage buf[0]=special keys, too
		int i;
		keypress = 0;
		for (i=2; i<9; i++) {
			if (buf[i] != 0)
				keypress = keymap[buf[i]];
			else
				break;
		}
		if ((keypress == -1) && (buf[2] != 0)) {
			printf ("%x %x %x %x %x %x %x %x\n", buf[0], buf[1], buf[2],
				buf[3], buf[4], buf[5], buf[6], buf[7]);
		}
		if (keypress != -1) {
			/* ignore key presses if buffer full */
			if (count < 16)
				keybuffer[count++] = keypress;
		}
	}
}

static void
usb_hid_set_idle (usbdev_t *dev, interface_descriptor_t *interface, u16 duration)
{
	dev_req_t dr;
	dr.data_dir = host_to_device;
	dr.req_type = class_type;
	dr.req_recp = iface_recp;
	dr.bRequest = SET_IDLE;
	dr.wValue = (duration >> 2) << 8;
	dr.wIndex = interface->bInterfaceNumber;
	dr.wLength = 0;
	dev->controller->control (dev, OUT, sizeof (dev_req_t), &dr, 0, 0);
}

static void
usb_hid_set_protocol (usbdev_t *dev, interface_descriptor_t *interface, hid_proto proto)
{
	dev_req_t dr;
	dr.data_dir = host_to_device;
	dr.req_type = class_type;
	dr.req_recp = iface_recp;
	dr.bRequest = SET_PROTOCOL;
	dr.wValue = proto;
	dr.wIndex = interface->bInterfaceNumber;
	dr.wLength = 0;
	dev->controller->control (dev, OUT, sizeof (dev_req_t), &dr, 0, 0);
}

static struct console_input_driver cons = {
	.havekey = usbhid_havechar,
	.getchar = usbhid_getchar
};

void
usb_hid_init (usbdev_t *dev)
{

	static int installed = 0;
	if (!installed) {
		installed = 1;
		console_add_input_driver (&cons);
	}

	configuration_descriptor_t *cd = (configuration_descriptor_t*)dev->configuration;
	interface_descriptor_t *interface = (interface_descriptor_t*)(((char *) cd) + cd->bLength);

	if (interface->bInterfaceSubClass == hid_subclass_boot) {
		printf ("  supports boot interface..\n");
		printf ("  it's a %s\n",
			boot_protos[interface->bInterfaceProtocol]);
		if (interface->bInterfaceProtocol == hid_boot_proto_keyboard) {
			dev->data = malloc (sizeof (usbhid_inst_t));
			printf ("  configuring...\n");
			usb_hid_set_protocol(dev, interface, hid_proto_boot);
			usb_hid_set_idle(dev, interface, 0);
			printf ("  activating...\n");

			// only add here, because we only support boot-keyboard HID devices
			dev->destroy = usb_hid_destroy;
			dev->poll = usb_hid_poll;
			int i;
			for (i = 0; i <= dev->num_endp; i++) {
				if (dev->endpoints[i].endpoint == 0)
					continue;
				if (dev->endpoints[i].type != INTERRUPT)
					continue;
				if (dev->endpoints[i].direction != IN)
					continue;
				break;
			}
			printf ("  found endpoint %x for interrupt-in\n", i);
			/* 20 buffers of 8 bytes, for every 10 msecs */
			HID_INST(dev)->queue = dev->controller->create_intr_queue (&dev->endpoints[i], 8, 20, 10);
			count = 0;
			printf ("  configuration done.\n");
		}
	}
}

int usbhid_havechar (void)
{
	return (count != 0);
}

int usbhid_getchar (void)
{
	if (count == 0) return 0;
	short ret = keybuffer[0];
	memmove (keybuffer, keybuffer+1, --count);
	return ret;
}
