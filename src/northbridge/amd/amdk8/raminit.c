#include <cpu/k8/mtrr.h>
#include "raminit.h"

/* Function 2 */
#define DRAM_CSBASE	   0x40
#define DRAM_CSMASK	   0x60
#define DRAM_BANK_ADDR_MAP 0x80
#define DRAM_TIMING_LOW	   0x88
#define	 DTL_TCL_SHIFT	   0
#define	 DTL_TCL_MASK	   0x7
#define	  DTL_CL_2	   1
#define	  DTL_CL_3	   2
#define	  DTL_CL_2_5	   5
#define	 DTL_TRC_SHIFT	   4
#define	 DTL_TRC_MASK	   0xf
#define	  DTL_TRC_BASE	   7
#define	  DTL_TRC_MIN	   7
#define	  DTL_TRC_MAX	   22
#define	 DTL_TRFC_SHIFT	   8
#define	 DTL_TRFC_MASK	   0xf
#define	  DTL_TRFC_BASE	   9
#define	  DTL_TRFC_MIN	   9
#define	  DTL_TRFC_MAX	   24
#define	 DTL_TRCD_SHIFT	   12
#define	 DTL_TRCD_MASK	   0x7
#define	  DTL_TRCD_BASE	   0
#define	  DTL_TRCD_MIN	   2
#define	  DTL_TRCD_MAX	   6
#define	 DTL_TRRD_SHIFT	   16
#define	 DTL_TRRD_MASK	   0x7
#define	  DTL_TRRD_BASE	   0
#define	  DTL_TRRD_MIN	   2
#define	  DTL_TRRD_MAX	   4
#define	 DTL_TRAS_SHIFT	   20
#define	 DTL_TRAS_MASK	   0xf
#define	  DTL_TRAS_BASE	   0
#define	  DTL_TRAS_MIN	   5
#define	  DTL_TRAS_MAX	   15
#define	 DTL_TRP_SHIFT	   24
#define	 DTL_TRP_MASK	   0x7
#define	  DTL_TRP_BASE	   0
#define	  DTL_TRP_MIN	   2
#define	  DTL_TRP_MAX	   6
#define	 DTL_TWR_SHIFT	   28
#define	 DTL_TWR_MASK	   0x1
#define	  DTL_TWR_BASE	   2
#define	  DTL_TWR_MIN	   2
#define	  DTL_TWR_MAX	   3
#define DRAM_TIMING_HIGH   0x8c
#define	 DTH_TWTR_SHIFT	   0
#define	 DTH_TWTR_MASK	   0x1
#define	  DTH_TWTR_BASE	   1
#define	  DTH_TWTR_MIN	   1
#define	  DTH_TWTR_MAX	   2
#define	 DTH_TRWT_SHIFT	   4
#define	 DTH_TRWT_MASK	   0x7
#define	  DTH_TRWT_BASE	   1
#define	  DTH_TRWT_MIN	   1
#define	  DTH_TRWT_MAX	   6
#define	 DTH_TREF_SHIFT	   8
#define	 DTH_TREF_MASK	   0x1f
#define	  DTH_TREF_100MHZ_4K 0x00
#define	  DTH_TREF_133MHZ_4K 0x01
#define	  DTH_TREF_166MHZ_4K 0x02
#define	  DTH_TREF_200MHZ_4K 0x03
#define	  DTH_TREF_100MHZ_8K 0x08
#define	  DTH_TREF_133MHZ_8K 0x09
#define	  DTH_TREF_166MHZ_8K 0x0A
#define	  DTH_TREF_200MHZ_8K 0x0B
#define	 DTH_TWCL_SHIFT	    20
#define	 DTH_TWCL_MASK	    0x7
#define	  DTH_TWCL_BASE	    1
#define	  DTH_TWCL_MIN	    1
#define	  DTH_TWCL_MAX	    2
#define DRAM_CONFIG_LOW	   0x90
#define	 DCL_DLL_Disable   (1<<0)
#define	 DCL_D_DRV	   (1<<1)
#define	 DCL_QFC_EN	   (1<<2)
#define	 DCL_DisDqsHys	   (1<<3)
#define	 DCL_DramInit	   (1<<8)
#define	 DCL_DramEnable	   (1<<10)
#define	 DCL_MemClrStatus  (1<<11)
#define	 DCL_ESR	   (1<<12)
#define	 DCL_SRS	   (1<<13)
#define	 DCL_128BitEn	   (1<<16)
#define	 DCL_DimmEccEn	   (1<<17)
#define	 DCL_UnBufDimm	   (1<<18)
#define	 DCL_32ByteEn	   (1<<19)
#define	 DCL_x4DIMM_SHIFT  20
#define DRAM_CONFIG_HIGH   0x94
#define	 DCH_ASYNC_LAT_SHIFT  0
#define	 DCH_ASYNC_LAT_MASK   0xf
#define	  DCH_ASYNC_LAT_BASE  0
#define	  DCH_ASYNC_LAT_MIN   0
#define	  DCH_ASYNC_LAT_MAX   15
#define	 DCH_RDPREAMBLE_SHIFT 8
#define	 DCH_RDPREAMBLE_MASK  0xf
#define	  DCH_RDPREAMBLE_BASE ((2<<1)+0) /* 2.0 ns */
#define	  DCH_RDPREAMBLE_MIN  ((2<<1)+0) /* 2.0 ns */
#define	  DCH_RDPREAMBLE_MAX  ((9<<1)+1) /* 9.5 ns */
#define	 DCH_IDLE_LIMIT_SHIFT 16
#define	 DCH_IDLE_LIMIT_MASK  0x7
#define	  DCH_IDLE_LIMIT_0    0
#define	  DCH_IDLE_LIMIT_4    1
#define	  DCH_IDLE_LIMIT_8    2
#define	  DCH_IDLE_LIMIT_16   3
#define	  DCH_IDLE_LIMIT_32   4
#define	  DCH_IDLE_LIMIT_64   5
#define	  DCH_IDLE_LIMIT_128  6
#define	  DCH_IDLE_LIMIT_256  7
#define	 DCH_DYN_IDLE_CTR_EN (1 << 19)
#define	 DCH_MEMCLK_SHIFT     20
#define	 DCH_MEMCLK_MASK      0x7
#define	  DCH_MEMCLK_100MHZ   0
#define	  DCH_MEMCLK_133MHZ   2
#define	  DCH_MEMCLK_166MHZ   5
#define	  DCH_MEMCLK_200MHZ   7
#define	 DCH_MEMCLK_VALID     (1 << 25)
#define	 DCH_MEMCLK_EN0	      (1 << 26) 
#define	 DCH_MEMCLK_EN1	      (1 << 27) 
#define	 DCH_MEMCLK_EN2	      (1 << 28) 
#define	 DCH_MEMCLK_EN3	      (1 << 29) 

/* Function 3 */
#define SCRUB_CONTROL	   0x58
#define	  SCRUB_NONE	    0
#define	  SCRUB_40ns	    1
#define	  SCRUB_80ns	    2
#define	  SCRUB_160ns	    3
#define	  SCRUB_320ns	    4
#define	  SCRUB_640ns	    5
#define	  SCRUB_1_28us	    6
#define	  SCRUB_2_56us	    7
#define	  SCRUB_5_12us	    8
#define	  SCRUB_10_2us	    9
#define	  SCRUB_20_5us	   10
#define	  SCRUB_41_0us	   11
#define	  SCRUB_81_9us	   12
#define	  SCRUB_163_8us	   13
#define	  SCRUB_327_7us	   14
#define	  SCRUB_655_4us	   15
#define	  SCRUB_1_31ms	   16
#define	  SCRUB_2_62ms	   17
#define	  SCRUB_5_24ms	   18 
#define	  SCRUB_10_49ms	   19
#define	  SCRUB_20_97ms	   20
#define	  SCRUB_42ms	   21
#define	  SCRUB_84ms	   22
#define	 SC_DRAM_SCRUB_RATE_SHFIT  0
#define	 SC_DRAM_SCRUB_RATE_MASK   0x1f
#define	 SC_L2_SCRUB_RATE_SHIFT	   8
#define	 SC_L2_SCRUB_RATE_MASK	   0x1f
#define	 SC_L1D_SCRUB_RATE_SHIFT   16
#define	 SC_L1D_SCRUB_RATE_MASK	   0x1f
#define SCRUB_ADDR_LOW	   0x5C
#define SCRUB_ADDR_HIGH	   0x60
#define NORTHBRIDGE_CAP	   0xE8
#define	 NBCAP_128Bit	      0x0001
#define	 NBCAP_MP	      0x0002
#define	 NBCAP_BIG_MP	      0x0004
#define	 NBCAP_ECC	      0x0004
#define	 NBCAP_CHIPKILL_ECC   0x0010
#define	 NBCAP_MEMCLK_SHIFT   5
#define	 NBCAP_MEMCLK_MASK    3
#define	 NBCAP_MEMCLK_100MHZ  3
#define	 NBCAP_MEMCLK_133MHZ  2
#define	 NBCAP_MEMCLK_166MHZ  1
#define	 NBCAP_MEMCLK_200MHZ  0
#define	 NBCAP_MEMCTRL	      0x0100


static void setup_resource_map(const unsigned int *register_values, int max)
{
	int i;
	print_debug("setting up resource map....\r\n");
	for(i = 0; i < max; i += 3) {
		device_t dev;
		unsigned where;
		unsigned long reg;
#if 0
		print_debug_hex32(register_values[i]);
		print_debug(" <-");
		print_debug_hex32(register_values[i+2]);
		print_debug("\r\n");
#endif
		dev = register_values[i] & ~0xff;
		where = register_values[i] & 0xff;
		reg = pci_read_config32(dev, where);
		reg &= register_values[i+1];
		reg |= register_values[i+2];
		pci_write_config32(dev, where, reg);
#if 0
		reg = pci_read_config32(register_values[i]);
		reg &= register_values[i+1];
		reg |= register_values[i+2] & ~register_values[i+1];
		pci_write_config32(register_values[i], reg);
#endif
	}
	print_debug("done.\r\n");
}

static void setup_default_resource_map(void)
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
	 *	   000 = Node 0
	 *	   001 = Node 1
	 *	   010 = Node 2
	 *	   011 = Node 3
	 *	   100 = Node 4
	 *	   101 = Node 5
	 *	   110 = Node 6
	 *	   111 = Node 7
	 * [ 3: 3] Reserved
	 * [ 5: 4] Destination Link ID
	 *	   00 = Link 0
	 *	   01 = Link 1
	 *	   10 = Link 2
	 *	   11 = Reserved
	 * [ 6: 6] Reserved
	 * [ 7: 7] Non-Posted
	 *	   0 = CPU writes may be posted
	 *	   1 = CPU writes must be non-posted
	 * [31: 8] Memory-Mapped I/O Limit Address i (39-16)
	 *	   This field defines the upp adddress bits of a 40-bit address that
	 *	   defines the end of a memory-mapped I/O region n
	 */
	PCI_ADDR(0, 0x18, 1, 0x84), 0x00000048, 0x00000000,
	PCI_ADDR(0, 0x18, 1, 0x8C), 0x00000048, 0x00000000,
	PCI_ADDR(0, 0x18, 1, 0x94), 0x00000048, 0x00000000,
	PCI_ADDR(0, 0x18, 1, 0x9C), 0x00000048, 0x00000000,
	PCI_ADDR(0, 0x18, 1, 0xA4), 0x00000048, 0x00000000,
	PCI_ADDR(0, 0x18, 1, 0xAC), 0x00000048, 0x00000000,
	PCI_ADDR(0, 0x18, 1, 0xB4), 0x00000048, 0x00000000,
	PCI_ADDR(0, 0x18, 1, 0xBC), 0x00000048, 0x00ffff00,

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
	 *	   0 = Reads disabled
	 *	   1 = Reads Enabled
	 * [ 1: 1] Write Enable
	 *	   0 = Writes disabled
	 *	   1 = Writes Enabled
	 * [ 2: 2] Cpu Disable
	 *	   0 = Cpu can use this I/O range
	 *	   1 = Cpu requests do not use this I/O range
	 * [ 3: 3] Lock
	 *	   0 = base/limit registers i are read/write
	 *	   1 = base/limit registers i are read-only
	 * [ 7: 4] Reserved
	 * [31: 8] Memory-Mapped I/O Base Address i (39-16)
	 *	   This field defines the upper address bits of a 40bit address 
	 *	   that defines the start of memory-mapped I/O region i
	 */
	PCI_ADDR(0, 0x18, 1, 0x80), 0x000000f0, 0x00000000,
	PCI_ADDR(0, 0x18, 1, 0x88), 0x000000f0, 0x00000000,
	PCI_ADDR(0, 0x18, 1, 0x90), 0x000000f0, 0x00000000,
	PCI_ADDR(0, 0x18, 1, 0x98), 0x000000f0, 0x00000000,
	PCI_ADDR(0, 0x18, 1, 0xA0), 0x000000f0, 0x00000000,
	PCI_ADDR(0, 0x18, 1, 0xA8), 0x000000f0, 0x00000000,
	PCI_ADDR(0, 0x18, 1, 0xB0), 0x000000f0, 0x00000000,
	PCI_ADDR(0, 0x18, 1, 0xB8), 0x000000f0, 0x00fc0003,

	/* PCI I/O Limit i Registers
	 * F1:0xC4 i = 0
	 * F1:0xCC i = 1
	 * F1:0xD4 i = 2
	 * F1:0xDC i = 3
	 * [ 2: 0] Destination Node ID
	 *	   000 = Node 0
	 *	   001 = Node 1
	 *	   010 = Node 2
	 *	   011 = Node 3
	 *	   100 = Node 4
	 *	   101 = Node 5
	 *	   110 = Node 6
	 *	   111 = Node 7
	 * [ 3: 3] Reserved
	 * [ 5: 4] Destination Link ID
	 *	   00 = Link 0
	 *	   01 = Link 1
	 *	   10 = Link 2
	 *	   11 = reserved
	 * [11: 6] Reserved
	 * [24:12] PCI I/O Limit Address i
	 *	   This field defines the end of PCI I/O region n
	 * [31:25] Reserved
	 */
	PCI_ADDR(0, 0x18, 1, 0xC4), 0xFE000FC8, 0x01fff000,
	PCI_ADDR(0, 0x18, 1, 0xCC), 0xFE000FC8, 0x00000000,
	PCI_ADDR(0, 0x18, 1, 0xD4), 0xFE000FC8, 0x00000000,
	PCI_ADDR(0, 0x18, 1, 0xDC), 0xFE000FC8, 0x00000000,

	/* PCI I/O Base i Registers
	 * F1:0xC0 i = 0
	 * F1:0xC8 i = 1
	 * F1:0xD0 i = 2
	 * F1:0xD8 i = 3
	 * [ 0: 0] Read Enable
	 *	   0 = Reads Disabled
	 *	   1 = Reads Enabled
	 * [ 1: 1] Write Enable
	 *	   0 = Writes Disabled
	 *	   1 = Writes Enabled
	 * [ 3: 2] Reserved
	 * [ 4: 4] VGA Enable
	 *	   0 = VGA matches Disabled
	 *	   1 = matches all address < 64K and where A[9:0] is in the 
	 *	       range 3B0-3BB or 3C0-3DF independen of the base & limit registers
	 * [ 5: 5] ISA Enable
	 *	   0 = ISA matches Disabled
	 *	   1 = Blocks address < 64K and in the last 768 bytes of eack 1K block
	 *	       from matching agains this base/limit pair
	 * [11: 6] Reserved
	 * [24:12] PCI I/O Base i
	 *	   This field defines the start of PCI I/O region n 
	 * [31:25] Reserved
	 */
	PCI_ADDR(0, 0x18, 1, 0xC0), 0xFE000FCC, 0x00000003,
	PCI_ADDR(0, 0x18, 1, 0xC8), 0xFE000FCC, 0x00000000,
	PCI_ADDR(0, 0x18, 1, 0xD0), 0xFE000FCC, 0x00000000,
	PCI_ADDR(0, 0x18, 1, 0xD8), 0xFE000FCC, 0x00000000,

	/* Config Base and Limit i Registers
	 * F1:0xE0 i = 0
	 * F1:0xE4 i = 1
	 * F1:0xE8 i = 2
	 * F1:0xEC i = 3
	 * [ 0: 0] Read Enable
	 *	   0 = Reads Disabled
	 *	   1 = Reads Enabled
	 * [ 1: 1] Write Enable
	 *	   0 = Writes Disabled
	 *	   1 = Writes Enabled
	 * [ 2: 2] Device Number Compare Enable
	 *	   0 = The ranges are based on bus number
	 *	   1 = The ranges are ranges of devices on bus 0
	 * [ 3: 3] Reserved
	 * [ 6: 4] Destination Node
	 *	   000 = Node 0
	 *	   001 = Node 1
	 *	   010 = Node 2
	 *	   011 = Node 3
	 *	   100 = Node 4
	 *	   101 = Node 5
	 *	   110 = Node 6
	 *	   111 = Node 7
	 * [ 7: 7] Reserved
	 * [ 9: 8] Destination Link
	 *	   00 = Link 0
	 *	   01 = Link 1
	 *	   10 = Link 2
	 *	   11 - Reserved
	 * [15:10] Reserved
	 * [23:16] Bus Number Base i
	 *	   This field defines the lowest bus number in configuration region i
	 * [31:24] Bus Number Limit i
	 *	   This field defines the highest bus number in configuration regin i
	 */
	PCI_ADDR(0, 0x18, 1, 0xE0), 0x0000FC88, 0xff000003,
	PCI_ADDR(0, 0x18, 1, 0xE4), 0x0000FC88, 0x00000000,
	PCI_ADDR(0, 0x18, 1, 0xE8), 0x0000FC88, 0x00000000,
	PCI_ADDR(0, 0x18, 1, 0xEC), 0x0000FC88, 0x00000000,
	};
	int max;
	max = sizeof(register_values)/sizeof(register_values[0]);
	setup_resource_map(register_values, max);
}

static void sdram_set_registers(const struct mem_controller *ctrl)
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
	 *	   00000 = 100Mhz 4K rows
	 *	   00001 = 133Mhz 4K rows
	 *	   00010 = 166Mhz 4K rows
	 *	   00011 = 200Mhz 4K rows
	 *	   01000 = 100Mhz 8K/16K rows
	 *	   01001 = 133Mhz 8K/16K rows
	 *	   01010 = 166Mhz 8K/16K rows
	 *	   01011 = 200Mhz 8K/16K rows
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
	 *	   000 = 100Mhz
	 *	   001 = reserved
	 *	   010 = 133Mhz
	 *	   011 = reserved
	 *	   100 = reserved
	 *	   101 = 166Mhz
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
//BY LYH  add IOMMU 64M APERTURE
	PCI_ADDR(0, 0x18, 3, 0x94), 0xffff8000, 0x00000f70,
	PCI_ADDR(0, 0x18, 3, 0x90), 0xffffff80, 0x00000002,
	PCI_ADDR(0, 0x18, 3, 0x98), 0x0000000f, 0x00068300,

//BY LYH END
	};
	int i;
	int max;
	print_debug("setting up CPU");
	print_debug_hex8(ctrl->node_id);
	print_debug(" northbridge registers\r\n");
	max = sizeof(register_values)/sizeof(register_values[0]);
	for(i = 0; i < max; i += 3) {
		device_t dev;
		unsigned where;
		unsigned long reg;
#if 0
		print_debug_hex32(register_values[i]);
		print_debug(" <-");
		print_debug_hex32(register_values[i+2]);
		print_debug("\r\n");
#endif
		dev = (register_values[i] & ~0xff) - PCI_DEV(0, 0x18, 0) + ctrl->f0;
		where = register_values[i] & 0xff;
		reg = pci_read_config32(dev, where);
		reg &= register_values[i+1];
		reg |= register_values[i+2];
		pci_write_config32(dev, where, reg);
#if 0

		reg = pci_read_config32(register_values[i]);
		reg &= register_values[i+1];
		reg |= register_values[i+2];
		pci_write_config32(register_values[i], reg);
#endif
	}
	print_debug("done.\r\n");
}


static int is_dual_channel(const struct mem_controller *ctrl)
{
	uint32_t dcl;
	dcl = pci_read_config32(ctrl->f2, DRAM_CONFIG_LOW);
	return dcl & DCL_128BitEn;
}

static int is_opteron(const struct mem_controller *ctrl)
{
	/* Test to see if I am an Opteron.  
	 * FIXME Testing dual channel capability is correct for now
	 * but a beter test is probably required.
	 */
#warning "FIXME implement a better test for opterons"
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
	return !(dcl & DCL_UnBufDimm);
}

struct dimm_size {
	unsigned long side1;
	unsigned long side2;
};

static struct dimm_size spd_get_dimm_size(unsigned device)
{
	/* Calculate the log base 2 size of a DIMM in bits */
	struct dimm_size sz;
	int value, low;
	sz.side1 = 0;
	sz.side2 = 0;

	/* Note it might be easier to use byte 31 here, it has the DIMM size as
	 * a multiple of 4MB.  The way we do it now we can size both
	 * sides of an assymetric dimm.
	 */
	value = spd_read_byte(device, 3);	/* rows */
	if (value < 0) goto out;
	sz.side1 += value & 0xf;

	value = spd_read_byte(device, 4);	/* columns */
	if (value < 0) goto out;
	sz.side1 += value & 0xf;

	value = spd_read_byte(device, 17);	/* banks */
	if (value < 0) goto out;
	sz.side1 += log2(value & 0xff);

	/* Get the module data width and convert it to a power of two */
	value = spd_read_byte(device, 7);	/* (high byte) */
	if (value < 0) goto out;
	value &= 0xff;
	value <<= 8;
	
	low = spd_read_byte(device, 6);	/* (low byte) */
	if (low < 0) goto out;
	value = value | (low & 0xff);
	sz.side1 += log2(value);

	/* side 2 */
	value = spd_read_byte(device, 5);	/* number of physical banks */
	if (value <= 1) goto out;

	/* Start with the symmetrical case */
	sz.side2 = sz.side1;

	value = spd_read_byte(device, 3);	/* rows */
	if (value < 0) goto out;
	if ((value & 0xf0) == 0) goto out;	/* If symmetrical we are done */
	sz.side2 -= (value & 0x0f);		/* Subtract out rows on side 1 */
	sz.side2 += ((value >> 4) & 0x0f);	/* Add in rows on side 2 */

	value = spd_read_byte(device, 4);	/* columns */
	if (value < 0) goto out;
	sz.side2 -= (value & 0x0f);		/* Subtract out columns on side 1 */
	sz.side2 += ((value >> 4) & 0x0f);	/* Add in columsn on side 2 */

 out:
	return sz;
}

static void set_dimm_size(const struct mem_controller *ctrl, struct dimm_size sz, unsigned index)
{
	uint32_t base0, base1, map;
	uint32_t dch;

#if 0
	print_debug("set_dimm_size: (");
	print_debug_hex32(sz.side1);
	print_debug_char(',');
	print_debug_hex32(sz.side2);
	print_debug_char(',');
	print_debug_hex32(index);
	print_debug(")\r\n");
#endif
	if (sz.side1 != sz.side2) {
		sz.side2 = 0;
	}
	map = pci_read_config32(ctrl->f2, DRAM_BANK_ADDR_MAP);
	map &= ~(0xf << (index + 4));

	/* For each base register.
	 * Place the dimm size in 32 MB quantities in the bits 31 - 21.
	 * The initialize dimm size is in bits.
	 * Set the base enable bit0.
	 */
	
	base0 = base1 = 0;

	/* Make certain side1 of the dimm is at least 32MB */
	if (sz.side1 >= (25 +3)) {
		map |= (sz.side1 - (25 + 3)) << (index *4);
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
	pci_write_config32(ctrl->f2, DRAM_BANK_ADDR_MAP, map);
	
	/* Enable the memory clocks for this DIMM */
	if (base0) {
		dch = pci_read_config32(ctrl->f2, DRAM_CONFIG_HIGH);
		dch |= DCH_MEMCLK_EN0 << index;
		pci_write_config32(ctrl->f2, DRAM_CONFIG_HIGH, dch);
	}
}

static void spd_set_ram_size(const struct mem_controller *ctrl)
{
	int i;
	
	for(i = 0; (i < 4) && (ctrl->channel0[i]); i++) {
		struct dimm_size sz;
		sz = spd_get_dimm_size(ctrl->channel0[i]);
		set_dimm_size(ctrl, sz, i);
	}
}

//BY LYH //Fill next base reg with right value
static void fill_last(unsigned long node_id,unsigned long base)
{
        unsigned i;
        unsigned base_reg;
        base &=0xffff0000;
        device_t device;
        for(device = PCI_DEV(0, 0x18, 1); device <= PCI_DEV(0, 0x1f, 1); device
+= PCI_DEV(0, 1, 0)) {
                for(i=node_id+1;i<=7;i++) {
                        base_reg=0x40+(i<<3);
                        pci_write_config32(device,base_reg,base);
                }
        }
}
//BY LYH END
 
static void route_dram_accesses(const struct mem_controller *ctrl,
	unsigned long base_k, unsigned long limit_k)
{
	/* Route the addresses to the controller node */
	unsigned node_id;
	unsigned limit;
	unsigned base;
	unsigned index;
	unsigned limit_reg, base_reg;
	device_t device;

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
	for(device = PCI_DEV(0, 0x18, 1); device <= PCI_DEV(0, 0x1f, 1); device += PCI_DEV(0, 1, 0)) {
		pci_write_config32(device, limit_reg, limit);
		pci_write_config32(device, base_reg, base);
	}
}

static void set_top_mem(unsigned tom_k)
{
	/* Error if I don't have memory */
	if (!tom_k) {
		die("No memory");
	}

#if 1
	/* Report the amount of memory. */
	print_debug("RAM: 0x");
	print_debug_hex32(tom_k);
	print_debug(" KB\r\n");
#endif

	/* Now set top of memory */
	msr_t msr;
	msr.lo = (tom_k & 0x003fffff) << 10;
	msr.hi = (tom_k & 0xffc00000) >> 22;
	wrmsr(TOP_MEM2, msr);

	/* Leave a 64M hole between TOP_MEM and TOP_MEM2
	 * so I can see my rom chip and other I/O devices.
	 */
	if (tom_k >= 0x003f0000) {
		tom_k = 0x3f0000;
	}
	msr.lo = (tom_k & 0x003fffff) << 10;
	msr.hi = (tom_k & 0xffc00000) >> 22;
	wrmsr(TOP_MEM, msr);
}

static void order_dimms(const struct mem_controller *ctrl)
{
	unsigned long tom, tom_k, base_k;
	unsigned node_id;

	/* Compute the memory base address address */
	base_k = 0;
	/* Remember which registers we have used in the high 8 bits of tom */
	tom = base_k >> 15;
	for(;;) {
		/* Find the largest remaining canidate */
		unsigned index, canidate;
		uint32_t csbase, csmask;
		unsigned size;
		csbase = 0;
		canidate = 0;
		for(index = 0; index < 8; index++) {
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
			/* I have a new canidate */
			csbase = value;
			canidate = index;
		}
		/* See if I have found a new canidate */
		if (csbase == 0) {
			break;
		}

		/* Remember the dimm size */
		size = csbase >> 21;

		/* Remember I have used this register */
		tom |= (1 << (canidate + 24));

		/* Recompute the cs base register value */
		csbase = (tom << 21) | 1;

		/* Increment the top of memory */
		tom += size;

		/* Compute the memory mask */
		csmask = ((size -1) << 21);
		csmask |= 0xfe00;		/* For now don't optimize */
#warning "Don't forget to optimize the DIMM size"

		/* Write the new base register */
		pci_write_config32(ctrl->f2, DRAM_CSBASE + (canidate << 2), csbase);
		/* Write the new mask register */
		pci_write_config32(ctrl->f2, DRAM_CSMASK + (canidate << 2), csmask);
		
	}
	tom_k = (tom & ~0xff000000) << 15;

	/* Compute the memory base address */
	base_k = 0;
	for(node_id = 0; node_id < ctrl->node_id; node_id++) {
		uint32_t limit, base;
		unsigned index;
		index = node_id << 3;
		base = pci_read_config32(ctrl->f1, 0x40 + index);
		/* Only look at the limit if the base is enabled */
		if ((base & 3) == 3) {
			limit = pci_read_config32(ctrl->f1, 0x44 + index);
			base_k = ((limit + 0x00010000) & 0xffff0000) >> 2;
		}
	}
	tom_k += base_k;
#if 0
	print_debug("tom: ");
	print_debug_hex32(tom);
	print_debug(" base_k: ");
	print_debug_hex32(base_k);
	print_debug(" tom_k: ");
	print_debug_hex32(tom_k);
	print_debug("\r\n");
#endif
	route_dram_accesses(ctrl, base_k, tom_k);
//BY LYH
        fill_last(ctrl->node_id, tom_k<<2);
//BY LYH END
	set_top_mem(tom_k);
}

static void disable_dimm(const struct mem_controller *ctrl, unsigned index)
{
	print_debug("disabling dimm"); 
	print_debug_hex8(index); 
	print_debug("\r\n");
	pci_write_config32(ctrl->f2, DRAM_CSBASE + (((index << 1)+0)<<2), 0);
	pci_write_config32(ctrl->f2, DRAM_CSBASE + (((index << 1)+1)<<2), 0);
}


static void spd_handle_unbuffered_dimms(const struct mem_controller *ctrl)
{
	int i;
	int registered;
	int unbuffered;
	uint32_t dcl;
	unbuffered = 0;
	registered = 0;
	for(i = 0; (i < 4) && (ctrl->channel0[i]); i++) {
		int value;
		value = spd_read_byte(ctrl->channel0[i], 21);
		if (value < 0) {
			disable_dimm(ctrl, i);
			continue;
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
	if (unbuffered && is_opteron(ctrl)) {
		die("Unbuffered Dimms not supported on Opteron");
	}

	dcl = pci_read_config32(ctrl->f2, DRAM_CONFIG_LOW);
	dcl &= ~DCL_UnBufDimm;
	if (unbuffered) {
		dcl |= DCL_UnBufDimm;
	}
	pci_write_config32(ctrl->f2, DRAM_CONFIG_LOW, dcl);
#if 0
	if (is_registered(ctrl)) {
		print_debug("Registered\r\n");
	} else {
		print_debug("Unbuffered\r\n");
	}
#endif
}

static void spd_enable_2channels(const struct mem_controller *ctrl)
{
	int i;
	uint32_t nbcap;
	/* SPD addresses to verify are identical */
#warning "FINISHME review and see if these are the bytes I need"
	/* FINISHME review and see if these are the bytes I need */
	static const unsigned addresses[] = {
		2,	/* Type should be DDR SDRAM */
		3,	/* *Row addresses */
		4,	/* *Column addresses */
		5,	/* *Physical Banks */
		6,	/* *Module Data Width low */
		7,	/* *Module Data Width high */
		9,	/* *Cycle time at highest CAS Latency CL=X */
		11,	/* *SDRAM Type */
		13,	/* *SDRAM Width */
		17,	/* *Logical Banks */
		18,	/* *Supported CAS Latencies */
		21,	/* *SDRAM Module Attributes */
		23,	/* *Cycle time at CAS Latnecy (CLX - 0.5) */
		26,	/* *Cycle time at CAS Latnecy (CLX - 1.0) */
		27,	/* *tRP Row precharge time */
		28,	/* *Minimum Row Active to Row Active Delay (tRRD) */
		29,	/* *tRCD RAS to CAS */
		30,	/* *tRAS Activate to Precharge */
		41,	/* *Minimum Active to Active/Auto Refresh Time(Trc) */
		42,	/* *Minimum Auto Refresh Command Time(Trfc) */
	};
	nbcap = pci_read_config32(ctrl->f3, NORTHBRIDGE_CAP);
	if (!(nbcap & NBCAP_128Bit)) {
		return;
	}
	for(i = 0; (i < 4) && (ctrl->channel0[i]); i++) {
		unsigned device0, device1;
		int value0, value1;
		int j;
		device0 = ctrl->channel0[i];
		device1 = ctrl->channel1[i];
		if (!device1)
			return;
		for(j = 0; j < sizeof(addresses)/sizeof(addresses[0]); j++) {
			unsigned addr;
			addr = addresses[j];
			value0 = spd_read_byte(device0, addr);
			if (value0 < 0) {
				break;
			}
			value1 = spd_read_byte(device1, addr);
			if (value1 < 0) {
				return;
			}
			if (value0 != value1) {
				return;
			}
		}
	}
	print_debug("Enabling dual channel memory\r\n");
	uint32_t dcl;
	dcl = pci_read_config32(ctrl->f2, DRAM_CONFIG_LOW);
	dcl &= ~DCL_32ByteEn;
	dcl |= DCL_128BitEn;
	pci_write_config32(ctrl->f2, DRAM_CONFIG_LOW, dcl);
}

struct mem_param {
	uint8_t cycle_time;
	uint8_t divisor; /* In 1/2 ns increments */
	uint8_t tRC;
	uint8_t tRFC;
	uint32_t dch_memclk;
	uint16_t dch_tref4k, dch_tref8k;
	uint8_t	 dtl_twr;
	char name[9];
};

static const struct mem_param *get_mem_param(unsigned min_cycle_time)
{
	static const struct mem_param speed[] = {
		{
			.name	    = "100Mhz\r\n",
			.cycle_time = 0xa0,
			.divisor    = (10 <<1),
			.tRC	    = 0x46,
			.tRFC	    = 0x50,
			.dch_memclk = DCH_MEMCLK_100MHZ << DCH_MEMCLK_SHIFT,
			.dch_tref4k = DTH_TREF_100MHZ_4K,
			.dch_tref8k = DTH_TREF_100MHZ_8K,
			.dtl_twr    = 2,
		},
		{
			.name	    = "133Mhz\r\n",
			.cycle_time = 0x75,
			.divisor    = (7<<1)+1,
			.tRC	    = 0x41,
			.tRFC	    = 0x4B,
			.dch_memclk = DCH_MEMCLK_133MHZ << DCH_MEMCLK_SHIFT,
			.dch_tref4k = DTH_TREF_133MHZ_4K,
			.dch_tref8k = DTH_TREF_133MHZ_8K,
			.dtl_twr    = 2,
		},
		{
			.name	    = "166Mhz\r\n",
			.cycle_time = 0x60,
			.divisor    = (6<<1),
			.tRC	    = 0x3C,
			.tRFC	    = 0x48,
			.dch_memclk = DCH_MEMCLK_166MHZ << DCH_MEMCLK_SHIFT,
			.dch_tref4k = DTH_TREF_166MHZ_4K,
			.dch_tref8k = DTH_TREF_166MHZ_8K,
			.dtl_twr    = 3,
		},
		{
			.name	    = "200Mhz\r\n",
			.cycle_time = 0x50,
			.divisor    = (5<<1),
			.tRC	    = 0x37,
			.tRFC	    = 0x46,
			.dch_memclk = DCH_MEMCLK_200MHZ << DCH_MEMCLK_SHIFT,
			.dch_tref4k = DTH_TREF_200MHZ_4K,
			.dch_tref8k = DTH_TREF_200MHZ_8K,
			.dtl_twr    = 3,
		},
		{
			.cycle_time = 0x00,
		},
	};
	const struct mem_param *param;
	for(param = &speed[0]; param->cycle_time ; param++) {
		if (min_cycle_time > (param+1)->cycle_time) {
			break;
		}
	}
	if (!param->cycle_time) {
		die("min_cycle_time to low");
	}
#if 1
	print_debug(param->name);
#endif
	return param;
}

static const struct mem_param *spd_set_memclk(const struct mem_controller *ctrl)
{
	/* Compute the minimum cycle time for these dimms */
	const struct mem_param *param;
	unsigned min_cycle_time, min_latency;
	int i;
	uint32_t value;

	static const int latency_indicies[] = { 26, 23, 9 };
	static const unsigned char min_cycle_times[] = {
		[NBCAP_MEMCLK_200MHZ] = 0x50, /* 5ns */
		[NBCAP_MEMCLK_166MHZ] = 0x60, /* 6ns */
		[NBCAP_MEMCLK_133MHZ] = 0x75, /* 7.5ns */
		[NBCAP_MEMCLK_100MHZ] = 0xa0, /* 10ns */
	};


	value = pci_read_config32(ctrl->f3, NORTHBRIDGE_CAP);
	min_cycle_time = min_cycle_times[(value >> NBCAP_MEMCLK_SHIFT) & NBCAP_MEMCLK_MASK];
	min_latency = 2;

#if 0
	print_debug("min_cycle_time: "); 
	print_debug_hex8(min_cycle_time); 
	print_debug(" min_latency: ");
	print_debug_hex8(min_latency);
	print_debug("\r\n");
#endif

	/* Compute the least latency with the fastest clock supported
	 * by both the memory controller and the dimms.
	 */
	for(i = 0; (i < 4) && (ctrl->channel0[i]); i++) {
		int new_cycle_time, new_latency;
		int index;
		int latencies;
		int latency;

		/* First find the supported CAS latencies
		 * Byte 18 for DDR SDRAM is interpreted:
		 * bit 0 == CAS Latency = 1.0
		 * bit 1 == CAS Latency = 1.5
		 * bit 2 == CAS Latency = 2.0
		 * bit 3 == CAS Latency = 2.5
		 * bit 4 == CAS Latency = 3.0
		 * bit 5 == CAS Latency = 3.5
		 * bit 6 == TBD
		 * bit 7 == TBD
		 */
		new_cycle_time = 0xa0;
		new_latency = 5;

		latencies = spd_read_byte(ctrl->channel0[i], 18);
		if (latencies <= 0) continue;

		/* Compute the lowest cas latency supported */
		latency = log2(latencies) -2;

		/* Loop through and find a fast clock with a low latency */
		for(index = 0; index < 3; index++, latency++) {
			int value;
			if ((latency < 2) || (latency > 4) ||
				(!(latencies & (1 << latency)))) {
				continue;
			}
			value = spd_read_byte(ctrl->channel0[i], latency_indicies[index]);
			if (value < 0) {
				continue;
			}

			/* Only increase the latency if we decreas the clock */
			if ((value >= min_cycle_time) && (value < new_cycle_time)) {
				new_cycle_time = value;
				new_latency = latency;
			}
		}
		if (new_latency > 4){
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
#if 0
		print_debug("i: ");
		print_debug_hex8(i);
		print_debug(" min_cycle_time: "); 
		print_debug_hex8(min_cycle_time); 
		print_debug(" min_latency: ");
		print_debug_hex8(min_latency);
		print_debug("\r\n");
#endif
	}
	/* Make a second pass through the dimms and disable
	 * any that cannot support the selected memclk and cas latency.
	 */
	
	for(i = 0; (i < 4) && (ctrl->channel0[i]); i++) {
		int latencies;
		int latency;
		int index;
		int value;
		int dimm;
		latencies = spd_read_byte(ctrl->channel0[i], 18);
		if (latencies <= 0) {
			goto dimm_err;
		}

		/* Compute the lowest cas latency supported */
		latency = log2(latencies) -2;

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
		
		/* All is good if the selected clock speed 
		 * is what I need or slower.
		 */
		if (value <= min_cycle_time) {
			continue;
		}
		/* Otherwise I have an error, disable the dimm */
	dimm_err:
		disable_dimm(ctrl, i);
	}
#if 0
	print_debug("min_cycle_time: "); 
	print_debug_hex8(min_cycle_time); 
	print_debug(" min_latency: ");
	print_debug_hex8(min_latency);
	print_debug("\r\n");
#endif
	/* Now that I know the minimum cycle time lookup the memory parameters */
	param = get_mem_param(min_cycle_time);

	/* Update DRAM Config High with our selected memory speed */
	value = pci_read_config32(ctrl->f2, DRAM_CONFIG_HIGH);
	value &= ~(DCH_MEMCLK_MASK << DCH_MEMCLK_SHIFT);
	value |= param->dch_memclk;
	pci_write_config32(ctrl->f2, DRAM_CONFIG_HIGH, value);

	static const unsigned latencies[] = { DTL_CL_2, DTL_CL_2_5, DTL_CL_3 };
	/* Update DRAM Timing Low with our selected cas latency */
	value = pci_read_config32(ctrl->f2, DRAM_TIMING_LOW);
	value &= ~(DTL_TCL_MASK << DTL_TCL_SHIFT);
	value |= latencies[min_latency - 2] << DTL_TCL_SHIFT;
	pci_write_config32(ctrl->f2, DRAM_TIMING_LOW, value);
	
	return param;
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
	clocks = ((value << 1) + param->divisor - 1)/param->divisor;
	if (clocks < DTL_TRC_MIN) {
		clocks = DTL_TRC_MIN;
	}
	if (clocks > DTL_TRC_MAX) {
		return -1;
	}

	dtl = pci_read_config32(ctrl->f2, DRAM_TIMING_LOW);
	old_clocks = ((dtl >> DTL_TRC_SHIFT) & DTL_TRC_MASK) + DTL_TRC_BASE;
	if (old_clocks > clocks) {
		clocks = old_clocks;
	}
	dtl &= ~(DTL_TRC_MASK << DTL_TRC_SHIFT);
	dtl |=	((clocks - DTL_TRC_BASE) << DTL_TRC_SHIFT);
	pci_write_config32(ctrl->f2, DRAM_TIMING_LOW, dtl);
	return 0;
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
	clocks = ((value << 1) + param->divisor - 1)/param->divisor;
	if (clocks < DTL_TRFC_MIN) {
		clocks = DTL_TRFC_MIN;
	}
	if (clocks > DTL_TRFC_MAX) {
		return -1;
	}
	dtl = pci_read_config32(ctrl->f2, DRAM_TIMING_LOW);
	old_clocks = ((dtl >> DTL_TRFC_SHIFT) & DTL_TRFC_MASK) + DTL_TRFC_BASE;
	if (old_clocks > clocks) {
		clocks = old_clocks;
	}
	dtl &= ~(DTL_TRFC_MASK << DTL_TRFC_SHIFT);
	dtl |= ((clocks - DTL_TRFC_BASE) << DTL_TRFC_SHIFT);
	pci_write_config32(ctrl->f2, DRAM_TIMING_LOW, dtl);
	return 0;
}


static int update_dimm_Trcd(const struct mem_controller *ctrl, const struct mem_param *param, int i)
{
	unsigned clocks, old_clocks;
	uint32_t dtl;
	int value;
	value = spd_read_byte(ctrl->channel0[i], 29);
	if (value < 0) return -1;
#if 0
	clocks = (value + (param->divisor << 1) -1)/(param->divisor << 1);
#else
	clocks = (value + ((param->divisor & 0xff) << 1) -1)/((param->divisor & 0xff) << 1);
#endif
	if (clocks < DTL_TRCD_MIN) {
		clocks = DTL_TRCD_MIN;
	}
	if (clocks > DTL_TRCD_MAX) {
		return -1;
	}
	dtl = pci_read_config32(ctrl->f2, DRAM_TIMING_LOW);
	old_clocks = ((dtl >> DTL_TRCD_SHIFT) & DTL_TRCD_MASK) + DTL_TRCD_BASE;
	if (old_clocks > clocks) {
		clocks = old_clocks;
	}
	dtl &= ~(DTL_TRCD_MASK << DTL_TRCD_SHIFT);
	dtl |= ((clocks - DTL_TRCD_BASE) << DTL_TRCD_SHIFT);
	pci_write_config32(ctrl->f2, DRAM_TIMING_LOW, dtl);
	return 0;
}

static int update_dimm_Trrd(const struct mem_controller *ctrl, const struct mem_param *param, int i)
{
	unsigned clocks, old_clocks;
	uint32_t dtl;
	int value;
	value = spd_read_byte(ctrl->channel0[i], 28);
	if (value < 0) return -1;
	clocks = (value + ((param->divisor & 0xff) << 1) -1)/((param->divisor & 0xff) << 1);
	if (clocks < DTL_TRRD_MIN) {
		clocks = DTL_TRRD_MIN;
	}
	if (clocks > DTL_TRRD_MAX) {
		return -1;
	}
	dtl = pci_read_config32(ctrl->f2, DRAM_TIMING_LOW);
	old_clocks = ((dtl >> DTL_TRRD_SHIFT) & DTL_TRRD_MASK) + DTL_TRRD_BASE;
	if (old_clocks > clocks) {
		clocks = old_clocks;
	}
	dtl &= ~(DTL_TRRD_MASK << DTL_TRRD_SHIFT);
	dtl |= ((clocks - DTL_TRRD_BASE) << DTL_TRRD_SHIFT);
	pci_write_config32(ctrl->f2, DRAM_TIMING_LOW, dtl);
	return 0;
}

static int update_dimm_Tras(const struct mem_controller *ctrl, const struct mem_param *param, int i)
{
	unsigned clocks, old_clocks;
	uint32_t dtl;
	int value;
	value = spd_read_byte(ctrl->channel0[i], 30);
	if (value < 0) return -1;
	clocks = ((value << 1) + param->divisor - 1)/param->divisor;
	if (clocks < DTL_TRAS_MIN) {
		clocks = DTL_TRAS_MIN;
	}
	if (clocks > DTL_TRAS_MAX) {
		return -1;
	}
	dtl = pci_read_config32(ctrl->f2, DRAM_TIMING_LOW);
	old_clocks = ((dtl >> DTL_TRAS_SHIFT) & DTL_TRAS_MASK) + DTL_TRAS_BASE;
	if (old_clocks > clocks) {
		clocks = old_clocks;
	}
	dtl &= ~(DTL_TRAS_MASK << DTL_TRAS_SHIFT);
	dtl |= ((clocks - DTL_TRAS_BASE) << DTL_TRAS_SHIFT);
	pci_write_config32(ctrl->f2, DRAM_TIMING_LOW, dtl);
	return 0;
}

static int update_dimm_Trp(const struct mem_controller *ctrl, const struct mem_param *param, int i)
{
	unsigned clocks, old_clocks;
	uint32_t dtl;
	int value;
	value = spd_read_byte(ctrl->channel0[i], 27);
	if (value < 0) return -1;
#if 0
	clocks = (value + (param->divisor << 1) - 1)/(param->divisor << 1);
#else
	clocks = (value + ((param->divisor & 0xff) << 1) - 1)/((param->divisor & 0xff) << 1);
#endif
#if 0
	print_debug("Trp: ");
	print_debug_hex8(clocks);
	print_debug(" spd value: ");
	print_debug_hex8(value);
	print_debug(" divisor: ");
	print_debug_hex8(param->divisor);
	print_debug("\r\n");
#endif
	if (clocks < DTL_TRP_MIN) {
		clocks = DTL_TRP_MIN;
	}
	if (clocks > DTL_TRP_MAX) {
		return -1;
	}
	dtl = pci_read_config32(ctrl->f2, DRAM_TIMING_LOW);
	old_clocks = ((dtl >> DTL_TRP_SHIFT) & DTL_TRP_MASK) + DTL_TRP_BASE;
	if (old_clocks > clocks) {
		clocks = old_clocks;
	}
	dtl &= ~(DTL_TRP_MASK << DTL_TRP_SHIFT);
	dtl |= ((clocks - DTL_TRP_BASE) << DTL_TRP_SHIFT);
	pci_write_config32(ctrl->f2, DRAM_TIMING_LOW, dtl);
	return 0;
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
	return 0;
}


static int update_dimm_x4(const struct mem_controller *ctrl, const struct mem_param *param, int i)
{
	uint32_t dcl;
	int value;
	int dimm;
	value = spd_read_byte(ctrl->channel0[i], 13);
	if (value < 0) {
		return -1;
	}
	dimm = i;
	dimm += DCL_x4DIMM_SHIFT;
	dcl = pci_read_config32(ctrl->f2, DRAM_CONFIG_LOW);
	dcl &= ~(1 << dimm);
	if (value == 4) {
		dcl |= (1 << dimm);
	}
	pci_write_config32(ctrl->f2, DRAM_CONFIG_LOW, dcl);
	return 0;
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
	return 0;
}

static int count_dimms(const struct mem_controller *ctrl)
{
	int dimms;
	unsigned index;
	dimms = 0;
	for(index = 0; index < 8; index += 2) {
		uint32_t csbase;
		csbase = pci_read_config32(ctrl->f2, (DRAM_CSBASE + index << 2));
		if (csbase & 1) {
			dimms += 1;
		}
	}
	return dimms;
}

static void set_Twtr(const struct mem_controller *ctrl, const struct mem_param *param)
{
	uint32_t dth;
	unsigned clocks;
	clocks = 1; /* AMD says hard code this */
	dth = pci_read_config32(ctrl->f2, DRAM_TIMING_HIGH);
	dth &= ~(DTH_TWTR_MASK << DTH_TWTR_SHIFT);
	dth |= ((clocks - DTH_TWTR_BASE) << DTH_TWTR_SHIFT);
	pci_write_config32(ctrl->f2, DRAM_TIMING_HIGH, dth);
}

static void set_Trwt(const struct mem_controller *ctrl, const struct mem_param *param)
{
	uint32_t dth, dtl;
	unsigned divisor;
	unsigned latency;
	unsigned clocks;

	clocks = 0;
	dtl = pci_read_config32(ctrl->f2, DRAM_TIMING_LOW);
	latency = (dtl >> DTL_TCL_SHIFT) & DTL_TCL_MASK;
	divisor = param->divisor;

	if (is_opteron(ctrl)) {
		if (latency == DTL_CL_2) {
			if (divisor == ((6 << 0) + 0)) {
				/* 166Mhz */
				clocks = 3;
			}
			else if (divisor > ((6 << 0)+0)) {
				/* 100Mhz && 133Mhz */
				clocks = 2;
			}
		}
		else if (latency == DTL_CL_2_5) {
			clocks = 3;
		}
		else if (latency == DTL_CL_3) {
			if (divisor == ((6 << 0)+0)) {
				/* 166Mhz */
				clocks = 4;
			}
			else if (divisor > ((6 << 0)+0)) {
				/* 100Mhz && 133Mhz */
				clocks = 3;
			}
		}
	}
	else /* Athlon64 */ {
		if (is_registered(ctrl)) {
			if (latency == DTL_CL_2) {
				clocks = 2;
			}
			else if (latency == DTL_CL_2_5) {
				clocks = 3;
			}
			else if (latency == DTL_CL_3) {
				clocks = 3;
			}
		}
		else /* Unbuffered */{
			if (latency == DTL_CL_2) {
				clocks = 3;
			}
			else if (latency == DTL_CL_2_5) {
				clocks = 4;
			}
			else if (latency == DTL_CL_3) {
				clocks = 4;
			}
		}
	}
	if ((clocks < DTH_TRWT_MIN) || (clocks > DTH_TRWT_MAX)) {
		die("Unknown Trwt");
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
	unsigned divisor;
	unsigned rdpreamble;
	divisor = param->divisor;
	dch = pci_read_config32(ctrl->f2, DRAM_CONFIG_HIGH);
	dch &= ~(DCH_RDPREAMBLE_MASK << DCH_RDPREAMBLE_SHIFT);
	rdpreamble = 0;
	if (is_registered(ctrl)) {
		if (divisor == ((10 << 1)+0)) {
			/* 100Mhz, 9ns */
			rdpreamble = ((9 << 1)+ 0);
		}
		else if (divisor == ((7 << 1)+1)) {
			/* 133Mhz, 8ns */
			rdpreamble = ((8 << 1)+0);
		}
		else if (divisor == ((6 << 1)+0)) {
			/* 166Mhz, 7.5ns */
			rdpreamble = ((7 << 1)+1);
		}
	}
	else {
		int slots;
		int i;
		slots = 0;
		for(i = 0; i < 4; i++) {
			if (ctrl->channel0[i]) {
				slots += 1;
			}
		}
		if (divisor == ((10 << 1)+0)) {
			/* 100Mhz */
			if (slots <= 2) {
				/* 9ns */
				rdpreamble = ((9 << 1)+0);
			} else {
				/* 14ns */
				rdpreamble = ((14 << 1)+0);
			}
		}
		else if (divisor == ((7 << 1)+1)) {
			/* 133Mhz */
			if (slots <= 2) {
				/* 7ns */
				rdpreamble = ((7 << 1)+0);
			} else {
				/* 11 ns */
				rdpreamble = ((11 << 1)+0);
			}
		}
		else if (divisor == ((6 << 1)+0)) {
			/* 166Mhz */
			if (slots <= 2) {
				/* 6ns */
				rdpreamble = ((7 << 1)+0);
			} else {
				/* 9ns */
				rdpreamble = ((9 << 1)+0);
			}
		}
		else if (divisor == ((5 << 1)+0)) {
			/* 200Mhz */
			if (slots <= 2) {
				/* 5ns */
				rdpreamble = ((5 << 1)+0);
			} else {
				/* 7ns */
				rdpreamble = ((7 << 1)+0);
			}
		}
	}
	if ((rdpreamble < DCH_RDPREAMBLE_MIN) || (rdpreamble > DCH_RDPREAMBLE_MAX)) {
		die("Unknown rdpreamble");
	}
	dch |= (rdpreamble - DCH_RDPREAMBLE_BASE) << DCH_RDPREAMBLE_SHIFT;
	pci_write_config32(ctrl->f2, DRAM_CONFIG_HIGH, dch);
}

static void set_max_async_latency(const struct mem_controller *ctrl, const struct mem_param *param)
{
	uint32_t dch;
	int i;
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

static void spd_set_dram_timing(const struct mem_controller *ctrl, const struct mem_param *param)
{
	int dimms;
	int i;
	init_Tref(ctrl, param);
	for(i = 0; (i < 4) && ctrl->channel0[i]; i++) {
		int rc;
		/* DRAM Timing Low Register */
		if (update_dimm_Trc (ctrl, param, i) < 0) goto dimm_err;
		if (update_dimm_Trfc(ctrl, param, i) < 0) goto dimm_err;
		if (update_dimm_Trcd(ctrl, param, i) < 0) goto dimm_err;
		if (update_dimm_Trrd(ctrl, param, i) < 0) goto dimm_err;
		if (update_dimm_Tras(ctrl, param, i) < 0) goto dimm_err;
		if (update_dimm_Trp (ctrl, param, i) < 0) goto dimm_err;

		/* DRAM Timing High Register */
		if (update_dimm_Tref(ctrl, param, i) < 0) goto dimm_err;

		/* DRAM Config Low */
		if (update_dimm_x4 (ctrl, param, i) < 0) goto dimm_err;
		if (update_dimm_ecc(ctrl, param, i) < 0) goto dimm_err;
		continue;
	dimm_err:
		disable_dimm(ctrl, i);
		
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
}

static void sdram_set_spd_registers(const struct mem_controller *ctrl) 
{
	const struct mem_param *param;
	spd_enable_2channels(ctrl);
	spd_set_ram_size(ctrl);
	spd_handle_unbuffered_dimms(ctrl);
	param = spd_set_memclk(ctrl);
	spd_set_dram_timing(ctrl, param);
	order_dimms(ctrl);
}

#define TIMEOUT_LOOPS 300000
static void sdram_enable(int controllers, const struct mem_controller *ctrl)
{
	int i;

	/* Before enabling memory start the memory clocks */
	for(i = 0; i < controllers; i++) {
		uint32_t dch;
		dch = pci_read_config32(ctrl[i].f2, DRAM_CONFIG_HIGH);
		dch |= DCH_MEMCLK_VALID;
		pci_write_config32(ctrl[i].f2, DRAM_CONFIG_HIGH, dch);
	}

	/* And if necessary toggle the the reset on the dimms by hand */
	memreset(controllers, ctrl);

	for(i = 0; i < controllers; i++) {
		uint32_t dcl;
		/* Toggle DisDqsHys to get it working */
		dcl = pci_read_config32(ctrl[i].f2, DRAM_CONFIG_LOW);
#if 0
		print_debug("dcl: ");
		print_debug_hex32(dcl);
		print_debug("\r\n");
#endif
#warning "FIXME set the ECC type to perform"
#warning "FIXME initialize the scrub registers"
#if 1
		if (dcl & DCL_DimmEccEn) {
			print_debug("ECC enabled\r\n");
		}
#endif
		dcl |= DCL_DisDqsHys;
		pci_write_config32(ctrl[i].f2, DRAM_CONFIG_LOW, dcl);
		dcl &= ~DCL_DisDqsHys;
		dcl &= ~DCL_DLL_Disable;
		dcl &= ~DCL_D_DRV;
		dcl &= ~DCL_QFC_EN;
		dcl |= DCL_DramInit;
		pci_write_config32(ctrl[i].f2, DRAM_CONFIG_LOW, dcl);

	}
	for(i = 0; i < controllers; i++) {
		uint32_t dcl;
		print_debug("Initializing memory: ");
		int loops = 0;
		do {
			dcl = pci_read_config32(ctrl[i].f2, DRAM_CONFIG_LOW);
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
		if (dcl & DCL_DimmEccEn) {
			print_debug("Clearing memory: ");
			loops = 0;
			dcl &= ~DCL_MemClrStatus;
			pci_write_config32(ctrl[i].f2, DRAM_CONFIG_LOW, dcl);
			
			do {
				dcl = pci_read_config32(ctrl[i].f2, DRAM_CONFIG_LOW);
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
			pci_write_config32(ctrl[i].f3, SCRUB_ADDR_LOW, 0);
			pci_write_config32(ctrl[i].f3, SCRUB_ADDR_HIGH, 0);
		}
#endif
	}
}
