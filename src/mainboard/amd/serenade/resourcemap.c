/*
 * AMD Serenade needs a different resource map
 *
 */

static void setup_amd_serenade_resource_map(void)
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

	/* Memory-Mapped I/O Base and Limit i Registers */
	/* Base = 0x8000, Limit = 0xffff, Read/Write, DstNode = 0, DstLink = LDT2 */
	PCI_ADDR(0, 0x18, 1, 0xbc), 0x00000048, 0x00ffff20,
	PCI_ADDR(0, 0x18, 1, 0xb8), 0x000000f0, 0x00fc0003,
	/* Base = 0x0a00, Limit = 0x0b00, Read/Write, DstNode = 0, DstLink = LDT2 */
	PCI_ADDR(0, 0x18, 1, 0xac), 0x00000048, 0x00000b20,
	PCI_ADDR(0, 0x18, 1, 0xa8), 0x000000f0, 0x00000a03,
	PCI_ADDR(0, 0x18, 1, 0xb4), 0x00000048, 0x00000000, 
	PCI_ADDR(0, 0x18, 1, 0xb0), 0x000000f0, 0x00000000,
	PCI_ADDR(0, 0x18, 1, 0xac), 0x00000048, 0x00000000,
	PCI_ADDR(0, 0x18, 1, 0xa8), 0x000000f0, 0x00000000,
	PCI_ADDR(0, 0x18, 1, 0xa4), 0x00000048, 0x00000000,
	PCI_ADDR(0, 0x18, 1, 0xa0), 0x000000f0, 0x00000000,
	PCI_ADDR(0, 0x18, 1, 0x9c), 0x00000048, 0x00000000,
	PCI_ADDR(0, 0x18, 1, 0x98), 0x000000f0, 0x00000000,
	PCI_ADDR(0, 0x18, 1, 0x94), 0x00000048, 0x00000000,
	PCI_ADDR(0, 0x18, 1, 0x90), 0x000000f0, 0x00000000,
	PCI_ADDR(0, 0x18, 1, 0x8c), 0x00000048, 0x00000000,
	PCI_ADDR(0, 0x18, 1, 0x88), 0x000000f0, 0x00000000,
	PCI_ADDR(0, 0x18, 1, 0x84), 0x00000048, 0x00000000,
	PCI_ADDR(0, 0x18, 1, 0x80), 0x000000f0, 0x00000000,

	/* PCI I/O Base and Limit i Registers */
	/* Base = 0x0000, Limit = 0x1fff, Read/Write, VAG/ISA enabled,
	   DstNode = 0, DstLink = LDT2 */
	PCI_ADDR(0, 0x18, 1, 0xc4), 0xFE000FC8, 0x01fff020,
	PCI_ADDR(0, 0x18, 1, 0xc0), 0xFE000FCC, 0x00000003,
	PCI_ADDR(0, 0x18, 1, 0xcc), 0xFE000FC8, 0x00000000,
	PCI_ADDR(0, 0x18, 1, 0xc8), 0xFE000FCC, 0x00000000,
	PCI_ADDR(0, 0x18, 1, 0xd4), 0xFE000FC8, 0x00000000,
	PCI_ADDR(0, 0x18, 1, 0xd0), 0xFE000FCC, 0x00000000,
	PCI_ADDR(0, 0x18, 1, 0xdc), 0xFE000FC8, 0x00000000,
	PCI_ADDR(0, 0x18, 1, 0xd8), 0xFE000FCC, 0x00000000,

	/* Config Base and Limit i Registers for Non-coherent HT
	 * Bus 0x00 - 0xff, DstNode = 0, DstLink = LDT2, Read/Write */
	PCI_ADDR(0, 0x18, 1, 0xe0), 0x0000FC88, 0xff000203,
	PCI_ADDR(0, 0x18, 1, 0xe4), 0x0000FC88, 0x00000000,
	PCI_ADDR(0, 0x18, 1, 0xe8), 0x0000FC88, 0x00000000,
	PCI_ADDR(0, 0x18, 1, 0xec), 0x0000FC88, 0x00000000,
        };
        int max;
        max = sizeof(register_values)/sizeof(register_values[0]);
        setup_resource_map(register_values, max);
}
