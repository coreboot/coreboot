/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2002 Linux Networx
 * (Written by Eric Biederman <ebiederman@lnxi.com> for Linux Networx)
 * Copyright (C) 2004 YingHai Lu
 * Copyright (C) 2007 Ronald G. Minnich <rminnich@gmail.com>
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
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA, 02110-1301 USA
 */
/*
	2004.11 yhlu add 4 rank DIMM support
	2004.12 yhlu add D0 support
	2005.02 yhlu add E0 memory hole support
	2005.10 yhlu make it support DDR2 only
*/

#include <mainboard.h>
#include <console.h>
#include <string.h>
#include <mtrr.h>
#include <macros.h>
#include <spd_ddr2.h>
#include <cpu.h>
#include <msr.h>
#include <amd/k8/k8.h>
#include <amd/k8/sysconf.h>
#include <device/pci.h>
#include <pci_ops.h>
#include <mc146818rtc.h>
#include <lib.h>

#include <spd_ddr2.h>

#warning where to we define supported DIMM types
#define DIMM_SUPPORT 0x0104

/* NOTE: F2 and later ONLY */
inline void print_raminit(const char *strval, u32 val)
{
        printk(BIOS_DEBUG, "%s%08x\n", strval, val);
}

#if 0
#if defined(CONFIG_LB_MEM_TOPK) & ((CONFIG_LB_MEM_TOPK -1) != 0)
# error "CONFIG_LB_MEM_TOPK must be a power of 2"
#endif
#endif

/* bit [10,8] are dev func, bit[1,0] are dev index */
u32 pci_read_config32_index(u32 dev, u32 index_reg, u32 index)
{
        u32 dword;

        pci_conf1_write_config32(dev, index_reg, index);

        dword = pci_conf1_read_config32(dev, index_reg+0x4);

        return dword;
}

void pci_write_config32_index(u32 dev, u32 index_reg, u32 index, u32 data)
{

        pci_conf1_write_config32(dev, index_reg, index);

        pci_conf1_write_config32(dev, index_reg + 0x4, data);

}

u32 pci_read_config32_index_wait(u32 dev, u32 index_reg, u32 index)
{

        u32 dword;

        index &= ~(1<<30);
        pci_conf1_write_config32(dev, index_reg, index);

        do {
                dword = pci_conf1_read_config32(dev, index_reg);
        } while (!(dword & (1<<31)));

        dword = pci_conf1_read_config32(dev, index_reg+0x4);

        return dword;
}

void pci_write_config32_index_wait(u32 dev, u32 index_reg, u32 index, u32 data)
{

        u32 dword;

        pci_conf1_write_config32(dev, index_reg + 0x4, data);

        index |= (1<<30);
        pci_conf1_write_config32(dev, index_reg, index);
        do {
                dword = pci_conf1_read_config32(dev, index_reg);
        } while (!(dword & (1<<31)));

}



	// for PCM(0, 0x18, 2, 0x98) index, and PCM(0x, 0x18, 2, 0x9c) data
	/*
		index: 
		[29: 0] DctOffset (Dram Controller Offset)
		[30:30] DctAccessWrite (Dram Controller Read/Write Select)	
			0 = read access
			1 = write access
		[31:31] DctAccessDone (Dram Controller Access Done)
			0 = Access in progress
			1 = No access is progress

		Data:
		[31: 0] DctOffsetData (Dram Controller Offset Data)
		
		Read:
			- Write the register num to DctOffset with DctAccessWrite = 0
			- poll the DctAccessDone until it = 1
			- Read the data from DctOffsetData
		Write:
			- Write the data to DctOffsetData
			- Write register num to DctOffset with DctAccessWrite = 1
			- poll the DctAccessDone untio it = 1
		
	*/

int controller_present(const struct mem_controller *ctrl)
{
        return pci_conf1_read_config32(ctrl->f0, 0) == 0x11001022;
}

void sdram_set_registers(const struct mem_controller *ctrl, struct sys_info *sysinfo)
{
	static const struct rmap register_values[] = {

	/* Careful set limit registers before base registers which contain the enables */
	/* DRAM Limit i Registers
	 * F1:0x44 i = 0
	 * F1:0x4C i = 1
	 * F1:0x54 i = 2
	 * F1:0x5C i = 3
	 * F1:0x64 i = 4
	 * F1:0x6C i = 5
	 * F1:0x74 i = 6
	 * F1:0x7C i = 7
	 * [ 2: 0] Destination Node ID
	 *	   000 = Node 0
	 *	   001 = Node 1
	 *	   010 = Node 2
	 *	   011 = Node 3
	 *	   100 = Node 4
	 *	   101 = Node 5
	 *	   110 = Node 6
	 *	   111 = Node 7
	 * [ 7: 3] Reserved
	 * [10: 8] Interleave select
	 *	   specifies the values of A[14:12] to use with interleave enable.
	 * [15:11] Reserved
	 * [31:16] DRAM Limit Address i Bits 39-24
	 *	   This field defines the upper address bits of a 40 bit  address
	 *	   that define the end of the DRAM region.
	 */
	PCM(0, 0x18, 1, 0x44, 0x0000f8f8, 0x00000000),
	PCM(0, 0x18, 1, 0x4C, 0x0000f8f8, 0x00000001),
	PCM(0, 0x18, 1, 0x54, 0x0000f8f8, 0x00000002),
	PCM(0, 0x18, 1, 0x5C, 0x0000f8f8, 0x00000003),
	PCM(0, 0x18, 1, 0x64, 0x0000f8f8, 0x00000004),
	PCM(0, 0x18, 1, 0x6C, 0x0000f8f8, 0x00000005),
	PCM(0, 0x18, 1, 0x74, 0x0000f8f8, 0x00000006),
	PCM(0, 0x18, 1, 0x7C, 0x0000f8f8, 0x00000007),
	/* DRAM Base i Registers
	 * F1:0x40 i = 0
	 * F1:0x48 i = 1
	 * F1:0x50 i = 2
	 * F1:0x58 i = 3
	 * F1:0x60 i = 4
	 * F1:0x68 i = 5
	 * F1:0x70 i = 6
	 * F1:0x78 i = 7
	 * [ 0: 0] Read Enable
	 *	   0 = Reads Disabled
	 *	   1 = Reads Enabled
	 * [ 1: 1] Write Enable
	 *	   0 = Writes Disabled
	 *	   1 = Writes Enabled
	 * [ 7: 2] Reserved
	 * [10: 8] Interleave Enable
	 *	   000 = No interleave
	 *	   001 = Interleave on A[12] (2 nodes)
	 *	   010 = reserved
	 *	   011 = Interleave on A[12] and A[14] (4 nodes)
	 *	   100 = reserved
	 *	   101 = reserved
	 *	   110 = reserved
	 *	   111 = Interleve on A[12] and A[13] and A[14] (8 nodes)
	 * [15:11] Reserved
	 * [13:16] DRAM Base Address i Bits 39-24
	 *	   This field defines the upper address bits of a 40-bit address
	 *	   that define the start of the DRAM region.
	 */
	PCM(0, 0x18, 1, 0x40, 0x0000f8fc, 0x00000000),
	PCM(0, 0x18, 1, 0x48, 0x0000f8fc, 0x00000000),
	PCM(0, 0x18, 1, 0x50, 0x0000f8fc, 0x00000000),
	PCM(0, 0x18, 1, 0x58, 0x0000f8fc, 0x00000000),
	PCM(0, 0x18, 1, 0x60, 0x0000f8fc, 0x00000000),
	PCM(0, 0x18, 1, 0x68, 0x0000f8fc, 0x00000000),
	PCM(0, 0x18, 1, 0x70, 0x0000f8fc, 0x00000000),
	PCM(0, 0x18, 1, 0x78, 0x0000f8fc, 0x00000000),

	/* DRAM CS Base Address i Registers
	 * F2:0x40 i = 0
	 * F2:0x44 i = 1
	 * F2:0x48 i = 2
	 * F2:0x4C i = 3
	 * F2:0x50 i = 4
	 * F2:0x54 i = 5
	 * F2:0x58 i = 6
	 * F2:0x5C i = 7
	 * [ 0: 0] Chip-Select Bank Enable
	 *	   0 = Bank Disabled
	 *	   1 = Bank Enabled
	 * [ 1: 1] Spare Rank
	 * [ 2: 2] Memory Test Failed
	 * [ 4: 3] Reserved
	 * [13: 5] Base Address (21-13)
	 *	   An optimization used when all DIMM are the same size...
	 * [18:14] Reserved
	 * [28:19] Base Address (36-27)
	 *	   This field defines the top 11 addresses bit of a 40-bit
	 *	   address that define the memory address space.  These
	 *	   bits decode 32-MByte blocks of memory.
	 * [31:29] Reserved
	 */
	PCM(0, 0x18, 2, 0x40, 0xe007c018, 0x00000000),
	PCM(0, 0x18, 2, 0x44, 0xe007c018, 0x00000000),
	PCM(0, 0x18, 2, 0x48, 0xe007c018, 0x00000000),
	PCM(0, 0x18, 2, 0x4C, 0xe007c018, 0x00000000),
	PCM(0, 0x18, 2, 0x50, 0xe007c018, 0x00000000),
	PCM(0, 0x18, 2, 0x54, 0xe007c018, 0x00000000),
	PCM(0, 0x18, 2, 0x58, 0xe007c018, 0x00000000),
	PCM(0, 0x18, 2, 0x5C, 0xe007c018, 0x00000000),
	/* DRAM CS Mask Address i Registers
	 * F2:0x60 i = 0,1
	 * F2:0x64 i = 2,3
	 * F2:0x68 i = 4,5
	 * F2:0x6C i = 6,7
	 * Select bits to exclude from comparison with the DRAM Base address register.
	 * [ 4: 0] Reserved
	 * [13: 5] Address Mask (21-13)
	 *	   Address to be excluded from the optimized case
	 * [18:14] Reserved
	 * [28:19] Address Mask (36-27)
	 *	   The bits with an address mask of 1 are excluded from address comparison
	 * [31:29] Reserved
	 * 
	 */
	PCM(0, 0x18, 2, 0x60, 0xe007c01f, 0x00000000),
	PCM(0, 0x18, 2, 0x64, 0xe007c01f, 0x00000000),
	PCM(0, 0x18, 2, 0x68, 0xe007c01f, 0x00000000),
	PCM(0, 0x18, 2, 0x6C, 0xe007c01f, 0x00000000),

        /* DRAM Control Register
         * F2:0x78
         * [ 3: 0] RdPtrInit ( Read Pointer Initial Value)
	 * 	0x03-0x00: reserved
	 * [ 6: 4] RdPadRcvFifoDly (Read Delay from Pad Receive FIFO)
	 *	000 = reserved
	 *	001 = reserved
	 *      010 = 1.5 Memory Clocks
	 *	011 = 2 Memory Clocks
	 *	100 = 2.5 Memory Clocks
	 *	101 = 3 Memory Clocks
	 *	110 = 3.5 Memory Clocks
	 *	111 = Reseved
         * [15: 7] Reserved
         * [16:16] AltVidC3MemClkTriEn (AltVID Memory Clock Tristate Enable)
	 *	Enables the DDR memory clocks to be tristated when alternate VID mode is enabled. This bit has no effect if the DisNbClkRamp bit (F3, 0x88) is set
	 * [17:17] DllTempAdjTime (DLL Temperature Adjust Cycle Time)
	 *	0 = 5 ms
	 *	1 = 1 ms
	 * [18:18] DqsRcvEnTrain (DQS Receiver Enable Training Mode)
	 *	0 = Normal DQS Receiver enable operation
	 *	1 = DQS receiver enable training mode
         * [31:19] reverved
        */
        PCM(0, 0x18, 2, 0x78, 0xfff80000, (6<<4)|(6<<0)),

        /* DRAM Initialization Register
         * F2:0x7C
         * [15: 0] MrsAddress (Address for MRS/EMRS Commands)
         *      this field specifies the dsata driven on the DRAM address pins 15-0 for MRS and EMRS commands
         * [18:16] MrsBank (Bank Address for MRS/EMRS Commands)
	 *	this files specifies the data driven on the DRAM bank pins for the MRS and EMRS commands
	 * [23:19] reverved
         * [24:24] SendPchgAll (Send Precharge All Command)
	 *	Setting this bit causes the DRAM controller to send a precharge all command. This bit is cleared by the hardware after the command completes
	 * [25:25] SendAutoRefresh (Send Auto Refresh Command)
	 *	Setting this bit causes the DRAM controller to send an auto refresh command. This bit is cleared by the hardware after the command completes
	 * [26:26] SendMrsCmd (Send MRS/EMRS Command)
	 *	Setting this bit causes the DRAM controller to send the MRS or EMRS command defined by the MrsAddress and MrsBank fields. This bit is cleared by the hardware adter the commmand completes
	 * [27:27] DeassertMemRstX (De-assert Memory Reset)
	 * 	Setting this bit causes the DRAM controller to de-assert the memory reset pin. This bit cannot be used to assert the memory reset pin
	 * [28:28] AssertCke (Assert CKE)
	 *	setting this bit causes the DRAM controller to assert the CKE pins. This bit cannot be used to de-assert the CKE pins
	 * [30:29] reverved
	 * [31:31] EnDramInit (Enable DRAM Initialization)
	 *	Setting this bit puts the DRAM controller in a BIOS controlled DRAM initialization mode. BIOS must clear this bit aster DRAM initialization is complete.
        */
//        PCM(0, 0x18, 2, 0x7C), 0x60f80000, 0, 


	/* DRAM Bank Address Mapping Register
	 * F2:0x80
	 * Specify the memory module size
	 * [ 3: 0] CS1/0 
	 * [ 7: 4] CS3/2
	 * [11: 8] CS5/4
	 * [15:12] CS7/6
	 * [31:16]
	      row    col   bank
	  0:  13     9      2    :128M
	  1:  13     10     2    :256M
	  2:  14     10     2    :512M
	  3:  13     11     2    :512M
	  4:  13     10     3    :512M
	  5:  14     10     3    :1G
	  6:  14     11     2    :1G
	  7:  15     10     3    :2G
	  8:  14     11     3    :2G
	  9:  15     11     3    :4G
	 10:  16     10     3    :4G
	 11:  16     11     3    :8G
	*/
	PCM(0, 0x18, 2, 0x80, 0xffff0000, 0x00000000),
	/* DRAM Timing Low Register
	 * F2:0x88
	 * [ 2: 0] Tcl (Cas# Latency, Cas# to read-data-valid)
	 *	   000 = reserved
	 *	   001 = reserved
	 *	   010 = CL 3
	 *	   011 = CL 4
	 *	   100 = CL 5
	 *	   101 = CL 6
	 *	   110 = reserved
	 *	   111 = reserved
	 * [ 3: 3] Reserved
	 * [ 5: 4] Trcd (Ras#-active to Cas# read/write delay)
	 *	   00 = 3 clocks
	 *	   01 = 4 clocks
	 *	   10 = 5 clocks
	 *	   11 = 6 clocks
         * [ 7: 6] Reserved
	 * [ 9: 8] Trp (Row Precharge Time, Precharge-to-Active or Auto-Refresh)
	 *	   00 = 3 clocks
	 *	   01 = 4 clocks
	 *	   10 = 5 clocks
	 *	   11 = 6 clocks
	 * [10:10] Reserved
	 * [11:11] Trtp (Read to Precharge Time, read Cas# to precharge time)
	 *	   0 = 2 clocks for Burst Length of 32 Bytes
	 *             4 clocks for Burst Length of 64 Bytes
         *         1 = 3 clocks for Burst Length of 32 Bytes
         *             5 clocks for Burst Length of 64 Bytes
	 * [15:12] Tras (Minimum Ras# Active Time)
         *         0000 = reserved
	 *	   0001 = reserved
         *         0010 = 5 bus clocks
         *         ...
         *         1111 = 18 bus clocks
	 * [19:16] Trc (Row Cycle Time, Ras#-active to Ras#-active or auto refresh of the same bank)
	 *	   0000 = 11 bus clocks
	 *	   0010 = 12 bus clocks
         *	   ...
	 *	   1110 = 25 bus clocks
	 *	   1111 = 26 bus clocks
	 * [21:20] Twr (Write Recovery Time, From the last data to precharge, writes can go back-to-back) 
         *         00 = 3 bus clocks
         *         01 = 4 bus clocks
         *         10 = 5 bus clocks
         *         11 = 6 bus clocks
         * [23:22] Trrd (Active-to-active (Ras#-to-Ras#) Delay of different banks)
         *         00 = 2 bus clocks
         *         01 = 3 bus clocks
         *         10 = 4 bus clocks
         *         11 = 5 bus clocks
	 * [31:24] MemClkDis ( Disable the MEMCLK outputs for DRAM channel A, BIOS should set it to reduce the power consumption)
         *         Bit		F(1207)		M2 Package	S1g1 Package
	 *          0           N/A             MA1_CLK1	N/A
	 *	    1		N/A		MA0_CLK1	MA0_CLK1
         *	    2		MA3_CLK		N/A		N/A
	 *	    3		MA2_CLK		N/A		N/A
	 *	    4		MA1_CLK		MA1_CLK0	N/A
	 *	    5		MA0_CLK		MA0_CLK0	MA0_CLK0
	 *	    6		N/A		MA1_CLK2	N/A
	 *	    7		N/A		MA0_CLK2	MA0_CLK2
	 */
	PCM(0, 0x18, 2, 0x88, 0x000004c8, 0xff000002 /* 0x03623125 */ ),
	/* DRAM Timing High Register
	 * F2:0x8C
	 * [ 3: 0] Reserved
	 * [ 6: 4] TrwtTO (Read-to-Write Turnaround for Data, DQS Contention)
	 *	   000 = 2 bus clocks
	 *	   001 = 3 bus clocks
	 *	   010 = 4 bus clocks
	 *	   011 = 5 bus clocks
	 *	   100 = 6 bus clocks
	 *	   101 = 7 bus clocks
	 *	   110 = 8 bus clocks
	 *	   111 = 9 bus clocks
	 * [ 7: 7] Reserved
	 * [ 9: 8] Twtr (Internal DRAM Write-to-Read Command Delay, minium write-to-read delay when both access the same chip select)
         *         00 = Reserved
         *         01 = 1 bus clocks
         *         10 = 2 bus clocks
         *         11 = 3 bus clocks
	 * [11:10] Twrrd (Write to Read DIMM Termination Turnaround, minimum write-to-read delay when accessing two different DIMMs)
         *         00 = 0 bus clocks
         *         01 = 1 bus clocks
         *         10 = 2 bus clocks
         *         11 = 3 bus clocks
         * [13:12] Twrwr (Write to Write Timing)
         *         00 = 1 bus clocks ( 0 idle cycle on the bus)
         *         01 = 2 bus clocks ( 1 idle cycle on the bus)
         *         10 = 3 bus clocks ( 2 idle cycles on the bus)
         *         11 = Reserved
         * [15:14] Trdrd ( Read to Read Timing)
         *         00 = 2 bus clocks ( 1 idle cycle on the bus)
         *         01 = 3 bus clocks ( 2 idle cycles on the bus)
         *         10 = 4 bus clocks ( 3 idle cycles on the bus)
         *         11 = 5 bus clocks ( 4 idel cycles on the bus)
         * [17:16] Tref (Refresh Rate)
         *         00 = Undefined behavior
         *         01 = Reserved
         *         10 = Refresh interval of 7.8 microseconds
         *         11 = Refresh interval of 3.9 microseconds
	 * [19:18] Reserved
	 * [22:20] Trfc0 ( Auto-Refresh Row Cycle Time for the Logical DIMM0, based on DRAM density and speed)
         *         000 = 75 ns (all speeds, 256Mbit)
         *         001 = 105 ns (all speeds, 512Mbit)
         *         010 = 127.5 ns (all speeds, 1Gbit)
         *         011 = 195 ns (all speeds, 2Gbit)
         *         100 = 327.5 ns (all speeds, 4Gbit)
         *         101 = reserved
         *         110 = reserved
         *         111 = reserved
	 * [25:23] Trfc1 ( Auto-Refresh Row Cycle Time for the Logical DIMM1, based on DRAM density and speed)
	 * [28:26] Trfc2 ( Auto-Refresh Row Cycle Time for the Logical DIMM2, based on DRAM density and speed)
	 * [31:29] Trfc3 ( Auto-Refresh Row Cycle Time for the Logical DIMM3, based on DRAM density and speed)
	 */
	PCM(0, 0x18, 2, 0x8c, 0x000c008f, (2 << 16)|(1 << 8)),
	/* DRAM Config Low Register
	 * F2:0x90
	 * [ 0: 0] InitDram (Initialize DRAM)
	 *	   1 = write 1 cause DRAM controller to execute the DRAM initialization, when done it read to 0
	 * [ 1: 1] ExitSelfRef ( Exit Self Refresh Command )
	 *	   1 = write 1 causes the DRAM controller to bring the DRAMs out fo self refresh mode
	 * [ 3: 2] Reserved
	 * [ 5: 4] DramTerm (DRAM Termination)
         *         00 = On die termination disabled
         *         01 = 75 ohms
         *         10 = 150 ohms
         *         11 = 50 ohms
	 * [ 6: 6] Reserved
	 * [ 7: 7] DramDrvWeak ( DRAM Drivers Weak Mode)
	 *	   0 = Normal drive strength mode.
	 *	   1 = Weak drive strength mode
	 * [ 8: 8] ParEn (Parity Enable)
	 *	   1 = Enable address parity computation output, PAR, and enables the parity error input, ERR
         * [ 9: 9] SelfRefRateEn (Faster Self Refresh Rate Enable)
         *         1 = Enable high temperature ( two times normal ) self refresh rate
	 * [10:10] BurstLength32 ( DRAM Burst Length Set for 32 Bytes)
	 *	   0 = 64-byte mode
	 *	   1 = 32-byte mode
	 * [11:11] Width128 ( Width of DRAM interface) 
	 *	   0 = the controller DRAM interface is 64-bits wide
	 *	   1 = the controller DRAM interface is 128-bits wide
         * [12:12] X4Dimm (DIMM 0 is x4)
         * [13:13] X4Dimm (DIMM 1 is x4)
         * [14:14] X4Dimm (DIMM 2 is x4)
         * [15:15] X4Dimm (DIMM 3 is x4)
         *         0 = DIMM is not x4
         *         1 = x4 DIMM present
	 * [16:16] UnBuffDimm ( Unbuffered DIMMs)
	 *	   0 = Buffered DIMMs
	 *	   1 = Unbuffered DIMMs
	 * [18:17] Reserved
	 * [19:19] DimmEccEn ( DIMM ECC Enable )
		   1 =  ECC checking is being enabled for all DIMMs on the DRAM controller ( Through F3 0x44[EccEn])
	 * [31:20] Reserved
	 */
	PCM(0, 0x18, 2, 0x90, 0xfff6004c, 0x00000010),
	/* DRAM Config High Register
	 * F2:0x94
	 * [ 0: 2] MemClkFreq ( Memory Clock Frequency)
	 *	   000 = 200MHz
	 *	   001 = 266MHz
	 *	   010 = 333MHz
         *	   011 = reserved
	 *	   1xx = reserved
	 * [ 3: 3] MemClkFreqVal (Memory Clock Freqency Valid)
	 *	   1 = BIOS need to set the bit when setting up MemClkFreq to the proper value  
	 * [ 7: 4] MaxAsyncLat ( Maximum Asynchronous Latency)
         *         0000 = 0 ns
         *         ...
         *         1111 = 15 ns
         * [11: 8] Reserved
         * [12:12] RDqsEn ( Read DQS Enable) This bit is only be set if x8 registered DIMMs are present in the system
         *         0 = DM pins function as data mask pins
         *         1 = DM pins function as read DQS pins
         * [13:13] Reserved
         * [14:14] DisDramInterface ( Disable the DRAM interface ) When this bit is set, the DRAM controller is disabled, and interface in low power state
         *         0 = Enabled (default)
         *         1 = Disabled
         * [15:15] PowerDownEn ( Power Down Mode Enable ) 
         *         0 = Disabled (default)
         *         1 = Enabled
         * [16:16] PowerDown ( Power Down Mode )
         *         0 = Channel CKE Control
         *         1 = Chip Select CKE Control
         * [17:17] FourRankSODimm (Four Rank SO-DIMM) 
         *         1 = this bit is set by BIOS to indicate that a four rank SO-DIMM is present
         * [18:18] FourRankRDimm (Four Rank Registered DIMM)
         *         1 = this bit is set by BIOS to indicate that a four rank registered DIMM is present
	 * [19:19] Reserved
         * [20:20] SlowAccessMode (Slow Access Mode (2T Mode))
         *         0 = DRAM address and control signals are driven for one MEMCLK cycle
         *         1 = One additional MEMCLK of setup time is provided on all DRAM address and control signals except CS, CKE, and ODT; i.e., these signals are drivern for two MEMCLK cycles rather than one
 	 * [21:21] Reserved
         * [22:22] BankSwizzleMode ( Bank Swizzle Mode), 
         *         0 = Disabled (default)
         *         1 = Enabled
         * [23:23] Reserved
	 * [27:24] DcqBypassMax ( DRAM Controller Queue Bypass Maximum)
	 *	   0000 = No bypass; the oldest request is never bypassed
	 *	   0001 = The oldest request may be bypassed no more than 1 time
	 *	   ...
	 *	   1111 = The oldest request may be bypassed no more than 15 times
         * [31:28] FourActWindow ( Four Bank Activate Window) , not more than 4 banks in a 8 bank device are activated
         *         0000 = No tFAW window restriction 
         *         0001 = 8 MEMCLK cycles
         *	   0010 = 9 MEMCLK cycles
         *         ...
         *         1101 = 20 MEMCLK cycles
	 *	   111x = reserved
	 */
	PCM(0, 0x18, 2, 0x94, 0x00a82f00,0x00008000),
	/* DRAM Delay Line Register
	 * F2:0xa0
	 * [ 0: 0] MemClrStatus (Memory Clear Status) :    ---------Readonly 
	 *	   when set, this bit indicates that the memory clear function is complete. Only clear by reset. BIOS should not write or read the DRAM until this bit is set by hardware
	 * [ 1: 1] DisableJitter ( Disable Jitter)
	 *	   When set the DDR compensation circuit will not change the values unless the change is more than one step from the current value 
	 * [ 3: 2] RdWrQByp ( Read/Write Queue Bypass Count)
	 *	   00 = 2
	 *	   01 = 4
	 *	   10 = 8
	 *	   11 = 16
	 * [ 4: 4] Mode64BitMux (Mismatched DIMM Support Enable)
	 *	   1 When bit enables support for mismatched DIMMs when using 128-bit DRAM interface, the Width128 no effect, only for M2 and s1g1
	 * [ 5: 5] DCC_EN ( Dynamica Idle Cycle Counter Enable)
	 *	   When set to 1, indicates that each entry in the page tables dynamically adjusts the idle cycle limit based on page Conflict/Page Miss (PC/PM) traffic
	 * [ 8: 6] ILD_lmt ( Idle Cycle Limit)
	 *	   000 = 0 cycles
	 *	   001 = 4 cycles
	 *	   010 = 8 cycles
	 *	   011 = 16 cycles
	 *	   100 = 32 cycles
	 * 	   101 = 64 cycles
	 *	   110 = 128 cycles
	 *	   111 = 256 cycles 
	 * [ 9: 9] DramEnabled ( DRAM Enabled)
	 *	   When Set, this bit indicates that the DRAM is enabled, this bit is set by hardware after DRAM initialization or on an exit from self refresh. The DRAM controller is intialized after the
	 *	   hardware-controlled initialization process ( initiated by the F2 0x90[DramInit]) completes or when the BIOS-controlled initialization process completes (F2 0x7c(EnDramInit] is 
	 *	   written from 1 to 0)
	 * [23:10] Reserved
         * [31:24] MemClkDis ( Disable the MEMCLK outputs for DRAM channel B, BIOS should set it to reduce the power consumption)
         *         Bit          F(1207)         M2 Package      S1g1 Package
         *          0           N/A             MA1_CLK1        N/A
         *          1           N/A             MA0_CLK1        MA0_CLK1
         *          2           MA3_CLK         N/A             N/A
         *          3           MA2_CLK         N/A             N/A
         *          4           MA1_CLK         MA1_CLK0        N/A
         *          5           MA0_CLK         MA0_CLK0        MA0_CLK0
         *          6           N/A             MA1_CLK2        N/A
         *          7           N/A             MA0_CLK2        MA0_CLK2
	 */
	PCM(0, 0x18, 2, 0xa0, 0x00fffc00, 0xff000000), 

	/* DRAM Scrub Control Register
	 * F3:0x58
	 * [ 4: 0] DRAM Scrube Rate
	 * [ 7: 5] reserved
	 * [12: 8] L2 Scrub Rate
	 * [15:13] reserved
	 * [20:16] Dcache Scrub
	 * [31:21] reserved
	 *	   Scrub Rates
	 *	   00000 = Do not scrub
	 *	   00001 =  40.00 ns
	 *	   00010 =  80.00 ns
	 *	   00011 = 160.00 ns
	 *	   00100 = 320.00 ns
	 *	   00101 = 640.00 ns
	 *	   00110 =   1.28 us
	 *	   00111 =   2.56 us
	 *	   01000 =   5.12 us
	 *	   01001 =  10.20 us
	 *	   01011 =  41.00 us
	 *	   01100 =  81.90 us
	 *	   01101 = 163.80 us
	 *	   01110 = 327.70 us
	 *	   01111 = 655.40 us
	 *	   10000 =   1.31 ms
	 *	   10001 =   2.62 ms
	 *	   10010 =   5.24 ms
	 *	   10011 =  10.49 ms
	 *	   10100 =  20.97 ms
	 *	   10101 =  42.00 ms
	 *	   10110 =  84.00 ms
	 *	   All Others = Reserved
	 */
	PCM(0, 0x18, 3, 0x58, 0xffe0e0e0, 0x00000000),
	/* DRAM Scrub Address Low Register
	 * F3:0x5C
	 * [ 0: 0] DRAM Scrubber Redirect Enable
	 *	   0 = Do nothing
	 *	   1 = Scrubber Corrects errors found in normal operation
	 * [ 5: 1] Reserved
	 * [31: 6] DRAM Scrub Address 31-6
	 */
	PCM(0, 0x18, 3, 0x5C, 0x0000003e, 0x00000000),
	/* DRAM Scrub Address High Register
	 * F3:0x60
	 * [ 7: 0] DRAM Scrubb Address 39-32
	 * [31: 8] Reserved
	 */
	PCM(0, 0x18, 3, 0x60, 0xffffff00, 0x00000000),
	};
	// for PCM(0, 0x18, 2, 0x98) index, and PCM(0x, 0x18, 2, 0x9c) data
	/*
		index: 
		[29: 0] DctOffset (Dram Controller Offset)
		[30:30] DctAccessWrite (Dram Controller Read/Write Select)	
			0 = read access
			1 = write access
		[31:31] DctAccessDone (Dram Controller Access Done)
			0 = Access in progress
			1 = No access is progress

		Data:
		[31: 0] DctOffsetData (Dram Controller Offset Data)
		
		Read:
			- Write the register num to DctOffset with DctAccessWrite = 0
			- poll the DctAccessDone until it = 1
			- Read the data from DctOffsetData
		Write:
			- Write the data to DctOffsetData
			- Write register num to DctOffset with DctAccessWrite = 1
			- poll the DctAccessDone untio it = 1
		
	*/
	int i;
	int max;

        if (!controller_present(ctrl)) {
		sysinfo->ctrl_present[ctrl->node_id] = 0;
                return;
        }
	sysinfo->ctrl_present[ctrl->node_id] = 1;
	
	printk(BIOS_SPEW, "setting up CPU 0x%02x northbridge registers\n", ctrl->node_id);
	/* do this later when we're sure it actually works */
	//setup_resource_map_x_offset(register_values, ARRAY_SIZE(register_values));
	max = ARRAY_SIZE(register_values);
	for(i = 0; i < max; i++) {
		struct rmap const *rm = &register_values[i];
		u32 dev;
		unsigned where;
		unsigned long reg;
		dev = PCI_BDF(rm->pcm.bus, rm->pcm.dev, rm->pcm.fn);
		dev -= PCI_BDF(0, 0x18, 0);
		dev += ctrl->f0;
		//dev = (register_values[i] & ~0xff) - PCI_BDF(0, 0x18, 0) + ctrl->f0;
		where = rm->pcm.reg;
		reg = pci_conf1_read_config32(dev, where);
		reg &= rm->pcm.and;
		reg |= rm->pcm.or;
		pci_conf1_write_config32(dev, where, reg);
	}

	printk(BIOS_SPEW, "done.\n");
}

int is_dual_channel(const struct mem_controller *ctrl)
{
	u32 dcl;
	dcl = pci_conf1_read_config32(ctrl->f2, DRAM_CONFIG_LOW);
	return dcl & DCL_Width128;
}

/* the very existence of this function is puzzling. How did we get here otherwise? */
int is_opteron(const struct mem_controller *ctrl)
{
	/* Test to see if I am an Opteron.  
	 * FIXME Testing dual channel capability is correct for now
	 * but a better test is probably required.
	 * m2 and s1g1 support dual channel too. but only support unbuffered dimm
	 */
#warning "FIXME implement a better test for opterons"
	u32 nbcap;
	nbcap = pci_conf1_read_config32(ctrl->f3, NORTHBRIDGE_CAP);
	return !!(nbcap & NBCAP_128Bit);
}

int is_registered(const struct mem_controller *ctrl)
{
	/* Test to see if we are dealing with registered SDRAM.
	 * If we are not registered we are unbuffered.
	 * This function must be called after spd_handle_unbuffered_dimms.
	 */
	u32 dcl;
	dcl = pci_conf1_read_config32(ctrl->f2, DRAM_CONFIG_LOW);
	return !(dcl & DCL_UnBuffDimm);
}

void spd_get_dimm_size(unsigned device, struct dimm_size *sz)
{
	/* Calculate the log base 2 size of a DIMM in bits */
	int value;
	sz->per_rank = 0;
	sz->rows = 0;
	sz->col = 0;
	sz->rank = 0;

	value = spd_read_byte(device, SPD_ROW_NUM);	/* rows */
	if (value < 0) goto hw_err;
	if ((value & 0xff) == 0) goto val_err; // max is 16 ?
	sz->per_rank += value & 0xff;
	sz->rows = value & 0xff;
	printk(BIOS_SPEW, "%d rows\n", sz->rows);

	value = spd_read_byte(device, SPD_COL_NUM);	/* columns */
	if (value < 0) goto hw_err;
	if ((value & 0xff) == 0) goto val_err;  //max is 11
	sz->per_rank += value & 0xff;
	sz->col = value & 0xff;
	printk(BIOS_SPEW, "%d pre_rank %d col\n", sz->per_rank, sz->col);

	value = spd_read_byte(device, SPD_BANK_NUM);	/* banks */
	if (value < 0) goto hw_err;
	if ((value & 0xff) == 0) goto val_err;
	sz->bank = log2c(value & 0xff);  // convert 4 to 2, and 8 to 3
	printk(BIOS_SPEW, "%d SPD banks %d bank\n", value, sz->bank);
	sz->per_rank += sz->bank;
	printk(BIOS_SPEW, "sz->per_rank is now %d\n", sz->per_rank);
	/* Get the module data width and convert it to a power of two */
	value = spd_read_byte(device, SPD_DATA_WIDTH);	
	if (value < 0) goto hw_err;
	value &= 0xff;
	if ((value != 72) && (value != 64)) goto val_err;
	/* why log2f (floor) here? because 72 bits is really 64 bits + parity */
	sz->per_rank += log2f(value) - 3; //64 bit So another 3 lines
	printk(BIOS_SPEW, "value %d log2f(value) %d sz->per_rank now %d\n", value, log2f(value), sz->per_rank);

	/* How many ranks? */
	value = spd_read_byte(device, SPD_MOD_ATTRIB_RANK);	/* number of physical banks */
	if (value < 0) goto hw_err;
//	value >>= SPD_MOD_ATTRIB_RANK_NUM_SHIFT;
	value &= SPD_MOD_ATTRIB_RANK_NUM_MASK;
	value += SPD_MOD_ATTRIB_RANK_NUM_BASE; // 0-->1, 1-->2, 3-->4
	printk(BIOS_SPEW, "# ranks %d\n", value);
	/*
	  rank == 1 only one rank or say one side
	  rank == 2 two side , and two ranks
	  rank == 4 two side , and four ranks total
	  Some one side two ranks, because of stacked
	*/
	if ((value != 1) && (value != 2) && (value != 4 )) {
		goto val_err;
	}
	sz->rank = value;

	/* verify if per_rank is equal byte 31 
	  it has the DIMM size as a multiple of 128MB.
         */
        value = spd_read_byte(device, SPD_RANK_SIZE);
        if (value < 0) goto hw_err;
        value &= 0xff;
	printk(BIOS_SPEW, "spd rank size is %d\n", value);
	value = log2f(value);
	if(value <=4 ) value += 8; // add back to 1G to high
	value += (27-5); // make 128MB to the real lines
	printk(BIOS_SPEW, " computed value is %d\n", value);
	if( value != (sz->per_rank)) { 
		printk(BIOS_ERR, "Bad RANK Size -- value is 0x%x, and it should be 0x%x\n", value, sz->per_rank);
		printk(BIOS_ERR, "This error has been reduced to a warning for now\n");
//		goto val_err;
	}

	goto out;

 val_err:
	die("Bad SPD value\n");
	/* If a hardware error occurs report that I have no memory */
hw_err:
	sz->per_rank = 0;
	sz->rows = 0;
	sz->col = 0;
	sz->bank = 0;
	sz->rank = 0;
 out:
	return;
}

void set_dimm_size(const struct mem_controller *ctrl, struct dimm_size *sz, unsigned index, int is_Width128)
{
	u32 base0, base1;


	/* For each base register.
	 * Place the dimm size in 32 MB quantities in the bits 31 - 21.
	 * The initialize dimm size is in bits.
	 * Set the base enable bit0.
	 */
	
	base0 = base1 = 0;

	/* Make certain side1 of the dimm is at least 128MB */
	if (sz->per_rank >= 27) {
		base0 = (1 << ((sz->per_rank - 27 ) + 19)) | 1;
	}
	
	/* Make certain side2 of the dimm is at least 128MB */
	if (sz->rank > 1) { // 2 ranks or 4 ranks
		base1 = (1 << ((sz->per_rank - 27 ) + 19)) | 1;
	}

	/* Double the size if we are using dual channel memory */
	if (is_Width128) {
		base0 = (base0 << 1) | (base0 & 1);
		base1 = (base1 << 1) | (base1 & 1);
	}

	/* Clear the reserved bits */
	base0 &= ~0xe007fffe;
	base1 &= ~0xe007fffe;

	/* Set the appropriate DIMM base address register */
	pci_conf1_write_config32(ctrl->f2, DRAM_CSBASE + (((index << 1)+0)<<2), base0);
	pci_conf1_write_config32(ctrl->f2, DRAM_CSBASE + (((index << 1)+1)<<2), base1);
#ifdef QRANK_DIMM_SUPPORT
	if(sz->rank == 4) {
		pci_conf1_write_config32(ctrl->f2, DRAM_CSBASE + (((index << 1)+4)<<2), base0);
		pci_conf1_write_config32(ctrl->f2, DRAM_CSBASE + (((index << 1)+5)<<2), base1);
	}
#endif

	/* Enable the memory clocks for this DIMM by Clear the MemClkDis bit*/
	if (base0) {
		u32 dword;
		u32 ClkDis0;
#if CPU_SOCKET_TYPE == SOCKET_L1
		ClkDis0 = DTL_MemClkDis0;
#else 
	#if CPU_SOCKET_TYPE == SOCKET_AM2
		ClkDis0 = DTL_MemClkDis0_AM2;
	#endif
#endif 

		dword = pci_conf1_read_config32(ctrl->f2, DRAM_TIMING_LOW); //Channel A
		dword &= ~(ClkDis0 >> index);
#ifdef QRANK_DIMM_SUPPORT
		if(sz->rank == 4) {
			dword &= ~(ClkDis0 >> (index+2));
		}
#endif
		pci_conf1_write_config32(ctrl->f2, DRAM_TIMING_LOW, dword);
	
		if (is_Width128) { //Channel B	
	                dword = pci_conf1_read_config32(ctrl->f2, DRAM_CTRL_MISC);
	                dword &= ~(ClkDis0 >> index);
#ifdef QRANK_DIMM_SUPPORT
	                if(sz->rank == 4) {
        	                dword &= ~(ClkDis0 >> (index+2));
                	}
#endif
	                pci_conf1_write_config32(ctrl->f2, DRAM_CTRL_MISC, dword);
		}

	}
}

/*    row    col   bank  for 64 bit
  0:  13     9      2    :128M
  1:  13     10     2    :256M
  2:  14     10     2    :512M
  3:  13     11     2    :512M
  4:  13     10     3    :512M
  5:  14     10     3    :1G
  6:  14     11     2    :1G
  7:  15     10     3    :2G
  8:  14     11     3    :2G
  9:  15     11     3    :4G
 10:  16     10     3    :4G
 11:  16     11     3    :8G
*/

void set_dimm_cs_map(const struct mem_controller *ctrl, struct dimm_size *sz, unsigned index)
{
	static const u8 cs_map_aaa[24] = {
	        /* (bank=2, row=13, col=9)(3, 16, 11) ---> (0, 0, 0) (1, 3, 2) */
	//Bank2
	        0, 1, 3,
	        0, 2, 6,
	        0, 0, 0,
	        0, 0, 0,
	//Bank3
	        0, 4, 0,
	        0, 5, 8,
	        0, 7, 9,
	        0,10,11,
	};

        u32 map;

        map = pci_conf1_read_config32(ctrl->f2, DRAM_BANK_ADDR_MAP);
        map &= ~(0xf << (index * 4));
#ifdef QRANK_DIMM_SUPPORT
        if(sz->rank == 4) {
                map &= ~(0xf << ( (index + 2) * 4));
        }
#endif

        /* Make certain side1 of the dimm is at least 128MB */
        if (sz->per_rank >= 27) {
                unsigned temp_map;
                temp_map = cs_map_aaa[(sz->bank-2)*3*4 + (sz->rows - 13)*3 + (sz->col - 9) ];
                map |= temp_map << (index*4);
#ifdef QRANK_DIMM_SUPPORT
                if(sz->rank == 4) {
                       map |=  temp_map << ( (index + 2) * 4);
                }
#endif
        }

        pci_conf1_write_config32(ctrl->f2, DRAM_BANK_ADDR_MAP, map);

}

long spd_set_ram_size(const struct mem_controller *ctrl, long dimm_mask, struct mem_info *meminfo)
{
	int i;
	
	for(i = 0; i < DIMM_SOCKETS; i++) {
		struct dimm_size *sz = &(meminfo->sz[i]);
		if (!(dimm_mask & (1 << i))) {
			continue;
		}
		spd_get_dimm_size(ctrl->channel0[i], sz);
		if (sz->per_rank == 0) {
			return -1; /* Report SPD error */
		}
		set_dimm_size(ctrl, sz, i, meminfo->is_Width128);
		set_dimm_cs_map (ctrl, sz, i);
	}
	return dimm_mask;
}

void route_dram_accesses(const struct mem_controller *ctrl,
	unsigned long base_k, unsigned long limit_k)
{
	/* Route the addresses to the controller node */
	unsigned node_id;
	unsigned limit;
	unsigned base;
	unsigned index;
	unsigned limit_reg, base_reg;
	u32 device;

	node_id = ctrl->node_id;
	index = (node_id << 3);
	limit = (limit_k << 2);
	limit &= 0xffff0000;
	limit -= 0x00010000;
	limit |= ( 0 << 8) | (node_id << 0);
	base = (base_k << 2);
	base &= 0xffff0000;
	base |= (0 << 8) | (1<<1) | (1<<0);

	limit_reg = 0x44 + index;
	base_reg = 0x40 + index;
	for(device = PCI_BDF(0, 0x18, 1); device <= PCI_BDF(0, 0x1f, 1); device += PCI_BDF(0, 1, 0)) {
		pci_conf1_write_config32(device, limit_reg, limit);
		pci_conf1_write_config32(device, base_reg, base);
	}
}

void set_top_mem(unsigned tom_k, unsigned hole_startk)
{
	/* Error if I don't have memory */
	if (!tom_k) {
		die("No memory?");
	}

	/* Report the amount of memory. */
	printk(BIOS_DEBUG, "RAM: 0x%x KB\n", tom_k);

	struct msr msr;
	if(tom_k > (4*1024*1024)) {
		/* Now set top of memory */
		msr.lo = (tom_k & 0x003fffff) << 10;
		msr.hi = (tom_k & 0xffc00000) >> 22;
		wrmsr(TOP_MEM2, msr);
	}

	/* Leave a 64M hole between TOP_MEM and TOP_MEM2
	 * so I can see my rom chip and other I/O devices.
	 */
	if (tom_k >= 0x003f0000) {
#if CONFIG_HW_MEM_HOLE_SIZEK != 0
		if(hole_startk != 0) {
	                tom_k = hole_startk;
		} else
#endif
		tom_k = 0x3f0000;
	}
	msr.lo = (tom_k & 0x003fffff) << 10;
	msr.hi = (tom_k & 0xffc00000) >> 22;
	wrmsr(TOP_MEM, msr);
}

unsigned long interleave_chip_selects(const struct mem_controller *ctrl, int is_Width128)
{
	/* 35 - 27 */

        static const u8 csbase_low_f0_shift[] = {
        /* 128MB */       (14 - (13-5)),
        /* 256MB */       (15 - (13-5)),
        /* 512MB */       (15 - (13-5)),
	/* 512MB */       (16 - (13-5)),
        /* 512MB */       (16 - (13-5)),
        /* 1GB   */       (16 - (13-5)),
        /* 1GB   */       (16 - (13-5)),
        /* 2GB   */       (16 - (13-5)),
        /* 2GB   */       (17 - (13-5)),
	/* 4GB   */       (17 - (13-5)),
        /* 4GB   */       (16 - (13-5)),
	/* 8GB   */       (17 - (13-5)),
        };

	/* cs_base_high is not changed */

	u32 csbase_inc;
	int chip_selects, index;
	int bits;
	unsigned common_size;
	unsigned common_cs_mode;
	u32 csbase, csmask;

	/* See if all of the memory chip selects are the same size
	 * and if so count them.
	 */
	chip_selects = 0;
	common_size = 0;
	common_cs_mode = 0xff;
	for(index = 0; index < 8; index++) {
		unsigned size;
		unsigned cs_mode;
		u32 value;
		
		value = pci_conf1_read_config32(ctrl->f2, DRAM_CSBASE + (index << 2));
		
		/* Is it enabled? */
		if (!(value & 1)) {
			continue;
		}
		chip_selects++;
		size = (value >> 19) & 0x3ff;
		if (common_size == 0) {
			common_size = size;
		}
		/* The size differed fail */
		if (common_size != size) {
			return 0;
		}

		value = pci_conf1_read_config32(ctrl->f2, DRAM_BANK_ADDR_MAP);
                cs_mode =( value >> ((index>>1)*4)) & 0xf;
                if(common_cs_mode == 0xff) {
                	common_cs_mode = cs_mode;
                }
                /* The cs_mode differed fail */
                if(common_cs_mode != cs_mode) {
                        return 0;
                }
	}

	/* Chip selects can only be interleaved when there is
	 * more than one and their is a power of two of them.
	 */
	bits = log2c(chip_selects);
	if (((1 << bits) != chip_selects) || (bits < 1) || (bits > 3)) { //chip_selects max = 8
		return 0;
	}

	/* Find the bits of csbase that we need to interleave on */
	csbase_inc = 1 << (csbase_low_f0_shift[common_cs_mode]);
	if(is_Width128) {
		csbase_inc <<=1;
        }   
	

	/* Compute the initial values for csbase and csbask. 
	 * In csbase just set the enable bit and the base to zero.
	 * In csmask set the mask bits for the size and page level interleave.
	 */
	csbase = 0 | 1;
	csmask = (((common_size  << bits) - 1) << 19);
	csmask |= 0x3fe0 & ~((csbase_inc << bits) - csbase_inc);
	for(index = 0; index < 8; index++) {
		u32 value;

		value = pci_conf1_read_config32(ctrl->f2, DRAM_CSBASE + (index << 2));
		/* Is it enabled? */
		if (!(value & 1)) {
			continue;
		}
		pci_conf1_write_config32(ctrl->f2, DRAM_CSBASE + (index << 2), csbase);
		if((index & 1) == 0) {  //only have 4 CSMASK
			pci_conf1_write_config32(ctrl->f2, DRAM_CSMASK + ((index>>1) << 2), csmask);
		}
		csbase += csbase_inc;
	}
	
	printk(BIOS_DEBUG, "Interleaved\n");

	/* Return the memory size in K */ 
	return common_size << ((27-10) + bits);
}
unsigned long order_chip_selects(const struct mem_controller *ctrl)
{
	unsigned long tom;

	/* Remember which registers we have used in the high 8 bits of tom */
	tom = 0;
	for(;;) {
		/* Find the largest remaining candidate */
		unsigned index, candidate;
		u32 csbase, csmask;
		unsigned size;
		csbase = 0;
		candidate = 0;
		for(index = 0; index < 8; index++) {
			u32 value;
			value = pci_conf1_read_config32(ctrl->f2, DRAM_CSBASE + (index << 2));

			/* Is it enabled? */
			if (!(value & 1)) {
				continue;
			}
			
			/* Is it greater? */
			if (value <= csbase) {
				continue;
			}
			
			/* Has it already been selected */
			if (tom & (1 << (index + 24))) {
				continue;
			}
			/* I have a new candidate */
			csbase = value;
			candidate = index;
		}
		/* See if I have found a new candidate */
		if (csbase == 0) {
			break;
		}

		/* Remember the dimm size */
		size = csbase >> 19;

		/* Remember I have used this register */
		tom |= (1 << (candidate + 24));

		/* Recompute the cs base register value */
		csbase = (tom << 19) | 1;

		/* Increment the top of memory */
		tom += size;

		/* Compute the memory mask */
		csmask = ((size -1) << 19);
		csmask |= 0x3fe0;		/* For now don't optimize */

		/* Write the new base register */
		pci_conf1_write_config32(ctrl->f2, DRAM_CSBASE + (candidate << 2), csbase);
		/* Write the new mask register */
                if((candidate & 1) == 0) {  //only have 4 CSMASK
                        pci_conf1_write_config32(ctrl->f2, DRAM_CSMASK + ((candidate>>1) << 2), csmask);
                }
		
	}
	/* Return the memory size in K */ 
	return (tom & ~0xff000000) << (27-10);
}

unsigned long memory_end_k(const struct mem_controller *ctrl, int max_node_id)
{
	unsigned node_id;
	unsigned end_k;
	/* Find the last memory address used */
	end_k = 0;
	for(node_id = 0; node_id < max_node_id; node_id++) {
		u32 limit, base;
		unsigned index;
		index = node_id << 3;
		base = pci_conf1_read_config32(ctrl->f1, 0x40 + index);
		/* Only look at the limit if the base is enabled */
		if ((base & 3) == 3) {
			limit = pci_conf1_read_config32(ctrl->f1, 0x44 + index);
			end_k = ((limit + 0x00010000) & 0xffff0000) >> 2;
		}
	}
	return end_k;
}

void order_dimms(const struct mem_controller *ctrl, struct mem_info *meminfo)
{
	unsigned long tom_k, base_k;
	int interleave = 1;

	if (get_option(&interleave, "interleave_chip_selects")) {
		tom_k = interleave_chip_selects(ctrl, meminfo->is_Width128);
	} else {
		printk(BIOS_DEBUG, "Interleaving disabled\n");
		tom_k = 0;
	}
	if (!tom_k) {
		tom_k = order_chip_selects(ctrl);
	}
	/* Compute the memory base address */
	base_k = memory_end_k(ctrl, ctrl->node_id);
	tom_k += base_k;
	route_dram_accesses(ctrl, base_k, tom_k);
	set_top_mem(tom_k, 0);
}

long disable_dimm(const struct mem_controller *ctrl, unsigned index, long dimm_mask, struct mem_info *meminfo)
{
	printk(BIOS_DEBUG, "disabling dimm 0x%x \n", index); 
	pci_conf1_write_config32(ctrl->f2, DRAM_CSBASE + (((index << 1)+0)<<2), 0);
	pci_conf1_write_config32(ctrl->f2, DRAM_CSBASE + (((index << 1)+1)<<2), 0);
#ifdef QRANK_DIMM_SUPPORT
        if(meminfo->sz[index].rank == 4) {
                pci_conf1_write_config32(ctrl->f2, DRAM_CSBASE + (((index << 1)+4)<<2), 0);
                pci_conf1_write_config32(ctrl->f2, DRAM_CSBASE + (((index << 1)+5)<<2), 0);
        }
#endif

	dimm_mask &= ~(1 << index);
	return dimm_mask;
}

long spd_handle_unbuffered_dimms(const struct mem_controller *ctrl, long dimm_mask, struct mem_info *meminfo)
{
	int i;
	u32 registered;
	u32 dcl;
	registered = 0;
	for(i = 0; (i < DIMM_SOCKETS); i++) {
		int value;
		if (!(dimm_mask & (1 << i))) {
			continue;
		}
		value = spd_read_byte(ctrl->channel0[i], SPD_DIMM_TYPE);
		if (value < 0) {
			return -1;
		}
		/* Registered dimm ? */
		value &= 0x3f;
		if ((value == SPD_DIMM_TYPE_RDIMM) || (value == SPD_DIMM_TYPE_mRDIMM)) {
			//check SPD_MOD_ATTRIB to verify it is SPD_MOD_ATTRIB_REGADC (0x11)?
			registered |= (1<<i);
		} 
	}

	if (is_opteron(ctrl)) {
#if 0
		if ( registered != (dimm_mask & ((1<<DIMM_SOCKETS)-1)) ) {
			dimm_mask &= (registered | (registered << DIMM_SOCKETS) ); //disable unbuffed dimm
	//		die("Mixed buffered and registered dimms not supported");
		}
		//By yhlu for debug M2, s1g1 can do dual channel, but it use unbuffer DIMM
		if (!registered) {
			die("Unbuffered Dimms not supported on Opteron");
		}
#endif
	}


	dcl = pci_conf1_read_config32(ctrl->f2, DRAM_CONFIG_LOW);
	dcl &= ~DCL_UnBuffDimm;
	meminfo->is_registered = 1;
	if (!registered) {
		dcl |= DCL_UnBuffDimm;
		meminfo->is_registered = 0;
	}
	pci_conf1_write_config32(ctrl->f2, DRAM_CONFIG_LOW, dcl);
	
#if 1
	if (meminfo->is_registered) {
		printk(BIOS_DEBUG, "Registered\n");
	} else {
		printk(BIOS_DEBUG, "Unbuffered\n");
	}
#endif
	return dimm_mask;
}

unsigned int spd_detect_dimms(const struct mem_controller *ctrl)
{
	unsigned dimm_mask;
	int i;
	dimm_mask = 0;
	for(i = 0; i < DIMM_SOCKETS; i++) {
		int byte;
		unsigned device;
		device = ctrl->channel0[i];
		if (device) {
			byte = spd_read_byte(ctrl->channel0[i], SPD_MEM_TYPE);  /* Type */
			if (byte == SPD_MEM_TYPE_SDRAM_DDR2) {
				dimm_mask |= (1 << i);
			}
		}
		device = ctrl->channel1[i];
		if (device) {
			byte = spd_read_byte(ctrl->channel1[i], SPD_MEM_TYPE);
			if (byte == SPD_MEM_TYPE_SDRAM_DDR2) {
				dimm_mask |= (1 << (i + DIMM_SOCKETS));
			}
		}
	}
	return dimm_mask;
}

long spd_enable_2channels(const struct mem_controller *ctrl, long dimm_mask, struct mem_info *meminfo)
{
	int i;
	u32 nbcap;
	/* SPD addresses to verify are identical */
	static const u8 addresses[] = {
		2,	/* Type should be DDR2 SDRAM */
		3,	/* *Row addresses */
		4,	/* *Column addresses */
		5,	/* *Number of DIMM Ranks */
		6,	/* *Module Data Width*/
		9,	/* *Cycle time at highest CAS Latency CL=X */
		11,	/* *DIMM Conf Type */
		13,	/* *Pri SDRAM Width */
		17,	/* *Logical Banks */
		18,	/* *Supported CAS Latencies */
		20,     /* *DIMM Type Info */
		21,	/* *SDRAM Module Attributes */
		23,	/* *Cycle time at CAS Latnecy (CLX - 1) */
		26,	/* *Cycle time at CAS Latnecy (CLX - 2) */
		27,	/* *tRP Row precharge time */
		28,	/* *Minimum Row Active to Row Active Delay (tRRD) */
		29,	/* *tRCD RAS to CAS */
		30,	/* *tRAS Activate to Precharge */
		36,	/* *Write recovery time (tWR) */
		37, 	/* *Internal write to read command delay (tRDP) */
		38, 	/* *Internal read to precharge commanfd delay (tRTP) */
		41, 	/* *Extension of Byte 41 tRC and Byte 42 tRFC */
		41,	/* *Minimum Active to Active/Auto Refresh Time(Trc) */
		42,	/* *Minimum Auto Refresh Command Time(Trfc) */
	};
	/* If the dimms are not in pairs do not do dual channels */
	if ((dimm_mask & ((1 << DIMM_SOCKETS) - 1)) !=
		((dimm_mask >> DIMM_SOCKETS) & ((1 << DIMM_SOCKETS) - 1))) { 
		goto single_channel;
	}
	/* If the cpu is not capable of doing dual channels don't do dual channels */
	nbcap = pci_conf1_read_config32(ctrl->f3, NORTHBRIDGE_CAP);
	if (!(nbcap & NBCAP_128Bit)) {
		goto single_channel;
	}
	for(i = 0; (i < 4) && (ctrl->channel0[i]); i++) {
		unsigned device0, device1;
		int value0, value1;
		int j;
		/* If I don't have a dimm skip this one */
		if (!(dimm_mask & (1 << i))) {
			continue;
		}
		device0 = ctrl->channel0[i];
		device1 = ctrl->channel1[i];
		for(j = 0; j < ARRAY_SIZE(addresses); j++) {
			unsigned addr;
			addr = addresses[j];
			value0 = spd_read_byte(device0, addr);
			if (value0 < 0) {
				return -1;
			}
			value1 = spd_read_byte(device1, addr);
			if (value1 < 0) {
				return -1;
			}
			if (value0 != value1) {
				goto single_channel;
			}
		}
	}
	printk(BIOS_SPEW, "Enabling dual channel memory\n");
	u32 dcl;
	dcl = pci_conf1_read_config32(ctrl->f2, DRAM_CONFIG_LOW);
	dcl &= ~DCL_BurstLength32;  
	/*	32byte mode may be preferred in platforms that include graphics controllers
	 *  that generate a lot of 32-bytes system memory accesses
	 * 32byte mode is not supported when the DRAM interface is 128 bits wide, 
	 * even 32byte mode is set, system still use 64 byte mode	*/
	dcl |= DCL_Width128;
	pci_conf1_write_config32(ctrl->f2, DRAM_CONFIG_LOW, dcl);
	meminfo->is_Width128 = 1;
	return dimm_mask;
 single_channel:
	dimm_mask &= ~((1 << (DIMM_SOCKETS *2)) - (1 << DIMM_SOCKETS));
	meminfo->is_Width128 = 0;
	return dimm_mask;
}

struct mem_param {
	u16 cycle_time;
	u8 divisor; /* In 1/40 ns increments */
	u8 TrwtTO;
        u8 Twrrd;
        u8 Twrwr;
        u8 Trdrd;
        u8 DcqByPassMax;
	u32 dch_memclk;
	char name[9];
};

	static const struct mem_param speed[] = {
		{
			.name	    = "200Mhz\n",
			.cycle_time = 0x500,
			.divisor    = 200, // how many 1/40ns per clock 
			.dch_memclk = DCH_MemClkFreq_200MHz, //0
			.TrwtTO	    = 7,
			.Twrrd	    = 2,
			.Twrwr	    = 2,
			.Trdrd	    = 3,
			.DcqByPassMax = 4,

		},
		{
			.name	    = "266Mhz\n",
			.cycle_time = 0x375,
			.divisor    = 150, //????
			.dch_memclk = DCH_MemClkFreq_266MHz, //1
                        .TrwtTO     = 7,
                        .Twrrd      = 2,
                        .Twrwr      = 2,
                        .Trdrd      = 3,
                        .DcqByPassMax = 4,
		},
                {
                        .name       = "333Mhz\n",
                        .cycle_time = 0x300,
                        .divisor    = 120,
                        .dch_memclk = DCH_MemClkFreq_333MHz, //2
                        .TrwtTO     = 7,
                        .Twrrd      = 2,
                        .Twrwr      = 2,
                        .Trdrd      = 3,
                        .DcqByPassMax = 4,

                },
		{
			.name	    = "400Mhz\n",
			.cycle_time = 0x250,
			.divisor    = 100,
			.dch_memclk = DCH_MemClkFreq_400MHz,//3
                        .TrwtTO     = 7,
                        .Twrrd      = 2,
                        .Twrwr      = 2,
                        .Trdrd      = 3,
                        .DcqByPassMax = 4,
		},
		{
			.cycle_time = 0x000,
		},
	};

const struct mem_param *get_mem_param(unsigned min_cycle_time)
{

	const struct mem_param *param;
	for(param = &speed[0]; param->cycle_time ; param++) {
		if (min_cycle_time > (param+1)->cycle_time) {
			break;
		}
	}
	if (!param->cycle_time) {
		die("min_cycle_time to low");
	}
	printk(BIOS_SPEW, param->name);
#ifdef DRAM_MIN_CYCLE_TIME
	printk(BIOS_DEBUG, param->name);
#endif
	return param;
}

u8 get_exact_divisor(int i, u8 divisor)
{
	//input divisor could be 200(200), 150(266), 120(333), 100 (400)
	static const u8 dv_a[] = {
	       /* 200  266  333  400 */
	 /*4 */	  250, 250, 250, 250,
	 /*5 */   200, 200, 200, 100,
	 /*6 */   200, 166, 166, 100,
	 /*7 */   200, 171, 142, 100,

         /*8 */   200, 150, 125, 100,
         /*9 */   200, 156, 133, 100,
         /*10*/   200, 160, 120, 100,
         /*11*/   200, 163, 127, 100,

         /*12*/   200, 150, 133, 100,
         /*13*/   200, 153, 123, 100,
         /*14*/   200, 157, 128, 100,
         /*15*/   200, 160, 120, 100,
	}; 
	
	unsigned fid_cur;
	int index;
	
	struct msr msr;
	msr = rdmsr(0xc0010042);
        fid_cur = msr.lo & 0x3f;

	index = fid_cur>>1;

	if(index>12) return divisor;

	if(i>3) return divisor;

	return dv_a[index * 4+i];

}

struct spd_set_memclk_result {
	const struct mem_param *param;
	long dimm_mask;
};

unsigned convert_to_linear(unsigned value)
{
         static const unsigned fraction[] = { 0x25, 0x33, 0x66, 0x75 };
         unsigned valuex;

         /* We need to convert value to more readable */
         if((value & 0xf) < 10) { //no .25, .33, .66, .75
                value <<= 4;
         } else {
		valuex = ((value & 0xf0) << 4) | fraction [(value & 0xf)-10];
                value = valuex;
         }
	 return value;
}

struct spd_set_memclk_result spd_set_memclk(const struct mem_controller *ctrl, long dimm_mask, struct mem_info *meminfo)
{
	/* Compute the minimum cycle time for these dimms */
	struct spd_set_memclk_result result;
	unsigned min_cycle_time, min_latency, bios_cycle_time;
	int i;
	u32 value;
	unsigned int max_mem_clock = 0;

	static const u8 latency_indicies[] = { 25, 23, 9 };

	static const u16 min_cycle_times[] = { // use full speed to compare
		[NBCAP_MEMCLK_NOLIMIT] = 0x250, /*2.5ns */
		[NBCAP_MEMCLK_333MHZ] = 0x300, /* 3.0ns */
		[NBCAP_MEMCLK_266MHZ] = 0x375, /* 3.75ns */
		[NBCAP_MEMCLK_200MHZ] = 0x500, /* 5.0s */
	};


	value = pci_conf1_read_config32(ctrl->f3, NORTHBRIDGE_CAP);
	min_cycle_time = min_cycle_times[(value >> NBCAP_MEMCLK_SHIFT) & NBCAP_MEMCLK_MASK];
	get_option(&max_mem_clock, "max_mem_clock");
	bios_cycle_time = min_cycle_times[max_mem_clock];
	if (bios_cycle_time > min_cycle_time) {
		min_cycle_time = bios_cycle_time;
	}
	min_latency = 3; 

        printk(BIOS_DEBUG, "%s: 0x%x\n", "1 min_cycle_time:",  min_cycle_time);

	/* Compute the least latency with the fastest clock supported
	 * by both the memory controller and the dimms.
	 */
	for(i = 0; i < DIMM_SOCKETS; i++) {
		int new_cycle_time, new_latency;
		int index;
		int latencies;
		int latency;

		if (!(dimm_mask & (1 << i))) {
			continue;
		}

		/* First find the supported CAS latencies
		 * Byte 18 for DDR SDRAM is interpreted:
		 * bit 3 == CAS Latency = 3
		 * bit 4 == CAS Latency = 4
		 * bit 5 == CAS Latency = 5
		 * bit 6 == CAS Latency = 6
		 */
		new_cycle_time = 0x500;
		new_latency = 6;

		latencies = spd_read_byte(ctrl->channel0[i], SPD_CAS_LAT);
		if (latencies <= 0) continue;

		printk(BIOS_DEBUG, "%s: 0x%x\n", "i:", i);
		printk(BIOS_DEBUG, "%s: 0x%x\n", "\tlatencies:",  latencies);
		/* Compute the lowest cas latency supported */
		latency = log2f(latencies) - 2;

		/* Loop through and find a fast clock with a low latency */
		for(index = 0; index < 3; index++, latency++) {
			int value;
			if ((latency < 3) || (latency > 6) ||
				(!(latencies & (1 << latency)))) {
				continue;
			}
			value = spd_read_byte(ctrl->channel0[i], latency_indicies[index]);
			if (value < 0) {
				goto hw_error;
			}
			printk(BIOS_DEBUG, "%s: 0x%x\n", "\tindex:",  index);
			printk(BIOS_DEBUG, "%s: 0x%x\n", "\t\tlatency:",  latency);
			printk(BIOS_DEBUG, "%s: 0x%x\n", "\t\tvalue1:",  value);

			value = convert_to_linear(value);

			printk(BIOS_DEBUG, "%s: 0x%x\n", "\t\tvalue2:",  value);

			/* Only increase the latency if we decreas the clock */
			if (value >= min_cycle_time ) {
				if(value < new_cycle_time) {
					new_cycle_time = value;
					new_latency = latency;
				} else if (value == new_cycle_time) {
					if(new_latency > latency) {
						new_latency = latency;
					}
				}
			}
                        printk(BIOS_DEBUG, "%s: 0x%x\n", "\t\tnew_cycle_time:",  new_cycle_time);
                        printk(BIOS_DEBUG, "%s: 0x%x\n", "\t\tnew_latency:",  new_latency);

		}
		if (new_latency > 6){
			continue;
		}
		/* Does min_latency need to be increased? */
		if (new_cycle_time > min_cycle_time) {
			min_cycle_time = new_cycle_time;
		}
		/* Does min_cycle_time need to be increased? */
		if (new_latency > min_latency) {
			min_latency = new_latency;
		}

		printk(BIOS_DEBUG, "%s: 0x%x\n", "2 min_cycle_time:",  min_cycle_time);
		printk(BIOS_DEBUG, "%s: 0x%x\n", "2 min_latency:",  min_latency);
	}
	/* Make a second pass through the dimms and disable
	 * any that cannot support the selected memclk and cas latency.
	 */

	printk(BIOS_DEBUG, "%s: 0x%x\n", "3 min_cycle_time:",  min_cycle_time);
	printk(BIOS_DEBUG, "%s: 0x%x\n", "3 min_latency:",  min_latency);
	
	for(i = 0; (i < DIMM_SOCKETS) && (ctrl->channel0[i]); i++) {
		int latencies;
		int latency;
		int index;
		int value;
		if (!(dimm_mask & (1 << i))) {
			continue;
		}
		latencies = spd_read_byte(ctrl->channel0[i], SPD_CAS_LAT);
		if (latencies < 0) goto hw_error;
		if (latencies == 0) {
			continue;
		}

		/* Compute the lowest cas latency supported */
		latency = log2f(latencies) -2;

		/* Walk through searching for the selected latency */
		for(index = 0; index < 3; index++, latency++) {
			if (!(latencies & (1 << latency))) {
				continue;
			}
			if (latency == min_latency)
				break;
		}
		/* If I can't find the latency or my index is bad error */
		if ((latency != min_latency) || (index >= 3)) {
			goto dimm_err;
		}
	
		/* Read the min_cycle_time for this latency */
		value = spd_read_byte(ctrl->channel0[i], latency_indicies[index]);
		if (value < 0) goto hw_error;
	
		value = convert_to_linear(value);	
		/* All is good if the selected clock speed 
		 * is what I need or slower.
		 */
		if (value <= min_cycle_time) {
			continue;
		}
		/* Otherwise I have an error, disable the dimm */
	dimm_err:
		dimm_mask = disable_dimm(ctrl, i, dimm_mask, meminfo);
	}

	printk(BIOS_DEBUG, "%s: 0x%x\n", "4 min_cycle_time:",  min_cycle_time);
	
	/* Now that I know the minimum cycle time lookup the memory parameters */
	result.param = get_mem_param(min_cycle_time);

	/* Update DRAM Config High with our selected memory speed */
	value = pci_conf1_read_config32(ctrl->f2, DRAM_CONFIG_HIGH);
	value &= ~(DCH_MemClkFreq_MASK << DCH_MemClkFreq_SHIFT);

	value |= result.param->dch_memclk << DCH_MemClkFreq_SHIFT;
	pci_conf1_write_config32(ctrl->f2, DRAM_CONFIG_HIGH, value);

	printk(BIOS_DEBUG, result.param->name);

	/* Update DRAM Timing Low with our selected cas latency */
	value = pci_conf1_read_config32(ctrl->f2, DRAM_TIMING_LOW);
	value &= ~(DTL_TCL_MASK << DTL_TCL_SHIFT);
	value |= (min_latency - DTL_TCL_BASE)  << DTL_TCL_SHIFT;
	pci_conf1_write_config32(ctrl->f2, DRAM_TIMING_LOW, value);
	
	result.dimm_mask = dimm_mask;
	return result;
 hw_error:
	result.param = (const struct mem_param *)0;
	result.dimm_mask = -1;
	return result;
}

unsigned convert_to_1_4(unsigned value)
{
         static const u8 fraction[] = { 0, 1, 2, 2, 3, 3, 0 };
         unsigned valuex;

         /* We need to convert value to more readable */
         valuex =  fraction [value & 0x7];
         return valuex;
}
int update_dimm_Trc(const struct mem_controller *ctrl, const struct mem_param *param, int i)
{
	unsigned clocks, old_clocks;
	u32 dtl;
	int value;
	int value2;
	value = spd_read_byte(ctrl->channel0[i], SPD_TRC);
	if (value < 0) return -1;

		value2 = spd_read_byte(ctrl->channel0[i], SPD_TRC -1);
		value <<= 2;
		value += convert_to_1_4(value2>>4);

	value *=10;

	clocks = (value + param->divisor - 1)/param->divisor;
	
	if (clocks < DTL_TRC_MIN) {
		clocks = DTL_TRC_MIN;
	}
	if (clocks > DTL_TRC_MAX) {
		return 0;
	}

	dtl = pci_conf1_read_config32(ctrl->f2, DRAM_TIMING_LOW);
	old_clocks = ((dtl >> DTL_TRC_SHIFT) & DTL_TRC_MASK) + DTL_TRC_BASE;
	if (old_clocks >= clocks) {  //?? someone did it 
		// clocks = old_clocks;
		return 1;
	}
	dtl &= ~(DTL_TRC_MASK << DTL_TRC_SHIFT);
	dtl |=	((clocks - DTL_TRC_BASE) << DTL_TRC_SHIFT);
	pci_conf1_write_config32(ctrl->f2, DRAM_TIMING_LOW, dtl);
	return 1;
}

int update_dimm_Trfc(const struct mem_controller *ctrl, const struct mem_param *param, int i, struct mem_info *meminfo)
{
	unsigned clocks, old_clocks;
	u32 dth;
	int value;

	//get the cs_size --> logic dimm size
	value = spd_read_byte(ctrl->channel0[i], SPD_PRI_WIDTH);
        if (value < 0) {
                return -1;
        }

	value = 6 - log2f(value); //4-->4, 8-->3, 16-->2

	clocks = meminfo->sz[i].per_rank - 27 + 2 - value;

	dth = pci_conf1_read_config32(ctrl->f2, DRAM_TIMING_HIGH);

	old_clocks = ((dth >> (DTH_TRFC0_SHIFT+i*3)) & DTH_TRFC_MASK);
	if (old_clocks >= clocks) { // some one did it?
		return 1;
	}
	dth &= ~(DTH_TRFC_MASK << (DTH_TRFC0_SHIFT+i*3));
	dth |= clocks  << (DTH_TRFC0_SHIFT+i*3);
	pci_conf1_write_config32(ctrl->f2, DRAM_TIMING_HIGH, dth);
	return 1;
}

int update_dimm_TT_1_4(const struct mem_controller *ctrl, const struct mem_param *param, 
					int i, 
					unsigned TT_REG, 
					unsigned SPD_TT, unsigned TT_SHIFT, 
					unsigned TT_MASK, unsigned TT_BASE, 
					unsigned TT_MIN, unsigned TT_MAX )
{
        unsigned clocks, old_clocks;
        u32 dtl;
        int value;
        value = spd_read_byte(ctrl->channel0[i], SPD_TT); //already in 1/4 ns
        if (value < 0) return -1;
	value *=10;
        clocks = (value + param->divisor -1)/param->divisor;
        if (clocks < TT_MIN) {
                clocks = TT_MIN;
        }
        if (clocks > TT_MAX) {
                return 0;
        }
        dtl = pci_conf1_read_config32(ctrl->f2, TT_REG);

        old_clocks = ((dtl >> TT_SHIFT) & TT_MASK) + TT_BASE;
        if (old_clocks >= clocks) { //some one did it?
//              clocks = old_clocks;
                return 1;
        }
        dtl &= ~(TT_MASK << TT_SHIFT);
        dtl |= ((clocks - TT_BASE) << TT_SHIFT);
        pci_conf1_write_config32(ctrl->f2, TT_REG, dtl);
        return 1;
}

int update_dimm_Trcd(const struct mem_controller *ctrl, const struct mem_param *param, int i)
{
	return update_dimm_TT_1_4(ctrl, param, i, DRAM_TIMING_LOW, SPD_TRCD, DTL_TRCD_SHIFT, DTL_TRCD_MASK, DTL_TRCD_BASE, DTL_TRCD_MIN, DTL_TRCD_MAX);
}

int update_dimm_Trrd(const struct mem_controller *ctrl, const struct mem_param *param, int i)
{
	return update_dimm_TT_1_4(ctrl, param, i, DRAM_TIMING_LOW, SPD_TRRD, DTL_TRRD_SHIFT, DTL_TRRD_MASK, DTL_TRRD_BASE, DTL_TRRD_MIN, DTL_TRRD_MAX);
}

int update_dimm_Tras(const struct mem_controller *ctrl, const struct mem_param *param, int i)
{
	unsigned clocks, old_clocks;
	u32 dtl;
	int value;
	value = spd_read_byte(ctrl->channel0[i], SPD_TRAS); //in 1 ns
	if (value < 0) return -1;
	printk(BIOS_DEBUG, "%s: 0x%x\n", "update_dimm_Tras: 0 value=",  value);
	
	value<<=2; //convert it to in 1/4ns

	value *= 10;
	printk(BIOS_DEBUG, "%s: 0x%x\n", "update_dimm_Tras:  1 value=",  value);

	clocks = (value  + param->divisor - 1)/param->divisor;
	printk(BIOS_DEBUG, "%s: 0x%x\n", "update_dimm_Tras: divisor=",  param->divisor);
	printk(BIOS_DEBUG, "%s: 0x%x\n", "update_dimm_Tras: clocks=",  clocks);
	if (clocks < DTL_TRAS_MIN) {
		clocks = DTL_TRAS_MIN;
	}
	if (clocks > DTL_TRAS_MAX) {
		return 0;
	}
	dtl = pci_conf1_read_config32(ctrl->f2, DRAM_TIMING_LOW);
	old_clocks = ((dtl >> DTL_TRAS_SHIFT) & DTL_TRAS_MASK) + DTL_TRAS_BASE;
	if (old_clocks >= clocks) { // someone did it?
		return 1;
	}
	dtl &= ~(DTL_TRAS_MASK << DTL_TRAS_SHIFT);
	dtl |= ((clocks - DTL_TRAS_BASE) << DTL_TRAS_SHIFT);
	pci_conf1_write_config32(ctrl->f2, DRAM_TIMING_LOW, dtl);
	return 1;
}

int update_dimm_Trp(const struct mem_controller *ctrl, const struct mem_param *param, int i)
{
	return update_dimm_TT_1_4(ctrl, param, i, DRAM_TIMING_LOW, SPD_TRP, DTL_TRP_SHIFT, DTL_TRP_MASK, DTL_TRP_BASE, DTL_TRP_MIN, DTL_TRP_MAX);
}

int update_dimm_Trtp(const struct mem_controller *ctrl, const struct mem_param *param, int i, struct mem_info *meminfo)
{
	//need to figure if it is 32 byte burst or 64 bytes burst
	int offset = 2;
	if(!meminfo->is_Width128) {
		u32 dword;
		dword = pci_conf1_read_config32(ctrl->f2, DRAM_CONFIG_LOW);
		if((dword &  DCL_BurstLength32)) offset = 0;
	}
        return update_dimm_TT_1_4(ctrl, param, i, DRAM_TIMING_LOW, SPD_TRTP, DTL_TRTP_SHIFT, DTL_TRTP_MASK, DTL_TRTP_BASE+offset, DTL_TRTP_MIN+offset, DTL_TRTP_MAX+offset);
}


int update_dimm_Twr(const struct mem_controller *ctrl, const struct mem_param *param, int i)
{
	return update_dimm_TT_1_4(ctrl, param, i, DRAM_TIMING_LOW, SPD_TWR, DTL_TWR_SHIFT, DTL_TWR_MASK, DTL_TWR_BASE, DTL_TWR_MIN, DTL_TWR_MAX);
}


int update_dimm_Tref(const struct mem_controller *ctrl, const struct mem_param *param, int i)
{
        u32 dth, dth_old;
        int value;
        value = spd_read_byte(ctrl->channel0[i], SPD_TREF); // 0: 15.625us, 1: 3.9us 2: 7.8 us....
        if (value < 0) return -1;

	if(value == 1 ) {
		value = 3;
	} else {
		value = 2;
	}

	dth = pci_conf1_read_config32(ctrl->f2, DRAM_TIMING_HIGH);

	dth_old = dth;
	dth &= ~(DTH_TREF_MASK << DTH_TREF_SHIFT);
	dth |= (value << DTH_TREF_SHIFT);
	if(dth_old != dth) {
		pci_conf1_write_config32(ctrl->f2, DRAM_TIMING_HIGH, dth);
	}
	return 1;
}

void set_4RankRDimm(const struct mem_controller *ctrl, const struct mem_param *param, struct mem_info *meminfo)
{
#ifdef QRANK_DIMM_SUPPORT
        int value;
	int i;
	

	if(!(meminfo->is_registered)) return; 

	value = 0;

        for(i = 0; i < DIMM_SOCKETS; i++) {
                if (!(dimm_mask & (1 << i))) {
                        continue;
                }

		if(meminfo->sz.rank == 4) {
			value = 1;
			break;
		}
	}

	if(value == 1) {
		u32 dch;
	        dch = pci_conf1_read_config32(ctrl->f2, DRAM_CONFIG_HIGH);
		dch |= DCH_FourRankRDimm;
	        pci_conf1_write_config32(ctrl->f2, DRAM_CONFIG_HIGH, dch);
	}
#endif
}


u32 get_extra_dimm_mask(const struct mem_controller *ctrl, struct mem_info *meminfo)
{
        int i;

        u32 mask_x4;
	u32 mask_x16;
	u32 mask_single_rank;
	u32 mask_page_1k;
	int value;
#ifdef QRANK_DIMM_SUPPORT
        int rank;
#endif

	long dimm_mask = meminfo->dimm_mask;


        mask_x4 = 0;
	mask_x16 = 0;
	mask_single_rank = 0;
	mask_page_1k = 0;

        for(i = 0; i < DIMM_SOCKETS; i++) {
		
                if (!(dimm_mask & (1 << i))) {
                        continue;
                }

		if(meminfo->sz[i].rank == 1) {
			mask_single_rank |= 1<<i;
		}

                if(meminfo->sz[i].col==10) {
                        mask_page_1k |= 1<<i;
                }


	        value = spd_read_byte(ctrl->channel0[i], SPD_PRI_WIDTH);

		#ifdef QRANK_DIMM_SUPPORT
       		rank = meminfo->sz[i].rank;
		#endif

		if(value==4) {
			mask_x4 |= (1<<i);
			#ifdef QRANK_DIMM_SUPPORT
		        if(rank==4) {
                		mask_x4 |= 1<<(i+2);
        		}
			#endif
		} else if(value==16) {
			mask_x16 |= (1<<i);
			#ifdef QRANK_DIMM_SUPPORT
                        if(rank==4) {
                                mask_x16 |= 1<<(i+2);
                        }
			#endif
		}
                
        }
	
        meminfo->x4_mask= mask_x4;
	meminfo->x16_mask = mask_x16;
	
	meminfo->single_rank_mask = mask_single_rank;
	meminfo->page_1k_mask = mask_page_1k;

        return mask_x4;

}


void set_dimm_x4(const struct mem_controller *ctrl, const struct mem_param *param, struct mem_info *meminfo)
{
	u32 dcl;
	dcl = pci_conf1_read_config32(ctrl->f2, DRAM_CONFIG_LOW);
	dcl &= ~(DCL_X4Dimm_MASK<<DCL_X4Dimm_SHIFT);
	dcl |= ((meminfo->x4_mask) & 0xf) << (DCL_X4Dimm_SHIFT);
	pci_conf1_write_config32(ctrl->f2, DRAM_CONFIG_LOW, dcl);
}

int count_ones(u32 dimm_mask)
{
        int dimms;
        unsigned index;
        dimms = 0;
        for(index = 0; index < DIMM_SOCKETS; index++, dimm_mask>>=1) {
                if (dimm_mask & 1) {
                        dimms++;
                }
        }
        return dimms;
}


void set_DramTerm(const struct mem_controller *ctrl, const struct mem_param *param, struct mem_info *meminfo)
{
        u32 dcl;
	unsigned odt;
	odt = 1; // 75 ohms

	if(param->divisor == 100) { //DDR2 800
		if(meminfo->is_Width128) {
			if(count_ones(meminfo->dimm_mask & 0x0f)==2) {
				odt = 3;  //50 ohms
			}
		}

	}	
        dcl = pci_conf1_read_config32(ctrl->f2, DRAM_CONFIG_LOW);
        dcl &= ~(DCL_DramTerm_MASK<<DCL_DramTerm_SHIFT);
        dcl |= (odt & DCL_DramTerm_MASK) << (DCL_DramTerm_SHIFT);
        pci_conf1_write_config32(ctrl->f2, DRAM_CONFIG_LOW, dcl);
}


void set_ecc(const struct mem_controller *ctrl,const struct mem_param *param, long dimm_mask, struct mem_info *meminfo)
{
	int i;
	int value;
	int ECC_memory = 1;
	
        u32 dcl, nbcap;
        nbcap = pci_conf1_read_config32(ctrl->f3, NORTHBRIDGE_CAP);
        dcl = pci_conf1_read_config32(ctrl->f2, DRAM_CONFIG_LOW);
        dcl &= ~DCL_DimmEccEn;
        if (nbcap & NBCAP_ECC) {
                dcl |= DCL_DimmEccEn;
        }
        get_option(&ECC_memory, "ECC_memory");
	if (ECC_memory) {
                dcl &= ~DCL_DimmEccEn;
        }
        pci_conf1_write_config32(ctrl->f2, DRAM_CONFIG_LOW, dcl);

	meminfo->is_ecc = 1;
	if(!(dcl & DCL_DimmEccEn)) {
		meminfo->is_ecc = 0;
		return; // already disabled the ECC, so don't need to read SPD any more
	}

        for(i = 0; i < DIMM_SOCKETS; i++) {
		
                if (!(dimm_mask & (1 << i))) {
                        continue;
                }

		value = spd_read_byte(ctrl->channel0[i], SPD_DIMM_CONF_TYPE);

		if(!(value & SPD_DIMM_CONF_TYPE_ECC)) {
	                dcl &= ~DCL_DimmEccEn;
	                pci_conf1_write_config32(ctrl->f2, DRAM_CONFIG_LOW, dcl);
			meminfo->is_ecc = 0;
			return;
		}

        }
}

int update_dimm_Twtr(const struct mem_controller *ctrl, const struct mem_param *param, int i)
{

	return update_dimm_TT_1_4(ctrl, param, i, DRAM_TIMING_HIGH, SPD_TWTR, DTH_TWTR_SHIFT, DTH_TWTR_MASK, DTH_TWTR_BASE, DTH_TWTR_MIN, DTH_TWTR_MAX);

}

void set_TT(const struct mem_controller *ctrl, const struct mem_param *param, unsigned TT_REG,
                unsigned TT_SHIFT, unsigned TT_MASK, unsigned TT_BASE, unsigned TT_MIN, unsigned TT_MAX, unsigned val, const char *str)
{
        u32 reg;

        if ((val < TT_MIN) || (val > TT_MAX)) {
                printk(BIOS_ERR, str);
                die(" Unknown\n");
        }

        reg = pci_conf1_read_config32(ctrl->f2, TT_REG);
        reg &= ~(TT_MASK << TT_SHIFT);
        reg |= ((val - TT_BASE) << TT_SHIFT);
        pci_conf1_write_config32(ctrl->f2, TT_REG, reg);
        return;
}

void set_TrwtTO(const struct mem_controller *ctrl, const struct mem_param *param)
{
	set_TT(ctrl, param, DRAM_TIMING_HIGH, DTH_TRWTTO_SHIFT, DTH_TRWTTO_MASK,DTH_TRWTTO_BASE, DTH_TRWTTO_MIN, DTH_TRWTTO_MAX, param->TrwtTO, "TrwtTO");
}

void set_Twrrd(const struct mem_controller *ctrl, const struct mem_param *param)
{
        set_TT(ctrl, param, DRAM_TIMING_HIGH, DTH_TWRRD_SHIFT, DTH_TWRRD_MASK,DTH_TWRRD_BASE, DTH_TWRRD_MIN, DTH_TWRRD_MAX, param->Twrrd, "Twrrd");
}

void set_Twrwr(const struct mem_controller *ctrl, const struct mem_param *param)
{
        set_TT(ctrl, param, DRAM_TIMING_HIGH, DTH_TWRWR_SHIFT, DTH_TWRWR_MASK,DTH_TWRWR_BASE, DTH_TWRWR_MIN, DTH_TWRWR_MAX, param->Twrwr, "Twrwr");
}

void set_Trdrd(const struct mem_controller *ctrl, const struct mem_param *param)
{
        set_TT(ctrl, param, DRAM_TIMING_HIGH, DTH_TRDRD_SHIFT, DTH_TRDRD_MASK,DTH_TRDRD_BASE, DTH_TRDRD_MIN, DTH_TRDRD_MAX, param->Trdrd, "Trdrd");
}

void set_DcqBypassMax(const struct mem_controller *ctrl, const struct mem_param *param)
{
        set_TT(ctrl, param, DRAM_CONFIG_HIGH, DCH_DcqBypassMax_SHIFT, DCH_DcqBypassMax_MASK,DCH_DcqBypassMax_BASE, DCH_DcqBypassMax_MIN, DCH_DcqBypassMax_MAX, param->DcqByPassMax, "DcqBypassMax"); // value need to be in CMOS
}

void set_Tfaw(const struct mem_controller *ctrl, const struct mem_param *param, struct mem_info *meminfo)
{
        static const u8 faw_1k[] = {8, 10, 13, 14};
        static const u8 faw_2k[] = {10, 14, 17, 18};
        unsigned memclkfreq_index;
        unsigned faw;


        memclkfreq_index = param->dch_memclk;

        if(meminfo->page_1k_mask != 0) { //1k page
                faw = faw_1k[memclkfreq_index];
        }
        else {
                faw = faw_2k[memclkfreq_index];
        }

        set_TT(ctrl, param, DRAM_CONFIG_HIGH, DCH_FourActWindow_SHIFT, DCH_FourActWindow_MASK, DCH_FourActWindow_BASE, DCH_FourActWindow_MIN, DCH_FourActWindow_MAX, faw, "FourActWindow");

}


void set_max_async_latency(const struct mem_controller *ctrl, const struct mem_param *param)
{
	u32 dch;
	unsigned async_lat;


	dch = pci_conf1_read_config32(ctrl->f2, DRAM_CONFIG_HIGH);
	dch &= ~(DCH_MaxAsyncLat_MASK << DCH_MaxAsyncLat_SHIFT);

	//FIXME: We need to use Max of DqsRcvEnDelay + 6ns here: After trainning and get that from index reg 0x10, 0x13, 0x16, 0x19, 0x30, 0x33, 0x36, 0x39
	async_lat = 6+6;
	
	
	dch |= ((async_lat - DCH_MaxAsyncLat_BASE) << DCH_MaxAsyncLat_SHIFT);
	pci_conf1_write_config32(ctrl->f2, DRAM_CONFIG_HIGH, dch);
}

void set_SlowAccessMode(const struct mem_controller *ctrl)
{
        u32 dch;

        dch = pci_conf1_read_config32(ctrl->f2, DRAM_CONFIG_HIGH);

        dch |= (1<<20);

        pci_conf1_write_config32(ctrl->f2, DRAM_CONFIG_HIGH, dch);
}


/*
	DRAM_OUTPUT_DRV_COMP_CTRL 0, 0x20
	DRAM_ADDR_TIMING_CTRL 04, 0x24
*/
void set_misc_timing(const struct mem_controller *ctrl, struct mem_info *meminfo)
{
        u32 dword;
	u32 dwordx;
	long dimm_mask = meminfo->dimm_mask & 0x0f;

#if DIMM_SUPPORT==0x0104   /* DDR2 and REG */
	/* for REG DIMM */	
        dword = 0x00111222;
        dwordx = 0x002f0000;
        switch (meminfo->memclk_set) {
        case DCH_MemClkFreq_266MHz:
                if( (dimm_mask == 0x03) || (dimm_mask == 0x02) || (dimm_mask == 0x01)) { 
                	dwordx = 0x002f2700; 
                }
                break;
        case DCH_MemClkFreq_333MHz:
	        if( (dimm_mask == 0x03) || (dimm_mask == 0x02) || (dimm_mask == 0x01)) {
                        if ((meminfo->single_rank_mask & 0x03)!=0x03) { //any double rank there?
                                dwordx = 0x002f2f00;
                        }
                }
                break;
        case DCH_MemClkFreq_400MHz:
                dwordx = 0x002f3300;
                break;
	}

#endif

#if DIMM_SUPPORT==0x0004  /* DDR2 and unbuffered */
 	unsigned SlowAccessMode = 0;
       /* for UNBUF DIMM */
        dword = 0x00111222;
	dwordx = 0x002f2f00;
	switch (meminfo->memclk_set) {
	case DCH_MemClkFreq_200MHz:
		if(dimm_mask == 0x03) {
			SlowAccessMode = 1;
			dword = 0x00111322;
		}
		break;
	case DCH_MemClkFreq_266MHz:
		if(dimm_mask == 0x03) {
			SlowAccessMode = 1;
			dword = 0x00111322;
			if((meminfo->x4_mask == 0 ) && (meminfo->x16_mask == 0)) {
				switch (meminfo->single_rank_mask) {
				case 0x03:
					dwordx = 0x00002f00; //x8 single Rank
					break;
				case 0x00:
					dwordx = 0x00342f00; //x8 double Rank
					break;
				default: 
					dwordx = 0x00372f00; //x8 single Rank and double Rank mixed
				}
			} else if((meminfo->x4_mask == 0 ) && (meminfo->x16_mask == 0x01) && (meminfo->single_rank_mask == 0x01)) {
                                        dwordx = 0x00382f00; //x8 Double Rank and x16 single Rank mixed
                        } else if((meminfo->x4_mask == 0 ) && (meminfo->x16_mask == 0x02) && (meminfo->single_rank_mask == 0x02)) {
                                        dwordx = 0x00382f00; //x16 single Rank and x8 double Rank mixed
			}

		}
		else {
			if((meminfo->x4_mask == 0 ) && (meminfo->x16_mask == 0x00) && ((meminfo->single_rank_mask == 0x01)||(meminfo->single_rank_mask == 0x02)))  { //x8 single rank
				dwordx = 0x002f2f00;
			} else {
				dwordx = 0x002b2f00;
			}
		}
		break;
	case DCH_MemClkFreq_333MHz:
		dwordx = 0x00202220;
                if(dimm_mask == 0x03) {
                        SlowAccessMode = 1;
                        dword = 0x00111322;
                        if((meminfo->x4_mask == 0 ) && (meminfo->x16_mask == 0)) {
                                switch (meminfo->single_rank_mask) {
                                case 0x03:
                                        dwordx = 0x00302220; //x8 single Rank
                                        break;
                                case 0x00:
                                        dwordx = 0x002b2220; //x8 double Rank
                                        break;
                                default:
                                        dwordx = 0x002a2220; //x8 single Rank and double Rank mixed
                                }
                        } else if((meminfo->x4_mask == 0) && (meminfo->x16_mask == 0x01) && (meminfo->single_rank_mask == 0x01)) {
                                        dwordx = 0x002c2220; //x8 Double Rank and x16 single Rank mixed
                        } else if((meminfo->x4_mask == 0) && (meminfo->x16_mask == 0x02) && (meminfo->single_rank_mask == 0x02)) {
                                        dwordx = 0x002c2220; //x16 single Rank and x8 double Rank mixed
                        }
                }
                break;
	case DCH_MemClkFreq_400MHz:
                dwordx = 0x00202520;
		SlowAccessMode = 1;
                if(dimm_mask == 0x03) {
                        dword = 0x00113322;
                } else {
                        dword = 0x00113222;
		}
                break;
	} 

	printk(BIOS_DEBUG, "\tdimm_mask =  0x%x\n",  meminfo->dimm_mask);
	printk(BIOS_DEBUG, "\tx4_mask =  0x%x\n",  meminfo->x4_mask);
	printk(BIOS_DEBUG, "\tx16_mask =  0x%x\n",  meminfo->x16_mask);
	printk(BIOS_DEBUG, "\tsingle_rank_mask =  0x%x\n",  meminfo->single_rank_mask);
	printk(BIOS_DEBUG, "\tODC =  0x%x\n",  dword);
	printk(BIOS_DEBUG, "\tAddr Timing=  0x%x\n",  dwordx);
#endif

#if (DIMM_SUPPORT & 0x0100)==0x0000 /* 2T mode only used for unbuffered DIMM */
	if(SlowAccessMode) {
		set_SlowAccessMode(ctrl);
	}
#endif

        /* Program the Output Driver Compensation Control Registers (Function 2:Offset 0x9c, index 0, 0x20) */
        pci_write_config32_index_wait(ctrl->f2, 0x98, 0, dword);
	if(meminfo->is_Width128) {	
	        pci_write_config32_index_wait(ctrl->f2, 0x98, 0x20, dword);
	}

        /* Program the Address Timing Control Registers (Function 2:Offset 0x9c, index 4, 0x24) */
        pci_write_config32_index_wait(ctrl->f2, 0x98, 4, dwordx);
	if(meminfo->is_Width128) {
	        pci_write_config32_index_wait(ctrl->f2, 0x98, 0x24, dwordx);
	}

}


void set_RDqsEn(const struct mem_controller *ctrl, const struct mem_param *param, struct mem_info *meminfo)
{
#if CPU_SOCKET_TYPE==SOCKET_L1
	//only need to set for reg and x8
        u32 dch;

        dch = pci_conf1_read_config32(ctrl->f2, DRAM_CONFIG_HIGH);

	dch &= ~DCH_RDqsEn;
	if((!meminfo->x4_mask) && (!meminfo->x16_mask)) {
	        dch |= DCH_RDqsEn;
	}

       	pci_conf1_write_config32(ctrl->f2, DRAM_CONFIG_HIGH, dch);
#endif
}


void set_idle_cycle_limit(const struct mem_controller *ctrl, const struct mem_param *param)
{
	u32 dcm;
	/* AMD says to Hardcode this */
	dcm = pci_conf1_read_config32(ctrl->f2, DRAM_CTRL_MISC);
	dcm &= ~(DCM_ILD_lmt_MASK << DCM_ILD_lmt_SHIFT);
	dcm |= DCM_ILD_lmt_16 << DCM_ILD_lmt_SHIFT;
	dcm |= DCM_DCC_EN;
	pci_conf1_write_config32(ctrl->f2, DRAM_CTRL_MISC, dcm);
}

void set_RdWrQByp(const struct mem_controller *ctrl, const struct mem_param *param)
{
	set_TT(ctrl, param, DRAM_CTRL_MISC, DCM_RdWrQByp_SHIFT, DCM_RdWrQByp_MASK,0, 0, 3, 2, "RdWrQByp");
}



long spd_set_dram_timing(const struct mem_controller *ctrl, const struct mem_param *param, long dimm_mask, struct mem_info *meminfo)
{
	int i;

	for(i = 0; i < DIMM_SOCKETS; i++) {
		int rc;
		if (!(dimm_mask & (1 << i))) {
			continue;
		}
		printk(BIOS_DEBUG, "%s: 0x%x\n", "dimm socket: ",  i);
		/* DRAM Timing Low Register */
		printk(BIOS_DEBUG, "\ttrc\n");
		if ((rc = update_dimm_Trc (ctrl, param, i)) <= 0) goto dimm_err;

		printk(BIOS_DEBUG, "\ttrcd\n");
		if ((rc = update_dimm_Trcd(ctrl, param, i)) <= 0) goto dimm_err;

		printk(BIOS_DEBUG, "\ttrrd\n");
		if ((rc = update_dimm_Trrd(ctrl, param, i)) <= 0) goto dimm_err;

		printk(BIOS_DEBUG, "\ttras\n");
		if ((rc = update_dimm_Tras(ctrl, param, i)) <= 0) goto dimm_err;

		printk(BIOS_DEBUG, "\ttrp\n");
		if ((rc = update_dimm_Trp (ctrl, param, i)) <= 0) goto dimm_err;

		printk(BIOS_DEBUG, "\ttrtp\n");
		if ((rc = update_dimm_Trtp(ctrl, param, i, meminfo)) <= 0) goto dimm_err;

		printk(BIOS_DEBUG, "\ttwr\n");
		if ((rc = update_dimm_Twr (ctrl, param, i)) <= 0) goto dimm_err;

		/* DRAM Timing High Register */
		printk(BIOS_DEBUG, "\ttref\n");
		if ((rc = update_dimm_Tref(ctrl, param, i)) <= 0) goto dimm_err;

		printk(BIOS_DEBUG, "\ttwtr\n");
		if ((rc = update_dimm_Twtr(ctrl, param, i)) <= 0) goto dimm_err;

		printk(BIOS_DEBUG, "\ttrfc\n");
		if ((rc = update_dimm_Trfc(ctrl, param, i, meminfo)) <= 0) goto dimm_err;

		/* DRAM Config Low */

		continue;
	dimm_err:
		if (rc < 0) {
			return -1;
		}
		dimm_mask = disable_dimm(ctrl, i, dimm_mask, meminfo);
	}

	meminfo->dimm_mask = dimm_mask; // store final dimm_mask

	get_extra_dimm_mask(ctrl, meminfo); // will be used by RDqsEn and dimm_x4
	/* DRAM Timing Low Register */

	/* DRAM Timing High Register */
	set_TrwtTO(ctrl, param);
	set_Twrrd (ctrl, param);
	set_Twrwr (ctrl, param);
	set_Trdrd (ctrl, param);
        
	set_4RankRDimm(ctrl, param, meminfo);

	/* DRAM Config High */
	set_Tfaw(ctrl, param, meminfo);
	set_DcqBypassMax(ctrl, param);
	set_max_async_latency(ctrl, param);
	set_RDqsEn(ctrl, param, meminfo);

	/* DRAM Config Low */
	set_ecc(ctrl, param, dimm_mask, meminfo);
	set_dimm_x4(ctrl, param, meminfo);
	set_DramTerm(ctrl, param, meminfo);
	
	/* DRAM Control Misc */
	set_idle_cycle_limit(ctrl, param);
	set_RdWrQByp(ctrl, param);

	return dimm_mask;
}

void sdram_set_spd_registers(const struct mem_controller *ctrl, struct sys_info *sysinfo) 
{
	struct spd_set_memclk_result result;
	const struct mem_param *param;
	struct mem_param paramx;
	struct mem_info *meminfo;
	long dimm_mask;
	void activate_spd_rom(const struct mem_controller *ctrl);
	void hard_reset(void);
#if 1
	if (!sysinfo->ctrl_present[ctrl->node_id]) {
		return;
	}
#endif
	meminfo = &sysinfo->meminfo[ctrl->node_id];

	printk(BIOS_DEBUG, "FIXME sdram_set_spd_registers: paramx :%p\n", &paramx);
	
	activate_spd_rom(ctrl);
	dimm_mask = spd_detect_dimms(ctrl);
	if (!(dimm_mask & ((1 << DIMM_SOCKETS) - 1))) {
		printk(BIOS_DEBUG, "No memory for this cpu\n");
		return;
	}
	dimm_mask = spd_enable_2channels(ctrl, dimm_mask, meminfo);        
	if (dimm_mask < 0) 
		goto hw_spd_err;
	dimm_mask = spd_set_ram_size(ctrl , dimm_mask, meminfo);           
	if (dimm_mask < 0) 
		goto hw_spd_err;
	dimm_mask = spd_handle_unbuffered_dimms(ctrl, dimm_mask, meminfo); 
	if (dimm_mask < 0) 
		goto hw_spd_err;
	result = spd_set_memclk(ctrl, dimm_mask, meminfo);
	param     = result.param;
	dimm_mask = result.dimm_mask;
	if (dimm_mask < 0) 
		goto hw_spd_err;

	//store memclk set to sysinfo, incase we need rebuilt param again
	meminfo->memclk_set = param->dch_memclk;

	memcpy(&paramx, param, sizeof(paramx));
	
	paramx.divisor = get_exact_divisor(param->dch_memclk, paramx.divisor);

	dimm_mask = spd_set_dram_timing(ctrl, &paramx , dimm_mask, meminfo); // dimm_mask will be stored to meminfo->dimm_mask
	if (dimm_mask < 0)
		goto hw_spd_err;
	
	order_dimms(ctrl, meminfo);

	return;
 hw_spd_err:
	/* Unrecoverable error reading SPD data */
	printk(BIOS_ERR, "SPD error - reset\n");
	hard_reset();
	return;
}

#define TIMEOUT_LOOPS 300000

//#include "raminit_f_dqs.c"

#if CONFIG_HW_MEM_HOLE_SIZEK != 0
u32 hoist_memory(int controllers, const struct mem_controller *ctrl,unsigned hole_startk, int i)
{
        int ii;
        u32 carry_over;
        u32 dev;
        u32 base, limit;
        u32 basek;
        u32 hoist;
	int j;

        carry_over = (4*1024*1024) - hole_startk;

        for(ii=controllers - 1;ii>i;ii--) {
                base  = pci_conf1_read_config32(ctrl[0].f1, 0x40 + (ii << 3));
                if ((base & ((1<<1)|(1<<0))) != ((1<<1)|(1<<0))) {
                        continue;
                }
		limit = pci_conf1_read_config32(ctrl[0].f1, 0x44 + (ii << 3));
		limit += (carry_over << 2 );
		base  += (carry_over << 2 );
		for(j = 0; j < controllers; j++) {	
                	pci_conf1_write_config32(ctrl[j].f1, 0x44 + (ii << 3), limit);
                	pci_conf1_write_config32(ctrl[j].f1, 0x40 + (ii << 3), base );
		}
        }
        limit = pci_conf1_read_config32(ctrl[0].f1, 0x44 + (i << 3));
	limit += (carry_over << 2);
	for(j = 0; j < controllers; j++) {
	        pci_conf1_write_config32(ctrl[j].f1, 0x44 + (i << 3), limit);
	}
        dev = ctrl[i].f1;
        base  = pci_conf1_read_config32(dev, 0x40 + (i << 3));
        basek  = (base & 0xffff0000) >> 2;
        if(basek == hole_startk) {
                //don't need set memhole here, because hole off set will be 0, overflow
                //so need to change base reg instead, new basek will be 4*1024*1024
                base &= 0x0000ffff;
                base |= (4*1024*1024)<<2;
		for(j = 0; j < controllers; j++) {
	                pci_conf1_write_config32(ctrl[j].f1, 0x40 + (i<<3), base);
		}
        }
        else
        {
        	hoist = /* hole start address */
	                ((hole_startk << 10) & 0xff000000) +
        	        /* hole address to memory controller address */
	                (((basek + carry_over) >> 6) & 0x0000ff00) +
        	        /* enable */
	                1;
        	pci_conf1_write_config32(dev, 0xf0, hoist);
	}	

        return carry_over;
}

void set_hw_mem_hole(int controllers, const struct mem_controller *ctrl)
{

        u32 hole_startk;
	int i;

        hole_startk = 4*1024*1024 - CONFIG_HW_MEM_HOLE_SIZEK;

#if HW_MEM_HOLE_SIZE_AUTO_INC == 1
	//We need to double check if the hole_startk is valid, if it is equal to basek, we need to decrease it some
	u32 basek_pri = 0;
        for(i=0; i<controllers; i++) {
                        u32 base;
                        u32 base_k;
                        base  = pci_conf1_read_config32(ctrl[0].f1, 0x40 + (i << 3));
                        if ((base & ((1<<1)|(1<<0))) != ((1<<1)|(1<<0))) {
                                continue;
                        }
                        base_k = (base & 0xffff0000) >> 2;
                        if(base_k == hole_startk) {
                                hole_startk -= (base_k - basek_pri)>>1; // decrease mem hole startk to make sure it is on middle of previous node
                                break; //only one hole
                        }
			basek_pri = base_k;
        }
#endif
        //find node index that need do set hole
        for(i=0; i<controllers; i++) {
                        u32 base, limit;
                        unsigned base_k, limit_k;
                        base  = pci_conf1_read_config32(ctrl[0].f1, 0x40 + (i << 3));
                        if ((base & ((1<<1)|(1<<0))) != ((1<<1)|(1<<0))) {
                                continue;
                        }
                        limit = pci_conf1_read_config32(ctrl[0].f1, 0x44 + (i << 3));
                        base_k = (base & 0xffff0000) >> 2;
                        limit_k = ((limit + 0x00010000) & 0xffff0000) >> 2;
                        if ((base_k <= hole_startk) && (limit_k > hole_startk)) {
                                unsigned end_k;
                                hoist_memory(controllers, ctrl, hole_startk, i);
				end_k = memory_end_k(ctrl, controllers);
                                set_top_mem(end_k, hole_startk);
				break; //only one hole
                        }
        }

}

#endif
static void wait_all_core0_mem_trained(struct sys_info *sysinfo)
{

        int i;
        u32 mask = 0;
	unsigned int needs_reset = 0;
	void hard_reset(void);

	if(sysinfo->nodes == 1) return; // in case only one cpu installed	

        for(i=1; i<sysinfo->nodes; i++) {
                /* Skip everything if I don't have any memory on this controller */
                if(sysinfo->mem_trained[i]==0x00) continue;

                mask |= (1<<i);

        }

        i = 1;
        while(1) {
		if(mask & (1<<i)) {
			if((sysinfo->mem_trained[i])!=0x80) {
				mask &= ~(1<<i);
			}
		}

                if(!mask) break;

                i++;
                i%=sysinfo->nodes;
	}

	for(i=0; i<sysinfo->nodes; i++) {
		printk(BIOS_DEBUG, "mem_trained[%02x]=%02x\n", i, sysinfo->mem_trained[i]); 
                switch(sysinfo->mem_trained[i]) {
		case 0: //don't need train
		case 1: //trained
			break;
		case 0x81: //recv1: fail
		case 0x82: //Pos :fail
		case 0x83: //recv2: fail
			needs_reset = 1;
			break;
		}
	}
	if(needs_reset) {
		printk(BIOS_DEBUG, "mem trained failed\n"); 
		hard_reset();
	}

}

void sdram_enable(int controllers, const struct mem_controller *ctrl, struct sys_info *sysinfo)
{
	int i;
	void dqs_timing(int controllers, const struct mem_controller *ctrl, struct sys_info *sysinfo);
	void memreset(int controllers, const struct mem_controller *ctrl);

	/* Error if I don't have memory */
	if (memory_end_k(ctrl, controllers) == 0) {
		die("No memory\n");
	}

	/* Before enabling memory start the memory clocks */
	for(i = 0; i < controllers; i++) {
		u32 dch;
		if (!sysinfo->ctrl_present[ i ])
			continue;
                dch = pci_conf1_read_config32(ctrl[i].f2, DRAM_CONFIG_HIGH);

		// if no memory installed, disabled the interface
		if(sysinfo->meminfo[i].dimm_mask==0x00){
                        dch |= DCH_DisDramInterface;
                        pci_conf1_write_config32(ctrl[i].f2, DRAM_CONFIG_HIGH, dch);

                }
		else {
                        dch |= DCH_MemClkFreqVal;
                        pci_conf1_write_config32(ctrl[i].f2, DRAM_CONFIG_HIGH, dch);
                	/* address timing and Output driver comp Control */
	                set_misc_timing(ctrl+i, sysinfo->meminfo+i );
		}
	}

	/* We need to wait a mimmium of 20 MEMCLKS to enable the  InitDram */
	memreset(controllers, ctrl);
#if 1
	printk(BIOS_DEBUG, "prepare to InitDram:");
	for(i=0; i<10; i++) {
		printk(BIOS_DEBUG, "%08x\b\b\b\b\b\b\b\b", i);
	}
	printk(BIOS_DEBUG, "\n");
#endif

	for(i = 0; i < controllers; i++) {
		u32 dcl, dch;
		if (!sysinfo->ctrl_present[ i ])
			continue;
		/* Skip everything if I don't have any memory on this controller */
		dch = pci_conf1_read_config32(ctrl[i].f2, DRAM_CONFIG_HIGH);
		if (!(dch & DCH_MemClkFreqVal)) {
			continue;
		}

		/* ChipKill */
		dcl = pci_conf1_read_config32(ctrl[i].f2, DRAM_CONFIG_LOW);
		if (dcl & DCL_DimmEccEn) {
			u32 mnc;
			printk(BIOS_SPEW, "ECC enabled\n");
			mnc = pci_conf1_read_config32(ctrl[i].f3, MCA_NB_CONFIG);
			mnc |= MNC_ECC_EN;
			if (dcl & DCL_Width128) {
				mnc |= MNC_CHIPKILL_EN;
			}
			pci_conf1_write_config32(ctrl[i].f3, MCA_NB_CONFIG, mnc);
		}

#if 0
                               /* Set the DqsRcvEnTrain bit */
                        dword = pci_conf1_read_config32(ctrl[i].f2, DRAM_CTRL);
                        dword |= DC_DqsRcvEnTrain;
                        pci_conf1_write_config32(ctrl[i].f2, DRAM_CTRL, dword);
#endif

		pci_conf1_write_config32(ctrl[i].f2, DRAM_CONFIG_LOW, dcl);
		dcl |= DCL_InitDram;
		pci_conf1_write_config32(ctrl[i].f2, DRAM_CONFIG_LOW, dcl);

	}

	for(i = 0; i < controllers; i++) {
		u32 dcl, dcm;
		if (!sysinfo->ctrl_present[ i ])
			continue;
		/* Skip everything if I don't have any memory on this controller */
		if(sysinfo->meminfo[i].dimm_mask==0x00) continue;

		printk(BIOS_DEBUG, "Initializing memory: ");
		int loops = 0;
		do {
			dcl = pci_conf1_read_config32(ctrl[i].f2, DRAM_CONFIG_LOW);
			loops++;
			if ((loops & 1023) == 0) {
				printk(BIOS_DEBUG, ".");
			}
		} while(((dcl & DCL_InitDram) != 0) && (loops < TIMEOUT_LOOPS));
		if (loops >= TIMEOUT_LOOPS) {
			printk(BIOS_DEBUG, " failed\n");
			continue;
		}

		/* Wait until it is safe to touch memory */
		do {
			dcm = pci_conf1_read_config32(ctrl[i].f2, DRAM_CTRL_MISC);
		} while(((dcm & DCM_MemClrStatus) == 0) /* || ((dcm & DCM_DramEnabled) == 0)*/ );

		printk(BIOS_DEBUG, " done\n");
	}

#if CONFIG_HW_MEM_HOLE_SIZEK != 0
	 // init hw mem hole here
	/* DramHoleValid bit only can be set after MemClrStatus is set by Hardware */
	set_hw_mem_hole(controllers, ctrl);
#endif
	
        //store tom to sysinfo, and it will be used by dqs_timing
        {
                struct msr msr;
                //[1M, TOM)
                msr = rdmsr(TOP_MEM);
                sysinfo->tom_k = ((msr.hi<<24) | (msr.lo>>8))>>2;

                //[4G, TOM2)
                msr = rdmsr(TOP_MEM2);
                sysinfo->tom2_k = ((msr.hi<<24)| (msr.lo>>8))>>2;
        }

        for(i = 0; i < controllers; i++) {
                sysinfo->mem_trained[i] = 0;

                if (!sysinfo->ctrl_present[ i ])
                        continue;

                /* Skip everything if I don't have any memory on this controller */
                if(sysinfo->meminfo[i].dimm_mask==0x00) 
			continue;

                sysinfo->mem_trained[i] = 0x80; // mem need to be trained
        }


#if MEM_TRAIN_SEQ ==  0
	dqs_timing(controllers, ctrl, sysinfo);
#else

   #if MEM_TRAIN_SEQ == 2
           //need to enable mtrr, so dqs training could access the test address
        setup_mtrr_dqs(sysinfo->tom_k, sysinfo->tom2_k);
   #endif

        for(i = 0; i < controllers; i++) {
                /* Skip everything if I don't have any memory on this controller */
                if(sysinfo->mem_trained[i]!=0x80) 
			continue;

                dqs_timing(i, &ctrl[i], sysinfo);

   #if MEM_TRAIN_SEQ == 1
                break; // only train the first node with ram
   #endif
        }

   #if MEM_TRAIN_SEQ == 2
        clear_mtrr_dqs(sysinfo->tom2_k);
   #endif

#endif

#if MEM_TRAIN_SEQ != 1
	wait_all_core0_mem_trained(sysinfo);
#endif

}
void fill_mem_ctrl(int controllers, struct mem_controller *ctrl_a, const u16 *spd_addr)
{
	int i; 
	int j;
	struct mem_controller *ctrl;
        for(i=0;i<controllers; i++) {
                ctrl = &ctrl_a[i];
                ctrl->node_id = i;
                ctrl->f0 = PCI_BDF(0, 0x18+i, 0);
                ctrl->f1 = PCI_BDF(0, 0x18+i, 1);
                ctrl->f2 = PCI_BDF(0, 0x18+i, 2);
                ctrl->f3 = PCI_BDF(0, 0x18+i, 3);

		if(spd_addr == (void *)0) continue;

                for(j=0;j<DIMM_SOCKETS;j++) {
                        ctrl->channel0[j] = spd_addr[(i*2+0)*DIMM_SOCKETS + j];
                        ctrl->channel1[j] = spd_addr[(i*2+1)*DIMM_SOCKETS + j];
                }
        }
}

/**
 * initialize sdram. There was a "generic" function in v2 that was not really that useful. 
 * we break it out by northbridge now to accomodate the different peculiarities of 
 * different chipsets. 
 *@param controllers Number of controllers
 *@param ctlr array of memory controllers
 *@param sysinfo pointer to sysinfo struct. 
 */
void sdram_initialize(int controllers, const struct mem_controller *ctrl, struct sys_info *sysinfo)
{
	int i;
	/* Set the registers we can set once to reasonable values */
	for(i = 0; i < controllers; i++) {
		printk(BIOS_DEBUG, "Ram1.%d, ",i);

		sdram_set_registers(ctrl + i , sysinfo);
	}

	/* Now setup those things we can auto detect */
	for(i = 0; i < controllers; i++) {
		printk(BIOS_DEBUG, "Ram2.%d, ",i);
		sdram_set_spd_registers(ctrl + i , sysinfo);

	}

	/* Now that everything is setup enable the SDRAM.
	 * Some chipsets do the work for us while on others 
	 * we need to it by hand.
	 */
	printk(BIOS_DEBUG, "Ram3\n");

	sdram_enable(controllers, ctrl, sysinfo);

	printk(BIOS_DEBUG, "Ram4\n");

}
