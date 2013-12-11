/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2013 Sage Electronic Engineering, LLC.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
 */

unsigned char user_ddr3_spd[128] =
{
	// SPD contents for Elpida 2Gb - 128M x 16 bits
	0x11, //  0 Number of SPD Bytes used / Total SPD Size / CRC Coverage
		  //    bits[3:0]: 1 = 128 SPD Bytes Used
		  //    bits[6:4]: 1 = 256 SPD Bytes Total
		  //    bit7     : 0 = CRC covers bytes 0 ~ 125

	0x10, //  1 SPD Revision -
		  //    0x10 = Revision 1.0

	0x0b, //  2 Key Byte / DRAM Device Type
		  //    bits[7:0]: 0x0b = DDR3 SDRAM

	0x03, //  3 Key Byte / Module Type
		  //    bits[3:0]: 3 = SO-DIMM
		  //    bits[7:4]:     reserved

	0x03, //  4 SDRAM CHIP Density and Banks
		  //    bits[3:0]: 3 = 2 Gigabits Total SDRAM capacity per chip
		  //    bits[6:4]: 0 = 3 (8 banks)
		  //    bit7     :     reserved

	0x11, //  5 SDRAM Addressing
		  //    bits[2:0]: 1 = 10 Column Address Bits
		  //    bits[5:3]: 2 = 14 Row Address Bits
		  //    bits[7:6]:     reserved

	0x00, //  6 Module Nominal Voltage, VDD
		  //    bit0     : 0 = 1.5 V operable
		  //    bit1     : 0 = NOT 1.35 V operable
		  //    bit2     : 0 = NOT 1.25 V operable
		  //    bits[7:3]:     reserved

	0x02, //  7 Module Organization
		  //    bits[2:0]: 2 = 16 bits
		  //    bits[5:3]: 0 = 1 Rank
		  //    bits[7:6]:     reserved

	0x03, //  8 Module Memory Bus Width
		  //    bits[2:0]: 3 = Primary bus width is 64 bits
		  //    bits[4:3]: 0 = 0 bits (no bus width extension)
		  //    bits[7:5]:     reserved

	0x52, //  9 Fine Timebase (FTB) Dividend / Divisor
		  //    bits[3:0]: 0x02 divisor
		  //    bits[7:4]: 0x05 dividend
		  //               5 / 2 = 2.5ps

	0x01, // 10 Medium Timebase (MTB) Dividend
	0x08, // 11 Medium Timebase (MTB) Divisor
		  //    1 / 8 = .125 ns - used for clock freq of 400 through 1066 MHz

	0x0c, // 12 SDRAM Minimum Cycle Time (tCKmin)
		  //    0x0a  = tCKmin of 1.25 ns = DDR3-1600 (800 MHz clock)
		  //    0x0c  = tCKmin of 1.25 ns = DDR3-1333 (667 MHz clock)

	0x00, // 13 Reserved

	0xfc, // 14 CAS Latencies Supported, Least Significant Byte
		  //    0xfc = CL 6,7,8,9,10,11
		  //    0xfe = CL 5,6,7,8,9,10,11
	0x00, // 15 CAS Latencies Supported, Most Significant Byte
		  //    Cas Latencies of 11 - 5 are supported

	0x69, // 16 Minimum CAS Latency Time (tAAmin)
		  //    0x69 = 13.125ns
		  //    0x6E = 13.75ns - DDR3-1600K

	0x78, // 17 Minimum Write Recovery Time (tWRmin)
		  //    0x78 = tWR of 15ns - All DDR3 speed grades

	0x69, // 18 Minimum RAS# to CAS# Delay Time (tRCDmin)
		  //    0x69 = 13.125ns
		  //    0x6E = 13.75ns -  DDR3-1600K

	0x38, // 19 Minimum Row Active to Row Active Delay Time (tRRDmin)
		  //   0x38 = 7.0ns
		  //   0x3C = 7.5ns

	0x69, // 20 Minimum Row Precharge Delay Time (tRPmin)
		  //    0x69 = 13.125ns -
		  //    0x6E = 13.75ns -  DDR3-1600K

	0x11, // 21 Upper Nibbles for tRAS and tRC
		  //    bits[3:0]: tRAS most significant nibble = 1 (see byte 22)
		  //    bits[7:4]: tRC most significant nibble = 1 (see byte 23)

	0x20, // 22 Minimum Active to Precharge Delay Time (tRASmin), LSB
		  //    0x118 = 35ns - DDR3-1600 (see byte 21)
		  //    0x120 = 36ns - DDR3

	0x89, // 23 Minimum Active to Active/Refresh Delay Time (tRCmin), LSB
		  //    0x186 = 48.75ns - DDR3-1600K
		  //    0x189 = 49.125ns - DDR3-

	0x00, // 24 Minimum Refresh Recovery Delay Time (tRFCmin), LSB
	0x05, // 25 Minimum Refresh Recovery Delay Time (tRFCmin), MSB
		  //    0x500 = 160ns - for 2 Gigabit chips

	0x3c, // 26 Minimum Internal Write to Read Command Delay Time (tWTRmin)
		  //    0x3c = 7.5 ns - All DDR3 SDRAM speed bins

	0x3c, // 27 Minimum Internal Read to Precharge Command Delay Time (tRTPmin)
		  //    0x3c =  7.5ns -  All DDR3 SDRAM speed bins

	0x01, // 28 Upper Nibble for tFAWmin
	0x68, // 29 Minimum Four Activate Window Delay Time (tFAWmin)
		  //    0x0140 = 40ns -  DDR3-1600, 2 KB page size
		  //    0x0168 = 45ns -  DDR3-    , 2 KB page size

	0x83, // 30 SDRAM Optional Feature
		  //    bit0     : 1= RZQ/6 supported
		  //    bit1     : 1 = RZQ/7 supported
		  //    bits[6:2]:     reserved
		  //    bit7     : 1 = DLL Off mode supported

	0x05, // 31 SDRAM Thermal and Refresh Options
		  //    bit0     : 1 = Temp up to 95c supported
		  //    bit1     : 0 = 85-95c uses 2x refresh rate
		  //    bit2     : 1 = Auto Self Refresh supported
		  //    bit3     : 0 = no on die thermal sensor
		  //    bits[6:4]:     reserved
		  //    bit7     : 0 = partial self refresh supported

	0x00, // 32 Module Thermal Sensor
		  //    0 = Thermal sensor not incorporated onto this assembly

	0x00, // 33 SDRAM Device Type
		  //    bits[1:0]: 0 = Signal Loading not specified
		  //    bits[3:2]:     reserved
		  //    bits[6:4]: 0 = Die count not specified
		  //    bit7     : 0 = Standard Monolithic DRAM Device

	0x00, // 34 Fine Offset for SDRAM Minimum Cycle Time (tCKmin)
	0x00, // 35 Fine Offset for Minimum CAS Latency Time (tAAmin)
	0x00, // 36 Fine Offset for Minimum RAS# to CAS# Delay Time (tRCDmin)
	0x00, // 37 Fine Offset for Minimum Row Precharge Delay Time (tRPmin)
	0x00, // 38 Fine Offset for Minimum Active to Active/Refresh Delay (tRCmin)

	0x00, // 39 (reserved)
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // 40 - 47 (reserved)
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // 48 - 55 (reserved)
	0x00, 0x00, 0x00, 0x00, // 56 - 59 (reserved)

	0x00, // 60 Raw Card Extension, Module Nominal Height
		  //    bits[4:0]: 0 = <= 15mm tall
		  //    bits[7:5]: 0 = raw card revision 0-3

	0x00, // 61 Module Maximum Thickness
		  //    bits[3:0]: 0 = thickness front <= 1mm
		  //    bits[7:4]: 0 = thinkness back <= 1mm

	0x00, // 62 Reference Raw Card Used
		  //    bits[4:0]: 0 = Reference Raw card A used
		  //    bits[6:5]: 0 = revision 0
		  //    bit7     : 0 = Reference raw cards A through AL

	0x00, // 63 Address Mapping from Edge Connector to DRAM
		  //    bit0     : 0 = standard mapping (not mirrored)
		  //    bits[7:1]:     reserved

	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, //  64 -  71 (reserved)
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, //  72 -  79 (reserved)
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, //  80 -  87 (reserved)
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, //  88 -  95 (reserved)
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, //  96 - 103 (reserved)
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // 104 - 111 (reserved)
	0x00, 0x00, 0x00, 0x00, 0x00,                   // 112 - 116 (reserved)

	0x00, 0x01, // 117 - 118 Module ID: Module Manufacturers JEDEC ID Code
				//           0x0001 = AMD

	0x00, // 119 Module ID: Module Manufacturing Location - oem specified
	0x13, // 120 Module ID: Module Manufacture Year in BCD
		  //     0x13 = 2013

	0x12, // 121 Module ID: Module Manufacture week
		  //     0x12 = 12th week

	0x53, 0x41, 0x47, 0x45, // 122 - 125: Module Serial Number
	0x00, 0x00,             // 126 - 127: Cyclical Redundancy Code
};
