/*
 * This file is part of the libpayload project.
 *
 * Copyright (C) 2010 coresystems GmbH
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

#include <libpayload-config.h>
#include <usb/usb.h>

typedef struct {
	u16 vendor, device;
	u32 quirks;
	int interface;
} usb_quirks_t;

// IDs without a quirk don't need to be mentioned in this list
// but some are here for easier testing.

usb_quirks_t usb_quirks[] = {
	/* Working chips,... remove before next release */
	{ 0x3538, 0x0054, USB_QUIRK_NONE, 0 },	// PQI 1GB
	{ 0x13fd, 0x0841, USB_QUIRK_NONE, 0 },	// Samsung SE-S084

	/* Silence the warning for known devices with more
	 * than one interface
	 */
	{ 0x1267, 0x0103, USB_QUIRK_NONE, 1 },	// Keyboard Trust KB-1800S
	{ 0x0a12, 0x0001, USB_QUIRK_NONE, 1 },	// Bluetooth Allnet ALL1575

	/* Currently unsupported, possibly interesting devices:
	 * FTDI serial: device 0x0403:0x6001 is USB 1.10 (class ff)
	 * UPEK TouchChip: device 0x147e:0x2016 is USB 1.0 (class ff)
	 */
};

u32 usb_quirk_check(u16 vendor, u16 device)
{
	int i;
	for (i = 0; i < ARRAY_SIZE(usb_quirks); i++) {
		if ((usb_quirks[i].vendor == vendor) &&
				(usb_quirks[i].device == device)) {
			usb_debug("USB quirks enabled: %08x\n",
					usb_quirks[i].quirks);
			return usb_quirks[i].quirks;
		}
	}

	return USB_QUIRK_NONE;
}

int usb_interface_check(u16 vendor, u16 device)
{
	int i;
	for (i = 0; i < ARRAY_SIZE(usb_quirks); i++) {
		if ((usb_quirks[i].vendor == vendor) &&
				(usb_quirks[i].device == device)) {
			return usb_quirks[i].interface;
		}
	}

	return 0;
}

