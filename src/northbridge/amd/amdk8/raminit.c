/*
 * This file is part of the coreboot project.
 *
 * 2004.11 yhlu add 4 rank DIMM support
 * 2004.12 yhlu add D0 support
 * 2005.02 yhlu add E0 memory hole support
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <cpu/x86/cache.h>
#include <cpu/x86/lapic.h>
#include <cpu/x86/mtrr.h>
#include <cpu/x86/msr.h>
#include <cpu/amd/mtrr.h>
#include <lib.h>
#include <stdlib.h>
#include <arch/acpi.h>
#include <reset.h>
#include "raminit.h"
#include "amdk8.h"
#if IS_ENABLED(CONFIG_HAVE_OPTION_TABLE)
#include "option_table.h"
#endif

#include <arch/early_variables.h>
struct sys_info sysinfo_car CAR_GLOBAL;

void setup_resource_map(const unsigned int *register_values, int max)
{
	int i;
	for (i = 0; i < max; i += 3) {
		pci_devfn_t dev;
		unsigned where;
		unsigned long reg;
		dev = register_values[i] & ~0xfff;
		where = register_values[i] & 0xfff;
		reg = pci_read_config32(dev, where);
		reg &= register_values[i+1];
		reg |= register_values[i+2];
		pci_write_config32(dev, where, reg);
	}
}

static int controller_present(const struct mem_controller *ctrl)
{
	return pci_read_config32(ctrl->f0, 0) == 0x11001022;
}

#if IS_ENABLED(CONFIG_RAMINIT_SYSINFO)
void sdram_set_registers(const struct mem_controller *ctrl, struct sys_info *sysinfo)
#else
void sdram_set_registers(const struct mem_controller *ctrl)
#endif
{
	static const unsigned int register_values[] = {

	/* Careful set limit registers before base registers which
	   contain the enables */
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
	PCI_ADDR(0, 0x18, 1, 0x44), 0x0000f8f8, 0x00000000,
	PCI_ADDR(0, 0x18, 1, 0x4C), 0x0000f8f8, 0x00000001,
	PCI_ADDR(0, 0x18, 1, 0x54), 0x0000f8f8, 0x00000002,
	PCI_ADDR(0, 0x18, 1, 0x5C), 0x0000f8f8, 0x00000003,
	PCI_ADDR(0, 0x18, 1, 0x64), 0x0000f8f8, 0x00000004,
	PCI_ADDR(0, 0x18, 1, 0x6C), 0x0000f8f8, 0x00000005,
	PCI_ADDR(0, 0x18, 1, 0x74), 0x0000f8f8, 0x00000006,
	PCI_ADDR(0, 0x18, 1, 0x7C), 0x0000f8f8, 0x00000007,
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
	PCI_ADDR(0, 0x18, 1, 0x40), 0x0000f8fc, 0x00000000,
	PCI_ADDR(0, 0x18, 1, 0x48), 0x0000f8fc, 0x00000000,
	PCI_ADDR(0, 0x18, 1, 0x50), 0x0000f8fc, 0x00000000,
	PCI_ADDR(0, 0x18, 1, 0x58), 0x0000f8fc, 0x00000000,
	PCI_ADDR(0, 0x18, 1, 0x60), 0x0000f8fc, 0x00000000,
	PCI_ADDR(0, 0x18, 1, 0x68), 0x0000f8fc, 0x00000000,
	PCI_ADDR(0, 0x18, 1, 0x70), 0x0000f8fc, 0x00000000,
	PCI_ADDR(0, 0x18, 1, 0x78), 0x0000f8fc, 0x00000000,

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
	 * [ 8: 1] Reserved
	 * [15: 9] Base Address (19-13)
	 *	   An optimization used when all DIMM are the same size...
	 * [20:16] Reserved
	 * [31:21] Base Address (35-25)
	 *	   This field defines the top 11 addresses bit of a 40-bit
	 *	   address that define the memory address space.  These
	 *	   bits decode 32-MByte blocks of memory.
	 */
	PCI_ADDR(0, 0x18, 2, 0x40), 0x001f01fe, 0x00000000,
	PCI_ADDR(0, 0x18, 2, 0x44), 0x001f01fe, 0x00000000,
	PCI_ADDR(0, 0x18, 2, 0x48), 0x001f01fe, 0x00000000,
	PCI_ADDR(0, 0x18, 2, 0x4C), 0x001f01fe, 0x00000000,
	PCI_ADDR(0, 0x18, 2, 0x50), 0x001f01fe, 0x00000000,
	PCI_ADDR(0, 0x18, 2, 0x54), 0x001f01fe, 0x00000000,
	PCI_ADDR(0, 0x18, 2, 0x58), 0x001f01fe, 0x00000000,
	PCI_ADDR(0, 0x18, 2, 0x5C), 0x001f01fe, 0x00000000,
	/* DRAM CS Mask Address i Registers
	 * F2:0x60 i = 0
	 * F2:0x64 i = 1
	 * F2:0x68 i = 2
	 * F2:0x6C i = 3
	 * F2:0x70 i = 4
	 * F2:0x74 i = 5
	 * F2:0x78 i = 6
	 * F2:0x7C i = 7
	 * Select bits to exclude from comparison with the DRAM Base address register.
	 * [ 8: 0] Reserved
	 * [15: 9] Address Mask (19-13)
	 *	   Address to be excluded from the optimized case
	 * [20:16] Reserved
	 * [29:21] Address Mask (33-25)
	 *	   The bits with an address mask of 1 are excluded from address comparison
	 * [31:30] Reserved
	 *
	 */
	PCI_ADDR(0, 0x18, 2, 0x60), 0xC01f01ff, 0x00000000,
	PCI_ADDR(0, 0x18, 2, 0x64), 0xC01f01ff, 0x00000000,
	PCI_ADDR(0, 0x18, 2, 0x68), 0xC01f01ff, 0x00000000,
	PCI_ADDR(0, 0x18, 2, 0x6C), 0xC01f01ff, 0x00000000,
	PCI_ADDR(0, 0x18, 2, 0x70), 0xC01f01ff, 0x00000000,
	PCI_ADDR(0, 0x18, 2, 0x74), 0xC01f01ff, 0x00000000,
	PCI_ADDR(0, 0x18, 2, 0x78), 0xC01f01ff, 0x00000000,
	PCI_ADDR(0, 0x18, 2, 0x7C), 0xC01f01ff, 0x00000000,
	/* DRAM Bank Address Mapping Register
	 * F2:0x80
	 * Specify the memory module size
	 * [ 2: 0] CS1/0
	 * [ 6: 4] CS3/2
	 * [10: 8] CS5/4
	 * [14:12] CS7/6
	 *	   000 = 32Mbyte  (Rows = 12 & Col =  8)
	 *	   001 = 64Mbyte  (Rows = 12 & Col =  9)
	 *	   010 = 128Mbyte (Rows = 13 & Col =  9)|(Rows = 12 & Col = 10)
	 *	   011 = 256Mbyte (Rows = 13 & Col = 10)|(Rows = 12 & Col = 11)
	 *	   100 = 512Mbyte (Rows = 13 & Col = 11)|(Rows = 14 & Col = 10)
	 *	   101 = 1Gbyte	  (Rows = 14 & Col = 11)|(Rows = 13 & Col = 12)
	 *	   110 = 2Gbyte	  (Rows = 14 & Col = 12)
	 *	   111 = reserved
	 * [ 3: 3] Reserved
	 * [ 7: 7] Reserved
	 * [11:11] Reserved
	 * [31:15]
	 */
	PCI_ADDR(0, 0x18, 2, 0x80), 0xffff8888, 0x00000000,
	/* DRAM Timing Low Register
	 * F2:0x88
	 * [ 2: 0] Tcl (Cas# Latency, Cas# to read-data-valid)
	 *	   000 = reserved
	 *	   001 = CL 2
	 *	   010 = CL 3
	 *	   011 = reserved
	 *	   100 = reserved
	 *	   101 = CL 2.5
	 *	   110 = reserved
	 *	   111 = reserved
	 * [ 3: 3] Reserved
	 * [ 7: 4] Trc (Row Cycle Time, Ras#-active to Ras#-active/bank auto refresh)
	 *	   0000 =  7 bus clocks
	 *	   0001 =  8 bus clocks
	 *	   ...
	 *	   1110 = 21 bus clocks
	 *	   1111 = 22 bus clocks
	 * [11: 8] Trfc (Row refresh Cycle time, Auto-refresh-active to RAS#-active or RAS#auto-refresh)
	 *	   0000 = 9 bus clocks
	 *	   0010 = 10 bus clocks
	 *	   ....
	 *	   1110 = 23 bus clocks
	 *	   1111 = 24 bus clocks
	 * [14:12] Trcd (Ras#-active to Case#-read/write Delay)
	 *	   000 = reserved
	 *	   001 = reserved
	 *	   010 = 2 bus clocks
	 *	   011 = 3 bus clocks
	 *	   100 = 4 bus clocks
	 *	   101 = 5 bus clocks
	 *	   110 = 6 bus clocks
	 *	   111 = reserved
	 * [15:15] Reserved
	 * [18:16] Trrd (Ras# to Ras# Delay)
	 *	   000 = reserved
	 *	   001 = reserved
	 *	   010 = 2 bus clocks
	 *	   011 = 3 bus clocks
	 *	   100 = 4 bus clocks
	 *	   101 = reserved
	 *	   110 = reserved
	 *	   111 = reserved
	 * [19:19] Reserved
	 * [23:20] Tras (Minmum Ras# Active Time)
	 *	   0000 to 0100 = reserved
	 *	   0101 = 5 bus clocks
	 *	   ...
	 *	   1111 = 15 bus clocks
	 * [26:24] Trp (Row Precharge Time)
	 *	   000 = reserved
	 *	   001 = reserved
	 *	   010 = 2 bus clocks
	 *	   011 = 3 bus clocks
	 *	   100 = 4 bus clocks
	 *	   101 = 5 bus clocks
	 *	   110 = 6 bus clocks
	 *	   111 = reserved
	 * [27:27] Reserved
	 * [28:28] Twr (Write Recovery Time)
	 *	   0 = 2 bus clocks
	 *	   1 = 3 bus clocks
	 * [31:29] Reserved
	 */
	PCI_ADDR(0, 0x18, 2, 0x88), 0xe8088008, 0x02522001 /* 0x03623125 */ ,
	/* DRAM Timing High Register
	 * F2:0x8C
	 * [ 0: 0] Twtr (Write to Read Delay)
	 *	   0 = 1 bus Clocks
	 *	   1 = 2 bus Clocks
	 * [ 3: 1] Reserved
	 * [ 6: 4] Trwt (Read to Write Delay)
	 *	   000 = 1 bus clocks
	 *	   001 = 2 bus clocks
	 *	   010 = 3 bus clocks
	 *	   011 = 4 bus clocks
	 *	   100 = 5 bus clocks
	 *	   101 = 6 bus clocks
	 *	   110 = reserved
	 *	   111 = reserved
	 * [ 7: 7] Reserved
	 * [12: 8] Tref (Refresh Rate)
	 *	   00000 = 100MHz 4K rows
	 *	   00001 = 133MHz 4K rows
	 *	   00010 = 166MHz 4K rows
	 *	   00011 = 200MHz 4K rows
	 *	   01000 = 100MHz 8K/16K rows
	 *	   01001 = 133MHz 8K/16K rows
	 *	   01010 = 166MHz 8K/16K rows
	 *	   01011 = 200MHz 8K/16K rows
	 * [19:13] Reserved
	 * [22:20] Twcl (Write CAS Latency)
	 *	   000 = 1 Mem clock after CAS# (Unbuffered Dimms)
	 *	   001 = 2 Mem clocks after CAS# (Registered Dimms)
	 * [31:23] Reserved
	 */
	PCI_ADDR(0, 0x18, 2, 0x8c), 0xff8fe08e, (0 << 20)|(0 << 8)|(0 << 4)|(0 << 0),
	/* DRAM Config Low Register
	 * F2:0x90
	 * [ 0: 0] DLL Disable
	 *	   0 = Enabled
	 *	   1 = Disabled
	 * [ 1: 1] D_DRV
	 *	   0 = Normal Drive
	 *	   1 = Weak Drive
	 * [ 2: 2] QFC_EN
	 *	   0 = Disabled
	 *	   1 = Enabled
	 * [ 3: 3] Disable DQS Hystersis  (FIXME handle this one carefully)
	 *	   0 = Enable DQS input filter
	 *	   1 = Disable DQS input filtering
	 * [ 7: 4] Reserved
	 * [ 8: 8] DRAM_Init
	 *	   0 = Initialization done or not yet started.
	 *	   1 = Initiate DRAM intialization sequence
	 * [ 9: 9] SO-Dimm Enable
	 *	   0 = Do nothing
	 *	   1 = SO-Dimms present
	 * [10:10] DramEnable
	 *	   0 = DRAM not enabled
	 *	   1 = DRAM initialized and enabled
	 * [11:11] Memory Clear Status
	 *	   0 = Memory Clear function has not completed
	 *	   1 = Memory Clear function has completed
	 * [12:12] Exit Self-Refresh
	 *	   0 = Exit from self-refresh done or not yet started
	 *	   1 = DRAM exiting from self refresh
	 * [13:13] Self-Refresh Status
	 *	   0 = Normal Operation
	 *	   1 = Self-refresh mode active
	 * [15:14] Read/Write Queue Bypass Count
	 *	   00 = 2
	 *	   01 = 4
	 *	   10 = 8
	 *	   11 = 16
	 * [16:16] 128-bit/64-Bit
	 *	   0 = 64bit Interface to DRAM
	 *	   1 = 128bit Interface to DRAM
	 * [17:17] DIMM ECC Enable
	 *	   0 = Some DIMMs do not have ECC
	 *	   1 = ALL DIMMS have ECC bits
	 * [18:18] UnBuffered DIMMs
	 *	   0 = Buffered DIMMS
	 *	   1 = Unbuffered DIMMS
	 * [19:19] Enable 32-Byte Granularity
	 *	   0 = Optimize for 64byte bursts
	 *	   1 = Optimize for 32byte bursts
	 * [20:20] DIMM 0 is x4
	 * [21:21] DIMM 1 is x4
	 * [22:22] DIMM 2 is x4
	 * [23:23] DIMM 3 is x4
	 *	   0 = DIMM is not x4
	 *	   1 = x4 DIMM present
	 * [24:24] Disable DRAM Receivers
	 *	   0 = Receivers enabled
	 *	   1 = Receivers disabled
	 * [27:25] Bypass Max
	 *	   000 = Arbiters chois is always respected
	 *	   001 = Oldest entry in DCQ can be bypassed 1 time
	 *	   010 = Oldest entry in DCQ can be bypassed 2 times
	 *	   011 = Oldest entry in DCQ can be bypassed 3 times
	 *	   100 = Oldest entry in DCQ can be bypassed 4 times
	 *	   101 = Oldest entry in DCQ can be bypassed 5 times
	 *	   110 = Oldest entry in DCQ can be bypassed 6 times
	 *	   111 = Oldest entry in DCQ can be bypassed 7 times
	 * [31:28] Reserved
	 */
	PCI_ADDR(0, 0x18, 2, 0x90), 0xf0000000,
	(4 << 25)|(0 << 24)|
	(0 << 23)|(0 << 22)|(0 << 21)|(0 << 20)|
	(1 << 19)|(0 << 18)|(1 << 17)|(0 << 16)|
	(2 << 14)|(0 << 13)|(0 << 12)|
	(0 << 11)|(0 << 10)|(0 << 9)|(0 << 8)|
	(0 << 3) |(0 << 1) |(0 << 0),
	/* DRAM Config High Register
	 * F2:0x94
	 * [ 0: 3] Maximum Asynchronous Latency
	 *	   0000 = 0 ns
	 *	   ...
	 *	   1111 = 15 ns
	 * [ 7: 4] Reserved
	 * [11: 8] Read Preamble
	 *	   0000 = 2.0 ns
	 *	   0001 = 2.5 ns
	 *	   0010 = 3.0 ns
	 *	   0011 = 3.5 ns
	 *	   0100 = 4.0 ns
	 *	   0101 = 4.5 ns
	 *	   0110 = 5.0 ns
	 *	   0111 = 5.5 ns
	 *	   1000 = 6.0 ns
	 *	   1001 = 6.5 ns
	 *	   1010 = 7.0 ns
	 *	   1011 = 7.5 ns
	 *	   1100 = 8.0 ns
	 *	   1101 = 8.5 ns
	 *	   1110 = 9.0 ns
	 *	   1111 = 9.5 ns
	 * [15:12] Reserved
	 * [18:16] Idle Cycle Limit
	 *	   000 = 0 cycles
	 *	   001 = 4 cycles
	 *	   010 = 8 cycles
	 *	   011 = 16 cycles
	 *	   100 = 32 cycles
	 *	   101 = 64 cycles
	 *	   110 = 128 cycles
	 *	   111 = 256 cycles
	 * [19:19] Dynamic Idle Cycle Center Enable
	 *	   0 = Use Idle Cycle Limit
	 *	   1 = Generate a dynamic Idle cycle limit
	 * [22:20] DRAM MEMCLK Frequency
	 *	   000 = 100MHz
	 *	   001 = reserved
	 *	   010 = 133MHz
	 *	   011 = reserved
	 *	   100 = reserved
	 *	   101 = 166MHz
	 *	   110 = reserved
	 *	   111 = reserved
	 * [24:23] Reserved
	 * [25:25] Memory Clock Ratio Valid (FIXME carefully enable memclk)
	 *	   0 = Disable MemClks
	 *	   1 = Enable MemClks
	 * [26:26] Memory Clock 0 Enable
	 *	   0 = Disabled
	 *	   1 = Enabled
	 * [27:27] Memory Clock 1 Enable
	 *	   0 = Disabled
	 *	   1 = Enabled
	 * [28:28] Memory Clock 2 Enable
	 *	   0 = Disabled
	 *	   1 = Enabled
	 * [29:29] Memory Clock 3 Enable
	 *	   0 = Disabled
	 *	   1 = Enabled
	 * [31:30] Reserved
	 */
	PCI_ADDR(0, 0x18, 2, 0x94), 0xc180f0f0,
	(0 << 29)|(0 << 28)|(0 << 27)|(0 << 26)|(0 << 25)|
	(0 << 20)|(0 << 19)|(DCH_IDLE_LIMIT_16 << 16)|(0 << 8)|(0 << 0),
	/* DRAM Delay Line Register
	 * F2:0x98
	 * Adjust the skew of the input DQS strobe relative to DATA
	 * [15: 0] Reserved
	 * [23:16] Delay Line Adjust
	 *	   Adjusts the DLL derived PDL delay by one or more delay stages
	 *	   in either the faster or slower direction.
	 * [24:24} Adjust Slower
	 *	   0 = Do Nothing
	 *	   1 = Adj is used to increase the PDL delay
	 * [25:25] Adjust Faster
	 *	   0 = Do Nothing
	 *	   1 = Adj is used to decrease the PDL delay
	 * [31:26] Reserved
	 */
	PCI_ADDR(0, 0x18, 2, 0x98), 0xfc00ffff, 0x00000000,
	/* MCA NB Status Low reg */
	PCI_ADDR(0, 0x18, 3, 0x48), 0x00f00000, 0x00000000,
	/* MCA NB Status high reg */
	PCI_ADDR(0, 0x18, 3, 0x4c), 0x01801e8c, 0x00000000,
	/* MCA NB address Low reg */
	PCI_ADDR(0, 0x18, 3, 0x50), 0x00000007, 0x00000000,
	/* MCA NB address high reg */
	PCI_ADDR(0, 0x18, 3, 0x54), 0xffffff00, 0x00000000,
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
	PCI_ADDR(0, 0x18, 3, 0x58), 0xffe0e0e0, 0x00000000,
	/* DRAM Scrub Address Low Register
	 * F3:0x5C
	 * [ 0: 0] DRAM Scrubber Redirect Enable
	 *	   0 = Do nothing
	 *	   1 = Scrubber Corrects errors found in normal operation
	 * [ 5: 1] Reserved
	 * [31: 6] DRAM Scrub Address 31-6
	 */
	PCI_ADDR(0, 0x18, 3, 0x5C), 0x0000003e, 0x00000000,
	/* DRAM Scrub Address High Register
	 * F3:0x60
	 * [ 7: 0] DRAM Scrubb Address 39-32
	 * [31: 8] Reserved
	 */
	PCI_ADDR(0, 0x18, 3, 0x60), 0xffffff00, 0x00000000,
	};
	int i;
	int max;

	if (!controller_present(ctrl)) {
		return;
	}
	printk(BIOS_SPEW, "setting up CPU%02x northbridge registers\n", ctrl->node_id);
	max = ARRAY_SIZE(register_values);
	for (i = 0; i < max; i += 3) {
		pci_devfn_t dev;
		unsigned where;
		unsigned long reg;
		dev = (register_values[i] & ~0xfff) - PCI_DEV(0, 0x18, 0) + ctrl->f0;
		where = register_values[i] & 0xfff;
		reg = pci_read_config32(dev, where);
		reg &= register_values[i+1];
		reg |= register_values[i+2];
		pci_write_config32(dev, where, reg);
	}
	printk(BIOS_SPEW, "done.\n");
}

static void hw_enable_ecc(const struct mem_controller *ctrl)
{
	uint32_t dcl, nbcap;
	nbcap = pci_read_config32(ctrl->f3, NORTHBRIDGE_CAP);
	dcl = pci_read_config32(ctrl->f2, DRAM_CONFIG_LOW);
	dcl &= ~DCL_DimmEccEn;
	if (nbcap & NBCAP_ECC) {
		dcl |= DCL_DimmEccEn;
	}
	if (read_option(ECC_memory, 1) == 0) {
		dcl &= ~DCL_DimmEccEn;
	}
	pci_write_config32(ctrl->f2, DRAM_CONFIG_LOW, dcl);
}

static int is_dual_channel(const struct mem_controller *ctrl)
{
	uint32_t dcl;
	dcl = pci_read_config32(ctrl->f2, DRAM_CONFIG_LOW);
	return dcl & DCL_128BitEn;
}

static int is_opteron(const struct mem_controller *ctrl)
{
	/* Test to see if I am an Opteron.  Socket 939 based Athlon64
	 * have dual channel capability, too, so we need a better test
	 * for Opterons.
	 * However, all code uses is_opteron() to find out whether to
	 * use dual channel, so if we really check for opteron here, we
	 * need to fix up all code using this function, too.
	 */
	uint32_t nbcap;
	nbcap = pci_read_config32(ctrl->f3, NORTHBRIDGE_CAP);
	return !!(nbcap & NBCAP_128Bit);
}

static int is_registered(const struct mem_controller *ctrl)
{
	/* Test to see if we are dealing with registered SDRAM.
	 * If we are not registered we are unbuffered.
	 * This function must be called after spd_handle_unbuffered_dimms.
	 */
	uint32_t dcl;
	dcl = pci_read_config32(ctrl->f2, DRAM_CONFIG_LOW);
	return !(dcl & DCL_UnBuffDimm);
}

struct dimm_size {
	unsigned long side1;
	unsigned long side2;
	unsigned long rows;
	unsigned long col;
#if IS_ENABLED(CONFIG_QRANK_DIMM_SUPPORT)
	unsigned long rank;
#endif
};

static struct dimm_size spd_get_dimm_size(unsigned device)
{
	/* Calculate the log base 2 size of a DIMM in bits */
	struct dimm_size sz;
	int value, low;
	sz.side1 = 0;
	sz.side2 = 0;
	sz.rows = 0;
	sz.col = 0;
#if IS_ENABLED(CONFIG_QRANK_DIMM_SUPPORT)
	sz.rank = 0;
#endif

	/* Note it might be easier to use byte 31 here, it has the DIMM size as
	 * a multiple of 4MB.  The way we do it now we can size both
	 * sides of an assymetric dimm.
	 */
	value = spd_read_byte(device, 3);	/* rows */
	if (value < 0) goto hw_err;
	if ((value & 0xf) == 0) goto val_err;
	sz.side1 += value & 0xf;
	sz.rows = value & 0xf;

	value = spd_read_byte(device, 4);	/* columns */
	if (value < 0) goto hw_err;
	if ((value & 0xf) == 0) goto val_err;
	sz.side1 += value & 0xf;
	sz.col = value & 0xf;

	value = spd_read_byte(device, 17);	/* banks */
	if (value < 0) goto hw_err;
	if ((value & 0xff) == 0) goto val_err;
	sz.side1 += log2(value & 0xff);

	/* Get the module data width and convert it to a power of two */
	value = spd_read_byte(device, 7);	/* (high byte) */
	if (value < 0) goto hw_err;
	value &= 0xff;
	value <<= 8;

	low = spd_read_byte(device, 6);	/* (low byte) */
	if (low < 0) goto hw_err;
	value = value | (low & 0xff);
	if ((value != 72) && (value != 64)) goto val_err;
	sz.side1 += log2(value);

	/* side 2 */
	value = spd_read_byte(device, 5);	/* number of physical banks */
	if (value < 0) goto hw_err;
	if (value == 1) goto out;
	if ((value != 2) && (value != 4)) {
		goto val_err;
	}
#if IS_ENABLED(CONFIG_QRANK_DIMM_SUPPORT)
	sz.rank = value;
#endif

	/* Start with the symmetrical case */
	sz.side2 = sz.side1;

	value = spd_read_byte(device, 3);	/* rows */
	if (value < 0) goto hw_err;
	if ((value & 0xf0) == 0) goto out;	/* If symmetrical we are done */
	sz.side2 -= (value & 0x0f);		/* Subtract out rows on side 1 */
	sz.side2 += ((value >> 4) & 0x0f);	/* Add in rows on side 2 */

	value = spd_read_byte(device, 4);	/* columns */
	if (value < 0) goto hw_err;
	if ((value & 0xff) == 0) goto val_err;
	sz.side2 -= (value & 0x0f);		/* Subtract out columns on side 1 */
	sz.side2 += ((value >> 4) & 0x0f);	/* Add in columsn on side 2 */

	goto out;

 val_err:
	die("Bad SPD value\n");
	/* If an hw_error occurs report that I have no memory */
hw_err:
	sz.side1 = 0;
	sz.side2 = 0;
	sz.rows = 0;
	sz.col = 0;
#if IS_ENABLED(CONFIG_QRANK_DIMM_SUPPORT)
	sz.rank = 0;
#endif
out:
	return sz;
}


static void set_dimm_size(const struct mem_controller *ctrl, struct dimm_size sz, unsigned index)
{
	uint32_t base0, base1;
	uint32_t dch;

	if (sz.side1 != sz.side2) {
		sz.side2 = 0;
	}

	/* For each base register.
	 * Place the dimm size in 32 MB quantities in the bits 31 - 21.
	 * The initialize dimm size is in bits.
	 * Set the base enable bit0.
	 */

	base0 = base1 = 0;

	/* Make certain side1 of the dimm is at least 32MB */
	if (sz.side1 >= (25 +3)) {
		base0 = (1 << ((sz.side1 - (25 + 3)) + 21)) | 1;
	}

	/* Make certain side2 of the dimm is at least 32MB */
	if (sz.side2 >= (25 + 3)) {
		base1 = (1 << ((sz.side2 - (25 + 3)) + 21)) | 1;
	}

	/* Double the size if we are using dual channel memory */
	if (is_dual_channel(ctrl)) {
		base0 = (base0 << 1) | (base0 & 1);
		base1 = (base1 << 1) | (base1 & 1);
	}

	/* Clear the reserved bits */
	base0 &= ~0x001ffffe;
	base1 &= ~0x001ffffe;

	/* Set the appropriate DIMM base address register */
	pci_write_config32(ctrl->f2, DRAM_CSBASE + (((index << 1)+0)<<2), base0);
	pci_write_config32(ctrl->f2, DRAM_CSBASE + (((index << 1)+1)<<2), base1);
#if IS_ENABLED(CONFIG_QRANK_DIMM_SUPPORT)
	if (sz.rank == 4) {
		pci_write_config32(ctrl->f2, DRAM_CSBASE + (((index << 1)+4)<<2), base0);
		pci_write_config32(ctrl->f2, DRAM_CSBASE + (((index << 1)+5)<<2), base1);
	}
#endif

	/* Enable the memory clocks for this DIMM */
	if (base0) {
		dch = pci_read_config32(ctrl->f2, DRAM_CONFIG_HIGH);
		dch |= DCH_MEMCLK_EN0 << index;
#if IS_ENABLED(CONFIG_QRANK_DIMM_SUPPORT)
		if (sz.rank == 4) {
			dch |= DCH_MEMCLK_EN0 << (index + 2);
		}
#endif
		pci_write_config32(ctrl->f2, DRAM_CONFIG_HIGH, dch);
	}
}

static void set_dimm_map(const struct mem_controller *ctrl, struct dimm_size sz, unsigned index)
{
	static const unsigned cs_map_aa[] = {
		/* (row=12, col=8)(14, 12) ---> (0, 0) (2, 4) */
		0, 1, 3, 6, 0,
		0, 2, 4, 7, 9,
		0, 0, 5, 8,10,
	};

	uint32_t map;

	map = pci_read_config32(ctrl->f2, DRAM_BANK_ADDR_MAP);
	map &= ~(0xf << (index * 4));
#if IS_ENABLED(CONFIG_QRANK_DIMM_SUPPORT)
	if (sz.rank == 4) {
		map &= ~(0xf << ((index + 2) * 4));
	}
#endif


	/* Make certain side1 of the dimm is at least 32MB */
	if (sz.side1 >= (25 +3)) {
		if (is_cpu_pre_d0()) {
			map |= (sz.side1 - (25 + 3)) << (index *4);
#if IS_ENABLED(CONFIG_QRANK_DIMM_SUPPORT)
			if (sz.rank == 4) {
				map |= (sz.side1 - (25 + 3)) << ((index + 2) * 4);
			}
#endif
		}
		else {
			map |= cs_map_aa[(sz.rows - 12) * 5 + (sz.col - 8) ] << (index*4);
#if IS_ENABLED(CONFIG_QRANK_DIMM_SUPPORT)
			if (sz.rank == 4) {
				map |=  cs_map_aa[(sz.rows - 12) * 5 + (sz.col - 8) ] << ((index + 2) * 4);
			}
#endif
		}
	}

	pci_write_config32(ctrl->f2, DRAM_BANK_ADDR_MAP, map);

}

static long spd_set_ram_size(const struct mem_controller *ctrl, long dimm_mask)
{
	int i;

	for (i = 0; i < DIMM_SOCKETS; i++) {
		struct dimm_size sz;
		if (!(dimm_mask & (1 << i))) {
			continue;
		}
		sz = spd_get_dimm_size(ctrl->channel0[i]);
		if (sz.side1 == 0) {
			return -1; /* Report SPD error */
		}
		set_dimm_size(ctrl, sz, i);
		set_dimm_map (ctrl, sz, i);
	}
	return dimm_mask;
}

static void route_dram_accesses(const struct mem_controller *ctrl,
	unsigned long base_k, unsigned long limit_k)
{
	/* Route the addresses to the controller node */
	unsigned node_id;
	unsigned limit;
	unsigned base;
	unsigned index;
	unsigned limit_reg, base_reg;
	pci_devfn_t device;

	node_id = ctrl->node_id;
	index = (node_id << 3);
	limit = (limit_k << 2);
	limit &= 0xffff0000;
	limit -= 0x00010000;
	limit |= (0 << 8) | (node_id << 0);
	base = (base_k << 2);
	base &= 0xffff0000;
	base |= (0 << 8) | (1<<1) | (1<<0);

	limit_reg = 0x44 + index;
	base_reg = 0x40 + index;
	for (device = PCI_DEV(0, 0x18, 1); device <= PCI_DEV(0, 0x1f, 1); device += PCI_DEV(0, 1, 0)) {
		pci_write_config32(device, limit_reg, limit);
		pci_write_config32(device, base_reg, base);
	}
}

static void set_top_mem(unsigned tom_k, unsigned hole_startk)
{
	/* Error if I don't have memory */
	if (!tom_k) {
		die("No memory?");
	}

	/* Report the amount of memory. */
	printk(BIOS_DEBUG, "RAM end at 0x%08x kB\n", tom_k);

	/* Now set top of memory */
	msr_t msr;
	if (tom_k > (4*1024*1024)) {
		printk(BIOS_SPEW, "Handling memory mapped above 4 GB\n");
		printk(BIOS_SPEW, "Upper RAM end at 0x%08x kB\n", tom_k);
		msr.lo = (tom_k & 0x003fffff) << 10;
		msr.hi = (tom_k & 0xffc00000) >> 22;
		wrmsr(TOP_MEM2, msr);
		printk(BIOS_SPEW, "Correcting memory amount mapped below 4 GB\n");
	}

	/* Leave a 64M hole between TOP_MEM and TOP_MEM2
	 * so I can see my ROM chip and other I/O devices.
	 */
	if (tom_k >= 0x003f0000) {
#if CONFIG_HW_MEM_HOLE_SIZEK != 0
		if (hole_startk != 0) {
			tom_k = hole_startk;
		} else
#endif
		tom_k = 0x3f0000;
		printk(BIOS_SPEW, "Adjusting lower RAM end\n");
	}
	printk(BIOS_SPEW, "Lower RAM end at 0x%08x kB\n", tom_k);
	msr.lo = (tom_k & 0x003fffff) << 10;
	msr.hi = (tom_k & 0xffc00000) >> 22;
	wrmsr(TOP_MEM, msr);
}

static unsigned long interleave_chip_selects(const struct mem_controller *ctrl)
{
	/* 35 - 25 */
	static const uint8_t csbase_low_shift[] = {
	/* 32MB */	(13 - 4),
	/* 64MB */	(14 - 4),
	/* 128MB */	(14 - 4),
	/* 256MB */	(15 - 4),
	/* 512MB */	(15 - 4),
	/* 1GB */	(16 - 4),
	/* 2GB */	(16 - 4),
	};

	static const uint8_t csbase_low_d0_shift[] = {
	/* 32MB */      (13 - 4),
	/* 64MB */      (14 - 4),
	/* 128MB */     (14 - 4),
	/* 128MB */     (15 - 4),
	/* 256MB */     (15 - 4),
	/* 512MB */     (15 - 4),
	/* 256MB */     (16 - 4),
	/* 512MB */     (16 - 4),
	/* 1GB */       (16 - 4),
	/* 1GB */       (17 - 4),
	/* 2GB */       (17 - 4),
	};

	/* cs_base_high is not changed */

	uint32_t csbase_inc;
	int chip_selects, index;
	int bits;
	unsigned common_size;
	unsigned common_cs_mode;
	uint32_t csbase, csmask;

	/* See if all of the memory chip selects are the same size
	 * and if so count them.
	 */
	chip_selects = 0;
	common_size = 0;
	common_cs_mode = 0;
	for (index = 0; index < 8; index++) {
		unsigned size;
		unsigned cs_mode;
		uint32_t value;

		value = pci_read_config32(ctrl->f2, DRAM_CSBASE + (index << 2));

		/* Is it enabled? */
		if (!(value & 1)) {
			continue;
		}
		chip_selects++;
		size = value >> 21;
		if (common_size == 0) {
			common_size = size;
		}
		/* The size differed fail */
		if (common_size != size) {
			return 0;
		}

		value = pci_read_config32(ctrl->f2, DRAM_BANK_ADDR_MAP);
		cs_mode =(value >> ((index>>1)*4)) & 0xf;
		if (cs_mode == 0) continue;
		if (common_cs_mode == 0) {
			common_cs_mode = cs_mode;
		}
		/* The cs_mode differed fail */
		if (common_cs_mode != cs_mode) {
			return 0;
		}
	}

	/* Chip selects can only be interleaved when there is
	 * more than one and their is a power of two of them.
	 */
	bits = log2(chip_selects);
	if (((1 << bits) != chip_selects) || (bits < 1) || (bits > 3)) {
		return 0;
	}

	/* Find the bits of csbase that we need to interleave on */
	if (is_cpu_pre_d0()) {
		csbase_inc = 1 << csbase_low_shift[common_cs_mode];
		if (is_dual_channel(ctrl)) {
		/* Also we run out of address mask bits if we try and interleave 8 4GB dimms */
			if ((bits == 3) && (common_size == (1 << (32 - 3)))) {
				return 0;
			}
			csbase_inc <<=1;
		}
	}
	else {
		csbase_inc = 1 << csbase_low_d0_shift[common_cs_mode];
		if (is_dual_channel(ctrl)) {
			if ((bits == 3) && (common_cs_mode > 8)) {
				return 0;
			}
			csbase_inc <<=1;
		}
	}

	/* Compute the initial values for csbase and csbask.
	 * In csbase just set the enable bit and the base to zero.
	 * In csmask set the mask bits for the size and page level interleave.
	 */
	csbase = 0 | 1;
	csmask = (((common_size  << bits) - 1) << 21);
	csmask |= 0xfe00 & ~((csbase_inc << bits) - csbase_inc);
	for (index = 0; index < 8; index++) {
		uint32_t value;

		value = pci_read_config32(ctrl->f2, DRAM_CSBASE + (index << 2));
		/* Is it enabled? */
		if (!(value & 1)) {
			continue;
		}
		pci_write_config32(ctrl->f2, DRAM_CSBASE + (index << 2), csbase);
		pci_write_config32(ctrl->f2, DRAM_CSMASK + (index << 2), csmask);
		csbase += csbase_inc;
	}

	printk(BIOS_SPEW, "Interleaved\n");

	/* Return the memory size in K */
	return common_size << (15 + bits);
}

static unsigned long order_chip_selects(const struct mem_controller *ctrl)
{
	unsigned long tom;

	/* Remember which registers we have used in the high 8 bits of tom */
	tom = 0;
	for (;;) {
		/* Find the largest remaining candidate */
		unsigned index, candidate;
		uint32_t csbase, csmask;
		unsigned size;
		csbase = 0;
		candidate = 0;
		for (index = 0; index < 8; index++) {
			uint32_t value;
			value = pci_read_config32(ctrl->f2, DRAM_CSBASE + (index << 2));

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
		size = csbase >> 21;

		/* Remember I have used this register */
		tom |= (1 << (candidate + 24));

		/* Recompute the cs base register value */
		csbase = (tom << 21) | 1;

		/* Increment the top of memory */
		tom += size;

		/* Compute the memory mask */
		csmask = ((size -1) << 21);
		csmask |= 0xfe00;		/* For now don't optimize */

		/* Write the new base register */
		pci_write_config32(ctrl->f2, DRAM_CSBASE + (candidate << 2), csbase);
		/* Write the new mask register */
		pci_write_config32(ctrl->f2, DRAM_CSMASK + (candidate << 2), csmask);

	}
	/* Return the memory size in K */
	return (tom & ~0xff000000) << 15;
}

static unsigned long memory_end_k(const struct mem_controller *ctrl, int max_node_id)
{
	unsigned node_id;
	unsigned end_k;
	/* Find the last memory address used */
	end_k = 0;
	for (node_id = 0; node_id < max_node_id; node_id++) {
		uint32_t limit, base;
		unsigned index;
		index = node_id << 3;
		base = pci_read_config32(ctrl->f1, 0x40 + index);
		/* Only look at the limit if the base is enabled */
		if ((base & 3) == 3) {
			limit = pci_read_config32(ctrl->f1, 0x44 + index);
			end_k = ((limit + 0x00010000) & 0xffff0000) >> 2;
		}
	}
	return end_k;
}

static void order_dimms(const struct mem_controller *ctrl)
{
	unsigned long tom_k, base_k;

	if (read_option(interleave_chip_selects, 1) != 0) {
		tom_k = interleave_chip_selects(ctrl);
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

static long disable_dimm(const struct mem_controller *ctrl, unsigned index, long dimm_mask)
{
	printk(BIOS_DEBUG, "disabling dimm %02x\n", index);
	pci_write_config32(ctrl->f2, DRAM_CSBASE + (((index << 1)+0)<<2), 0);
	pci_write_config32(ctrl->f2, DRAM_CSBASE + (((index << 1)+1)<<2), 0);
	dimm_mask &= ~(1 << index);
	return dimm_mask;
}

static long spd_handle_unbuffered_dimms(const struct mem_controller *ctrl,
					long dimm_mask)
{
	int i;
	int registered;
	int unbuffered;
	int has_dualch = is_opteron(ctrl);
	uint32_t dcl;
	unbuffered = 0;
	registered = 0;
	for (i = 0; (i < DIMM_SOCKETS); i++) {
		int value;
		if (!(dimm_mask & (1 << i))) {
			continue;
		}
		value = spd_read_byte(ctrl->channel0[i], 21);
		if (value < 0) {
			return -1;
		}

		/* Registered dimm ? */
		if (value & (1 << 1)) {
			registered = 1;
		}
		/* Otherwise it must be an unbuffered dimm */
		else {
			unbuffered = 1;
		}
	}
	if (unbuffered && registered) {
		die("Mixed buffered and registered dimms not supported");
	}

	dcl = pci_read_config32(ctrl->f2, DRAM_CONFIG_LOW);
	dcl &= ~DCL_UnBuffDimm;
	if (unbuffered) {
		if ((has_dualch) && (!is_cpu_pre_d0())) {
			dcl |= DCL_UnBuffDimm;
#if IS_ENABLED(CONFIG_CPU_AMD_SOCKET_939)
			if ((cpuid_eax(1) & 0x30) == 0x30) {
				/* CS[7:4] is copy of CS[3:0], should be set for 939 socket */
				dcl |= DCL_UpperCSMap;
			}
#endif
		} else {
			dcl |= DCL_UnBuffDimm;
		}
	}
	pci_write_config32(ctrl->f2, DRAM_CONFIG_LOW, dcl);

	if (is_registered(ctrl)) {
		printk(BIOS_SPEW, "Registered\n");
	} else {
		printk(BIOS_SPEW, "Unbuffered\n");
	}

	return dimm_mask;
}

static unsigned int spd_detect_dimms(const struct mem_controller *ctrl)
{
	unsigned dimm_mask;
	int i;
	dimm_mask = 0;
	for (i = 0; i < DIMM_SOCKETS; i++) {
		int byte;
		unsigned device;
		device = ctrl->channel0[i];
		if (device) {
			byte = spd_read_byte(ctrl->channel0[i], 2);  /* Type */
			if (byte == 7) {
				dimm_mask |= (1 << i);
			}
		}
		device = ctrl->channel1[i];
		if (device) {
			byte = spd_read_byte(ctrl->channel1[i], 2);
			if (byte == 7) {
				dimm_mask |= (1 << (i + DIMM_SOCKETS));
			}
		}
	}
	return dimm_mask;
}

static long spd_enable_2channels(const struct mem_controller *ctrl, long dimm_mask)
{
	int i;
	uint32_t nbcap;
	/* SPD addresses to verify are identical */
	static const uint8_t addresses[] = {
		2,	/* Type should be DDR SDRAM */
		3,	/* *Row addresses */
		4,	/* *Column addresses */
		5,	/* *Physical Banks */
		6,	/* *Module Data Width low */
		7,	/* *Module Data Width high */
		9,	/* *Cycle time at highest CAS Latency CL = X */
		11,	/* *SDRAM Type */
		13,	/* *SDRAM Width */
		17,	/* *Logical Banks */
		18,	/* *Supported CAS Latencies */
		21,	/* *SDRAM Module Attributes */
		23,	/* *Cycle time at CAS Latency (CLX - 0.5) */
		25,	/* *Cycle time at CAS Latency (CLX - 1.0) */
		27,	/* *tRP Row precharge time */
		28,	/* *Minimum Row Active to Row Active Delay (tRRD) */
		29,	/* *tRCD RAS to CAS */
		30,	/* *tRAS Activate to Precharge */
		41,	/* *Minimum Active to Active/Auto Refresh Time(Trc) */
		42,	/* *Minimum Auto Refresh Command Time(Trfc) */
	};
	/* If the dimms are not in pairs do not do dual channels */
	if ((dimm_mask & ((1 << DIMM_SOCKETS) - 1)) !=
		((dimm_mask >> DIMM_SOCKETS) & ((1 << DIMM_SOCKETS) - 1))) {
		goto single_channel;
	}
	/* If the CPU is not capable of doing dual channels don't do dual channels */
	nbcap = pci_read_config32(ctrl->f3, NORTHBRIDGE_CAP);
	if (!(nbcap & NBCAP_128Bit)) {
		goto single_channel;
	}
	for (i = 0; (i < 4) && (ctrl->channel0[i]); i++) {
		unsigned device0, device1;
		int value0, value1;
		int j;
		/* If I don't have a dimm skip this one */
		if (!(dimm_mask & (1 << i))) {
			continue;
		}
		device0 = ctrl->channel0[i];
		device1 = ctrl->channel1[i];
		for (j = 0; j < ARRAY_SIZE(addresses); j++) {
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
	uint32_t dcl;
	dcl = pci_read_config32(ctrl->f2, DRAM_CONFIG_LOW);
	dcl &= ~DCL_32ByteEn;
	dcl |= DCL_128BitEn;
	pci_write_config32(ctrl->f2, DRAM_CONFIG_LOW, dcl);
	return dimm_mask;
 single_channel:
	dimm_mask &= ~((1 << (DIMM_SOCKETS *2)) - (1 << DIMM_SOCKETS));
	return dimm_mask;
}

struct mem_param {
	uint8_t cycle_time;
	uint8_t divisor; /* In 1/2 ns increments */
	uint8_t tRC;
	uint8_t tRFC;
	uint32_t dch_memclk;
	uint16_t dch_tref4k, dch_tref8k;
	uint8_t	 dtl_twr;
	uint8_t	 dtl_twtr;
	uint8_t  dtl_trwt[3][3]; /* first index is CAS_LAT 2/2.5/3 and 128/registered64/64 */
 	uint8_t	 rdpreamble[4]; /* 0 is for registered, 1 for 1-2 DIMMS, 2 and 3 for 3 or 4 unreg dimm slots */
	char name[9];
};

static const struct mem_param *get_mem_param(int freq)
{
	static const struct mem_param speed[] = {
		[NBCAP_MEMCLK_100MHZ] = {
			.name	    = "100MHz",
			.cycle_time = 0xa0,
			.divisor    = (10 <<1),
			.tRC	    = 0x46,
			.tRFC	    = 0x50,
			.dch_memclk = DCH_MEMCLK_100MHZ << DCH_MEMCLK_SHIFT,
			.dch_tref4k = DTH_TREF_100MHZ_4K,
			.dch_tref8k = DTH_TREF_100MHZ_8K,
			.dtl_twr    = 2,
			.dtl_twtr   = 1,
			.dtl_trwt   = { { 2, 2, 3 }, { 3, 3, 4 }, { 3, 3, 4 }},
			.rdpreamble = { ((9 << 1) + 0), ((9 << 1) + 0), ((9 << 1) + 0), ((9 << 1) + 0) }
		},
		[NBCAP_MEMCLK_133MHZ] = {
			.name	    = "133MHz",
			.cycle_time = 0x75,
			.divisor    = (7<<1)+1,
			.tRC	    = 0x41,
			.tRFC	    = 0x4B,
			.dch_memclk = DCH_MEMCLK_133MHZ << DCH_MEMCLK_SHIFT,
			.dch_tref4k = DTH_TREF_133MHZ_4K,
			.dch_tref8k = DTH_TREF_133MHZ_8K,
			.dtl_twr    = 2,
			.dtl_twtr   = 1,
			.dtl_trwt   = { { 2, 2, 3 }, { 3, 3, 4 }, { 3, 3, 4 }},
			.rdpreamble = { ((8 << 1) + 0), ((7 << 1) + 0), ((7 << 1) + 1), ((7 << 1) + 0) }
		},
		[NBCAP_MEMCLK_166MHZ] = {
			.name	    = "166MHz",
			.cycle_time = 0x60,
			.divisor    = (6<<1),
			.tRC	    = 0x3C,
			.tRFC	    = 0x48,
			.dch_memclk = DCH_MEMCLK_166MHZ << DCH_MEMCLK_SHIFT,
			.dch_tref4k = DTH_TREF_166MHZ_4K,
			.dch_tref8k = DTH_TREF_166MHZ_8K,
			.dtl_twr    = 3,
			.dtl_twtr   = 1,
			.dtl_trwt   = { { 3, 2, 3 }, { 3, 3, 4 }, { 4, 3, 4 }},
			.rdpreamble = { ((7 << 1) + 1), ((6 << 1) + 0), ((6 << 1) + 1), ((6 << 1) + 0) }
		},
		[NBCAP_MEMCLK_200MHZ] = {
			.name	    = "200MHz",
			.cycle_time = 0x50,
			.divisor    = (5<<1),
			.tRC	    = 0x37,
			.tRFC	    = 0x46,
			.dch_memclk = DCH_MEMCLK_200MHZ << DCH_MEMCLK_SHIFT,
			.dch_tref4k = DTH_TREF_200MHZ_4K,
			.dch_tref8k = DTH_TREF_200MHZ_8K,
			.dtl_twr    = 3,
			.dtl_twtr   = 2,
			.dtl_trwt   = { { 0, 2, 3 }, { 3, 3, 4 }, { 3, 3, 4 }},
			.rdpreamble = { ((7 << 1) + 0), ((5 << 1) + 0), ((5 << 1) + 1), ((5 << 1) + 1) }
		}
	};
	const struct mem_param *param;

	param = speed + freq;
	printk(BIOS_SPEW, "%s\n", param->name);
	return param;
}

struct spd_set_memclk_result {
	const struct mem_param *param;
	long dimm_mask;
};

static int spd_dimm_loading_socket(const struct mem_controller *ctrl, long dimm_mask, int *freq_1t)
{

#if IS_ENABLED(CONFIG_CPU_AMD_SOCKET_939)

/* + 1 raise so we detect 0 as bad field */
#define DDR200 (NBCAP_MEMCLK_100MHZ + 1)
#define DDR333 (NBCAP_MEMCLK_166MHZ + 1)
#define DDR400 (NBCAP_MEMCLK_200MHZ + 1)
#define DDR_2T 0x80
#define DDR_MASK 0x7

#define DDR200_2T (DDR_2T | DDR200)
#define DDR333_2T (DDR_2T | DDR333)
#define DDR400_2T (DDR_2T | DDR400)

/*
	Following table comes directly from BKDG (unbuffered DIMM support)
	[Y][X] Y = ch0_0, ch1_0, ch0_1, ch1_1 1 = present 0 = empty
	  X uses same layout but 1 means double rank 0 is single rank/empty

	Following tables come from BKDG the ch{0_0,1_0,0_1,1_1} maps to
	MEMCS_{1L,1H,2L,2H} in i the PDF. PreE is table 45, and revE table 46.
*/

	static const unsigned char dimm_loading_config_preE[16][16] = {
		[0x8] = {[0x0] = DDR400,[0x8] = DDR400},
		[0x2] = {[0x0] = DDR333,[0x2] = DDR400},
		[0xa] = {[0x0] = DDR400_2T,[0x2] = DDR400_2T,
			 [0x8] = DDR400_2T,[0xa] = DDR333_2T},
		[0xc] = {[0x0] = DDR400,[0xc] = DDR400},
		[0x3] = {[0x0] = DDR333,[0x3] = DDR400},
		[0xf] = {[0x0] = DDR400_2T,[0x3] = DDR400_2T,
			 [0xc] = DDR400_2T,[0xf] = DDR333_2T},
	};

	static const unsigned char dimm_loading_config_revE[16][16] = {
		[0x8] = {[0x0] = DDR400, [0x8] = DDR400},
		[0x2] = {[0x0] = DDR333, [0x2] = DDR400},
		[0x4] = {[0x0] = DDR400, [0x4] = DDR400},
		[0x1] = {[0x0] = DDR333, [0x1] = DDR400},
		[0xa] = {[0x0] = DDR400_2T, [0x2] = DDR400_2T,
			 [0x8] = DDR400_2T, [0xa] = DDR333_2T},
		[0x5] = {[0x0] = DDR400_2T, [0x1] = DDR400_2T,
			 [0x4] = DDR400_2T, [0x5] = DDR333_2T},
		[0xc] = {[0x0] = DDR400, [0xc] = DDR400, [0x4] = DDR400, [0x8] = DDR400},
		[0x3] = {[0x0] = DDR333, [0x1] = DDR333, [0x2] = DDR333, [0x3] = DDR400},
		[0xe] = {[0x0] = DDR400_2T, [0x4] = DDR400_2T, [0x2] = DDR400_2T,
			 [0x6] = DDR400_2T, [0x8] = DDR400_2T, [0xc] = DDR400_2T,
			 [0xa] = DDR333_2T, [0xe] = DDR333_2T},
		[0xb] = {[0x0] = DDR333, [0x1] = DDR400_2T, [0x2] = DDR333_2T,
			 [0x3] = DDR400_2T, [0x8] = DDR333_2T, [0x9] = DDR400_2T,
			 [0xa] = DDR333_2T, [0xb] = DDR333_2T},
		[0xd] = {[0x0] = DDR400_2T, [0x8] = DDR400_2T, [0x1] = DDR400_2T,
			 [0x9] = DDR333_2T, [0x4] = DDR400_2T, [0xc] = DDR400_2T,
			 [0x5] = DDR333_2T, [0xd] = DDR333_2T},
		[0x7] = {[0x0] = DDR333,    [0x2] = DDR400_2T, [0x1] = DDR333_2T,
			 [0x3] = DDR400_2T, [0x4] = DDR333_2T, [0x6] = DDR400_2T,
			 [0x5] = DDR333_2T, [0x7] = DDR333_2T},
		[0xf] = {[0x0] = DDR400_2T, [0x1] = DDR400_2T, [0x4] = DDR400_2T,
			 [0x5] = DDR333_2T, [0x2] = DDR400_2T, [0x3] = DDR400_2T,
			 [0x6] = DDR400_2T, [0x7] = DDR333_2T, [0x8] = DDR400_2T,
			 [0x9] = DDR400_2T, [0xc] = DDR400_2T, [0xd] = DDR333_2T,
			 [0xa] = DDR333_2T, [0xb] = DDR333_2T, [0xe] = DDR333_2T,
			 [0xf] = DDR333_2T},
	};
	/*The dpos matches channel positions defined in BKDG and above arrays
	  The rpos is bitmask of dual rank dimms in same order as dpos */
	unsigned int dloading = 0, i, rpos = 0, dpos = 0;
	const unsigned char (*dimm_loading_config)[16] = dimm_loading_config_revE;
	int rank;
	uint32_t dcl;

	if (is_cpu_pre_e0()) {
		dimm_loading_config = dimm_loading_config_preE;
	}

	/* only DIMMS two per channel */
	for (i = 0; i < 2; i++) {
		if ((dimm_mask & (1 << i))) {
			/* read rank channel 0 */
			rank = spd_read_byte(ctrl->channel0[i], 5);
			if (rank < 0) goto hw_error;
			rpos |= (rank == 2) ? (1 << (3 - (i * 2))) : 0;
			dpos |= (1 << (3 - (i * 2)));
		}

		if ((dimm_mask & (1 << (i+DIMM_SOCKETS)))) {
			/* read rank channel 1*/
			rank = spd_read_byte(ctrl->channel1[i], 5);
			if (rank < 0) goto hw_error;
			rpos |= (rank == 2) ? (1 << (2 - (i * 2))) : 0;
			dpos |= (1 << (2 - (i * 2)));
		}
	}
	/* now the lookup, decode the max speed DDR400_2T etc */
	dloading = dimm_loading_config[dpos][rpos] & DDR_MASK;

hw_error:
	if (dloading != 0) {
		/* we have valid combination check the restrictions */
		dcl = pci_read_config32(ctrl->f2, DRAM_CONFIG_LOW);
		dcl |= ((dimm_loading_config[dpos][rpos] & DDR_2T) || CONFIG_K8_FORCE_2T_DRAM_TIMING) ? (DCL_En2T) : 0;
		/* Set DuallDimm is second channel is completely empty (revD+) */
		if (((cpuid_eax(1) & 0xfff0f) >= 0x10f00) && ((dpos & 0x5) == 0)) {
			printk(BIOS_DEBUG, "Setting DualDIMMen\n");
			dcl |= DCL_DualDIMMen;
		}
		pci_write_config32(ctrl->f2, DRAM_CONFIG_LOW, dcl);
		return dloading - 1;
	} else {
		/* if we don't find it we se it to DDR400 */
		printk(BIOS_WARNING, "Detected strange DIMM configuration, may not work! (or bug)\n");
		return NBCAP_MEMCLK_200MHZ;
	}

#elif IS_ENABLED(CONFIG_CPU_AMD_SOCKET_754)

#define CFGIDX(DIMM1,DIMM2,DIMM3) ((DIMM3)*9+(DIMM2)*3+(DIMM1))

#define EMPTY 0
#define X8S_X16 1
#define X8D 2

#define DDR200 NBCAP_MEMCLK_100MHZ
#define DDR333 NBCAP_MEMCLK_166MHZ
#define DDR400 NBCAP_MEMCLK_200MHZ

	/* this is table 42 from the BKDG, ignoring footnote 4,
	 * with the EMPTY, EMPTY, EMPTY row added */
	static const unsigned char cfgtable[][2] = {
		[CFGIDX(EMPTY,		EMPTY,		EMPTY	)] = { DDR400, DDR400 },
		[CFGIDX(X8S_X16,	EMPTY,		EMPTY	)] = { DDR400, DDR400 },
		[CFGIDX(EMPTY,		X8S_X16,	EMPTY	)] = { DDR400, DDR400 },
		[CFGIDX(EMPTY,		EMPTY,		X8S_X16	)] = { DDR400, DDR400 },
		[CFGIDX(X8D,		EMPTY,		EMPTY	)] = { DDR400, DDR400 },
		[CFGIDX(EMPTY,		X8D,		EMPTY	)] = { DDR400, DDR400 },
		[CFGIDX(EMPTY,		EMPTY,		X8D	)] = { DDR400, DDR400 },
		[CFGIDX(X8S_X16,	X8S_X16,	EMPTY	)] = { DDR400, DDR400 },
		[CFGIDX(X8S_X16,	X8D,		EMPTY	)] = { DDR400, DDR400 },
		[CFGIDX(X8S_X16,	EMPTY,		X8S_X16	)] = { DDR400, DDR400 },
		[CFGIDX(X8S_X16,	EMPTY,		X8D	)] = { DDR400, DDR400 },
		[CFGIDX(X8D,		X8S_X16,	EMPTY	)] = { DDR400, DDR400 },
		[CFGIDX(X8D,		X8D,		EMPTY	)] = { DDR333, DDR333 },
		[CFGIDX(X8D,		EMPTY,		X8S_X16	)] = { DDR400, DDR400 },
		[CFGIDX(X8D,		EMPTY,		X8D	)] = { DDR333, DDR333 },
		[CFGIDX(EMPTY,		X8S_X16,	X8S_X16	)] = { DDR333, DDR400 },
		[CFGIDX(EMPTY,		X8S_X16,	X8D	)] = { DDR200, DDR400 },
		[CFGIDX(EMPTY,		X8D,		X8S_X16	)] = { DDR200, DDR400 },
		[CFGIDX(EMPTY,		X8D,		X8D	)] = { DDR200, DDR333 },
		[CFGIDX(X8S_X16,	X8S_X16,	X8S_X16	)] = { DDR333, DDR400 },
		[CFGIDX(X8S_X16,	X8S_X16,	X8D	)] = { DDR200, DDR333 },
		[CFGIDX(X8S_X16,	X8D,		X8S_X16	)] = { DDR200, DDR333 },
		[CFGIDX(X8S_X16,	X8D,		X8D	)] = { DDR200, DDR333 },
		[CFGIDX(X8D,		X8S_X16,	X8S_X16	)] = { DDR333, DDR333 },
		[CFGIDX(X8D,		X8S_X16,	X8D	)] = { DDR200, DDR333 },
		[CFGIDX(X8D,		X8D,		X8S_X16	)] = { DDR200, DDR333 },
		[CFGIDX(X8D,		X8D,		X8D	)] = { DDR200, DDR333 }
	};

	int i, rank, width, dimmtypes[3];
	const unsigned char *cfg;

	for (i = 0; i < 3; i++) {
		if (dimm_mask & (1 << i)) {
			rank = spd_read_byte(ctrl->channel0[i], 5);
			width = spd_read_byte(ctrl->channel0[i], 13);
			if (rank < 0 || width < 0) die("failed to read SPD");
			width &= 0x7f;
			/* this is my guess as to how the criteria in the table
			 * are to be understood:
			 */
			dimmtypes[i] = width >= (rank == 1 ? 8 : 16) ? X8S_X16 : X8D;
		} else {
			dimmtypes[i] = EMPTY;
		}
	}
	cfg = cfgtable[CFGIDX(dimmtypes[0], dimmtypes[1], dimmtypes[2])];
	*freq_1t = cfg[0];
	return is_cpu_c0() ? cfg[0] : cfg[1];

#else /* CONFIG_CPU_AMD_SOCKET_* */

/* well, there are socket 940 boards supported which obviously fail to
 * compile with this */
//	#error load dependent memory clock limiting is not implemented for this socket

	/* see BKDG 4.1.3--if you just want to test a setup that doesn't
	 * require limiting, you may use the following code */

	*freq_1t = NBCAP_MEMCLK_200MHZ;
	return NBCAP_MEMCLK_200MHZ;

#endif /* CONFIG_CPU_AMD_SOCKET_* */

}

static struct spd_set_memclk_result spd_set_memclk(const struct mem_controller *ctrl, long dimm_mask)
{
	struct spd_set_memclk_result result;
	unsigned char cl_at_freq[NBCAP_MEMCLK_MASK + 1];
	int dimm, freq, max_freq_bios, max_freq_dloading, max_freq_1t;
	uint32_t value;

	static const uint8_t spd_min_cycle_time_indices[] = { 9, 23, 25 };
	static const unsigned char cycle_time_at_freq[] = {
		[NBCAP_MEMCLK_200MHZ] = 0x50, /* 5ns */
		[NBCAP_MEMCLK_166MHZ] = 0x60, /* 6ns */
		[NBCAP_MEMCLK_133MHZ] = 0x75, /* 7.5ns */
		[NBCAP_MEMCLK_100MHZ] = 0xa0, /* 10ns */
	};

	/* BEWARE that the constants for frequencies order in reverse of what
	 * would be intuitive. 200 MHz has the lowest constant, 100 MHz the
	 * highest. Thus, all comparisons and traversal directions having to
	 * do with frequencies are/have to be the opposite of what would be
	 * intuitive.
	 */

	/* the CLs supported by the controller: */
	memset(cl_at_freq, 0x1c, sizeof(cl_at_freq));
	memset(cl_at_freq, 0x00,
		(pci_read_config32(ctrl->f3, NORTHBRIDGE_CAP) >>
		 NBCAP_MEMCLK_SHIFT) & NBCAP_MEMCLK_MASK);
	max_freq_bios = read_option(max_mem_clock, 0);
	if (max_freq_bios <= NBCAP_MEMCLK_100MHZ)
		memset(cl_at_freq, 0x00, max_freq_bios);
	for (dimm = 0; dimm < DIMM_SOCKETS; dimm++) {
		int x,i,spd_cls,cl,spd_min_cycle_time;
		unsigned char cl_at_freq_mask[sizeof(cl_at_freq)];

		if (!(dimm_mask & (1 << dimm)))
			continue;
		/* Byte 18 for DDR SDRAM is interpreted:
		 * bit 0 == CAS Latency = 1.0
		 * bit 1 == CAS Latency = 1.5
		 * bit 2 == CAS Latency = 2.0
		 * bit 3 == CAS Latency = 2.5
		 * bit 4 == CAS Latency = 3.0
		 * bit 5 == CAS Latency = 3.5
		 * bit 6 == CAS Latency = 4.0
		 * bit 7 == TBD
		 */
		spd_cls = spd_read_byte(ctrl->channel0[dimm], 18);
		if (spd_cls <= 0)
			goto hw_error;
		memset(cl_at_freq_mask, 0x00, sizeof(cl_at_freq_mask));
		for (cl = 1 << log2(spd_cls), i = 0; i < 3; cl >>= 1, i++) {
			if (!(spd_cls & cl))
				continue;
			spd_min_cycle_time = spd_read_byte(ctrl->channel0[dimm],
					spd_min_cycle_time_indices[i]);
			if (spd_min_cycle_time < 0)
				goto hw_error;
			if ((!spd_min_cycle_time) || (spd_min_cycle_time & 0x0f) > 9)
				continue;
			for (x = 0; x < sizeof(cl_at_freq_mask); x++)
				if (cycle_time_at_freq[x] >= spd_min_cycle_time)
					cl_at_freq_mask[x] |= cl;
		}
		for (x = 0; x < sizeof(cl_at_freq_mask); x++)
			cl_at_freq[x] &= cl_at_freq_mask[x];
	}

	freq = NBCAP_MEMCLK_200MHZ;
	while (freq < sizeof(cl_at_freq) && !cl_at_freq[freq])
		freq++;

	max_freq_dloading = spd_dimm_loading_socket(ctrl, dimm_mask, &max_freq_1t);
	if (max_freq_dloading > freq) {
		printk(BIOS_WARNING, "Memory speed reduced due to signal loading conditions\n");
		freq = max_freq_dloading;
		while (freq < sizeof(cl_at_freq) && !cl_at_freq[freq])
			freq++;
	}

	/* if the next lower frequency gives a CL at least one whole cycle
	 * shorter, select that (see end of BKDG 4.1.1.1) */
	if (freq < sizeof(cl_at_freq)-1 && cl_at_freq[freq+1] &&
		__ffs(cl_at_freq[freq]) - __ffs(cl_at_freq[freq+1]) >= 2)
			freq++;

	if (freq == sizeof(cl_at_freq))
		goto hw_error;

#if IS_ENABLED(CONFIG_CPU_AMD_SOCKET_754)
	if (freq < max_freq_1t || CONFIG_K8_FORCE_2T_DRAM_TIMING) {
		pci_write_config32(ctrl->f2, DRAM_CONFIG_LOW,
			pci_read_config32(ctrl->f2, DRAM_CONFIG_LOW) | DCL_En2T);
	}
#endif

	result.param = get_mem_param(freq);

	/* Update DRAM Config High with our selected memory speed */
	value = pci_read_config32(ctrl->f2, DRAM_CONFIG_HIGH);
	value &= ~(DCH_MEMCLK_MASK << DCH_MEMCLK_SHIFT);

	value |= result.param->dch_memclk;
	pci_write_config32(ctrl->f2, DRAM_CONFIG_HIGH, value);

	static const unsigned latencies[] = { DTL_CL_2, DTL_CL_2_5, DTL_CL_3 };

	/* Update DRAM Timing Low with our selected cas latency */
	value = pci_read_config32(ctrl->f2, DRAM_TIMING_LOW);
	value &= ~(DTL_TCL_MASK << DTL_TCL_SHIFT);
	value |= latencies[__ffs(cl_at_freq[freq]) - 2] << DTL_TCL_SHIFT;
	pci_write_config32(ctrl->f2, DRAM_TIMING_LOW, value);

	result.dimm_mask = dimm_mask;
	return result;
 hw_error:
	result.param = (const struct mem_param *)0;
	result.dimm_mask = -1;
	return result;
}


static int update_dimm_Trc(const struct mem_controller *ctrl, const struct mem_param *param, int i)
{
	unsigned clocks, old_clocks;
	uint32_t dtl;
	int value;
	value = spd_read_byte(ctrl->channel0[i], 41);
	if (value < 0) return -1;
	if ((value == 0) || (value == 0xff)) {
		value = param->tRC;
	}
	clocks = CEIL_DIV((value << 1), param->divisor);
	if (clocks < DTL_TRC_MIN) {
		clocks = DTL_TRC_MIN;
	}
	if (clocks > DTL_TRC_MAX) {
		return 0;
	}

	dtl = pci_read_config32(ctrl->f2, DRAM_TIMING_LOW);
	old_clocks = ((dtl >> DTL_TRC_SHIFT) & DTL_TRC_MASK) + DTL_TRC_BASE;
	if (old_clocks > clocks) {
		clocks = old_clocks;
	}
	dtl &= ~(DTL_TRC_MASK << DTL_TRC_SHIFT);
	dtl |=	((clocks - DTL_TRC_BASE) << DTL_TRC_SHIFT);
	pci_write_config32(ctrl->f2, DRAM_TIMING_LOW, dtl);
	return 1;
}

static int update_dimm_Trfc(const struct mem_controller *ctrl, const struct mem_param *param, int i)
{
	unsigned clocks, old_clocks;
	uint32_t dtl;
	int value;
	value = spd_read_byte(ctrl->channel0[i], 42);
	if (value < 0) return -1;
	if ((value == 0) || (value == 0xff)) {
		value = param->tRFC;
	}
	clocks = CEIL_DIV((value << 1), param->divisor);
	if (clocks < DTL_TRFC_MIN) {
		clocks = DTL_TRFC_MIN;
	}
	if (clocks > DTL_TRFC_MAX) {
		return 0;
	}
	dtl = pci_read_config32(ctrl->f2, DRAM_TIMING_LOW);
	old_clocks = ((dtl >> DTL_TRFC_SHIFT) & DTL_TRFC_MASK) + DTL_TRFC_BASE;
	if (old_clocks > clocks) {
		clocks = old_clocks;
	}
	dtl &= ~(DTL_TRFC_MASK << DTL_TRFC_SHIFT);
	dtl |= ((clocks - DTL_TRFC_BASE) << DTL_TRFC_SHIFT);
	pci_write_config32(ctrl->f2, DRAM_TIMING_LOW, dtl);
	return 1;
}


static int update_dimm_Trcd(const struct mem_controller *ctrl, const struct mem_param *param, int i)
{
	unsigned clocks, old_clocks;
	uint32_t dtl;
	int value;
	value = spd_read_byte(ctrl->channel0[i], 29);
	if (value < 0) return -1;
	clocks = CEIL_DIV(value, (param->divisor << 1));
	if (clocks < DTL_TRCD_MIN) {
		clocks = DTL_TRCD_MIN;
	}
	if (clocks > DTL_TRCD_MAX) {
		return 0;
	}
	dtl = pci_read_config32(ctrl->f2, DRAM_TIMING_LOW);
	old_clocks = ((dtl >> DTL_TRCD_SHIFT) & DTL_TRCD_MASK) + DTL_TRCD_BASE;
	if (old_clocks > clocks) {
		clocks = old_clocks;
	}
	dtl &= ~(DTL_TRCD_MASK << DTL_TRCD_SHIFT);
	dtl |= ((clocks - DTL_TRCD_BASE) << DTL_TRCD_SHIFT);
	pci_write_config32(ctrl->f2, DRAM_TIMING_LOW, dtl);
	return 1;
}

static int update_dimm_Trrd(const struct mem_controller *ctrl, const struct mem_param *param, int i)
{
	unsigned clocks, old_clocks;
	uint32_t dtl;
	int value;
	value = spd_read_byte(ctrl->channel0[i], 28);
	if (value < 0) return -1;
	clocks = CEIL_DIV(value, (param->divisor << 1));
	if (clocks < DTL_TRRD_MIN) {
		clocks = DTL_TRRD_MIN;
	}
	if (clocks > DTL_TRRD_MAX) {
		return 0;
	}
	dtl = pci_read_config32(ctrl->f2, DRAM_TIMING_LOW);
	old_clocks = ((dtl >> DTL_TRRD_SHIFT) & DTL_TRRD_MASK) + DTL_TRRD_BASE;
	if (old_clocks > clocks) {
		clocks = old_clocks;
	}
	dtl &= ~(DTL_TRRD_MASK << DTL_TRRD_SHIFT);
	dtl |= ((clocks - DTL_TRRD_BASE) << DTL_TRRD_SHIFT);
	pci_write_config32(ctrl->f2, DRAM_TIMING_LOW, dtl);
	return 1;
}

static int update_dimm_Tras(const struct mem_controller *ctrl, const struct mem_param *param, int i)
{
	unsigned clocks, old_clocks;
	uint32_t dtl;
	int value;
	value = spd_read_byte(ctrl->channel0[i], 30);
	if (value < 0) return -1;
	clocks = CEIL_DIV((value << 1), param->divisor);
	if (clocks < DTL_TRAS_MIN) {
		clocks = DTL_TRAS_MIN;
	}
	if (clocks > DTL_TRAS_MAX) {
		return 0;
	}
	dtl = pci_read_config32(ctrl->f2, DRAM_TIMING_LOW);
	old_clocks = ((dtl >> DTL_TRAS_SHIFT) & DTL_TRAS_MASK) + DTL_TRAS_BASE;
	if (old_clocks > clocks) {
		clocks = old_clocks;
	}
	dtl &= ~(DTL_TRAS_MASK << DTL_TRAS_SHIFT);
	dtl |= ((clocks - DTL_TRAS_BASE) << DTL_TRAS_SHIFT);
	pci_write_config32(ctrl->f2, DRAM_TIMING_LOW, dtl);
	return 1;
}

static int update_dimm_Trp(const struct mem_controller *ctrl, const struct mem_param *param, int i)
{
	unsigned clocks, old_clocks;
	uint32_t dtl;
	int value;
	value = spd_read_byte(ctrl->channel0[i], 27);
	if (value < 0) return -1;
	clocks = CEIL_DIV(value, (param->divisor << 1));
	if (clocks < DTL_TRP_MIN) {
		clocks = DTL_TRP_MIN;
	}
	if (clocks > DTL_TRP_MAX) {
		return 0;
	}
	dtl = pci_read_config32(ctrl->f2, DRAM_TIMING_LOW);
	old_clocks = ((dtl >> DTL_TRP_SHIFT) & DTL_TRP_MASK) + DTL_TRP_BASE;
	if (old_clocks > clocks) {
		clocks = old_clocks;
	}
	dtl &= ~(DTL_TRP_MASK << DTL_TRP_SHIFT);
	dtl |= ((clocks - DTL_TRP_BASE) << DTL_TRP_SHIFT);
	pci_write_config32(ctrl->f2, DRAM_TIMING_LOW, dtl);
	return 1;
}

static void set_Twr(const struct mem_controller *ctrl, const struct mem_param *param)
{
	uint32_t dtl;
	dtl = pci_read_config32(ctrl->f2, DRAM_TIMING_LOW);
	dtl &= ~(DTL_TWR_MASK << DTL_TWR_SHIFT);
	dtl |= (param->dtl_twr - DTL_TWR_BASE) << DTL_TWR_SHIFT;
	pci_write_config32(ctrl->f2, DRAM_TIMING_LOW, dtl);
}


static void init_Tref(const struct mem_controller *ctrl, const struct mem_param *param)
{
	uint32_t dth;
	dth = pci_read_config32(ctrl->f2, DRAM_TIMING_HIGH);
	dth &= ~(DTH_TREF_MASK << DTH_TREF_SHIFT);
	dth |= (param->dch_tref4k << DTH_TREF_SHIFT);
	pci_write_config32(ctrl->f2, DRAM_TIMING_HIGH, dth);
}

static int update_dimm_Tref(const struct mem_controller *ctrl, const struct mem_param *param, int i)
{
	uint32_t dth;
	int value;
	unsigned tref, old_tref;
	value = spd_read_byte(ctrl->channel0[i], 3);
	if (value < 0) return -1;
	value &= 0xf;

	tref = param->dch_tref8k;
	if (value == 12) {
		tref = param->dch_tref4k;
	}

	dth = pci_read_config32(ctrl->f2, DRAM_TIMING_HIGH);
	old_tref = (dth >> DTH_TREF_SHIFT) & DTH_TREF_MASK;
	if ((value == 12) && (old_tref == param->dch_tref4k)) {
		tref = param->dch_tref4k;
	} else {
		tref = param->dch_tref8k;
	}
	dth &= ~(DTH_TREF_MASK << DTH_TREF_SHIFT);
	dth |= (tref << DTH_TREF_SHIFT);
	pci_write_config32(ctrl->f2, DRAM_TIMING_HIGH, dth);
	return 1;
}


static int update_dimm_x4(const struct mem_controller *ctrl, const struct mem_param *param, int i)
{
	uint32_t dcl;
	int value;
#if IS_ENABLED(CONFIG_QRANK_DIMM_SUPPORT)
	int rank;
#endif
	int dimm;
	value = spd_read_byte(ctrl->channel0[i], 13);
	if (value < 0) {
		return -1;
	}

#if IS_ENABLED(CONFIG_QRANK_DIMM_SUPPORT)
	rank = spd_read_byte(ctrl->channel0[i], 5);	/* number of physical banks */
	if (rank < 0) {
		return -1;
	}
#endif

	dimm = 1<<(DCL_x4DIMM_SHIFT+i);
#if IS_ENABLED(CONFIG_QRANK_DIMM_SUPPORT)
	if (rank == 4) {
		dimm |= 1<<(DCL_x4DIMM_SHIFT+i+2);
	}
#endif
	dcl = pci_read_config32(ctrl->f2, DRAM_CONFIG_LOW);
	dcl &= ~dimm;
	if (value == 4) {
		dcl |= dimm;
	}
	pci_write_config32(ctrl->f2, DRAM_CONFIG_LOW, dcl);
	return 1;
}

static int update_dimm_ecc(const struct mem_controller *ctrl, const struct mem_param *param, int i)
{
	uint32_t dcl;
	int value;
	value = spd_read_byte(ctrl->channel0[i], 11);
	if (value < 0) {
		return -1;
	}
	if (value != 2) {
		dcl = pci_read_config32(ctrl->f2, DRAM_CONFIG_LOW);
		dcl &= ~DCL_DimmEccEn;
		pci_write_config32(ctrl->f2, DRAM_CONFIG_LOW, dcl);
	}
	return 1;
}

static int count_dimms(const struct mem_controller *ctrl)
{
	int dimms;
	unsigned index;
	dimms = 0;
	for (index = 0; index < 8; index += 2) {
		uint32_t csbase;
		csbase = pci_read_config32(ctrl->f2, (DRAM_CSBASE + (index << 2)));
		if (csbase & 1) {
			dimms += 1;
		}
	}
	return dimms;
}

static void set_Twtr(const struct mem_controller *ctrl, const struct mem_param *param)
{
	uint32_t dth;

	dth = pci_read_config32(ctrl->f2, DRAM_TIMING_HIGH);
	dth &= ~(DTH_TWTR_MASK << DTH_TWTR_SHIFT);
	dth |= ((param->dtl_twtr - DTH_TWTR_BASE) << DTH_TWTR_SHIFT);
	pci_write_config32(ctrl->f2, DRAM_TIMING_HIGH, dth);
}

static void set_Trwt(const struct mem_controller *ctrl, const struct mem_param *param)
{
	uint32_t dth, dtl;
	unsigned latency;
	unsigned clocks;
	int lat, mtype;

	clocks = 0;
	dtl = pci_read_config32(ctrl->f2, DRAM_TIMING_LOW);
	latency = (dtl >> DTL_TCL_SHIFT) & DTL_TCL_MASK;

	if (is_opteron(ctrl)) {
		mtype = 0; /* dual channel */
	} else if (is_registered(ctrl)) {
		mtype = 1; /* registered 64bit interface */
	} else {
		mtype = 2; /* unbuffered 64bit interface */
	}

	switch (latency) {
		case DTL_CL_2:
			lat = 0;
			break;
		case DTL_CL_2_5:
			lat = 1;
			break;
		case DTL_CL_3:
			lat = 2;
			break;
		default:
			die("Unknown LAT for Trwt");
	}

	clocks = param->dtl_trwt[lat][mtype];
	if ((clocks < DTH_TRWT_MIN) || (clocks > DTH_TRWT_MAX)) {
		die("Unknown Trwt\n");
	}

	dth = pci_read_config32(ctrl->f2, DRAM_TIMING_HIGH);
	dth &= ~(DTH_TRWT_MASK << DTH_TRWT_SHIFT);
	dth |= ((clocks - DTH_TRWT_BASE) << DTH_TRWT_SHIFT);
	pci_write_config32(ctrl->f2, DRAM_TIMING_HIGH, dth);
	return;
}

static void set_Twcl(const struct mem_controller *ctrl, const struct mem_param *param)
{
	/* Memory Clocks after CAS# */
	uint32_t dth;
	unsigned clocks;
	if (is_registered(ctrl)) {
		clocks = 2;
	} else {
		clocks = 1;
	}
	dth = pci_read_config32(ctrl->f2, DRAM_TIMING_HIGH);
	dth &= ~(DTH_TWCL_MASK << DTH_TWCL_SHIFT);
	dth |= ((clocks - DTH_TWCL_BASE) << DTH_TWCL_SHIFT);
	pci_write_config32(ctrl->f2, DRAM_TIMING_HIGH, dth);
}


static void set_read_preamble(const struct mem_controller *ctrl, const struct mem_param *param)
{
	uint32_t dch;
	unsigned rdpreamble;
	int slots, i;

	slots = 0;

	for (i = 0; i < 4; i++) {
		if (ctrl->channel0[i]) {
			slots += 1;
		}
	}

	/* map to index to param.rdpreamble array */
	if (is_registered(ctrl)) {
		i = 0;
	} else if (slots < 3) {
		i = 1;
	} else if (slots == 3) {
		i = 2;
	} else if (slots == 4) {
		i = 3;
	} else {
		die("Unknown rdpreamble for this nr of slots");
	}

	dch = pci_read_config32(ctrl->f2, DRAM_CONFIG_HIGH);
	dch &= ~(DCH_RDPREAMBLE_MASK << DCH_RDPREAMBLE_SHIFT);
	rdpreamble = param->rdpreamble[i];

	if ((rdpreamble < DCH_RDPREAMBLE_MIN) || (rdpreamble > DCH_RDPREAMBLE_MAX)) {
		die("Unknown rdpreamble");
	}

	dch |= (rdpreamble - DCH_RDPREAMBLE_BASE) << DCH_RDPREAMBLE_SHIFT;
	pci_write_config32(ctrl->f2, DRAM_CONFIG_HIGH, dch);
}

static void set_max_async_latency(const struct mem_controller *ctrl, const struct mem_param *param)
{
	uint32_t dch;
	unsigned async_lat;
	int dimms;

	dimms = count_dimms(ctrl);

	dch = pci_read_config32(ctrl->f2, DRAM_CONFIG_HIGH);
	dch &= ~(DCH_ASYNC_LAT_MASK << DCH_ASYNC_LAT_SHIFT);
	async_lat = 0;
	if (is_registered(ctrl)) {
		if (dimms == 4) {
			/* 9ns */
			async_lat = 9;
		}
		else {
			/* 8ns */
			async_lat = 8;
		}
	}
	else {
		if (dimms > 3) {
			die("Too many unbuffered dimms");
		}
		else if (dimms == 3) {
			/* 7ns */
			async_lat = 7;
		}
		else {
			/* 6ns */
			async_lat = 6;
		}
	}
	dch |= ((async_lat - DCH_ASYNC_LAT_BASE) << DCH_ASYNC_LAT_SHIFT);
	pci_write_config32(ctrl->f2, DRAM_CONFIG_HIGH, dch);
}

static void set_idle_cycle_limit(const struct mem_controller *ctrl, const struct mem_param *param)
{
	uint32_t dch;
	/* AMD says to Hardcode this */
	dch = pci_read_config32(ctrl->f2, DRAM_CONFIG_HIGH);
	dch &= ~(DCH_IDLE_LIMIT_MASK << DCH_IDLE_LIMIT_SHIFT);
	dch |= DCH_IDLE_LIMIT_16 << DCH_IDLE_LIMIT_SHIFT;
	dch |= DCH_DYN_IDLE_CTR_EN;
	pci_write_config32(ctrl->f2, DRAM_CONFIG_HIGH, dch);
}

static long spd_set_dram_timing(const struct mem_controller *ctrl, const struct mem_param *param, long dimm_mask)
{
	int i;

	init_Tref(ctrl, param);
	for (i = 0; i < DIMM_SOCKETS; i++) {
		int rc;
		if (!(dimm_mask & (1 << i))) {
			continue;
		}
		/* DRAM Timing Low Register */
		if ((rc = update_dimm_Trc (ctrl, param, i)) <= 0) goto dimm_err;
		if ((rc = update_dimm_Trfc(ctrl, param, i)) <= 0) goto dimm_err;
		if ((rc = update_dimm_Trcd(ctrl, param, i)) <= 0) goto dimm_err;
		if ((rc = update_dimm_Trrd(ctrl, param, i)) <= 0) goto dimm_err;
		if ((rc = update_dimm_Tras(ctrl, param, i)) <= 0) goto dimm_err;
		if ((rc = update_dimm_Trp (ctrl, param, i)) <= 0) goto dimm_err;

		/* DRAM Timing High Register */
		if ((rc = update_dimm_Tref(ctrl, param, i)) <= 0) goto dimm_err;


		/* DRAM Config Low */
		if ((rc = update_dimm_x4 (ctrl, param, i)) <= 0) goto dimm_err;
		if ((rc = update_dimm_ecc(ctrl, param, i)) <= 0) goto dimm_err;
		continue;
	dimm_err:
		if (rc < 0) {
			return -1;
		}
		dimm_mask = disable_dimm(ctrl, i, dimm_mask);
	}
	/* DRAM Timing Low Register */
	set_Twr(ctrl, param);

	/* DRAM Timing High Register */
	set_Twtr(ctrl, param);
	set_Trwt(ctrl, param);
	set_Twcl(ctrl, param);

	/* DRAM Config High */
	set_read_preamble(ctrl, param);
	set_max_async_latency(ctrl, param);
	set_idle_cycle_limit(ctrl, param);
	return dimm_mask;
}

#if IS_ENABLED(CONFIG_RAMINIT_SYSINFO)
void sdram_set_spd_registers(const struct mem_controller *ctrl, struct sys_info *sysinfo)
#else
void sdram_set_spd_registers(const struct mem_controller *ctrl)
#endif
{
	struct spd_set_memclk_result result;
	const struct mem_param *param;
	long dimm_mask;
#if 1
	if (!controller_present(ctrl)) {
		return;
	}
#endif
	hw_enable_ecc(ctrl);
	activate_spd_rom(ctrl);
	dimm_mask = spd_detect_dimms(ctrl);
	if (!(dimm_mask & ((1 << DIMM_SOCKETS) - 1))) {
		printk(BIOS_DEBUG, "No memory for this cpu\n");
		return;
	}
	dimm_mask = spd_enable_2channels(ctrl, dimm_mask);
	if (dimm_mask < 0)
		goto hw_spd_err;
	dimm_mask = spd_set_ram_size(ctrl , dimm_mask);
	if (dimm_mask < 0)
		goto hw_spd_err;
	dimm_mask = spd_handle_unbuffered_dimms(ctrl, dimm_mask);
	if (dimm_mask < 0)
		goto hw_spd_err;
	result = spd_set_memclk(ctrl, dimm_mask);
	param     = result.param;
	dimm_mask = result.dimm_mask;
	if (dimm_mask < 0)
		goto hw_spd_err;
	dimm_mask = spd_set_dram_timing(ctrl, param , dimm_mask);
	if (dimm_mask < 0)
		goto hw_spd_err;
	order_dimms(ctrl);
	return;
 hw_spd_err:
	/* Unrecoverable error reading SPD data */
	printk(BIOS_ERR, "SPD error - reset\n");
	hard_reset();
	return;
}

#if CONFIG_HW_MEM_HOLE_SIZEK != 0
static uint32_t hoist_memory(int controllers, const struct mem_controller *ctrl,unsigned hole_startk, int i)
{
	int ii;
	uint32_t carry_over;
	pci_devfn_t dev;
	uint32_t base, limit;
	uint32_t basek;
	uint32_t hoist;
	int j;

	carry_over = (4*1024*1024) - hole_startk;

	for (ii = controllers - 1; ii > i; ii--) {
		base  = pci_read_config32(ctrl[0].f1, 0x40 + (ii << 3));
		if ((base & ((1<<1)|(1<<0))) != ((1<<1)|(1<<0))) {
			continue;
		}
		limit = pci_read_config32(ctrl[0].f1, 0x44 + (ii << 3));
		for (j = 0; j < controllers; j++) {
			pci_write_config32(ctrl[j].f1, 0x44 + (ii << 3), limit + (carry_over << 2));
			pci_write_config32(ctrl[j].f1, 0x40 + (ii << 3), base + (carry_over << 2));
		}
	}
	limit = pci_read_config32(ctrl[0].f1, 0x44 + (i << 3));
	for (j = 0; j < controllers; j++) {
		pci_write_config32(ctrl[j].f1, 0x44 + (i << 3), limit + (carry_over << 2));
	}
	dev = ctrl[i].f1;
	base  = pci_read_config32(dev, 0x40 + (i << 3));
	basek  = (base & 0xffff0000) >> 2;
	if (basek == hole_startk) {
		//don't need set memhole here, because hole off set will be 0, overflow
		//so need to change base reg instead, new basek will be 4*1024*1024
		base &= 0x0000ffff;
		base |= (4*1024*1024)<<2;
		for (j = 0; j < controllers; j++) {
			pci_write_config32(ctrl[j].f1, 0x40 + (i<<3), base);
		}
	}
	else {
		hoist = /* hole start address */
			((hole_startk << 10) & 0xff000000) +
			/* hole address to memory controller address */
			(((basek + carry_over) >> 6) & 0x0000ff00) +
			/* enable */
			1;
		pci_write_config32(dev, 0xf0, hoist);
	}

	return carry_over;
}

void set_hw_mem_hole(int controllers, const struct mem_controller *ctrl)
{

	uint32_t hole_startk;
	int i;

	hole_startk = 4*1024*1024 - CONFIG_HW_MEM_HOLE_SIZEK;

	printk(BIOS_SPEW, "Handling memory hole at 0x%08x (default)\n", hole_startk);
#if IS_ENABLED(CONFIG_HW_MEM_HOLE_SIZE_AUTO_INC)
	/* We need to double check if hole_startk is valid.
	 * If it is equal to the dram base address in K (base_k),
	 * we need to decrease it.
	 */
	uint32_t basek_pri;
	for (i = 0; i < controllers; i++) {
			uint32_t base;
			unsigned base_k;
			base  = pci_read_config32(ctrl[0].f1, 0x40 + (i << 3));
			if ((base & ((1<<1)|(1<<0))) != ((1<<1)|(1<<0))) {
				continue;
			}
			base_k = (base & 0xffff0000) >> 2;
			if (base_k == hole_startk) {
				/* decrease memory hole startk to make sure it is
				 * in the middle of the previous node
				 */
				hole_startk -= (base_k - basek_pri)>>1;
				break; /* only one hole */
			}
			basek_pri = base_k;
	}

	printk(BIOS_SPEW, "Handling memory hole at 0x%08x (adjusted)\n", hole_startk);
#endif
	/* Find node number that needs the memory hole configured */
	for (i = 0; i < controllers; i++) {
			uint32_t base, limit;
			unsigned base_k, limit_k;
			base  = pci_read_config32(ctrl[0].f1, 0x40 + (i << 3));
			if ((base & ((1<<1)|(1<<0))) != ((1<<1)|(1<<0))) {
				continue;
			}
			limit = pci_read_config32(ctrl[0].f1, 0x44 + (i << 3));
			base_k = (base & 0xffff0000) >> 2;
			limit_k = ((limit + 0x00010000) & 0xffff0000) >> 2;
			if ((base_k <= hole_startk) && (limit_k > hole_startk)) {
				unsigned end_k;
				hoist_memory(controllers, ctrl, hole_startk, i);
				end_k = memory_end_k(ctrl, controllers);
				set_top_mem(end_k, hole_startk);
				break; /* only one hole */
			}
	}

}

#endif

#if IS_ENABLED(CONFIG_RAMINIT_SYSINFO)
void sdram_enable(int controllers, const struct mem_controller *ctrl, struct sys_info *sysinfo)
#else
void sdram_enable(int controllers, const struct mem_controller *ctrl)
#endif
{
	int i;
	u32 whatWait = 0;
	int suspend = acpi_is_wakeup_s3();

	/* Error if I don't have memory */
	if (memory_end_k(ctrl, controllers) == 0) {
		die("No memory\n");
	}

	/* Before enabling memory start the memory clocks */
	for (i = 0; i < controllers; i++) {
		uint32_t dch;
		if (!controller_present(ctrl + i))
			continue;
		dch = pci_read_config32(ctrl[i].f2, DRAM_CONFIG_HIGH);
		if (dch & (DCH_MEMCLK_EN0|DCH_MEMCLK_EN1|DCH_MEMCLK_EN2|DCH_MEMCLK_EN3)) {
			dch |= DCH_MEMCLK_VALID;
			pci_write_config32(ctrl[i].f2, DRAM_CONFIG_HIGH, dch);
		}
		else {
			/* Disable dram receivers */
			uint32_t dcl;
			dcl = pci_read_config32(ctrl[i].f2, DRAM_CONFIG_LOW);
			dcl |= DCL_DisInRcvrs;
			pci_write_config32(ctrl[i].f2, DRAM_CONFIG_LOW, dcl);
		}
	}

	/* We need to wait a minimum of 20 MEMCLKS to enable the InitDram */
	/* And if necessary toggle the the reset on the dimms by hand */
	memreset(controllers, ctrl);

	for (i = 0; i < controllers; i++) {
		uint32_t dcl, dch;
		if (!controller_present(ctrl + i))
			continue;
		/* Skip everything if I don't have any memory on this controller */
		dch = pci_read_config32(ctrl[i].f2, DRAM_CONFIG_HIGH);
		if (!(dch & DCH_MEMCLK_VALID)) {
			continue;
		}

		/* Toggle DisDqsHys to get it working */
		dcl = pci_read_config32(ctrl[i].f2, DRAM_CONFIG_LOW);
		if (dcl & DCL_DimmEccEn) {
			uint32_t mnc;
			printk(BIOS_SPEW, "ECC enabled\n");
			mnc = pci_read_config32(ctrl[i].f3, MCA_NB_CONFIG);
			mnc |= MNC_ECC_EN;
			if (dcl & DCL_128BitEn) {
				mnc |= MNC_CHIPKILL_EN;
			}
			pci_write_config32(ctrl[i].f3, MCA_NB_CONFIG, mnc);
		}

		if (!suspend) {
			dcl |= DCL_DisDqsHys;
			pci_write_config32(ctrl[i].f2, DRAM_CONFIG_LOW, dcl);
		}
		dcl &= ~DCL_DisDqsHys;
		dcl &= ~DCL_DLL_Disable;
		dcl &= ~DCL_D_DRV;
		dcl &= ~DCL_QFC_EN;

		if (suspend) {
			enable_lapic();
			init_timer();
			dcl |= (DCL_ESR | DCL_SRS);
			/* Handle errata 85 Insufficient Delay Between MEMCLK Startup
			   and CKE Assertion During Resume From S3 */
			udelay(10); /* for unregistered */
			if (is_registered(&ctrl[i])) {
				udelay(100); /* 110us for registered (we wait 10us already) */
			}
			whatWait = DCL_ESR;
		} else {
			dcl |= DCL_DramInit;
			whatWait = DCL_DramInit;
		}
		pci_write_config32(ctrl[i].f2, DRAM_CONFIG_LOW, dcl);
	}

	for (i = 0; i < controllers; i++) {
		uint32_t dcl, dch;
		if (!controller_present(ctrl + i))
			continue;
		/* Skip everything if I don't have any memory on this controller */
		dch = pci_read_config32(ctrl[i].f2, DRAM_CONFIG_HIGH);
		if (!(dch & DCH_MEMCLK_VALID)) {
			continue;
		}

		printk(BIOS_DEBUG, "Initializing memory: ");
		int loops = 0;
		do {
			dcl = pci_read_config32(ctrl[i].f2, DRAM_CONFIG_LOW);
			loops++;
			if ((loops & 1023) == 0) {
				printk(BIOS_DEBUG, ".");
			}
		} while (((dcl & whatWait) != 0) && (loops < TIMEOUT_LOOPS));
		if (loops >= TIMEOUT_LOOPS) {
			printk(BIOS_DEBUG, " failed\n");
			continue;
		}

		if (!is_cpu_pre_c0()) {
			/* Wait until it is safe to touch memory */
			do {
				dcl = pci_read_config32(ctrl[i].f2, DRAM_CONFIG_LOW);
			} while (((dcl & DCL_MemClrStatus) == 0) || ((dcl & DCL_DramEnable) == 0) ||
					((dcl & DCL_SRS)));
		}

		printk(BIOS_DEBUG, " done\n");
	}

#if CONFIG_HW_MEM_HOLE_SIZEK != 0
	 // init hw mem hole here
	/* DramHoleValid bit only can be set after MemClrStatus is set by Hardware */
	if (!is_cpu_pre_e0())
		set_hw_mem_hole(controllers, ctrl);
#endif

	//FIXME add enable node interleaving here -- yhlu
	/*needed?
		1. check how many nodes we have , if not all has RAM installed get out
		2. check cs_base lo is 0, node 0 f2 0x40,,,,, if any one is not using lo is CS_BASE, get out
		3. check if other node is the same as node 0 about f2 0x40,,,,, otherwise get out
		4. if all ready enable node_interleaving in f1 0x40..... of every node
		5. for node interleaving we need to set mem hole to every node (need recalcute hole offset in f0 for every node)
	*/

}

void set_sysinfo_in_ram(unsigned val)
{
}

void fill_mem_ctrl(int controllers, struct mem_controller *ctrl_a,
			  const uint16_t *spd_addr)
{
	int i;
	int j;
	struct mem_controller *ctrl;
	for (i = 0; i < controllers; i++) {
		ctrl = &ctrl_a[i];
		ctrl->node_id = i;
		ctrl->f0 = PCI_DEV(0, 0x18+i, 0);
		ctrl->f1 = PCI_DEV(0, 0x18+i, 1);
		ctrl->f2 = PCI_DEV(0, 0x18+i, 2);
		ctrl->f3 = PCI_DEV(0, 0x18+i, 3);

		if (spd_addr == (void *)0) continue;

		for (j = 0; j < DIMM_SOCKETS; j++) {
			ctrl->channel0[j] = spd_addr[(i*2+0)*DIMM_SOCKETS + j];
			ctrl->channel1[j] = spd_addr[(i*2+1)*DIMM_SOCKETS + j];
		}
	}
}
