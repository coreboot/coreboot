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
#define PEI_VERSION 11

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
	/*
	 * USB Port Configuration:
	 *  [0] = enable
	 *  [1] = overcurrent pin
	 *  [2] = length
	 *
	 * Ports 0-7 can be mapped to OC0-OC3
	 * Ports 8-13 can be mapped to OC4-OC7
	 *
	 * Port Length
	 *  MOBILE:
	 *   < 0x050 = Setting 1 (back panel, 1-5in, lowest tx amplitude)
	 *   < 0x140 = Setting 2 (back panel, 5-14in, highest tx amplitude)
	 *  DESKTOP:
	 *   < 0x080 = Setting 1 (front/back panel, <8in, lowest tx amplitude)
	 *   < 0x130 = Setting 2 (back panel, 8-13in, higher tx amplitude)
	 *   < 0x150 = Setting 3 (back panel, 13-15in, higest tx amplitude)
	 */
	uint16_t usb_port_config[16][3];
	/* SPD data array for onboard RAM. Specify address 0xf0,
	 * 0xf1, 0xf2, 0xf3 to index one of the 4 slots in
	 * spd_address for a given "DIMM".
	 */
	uint8_t spd_data[4][256];
	tx_byte_func tx_byte;
} __attribute__((packed));

#endif
