#define MEMORY_512MB  0  /* SuSE Solo configuration */
#define MEMORY_1024MB 1  /* LNXI Solo configuration */

static void sdram_set_registers(void)
{
	static const unsigned int register_values[] = {
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
	 *         000 = Node 0
	 *         001 = Node 1
	 *         010 = Node 2
	 *         011 = Node 3
	 *         100 = Node 4
	 *         101 = Node 5
	 *         110 = Node 6
	 *         111 = Node 7
	 * [ 7: 3] Reserved
	 * [10: 8] Interleave select
	 *         specifies the values of A[14:12] to use with interleave enable.
	 * [15:11] Reserved
	 * [31:16] DRAM Limit Address i Bits 39-24
	 *         This field defines the upper address bits of a 40 bit  address
	 *         that define the end of the DRAM region.
	 */
#if MEMORY_1024MB
	PCI_ADDR(0, 0x18, 1, 0x44), 0x0000f8f8, 0x003f0000,
#endif
#if MEMORY_512MB
	PCI_ADDR(0, 0x18, 1, 0x44), 0x0000f8f8, 0x001f0000,
#endif
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
	 *         0 = Reads Disabled
	 *         1 = Reads Enabled
	 * [ 1: 1] Write Enable
	 *         0 = Writes Disabled
	 *         1 = Writes Enabled
	 * [ 7: 2] Reserved
	 * [10: 8] Interleave Enable
	 *         000 = No interleave
	 *         001 = Interleave on A[12] (2 nodes)
	 *         010 = reserved
	 *         011 = Interleave on A[12] and A[14] (4 nodes)
	 *         100 = reserved
	 *         101 = reserved
	 *         110 = reserved
	 *         111 = Interleve on A[12] and A[13] and A[14] (8 nodes)
	 * [15:11] Reserved
	 * [13:16] DRAM Base Address i Bits 39-24
	 *         This field defines the upper address bits of a 40-bit address
	 *         that define the start of the DRAM region.
	 */
	PCI_ADDR(0, 0x18, 1, 0x40), 0x0000f8fc, 0x00000003,
#if MEMORY_1024MB
	PCI_ADDR(0, 0x18, 1, 0x48), 0x0000f8fc, 0x00400000,
	PCI_ADDR(0, 0x18, 1, 0x50), 0x0000f8fc, 0x00400000,
	PCI_ADDR(0, 0x18, 1, 0x58), 0x0000f8fc, 0x00400000,
	PCI_ADDR(0, 0x18, 1, 0x60), 0x0000f8fc, 0x00400000,
	PCI_ADDR(0, 0x18, 1, 0x68), 0x0000f8fc, 0x00400000,
	PCI_ADDR(0, 0x18, 1, 0x70), 0x0000f8fc, 0x00400000,
	PCI_ADDR(0, 0x18, 1, 0x78), 0x0000f8fc, 0x00400000,
#endif
#if MEMORY_512MB
	PCI_ADDR(0, 0x18, 1, 0x48), 0x0000f8fc, 0x00200000,
	PCI_ADDR(0, 0x18, 1, 0x50), 0x0000f8fc, 0x00200000,
	PCI_ADDR(0, 0x18, 1, 0x58), 0x0000f8fc, 0x00200000,
	PCI_ADDR(0, 0x18, 1, 0x60), 0x0000f8fc, 0x00200000,
	PCI_ADDR(0, 0x18, 1, 0x68), 0x0000f8fc, 0x00200000,
	PCI_ADDR(0, 0x18, 1, 0x70), 0x0000f8fc, 0x00200000,
	PCI_ADDR(0, 0x18, 1, 0x78), 0x0000f8fc, 0x00200000,
#endif

	/* Memory-Mapped I/O Limit i Registers
	 * F1:0x84 i = 0
	 * F1:0x8C i = 1
	 * F1:0x94 i = 2
	 * F1:0x9C i = 3
	 * F1:0xA4 i = 4
	 * F1:0xAC i = 5
	 * F1:0xB4 i = 6
	 * F1:0xBC i = 7
	 * [ 2: 0] Destination Node ID
	 *         000 = Node 0
	 *         001 = Node 1
	 *         010 = Node 2
	 *         011 = Node 3
	 *         100 = Node 4
	 *         101 = Node 5
	 *         110 = Node 6
	 *         111 = Node 7
	 * [ 3: 3] Reserved
	 * [ 5: 4] Destination Link ID
	 *         00 = Link 0
	 *         01 = Link 1
	 *         10 = Link 2
	 *         11 = Reserved
	 * [ 6: 6] Reserved
	 * [ 7: 7] Non-Posted
	 *         0 = CPU writes may be posted
	 *         1 = CPU writes must be non-posted
	 * [31: 8] Memory-Mapped I/O Limit Address i (39-16)
	 *         This field defines the upp adddress bits of a 40-bit address that
	 *         defines the end of a memory-mapped I/O region n
	 */
	PCI_ADDR(0, 0x18, 1, 0x84), 0x00000048, 0x00e1ff00,
	PCI_ADDR(0, 0x18, 1, 0x8C), 0x00000048, 0x00dfff00,
	PCI_ADDR(0, 0x18, 1, 0x94), 0x00000048, 0x00e3ff00,
	PCI_ADDR(0, 0x18, 1, 0x9C), 0x00000048, 0x00000000,
	PCI_ADDR(0, 0x18, 1, 0xA4), 0x00000048, 0x00000000,
	PCI_ADDR(0, 0x18, 1, 0xAC), 0x00000048, 0x00000000,
	PCI_ADDR(0, 0x18, 1, 0xB4), 0x00000048, 0x00000b00,
	PCI_ADDR(0, 0x18, 1, 0xBC), 0x00000048, 0x00fe0b00,

	/* Memory-Mapped I/O Base i Registers
	 * F1:0x80 i = 0
	 * F1:0x88 i = 1
	 * F1:0x90 i = 2
	 * F1:0x98 i = 3
	 * F1:0xA0 i = 4
	 * F1:0xA8 i = 5
	 * F1:0xB0 i = 6
	 * F1:0xB8 i = 7
	 * [ 0: 0] Read Enable
	 *         0 = Reads disabled
	 *         1 = Reads Enabled
	 * [ 1: 1] Write Enable
	 *         0 = Writes disabled
	 *         1 = Writes Enabled
	 * [ 2: 2] Cpu Disable
	 *         0 = Cpu can use this I/O range
	 *         1 = Cpu requests do not use this I/O range
	 * [ 3: 3] Lock
	 *         0 = base/limit registers i are read/write
	 *         1 = base/limit registers i are read-only
	 * [ 7: 4] Reserved
	 * [31: 8] Memory-Mapped I/O Base Address i (39-16)
	 *         This field defines the upper address bits of a 40bit address 
	 *         that defines the start of memory-mapped I/O region i
	 */
	PCI_ADDR(0, 0x18, 1, 0x80), 0x000000f0, 0x00e00003,
	PCI_ADDR(0, 0x18, 1, 0x88), 0x000000f0, 0x00d80003,
	PCI_ADDR(0, 0x18, 1, 0x90), 0x000000f0, 0x00e20003,
	PCI_ADDR(0, 0x18, 1, 0x98), 0x000000f0, 0x00000000,
	PCI_ADDR(0, 0x18, 1, 0xA0), 0x000000f0, 0x00000000,
	PCI_ADDR(0, 0x18, 1, 0xA8), 0x000000f0, 0x00000000,
	PCI_ADDR(0, 0x18, 1, 0xB0), 0x000000f0, 0x00000a03,
#if MEMORY_1024MB
	PCI_ADDR(0, 0x18, 1, 0xB8), 0x000000f0, 0x00400003,
#endif
#if MEMORY_512MB
	PCI_ADDR(0, 0x18, 1, 0xB8), 0x000000f0, 0x00200003,
#endif

	/* PCI I/O Limit i Registers
	 * F1:0xC4 i = 0
	 * F1:0xCC i = 1
	 * F1:0xD4 i = 2
	 * F1:0xDC i = 3
	 * [ 2: 0] Destination Node ID
	 *         000 = Node 0
	 *         001 = Node 1
	 *         010 = Node 2
	 *         011 = Node 3
	 *         100 = Node 4
	 *         101 = Node 5
	 *         110 = Node 6
	 *         111 = Node 7
	 * [ 3: 3] Reserved
	 * [ 5: 4] Destination Link ID
	 *         00 = Link 0
	 *         01 = Link 1
	 *         10 = Link 2
	 *         11 = reserved
	 * [11: 6] Reserved
	 * [24:12] PCI I/O Limit Address i
	 *         This field defines the end of PCI I/O region n
	 * [31:25] Reserved
	 */
	PCI_ADDR(0, 0x18, 1, 0xC4), 0xFE000FC8, 0x0000d000,
	PCI_ADDR(0, 0x18, 1, 0xCC), 0xFE000FC8, 0x000ff000,
	PCI_ADDR(0, 0x18, 1, 0xD4), 0xFE000FC8, 0x00000000,
	PCI_ADDR(0, 0x18, 1, 0xDC), 0xFE000FC8, 0x00000000,

	/* PCI I/O Base i Registers
	 * F1:0xC0 i = 0
	 * F1:0xC8 i = 1
	 * F1:0xD0 i = 2
	 * F1:0xD8 i = 3
	 * [ 0: 0] Read Enable
	 *         0 = Reads Disabled
	 *         1 = Reads Enabled
	 * [ 1: 1] Write Enable
	 *         0 = Writes Disabled
	 *         1 = Writes Enabled
	 * [ 3: 2] Reserved
	 * [ 4: 4] VGA Enable
	 *         0 = VGA matches Disabled
	 *         1 = matches all address < 64K and where A[9:0] is in the 
	 *             range 3B0-3BB or 3C0-3DF independen of the base & limit registers
	 * [ 5: 5] ISA Enable
	 *         0 = ISA matches Disabled
	 *         1 = Blocks address < 64K and in the last 768 bytes of eack 1K block
	 *             from matching agains this base/limit pair
	 * [11: 6] Reserved
	 * [24:12] PCI I/O Base i
	 *         This field defines the start of PCI I/O region n 
	 * [31:25] Reserved
	 */
	PCI_ADDR(0, 0x18, 1, 0xC0), 0xFE000FCC, 0x0000d003,
	PCI_ADDR(0, 0x18, 1, 0xC8), 0xFE000FCC, 0x00001013,
	PCI_ADDR(0, 0x18, 1, 0xD0), 0xFE000FCC, 0x00000000,
	PCI_ADDR(0, 0x18, 1, 0xD8), 0xFE000FCC, 0x00000000,

	/* Config Base and Limit i Registers
	 * F1:0xE0 i = 0
	 * F1:0xE4 i = 1
	 * F1:0xE8 i = 2
	 * F1:0xEC i = 3
	 * [ 0: 0] Read Enable
	 *         0 = Reads Disabled
	 *         1 = Reads Enabled
	 * [ 1: 1] Write Enable
	 *         0 = Writes Disabled
	 *         1 = Writes Enabled
	 * [ 2: 2] Device Number Compare Enable
	 *         0 = The ranges are based on bus number
	 *         1 = The ranges are ranges of devices on bus 0
	 * [ 3: 3] Reserved
	 * [ 6: 4] Destination Node
	 *         000 = Node 0
	 *         001 = Node 1
	 *         010 = Node 2
	 *         011 = Node 3
	 *         100 = Node 4
	 *         101 = Node 5
	 *         110 = Node 6
	 *         111 = Node 7
	 * [ 7: 7] Reserved
	 * [ 9: 8] Destination Link
	 *         00 = Link 0
	 *         01 = Link 1
	 *         10 = Link 2
	 *         11 - Reserved
	 * [15:10] Reserved
	 * [23:16] Bus Number Base i
	 *         This field defines the lowest bus number in configuration region i
	 * [31:24] Bus Number Limit i
	 *         This field defines the highest bus number in configuration regin i
	 */
	PCI_ADDR(0, 0x18, 1, 0xE0), 0x0000FC88, 0xff000003,
	PCI_ADDR(0, 0x18, 1, 0xE4), 0x0000FC88, 0x00000000,
	PCI_ADDR(0, 0x18, 1, 0xE8), 0x0000FC88, 0x00000000,
	PCI_ADDR(0, 0x18, 1, 0xEC), 0x0000FC88, 0x00000000,

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
	 *         0 = Bank Disabled
	 *         1 = Bank Enabled
	 * [ 8: 1] Reserved
	 * [15: 9] Base Address (19-13)
	 *         An optimization used when all DIMM are the same size...
	 * [20:16] Reserved
	 * [31:21] Base Address (35-25)
	 *         This field defines the top 11 addresses bit of a 40-bit
	 *         address that define the memory address space.  These
	 *         bits decode 32-MByte blocks of memory.
	 */
	PCI_ADDR(0, 0x18, 2, 0x40), 0x001f01fe, 0x00000001,
#if MEMORY_1024MB
	PCI_ADDR(0, 0x18, 2, 0x44), 0x001f01fe, 0x01000001,
	PCI_ADDR(0, 0x18, 2, 0x48), 0x001f01fe, 0x02000001,
	PCI_ADDR(0, 0x18, 2, 0x4C), 0x001f01fe, 0x03000001,
#endif
#if MEMORY_512MB
	PCI_ADDR(0, 0x18, 2, 0x44), 0x001f01fe, 0x00800001,
	PCI_ADDR(0, 0x18, 2, 0x48), 0x001f01fe, 0x01000001,
	PCI_ADDR(0, 0x18, 2, 0x4C), 0x001f01fe, 0x01800001,
#endif
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
	 *         Address to be excluded from the optimized case
	 * [20:16] Reserved
	 * [29:21] Address Mask (33-25)
	 *         The bits with an address mask of 1 are excluded from address comparison
	 * [31:30] Reserved
	 * 
	 */
#if MEMORY_1024MB
	PCI_ADDR(0, 0x18, 2, 0x60), 0xC01f01ff, 0x00e0fe00,
	PCI_ADDR(0, 0x18, 2, 0x64), 0xC01f01ff, 0x00e0fe00,
	PCI_ADDR(0, 0x18, 2, 0x68), 0xC01f01ff, 0x00e0fe00,
	PCI_ADDR(0, 0x18, 2, 0x6C), 0xC01f01ff, 0x00e0fe00,
#endif
#if MEMORY_512MB
	PCI_ADDR(0, 0x18, 2, 0x60), 0xC01f01ff, 0x0060fe00,
	PCI_ADDR(0, 0x18, 2, 0x64), 0xC01f01ff, 0x0060fe00,
	PCI_ADDR(0, 0x18, 2, 0x68), 0xC01f01ff, 0x0060fe00,
	PCI_ADDR(0, 0x18, 2, 0x6C), 0xC01f01ff, 0x0060fe00,
#endif
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
	 *         000 = 32Mbyte  (Rows = 12 & Col =  8)
	 *         001 = 64Mbyte  (Rows = 12 & Col =  9)
	 *         010 = 128Mbyte (Rows = 13 & Col =  9)|(Rows = 12 & Col = 10)
	 *         011 = 256Mbyte (Rows = 13 & Col = 10)|(Rows = 12 & Col = 11)
	 *         100 = 512Mbyte (Rows = 13 & Col = 11)|(Rows = 14 & Col = 10)
	 *         101 = 1Gbyte   (Rows = 14 & Col = 11)|(Rows = 13 & Col = 12)
	 *         110 = 2Gbyte   (Rows = 14 & Col = 12)
	 *         111 = reserved 
	 * [ 3: 3] Reserved
	 * [ 7: 7] Reserved
	 * [11:11] Reserved
	 * [31:15]
	 */
#if MEMORY_1024MB
	PCI_ADDR(0, 0x18, 2, 0x80), 0xffff8888, 0x00000033,
#endif
#if MEMORY_512MB
	PCI_ADDR(0, 0x18, 2, 0x80), 0xffff8888, 0x00000022,
#endif
	/* DRAM Timing Low Register
	 * F2:0x88
	 * [ 2: 0] Tcl (Cas# Latency, Cas# to read-data-valid)
	 *         000 = reserved
	 *         001 = CL 2
	 *         010 = CL 3
	 *         011 = reserved
	 *         100 = reserved
	 *         101 = CL 2.5
	 *         110 = reserved
	 *         111 = reserved
	 * [ 3: 3] Reserved
	 * [ 7: 4] Trc (Row Cycle Time, Ras#-active to Ras#-active/bank auto refresh)
	 *         0000 =  7 bus clocks
	 *         0001 =  8 bus clocks
	 *         ...
	 *         1110 = 21 bus clocks
	 *         1111 = 22 bus clocks
	 * [11: 8] Trfc (Row refresh Cycle time, Auto-refresh-active to RAS#-active or RAS#auto-refresh)
	 *         0000 = 9 bus clocks
	 *         0010 = 10 bus clocks
	 *         ....
	 *         1110 = 23 bus clocks
	 *         1111 = 24 bus clocks
	 * [14:12] Trcd (Ras#-active to Case#-read/write Delay)
	 *         000 = reserved
	 *         001 = reserved
	 *         010 = 2 bus clocks
	 *         011 = 3 bus clocks
	 *         100 = 4 bus clocks
	 *         101 = 5 bus clocks
	 *         110 = 6 bus clocks
	 *         111 = reserved
	 * [15:15] Reserved
	 * [18:16] Trrd (Ras# to Ras# Delay)
	 *         000 = reserved
	 *         001 = reserved
	 *         010 = 2 bus clocks
	 *         011 = 3 bus clocks
	 *         100 = 4 bus clocks
	 *         101 = reserved
	 *         110 = reserved
	 *         111 = reserved
	 * [19:19] Reserved
	 * [23:20] Tras (Minmum Ras# Active Time)
	 *         0000 to 0100 = reserved
	 *         0101 = 5 bus clocks
	 *         ...
	 *         1111 = 15 bus clocks
	 * [26:24] Trp (Row Precharge Time)
	 *         000 = reserved
	 *         001 = reserved
	 *         010 = 2 bus clocks
	 *         011 = 3 bus clocks
	 *         100 = 4 bus clocks
	 *         101 = 5 bus clocks
	 *         110 = 6 bus clocks
	 *         111 = reserved
	 * [27:27] Reserved
	 * [28:28] Twr (Write Recovery Time)
	 *         0 = 2 bus clocks
	 *         1 = 3 bus clocks
	 * [31:29] Reserved
	 */
	PCI_ADDR(0, 0x18, 2, 0x88), 0xe8088008, 0x03623125,
	/* DRAM Timing High Register
	 * F2:0x8C
	 * [ 0: 0] Twtr (Write to Read Delay)
	 *         0 = 1 bus Clocks
	 *         1 = 2 bus Clocks
	 * [ 3: 1] Reserved
	 * [ 6: 4] Trwf (Read to Write Delay)
	 *         000 = 1 bus clocks
	 *         001 = 2 bus clocks
	 *         010 = 3 bus clocks
	 *         011 = 4 bus clocks
	 *         100 = 5 bus clocks
	 *         101 = 6 bus clocks
	 *         110 = reserved
	 *         111 = reserved
	 * [ 7: 7] Reserved
	 * [12: 8] Tref (Refresh Rate)
	 *         00000 = 100Mhz 4K rows
	 *         00001 = 133Mhz 4K rows
	 *         00010 = 166Mhz 4K rows
	 *         01000 = 100Mhz 8K/16K rows
	 *         01001 = 133Mhz 8K/16K rows
	 *         01010 = 166Mhz 8K/16K rows
	 * [19:13] Reserved
	 * [22:20] Twcl (Write CAS Latency)
	 *         000 = 1 Mem clock after CAS# (Unbuffered Dimms)
	 *         001 = 2 Mem clocks after CAS# (Registered Dimms)
	 * [31:23] Reserved
	 */
#if MEMORY_1024MB
	PCI_ADDR(0, 0x18, 2, 0x8c), 0xff8fe08e, 0x00000930,
#endif
#if MEMORY_512MB
	PCI_ADDR(0, 0x18, 2, 0x8c), 0xff8fe08e, 0x00000130,
#endif

	/* DRAM Config Low Register
	 * F2:0x90
	 * [ 0: 0] DLL Disable
	 *         0 = Enabled
	 *         1 = Disabled
	 * [ 1: 1] D_DRV
	 *         0 = Normal Drive
	 *         1 = Weak Drive
	 * [ 2: 2] QFC_EN
	 *         0 = Disabled
	 *         1 = Enabled
	 * [ 3: 3] Disable DQS Hystersis  (FIXME handle this one carefully)
	 *         0 = Enable DQS input filter 
	 *         1 = Disable DQS input filtering 
	 * [ 7: 4] Reserved
	 * [ 8: 8] DRAM_Init
	 *         0 = Initialization done or not yet started.
	 *         1 = Initiate DRAM intialization sequence
	 * [ 9: 9] SO-Dimm Enable
	 *         0 = Do nothing
	 *         1 = SO-Dimms present
	 * [10:10] DramEnable
	 *         0 = DRAM not enabled
	 *         1 = DRAM initialized and enabled
	 * [11:11] Memory Clear Status
	 *         0 = Memory Clear function has not completed
	 *         1 = Memory Clear function has completed
	 * [12:12] Exit Self-Refresh
	 *         0 = Exit from self-refresh done or not yet started
	 *         1 = DRAM exiting from self refresh
	 * [13:13] Self-Refresh Status
	 *         0 = Normal Operation
	 *         1 = Self-refresh mode active
	 * [15:14] Read/Write Queue Bypass Count
	 *         00 = 2
	 *         01 = 4
	 *         10 = 8
	 *         11 = 16
	 * [16:16] 128-bit/64-Bit
	 *         0 = 64bit Interface to DRAM
	 *         1 = 128bit Interface to DRAM
	 * [17:17] DIMM ECC Enable
	 *         0 = Some DIMMs do not have ECC
	 *         1 = ALL DIMMS have ECC bits
	 * [18:18] UnBuffered DIMMs
	 *         0 = Buffered DIMMS
	 *         1 = Unbuffered DIMMS
	 * [19:19] Enable 32-Byte Granularity
	 *         0 = Optimize for 64byte bursts
	 *         1 = Optimize for 32byte bursts
	 * [20:20] DIMM 0 is x4
	 * [21:21] DIMM 1 is x4
	 * [22:22] DIMM 2 is x4
	 * [23:23] DIMM 3 is x4
	 *         0 = DIMM is not x4
	 *         1 = x4 DIMM present
	 * [24:24] Disable DRAM Receivers
	 *         0 = Receivers enabled
	 *         1 = Receivers disabled
	 * [27:25] Bypass Max
	 *         000 = Arbiters chois is always respected
	 *         001 = Oldest entry in DCQ can be bypassed 1 time
	 *         010 = Oldest entry in DCQ can be bypassed 2 times
	 *         011 = Oldest entry in DCQ can be bypassed 3 times
	 *         100 = Oldest entry in DCQ can be bypassed 4 times
	 *         101 = Oldest entry in DCQ can be bypassed 5 times
	 *         110 = Oldest entry in DCQ can be bypassed 6 times
	 *         111 = Oldest entry in DCQ can be bypassed 7 times
	 * [31:28] Reserved
	 */
	PCI_ADDR(0, 0x18, 2, 0x90), 0xf0000000, 
	(4 << 25)|(0 << 24)| 
	(0 << 23)|(0 << 22)|(0 << 21)|(0 << 20)| 
	(1 << 19)|(1 << 18)|(0 << 17)|(0 << 16)| 
	(2 << 14)|(0 << 13)|(0 << 12)| 
	(0 << 11)|(0 << 10)|(0 << 9)|(0 << 8)| 
	(0 << 3) |(0 << 1) |(0 << 0),
	/* DRAM Config High Register
	 * F2:0x94
	 * [ 0: 3] Maximum Asynchronous Latency
	 *         0000 = 0 ns
	 *         ...
	 *         1111 = 15 ns
	 * [ 7: 4] Reserved
	 * [11: 8] Read Preamble
	 *         0000 = 2.0 ns
	 *         0001 = 2.5 ns
	 *         0010 = 3.0 ns
	 *         0011 = 3.5 ns
	 *         0100 = 4.0 ns
	 *         0101 = 4.5 ns
	 *         0110 = 5.0 ns
	 *         0111 = 5.5 ns
	 *         1000 = 6.0 ns
	 *         1001 = 6.5 ns
	 *         1010 = 7.0 ns
	 *         1011 = 7.5 ns
	 *         1100 = 8.0 ns
	 *         1101 = 8.5 ns
	 *         1110 = 9.0 ns
	 *         1111 = 9.5 ns
	 * [15:12] Reserved
	 * [18:16] Idle Cycle Limit
	 *         000 = 0 cycles
	 *         001 = 4 cycles
	 *         010 = 8 cycles
	 *         011 = 16 cycles
	 *         100 = 32 cycles
	 *         101 = 64 cycles
	 *         110 = 128 cycles
	 *         111 = 256 cycles
	 * [19:19] Dynamic Idle Cycle Center Enable
	 *         0 = Use Idle Cycle Limit
	 *         1 = Generate a dynamic Idle cycle limit
	 * [22:20] DRAM MEMCLK Frequency
	 *         000 = 100Mhz
	 *         001 = reserved
	 *         010 = 133Mhz
	 *         011 = reserved
	 *         100 = reserved
	 *         101 = 166Mhz
	 *         110 = reserved
	 *         111 = reserved
	 * [24:23] Reserved
	 * [25:25] Memory Clock Ratio Valid (FIXME carefully enable memclk)
	 *         0 = Disable MemClks
	 *         1 = Enable MemClks
	 * [26:26] Memory Clock 0 Enable
	 *         0 = Disabled
	 *         1 = Enabled
	 * [27:27] Memory Clock 1 Enable
	 *         0 = Disabled
	 *         1 = Enabled
	 * [28:28] Memory Clock 2 Enable
	 *         0 = Disabled
	 *         1 = Enabled
	 * [29:29] Memory Clock 3 Enable
	 *         0 = Disabled
	 *         1 = Enabled
	 * [31:30] Reserved
	 */
#if MEMORY_1024MB
	PCI_ADDR(0, 0x18, 2, 0x94), 0xc180f0f0, 0x0e2b0a05,
#endif
#if MEMORY_512MB
	PCI_ADDR(0, 0x18, 2, 0x94), 0xc180f0f0, 0x0e2b0a06,
#endif
	/* DRAM Delay Line Register
	 * F2:0x98
	 * Adjust the skew of the input DQS strobe relative to DATA
	 * [15: 0] Reserved
	 * [23:16] Delay Line Adjust
	 *         Adjusts the DLL derived PDL delay by one or more delay stages
	 *         in either the faster or slower direction.
	 * [24:24} Adjust Slower
	 *         0 = Do Nothing
	 *         1 = Adj is used to increase the PDL delay
	 * [25:25] Adjust Faster
	 *         0 = Do Nothing
	 *         1 = Adj is used to decrease the PDL delay
	 * [31:26] Reserved
	 */
	PCI_ADDR(0, 0x18, 2, 0x98), 0xfc00ffff, 0x00000000,
	/* DRAM Scrub Control Register
	 * F3:0x58
	 * [ 4: 0] DRAM Scrube Rate
	 * [ 7: 5] reserved
	 * [12: 8] L2 Scrub Rate
	 * [15:13] reserved
	 * [20:16] Dcache Scrub
	 * [31:21] reserved
	 *         Scrub Rates
	 *         00000 = Do not scrub
	 *         00001 =  40.00 ns
	 *         00010 =  80.00 ns
	 *         00011 = 160.00 ns
	 *         00100 = 320.00 ns
	 *         00101 = 640.00 ns
	 *         00110 =   1.28 us
	 *         00111 =   2.56 us
	 *         01000 =   5.12 us
	 *         01001 =  10.20 us
	 *         01011 =  41.00 us
	 *         01100 =  81.90 us
	 *         01101 = 163.80 us
	 *         01110 = 327.70 us
	 *         01111 = 655.40 us
	 *         10000 =   1.31 ms
	 *         10001 =   2.62 ms
	 *         10010 =   5.24 ms
	 *         10011 =  10.49 ms
	 *         10100 =  20.97 ms
	 *         10101 =  42.00 ms
	 *         10110 =  84.00 ms
	 *         All Others = Reserved
	 */
	PCI_ADDR(0, 0x18, 3, 0x58), 0xffe0e0e0, 0x00000000,
	/* DRAM Scrub Address Low Register
	 * F3:0x5C
	 * [ 0: 0] DRAM Scrubber Redirect Enable
	 *         0 = Do nothing
	 *         1 = Scrubber Corrects errors found in normal operation
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
	print_debug("setting up CPU0 northbridge registers\r\n");
	max = sizeof(register_values)/sizeof(register_values[0]);
	for(i = 0; i < max; i += 3) {
		unsigned long reg;
#if 0
		print_debug_hex32(register_values[i]);
		print_debug(" <-");
		print_debug_hex32(register_values[i+2]);
		print_debug("\r\n");
#endif
		reg = pci_read_config32(register_values[i]);
		reg &= register_values[i+1];
		reg |= register_values[i+2];
		pci_write_config32(register_values[i], reg);
	}
	print_debug("done.\r\n");
}

#define DRAM_CONFIG_LOW 0x90
#define  DCL_DLL_Disable   (1<<0)
#define  DCL_D_DRV         (1<<1)
#define  DCL_QFC_EN        (1<<2)
#define  DCL_DisDqsHys     (1<<3)
#define  DCL_DramInit      (1<<8)
#define  DCL_DramEnable    (1<<10)
#define  DCL_MemClrStatus  (1<<11)
#define  DCL_DimmEcEn      (1<<17)

#define NODE_ID		0x60
#define	HT_INIT_CONTROL 0x6c

#define HTIC_ColdR_Detect  (1<<4)
#define HTIC_BIOSR_Detect  (1<<5)
#define HTIC_INIT_Detect   (1<<6)

static void sdram_set_spd_registers(void) 
{
	unsigned long dcl;
	dcl = pci_read_config32(PCI_ADDR(0, 0x18, 2, DRAM_CONFIG_LOW));
	/* Until I know what is going on disable ECC support */
	dcl &= ~DCL_DimmEcEn;
	pci_write_config32(PCI_ADDR(0, 0x18, 2, DRAM_CONFIG_LOW), dcl);
}

#define TIMEOUT_LOOPS 300000
static void sdram_enable(void)
{
	unsigned long dcl;

	/* Toggle DisDqsHys to get it working */
	dcl = pci_read_config32(PCI_ADDR(0, 0x18, 2, DRAM_CONFIG_LOW));
	print_debug("dcl: ");
	print_debug_hex32(dcl);
	print_debug("\r\n");
	dcl |= DCL_DisDqsHys;
	pci_write_config32(PCI_ADDR(0, 0x18, 2, DRAM_CONFIG_LOW), dcl);
	dcl &= ~DCL_DisDqsHys;
	dcl &= ~DCL_DLL_Disable;
	dcl &= ~DCL_D_DRV;
	dcl &= ~DCL_QFC_EN;
	dcl |= DCL_DramInit;
	pci_write_config32(PCI_ADDR(0, 0x18, 2, DRAM_CONFIG_LOW), dcl);
	
	print_debug("Initializing memory: ");
	int loops = 0;
	do {
		dcl = pci_read_config32(PCI_ADDR(0, 0x18, 2, DRAM_CONFIG_LOW));
		loops += 1;
		if ((loops & 1023) == 0) {
			print_debug(".");
		}
	} while(((dcl & DCL_DramInit) != 0) && (loops < TIMEOUT_LOOPS));
	if (loops >= TIMEOUT_LOOPS) {
		print_debug(" failed\r\n");
	} else {
		print_debug(" done\r\n");
	}

#if 0
	print_debug("Clearing memory: ");
	loops = 0;
	do {
		dcl = pci_read_config32(PCI_ADDR(0, 0x18, 2, DRAM_CONFIG_LOW));
		loops += 1;
		if ((loops & 1023) == 0) {
			print_debug(" ");
			print_debug_hex32(loops);
		}
	} while(((dcl & DCL_MemClrStatus) == 0) && (loops < TIMEOUT_LOOPS));
	if (loops >= TIMEOUT_LOOPS) {
		print_debug("failed\r\n");
	} else {
		print_debug("done\r\n");
	}
#endif
}

static void sdram_first_normal_reference(void) {}
static void sdram_enable_refresh(void) {}
static void sdram_special_finishup(void) {}

