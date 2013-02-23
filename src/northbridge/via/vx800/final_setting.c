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

static const u8 RefreshCounter[7][2] = {
	//Non_256Mbit, 256Mbit
	{0xCA, 0xA8},		// DRAM400
	{0xCA, 0xA8},		// DRAM333
	{0xCA, 0x86},		// DRAM266
	{0xCA, 0x65},		// DRAM200
	{0xA8, 0x54},		// DRAM166
	{0x86, 0x43},		// DRAM133
	{0x65, 0x32}		// DRAM100
};

void DRAMRefreshCounter(DRAM_SYS_ATTR * DramAttr)
{
	u8 Data;
	u8 Freq = 5, i, Dram_256_Mb;
	if (DramAttr->DramFreq == DIMMFREQ_800)
		Freq = 0;
	else if (DramAttr->DramFreq == DIMMFREQ_667)
		Freq = 1;
	else if (DramAttr->DramFreq == DIMMFREQ_533)
		Freq = 2;
	else if (DramAttr->DramFreq == DIMMFREQ_400)
		Freq = 3;
	else if (DramAttr->DramFreq == DIMMFREQ_333)
		Freq = 4;
	else if (DramAttr->DramFreq == DIMMFREQ_266)
		Freq = 5;
	else if (DramAttr->DramFreq == DIMMFREQ_200)
		Freq = 6;
	else
		Freq = 6;

	Dram_256_Mb = 0;
	for (i = 0; i < MAX_SOCKETS; i++) {
		if (DramAttr->DimmInfo[i].SPDDataBuf[SPD_SDRAM_ROW_ADDR] == 13) {
			Dram_256_Mb = 1;
			break;
		}
	}

	Data = RefreshCounter[Freq][Dram_256_Mb];

	pci_write_config8(MEMCTRL, 0x6a, Data);
}

/*===================================================================
Function   : DRAMRegFinalValue()
Precondition :
Input      :
		   DramAttr:  pointer point to  DRAM_SYS_ATTR  which consist the DDR and Dimm information
		                    in MotherBoard
Output     : Void
Purpose   : Chipset Performance UP and other setting after DRAM Sizing
                 Turn on register directly to promote performance
===================================================================*/

//--------------------------------------------------------------------------
//        register       AND   OR
//--------------------------------------------------------------------------
#define DRAM_table_item		9
static const u8 DRAM_table[DRAM_table_item][3] = {
	{0x60, 0xff, 0xD0},
	{0x66, 0xcf, 0x80},	// DRAMC queue > 2
	{0x69, 0xff, 0x07},	// Enable multiple page
	{0x95, 0x00, 0x0D},
	{0x96, 0x0F, 0xA0},
	{0xFB, 0x00, 0x3E},
	{0xFD, 0x00, 0xA9},
	{0xFE, 0x00, 0x0f},
	{0xFF, 0x00, 0x3D}
};

#define PM_table_item		5
static const u8 PM_table[PM_table_item][3] = {
	{0xA0, 0x0F, 0xF0},
	{0xA1, 0x1F, 0xE0},
	{0xA2, 0x00, 0xFE},
	{0xA3, 0x7F, 0x80},
	{0xA5, 0x7E, 0x81},
};

void DRAMRegFinalValue(DRAM_SYS_ATTR * DramAttr)
{
	u8 Data;
	u8 i;

	for (i = 0; i < DRAM_table_item; i++) {
		Data = pci_read_config8(MEMCTRL, DRAM_table[i][0]);
		Data = (u8) ((Data & DRAM_table[i][1]) | DRAM_table[i][2]);
		pci_write_config8(MEMCTRL, DRAM_table[i][0], Data);
	}

	//enable dram By-Rank self refresh
	Data = pci_read_config8(MEMCTRL, 0x96);
	Data &= 0xF0;
	for (i = 0x01; i < 0x10; i = i << 1) {
		if ((DramAttr->RankPresentMap & i) != 0x00)
			Data |= i;
	}
	pci_write_config8(MEMCTRL, 0x96, Data);

	for (i = 0; i < PM_table_item; i++) {
		Data = pci_read_config8(PCI_DEV(0, 0, 4), PM_table[i][0]);
		Data = (u8) ((Data & PM_table[i][1]) | PM_table[i][2]);
		pci_write_config8(PCI_DEV(0, 0, 4), PM_table[i][0], Data);
	}

}
