#define ASSEMBLY 1
#include "arch/romcc_io.h"
#include "pc80/serial.c"
#include "arch/i386/lib/console.c"
#include "ram/ramtest.c"


static void sdram_set_registers(void)
{
	static const unsigned int register_values[] = {
	/* Routing Table Node i 
	 * F0:0x40 i = 0, 
	 * F0:0x44 i = 1,
	 * F0:0x48 i = 2, 
	 * F0:0x4c i = 3,
	 * F0:0x50 i = 4, 
	 * F0:0x54 i = 5,
	 * F0:0x58 i = 6, 
	 * F0:0x5c i = 7
	 * [ 0: 3] Request Route
	 *     [0] Route to this node
	 *     [1] Route to Link 0
	 *     [2] Route to Link 1
	 *     [3] Route to Link 2
	 * [11: 8] Response Route
	 *     [0] Route to this node
	 *     [1] Route to Link 0
	 *     [2] Route to Link 1
	 *     [3] Route to Link 2
	 * [19:16] Broadcast route
	 *     [0] Route to this node
	 *     [1] Route to Link 0
	 *     [2] Route to Link 1
	 *     [3] Route to Link 2
	 */
	0xc040, 0xfff0f0f0, 0x00010101,
	0xc044, 0xfff0f0f0, 0x00010101,
	0xc048, 0xfff0f0f0, 0x00010101,
	0xc04c, 0xfff0f0f0, 0x00010101,
	0xc050, 0xfff0f0f0, 0x00010101,
	0xc054, 0xfff0f0f0, 0x00010101,
	0xc058, 0xfff0f0f0, 0x00010101,
	0xc05c, 0xfff0f0f0, 0x00010101,

	/* Hypetransport Transaction Control Register 
	 * F0:0x68
	 * [ 0: 0] Disable read byte probe
	 *         0 = Probes issues
	 *         1 = Probes not issued
	 * [ 1: 1] Disable Read Doubleword probe
	 *         0 = Probes issued
	 *         1 = Probes not issued
	 * [ 2: 2] Disable write byte probes
	 *         0 = Probes issued
	 *         1 = Probes not issued
	 * [ 3: 3] Disalbe Write Doubleword Probes
	 *         0 = Probes issued
	 *         1 = Probes not issued.
	 * [ 4: 4] Disable Memroy Controller Target Start
	 *         0 = TgtStart packets are generated
	 *         1 = TgtStart packets are not generated.
	 * [ 5: 5] CPU1 Enable
	 *         0 = Second CPU disabled or not present
	 *         1 = Second CPU enabled.
	 * [ 6: 6] CPU Request PassPW
	 *         0 = CPU requests do not pass posted writes
	 *         1 = CPU requests pass posted writes.
	 * [ 7: 7] CPU read Respons PassPW
	 *         0 = CPU Responses do not pass posted writes
	 *         1 = CPU responses pass posted writes.
	 * [ 8: 8] Disable Probe Memory Cancel
	 *         0 = Probes may generate MemCancels
	 *         1 = Probes may not generate MemCancels
	 * [ 9: 9] Disable Remote Probe Memory Cancel.
	 *         0 = Probes hitting dirty blocks generate memory cancel packets
	 *         1 = Only probed caches on the same node as the memory controller
	 *              generate cancel packets.
	 * [10:10] Disable Fill Probe
	 *         0 = Probes issued for cache fills
	 *         1 = Probes not issued for cache fills.
	 * [11:11] Response PassPw
	 *         0 = Downstream response PassPW based on original request
	 *         1 = Downstream response PassPW set to 1
	 * [12:12] Change ISOC to Ordered
	 *         0 = Bit 1 of coherent HT RdSz/WrSz command used for iosynchronous prioritization
	 *         1 = Bit 1 of coherent HT RdSz/WrSz command used for ordering.
	 * [14:13] Buffer Release Priority select 
	 *         00 = 64
	 *         01 = 16
	 *         10 = 8
	 *         11 = 2
	 * [15:15] Limit Coherent HT Configuration Space Range
	 *         0 = No coherent HT configuration space restrictions
	 *         1 = Limit coherent HT configuration space based on node count
	 * [16:16] Local Interrupt Conversion Enable.
	 *         0 = ExtInt/NMI interrups unaffected.
	 *         1 = ExtInt/NMI broadcat interrupts converted to LINT0/1
	 * [17:17] APIC Extended Broadcast Enable.
	 *         0 = APIC broadcast is 0F
	 *         1 = APIC broadcast is FF
	 * [18:18] APIC Extended ID Enable
	 *         0 = APIC ID is 4 bits.
	 *         1 = APIC ID is 8 bits.
	 * [19:19] APIC Extended Spurious Vector Enable
	 *         0 = Lower 4 bits of spurious vector are read-only 1111
	 *         1 = Lower 4 bits of spurious vecotr are writeable.
	 * [20:20] Sequence ID Source Node Enable
	 *         0 = Normal operation
	 *         1 = Keep SeqID on routed packets for debugging.
	 * [22:21] Downstream non-posted request limit
	 *         00 = No limit
	 *         01 = Limited to 1
	 *         10 = Limited to 4
	 *         11 = Limited to 8
	 * [23:23] RESERVED
	 * [25:24] Medium-Priority Bypass Count
	 *         - Maximum # of times a medium priority access can pass a low
	 *           priority access before Medium-Priority mode is disabled for one access.
	 * [27:26] High-Priority Bypass Count
	 *         - Maximum # of times a high prioirty access can pass a medium or low
	 *           priority access before High-prioirty mode is disabled for one access.
	 * [28:28] Enable High Priority CPU Reads
	 *         0 = Cpu reads are medium prioirty
	 *         1 = Cpu reads are high prioirty
	 * [29:29] Disable Low Priority Writes
	 *         0 = Non-isochronous writes are low priority
	 *         1 = Non-isochronous writes are medium prioirty
	 * [30:30] Disable High Priority Isochronous writes
	 *         0 = Isochronous writes are high priority
	 *         1 = Isochronous writes are medium priority
	 * [31:31] Disable Medium Priority Isochronous writes
	 *         0 = Isochronous writes are medium are high
	 *         1 = With bit 30 set makes Isochrouns writes low priority.
	 */
	0xc068, 0x00800000, 0x0f00840f,
	/* HT Initialization Control Register
	 * F0:0x68
	 * [ 0: 0] Routing Table Disable
	 *         0 = Packets are routed according to routing tables
	 *         1 = Packets are routed according to the default link field
	 * [ 1: 1] Request Disable (BSP should clear this)
	 *         0 = Request packets may be generated
	 *         1 = Request packets may not be generated.
	 * [ 3: 2] Default Link (Read-only)
	 *         00 = LDT0
	 *         01 = LDT1
	 *         10 = LDT2
	 *         11 = CPU on same node
	 * [ 4: 4] Cold Reset
	 *         - Scratch bit cleared by a cold reset
	 * [ 5: 5] BIOS Reset Detect
	 *         - Scratch bit cleared by a cold reset
	 * [ 6: 6] INIT Detect
	 *         - Scratch bit cleared by a warm or cold reset not by an INIT
	 *
	 */
	0xc06C, 0xffffff8c, 0x00000000,
	/* LDTi Capabilities Registers
	 * F0:0x80 i = 0,
	 * F0:0xA0 i = 1,
	 * F0:0xC0 i = 2,
	 */
	/* LDTi Link Control Registrs
	 * F0:0x84 i = 0,
	 * F0:0xA4 i = 1,
	 * F0:0xC4 i = 2,
	 * [ 1: 1] CRC Flood Enable
	 *         0 = Do not generate sync packets on CRC error
	 *         1 = Generate sync packets on CRC error
	 * [ 2: 2] CRC Start Test (Read-Only)
	 * [ 3: 3] CRC Force Frame Error
	 *         0 = Do not generate bad CRC
	 *         1 = Generate bad CRC
	 * [ 4: 4] Link Failure
	 *         0 = No link failure detected
	 *         1 = Link failure detected
	 * [ 5: 5] Initialization Complete
	 *         0 = Initialization not complete
	 *         1 = Initialization complete
	 * [ 6: 6] Receiver off
	 *         0 = Recevier on
	 *         1 = Receiver off
	 * [ 7: 7] Transmitter Off
	 *         0 = Transmitter on
	 *         1 = Transmitter off
	 * [ 9: 8] CRC_Error
	 *         00 = No error
	 *         [0] = 1 Error on byte lane 0
	 *         [1] = 1 Error on byte lane 1
	 * [12:12] Isochrnous Enable  (Read-Only)
	 * [13:13] HT Stop Tristate Enable
	 *         0 = Driven during an LDTSTOP_L
	 *         1 = Tristated during and LDTSTOP_L
	 * [14:14] Extended CTL Time 
	 *         0 = CTL is asserted for 16 bit times during link initialization
	 *         1 = CTL is asserted for 50us during link initialization
	 * [18:16] Max Link Width In (Read-Only?)
	 *         000 = 8 bit link
	 *         001 = 16bit link
	 * [19:19] Doubleword Flow Control in (Read-Only)
	 *         0 = This link does not support doubleword flow control
	 *         1 = This link supports doubleword flow control
	 * [22:20] Max Link Width Out (Read-Only?)
	 *         000 = 8 bit link
	 *         001 = 16bit link
	 * [23:23] Doubleworld Flow Control out (Read-Only)
	 *         0 = This link does not support doubleword flow control
	 *         1 = This link supports doubleworkd flow control
	 * [26:24] Link Width In
	 *         000 = Use 8 bits
	 *         001 = Use 16 bits
	 *         010 = reserved
	 *         011 = Use 32 bits
	 *         100 = Use 2 bits
	 *         101 = Use 4 bits
	 *         110 = reserved
	 *         111 = Link physically not connected
	 * [27:27] Doubleword Flow Control In Enable
	 *         0 = Doubleword flow control disabled
	 *         1 = Doubleword flow control enabled (Not currently supported)
	 * [30:28] Link Width Out
	 *         000 = Use 8 bits
	 *         001 = Use 16 bits
	 *         010 = reserved
	 *         011 = Use 32 bits
	 *         100 = Use 2 bits
	 *         101 = Use 4 bits
	 *         110 = reserved
	 *         111 = Link physically not connected
	 * [31:31] Doubleworld Flow Control Out Enable
	 *         0 = Doubleworld flow control disabled
	 *         1 = Doubleword flow control enabled (Not currently supported)
	 */
	0xc084, 0x00009c05, 0x11110020,
	/* LDTi Frequency/Revision Registers
	 * F0:0x88 i = 0,
	 * F0:0xA8 i = 1,
	 * F0:0xC8 i = 2,
	 * [ 4: 0] Minor Revision
	 *         Contains the HT Minor revision
	 * [ 7: 5] Major Revision
	 *         Contains the HT Major revision
	 * [11: 8] Link Frequency  (Takes effect the next time the link is reconnected)
	 *         0000 = 200Mhz
	 *         0001 = reserved
	 *         0010 = 400Mhz
	 *         0011 = reserved
	 *         0100 = 600Mhz
	 *         0101 = 800Mhz
	 *         0110 = 1000Mhz
	 *         0111 = reserved
	 *         1000 = reserved
	 *         1001 = reserved
	 *         1010 = reserved
	 *         1011 = reserved
	 *         1100 = reserved
	 *         1101 = reserved
	 *         1110 = reserved
	 *         1111 = 100 Mhz
	 * [15:12] Error (Not currently Implemented)
	 * [31:16] Indicates the frequency capabilities of the link
	 *         [16] = 1 encoding 0000 of freq supported
	 *         [17] = 1 encoding 0001 of freq supported
	 *         [18] = 1 encoding 0010 of freq supported
	 *         [19] = 1 encoding 0011 of freq supported
	 *         [20] = 1 encoding 0100 of freq supported
	 *         [21] = 1 encoding 0101 of freq supported
	 *         [22] = 1 encoding 0110 of freq supported
	 *         [23] = 1 encoding 0111 of freq supported
	 *         [24] = 1 encoding 1000 of freq supported
	 *         [25] = 1 encoding 1001 of freq supported
	 *         [26] = 1 encoding 1010 of freq supported
	 *         [27] = 1 encoding 1011 of freq supported
	 *         [28] = 1 encoding 1100 of freq supported
	 *         [29] = 1 encoding 1101 of freq supported
	 *         [30] = 1 encoding 1110 of freq supported
	 *         [31] = 1 encoding 1111 of freq supported
	 */
	0xC088, 0xfffff0ff, 0x00000200,
	/* LDTi Feature Capability
	 * F0:0x8C i = 0,
	 * F0:0xAC i = 1,
	 * F0:0xCC i = 2,
	 */
	/* LDTi Buffer Count Registers
	 * F0:0x90 i = 0,
	 * F0:0xB0 i = 1,
	 * F0:0xD0 i = 2,
	 */
	/* LDTi Bus Number Registers
	 * F0:0x94 i = 0,
	 * F0:0xB4 i = 1,
	 * F0:0xD4 i = 2,
	 * For NonCoherent HT specifies the bus number downstream (behind the host bridge)
	 * [ 0: 7] Primary Bus Number
	 * [15: 8] Secondary Bus Number
	 * [23:15] Subordiante Bus Number
	 * [31:24] reserved
	 */
	0xC094, 0xff000000, 0x00ff0000,
	/* LDTi Type Registers
	 * F0:0x98 i = 0,
	 * F0:0xB8 i = 1,
	 * F0:0xD8 i = 2,
	 */
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
	0xC144, 0x0000f8f8, 0x003f0000,
	0xC148, 0x0000f8f8, 0x00000001,
	0xC154, 0x0000f8f8, 0x00000002,
	0xC158, 0x0000f8f8, 0x00000003,
	0xC164, 0x0000f8f8, 0x00000004,
	0xC168, 0x0000f8f8, 0x00000005,
	0xC174, 0x0000f8f8, 0x00000006,
	0xC178, 0x0000f8f8, 0x00000007,
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
	0xC140, 0x0000f8fc, 0x00000003,
	0xC148, 0x0000f8fc, 0x00400000,
	0xC150, 0x0000f8fc, 0x00400000,
	0xC158, 0x0000f8fc, 0x00400000,
	0xC160, 0x0000f8fc, 0x00400000,
	0xC168, 0x0000f8fc, 0x00400000,
	0xC170, 0x0000f8fc, 0x00400000,
	0xC178, 0x0000f8fc, 0x00400000,

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
	0xC184, 0x00000048, 0x00e1ff00,
	0xC18C, 0x00000048, 0x00dfff00,
	0xC194, 0x00000048, 0x00e3ff00,
	0xC19C, 0x00000048, 0x00000000,
	0xC1A4, 0x00000048, 0x00000000,
	0xC1AC, 0x00000048, 0x00000000,
	0xC1B4, 0x00000048, 0x00000b00,


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
	0xC1BC, 0x00000048, 0x00fe0b00,
	0xC180, 0x000000f0, 0x00e00003,
	0xC188, 0x000000f0, 0x00d80003,
	0xC190, 0x000000f0, 0x00e20003,
	0xC198, 0x000000f0, 0x00000000,
	0xC1A0, 0x000000f0, 0x00000000,
	0xC1A8, 0x000000f0, 0x00000000,
	0xC1B0, 0x000000f0, 0x0000a003,
	0xC1B8, 0x000000f0, 0x00400003,

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
	0xC1C4, 0xFE000FC8, 0x0000d000,
	0xC1CC, 0xFE000FC8, 0x000ff000,
	0xC1D4, 0xFE000FC8, 0x00000000,
	0xC1DC, 0xFE000FC8, 0x00000000,

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
	0xC1C0, 0xFE000FCC, 0x0000d003,
	0xC1C8, 0xFE000FCC, 0x00001013,
	0xC1D0, 0xFE000FCC, 0x00000000,
	0xC1D8, 0xFE000FCC, 0x00000000,

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
	0xC1E0, 0x0000FC88, 0xff000003,
	0xC1E4, 0x0000FC88, 0x00000000,
	0xC1E8, 0x0000FC88, 0x00000000,
	0xC1EC, 0x0000FC88, 0x00000000,

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
	0xC240, 0x001f01fe, 0x00000001,
	0xC244, 0x001f01fe, 0x01000001,
	0xC248, 0x001f01fe, 0x02000001,
	0xC24C, 0x001f01fe, 0x03000001,
	0xC250, 0x001f01fe, 0x00000000,
	0xC254, 0x001f01fe, 0x00000000,
	0xC258, 0x001f01fe, 0x00000000,
	0xC25C, 0x001f01fe, 0x00000000,
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
	0xC260, 0xC01f01ff, 0x00e0fe00,
	0xC264, 0xC01f01ff, 0x00e0fe00,
	0xC268, 0xC01f01ff, 0x00e0fe00,
	0xC26C, 0xC01f01ff, 0x00e0fe00,
	0xC270, 0xC01f01ff, 0x00000000,
	0xC274, 0xC01f01ff, 0x00000000,
	0xC278, 0xC01f01ff, 0x00000000,
	0xC27C, 0xC01f01ff, 0x00000000,
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
	0xC280, 0xffff8888, 0x00000033,
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
	0xC288, 0xe8088008, 0x03623125,
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
	0xC28c, 0xff8fe08e, 0x00000930,

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
	0xC290, 0xf0000000, 
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
	0xC294, 0xc180f0f0, 0x0e2b0a05,
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
	0xC298, 0xfc00ffff, 0x00000000,
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
	0xC358, 0xffe0e0e0, 0x00000000,
	/* DRAM Scrub Address Low Register
	 * F3:0x5C
	 * [ 0: 0] DRAM Scrubber Redirect Enable
	 *         0 = Do nothing
	 *         1 = Scrubber Corrects errors found in normal operation
	 * [ 5: 1] Reserved
	 * [31: 6] DRAM Scrub Address 31-6
	 */
	0xC35C, 0x0000003e, 0x00000000,
	/* DRAM Scrub Address High Register
	 * F3:0x60
	 * [ 7: 0] DRAM Scrubb Address 39-32
	 * [31: 8] Reserved
	 */
	0xC360, 0xffffff00, 0x00000000,
	};
	int i;
	int max;
	print_debug("setting up northbridge registers\r\n");
	max = sizeof(register_values)/sizeof(register_values[0]);
	for(i = 0; i < max; i += 3) {
		unsigned long reg;
#if 0
		print_debug_hex32(register_values[i]);
		print_debug(" <-");
		print_debug_hex32(register_values[i+2]);
		print_debug("\r\n");
#endif
		reg = pcibios_read_config_dword(
			0, register_values[i] >> 8, register_values[i] & 0xff);
		reg &= register_values[i+1];
		reg |= register_values[i+2] & ~register_values[i+1];
		pcibios_write_config_dword(
			0, register_values[i] >> 8, register_values[i] & 0xff, reg);
	}
	print_debug("setting up northbridge registers done. hurray!\r\n");
}

#define PCI_DEVFN(slot,func)	((((slot) & 0x1f) << 3) | ((func) & 0x07))

#define DRAM_CONFIG_LOW 0x90
#define  DCL_DLL_Disable   (1<<0)
#define  DCL_D_DRV         (1<<1)
#define  DCL_QFC_EN        (1<<2)
#define  DCL_DisDqsHys     (1<<3)
#define  DCL_DramInit      (1<<8)
#define  DCL_DramEnable    (1<<10)
#define  DCL_MemClrStatus  (1<<11)
#define  DCL_DimmEcEn      (1<<17)

static void sdram_set_spd_registers(void) 
{
	unsigned long dcl;
	dcl = pcibios_read_config_dword(0, PCI_DEVFN(0x18,2), DRAM_CONFIG_LOW);
	/* Until I know what is going on disable ECC support */
	dcl &= ~DCL_DimmEcEn;
	pcibios_write_config_dword(0, PCI_DEVFN(0x18,2), DRAM_CONFIG_LOW, dcl);
}

#define TIMEOUT_LOOPS 300000
static void sdram_enable(void)
{
	unsigned long dcl;

	/* Toggle DisDqsHys to get it working */
	dcl = pcibios_read_config_dword(0, PCI_DEVFN(0x18,2), DRAM_CONFIG_LOW);
	print_debug("dcl: ");
	print_debug_hex32(dcl);
	print_debug("\r\n");
	dcl |= DCL_DisDqsHys;
	pcibios_write_config_dword(0, PCI_DEVFN(0x18, 2), DRAM_CONFIG_LOW, dcl);
	dcl &= ~DCL_DisDqsHys;
	dcl &= ~DCL_DLL_Disable;
	dcl &= ~DCL_D_DRV;
	dcl &= ~DCL_QFC_EN;
	dcl |= DCL_DramInit;
	pcibios_write_config_dword(0, PCI_DEVFN(0x18, 2), DRAM_CONFIG_LOW, dcl);
	
	print_debug("Initializing memory: ");
	int loops = 0;
	do {
		dcl = pcibios_read_config_dword(0, PCI_DEVFN(0x18, 2), DRAM_CONFIG_LOW);
		loops += 1;
		if ((loops & 1023) == 0) {
			print_debug(" ");
			print_debug_hex32(loops);
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
		dcl = pcibios_read_config_dword(0, PCI_DEVFN(0x18, 2), DRAM_CONFIG_LOW);
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

static int sdram_enabled(void)
{
	unsigned long dcl;
	int enabled;
	dcl = pcibios_read_config_dword(0, PCI_DEVFN(0x18, 2), DRAM_CONFIG_LOW);
	enabled = !!(dcl & DCL_DramEnable);
	if (enabled) {
		print_debug("DRAM already enabled.");
	}
	return enabled;
}


#include "sdram/generic_sdram.c"

static void main(void)
{
	uart_init();
	console_init();
	if (!sdram_enabled()) {
		sdram_initialize();
#if 0
		ram_fill(  0x00100000, 0x00180000);
		ram_verify(0x00100000, 0x00180000);
#endif
#if 1
		ram_fill(  0x00000000, 0x00001000);
		ram_verify(0x00000000, 0x00001000);
#endif
	}
}
