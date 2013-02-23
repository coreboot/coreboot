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

void WaitMicroSec(UINTN MicroSeconds)
{
	u32 i;

	for (i = 0; i < 1024 * MicroSeconds; i++) {
		__asm__ volatile ("nop\n\t");
	}

	return;
}

/*===================================================================
Function   : via_write_phys()
Precondition :
Input      :  addr
                  value
Output     : void
Purpose    :
Reference  : None
===================================================================*/

void via_write_phys(volatile u32 addr, volatile u32 value)
{
	volatile u32 *ptr;
	ptr = (volatile u32 *)addr;
	*ptr = (volatile u32)value;
}

/*===================================================================
Function   : via_read_phys()
Precondition :
Input      :  addr
Output     : u32
Purpose    :
Reference  : None
===================================================================*/

u32 via_read_phys(volatile u32 addr)
{
	volatile u32 y;
	y = *(volatile u32 *)addr;
	return y;
}

/*===================================================================
Function   : DimmRead()
Precondition :
Input      :  x
Output     : u32
Purpose    :
Reference  : None
===================================================================*/

u32 DimmRead(volatile u32 x)
{				//  volatile u32 z;
	volatile u32 y;
	y = *(volatile u32 *)x;

	return y;
}

/*===================================================================
Function   : DramBaseTest()
Precondition : this function used to verify memory
Input      :
                 BaseAdd,
                 length,
                 mode
Output     : u32
Purpose    :write into and read out to verify if dram is correct
Reference  : None
===================================================================*/
BOOLEAN DramBaseTest(u32 BaseAdd, u32 Length,
		     DRAM_TEST_MODE Mode, BOOLEAN PrintFlag)
{
	u32 TestSpan;
	u32 Data, Address, Address2;
	u8 i, TestCount;

	//decide the test mode is continous or step
	if (Mode == EXTENSIVE) {
		//the test mode is continuos and must test each unit
		TestSpan = 4;
		TestCount = 1;
	} else if (Mode == SPARE) {
		// the test mode is step and test some unit
		TestSpan = STEPSPAN;
		TestCount = TESTCOUNT;
	} else {
		PRINT_DEBUG_MEM("the test mode is error\r");
		return FALSE;
	}

	//write each test unit the value with TEST_PATTERN
	for (Address = BaseAdd; Address < BaseAdd + Length; Address += TestSpan) {
		for (i = 0; i < TestCount; i++)
			via_write_phys(Address + i * 4, TEST_PATTERN);
		if (PrintFlag) {
			if ((u32) Address % 0x10000000 == 0) {
				PRINT_DEBUG_MEM("Write in Addr =");
				PRINT_DEBUG_MEM_HEX32(Address);
				PRINT_DEBUG_MEM("\r");
			}
		}
	}

	//compare each test unit with the value of TEST_PATTERN
	//and write it with compliment of TEST_PATTERN
	for (Address = BaseAdd; Address < BaseAdd + Length; Address += TestSpan) {
		for (i = 0; i < TestCount; i++) {
			Data = via_read_phys(Address + i * 4);
			via_write_phys(Address + i * 4, (u32) (~TEST_PATTERN));
			if (Data != TEST_PATTERN) {
				PRINT_DEBUG_MEM("TEST_PATTERN ERROR !!!!! ");
				Address2 = Address + i * 4;
				PRINT_DEBUG_MEM_HEX32(Address2);
				PRINT_DEBUG_MEM(" : ");
				PRINT_DEBUG_MEM_HEX32(Data);
				PRINT_DEBUG_MEM(" \r");
				return FALSE;
			}
		}
		if (PrintFlag) {
			if ((u32) Address % 0x10000000 == 0) {
				PRINT_DEBUG_MEM("Write in Addr =");
				PRINT_DEBUG_MEM_HEX32(Address);
				PRINT_DEBUG_MEM("\r");
			}
		}
	}

	//compare each test unit with the value of ~TEST_PATTERN
	for (Address = BaseAdd; Address < BaseAdd + Length; Address += TestSpan) {
		for (i = (u8) (TestCount); i > 0; i--) {
			Data = via_read_phys(Address + (i - 1) * 4);
			if (Data != ~TEST_PATTERN) {

				PRINT_DEBUG_MEM("~TEST_PATTERN ERROR !!!!! ");
				Address2 = Address + (i - 1) * 4;
				PRINT_DEBUG_MEM_HEX32(Address2);
				PRINT_DEBUG_MEM(" : ");
				PRINT_DEBUG_MEM_HEX32(Data);
				PRINT_DEBUG_MEM(" \r");
				return FALSE;
			}
		}
	}

	return TRUE;
}

/*===================================================================
Function   : DumpRegisters()
Precondition :
Input      :
                pPCIPPI,
                DevNum,
                FuncNum
Output     : Void
Purpose    :
Reference  : None
===================================================================*/

void DumpRegisters(INTN DevNum, INTN FuncNum)
{
	INTN i, j;
	u8 ByteVal;

	ByteVal = 0;
	//pci_write_config8(PCI_DEV(0, DevNum, FuncNum), 0xA1, ByteVal);
	PRINT_DEBUG_MEM("\rDev %02x Fun %02x\r");
	PRINT_DEBUG_MEM
	    ("\r    00 01 02 03 04 05 06 07 08 09 0a 0b 0c 0d 0e 0f\r");
	PRINT_DEBUG_MEM
	    ("---------------------------------------------------\r");
	for (i = 0; i < 0x10; i++) {
		PRINT_DEBUG_MEM_HEX32((u32)i);
		for (j = 0; j < 0x10; j++) {
			ByteVal =
			    pci_read_config8(PCI_DEV(0, DevNum, FuncNum),
					     i * 0x10 + j);
			PRINT_DEBUG_MEM_HEX8(ByteVal);
			PRINT_DEBUG_MEM(" ");

		}
		PRINT_DEBUG_MEM("\r");
	}
	return;
}

/*===================================================================
Function   : dumpnorth()
Precondition :
Input      :
                pPCIPPI,
                Func
Output     : Void
Purpose    :
Reference  : None
===================================================================*/

void dumpnorth(u8 Func)
{
	u16 r, c;
	u8 ByteVal;
	PRINT_DEBUG_MEM("Dump North!!!\r");
	for (r = 0; r < 32; r++) {
		for (c = (u16) (r << 3); c < (r << 3) + 8; c++) {
			ByteVal = 0;
			ByteVal = pci_read_config8(PCI_DEV(0, 0, Func), c);
			PRINT_DEBUG_MEM_HEX16(c);
			PRINT_DEBUG_MEM("= ");
			PRINT_DEBUG_MEM_HEX8(ByteVal);
		}
		PRINT_DEBUG_MEM("\r");
	}
}
