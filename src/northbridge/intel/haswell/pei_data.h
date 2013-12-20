/*
 * coreboot UEFI PEI wrapper
 *
 * Copyright (c) 2011, Google Inc.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of Google Inc. nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL GOOGLE INC BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef PEI_DATA_H
#define PEI_DATA_H

typedef void (*tx_byte_func)(unsigned char byte);
#define PEI_VERSION 14

#define MAX_USB2_PORTS 16
#define MAX_USB3_PORTS 16
#define USB_OC_PIN_SKIP 8

enum usb2_port_location {
	USB_PORT_BACK_PANEL = 0,
	USB_PORT_FRONT_PANEL,
	USB_PORT_DOCK,
	USB_PORT_MINI_PCIE,
	USB_PORT_FLEX,
	USB_PORT_INTERNAL,
	USB_PORT_SKIP
};

/* Usb Port Length:
 * [16:4] = length in inches in octal format
 * [3:0]  = decimal point
 */
struct usb2_port_setting {
	uint16_t length;
	uint8_t enable;
	uint8_t over_current_pin;
	uint8_t location;
} __attribute__((packed));

struct usb3_port_setting {
	uint8_t enable;
	uint8_t over_current_pin;
} __attribute__((packed));

struct pei_data
{
	uint32_t pei_version;
	uint32_t mchbar;
	uint32_t dmibar;
	uint32_t epbar;
	uint32_t pciexbar;
	uint16_t smbusbar;
	uint32_t wdbbar;
	uint32_t wdbsize;
	uint32_t hpet_address;
	uint32_t rcba;
	uint32_t pmbase;
	uint32_t gpiobase;
	uint32_t temp_mmio_base;
	uint32_t system_type; // 0 Mobile, 1 Desktop/Server
	uint32_t tseg_size;
	uint8_t spd_addresses[4];
	int boot_mode;
	int ec_present;
	int gbe_enable;
	// 0 = leave channel enabled
	// 1 = disable dimm 0 on channel
	// 2 = disable dimm 1 on channel
	// 3 = disable dimm 0+1 on channel
	int dimm_channel0_disabled;
	int dimm_channel1_disabled;
	/* Enable 2x Refresh Mode */
	int ddr_refresh_2x;
	/* Data read from flash and passed into MRC */
	unsigned char *mrc_input;
	unsigned int mrc_input_len;
	/* Data from MRC that should be saved to flash */
	unsigned char *mrc_output;
	unsigned int mrc_output_len;
	/*
	 * Max frequency DDR3 could be ran at. Could be one of four values: 800,
	 * 1067, 1333, 1600
	 */
	uint32_t max_ddr3_freq;
	/* Route all USB ports to XHCI controller in resume path */
	int usb_xhci_on_resume;
	struct usb2_port_setting usb2_ports[MAX_USB2_PORTS];
	struct usb3_port_setting usb3_ports[MAX_USB3_PORTS];
	uint8_t spd_data[4][256];
	tx_byte_func tx_byte;
} __attribute__((packed));

#endif
