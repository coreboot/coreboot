/* SPDX-License-Identifier: BSD-3-Clause */

#ifndef PEI_DATA_H
#define PEI_DATA_H

#include <types.h>

#define PEI_VERSION 22

#define ABI_X86 __attribute__((regparm(0)))

typedef void ABI_X86 (*tx_byte_func)(unsigned char byte);

enum board_type {
	BOARD_TYPE_CRB_MOBILE = 0,	/* CRB Mobile */
	BOARD_TYPE_CRB_DESKTOP,		/* CRB Desktop */
	BOARD_TYPE_USER1,		/* SV mobile */
	BOARD_TYPE_USER2,		/* SV desktop */
	BOARD_TYPE_USER3,		/* SV server */
	BOARD_TYPE_ULT,			/* ULT */
	BOARD_TYPE_CRB_EMBDEDDED,	/* CRB Embedded */
	BOARD_TYPE_UNKNOWN,
};

#define MAX_USB2_PORTS 14
#define MAX_USB3_PORTS 6
#define USB_OC_PIN_SKIP 8

enum usb2_port_location {
	USB_PORT_BACK_PANEL = 0,
	USB_PORT_FRONT_PANEL,
	USB_PORT_DOCK,
	USB_PORT_MINI_PCIE,
	USB_PORT_FLEX,
	USB_PORT_INTERNAL,
	USB_PORT_SKIP,
	USB_PORT_NGFF_DEVICE_DOWN,
};

struct usb2_port_setting {
	/*
	 * Usb Port Length:
	 * [16:4] = length in inches in octal format
	 * [3:0]  = decimal point
	 */
	uint16_t length;
	uint8_t enable;
	uint8_t oc_pin;
	uint8_t location;
} __packed;

struct usb3_port_setting {
	uint8_t enable;
	uint8_t oc_pin;
	/*
	 * Set to 0 if trace length is > 5 inches
	 * Set to 1 if trace length is <= 5 inches
	 */
	uint8_t fixed_eq;
} __packed;

#define PEI_DIMM_INFO_SERIAL_SIZE	5
#define PEI_DIMM_INFO_PART_NUMBER_SIZE	19
#define PEI_DIMM_INFO_TOTAL		8	/* Maximum num of dimm is 8 */

/**
 * This table is filled by the MRC blob and used to populate the mem_info
 * struct, which is placed in CBMEM and then used to generate SMBIOS type
 * 17 table(s)
 *
 * Values are specified according to the JEDEC SPD Standard.
 */
struct pei_dimm_info {
	/*
	 * Size of the module in MiB.
	 */
	uint32_t dimm_size;
	/*
	 * SMBIOS (not SPD) device type.
	 *
	 * See the smbios.h smbios_memory_device_type enum.
	 */
	uint16_t ddr_type;
	uint16_t ddr_frequency;
	uint8_t rank_per_dimm;
	uint8_t channel_num;
	uint8_t dimm_num;
	uint8_t bank_locator;
	/*
	 * The last byte is '\0' for the end of string.
	 *
	 * Even though the SPD spec defines this field as a byte array the value
	 * is passed directly to SMBIOS as a string, and thus must be printable
	 * ASCII.
	 */
	uint8_t serial[PEI_DIMM_INFO_SERIAL_SIZE];
	/*
	 * The last byte is '\0' for the end of string
	 *
	 * Must contain only printable ASCII.
	 */
	uint8_t module_part_number[PEI_DIMM_INFO_PART_NUMBER_SIZE];
	/*
	 * SPD Manufacturer ID
	 */
	uint16_t mod_id;
	/*
	 * SPD Module Type.
	 *
	 * See spd.h for valid values.
	 *
	 * e.g., SPD_RDIMM, SPD_SODIMM, SPD_MICRO_DIMM
	 */
	uint8_t mod_type;
	/*
	 * SPD bus width.
	 *
	 * Bits 0 - 2 encode the primary bus width:
	 *   0b000 = 8 bit width
	 *   0b001 = 16 bit width
	 *   0b010 = 32 bit width
	 *   0b011 = 64 bit width
	 *
	 * Bits 3 - 4 encode the extension bits (ECC):
	 *   0b00 = 0 extension bits
	 *   0b01 = 8 bit of ECC
	 *
	 * e.g.,
	 *   64 bit bus with 8 bits of ECC (72 bits total): 0b1011
	 *   64 bit bus with 0 bits of ECC (64 bits total): 0b0011
	 *
	 * See the smbios.h smbios_memory_bus_width enum.
	 */
	uint8_t bus_width;
} __packed;

struct pei_memory_info {
	uint8_t dimm_cnt;
	struct pei_dimm_info dimm[PEI_DIMM_INFO_TOTAL];
} __packed;

struct pei_data {
	uint32_t pei_version;

	enum board_type board_type;
	int boot_mode;
	int ec_present;
	int usbdebug;

	/* Base addresses */
	uint32_t pciexbar;
	uint16_t smbusbar;
	uint32_t xhcibar;
	uint32_t ehcibar;
	uint32_t gttbar;
	uint32_t rcba;
	uint32_t pmbase;
	uint32_t gpiobase;
	uint32_t temp_mmio_base;
	uint32_t tseg_size;

	/*
	 * 0 = leave channel enabled
	 * 1 = disable dimm 0 on channel
	 * 2 = disable dimm 1 on channel
	 * 3 = disable dimm 0+1 on channel
	 */
	int dimm_channel0_disabled;
	int dimm_channel1_disabled;
	/* Set to 0 for memory down */
	uint8_t spd_addresses[4];
	/* Enable 2x Refresh Mode */
	int ddr_refresh_2x;
	/* DQ pins are interleaved on board */
	int dq_pins_interleaved;
	/* Limit DDR3 frequency */
	int max_ddr3_freq;
	/* Disable self refresh */
	int disable_self_refresh;
	/* Disable cmd power/CKEPD */
	int disable_cmd_pwr;

	/* USB port configuration */
	struct usb2_port_setting usb2_ports[MAX_USB2_PORTS];
	struct usb3_port_setting usb3_ports[MAX_USB3_PORTS];

	/*
	 * USB3 board specific PHY tuning
	 */

	/* Valid range: 0x69 - 0x80 */
	uint8_t usb3_txout_volt_dn_amp_adj[MAX_USB3_PORTS];
	/* Valid range: 0x80 - 0x9c */
	uint8_t usb3_txout_imp_sc_volt_amp_adj[MAX_USB3_PORTS];
	/* Valid range: 0x39 - 0x80 */
	uint8_t usb3_txout_de_emp_adj[MAX_USB3_PORTS];
	/* Valid range: 0x3d - 0x4a */
	uint8_t usb3_txout_imp_adj_volt_amp[MAX_USB3_PORTS];

	/* Console output function */
	tx_byte_func tx_byte;

	/*
	 * DIMM SPD data for memory down configurations
	 * [CHANNEL][SLOT][SPD]
	 */
	uint8_t spd_data[2][2][512];

	/*
	 * LPDDR3 DQ byte map
	 * [CHANNEL][ITERATION][2]
	 *
	 * Maps which PI clocks are used by what LPDDR DQ Bytes (from CPU side)
	 * DQByteMap[0] - ClkDQByteMap:
	 * - If clock is per rank, program to [0xFF, 0xFF]
	 * - If clock is shared by 2 ranks, program to [0xFF, 0] or [0, 0xFF]
	 * - If clock is shared by 2 ranks but does not go to all bytes,
	 *   Entry[i] defines which DQ bytes Group i services
	 * DQByteMap[1] - CmdNDQByteMap: [0] is CmdN/CAA and [1] is CmdN/CAB
	 * DQByteMap[2] - CmdSDQByteMap: [0] is CmdS/CAA and [1] is CmdS/CAB
	 * DQByteMap[3] - CkeDQByteMap : [0] is CKE /CAA and [1] is CKE /CAB
	 *                For DDR, DQByteMap[3:1] = [0xFF, 0]
	 * DQByteMap[4] - CtlDQByteMap : Always program to [0xFF, 0]
	 *                since we have 1 CTL / rank
	 * DQByteMap[5] - CmdVDQByteMap: Always program to [0xFF, 0]
	 *                since we have 1 CA Vref
	 */
	uint8_t dq_map[2][6][2];

	/*
	 * LPDDR3 Map from CPU DQS pins to SDRAM DQS pins
	 * [CHANNEL][MAX_BYTES]
	 */
	uint8_t dqs_map[2][8];

	/* Data read from flash and passed into MRC */
	const void *saved_data;
	int saved_data_size;

	/* Disable use of saved data (can be set by mainboard) */
	int disable_saved_data;

	/* Data from MRC that should be saved to flash */
	void *data_to_save;
	int data_to_save_size;
	struct pei_memory_info meminfo;
} __packed;

#endif
