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

 /*
    Set Dram Timing functions
  */

void SetCL(DRAM_SYS_ATTR * DramAttr);

void SetTrp(DRAM_SYS_ATTR * DramAttr);

void SetTrcd(DRAM_SYS_ATTR * DramAttr);

void SetTras(DRAM_SYS_ATTR * DramAttr);

void SetTrfc(DRAM_SYS_ATTR * DramAttr);

void SetTrrd(DRAM_SYS_ATTR * DramAttr);

void SetTwr(DRAM_SYS_ATTR * DramAttr);

void SetTwtr(DRAM_SYS_ATTR * DramAttr);

void SetTrtp(DRAM_SYS_ATTR * DramAttr);

/* Set DRAM	Timing*/
void DRAMTimingSetting(DRAM_SYS_ATTR * DramAttr)
{
	PRINT_DEBUG_MEM("Set CAS latency value!");
	SetCL(DramAttr);

	PRINT_DEBUG_MEM("Set tRP value!");
	SetTrp(DramAttr);

	PRINT_DEBUG_MEM("Set tRCD value!");
	SetTrcd(DramAttr);

	PRINT_DEBUG_MEM("Set tRAS value!");
	SetTras(DramAttr);

	PRINT_DEBUG_MEM("Set tRFC value!");
	SetTrfc(DramAttr);

	PRINT_DEBUG_MEM("Set tRRD value!");
	SetTrrd(DramAttr);

	PRINT_DEBUG_MEM("Set tWR value!");
	SetTwr(DramAttr);

	PRINT_DEBUG_MEM("Set tWTR value!");
	SetTwtr(DramAttr);

	PRINT_DEBUG_MEM("Set tRTP value!");
	SetTrtp(DramAttr);
}

/*
Set DRAM Timing: CAS Latency for DDR1
D0F3RX62 bit[0:2] for CAS Latency;
*/
void SetCL(DRAM_SYS_ATTR * DramAttr)
{
	u8 Data;
	u8 CL;

	/*DDR2 CL Value: 20, 30, 40, 50 -> 2, 3, 4, 5 */
	CL = (u8) ((DramAttr->CL - 20) / 10);	//000,001,010,011

	PRINT_DEBUG_MEM("CAS = ");
	PRINT_DEBUG_MEM_HEX8(CL);
	PRINT_DEBUG_MEM("\n");
	Data = pci_read_config8(MEMCTRL, 0x62);
	Data = (u8) ((Data & 0xf8) | CL);
	pci_write_config8(MEMCTRL, 0x62, Data);
}

/*
 Minimum row precharge time, Trp for DDR1/DDR2
 D0F3Rx64[3:2] for Trp 2T~5T
*/
#define MAX_TRP 6
#define MIN_TRP 2

void SetTrp(DRAM_SYS_ATTR * DramAttr)
{
	u8 Data;
	u16 Max, Tmp;
	u8 Socket;

	/*get the max Trp value from SPD data
	   SPD Byte27, Bit7:2->1ns~63ns, Bit1:0->0ns, 0.25ns, 0.50ns, 0.75ns */
	Max = 0;
	for (Socket = 0; Socket < MAX_SOCKETS; Socket++) {
		if (DramAttr->DimmInfo[Socket].bPresence) {
			Tmp =
			    (u16) (DramAttr->
				   DimmInfo[Socket].SPDDataBuf[SPD_SDRAM_TRP]);
			if (Tmp > Max)
				Max = Tmp;
		}
		/*Calculate clock,this value should be 2T,3T,4T,5T */
	}
	Tmp =
	    (u16) ((Max * 100 + ((DramAttr->DramCyc) << 2) -
		    1) / ((DramAttr->DramCyc) << 2));
	PRINT_DEBUG_MEM("Trp = ");
	PRINT_DEBUG_MEM_HEX16(Tmp);
	PRINT_DEBUG_MEM("\r");

	if (Tmp > MAX_TRP)
		Tmp = MAX_TRP;
	else if (Tmp < MIN_TRP)
		Tmp = MIN_TRP;

	Tmp -= 2;		//00->2T, 01->3T, 10->4T, 11->5T
	Tmp <<= 1;		//bit1,2,3

	Data = pci_read_config8(MEMCTRL, 0x64);
	Data = (u8) ((Data & 0xf1) | (u8) Tmp);
	pci_write_config8(MEMCTRL, 0x64, Data);

	//enable DDR2 8-Bank Device Timing Constraint
	Data = pci_read_config8(MEMCTRL, 0x62);
	Data = (u8) ((Data & 0xf7) | 0x08);
	pci_write_config8(MEMCTRL, 0x62, Data);
}

/*
Minimum RAS to CAS dely,Trcd for DDR1/DDR2
D0F3Rx64[7:6] for Trcd
*/
#define MAX_TRCD 6
#define MIN_TRCD 2

void SetTrcd(DRAM_SYS_ATTR * DramAttr)
{
	u8 Data;
	u16 Max, Tmp;
	u8 Socket;

	/*get the max Trcd value from SPD data
	   SPD Byte29, Bit7:2->1ns~63ns, Bit1:0->0ns, 0.25ns, 0.50ns, 0.75ns */
	Max = 0;
	for (Socket = 0; Socket < MAX_SOCKETS; Socket++) {
		if (DramAttr->DimmInfo[Socket].bPresence) {
			Tmp =
			    (u16) (DramAttr->
				   DimmInfo[Socket].SPDDataBuf[SPD_SDRAM_TRCD]);
			if (Tmp > Max)
				Max = Tmp;
		}
	}
	/*Calculate clock,this value should be 2T,3T,4T,5T */
	Tmp =
	    (u16) ((Max * 100 + ((DramAttr->DramCyc) << 2) -
		    1) / ((DramAttr->DramCyc) << 2));
	PRINT_DEBUG_MEM("Trcd =");
	PRINT_DEBUG_MEM_HEX16(Tmp);
	PRINT_DEBUG_MEM("\r");

	if (Tmp > MAX_TRCD)
		Tmp = MAX_TRCD;
	else if (Tmp < MIN_TRCD)
		Tmp = MIN_TRCD;
	Tmp -= 2;		//00->2T, 01->3T, 10->4T, 11->5T
	Tmp <<= 5;		//bit5,6,7

	Data = pci_read_config8(MEMCTRL, 0x64);
	Data = (u8) ((Data & 0x1f) | (u8) Tmp);
	pci_write_config8(MEMCTRL, 0x64, Data);

}

/*
 minimum active to precharge time,Tras for DDR1/DDR2
 D0F3Rx62[7:4] Tras
*/
#define MAX_TRAS 20		//20T
#define MIN_TRAS 5		//5T
void SetTras(DRAM_SYS_ATTR * DramAttr)
{
	u8 Data;
	u16 Max, Tmp;
	u8 Socket;

	/*get the max Tras value from SPD data
	   SPD byte30: bit0:7 1ns~255ns */
	Max = 0;
	for (Socket = 0; Socket < MAX_SOCKETS; Socket++) {
		if (DramAttr->DimmInfo[Socket].bPresence) {
			Tmp =
			    (u16) (DramAttr->
				   DimmInfo[Socket].SPDDataBuf[SPD_SDRAM_TRAS]);
			if (Tmp > Max)
				Max = Tmp;
		}
	}

	/*Calculate clock,value range 5T-20T */
	Tmp = (u16) ((Max * 100 + DramAttr->DramCyc - 1) / (DramAttr->DramCyc));
	PRINT_DEBUG_MEM("Tras =");
	PRINT_DEBUG_MEM_HEX16(Tmp);
	PRINT_DEBUG_MEM("\r");

	if (Tmp > MAX_TRAS)
		Tmp = MAX_TRAS;
	else if (Tmp < MIN_TRAS)
		Tmp = MIN_TRAS;
	Tmp -= 5;		//0->5T  ... 1111->20T
	Tmp <<= 4;		//bit4:7

	Data = pci_read_config8(MEMCTRL, 0x62);
	Data = (u8) ((Data & 0x0f) | (u8) Tmp);
	pci_write_config8(MEMCTRL, 0x62, Data);
}

/*
Minimum refresh to activate/refresh command period Trfc for DDR1/DDR2
D0F3Rx61[5:0] for Trfc
*/

#define MAX_TRFC 71		// Max supported,71T
#define MIN_TRFC 8		// Min supported,8T

void SetTrfc(DRAM_SYS_ATTR * DramAttr)
{

	u8 Data;
	u32 Max, Tmp;
	u8 Byte40;
	u8 Socket;

	/*get the max Trfc value from SPD data */
	Max = 0;
	for (Socket = 0; Socket < MAX_SOCKETS; Socket++) {
		if (DramAttr->DimmInfo[Socket].bPresence) {
			Tmp =
			    (u32) (DramAttr->
				   DimmInfo[Socket].SPDDataBuf[SPD_SDRAM_TRFC])
			    * 100;
			/*only DDR2 need to add byte 40 bit[7:4] */
			Byte40 =
			    (DramAttr->
			     DimmInfo[Socket].SPDDataBuf[SPD_SDRAM_TRFC2]);
			/*if bit0 = 1, byte42(RFC)+256ns, SPD spec JEDEC standard No.21.c */
			if (Byte40 & 0x01)
				Tmp += (256 * 100);
			/*bit1,2,3 000->0ns+byte42; 001->0.25ns+byte42; 010->0.33ns+byte42; 011->0.5ns+byte42;100-> 0.75ns+byte42 */
			switch ((Byte40 >> 1) & 0x07) {	/*bit1,2,3 */
			case 1:
				Tmp += 25;
				break;
			case 2:
				Tmp += 33;
				break;
			case 3:
				Tmp += 50;
				break;
			case 4:
				Tmp += 66;
				break;
			case 5:
				Tmp += 75;
				break;
			case 6:	//what is FRU???
			default:
				break;
			}
			if (Tmp > Max)
				Max = Tmp;
		}
	}

	/*Calculate clock,value range 8T-71T */
	Tmp = (u16) ((Max + DramAttr->DramCyc - 1) / (DramAttr->DramCyc));
	PRINT_DEBUG_MEM("Trfc = ");
	PRINT_DEBUG_MEM_HEX16(Tmp);
	PRINT_DEBUG_MEM("\r");
	if (Tmp > MAX_TRFC)
		Tmp = MAX_TRFC;
	else if (Tmp < MIN_TRFC) {
		// return;
		Tmp = 0x40;
	}
	/*D0F3Rx61 bit[0:5] 0->8T ... 63->71T */
	Tmp -= 8;

	Data = pci_read_config8(MEMCTRL, 0x61);
	Data = (u8) ((Data & 0xc0) | ((u8) Tmp & 0x3f));
	pci_write_config8(MEMCTRL, 0x61, Data);
}

/*
Minimum row active to row active delay: Trrd for DDR1/DDR2
D0F3Rx61[7:6]:Trrd  00->2T, 01->3T, 10->4T, 11->5T
*/
#define MAX_TRRD 5
#define MIN_TRRD 2

void SetTrrd(DRAM_SYS_ATTR * DramAttr)
{
	u8 Data;
	u16 Max, Tmp;
	u8 Socket;

	/*get the max Trrd value from SPD data
	   SPD Byte28, Bit7:2->1ns~63ns, Bit1:0->0ns, 0.25ns, 0.50ns, 0.75ns */
	Max = 0;
	for (Socket = 0; Socket < MAX_SOCKETS; Socket++) {
		if (DramAttr->DimmInfo[Socket].bPresence) {
			Tmp =
			    (u16) (DramAttr->
				   DimmInfo[Socket].SPDDataBuf[SPD_SDRAM_TRRD]);
			if (Tmp > Max)
				Max = Tmp;
		}
	}

	/*Calculate clock,this value should be 2T,3T,4T,5T */
	Tmp =
	    (u16) ((Max * 100 + ((DramAttr->DramCyc) << 2) -
		    1) / ((DramAttr->DramCyc) << 2));
	PRINT_DEBUG_MEM("Trrd =");
	PRINT_DEBUG_MEM_HEX16(Tmp);
	PRINT_DEBUG_MEM("\r");

	if (Tmp > MAX_TRRD)
		Tmp = MAX_TRRD;
	else if (Tmp < MIN_TRRD)
		Tmp = MIN_TRRD;
	Tmp -= 2;		//00->2T, 01->3T, 10->4T, 11->5T
	Tmp <<= 6;

	Data = pci_read_config8(MEMCTRL, 0x61);
	Data = (u8) ((Data & 0x3f) | (u8) Tmp);
	pci_write_config8(MEMCTRL, 0x61, Data);
}

/*
Write recovery time: Twr for DDR1/DDR2
Device 0 Function 3:REG63[7:5]:Twr 00->2T 01->3T 10->4T 11->5T
*/
#define MAX_TWR 6
#define MIN_TWR 2

void SetTwr(DRAM_SYS_ATTR * DramAttr)
{
	u8 Data;
	u16 Max, Tmp;
	u8 Socket;

	/*get the max Trtp value from SPD data
	   SPD Byte36, Bit7:2->1ns~63ns, Bit1:0->0ns, 0.25ns, 0.50ns, 0.75ns */
	Max = 0;
	for (Socket = 0; Socket < MAX_SOCKETS; Socket++) {
		if (DramAttr->DimmInfo[Socket].bPresence) {
			Tmp =
			    (u16) (DramAttr->
				   DimmInfo[Socket].SPDDataBuf[SPD_SDRAM_TWR]);
			if (Tmp > Max)
				Max = Tmp;
		}
	}
	/*Calculate clock */
	Tmp = (u16) ((Max * 100 + ((DramAttr->DramCyc) << 2) - 1) / ((DramAttr->DramCyc) << 2));	//this value should be 2T,3T,4T,5T
	PRINT_DEBUG_MEM("Twr = ");
	PRINT_DEBUG_MEM_HEX16(Tmp);
	PRINT_DEBUG_MEM("\r");

	if (Tmp > MAX_TWR)
		Tmp = MAX_TWR;
	else if (Tmp < MIN_TWR)
		Tmp = MIN_TWR;
	Tmp -= 2;		//00->2T, 01->3T, 10->4T, 11->5T
	Tmp <<= 5;

	Data = pci_read_config8(MEMCTRL, 0x63);
	Data = (u8) ((Data & 0x1f) | (u8) Tmp);
	pci_write_config8(MEMCTRL, 0x63, Data);
}

/*
Internal write to read command delay: Twtr for DDR1/DDR2
Device 0 Function 3:REG63[1,0]:Twtr   DDR: 1T or 2T; DDR2 2T or 3T
*/
#define MAX_TWTR 5		//5T
#define MIN_TWTR 2		//2T

void SetTwtr(DRAM_SYS_ATTR * DramAttr)
{
	u8 Data;
	u16 Max, Tmp;
	u8 Socket;

	/*get the max Trtp value from SPD data
	   SPD Byte37, Bit7:2->1ns~63ns, Bit1:0->0ns, 0.25ns, 0.50ns, 0.75ns */
	Max = 0;
	for (Socket = 0; Socket < MAX_SOCKETS; Socket++) {
		if (DramAttr->DimmInfo[Socket].bPresence) {
			Tmp =
			    (u16) (DramAttr->
				   DimmInfo[Socket].SPDDataBuf[SPD_SDRAM_TWTR]);
			if (Tmp > Max)
				Max = Tmp;
		}
	}
	/*Calculate clock */
	Tmp = (u16) ((Max * 100 + ((DramAttr->DramCyc) << 2) - 1) / ((DramAttr->DramCyc) << 2));	//this value should be 2T or 3T

	PRINT_DEBUG_MEM("Twtr =");
	PRINT_DEBUG_MEM_HEX16(Tmp);
	PRINT_DEBUG_MEM("\r");

	if (Tmp > MAX_TWR)
		Tmp = MAX_TWTR;
	else if (Tmp < MIN_TWR)
		Tmp = MIN_TWTR;
	Tmp -= 2;		//00->2T, 01->3T, 10->4T, 11->5T
	Data = pci_read_config8(MEMCTRL, 0x63);
	Data = (u8) ((Data & 0xFC) | Tmp);
	pci_write_config8(MEMCTRL, 0x63, Data);
}

/*
Internal read to precharge command delay, Trtp for DDR1/DDR2
Device 0 Function 3:REG63[3]:Trtp  2T or 3T
*/
#define MAX_TRTP 3		//3T
#define MIN_TRTP 2		//2T

void SetTrtp(DRAM_SYS_ATTR * DramAttr)
{
	u8 Data;
	u16 Max, Tmp;
	u8 Socket;

	/*get the max Trtp value from SPD data
	   SPD Byte38, Bit7:2->1ns~63ns, Bit1:0->0ns, 0.25ns, 0.50ns, 0.75ns */
	Max = 0;
	for (Socket = 0; Socket < MAX_SOCKETS; Socket++) {
		if (DramAttr->DimmInfo[Socket].bPresence) {
			Tmp =
			    (u16) (DramAttr->
				   DimmInfo[Socket].SPDDataBuf[SPD_SDRAM_TRTP]);
			if (Tmp > Max)
				Max = Tmp;
		}
	}
	/*Calculate clock */
	Tmp = (u16) ((Max * 100 + ((DramAttr->DramCyc) << 2) - 1) / ((DramAttr->DramCyc) << 2));	//this value should be 2T or 3T

	PRINT_DEBUG_MEM("Trtp =");
	PRINT_DEBUG_MEM_HEX16(Tmp);
	PRINT_DEBUG_MEM("\r");

	Data = pci_read_config8(MEMCTRL, 0x63);
	if (Tmp > MIN_TRTP)
		Data = (u8) (Data | 0x08);	/*set bit3, set 3T */
	else
		Data = (u8) (Data & 0xf7);	/*clear bit3, set 2T */

	pci_write_config8(MEMCTRL, 0x63, Data);
}
