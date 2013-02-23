/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2009 One Laptop per Child, Association, Inc.
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
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */

// Set P6IF DRDY Timing
// Because there are 1.5T & 2.5T CAS latency in DDR1 mode, we need to use RDELAYMD-0
//
//      Entry:
//        EBP[29:25] = DRAM Speed, Dual_Channel
//        VIA_NB2HOST_REG54[7:5]        Host Frequency
//        VIA_NB3DRAM_REG62[2:0]        CAS Latency
//
//      Modify NB_Reg:
//        VIA_NB2HOST_REG54[3,1]
//        VIA_NB2HOST_REG55[1]
//        VIA_NB2HOST_REG60
//        VIA_NB2HOST_REG61
//        VIA_NB2HOST_REG62[3:0]
//        VIA_NB2HOST_REG63
//        VIA_NB2HOST_REG64
//        VIA_NB2HOST_REG65[3:0]
//        VIA_NB2HOST_REG66
//        VIA_NB2HOST_REG67[5:4]
//
// Processing:
//--------------------------------------------------------------------------
// P6IF DRDY Timing Control:
// *Following algorithm to set DRDY timing
// Set P6IF DRDY Timing by the following 3      conditions:
// 1. RDELAYMD
//    a.RDRPH(MD        input internal timing control)
//    b.CAS Latency
//    RDELAYMD(1bit) = bit0 of (CL + RDRPH)
//    for example: RDRPH=10b, CL3 -> F3_Rx56[5:4]=11b, 10b + 11b        = 101b, RDELAYMD=1 (bit0)
//                RDRPH=00b, CL2.5 -> F3_Rx56[5:4]=10b, 00b + 10b = 010b, RDELAYMD=0 (bit0)
// 2. CPU Frequency
// 3. DRAM Frequency
//
// According to above conditions, we create different tables:
// 1. RDELAYMD=0        : for integer CAS latency(ex. CL=3)
// 2. RDELAYMD=1        : for non-integer CAS latency(ex. CL=2.5)
// 3. Normal performance
// 4. Top performance :
//                     Using phase0 to a case has better performance.
//
// Note: The setting are        related to performance and maybe affect DRAM initialize.
//      Turn OFF(F2_Rx51[7]=0) this feature at csDRAMRegInitValueJ procedure.
//      Turn ON(F2_Rx51[7]=1) this feature at csDRAMRegFinalValueJ procedure.
//
// If F2_Rx51[7]=0, then        CPU always wait 8QW, a slower but most stable way
// If F2_Rx51[7]=1, then        the timing will refer to F2_Rx60 ~ F2_Rx67,
// a fast way but may cause the system to be unstable.
//
// Coding:
// 1. RDELAYMD and user's option        for performance can determine which table
// 2. CPU Frequency can get block offset        of table
// 3. DRAM Frequency can        get row offset of block
// 4. Set value
//
// PS: Fun2 Rx62, Rx65, Rx67 are        don't care bits in 3296, CPU 266MHz doesn't be supported by 3296,
//     but I still keep these bits in table to avoid the        usage in future
//     and do the fewest        modification for code.
//

// Early 3T
// Early 3T
#define P6IF_Misc_RFASTH		0x08
#define P6IF_Misc2_RRRDYH3E		0x10
#define P6IF_Misc2_RHTSEL		0x02

#define Rx54E3T			P6IF_Misc_RFASTH
#define Rx55E3T			P6IF_Misc2_RRRDYH3E

// Early 2T
#define Rx54E2T			0x00
#define Rx55E2T			P6IF_Misc2_RRRDYH3E

// Early 1T
#define Rx54E1T			0x00
#define Rx55E1T			0x00

// Early 0T
#define Rx54E0T			P6IF_Misc_RFASTH
#define Rx55E0T			P6IF_Misc2_RRRDYH3E + P6IF_Misc2_RHTSEL

// Latter       1T
#define Rx54L1T			P6IF_Misc_RFASTH
#define Rx55L1T			P6IF_Misc2_RHTSEL

#define PH0_0_0_0	0x00
#define PH0_0_0_1	0x01
#define PH0_0_0_2	0x02
#define PH0_0_0_3	0x03
#define PH0_0_1_0	0x04
#define PH0_0_1_1	0x05
#define PH0_0_1_2	0x06
#define PH0_0_2_1	0x09
#define PH0_0_2_2	0x0a
#define PH0_0_2_3	0x0b
#define PH0_0_3_2	0x0e
#define PH0_0_3_3	0x0f
#define PH0_1_1_0	0x14
#define PH0_1_1_1	0x15
#define PH0_2_1_2	0x26
#define PH0_2_2_1	0x29
#define PH0_2_2_2	0x2a
#define PH0_2_2_3	0x2b
#define PH0_2_3_2	0x2e
#define PH0_2_3_3	0x2f
#define PH0_3_2_2	0x3a
#define PH0_3_3_3	0x3f
#define PH1_0_0_0	0x40
#define PH1_0_0_1	0x41
#define PH1_0_1_1	0x45
#define PH1_1_1_1	0x55
#define PH1_2_1_1	0x65
#define PH1_2_2_1	0x69
#define PH2_1_1_1	0x95
#define PH2_1_2_1	0x99
#define PH2_1_2_2	0x9a
#define PH2_2_1_2	0xa6
#define PH2_2_2_1	0xa9
#define PH2_2_2_2	0xaa
#define PH2_2_3_2	0xae
#define PH2_2_3_3	0xaf
#define PH2_3_2_2	0xba
#define PH2_3_2_3	0xbb
#define PH2_3_3_2	0xbe
#define PH3_2_2_3	0xeb
#define PH3_2_3_2	0xee
#define PH3_2_3_3	0xef
#define PH3_3_3_3	0xff

#define PT894_RDRDY_TBL_Width		10
#define PT894_RDRDY_TBL_Block		60

static const u8 PT894_128bit_DELAYMD0_RCONV0[6][6][PT894_RDRDY_TBL_Width] =
//    -----------------------------------------------------------------------------------------------------------------
//    RX60           RX61           RX62            RX63           RX64       RX65           RX66  RX67   RX54[3,1]  RX55[3,1]    CPU/DRAM
//    LN4:1          LN8:5          LN10:9          QW4:1          QW8:5      QW10:9     WS8:1 WS10:9 RFASTH     RRRDYH3E
//                                                                                                                            RCONV          RHTSEL
//    -----------------------------------------------------------------------------------------------------------------
{
// cpu100
	{
	 {PH0_1_1_1, PH0_0_0_0, PH0_0_0_0, PH0_1_1_1, PH0_0_0_0, PH0_0_0_0, 0x00, 0x00, Rx54E3T, Rx55E3T},	// 100/100
	 {PH0_0_0_0, PH0_0_0_0, PH0_0_0_0, PH0_0_0_0, PH0_0_0_0, PH0_0_0_0, 0x00, 0x00, Rx54E3T, Rx55E3T},	// 100/133
	 {PH0_0_0_0, PH0_0_0_0, PH0_0_0_0, PH0_0_0_0, PH0_0_0_0, PH0_0_0_0, 0x00, 0x00, Rx54E3T, Rx55E3T},	// 100/166
	 {PH0_0_0_0, PH0_0_0_0, PH0_0_0_0, PH0_0_0_0, PH0_0_0_0, PH0_0_0_0, 0x00, 0x00, Rx54E3T, Rx55E3T},	// 100/200
	 {PH0_0_0_0, PH0_0_0_0, PH0_0_0_0, PH0_0_0_0, PH0_0_0_0, PH0_0_0_0, 0x00, 0x00, Rx54E3T, Rx55E3T},	// 100/266
	 {PH0_0_0_0, PH0_0_0_0, PH0_0_0_0, PH0_0_0_0, PH0_0_0_0, PH0_0_0_0, 0x00, 0x00, Rx54E3T, Rx55E3T}	// 100/333
	 },
// cpu133
	{
	 {PH0_2_2_1, PH0_0_0_0, PH0_0_0_0, PH0_2_2_1, PH0_0_0_0, PH0_0_0_0, 0x01, 0x00, Rx54E3T, Rx55E3T},	// 133/100
	 {PH1_1_1_1, PH0_0_0_0, PH0_0_0_0, PH1_1_1_1, PH0_0_0_0, PH0_0_0_0, 0x00, 0x00, Rx54E3T, Rx55E3T},	// 133/133
	 {PH0_0_0_0, PH0_0_0_0, PH0_0_0_0, PH0_0_0_0, PH0_0_0_0, PH0_0_0_0, 0x00, 0x00, Rx54E3T, Rx55E3T},	// 133/166
	 {PH0_0_0_0, PH0_0_0_0, PH0_0_0_0, PH0_0_0_0, PH0_0_0_0, PH0_0_0_0, 0x00, 0x00, Rx54E3T, Rx55E3T},	// 133/200
	 {PH0_0_0_0, PH0_0_0_0, PH0_0_0_0, PH0_0_0_0, PH0_0_0_0, PH0_0_0_0, 0x00, 0x00, Rx54E3T, Rx55E3T},	// 133/266
	 {PH0_0_0_0, PH0_0_0_0, PH0_0_0_0, PH0_0_0_0, PH0_0_0_0, PH0_0_0_0, 0x00, 0x00, Rx54E3T, Rx55E3T}	// 133/333
	 },
// cpu200
	{
	 {PH0_3_3_3, PH0_0_0_0, PH0_0_0_0, PH0_2_2_2, PH0_0_0_0, PH0_0_0_0, 0x07, 0x00, Rx54E2T, Rx55E2T},	// 200/100
	 {PH2_3_2_3, PH0_0_0_0, PH0_0_0_0, PH2_3_2_3, PH0_0_0_0, PH0_0_0_0, 0x0a, 0x00, Rx54E3T, Rx55E3T},	// 200/133
	 {PH1_2_2_1, PH0_0_0_1, PH0_0_0_0, PH1_2_2_1, PH0_0_0_1, PH0_0_0_0, 0x01, 0x00, Rx54E3T, Rx55E3T},	// 200/166
	 {PH1_1_1_1, PH0_0_1_1, PH0_0_0_0, PH1_1_1_1, PH0_0_1_1, PH0_0_0_0, 0x00, 0x00, Rx54E3T, Rx55E3T},	// 200/200
	 {PH0_0_0_0, PH0_0_0_0, PH0_0_0_0, PH0_0_0_0, PH0_0_0_0, PH0_0_0_0, 0x00, 0x00, Rx54E3T, Rx55E3T},	// 200/266
	 {PH0_0_0_0, PH0_0_0_0, PH0_0_0_0, PH0_0_0_0, PH0_0_0_0, PH0_0_0_0, 0x00, 0x00, Rx54E3T, Rx55E3T}	// 200/333
	 },
// cpu166
	{
	 {PH0_2_3_3, PH0_0_0_0, PH0_0_0_0, PH0_2_2_3, PH0_0_0_0, PH0_0_0_0, 0x05, 0x00, Rx54E3T, Rx55E3T},	// 166/100
	 {PH1_2_2_1, PH0_0_0_0, PH0_0_0_0, PH1_2_2_1, PH0_0_0_0, PH0_0_0_0, 0x01, 0x00, Rx54E3T, Rx55E3T},	// 166/133
	 {PH1_1_1_1, PH0_0_0_1, PH0_0_0_0, PH1_1_1_1, PH0_0_0_1, PH0_0_0_0, 0x00, 0x00, Rx54E3T, Rx55E3T},	// 166/166
	 {PH0_0_0_0, PH0_0_0_0, PH0_0_0_0, PH0_0_0_0, PH0_0_0_0, PH0_0_0_0, 0x00, 0x00, Rx54E3T, Rx55E3T},	// 166/200
	 {PH0_0_0_0, PH0_0_0_0, PH0_0_0_0, PH0_0_0_0, PH0_0_0_0, PH0_0_0_0, 0x00, 0x00, Rx54E3T, Rx55E3T},	// 166/266
	 {PH0_0_0_0, PH0_0_0_0, PH0_0_0_0, PH0_0_0_0, PH0_0_0_0, PH0_0_0_0, 0x00, 0x00, Rx54E3T, Rx55E3T}	// 166/333
	 },
// cpu266
	{
	 {PH0_2_2_3, PH0_0_0_0, PH0_0_0_0, PH0_0_1_1, PH0_0_0_0, PH0_0_0_0, 0x07, 0x00, Rx54E1T, Rx55E1T},	// 266/100
	 {PH3_3_3_3, PH0_0_0_0, PH0_0_0_0, PH2_2_2_2, PH0_0_0_0, PH0_0_0_0, 0x0f, 0x00, Rx54E2T, Rx55E2T},	// 266/133
	 {PH3_2_3_3, PH0_0_0_3, PH0_0_0_0, PH3_2_3_3, PH0_0_0_2, PH0_0_0_0, 0x0d, 0x00, Rx54E3T, Rx55E3T},	// 266/166
	 {PH2_2_2_2, PH0_0_2_2, PH0_0_0_0, PH2_1_2_2, PH0_0_1_2, PH0_0_0_0, 0x12, 0x00, Rx54E3T, Rx55E3T},	// 266/200
	 {PH1_1_1_1, PH1_1_1_1, PH0_0_0_0, PH1_1_1_1, PH1_1_1_1, PH0_0_0_0, 0x00, 0x00, Rx54E3T, Rx55E3T},	// 266/266
	 {PH0_0_0_0, PH0_0_0_0, PH0_0_0_0, PH0_0_0_0, PH0_0_0_0, PH0_0_0_0, 0x00, 0x00, Rx54E3T, Rx55E3T}	// 266/333
	 },
// cpu333
	{
	 {PH0_1_1_0, PH0_0_0_0, PH0_0_0_0, PH0_0_0_0, PH0_0_0_0, PH0_0_0_0, 0x07, 0x00, Rx54E0T, Rx55E0T},	// 333/100
	 {PH1_1_1_1, PH0_0_0_0, PH0_0_0_0, PH1_1_1_1, PH0_0_0_0, PH0_0_0_0, 0x0f, 0x00, Rx54E1T, Rx55E1T},	// 333/133
	 {PH3_3_3_3, PH0_0_0_3, PH0_0_0_0, PH3_3_3_3, PH0_0_0_3, PH0_0_0_0, 0x1f, 0x00, Rx54E2T, Rx55E2T},	// 333/166
	 {PH2_2_1_2, PH0_0_2_1, PH0_0_0_0, PH1_2_1_1, PH0_0_2_1, PH0_0_0_0, 0x36, 0x00, Rx54E2T, Rx55E2T},	// 333/200
	 {PH2_1_1_1, PH2_1_1_1, PH0_0_0_0, PH2_1_1_1, PH2_1_1_1, PH0_0_0_0, 0x44, 0x00, Rx54E3T, Rx55E3T},	// 333/266
	 {PH2_2_2_2, PH2_2_2_2, PH0_0_2_2, PH2_2_2_2, PH2_2_2_2, PH0_0_2_2, 0x00, 0x00, Rx54E3T, Rx55E3T}	// 333/333
	 }
};

static const u8 PT894_128bit_DELAYMD1_RCONV0[6][6][PT894_RDRDY_TBL_Width] =
//    -----------------------------------------------------------------------------------------------------------------
//    RX60           RX61           RX62            RX63           RX64       RX65           RX66  RX67   RX54[3,1]  RX55[3,1]    CPU/DRAM
//    LN4:1          LN8:5          LN10:9          QW4:1          QW8:5      QW10:9     WS8:1 WS10:9 RFASTH     RRRDYH3E
//                                                                                                                            RCONV          RHTSEL
//    -----------------------------------------------------------------------------------------------------------------
{
// cpu100
	{
	 {PH0_1_1_1, PH0_0_0_0, PH0_0_0_0, PH0_1_1_1, PH0_0_0_0, PH0_0_0_0, 0x00, 0x00, Rx54E3T, Rx55E3T},	// 100/100
	 {PH0_0_0_0, PH0_0_0_0, PH0_0_0_0, PH0_0_0_0, PH0_0_0_0, PH0_0_0_0, 0x00, 0x00, Rx54E3T, Rx55E3T},	// 100/133
	 {PH0_0_0_0, PH0_0_0_0, PH0_0_0_0, PH0_0_0_0, PH0_0_0_0, PH0_0_0_0, 0x00, 0x00, Rx54E3T, Rx55E3T},	// 100/166
	 {PH0_0_0_0, PH0_0_0_0, PH0_0_0_0, PH0_0_0_0, PH0_0_0_0, PH0_0_0_0, 0x00, 0x00, Rx54E3T, Rx55E3T},	// 100/200
	 {PH0_0_0_0, PH0_0_0_0, PH0_0_0_0, PH0_0_0_0, PH0_0_0_0, PH0_0_0_0, 0x00, 0x00, Rx54E3T, Rx55E3T},	// 100/266
	 {PH0_0_0_0, PH0_0_0_0, PH0_0_0_0, PH0_0_0_0, PH0_0_0_0, PH0_0_0_0, 0x00, 0x00, Rx54E3T, Rx55E3T}	// 100/333
	 },
// cpu133
	{
	 {PH0_3_2_2, PH0_0_0_0, PH0_0_0_0, PH0_3_2_2, PH0_0_0_0, PH0_0_0_0, 0x02, 0x00, Rx54E3T, Rx55E3T},	// 133/100
	 {PH1_1_1_1, PH0_0_0_0, PH0_0_0_0, PH1_1_1_1, PH0_0_0_0, PH0_0_0_0, 0x00, 0x00, Rx54E3T, Rx55E3T},	// 133/133
	 {PH1_0_0_0, PH0_0_0_0, PH0_0_0_0, PH1_0_0_0, PH0_0_0_0, PH0_0_0_0, 0x00, 0x00, Rx54E3T, Rx55E3T},	// 133/166
	 {PH0_0_0_0, PH0_0_0_0, PH0_0_0_0, PH0_0_0_0, PH0_0_0_0, PH0_0_0_0, 0x00, 0x00, Rx54E3T, Rx55E3T},	// 133/200
	 {PH0_0_0_0, PH0_0_0_0, PH0_0_0_0, PH0_0_0_0, PH0_0_0_0, PH0_0_0_0, 0x00, 0x00, Rx54E3T, Rx55E3T},	// 133/266
	 {PH0_0_0_0, PH0_0_0_0, PH0_0_0_0, PH0_0_0_0, PH0_0_0_0, PH0_0_0_0, 0x00, 0x00, Rx54E3T, Rx55E3T}	// 133/333
	 },
// cpu200
	{
	 {PH0_2_2_2, PH0_0_0_0, PH0_0_0_0, PH0_1_1_1, PH0_0_0_0, PH0_0_0_0, 0x07, 0x00, Rx54E1T, Rx55E1T},	// 200/100
	 {PH2_2_2_2, PH0_0_0_0, PH0_0_0_0, PH2_1_2_1, PH0_0_0_0, PH0_0_0_0, 0x0a, 0x00, Rx54E2T, Rx55E2T},	// 200/133
	 {PH2_2_2_2, PH0_0_0_2, PH0_0_0_0, PH2_2_2_2, PH0_0_0_2, PH0_0_0_0, 0x04, 0x00, Rx54E3T, Rx55E3T},	// 200/166
	 {PH2_2_2_2, PH0_0_2_2, PH0_0_0_0, PH2_2_2_2, PH0_0_2_2, PH0_0_0_0, 0x00, 0x00, Rx54E3T, Rx55E3T},	// 200/200
	 {PH0_0_0_0, PH0_0_0_0, PH0_0_0_0, PH0_0_0_0, PH0_0_0_0, PH0_0_0_0, 0x00, 0x00, Rx54E3T, Rx55E3T},	// 200/266
	 {PH0_0_0_0, PH0_0_0_0, PH0_0_0_0, PH0_0_0_0, PH0_0_0_0, PH0_0_0_0, 0x00, 0x00, Rx54E3T, Rx55E3T}	// 200/333
	 },
// cpu166
	{
	 {PH0_2_2_2, PH0_0_0_0, PH0_0_0_0, PH0_2_1_2, PH0_0_0_0, PH0_0_0_0, 0x05, 0x00, Rx54E2T, Rx55E2T},	// 166/100
	 {PH2_3_2_2, PH0_0_0_0, PH0_0_0_0, PH2_2_2_2, PH0_0_0_0, PH0_0_0_0, 0x02, 0x00, Rx54E3T, Rx55E3T},	// 166/133
	 {PH2_2_2_2, PH0_0_0_2, PH0_0_0_0, PH2_2_2_2, PH0_0_0_2, PH0_0_0_0, 0x00, 0x00, Rx54E3T, Rx55E3T},	// 166/166
	 {PH1_0_0_0, PH0_0_0_1, PH0_0_0_0, PH1_0_0_0, PH0_0_0_1, PH0_0_0_0, 0x00, 0x00, Rx54E3T, Rx55E3T},	// 166/200
	 {PH0_0_0_0, PH0_0_0_0, PH0_0_0_0, PH0_0_0_0, PH0_0_0_0, PH0_0_0_0, 0x00, 0x00, Rx54E3T, Rx55E3T},	// 166/266
	 {PH0_0_0_0, PH0_0_0_0, PH0_0_0_0, PH0_0_0_0, PH0_0_0_0, PH0_0_0_0, 0x00, 0x00, Rx54E3T, Rx55E3T}	// 166/333
	 },
// cpu266
	{
	 {PH0_1_1_1, PH0_0_0_0, PH0_0_0_0, PH0_0_0_0, PH0_0_0_0, PH0_0_0_0, 0x07, 0x00, Rx54E0T, Rx55E0T},	// 266/100
	 {PH2_2_2_2, PH0_0_0_0, PH0_0_0_0, PH1_1_1_1, PH0_0_0_0, PH0_0_0_0, 0x0f, 0x00, Rx54E1T, Rx55E1T},	// 266/133
	 {PH2_2_2_2, PH0_0_0_2, PH0_0_0_0, PH2_2_1_2, PH0_0_0_2, PH0_0_0_0, 0x15, 0x00, Rx54E2T, Rx55E2T},	// 266/166
	 {PH3_2_3_3, PH0_0_2_3, PH0_0_0_0, PH2_2_3_2, PH0_0_2_3, PH0_0_0_0, 0x24, 0x00, Rx54E3T, Rx55E3T},	// 266/200
	 {PH2_2_2_2, PH2_2_2_2, PH0_0_0_0, PH2_2_2_2, PH2_2_2_2, PH0_0_0_0, 0x00, 0x00, Rx54E3T, Rx55E3T},	// 266/266
	 {PH0_0_0_1, PH0_0_1_1, PH0_0_1_0, PH0_0_0_1, PH0_0_1_1, PH0_0_1_0, 0x00, 0x00, Rx54E3T, Rx55E3T}	// 266/333
	 },
// cpu333
	{
	 {PH0_3_2_2, PH0_0_0_0, PH0_0_0_0, PH0_0_0_0, PH0_0_0_0, PH0_0_0_0, 0x07, 0x00, Rx54E0T, Rx55E0T},	// 333/100
	 {PH1_1_1_1, PH0_0_0_0, PH0_0_0_0, PH0_0_0_0, PH0_0_0_0, PH0_0_0_0, 0x0f, 0x00, Rx54E0T, Rx55E0T},	// 333/133
	 {PH2_2_2_2, PH0_0_0_2, PH0_0_0_0, PH2_2_2_2, PH0_0_0_2, PH0_0_0_0, 0x1f, 0x00, Rx54E1T, Rx55E1T},	// 333/166
	 {PH2_3_2_2, PH0_0_3_2, PH0_0_0_0, PH2_2_2_2, PH0_0_2_2, PH0_0_0_0, 0x1b, 0x00, Rx54E2T, Rx55E2T},	// 333/200
	 {PH2_2_2_2, PH2_2_2_2, PH0_0_0_0, PH2_2_2_1, PH2_2_2_1, PH0_0_0_0, 0x88, 0x00, Rx54E3T, Rx55E3T},	// 333/266
	 {PH2_2_2_2, PH2_2_2_2, PH0_0_2_2, PH2_2_2_2, PH2_2_2_2, PH0_0_2_2, 0x00, 0x00, Rx54E3T, Rx55E3T}	// 333/333
	 }
};

static const u8 PT894_64bit_DELAYMD0_RCONV0[6][6][PT894_RDRDY_TBL_Width] =
//    -----------------------------------------------------------------------------------------------------------------
//    RX60           RX61           RX62            RX63           RX64       RX65           RX66  RX67   RX54[3,1]  RX55[3,1]    CPU/DRAM
//    LN4:1          LN8:5          LN10:9          QW4:1          QW8:5      QW10:9     WS8:1 WS10:9 RFASTH     RRRDYH3E
//                                                                                                                            RCONV          RHTSEL
//    -----------------------------------------------------------------------------------------------------------------
{
// cpu100
	{
	 {PH0_2_2_2, PH0_0_0_0, PH0_0_0_0, PH0_1_1_1, PH0_0_0_0, PH0_0_0_0, 0x07, 0x00, Rx54E3T, Rx55E3T},	// 100/100
	 {PH0_0_0_0, PH0_0_0_0, PH0_0_0_0, PH0_0_0_0, PH0_0_0_0, PH0_0_0_0, 0x09, 0x00, Rx54E3T, Rx55E3T},	// 100/133
	 {PH0_0_0_0, PH0_0_0_0, PH0_0_0_0, PH0_0_0_0, PH0_0_0_0, PH0_0_0_0, 0x00, 0x00, Rx54E3T, Rx55E3T},	// 100/166
	 {PH0_0_0_0, PH0_0_0_0, PH0_0_0_0, PH0_0_0_0, PH0_0_0_0, PH0_0_0_0, 0x00, 0x00, Rx54E3T, Rx55E3T},	// 100/200
	 {PH0_0_0_0, PH0_0_0_0, PH0_0_0_0, PH0_0_0_0, PH0_0_0_0, PH0_0_0_0, 0x00, 0x00, Rx54E3T, Rx55E3T},	// 100/266
	 {PH0_0_0_0, PH0_0_0_0, PH0_0_0_0, PH0_0_0_0, PH0_0_0_0, PH0_0_0_0, 0x00, 0x00, Rx54E3T, Rx55E3T}	// 100/333
	 },
// cpu133
	{
	 {PH0_2_3_2, PH0_0_0_0, PH0_0_0_0, PH0_0_1_0, PH0_0_0_0, PH0_0_0_0, 0x07, 0x00, Rx54E2T, Rx55E2T},	// 133/100
	 {PH2_2_2_2, PH0_0_0_0, PH0_0_0_0, PH1_1_1_1, PH0_0_0_0, PH0_0_0_0, 0x0f, 0x00, Rx54E3T, Rx55E3T},	// 133/133
	 {PH1_0_0_0, PH0_0_0_1, PH0_0_0_0, PH0_0_0_0, PH0_0_0_0, PH0_0_0_0, 0x07, 0x00, Rx54E3T, Rx55E3T},	// 133/166
	 {PH0_0_0_0, PH0_0_0_0, PH0_0_0_0, PH0_0_0_0, PH0_0_0_0, PH0_0_0_0, 0x00, 0x00, Rx54E3T, Rx55E3T},	// 133/200
	 {PH0_0_0_0, PH0_0_0_0, PH0_0_0_0, PH0_0_0_0, PH0_0_0_0, PH0_0_0_0, 0x00, 0x00, Rx54E3T, Rx55E3T},	// 133/266
	 {PH0_0_0_0, PH0_0_0_0, PH0_0_0_0, PH0_0_0_0, PH0_0_0_0, PH0_0_0_0, 0x00, 0x00, Rx54E3T, Rx55E3T}	// 133/333
	 },
// cpu200
	{
	 {PH0_3_3_3, PH0_0_0_0, PH0_0_0_0, PH0_0_0_0, PH0_0_0_0, PH0_0_0_0, 0x07, 0x00, Rx54E0T, Rx55E0T},	// 200/100
	 {PH2_2_2_2, PH0_0_0_0, PH0_0_0_0, PH0_0_0_0, PH0_0_0_0, PH0_0_0_0, 0x0f, 0x00, Rx54E1T, Rx55E1T},	// 200/133
	 {PH3_3_3_3, PH0_0_0_3, PH0_0_0_0, PH1_2_2_1, PH0_0_0_1, PH0_0_0_0, 0x1f, 0x00, Rx54E3T, Rx55E3T},	// 200/166
	 {PH2_2_2_2, PH0_0_2_2, PH0_0_0_0, PH1_1_1_1, PH0_0_1_1, PH0_0_0_0, 0x3f, 0x00, Rx54E3T, Rx55E3T},	// 200/200
	 {PH0_0_0_0, PH0_0_0_0, PH0_0_0_0, PH0_0_0_0, PH0_0_0_0, PH0_0_0_0, 0x00, 0x00, Rx54E1T, Rx55E1T},	// 200/266
	 {PH0_0_0_0, PH0_0_0_0, PH0_0_0_0, PH0_0_0_0, PH0_0_0_0, PH0_0_0_0, 0x00, 0x00, Rx54E3T, Rx55E3T}	// 200/333
// DDR2 Both E3T and E2T Fail, need set to E1T,  db     PH0_0_0_0, PH0_0_0_0, PH0_0_0_0, PH0_0_0_0, PH0_0_0_0, PH0_0_0_0,       00110011b, 00000000b, Rx54E3T,  Rx55E3T  ;200/266
	 },
// cpu166
	{
	 {PH0_2_3_2, PH0_0_0_0, PH0_0_0_0, PH0_0_0_0, PH0_0_0_0, PH0_0_0_0, 0x07, 0x00, Rx54E1T, Rx55E1T},	// 166/100
	 {PH2_2_2_2, PH0_0_0_0, PH0_0_0_0, PH0_0_1_0, PH0_0_0_0, PH0_0_0_0, 0x0f, 0x00, Rx54E2T, Rx55E2T},	// 166/133
	 {PH2_2_2_2, PH0_0_0_2, PH0_0_0_0, PH1_1_1_1, PH0_0_0_1, PH0_0_0_0, 0x1f, 0x00, Rx54E3T, Rx55E3T},	// 166/166
	 {PH1_0_0_1, PH0_0_1_1, PH0_0_0_0, PH0_0_0_0, PH0_0_0_0, PH0_0_0_0, 0x1e, 0x00, Rx54E3T, Rx55E3T},	// 166/200
	 {PH0_0_0_0, PH0_0_0_0, PH0_0_0_0, PH0_0_0_0, PH0_0_0_0, PH0_0_0_0, 0x00, 0x00, Rx54E3T, Rx55E3T},	// 166/266
	 {PH0_0_0_0, PH0_0_0_0, PH0_0_0_0, PH0_0_0_0, PH0_0_0_0, PH0_0_0_0, 0x00, 0x00, Rx54E3T, Rx55E3T}	// 166/333
	 },
// cpu266
	{
	 {PH0_2_2_2, PH0_0_0_0, PH0_0_0_0, PH0_0_0_0, PH0_0_0_0, PH0_0_0_0, 0x07, 0x00, Rx54L1T, Rx55L1T},	// 266/100
	 {PH1_1_1_1, PH0_0_0_0, PH0_0_0_0, PH0_0_0_0, PH0_0_0_0, PH0_0_0_0, 0x0f, 0x00, Rx54L1T, Rx55L1T},	// 266/133
	 {PH3_2_3_2, PH0_0_0_2, PH0_0_0_0, PH0_0_0_0, PH0_0_0_0, PH0_0_0_0, 0x1f, 0x00, Rx54E1T, Rx55E1T},	// 266/166
	 {PH3_2_2_3, PH0_0_2_2, PH0_0_0_0, PH1_0_0_1, PH0_0_0_0, PH0_0_0_0, 0x3f, 0x00, Rx54E2T, Rx55E2T},	// 266/200
	 {PH2_2_2_2, PH2_2_2_2, PH0_0_0_0, PH1_1_1_1, PH1_1_1_1, PH0_0_0_0, 0xff, 0x00, Rx54E3T, Rx55E3T},	// 266/266
	 {PH0_0_1_1, PH0_1_1_1, PH0_0_1_0, PH0_0_0_0, PH0_0_0_0, PH0_0_0_0, 0x9c, 0x03, Rx54E3T, Rx55E3T}	// 266/333
	 },
// cpu333
	{
	 {PH0_0_0_0, PH0_0_0_0, PH0_0_0_0, PH0_0_0_0, PH0_0_0_0, PH0_0_0_0, 0x00, 0x00, Rx54L1T, Rx55L1T},	// 333/100  ;DO NOT Support
	 {PH2_2_2_2, PH0_0_0_0, PH0_0_0_0, PH0_0_0_0, PH0_0_0_0, PH0_0_0_0, 0x0f, 0x00, Rx54L1T, Rx55L1T},	// 333/133
	 {PH3_3_3_3, PH0_0_0_3, PH0_0_0_0, PH0_0_0_0, PH0_0_0_0, PH0_0_0_0, 0x1f, 0x00, Rx54E0T, Rx55E0T},	// 333/166
	 {PH2_3_3_2, PH0_0_3_3, PH0_0_0_0, PH0_0_0_0, PH0_0_0_0, PH0_0_0_0, 0x3f, 0x00, Rx54E1T, Rx55E1T},	// 333/200
	 {PH3_3_3_3, PH3_3_3_3, PH0_0_0_0, PH2_1_1_1, PH2_1_1_1, PH0_0_0_0, 0xff, 0x00, Rx54E3T, Rx55E3T},	// 333/266
	 {PH2_2_2_2, PH2_2_2_2, PH0_0_2_2, PH2_2_2_2, PH2_2_2_2, PH0_0_2_2, 0xff, 0x03, Rx54E3T, Rx55E3T}	// 333/333
	 }
};

static const u8 PT894_64bit_DELAYMD1_RCONV0[6][6][PT894_RDRDY_TBL_Width] =
//    -----------------------------------------------------------------------------------------------------------------
//    RX60           RX61           RX62            RX63           RX64       RX65           RX66  RX67   RX54[3,1]  RX55[3,1]    CPU/DRAM
//    LN4:1          LN8:5          LN10:9          QW4:1          QW8:5      QW10:9     WS8:1 WS10:9 RFASTH     RRRDYH3E
//                                                                                                                            RCONV          RHTSEL
//    -----------------------------------------------------------------------------------------------------------------
{
// cpu100
	{
	 {PH0_2_2_2, PH0_0_0_0, PH0_0_0_0, PH0_1_1_1, PH0_0_0_0, PH0_0_0_0, 0x07, 0x00, Rx54E3T, Rx55E3T},	// 100/100
	 {PH1_0_0_1, PH0_0_0_0, PH0_0_0_0, PH0_0_0_0, PH0_0_0_0, PH0_0_0_0, 0x06, 0x00, Rx54E3T, Rx55E3T},	// 100/133
	 {PH0_0_0_0, PH0_0_0_0, PH0_0_0_0, PH0_0_0_0, PH0_0_0_0, PH0_0_0_0, 0x00, 0x00, Rx54E3T, Rx55E3T},	// 100/166
	 {PH0_0_0_0, PH0_0_0_0, PH0_0_0_0, PH0_0_0_0, PH0_0_0_0, PH0_0_0_0, 0x00, 0x00, Rx54E3T, Rx55E3T},	// 100/200
	 {PH0_0_0_0, PH0_0_0_0, PH0_0_0_0, PH0_0_0_0, PH0_0_0_0, PH0_0_0_0, 0x00, 0x00, Rx54E3T, Rx55E3T},	// ;100/266
	 {PH0_0_0_0, PH0_0_0_0, PH0_0_0_0, PH0_0_0_0, PH0_0_0_0, PH0_0_0_0, 0x00, 0x00, Rx54E3T, Rx55E3T}	// 100/333
	 },
// cpu133
	{
	 {PH0_3_3_3, PH0_0_0_0, PH0_0_0_0, PH0_1_1_1, PH0_0_0_0, PH0_0_0_0, 0x07, 0x00, Rx54E2T, Rx55E2T},	// 133/100
	 {PH2_2_2_2, PH0_0_0_0, PH0_0_0_0, PH1_1_1_1, PH0_0_0_0, PH0_0_0_0, 0x0f, 0x00, Rx54E3T, Rx55E3T},	// 133/133
	 {PH1_0_1_1, PH0_0_0_1, PH0_0_0_0, PH1_0_0_0, PH0_0_0_0, PH0_0_0_0, 0x1c, 0x00, Rx54E3T, Rx55E3T},	// 133/166
	 {PH0_0_0_0, PH0_0_0_0, PH0_0_0_0, PH0_0_0_0, PH0_0_0_0, PH0_0_0_0, 0x09, 0x00, Rx54E3T, Rx55E3T},	// 133/200
	 {PH0_0_0_0, PH0_0_0_0, PH0_0_0_0, PH0_0_0_0, PH0_0_0_0, PH0_0_0_0, 0x00, 0x00, Rx54E3T, Rx55E3T},	// 133/266
	 {PH0_0_0_0, PH0_0_0_0, PH0_0_0_0, PH0_0_0_0, PH0_0_0_0, PH0_0_0_0, 0x00, 0x00, Rx54E3T, Rx55E3T}	// 133/333
	 },
// cpu200
	{
	 {PH0_2_2_2, PH0_0_0_0, PH0_0_0_0, PH0_0_0_0, PH0_0_0_0, PH0_0_0_0, 0x07, 0x00, Rx54L1T, Rx55L1T},	// 200/100
	 {PH3_3_3_3, PH0_0_0_0, PH0_0_0_0, PH0_0_0_0, PH0_0_0_0, PH0_0_0_0, 0x0f, 0x00, Rx54E1T, Rx55E1T},	// 200/133
	 {PH2_2_3_3, PH0_0_0_2, PH0_0_0_0, PH1_0_1_1, PH0_0_0_1, PH0_0_0_0, 0x1f, 0x00, Rx54E2T, Rx55E2T},	// 200/166
	 {PH3_3_3_3, PH0_0_3_3, PH0_0_0_0, PH2_2_2_2, PH0_0_2_2, PH0_0_0_0, 0x3f, 0x00, Rx54E3T, Rx55E3T},	// 200/200
	 {PH0_0_1_1, PH0_0_1_1, PH0_0_0_0, PH0_0_0_0, PH0_0_0_0, PH0_0_0_0, 0xcc, 0x00, Rx54E3T, Rx55E3T},	// 200/266
	 {PH0_0_0_0, PH0_0_0_0, PH0_0_0_0, PH0_0_0_0, PH0_0_0_0, PH0_0_0_0, 0x00, 0x00, Rx54E3T, Rx55E3T}	// 200/333
	 },
// cpu166
	{
	 {PH0_3_3_3, PH0_0_0_0, PH0_0_0_0, PH0_0_0_0, PH0_0_0_0, PH0_0_0_0, 0x07, 0x00, Rx54E1T, Rx55E1T},	// 166/100
	 {PH2_2_3_3, PH0_0_0_0, PH0_0_0_0, PH1_0_1_1, PH0_0_0_0, PH0_0_0_0, 0x0f, 0x00, Rx54E2T, Rx55E2T},	// 166/133
	 {PH2_2_2_2, PH0_0_0_2, PH0_0_0_0, PH2_2_2_2, PH0_0_0_2, PH0_0_0_0, 0x1f, 0x00, Rx54E3T, Rx55E3T},	// 166/166
	 {PH1_1_1_1, PH0_0_1_1, PH0_0_0_0, PH1_0_0_0, PH0_0_0_1, PH0_0_0_0, 0x39, 0x00, Rx54E3T, Rx55E3T},	// 166/200
	 {PH0_0_0_0, PH0_0_0_0, PH0_0_0_0, PH0_0_0_0, PH0_0_0_0, PH0_0_0_0, 0x00, 0x00, Rx54E3T, Rx55E3T},	// 166/266
	 {PH0_0_0_0, PH0_0_0_0, PH0_0_0_0, PH0_0_0_0, PH0_0_0_0, PH0_0_0_0, 0x00, 0x00, Rx54E3T, Rx55E3T}	// 166/333
	 },
// cpu266
	{
	 {PH0_0_0_0, PH0_0_0_0, PH0_0_0_0, PH0_0_0_0, PH0_0_0_0, PH0_0_0_0, 0x00, 0x00, Rx54L1T, Rx55L1T},	// 266/100  ;DO NOT Support
	 {PH2_2_2_2, PH0_0_0_0, PH0_0_0_0, PH0_0_0_0, PH0_0_0_0, PH0_0_0_0, 0x0f, 0x00, Rx54L1T, Rx55L1T},	// 266/133
	 {PH2_2_1_2, PH0_0_0_1, PH0_0_0_0, PH0_0_0_0, PH0_0_0_0, PH0_0_0_0, 0x1f, 0x00, Rx54E0T, Rx55E0T},	// 266/166
	 {PH3_3_3_3, PH0_0_3_3, PH0_0_0_0, PH1_1_1_1, PH0_0_1_1, PH0_0_0_0, 0x3f, 0x00, Rx54E2T, Rx55E2T},	// 266/200
	 {PH3_3_3_3, PH3_3_3_3, PH0_0_0_0, PH2_2_2_2, PH2_2_2_2, PH0_0_0_0, 0xff, 0x00, Rx54E3T, Rx55E3T},	// 266/266
	 {PH1_1_1_1, PH1_1_1_1, PH0_0_1_1, PH0_0_0_1, PH0_0_1_1, PH0_0_1_0, 0x73, 0x02, Rx54E3T, Rx55E3T}	// 266/333
	 },
// cpu333
	{
	 {PH0_0_0_0, PH0_0_0_0, PH0_0_0_0, PH0_0_0_0, PH0_0_0_0, PH0_0_0_0, 0x00, 0x00, Rx54L1T, Rx55L1T},	// 333/100  ;DO NOT Support
	 {PH3_3_3_3, PH0_0_0_0, PH0_0_0_0, PH0_0_0_0, PH0_0_0_0, PH0_0_0_0, 0x0f, 0x00, Rx54L1T, Rx55L1T},	// 333/133
	 {PH2_2_2_2, PH0_0_0_2, PH0_0_0_0, PH0_0_0_0, PH0_0_0_0, PH0_0_0_0, 0x1f, 0x00, Rx54L1T, Rx55L1T},	// 333/166
	 {PH2_2_2_2, PH0_0_2_2, PH0_0_0_0, PH0_0_0_0, PH0_0_0_0, PH0_0_0_0, 0x3f, 0x00, Rx54E1T, Rx55E1T},	// 333/200
	 {PH2_3_2_2, PH2_3_2_2, PH0_0_0_0, PH0_1_1_0, PH0_1_1_0, PH0_0_0_0, 0xff, 0x00, Rx54E2T, Rx55E2T},	// 333/266
	 {PH3_3_3_3, PH3_3_3_3, PH0_0_3_3, PH2_2_2_2, PH2_2_2_2, PH0_0_2_2, 0xff, 0x03, Rx54E3T, Rx55E3T}	// 333/333
	 }
};

void DRAMDRDYSetting(DRAM_SYS_ATTR * DramAttr)
{
	u8 Data;
	/*
	   this function has 3 switchs, correspond to 3 level of Drdy setting.
	   0:Slowest, 1:Default, 2:Optimize
	   you can only open one switch
	 */
#if 1				//this is slowest
	//  0 -> Slowest
	//Write slowest value to register

	Data = 0xAA;
	pci_write_config8(PCI_DEV(0, 0, 2), 0x60, Data);

	Data = 0x0A;
	pci_write_config8(PCI_DEV(0, 0, 2), 0x61, Data);

	Data = 0x00;
	pci_write_config8(PCI_DEV(0, 0, 2), 0x62, Data);

	Data = 0xAA;
	pci_write_config8(PCI_DEV(0, 0, 2), 0x63, Data);

	Data = 0x0A;
	pci_write_config8(PCI_DEV(0, 0, 2), 0x64, Data);

	Data = 0x00;
	pci_write_config8(PCI_DEV(0, 0, 2), 0x65, Data);

	Data = 0x00;
	pci_write_config8(PCI_DEV(0, 0, 2), 0x66, Data);

	Data = 0x00;
	pci_write_config8(PCI_DEV(0, 0, 2), 0x67, Data);

	Data = pci_read_config8(PCI_DEV(0, 0, 2), 0x54);
	Data = Data & 0xF5;
	Data |= 0x08;
	pci_write_config8(PCI_DEV(0, 0, 2), 0x54, Data);

	//Data=pci_read_config8(PCI_DEV(0,0,2), 0x55);
	//Data = Data & (~0x20);
	//pci_write_config8(PCI_DEV(0,0,2), 0x55, Data);

	//enable drdy timing
	Data = pci_read_config8(PCI_DEV(0, 0, 2), 0x51);
	Data = Data | 0x80;
	pci_write_config8(PCI_DEV(0, 0, 2), 0x51, Data);
#endif
#if 0				//default
	{
		//disable drdy timing
		Data = pci_read_config8(PCI_DEV(0, 0, 2), 0x51);
		Data = Data & 0x7F;
		pci_write_config8(PCI_DEV(0, 0, 2), 0x51, Data);
	}
#endif
#if 0				//  2:Optimize
	u8 CpuFreq, DramFreq;
	u8 CL, RDRPH;

	//CL :reg6x[2:0]
	Data = pci_read_config8(MEMCTRL, 0x62);
	CL = Data & 0x07;

	//RDRPH: reg7B[6:4]
	Data = pci_read_config8(MEMCTRL, 0x7B);
	RDRPH = (Data & 0x70) >> 4;

	//CpuFreq: F2Reg54[7:5]
	Data = pci_read_config8(PCI_DEV(0, 0, 2), 0x54);
	CpuFreq = (Data & 0xE0) >> 5;

	//DramFreq:F3Reg90[2:0]
	Data = pci_read_config8(MEMCTRL, 0x90);
	DramFreq = Data & 0x07;

	u8 DelayMode;
	DelayMode = CL + RDRPH;	// RDELAYMD = bit0 of (CAS Latency + RDRPH)
	DelayMode &= 0x01;

	u8 ProgData[PT894_RDRDY_TBL_Width];

	//In 364, there is no 128 bit
	if (DelayMode == 1) {	// DelayMode 1
		u8 Index;
		for (Index = 0; Index < PT894_RDRDY_TBL_Width; Index++)
			ProgData[Index] =
			    PT894_64bit_DELAYMD1_RCONV0[CpuFreq][DramFreq]
			    [Index];
	} else {		// DelayMode 0
		u8 Index;
		for (Index = 0; Index < PT894_RDRDY_TBL_Width; Index++)
			ProgData[Index] =
			    PT894_64bit_DELAYMD0_RCONV0[CpuFreq][DramFreq]
			    [Index];
	}

	Data = ProgData[0];
	pci_write_config8(PCI_DEV(0, 0, 2), 0x60, Data);

	Data = ProgData[1];
	pci_write_config8(PCI_DEV(0, 0, 2), 0x61, Data);

	Data = ProgData[2];
	pci_write_config8(PCI_DEV(0, 0, 2), 0x62, Data);

	Data = ProgData[3];
	pci_write_config8(PCI_DEV(0, 0, 2), 0x63, Data);

	Data = ProgData[4];
	pci_write_config8(PCI_DEV(0, 0, 2), 0x64, Data);

	Data = ProgData[5];
	pci_write_config8(PCI_DEV(0, 0, 2), 0x65, Data);

	Data = ProgData[6];
	pci_write_config8(PCI_DEV(0, 0, 2), 0x66, Data);

	Data = ProgData[7];
	pci_write_config8(PCI_DEV(0, 0, 2), 0x67, Data);

	Data = pci_read_config8(PCI_DEV(0, 0, 2), 0x54);
	Data = (Data & 0xF5) | ProgData[8];
	pci_write_config8(PCI_DEV(0, 0, 2), 0x54, Data);

	Data = pci_read_config8(PCI_DEV(0, 0, 2), 0x55);
	Data = Data & (~0x22) | ProgData[9];
	pci_write_config8(PCI_DEV(0, 0, 2), 0x62, Data);

	//enable drdy timing
	Data = pci_read_config8(PCI_DEV(0, 0, 2), 0x51);
	Data = Data | 0x80;
	pci_write_config8(PCI_DEV(0, 0, 2), 0x51, Data);
#endif
}

/*This routine process the ability for North Bridge side burst functionality
There are 3 variances that are valid:
	1. DIMM	BL=8, chipset BL=8
	2. DIMM	BL=4, chipset BL=4
	3. DIMM	BL=4, chipset BL=8 (only happened on Dual channel)
     Device 0 function 2 HOST:REG54[4] must be 1 when 128-bit mode.
Since DIMM will be initialized	in each	rank individually,
	1.If all DIMM BL=4, DIMM will initialize BL=4 first,
	  then check dual_channel flag to enable VIA_NB2HOST_REG54[4].
	2.If all DIMM BL=8, DIMM will initialize BL=8 first,
	  then check dual_channel flag for re-initialize DIMM BL=4.
	  also VIA_NB2HOST_REG54[4] need	to be enabled.
Chipset_BL8==>chipset side can	set burst length=8
two register need to set
 1. Device 0 function 2 HOST:REG54[4]
 2. Device 0 function 3 DRAM:REG6C[3]
*/
void DRAMBurstLength(DRAM_SYS_ATTR * DramAttr)
{
	u8 Data, BL;
	u8 Sockets;
	/*SPD byte16 bit3,2 describes the burst length supported. bit3=1 support BL=8 bit2=1 support BL=4 */
	BL = 0x0c;
	for (Sockets = 0; Sockets < 2; Sockets++) {
		if (DramAttr->DimmInfo[Sockets].bPresence) {
			BL &=
			    (DramAttr->
			     DimmInfo[Sockets].SPDDataBuf
			     [SPD_SDRAM_BURSTLENGTH]);
		}
	}

	/*D0F3Rx6c bit3 CHA SDRAM effective burst length, for 64bit mode ranks =0 BL=4 ; =1 BL=8 */

	if (BL & 0x08)		/*All Assembly support BL=8 */
		BL = 0x8;	/*set bit3 */
	else
		BL = 0x00;	/*clear bit3 */

	Data = pci_read_config8(MEMCTRL, 0x6c);
	Data = (u8) ((Data & 0xf7) | BL);

#if ENABLE_CHB
	if (DramAttr->RankNumChB > 0) {
		BL = DramAttr->DimmInfo[2].SPDDataBuf[SPD_SDRAM_BURSTLENGTH];
		//Rx6c[1], CHB burst length
		if (BL & 0x08)	/*CHB support BL=8 */
			BL = 0x2;	/*set bit1 */
		else
			BL = 0x00;	/*clear bit1 */

		Data = (Data & 0xFD) | BL;
	}
#endif
	pci_write_config8(MEMCTRL, 0x6c, Data);
}
