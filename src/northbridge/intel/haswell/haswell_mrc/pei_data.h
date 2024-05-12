/* SPDX-License-Identifier: BSD-3-Clause */

#ifndef PEI_DATA_H
#define PEI_DATA_H

#include <stdint.h>

typedef void (*tx_byte_func)(unsigned char byte);
#define PEI_VERSION 15

#define PEI_USB_OC_PIN_SKIP 8

enum pei_usb2_port_location {
	PEI_USB_PORT_BACK_PANEL = 0,
	PEI_USB_PORT_FRONT_PANEL,
	PEI_USB_PORT_DOCK,
	PEI_USB_PORT_MINI_PCIE,
	PEI_USB_PORT_FLEX,
	PEI_USB_PORT_INTERNAL,
	PEI_USB_PORT_SKIP
};

/* Usb Port Length:
 * [16:4] = length in inches in octal format
 * [3:0]  = decimal point
 */
struct pei_usb2_port_setting {
	uint16_t length;
	uint8_t enable;
	uint8_t over_current_pin;
	uint8_t location;
} __packed;

struct pei_usb3_port_setting {
	uint8_t enable;
	uint8_t over_current_pin;
} __packed;

struct pei_data
{
	uint32_t pei_version;
	uint32_t mchbar;
	uint32_t dmibar;
	uint32_t epbar;
	uint32_t pciexbar;
	uint16_t smbusbar;
	/* Unused by HSW MRC, but changes to the memory layout of this struct break the ABI */
	uint32_t _unused_wdbbar;
	uint32_t _unused_wdbsize;
	uint32_t hpet_address;
	uint32_t rcba;
	uint32_t pmbase;
	uint32_t gpiobase;
	uint32_t temp_mmio_base;
	/* System type: 0 => Mobile, 1 => Desktop/Server, 5 => ULT, Others => Reserved */
	uint32_t system_type;
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
	int dq_pins_interleaved;
	/* Data read from flash and passed into MRC */
	unsigned char *mrc_input;
	unsigned int mrc_input_len;
	/* Data from MRC that should be saved to flash */
	unsigned char *mrc_output;
	unsigned int mrc_output_len;
	/* Max frequency to run DDR3 at. Can be one of four values: 800, 1067, 1333, 1600 */
	uint32_t max_ddr3_freq;
	/* Route all USB ports to XHCI controller in resume path */
	int usb_xhci_on_resume;
	struct pei_usb2_port_setting usb2_ports[16];
	struct pei_usb3_port_setting usb3_ports[16];
	uint8_t spd_data[4][SPD_SIZE_MAX_DDR3];
	tx_byte_func tx_byte;
} __packed;

#endif
