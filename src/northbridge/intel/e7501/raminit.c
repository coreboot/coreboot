
/* This was originally for the e7500, modified for e7501
 * The primary differences are that 7501 apparently can 
 * support single channel RAM (i haven't tested),
 * CAS1.5 is no longer supported, The ECC scrubber
 * now supports a mode to zero RAM and init ECC in one step
 * and the undocumented registers at 0x80 require new 
 * (undocumented) values determined by guesswork and
 * comparison w/ OEM BIOS values.
 * Steven James 02/06/2003
 */

/* converted to C 6/2004 yhlu */

#define DEBUG_RAM_CONFIG 1

#define dumpnorth() dump_pci_device(PCI_DEV(0, 0, 0)) 

/* DDR DIMM Mode register Definitions */

#define BURST_2           (1<<0)
#define BURST_4           (2<<0)
#define BURST_8           (3<<0)

#define BURST_SEQUENTIAL  (0<<3)
#define BURST_INTERLEAVED (1<<3)

#define CAS_2_0	          (0x2<<4)
#define CAS_3_0           (0x3<<4)
#define CAS_1_5           (0x5<<4)
#define CAS_2_5           (0x6<<4)

#define MODE_NORM         (0 << 7)
#define MODE_DLL_RESET    (2 << 7)
#define MODE_TEST         (1 << 7)

#define BURST_LENGTH BURST_4
#define BURST_TYPE   BURST_INTERLEAVED
#define CAS_LATENCY  CAS_2_0
//#define CAS_LATENCY  CAS_2_5
//#define CAS_LATENCY  CAS_1_5

#define MRS_VALUE (MODE_NORM | CAS_LATENCY | BURST_TYPE | BURST_LENGTH)
#define EMRS_VALUE 0x000

#define MD_SHIFT 4

#define RAM_COMMAND_NONE	0x0
#define RAM_COMMAND_NOP		0x1
#define RAM_COMMAND_PRECHARGE	0x2
#define RAM_COMMAND_MRS		0x3
#define RAM_COMMAND_EMRS	0x4
#define RAM_COMMAND_CBR		0x6
#define RAM_COMMAND_NORMAL	0x7


static inline void do_ram_command (const struct mem_controller *ctrl, uint32_t value) {
        uint32_t dword;
        uint8_t byte;
        int i;
        uint32_t result;
#if DEBUG_RAM_CONFIG >= 2
        print_debug("P:");
        print_debug_hex8(value);
        print_debug("\r\n");
#endif
        /* %ecx - initial address to read from */
        /* Compute the offset */
        dword = value >> 16;
        for(i=0;i<8;i++) {
                /* Set the ram command */
                byte = pci_read_config8(ctrl->d0, 0x7c);
                byte &= 0x8f;
                byte |= (uint8_t)(value & 0xff);
                pci_write_config8(ctrl->d0, 0x7c, byte);

                /* Assert the command to the memory */
#if DEBUG_RAM_CONFIG  >= 2
                print_debug("R:");
                print_debug_hex32(dword);
                print_debug("\r\n");
#endif

                result = read32(dword);

                /* Go to the next base address */
                dword += 0x04000000;

        } 

        /* The command has been sent to all dimms so get out */
}


static inline void RAM_CMD(const struct mem_controller *ctrl, uint32_t command, uint32_t offset)  {
	uint32_t value = 	((offset) << (MD_SHIFT + 16))|((command << 4) & 0x70) ; 
	do_ram_command(ctrl, value);
}
	
#define RAM_NOP(ctrl)		RAM_CMD(ctrl, RAM_COMMAND_NOP, 0)
#define RAM_PRECHARGE(ctrl)		RAM_CMD(ctrl, RAM_COMMAND_PRECHARGE, 0)
#define RAM_CBR(ctrl)		RAM_CMD(ctrl, RAM_COMMAND_CBR, 0)
#define RAM_EMRS(ctrl)		RAM_CMD(ctrl, RAM_COMMAND_EMRS, EMRS_VALUE)

static const uint8_t ram_cas_latency[] = {
	CAS_2_5, CAS_2_0, CAS_1_5, CAS_2_5
	};

static inline void ram_mrs(const struct mem_controller *ctrl, uint32_t value){
	/* Read the cas latency setting */
	uint8_t byte;
	uint32_t dword;
	byte = pci_read_config8(ctrl->d0, 0x78); 
	/* Transform it into the form expected by SDRAM */
	dword = ram_cas_latency[(byte>>4) & 3];

	value  |= (dword<<(16+MD_SHIFT));
	
	value |= (MODE_NORM | BURST_TYPE | BURST_LENGTH) << (16+MD_SHIFT);

	do_ram_command(ctrl, value);
}

#define RAM_MRS(ctrl, dll_reset) ram_mrs( ctrl, (dll_reset << (8+MD_SHIFT+ 16)) | ((RAM_COMMAND_MRS <<4)& 0x70) )

static void RAM_NORMAL(const struct mem_controller *ctrl) {
	uint8_t byte;
	byte = pci_read_config8(ctrl->d0, 0x7c);
	byte &=  0x8f;
	byte |= (RAM_COMMAND_NORMAL << 4);
	pci_write_config8(ctrl->d0, 0x7c, byte);
}

static void  RAM_RESET_DDR_PTR(const struct mem_controller *ctrl) {
	uint8_t byte;
	byte = pci_read_config8(ctrl->d0, 0x88);
	byte |= (1 << 4 );
	pci_write_config8(ctrl->d0, 0x88, byte);
	byte = pci_read_config8(ctrl->d0, 0x88);
	byte &= ~(1 << 4);
	pci_write_config8(ctrl->d0, 0x88, byte);
}

static void ENABLE_REFRESH(const struct mem_controller *ctrl) 
{
	uint32_t dword;
	dword = pci_read_config32(ctrl->d0, 0x7c);
	dword |= (1 << 29);
	pci_write_config32(ctrl->d0, 0x7c, dword);
}

	/*
	 * Table:	constant_register_values
	 */
static const long register_values[] = {
	/* SVID - Subsystem Vendor Identification Register
	 * 0x2c - 0x2d
	 * [15:00] Subsytem Vendor ID (Indicates system board vendor)
	 */
	/* SID - Subsystem Identification Register
	 * 0x2e - 0x2f
	 * [15:00] Subsystem ID
	 */
	 0x2c, 0, (0x15d9 << 0) | (0x3580 << 16),

	/* Undocumented
	 * 0x80 - 0x80
	 * This register has something to do with CAS latencies,
	 * possibily this is the real chipset control.
	 * At 0x00 CAS latency 1.5 works.
	 * At 0x06 CAS latency 2.5 works.
	 * At 0x01 CAS latency 2.0 works.
	 */
	/* This is still undocumented in e7501, but with different values
	 * CAS 2.0 values taken from Intel BIOS settings, others are a guess
	 * and may be terribly wrong. Old values preserved as comments until I
	 * figure this out for sure.
	 * e7501 docs claim that CAS1.5 is unsupported, so it may or may not 
	 * work at all.
	 * Steven James 02/06/2003
	 */
#if CAS_LATENCY == CAS_2_5
//	0x80, 0xfffffe00, 0x06 /* Intel E7500 recommended */
	0x80, 0xfffff000, 0x0662, /* from Factory Bios */
#elif CAS_LATENCY == CAS_2_0
//	0x80, 0xfffffe00, 0x0d /* values for register 0x80 */
	0x80, 0xfffff000, 0x0bb1, /* values for register 0x80 */
#endif

	/* Enable periodic memory recalibration */
	0x88, 0xffffff00, 0x80,

	/* FDHC - Fixed DRAM Hole Control
	 * 0x58
	 * [7:7] Hole_Enable
	 *       0 == No memory Hole
	 *       1 == Memory Hole from 15MB to 16MB
	 * [6:0] Reserved
	 *
	 * PAM - Programmable Attribute Map
	 * 0x59 [1:0] Reserved
	 * 0x59 [5:4] 0xF0000 - 0xFFFFF
	 * 0x5A [1:0] 0xC0000 - 0xC3FFF
	 * 0x5A [5:4] 0xC4000 - 0xC7FFF
	 * 0x5B [1:0] 0xC8000 - 0xCBFFF
	 * 0x5B [5:4] 0xCC000 - 0xCFFFF
	 * 0x5C [1:0] 0xD0000 - 0xD3FFF
	 * 0x5C [5:4] 0xD4000 - 0xD7FFF
	 * 0x5D [1:0] 0xD8000 - 0xDBFFF
	 * 0x5D [5:4] 0xDC000 - 0xDFFFF
	 * 0x5E [1:0] 0xE0000 - 0xE3FFF
	 * 0x5E [5:4] 0xE4000 - 0xE7FFF
	 * 0x5F [1:0] 0xE8000 - 0xEBFFF
	 * 0x5F [5:4] 0xEC000 - 0xEFFFF
	 *       00 == DRAM Disabled (All Access go to memory mapped I/O space)
	 *       01 == Read Only (Reads to DRAM, Writes to memory mapped I/O space)
	 *       10 == Write Only (Writes to DRAM, Reads to memory mapped I/O space)
	 *       11 == Normal (All Access go to DRAM)
	 */
	0x58, 0xcccccf7f, (0x00 << 0) | (0x30 << 8) | (0x33 << 16) | (0x33 << 24),
	0x5C, 0xcccccccc, (0x33 << 0) | (0x33 << 8) | (0x33 << 16) | (0x33 << 24),

	/* DRB - DRAM Row Boundary Registers
	 * 0x60 - 0x6F
	 *     An array of 8 byte registers, which hold the ending
	 *     memory address assigned  to each pair of DIMMS, in 64MB 
	 *     granularity.   
	 */
	/* Conservatively say each row has 64MB of ram, we will fix this up later */
	0x60, 0x00000000, (0x01 << 0) | (0x02 << 8) | (0x03 << 16) | (0x04 << 24),
	0x64, 0x00000000, (0x05 << 0) | (0x06 << 8) | (0x07 << 16) | (0x08 << 24),
	0x68, 0xffffffff, 0,
	0x6C, 0xffffffff, 0,

	/* DRA - DRAM Row Attribute Register 
	 * 0x70 Row 0,1
	 * 0x71 Row 2,3
 	 * 0x72 Row 4,5
	 * 0x73 Row 6,7
	 * [7:7] Device width for Odd numbered rows
	 *       0 == 8 bits wide x8
	 *       1 == 4 bits wide x4
	 * [6:4] Row Attributes for Odd numbered rows
	 *       010 == 8KB
	 *       011 == 16KB
	 *       100 == 32KB
	 *       101 == 64KB
	 *       Others == Reserved
	 * [3:3] Device width for Even numbered rows
	 *       0 == 8 bits wide x8
	 *       1 == 4 bits wide x4
	 * [2:0] Row Attributes for Even numbered rows
	 *       010 == 8KB
	 *       011 == 16KB
	 *       100 == 32KB
	 *       101 == 64KB (This page size appears broken)
	 *       Others == Reserved
	 */
	0x70, 0x00000000, 
		(((0<<3)|(0<<0))<< 0) | 
		(((0<<3)|(0<<0))<< 4) | 
		(((0<<3)|(0<<0))<< 8) | 
		(((0<<3)|(0<<0))<<12) | 
		(((0<<3)|(0<<0))<<16) | 
		(((0<<3)|(0<<0))<<20) | 
		(((0<<3)|(0<<0))<<24) | 
		(((0<<3)|(0<<0))<<28),
	0x74, 0xffffffff, 0,

	/* DRT - DRAM Time Register
	 * 0x78
	 * [31:30] Reserved
	 * [29:29] Back to Back Write-Read Turn Around
	 *         0 == 3 clocks between WR-RD commands
	 *         1 == 2 clocks between WR-RD commands
	 * [28:28] Back to Back Read-Write Turn Around
	 *         0 == 5 clocks between RD-WR commands
	 *         1 == 4 clocks between RD-WR commands
	 * [27:27] Back to Back Read Turn Around
	 *         0 == 4 clocks between RD commands
	 *         1 == 3 clocks between RD commands
	 * [26:24] Read Delay (tRD)
	 *         000 == 7 clocks
	 *         001 == 6 clocks
	 *         010 == 5 clocks
	 *         Others == Reserved
	 * [23:19] Reserved
	 * [18:16] DRAM idle timer
	 *	000 == infinite
	 *	011 == 16 dram clocks
	 *	001 == Datasheet says reserved, but Intel BIOS sets it
	 * [15:11] Reserved
	 * [10:09] Active to Precharge (tRAS)
	 *         00 == 7 clocks
	 *         01 == 6 clocks
	 *         10 == 5 clocks
	 *         11 == Reserved
	 * [08:06] Reserved
	 * [05:04] Cas Latency (tCL)
	 *         00 == 2.5 Clocks
	 *         01 == 2.0 Clocks
	 *         10 == 1.5 Clocks
	 *         11 == Reserved
	 * [03:03] Write Ras# to Cas# Delay (tRCD)
	 *         0 == 3 DRAM Clocks
	 *         1 == 2 DRAM Clocks
 	 * [02:01] Read RAS# to CAS# Delay (tRCD)
 	 *         00 == reserved
 	 *         01 == reserved
	 *         10 == 3 DRAM Clocks
	 *         11 == 2 DRAM Clocks
	 * [00:00] DRAM RAS# to Precharge (tRP)
	 *         0 == 3 DRAM Clocks
	 *         1 == 2 DRAM Clocks
	 */
#define DRT_CAS_2_5 (0<<4)
#define DRT_CAS_2_0 (1<<4)   
#define DRT_CAS_1_5 (2<<4)
#define DRT_CAS_MASK (3<<4)

#if CAS_LATENCY == CAS_2_5
#define DRT_CL DRT_CAS_2_5
#elif CAS_LATENCY == CAS_2_0
#define DRT_CL DRT_CAS_2_0
#elif CAS_LATENCY == CAS_1_5
#define DRT_CL DRT_CAS_1_5
#endif

	/* Most aggressive settings possible */
//	0x78, 0xc0fff8c4, (1<<29)|(1<<28)|(1<<27)|(2<<24)|(2<<9)|DRT_CL|(1<<3)|(1<<1)|(1<<0),
//	0x78, 0xc0f8f8c0, (1<<29)|(1<<28)|(1<<27)|(1<<24)|(1<<16)|(2<<9)|DRT_CL|(1<<3)|(3<<1)|(1<<0),
	0x78, 0xc0f8f9c0, (1<<29)|(1<<28)|(1<<27)|(1<<24)|(1<<16)|(2<<9)|DRT_CL|(1<<3)|(3<<1)|(1<<0),

	/* FIXME why was I attempting to set a reserved bit? */
	/* 0x0100040f */

	/* DRC - DRAM Contoller Mode Register
	 * 0x7c
	 * [31:30] Reserved
	 * [29:29] Initialization Complete
	 *         0 == Not Complete
	 *         1 == Complete
	 * [28:23] Reserved
	 * [22:22]	   Channels
	 * 		0 == Single channel
	 *		1 == Dual Channel
	 * [21:20] DRAM Data Integrity Mode
	 *         00 == Disabled, no ECC
	 *         01 == Reserved
	 *         10 == Error checking, using chip-kill, with correction
	 *         11 == Reserved
	 * [19:18] Reserved
	 *         Must equal 01
	 * [17:17] (Intel Undocumented)	should always be set to 1
	 * [16:16] Command Per Clock - Address/Control Assertion Rule (CPC)
	 *         0 == 2n Rule
	 *         1 == 1n rule
	 * [15:11] Reserved
	 * [10:08] Refresh mode select
	 *         000 == Refresh disabled
	 *         001 == Refresh interval 15.6 usec
	 *         010 == Refresh interval 7.8 usec
	 *         011 == Refresh interval 64 usec
	 *         111 == Refresh every 64 clocks (fast refresh)
	 * [07:07] Reserved
	 * [06:04] Mode Select (SMS)
	 *         000 == Self Refresh Mode
	 *         001 == NOP Command
	 *         010 == All Banks Precharge
	 *         011 == Mode Register Set
 	 *         100 == Extended Mode Register Set
	 *         101 == Reserved
	 *         110 == CBR Refresh
	 *         111 == Normal Operation
	 * [03:00] Reserved
	 */
//	.long 0x7c, 0xffcefcff, (1<<22)|(2 << 20)|(1 << 16)| (0 << 8),
//	.long 0x7c, 0xff8cfcff, (1<<22)|(2 << 20)|(1 << 17)|(1 << 16)| (0 << 8),
//	.long 0x7c, 0xff80fcff, (1<<22)|(2 << 20)|(1 << 18)|(1 << 17)|(1 << 16)| (0 << 8),
	0x7c, 0xff82fcff, (1<<22)|(2 << 20)|(1 << 18)|(1 << 16)| (0 << 8),


	/* Another Intel undocumented register */
	0x88, 0x080007ff, (1<<31)|(1 << 30)|(1<<28)|(0 << 26)|(0x10 << 21)|(10 << 16)|(0x13 << 11),

	/* CLOCK_DIS - CK/CK# Disable Register
	 * 0x8C
	 * [7:4] Reserved
	 * [3:3] CK3
	 *       0 == Enable
	 *       1 == Disable
	 * [2:2] CK2
	 *       0 == Enable
	 *       1 == Disable
	 * [1:1] CK1
	 *       0 == Enable
	 *       1 == Disable
	 * [0:0] CK0
	 *       0 == Enable
	 *       1 == Disable
	 */
	0x8C, 0xfffffff0, 0xf,

	/* TOLM - Top of Low Memory Register
	 * 0xC4 - 0xC5
	 * [15:11] Top of low memory (TOLM)
	 *         The address below 4GB that should be treated as RAM,
	 *         on a 128MB granularity.
	 * [10:00] Reserved
	 */
	/* REMAPBASE - Remap Base Address Regsiter
	 * 0xC6 - 0xC7
	 * [15:10] Reserved
	 * [09:00] Remap Base Address [35:26] 64M aligned
	 *         Bits [25:0] are assumed to be 0.
	 */
	0xc4, 0xfc0007ff, (0x2000 << 0) | (0x3ff << 16),
	/* REMAPLIMIT - Remap Limit Address Register
	 * 0xC8 - 0xC9
	 * [15:10] Reserved
	 * [09:00] Remap Limit Address [35:26] 64M aligned
	 * When remaplimit < remapbase this register is disabled.
	 */
	0xc8, 0xfffffc00, 0,

	/* DVNP - Device Not Present Register
	 * 0xE0 - 0xE1
	 * [15:05] Reserved
 	 * [04:04] Device 4 Function 1 Present
	 *         0 == Present
	 *         1 == Absent
 	 * [03:03] Device 3 Function 1 Present
	 *         0 == Present
	 *         1 == Absent
 	 * [02:02] Device 2 Function 1 Present
	 *         0 == Present
	 *         1 == Absent
 	 * [01:01] Reserved
 	 * [00:00] Device 0 Function 1 Present
	 *         0 == Present
	 *         1 == Absent
	 */
	0xe0, 0xffffffe2, (1<<4)|(1<<3)|(1<<2)|(0<<0),
	0xd8, 0xffff9fff, 0x00000000,
	0xf4, 0x3f8ffffd, 0x40300002,
	0x1050, 0xffffffcf, 0x00000030,
};


	/*
	 * Routine:	ram_set_registers
	 * Arguments:	none
	 * Results:	none
	 * Trashed:	%eax, %ebx, %ecx, %edx, %esi, %eflags
	 * Effects:	Do basic ram setup that does not depend on serial
	 *		presence detect information.
	 *		This sets PCI configuration registers to known good
	 *		values based on the table: 
	 *			constant_register_values
	 *		Which are a triple of configuration regiser, mask, and value.
	 *		
	 */
/* from 1M or 512K */
#define RCOMP_MMIO 0x100000

	/* DDR RECOMP table */

static const long ddr_rcomp_1[] = {
	0x44332211, 0xc9776655, 0xffffffff, 0xffffffff,
	0x22111111, 0x55444332, 0xfffca876, 0xffffffff,
};
static const long ddr_rcomp_2[] = {
	0x00000000, 0x76543210, 0xffffeca8, 0xffffffff,
	0x21000000, 0xa8765432, 0xffffffec, 0xffffffff,
};
static const long ddr_rcomp_3[] = {
	0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
	0x88888888, 0x88888888, 0x88888888, 0x88888888,
};

#define rcomp_init_str "Setting RCOMP registers.\r\n"

static void write_8dwords(uint32_t src_addr, uint32_t dst_addr) {
	int i;
	uint32_t dword;
	for(i=0;i<8;i++) {
		dword = read32(src_addr);
		write32(dst_addr, dword);
		src_addr+=4;
		dst_addr+=4;
		
	}
}

#if 1
#define SLOW_DOWN_IO inb(0x80);
#else
#define SLOW_DOWN_IO udelay(40);
#endif

static void ram_set_rcomp_regs(const struct mem_controller *ctrl) {
	uint32_t dword;
#if DEBUG_RAM_CONFIG
	print_debug(rcomp_init_str);
#endif

	/*enable access to the rcomp bar */
	dword = pci_read_config32(ctrl->d0, 0x0f4);
        dword &= ~(1<<31);
        dword |=((1<<30)|1<<22);
        pci_write_config32(ctrl->d0, 0x0f4, dword);
        

	/* Set the MMIO address to 512K */
        pci_write_config32(ctrl->d0, 0x14, RCOMP_MMIO);

	dword = read32(RCOMP_MMIO + 0x20);
	dword |= (1<<9);
	write32(RCOMP_MMIO + 0x20, dword);
	

	/* Begin to write the RCOMP registers */
	
	write8(RCOMP_MMIO + 0x2c, 0xff);
	write32(RCOMP_MMIO + 0x30, 0x01040444);
	write8(RCOMP_MMIO + 0x34, 0x04);
	write32(RCOMP_MMIO + 0x40, 0);
	write16(RCOMP_MMIO + 0x44, 0);
	write16(RCOMP_MMIO + 0x48, 0);
	write16(RCOMP_MMIO + 0x50, 0);
	write_8dwords((uint32_t)ddr_rcomp_1, RCOMP_MMIO + 0x60);
	write_8dwords((uint32_t)ddr_rcomp_2, RCOMP_MMIO + 0x80);
	write_8dwords((uint32_t)ddr_rcomp_2, RCOMP_MMIO + 0xa0);
	write_8dwords((uint32_t)ddr_rcomp_2, RCOMP_MMIO + 0x140);
	write_8dwords((uint32_t)ddr_rcomp_2, RCOMP_MMIO + 0x1c0);
	write_8dwords((uint32_t)ddr_rcomp_3, RCOMP_MMIO + 0x180);

	dword = read32(RCOMP_MMIO + 0x20);
	dword &= ~(3);
	dword |= 1;
	write32(RCOMP_MMIO + 0x20, dword);

	/* Wait 40 usec */
	SLOW_DOWN_IO;
	
	/* unblock updates */
	dword = read32(RCOMP_MMIO + 0x20);
	dword &= ~(1<<9);
	write32(RCOMP_MMIO+0x20, dword);
	dword |= (1<<8);
	write32(RCOMP_MMIO+0x20, dword);
	dword &= ~(1<<8);
	write32(RCOMP_MMIO+0x20, dword);
	
	/* Wait 40 usec */
	SLOW_DOWN_IO;

	/*disable access to the rcomp bar */
	dword = pci_read_config32(ctrl->d0, 0x0f4);
	dword &= ~(1<<22);
	pci_write_config32(ctrl->d0, 0x0f4, dword);	

}

static void ram_set_d0f0_regs(const struct mem_controller *ctrl) {
#if DEBUG_RAM_CONFIG >= 2
	dumpnorth();
#endif
	int i;
	int max;
        max = sizeof(register_values)/sizeof(register_values[0]);
        for(i = 0; i < max; i += 3) {
                uint32_t reg;
#if DEBUG_RAM_CONFIG >= 2
                print_debug_hex32(register_values[i]);
                print_debug(" <-");
                print_debug_hex32(register_values[i+2]);
                print_debug("\r\n");
#endif
                reg = pci_read_config32(ctrl->d0,register_values[i]);
                reg &= register_values[i+1];
                reg |= register_values[i+2] & ~(register_values[i+1]);
                pci_write_config32(ctrl->d0,register_values[i], reg);


        }
#if DEBUG_RAM_CONFIG >= 2
	dumpnorth();
#endif
}
static void sdram_set_registers(const struct mem_controller *ctrl){
        ram_set_rcomp_regs(ctrl);
        ram_set_d0f0_regs(ctrl);
}


	/*
	 * Routine:	sdram_spd_get_page_size
	 * Arguments:	%bl SMBUS_MEM_DEVICE
	 * Results:	
	 *		%edi log base 2 page size of DIMM side 1 in bits
	 *		%esi log base 2 page size of DIMM side 2 in bits
	 *
	 * Preserved:	%ebx (except %bh), %ebp
	 *
	 * Trashed:	%eax, %bh, %ecx, %edx, %esp, %eflags
	 * Used:	%eax, %ebx, %ecx, %edx, %esi, %edi, %esp, %eflags
	 *
	 * Effects:	Uses serial presence detect to set %edi & %esi
	 *		to the page size of a dimm.
	 * Notes:
	 *              %bl SMBUS_MEM_DEVICE
	 *              %edi holds the page size for the first side of the DIMM.
	 *		%esi holds the page size for the second side of the DIMM.
	 *		     memory size is represent as a power of 2.
	 *
	 *		This routine may be worth moving into generic code somewhere.
	 */
struct dimm_page_size { 
        unsigned long side1;
        unsigned long side2;
};      
  
static struct dimm_page_size sdram_spd_get_page_size(unsigned device) {

	uint32_t ecx;
	int value;
	struct dimm_page_size pgsz;

	pgsz.side1 = 0;
	pgsz.side2 = 0;	
		
	value  = spd_read_byte(device, 4); /* columns */
	if(value < 0) goto hw_err;
	pgsz.side1 = value & 0xf;
	
	/* Get the module data width and convert it to a power of two */
	value = spd_read_byte(device,7);		/* (high byte) */
	if(value < 0) goto hw_err;
	ecx = value & 0xff;
	ecx <<= 8;

	value = spd_read_byte(device, 6);	 /* (low byte) */
	if(value < 0) goto hw_err;
	ecx |= (value & 0xff);

	pgsz.side1 += log2(ecx);	 /* compute cheap log base 2 */	

	/* side two */
	value = spd_read_byte(device, 5);	/* number of physical banks */
	if(value < 0) goto hw_err;
	if(value==1) goto out;
	if(value!=2) goto val_err;

	/* Start with the symmetrical case */
	pgsz.side2 = pgsz.side1;
	value = spd_read_byte(device,4);   /* columns */
	if(value < 0) goto hw_err;
	if((value & 0xf0)==0 ) goto out;
	pgsz.side2 -=value & 0xf; /* Subtract out columns on side 1 */
	pgsz.side2 +=(value>>4)& 0xf; /* Add in columns on side 2 */
        goto out;

 val_err:
        die("Bad SPD value\r\n");
        /* If an hw_error occurs report that I have no memory */
hw_err:
	pgsz.side1 = 0;
	pgsz.side2 = 0;
out:
	return pgsz;	
}


	/*
	 * Routine:	sdram_spd_get_width
	 * Arguments:	%bl SMBUS_MEM_DEVICE
	 * Results:	
	 *		%edi width of SDRAM chips on DIMM side 1 in bits
	 *		%esi width of SDRAM chips on DIMM side 2 in bits
	 *
	 * Preserved:	%ebx (except %bh), %ebp
	 *
	 * Trashed:	%eax, %bh, %ecx, %edx, %esp, %eflags
	 * Used:	%eax, %ebx, %ecx, %edx, %esi, %edi, %esp, %eflags
	 *
	 * Effects:	Uses serial presence detect to set %edi & %esi
	 *		to the width of a dimm.
	 * Notes:
	 *              %bl SMBUS_MEM_DEVICE
	 *              %edi holds the width for the first side of the DIMM.
	 *		%esi holds the width for the second side of the DIMM.
	 *		     memory size is represent as a power of 2.
	 *
	 *		This routine may be worth moving into generic code somewhere.
	 */
struct dimm_width {
        unsigned side1;
        unsigned side2;
};      
  
static struct dimm_width sdram_spd_get_width(unsigned device) {
	int value;
	struct dimm_width wd;
	uint32_t ecx;
	
	wd.side1 = 0;
	wd.side2 = 0;

	value = spd_read_byte(device, 13); /* sdram width */
	if(value < 0 )  goto hw_err;
	ecx = value;
	
	wd.side1 = value & 0x7f;	
	
	/* side two */
	value = spd_read_byte(device, 5); /* number of physical banks */
	if(value < 0 ) goto hw_err;	
	if(value <=1 ) goto out;

	/* Start with the symmetrical case */
	wd.side2 = wd.side1;

	if((ecx & 0x80)==0) goto out;
	
	wd.side2 <<=1;
hw_err:
	wd.side1 = 0;
	wd.side2 = 0;

 out:
        return wd;
}
	
	/*
	 * Routine:	sdram_spd_get_dimm_size
	 * Arguments:	%bl SMBUS_MEM_DEVICE
	 * Results:	
	 *		%edi log base 2 size of DIMM side 1 in bits
	 *		%esi log base 2 size of DIMM side 2 in bits
	 *
	 * Preserved:	%ebx (except %bh), %ebp
	 *
	 * Trashed:	%eax, %bh, %ecx, %edx, %esp, %eflags
	 * Used:	%eax, %ebx, %ecx, %edx, %esi, %edi, %esp, %eflags
	 *
	 * Effects:	Uses serial presence detect to set %edi & %esi
	 *		the size of a dimm.
	 * Notes:
	 *              %bl SMBUS_MEM_DEVICE
	 *              %edi holds the memory size for the first side of the DIMM.
	 *		%esi holds the memory size for the second side of the DIMM.
	 *		     memory size is represent as a power of 2.
	 *
	 *		This routine may be worth moving into generic code somewhere.
	 */

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
        value = spd_read_byte(device, 3);       /* rows */
        if (value < 0) goto hw_err;
//        if ((value & 0xf) == 0) goto val_err;
        sz.side1 += value & 0xf;

        value = spd_read_byte(device, 4);       /* columns */
        if (value < 0) goto hw_err;
//        if ((value & 0xf) == 0) goto val_err;
        sz.side1 += value & 0xf;

        value = spd_read_byte(device, 17);      /* banks */
        if (value < 0) goto hw_err;
//        if ((value & 0xff) == 0) goto val_err;
	value &=0xff;
        sz.side1 += log2(value);

        /* Get the module data width and convert it to a power of two */
        value = spd_read_byte(device, 7);       /* (high byte) */
        if (value < 0) goto hw_err;
        value &= 0xff;
        value <<= 8;
        
        low = spd_read_byte(device, 6); /* (low byte) */
        if (low < 0) goto hw_err;
        value |= (low & 0xff);
//        if ((value != 72) && (value != 64)) goto val_err;
        sz.side1 += log2(value);
        
        /* side 2 */
        value = spd_read_byte(device, 5);       /* number of physical banks */
        if (value < 0) goto hw_err;
        if (value == 1) goto out;
//        if (value != 2) goto val_err;

        /* Start with the symmetrical case */
        sz.side2 = sz.side1;

        value = spd_read_byte(device, 3);       /* rows */
        if (value < 0) goto hw_err;
        if ((value & 0xf0) == 0) goto out;      /* If symmetrical we are done */
        sz.side2 -= (value & 0x0f);             /* Subtract out rows on side 1 */
        sz.side2 += ((value >> 4) & 0x0f);      /* Add in rows on side 2 */

        value = spd_read_byte(device, 4);       /* columns */
        if (value < 0) goto hw_err;
//        if ((value & 0xff) == 0) goto val_err;
        sz.side2 -= (value & 0x0f);             /* Subtract out columns on side 1 */
        sz.side2 += ((value >> 4) & 0x0f);      /* Add in columsn on side 2 */
        goto out;

 val_err:
        die("Bad SPD value\r\n");
        /* If an hw_error occurs report that I have no memory */
hw_err:
        sz.side1 = 0;
        sz.side2 = 0;
 out:
        return sz;
}



	/*
	 * This is a place holder fill this out
	 * Routine:	spd_set_row_attributes 
	 * Arguments:	%bl SMBUS_MEM_DEVICE
	 * Results:	
	 *		%edi log base 2 size of DIMM side 1 in bits
	 *		%esi log base 2 size of DIMM side 2 in bits
	 *
	 * Preserved:	%ebx (except %bh), %ebp
	 *
	 * Trashed:	%eax, %bh, %ecx, %edx, %esp, %eflags
	 * Used:	%eax, %ebx, %ecx, %edx, %esi, %edi, %esp, %eflags
	 *
	 * Effects:	Uses serial presence detect to set %edi & %esi
	 *		the size of a dimm.
	 * Notes:
	 *              %bl SMBUS_MEM_DEVICE
	 *              %edi holds the memory size for the first side of the DIMM.
	 *		%esi holds the memory size for the second side of the DIMM.
	 *		     memory size is represent as a power of 2.
	 *
	 *		This routine may be worth moving into generic code somewhere.
	 */
static long spd_set_row_attributes(const struct mem_controller *ctrl, long dimm_mask) {
	int i;  
        uint32_t dword=0;
        int value;
        

        /* Walk through all dimms and find the interesection of the support
         * for ecc sdram and refresh rates
         */        
        
 
        for(i = 0; i < DIMM_SOCKETS; i++) {
		if (!(dimm_mask & (1 << i))) {
                        continue;
                }
                 /* Test to see if I have ecc sdram */
		struct dimm_page_size sz;
                sz = sdram_spd_get_page_size(ctrl->channel0[i]);  /* SDRAM type */
#if DEBUG_RAM_CONFIG>=2 
		print_debug("page size =");
		print_debug_hex32(sz.side1);
		print_debug(" ");
		print_debug_hex32(sz.side2);
		print_debug("\r\n");
#endif
	
		/* Test to see if the dimm is present */
		if( sz.side1 !=0) {

			/* Test for a valid dimm width */
			if((sz.side1 <15) || (sz.side1>18) ) {
				print_err("unsupported page size\r\n");
			}

	 		/* double because I have 2 channels */
			sz.side1++;

			/* Convert to the format needed for the DRA register */
			sz.side1-=14;	

			/* Place in the %ebp the dra place holder */ //i
			dword |= sz.side1<<(i<<3);
			
			/* Test to see if the second side is present */

	                if( sz.side2 !=0) {
        
	                        /* Test for a valid dimm width */
        	                if((sz.side2 <15) || (sz.side2>18) ) {
                	                print_err("unsupported page size\r\n");
                        	}

                        	/* double because I have 2 channels */
                        	sz.side2++;

                        	/* Convert to the format needed for the DRA register */
                        	sz.side2-=14;

	                        /* Place in the %ebp the dra place holder */ //i
        	                dword |= sz.side2<<((i<<3) + 4 );

			}
		}

		/* Now add the SDRAM chip width to the DRA */
		struct dimm_width wd;
		wd = sdram_spd_get_width(ctrl->channel0[i]);

#if DEBUG_RAM_CONFIG
                print_debug("width =");
                print_debug_hex32(wd.side1);
                print_debug(" ");
                print_debug_hex32(wd.side2);
                print_debug("\r\n");
#endif
		
		if(wd.side1 == 0) continue;
		if(wd.side1 == 4) {
			/* Enable an x4 device */
			dword |= 0x08 << (i<<3);
		}

		if(wd.side2 == 0) continue;
                if(wd.side2 == 4) {
                        /* Enable an x4 device */
                        dword |= 0x08 << ((i<<3 ) + 4);
                }
		
	/* go to the next DIMM */
	}

	/* Write the new row attributes register */
	pci_write_config32(ctrl->d0, 0x70, dword);

	return dimm_mask;

}
#define spd_pre_init  "Reading SPD data...\r\n"
#define spd_pre_set "setting based on SPD data...\r\n"
#define spd_post_init "done\r\n"


static const uint32_t refresh_rate_rank[]= {
	/* Refresh rates ordered from most conservative (lowest)
	 * to most agressive (highest)
	 * disabled 0 -> rank 3 
	 * 15.6usec 1 -> rank 1
	 * 7.8 usec 2 -> rank 0
	 * 64usec   3 -> rank 2
	 */
	3, 1, 0, 2 };
static const uint32_t refresh_rate_index[] = {
	/* Map the spd refresh rates to memory controller settings 
	 * 15.625us -> 15.6us
 	 * 3.9us    -> err
	 * 7.8us    -> 7.8us
	 * 31.3s    -> 15.6us
	 * 62.5us   -> 15.6us
	 * 125us    -> 64us
	 */
	1, 0xff, 2, 1, 1, 3
};
#define MAX_SPD_REFRESH_RATE 5

static long spd_set_dram_controller_mode (const struct mem_controller *ctrl, long dimm_mask) {

        int i;  
        uint32_t dword;
        int value;
	uint32_t ecx;
	uint32_t edx;
        
        /* Read the inititial state */
        dword = pci_read_config32(ctrl->d0, 0x7c);

/*
        // Test if ECC cmos option is enabled 
        movb    $RTC_BOOT_BYTE, %al
        outb    %al, $0x70
        inb     $0x71, %al
        testb   $(1<<2), %al
        jnz     1f
        // Clear the ecc enable 
        andl    $~(3 << 20), %esi
1:
*/


        /* Walk through all dimms and find the interesection of the support
         * for ecc sdram and refresh rates
         */        

 
        for(i = 0; i < DIMM_SOCKETS; i++) {
		if (!(dimm_mask & (1 << i))) {
                        continue;
                }
		 /* Test to see if I have ecc sdram */
                value = spd_read_byte(ctrl->channel0[i], 11);  /* SDRAM type */
                if(value < 0) continue;
		if(value !=2 ) {
			/* Clear the ecc enable */
			dword &= ~(3 << 20);
		}
		value = spd_read_byte(ctrl->channel0[i], 12);  /* SDRAM refresh rate */
		if(value < 0 ) continue;
		value &= 0x7f;
		if(value > MAX_SPD_REFRESH_RATE) { print_err("unsupported refresh rate\r\n");}
//		if(value == 0xff) { print_err("unsupported refresh rate\r\n");}
		
		ecx = refresh_rate_index[value];

		/* Isolate the old refresh rate setting */
		/* Load the refresh rate ranks */
		edx = refresh_rate_rank[(dword >> 8) & 3]<<8;
		edx |= refresh_rate_rank[ecx] & 0xff; 
	
		/* See if the new refresh rate is more conservative than the old
	 	* refresh rate setting. (Lower ranks are more conservative)
	 	*/
		if((edx & 0xff)< ((edx >> 8) & 0xff) ) {
			/* Clear the old refresh rate */
			dword &= ~(3<<8);
			/* Move in the new refresh rate */
			dword |= (ecx<<8);
		}
		
		value = spd_read_byte(ctrl->channel0[i], 33); /* Address and command hold time after clock */
		if(value < 0) continue;
		if(value >= 0xa0) { 		/* At 133Mhz this constant should be 0x75 */
			dword &= ~(1<<16);	/* Use two clock cyles instead of one */
		}
	
	/* go to the next DIMM */
	}

	/* Now write the controller mode */
	pci_write_config32(ctrl->d0, 0x7c, dword);
	
	return dimm_mask;

}
static long spd_enable_clocks(const struct mem_controller *ctrl, long dimm_mask)
{
        int i;
	uint32_t dword;
	int value;

        /* Read the inititial state */
        dword = pci_read_config32(ctrl->d0, 0x8c);
/*
# Intel clears top bit here, should we?
# No the default is on and for normal timming it should be on.  Tom Z
        andl    $0x7f, %esi
*/

 
        for(i = 0; i < DIMM_SOCKETS; i++) {
		if (!(dimm_mask & (1 << i))) {
                        continue;
                }
	        /* Read any spd byte to see if the dimm is present */
	        value = spd_read_byte(ctrl->channel0[i], 5); /* Physical Banks */
       	 	if(value < 0) continue;
        
        	dword &= ~(1<<i);
        }
	
	pci_write_config32(ctrl->d0, 0x8c, dword);
	
	return dimm_mask;
}

static const uint16_t cas_latency_80[] = {
	/* For cas latency 2.0 0x01 works and until I see a large test sample
	 * I am not prepared to change this value, to the intel recommended value
	 * of 0x0d.  Eric Biederman
 	 */
 	/* The E7501 requires b1 rather than 01 for CAS2 or memory will be hosed
 	 * CAS 1.5 is claimed to be unsupported, will try to test that
 	 * will need to determine correct values for other CAS values
 	 * (perhaps b5, b1, b6?)
 	 * Steven James 02/06/2003
 	 */
 	 
//#	.byte 0x05, 0x01, 0x06 
//#	.byte 0xb5, 0xb1, 0xb6 
	0x0, 0x0bb1, 0x0662   /* RCVEN */ 
};
static const uint16_t cas_latency_80_4dimms[] = {
        0x0, 0x0bb1, 0x0882
};


static const uint8_t cas_latency_78[] = {
	DRT_CAS_1_5, DRT_CAS_2_0, DRT_CAS_2_5
};

static long spd_set_cas_latency(const struct mem_controller *ctrl, long dimm_mask) {
	/* Walk through all dimms and find the interesection of the
	 * supported cas latencies.
	 */
        int i;
        /* Initially allow cas latencies 2.5, 2.0
         * which the chipset supports.
         */
	uint32_t dword = (1<<3)| (1<<2);// esi
	uint32_t edi;
	uint32_t ecx;
	unsigned device;
	int value;
	uint8_t byte;
	uint16_t word;

	
        for(i = 0; i < DIMM_SOCKETS; i++) {
		if (!(dimm_mask & (1 << i))) {
                        continue;
                }
		value = spd_read_byte(ctrl->channel0[i], 18);
		if(value < 0) continue;
		/* Find the highest supported cas latency */
		ecx = log2(value & 0xff); 
		edi = (1<< ecx);

		 /* Remember the supported cas latencies */
        	ecx = (value & 0xff);

		/* Verify each cas latency at 133Mhz */
		/* Verify slowest/highest CAS latency */
		value = spd_read_byte(ctrl->channel0[i], 9);
		if(value < 0 ) continue;
		if(value > 0x75 ) {	
			/* The bus is too fast so we cannot support this case latency */
			ecx &= ~edi;
		}

		/* Verify the highest CAS latency - 0.5 clocks */
		edi >>= 1;
		if(edi != 0) {
			value = spd_read_byte(ctrl->channel0[i], 23);
			if(value < 0 ) continue;
			if(value > 0x75) {
				/* The bus is too fast so we cannot support this cas latency */
				ecx &= ~edi;
			}
		}

		/* Verify the highest CAS latency - 1.0 clocks */
		edi >>=1;
		if(edi !=0) {
                        value = spd_read_byte(ctrl->channel0[i], 25);
                        if(value < 0 ) continue;
                        if(value > 0x75) {
                                /* The bus is too fast so we cannot support this cas latency */
                                ecx &= ~edi;
                        }
		}

		/* Now find which cas latencies are supported for the bus */
		dword &= ecx;
	/* go to the next DIMM */
	}

	/* After all of the arduous calculation setup with the fastest
	 * cas latency I can use.
	 */
	value = __builtin_bsf(dword);  // bsrl = log2 how about bsfl?
	if(value ==0 ) return -1;
	ecx = value -1;

	byte = pci_read_config8(ctrl->d0, 0x78);
	byte &= ~(DRT_CAS_MASK);
	byte |= cas_latency_78[ecx];
	pci_write_config8(ctrl->d0,0x78, byte);

	/* set master DLL reset */
	dword = pci_read_config32(ctrl->d0, 0x88);
	dword |= (1<<26);
	
	/* the rest of the references are words */
//	ecx<<=1;  // don't need shift left, because we already define that in u16 array
	pci_write_config32(ctrl->d0, 0x88, dword);
	
	
	dword &= 0x0c0000ff;	/* patch try register 88 is undocumented tnz */
	dword |= 0xd2109800;

	pci_write_config32(ctrl->d0, 0x88, dword);
	
	word = pci_read_config16(ctrl->d0, 0x80);
	word &= ~(0x0fff);
	word |= cas_latency_80[ecx];

	dword = pci_read_config32(ctrl->d0, 0x70);

	if((dword & 0xff) !=0 ) {
		dword >>=8;
		if((dword & 0xff)!=0) {
			dword >>=8;
			if((dword & 0xff)!=0) {
				dword >>= 8;
				if( (dword & 0xff)!=0) {
        				word &=~(0x0fff);  /* we have dimms in all 4 slots */ 
        				word |=cas_latency_80_4dimms[ecx];
				}
			}
		}
	}
	
	pci_write_config16(ctrl->d0, 0x80, word);
	
	dword = pci_read_config32(ctrl->d0, 0x88);	/* reset master DLL reset */
	dword &= ~(1<<26);
	pci_write_config32(ctrl->d0, 0x88, dword);
	
	RAM_RESET_DDR_PTR(ctrl);

	return dimm_mask;

}

static long spd_set_dram_timing(const struct mem_controller *ctrl, long dimm_mask) {
	/* Walk through all dimms and find the interesection of the
	 * supported dram timings.
	 */

        int i;
        uint32_t dword;
        int value;

        /* Read the inititial state */
        dword = pci_read_config32(ctrl->d0, 0x78);
/*
# Intel clears top bit here, should we?
# No the default is on and for normal timming it should be on.  Tom Z
        andl    $0x7f, %esi
*/
        
        
        for(i = 0; i < DIMM_SOCKETS; i++) {
		if (!(dimm_mask & (1 << i))) {
                        continue;
                }
		/* Trp */
                value = spd_read_byte(ctrl->channel0[i], 27);
                if(value < 0) continue;
		if(value > (15<<2)) {
        		/* At 133Mhz if row precharge time is above than 15ns than we
         		 * need 3 clocks not 2 clocks.
         		*/
			dword &= ~(1<<0); 
		}
		/*  Trcd */
		value = spd_read_byte(ctrl->channel0[i],29);
		if(value < 0 ) continue;
		if(value > (15<<2)) {
		        /* At 133Mhz if the Minimum ras to cas delay is about 15ns we
        		 * need 3 clocks not 2 clocks.
         		*/
			dword &= ~((1<<3)|(1<<1));
		}
		/* Tras */
		value = spd_read_byte(ctrl->channel0[i],30);
		if(value < 0 ) continue;
		        /* Convert tRAS from ns to 133Mhz clock cycles */
		value <<=1;	 /* mult by 2 to make 7.5 15 */
		value  += 15;	/* Make certain we round up */
		value --;
		value &= 0xff;	/* Clear the upper bits of eax */
		value /= 15;
		
		/* Don't even process small timings */
		if(value >5) {
			uint32_t tmp;
			/* Die if the value is to large */
			if(value>7) {
				die ("unsupported_rcd\r\n");
			}
			/* Convert to clocks - 5 */
			value -=5;
			/* Convert the existing value into clocks - 5 */
			tmp = (~((dword>>9) & 3) - 1) & 3;
			/* See if we need a slower timing */
			if(value > tmp ) {
				/* O.k. put in our slower timing */
				dword &= ~(3<<9);
				dword |= ((~(value + 1)) & 3)<<9 ;
			}
		}
	
		/* Trd */ 
		/* Set to a 7 clock read delay. This is for 133Mhz
	 	*  with a CAS latency of 2.5  if 2.0 a 6 clock
	 	*  delay is good  */
		if( (pci_read_config8(ctrl->d0, 0x78) & 0x30) ==0 ){
			dword &= ~(7<<24); /* CAS latency is 2.5, make 7 clks */
		}

		/*
	 	* Back to Back Read Turn Around
	 	*/
		/* Set to a 3 clock back to back read turn around.  This
	 	 *  is good for CAS latencys 2.5 and 2.0 */
		dword |= (1<<27);
		/*
	 	* Back to Back Read-Write Turn Around
	 	*/
		/* Set to a 5 clock back to back read to write turn around.
	 	*  4 is a good delay if the CAS latency is 2.0 */
		if( ( pci_read_config8(ctrl->d0, 0x78) & (1<<4)) == 0) {
			dword &= ~(1<<28);
		}
		/*
	 	* Back to Back Write-Read Turn Around
	 	*/
		/* Set to a 2 clock back to back write to read turn around.
	 	*  This is good for 2.5 and 2.0 CAS Latencies. */
		dword |= (1<<29);
	}
	
	pci_write_config32(ctrl->d0, 0x78, dword);
	
	return dimm_mask;

}
static unsigned int spd_detect_dimms(const struct mem_controller *ctrl)
{               
        unsigned dimm_mask;
        int i;  
        dimm_mask = 0;  
#if DEBUG_RAM_CONFIG 
	print_debug("spd_detect_dimms:\r\n");
#endif
        for(i = 0; i < DIMM_SOCKETS; i++) {
                int byte;
                unsigned device;
#if DEBUG_RAM_CONFIG 
		print_debug_hex32(i);
		print_debug("\r\n");
#endif
                device = ctrl->channel0[i];
                if (device) {
                        byte = spd_read_byte(ctrl->channel0[i], 2);  /* Type */
                        if (byte == 7) {
                                dimm_mask |= (1 << i);
                        }
                }
#if 1
                device = ctrl->channel1[i];
                if (device) {
                        byte = spd_read_byte(ctrl->channel1[i], 2);
                        if (byte == 7) {
                                dimm_mask |= (1 << (i + DIMM_SOCKETS));
                        }
                }
#endif
        }       
#if 1
	i = (dimm_mask>>DIMM_SOCKETS);
	if(i != (dimm_mask & ( (1<<DIMM_SOCKETS) - 1) ) ) {
		die("now we only support dual channel\r\n");
	}

#endif

        return dimm_mask;
}               

static uint32_t set_dimm_size(const struct mem_controller *ctrl, struct dimm_size sz, uint32_t memsz, unsigned index)
{
        int i;
        uint32_t base0, base1;
        uint32_t dch;
	uint8_t byte;

        /* Double the size if we are using dual channel memory */
//        if (is_dual_channel(ctrl)) {
                /* Since I have 2 identical channels double the sizes */
                sz.side1++ ;
                sz.side2++;
//        }
              
	if (sz.side1 != sz.side2) {
                sz.side2 = 0;
        } 
 
        /* Make certain side1 of the dimm is at least 64MB */
        if (sz.side1 >= (25 + 4)) {
                memsz += (1 << (sz.side1 - (25 + 4)) ) ;
        }
	 /* Write the size of side 1 of the dimm */
	byte = memsz;
        pci_write_config8(ctrl->d0, 0x60+(index<<1), byte);

        /* Make certain side2 of the dimm is at least 64MB */
        if (sz.side2 >= (25 + 4)) {
                memsz += (1 << (sz.side2 - (25 + 4)) ) ;
        }
        
         /* Write the size of side 2 of the dimm */
	byte = memsz;
        pci_write_config8(ctrl->d0, 0x61+(index<<1), byte);
        
        /* now, fill in DRBs where no physical slot exists */
        
        for(i=index+1;i<4;i++) {
                pci_write_config8(ctrl->d0, 0x60+(i<<1),byte);
                pci_write_config8(ctrl->d0, 0x61+(i<<1),byte);
        
        }
        
        return memsz;

}
/* LAST_DRB_SLOT is a constant for any E7500 board */
#define LAST_DRB_SLOT 0x67

static long spd_set_ram_size(const struct mem_controller *ctrl, long dimm_mask)
{
        int i;
        uint32_t memsz=0;
	uint16_t word;

        for(i = 0; i < DIMM_SOCKETS; i++) {
                struct dimm_size sz;
                if (!(dimm_mask & (1 << i))) {
                        continue;
                }
                sz = spd_get_dimm_size(ctrl->channel0[i]);
#if DEBUG_RAM_CONFIG
                print_debug("dimm size =");
                print_debug_hex32(sz.side1);
                print_debug(" ");
                print_debug_hex32(sz.side2);
                print_debug("\r\n");
#endif

                if (sz.side1 == 0) {
			return -1; /* Report SPD error */
                }
                memsz = set_dimm_size(ctrl, sz, memsz, i);
        }
        /* For now hardset everything at 128MB boundaries */
        /* %ebp has the ram size in multiples of 64MB */
//        cmpl    $0, %ebp        /* test if there is no mem - smbus went bad */
//        jz      no_memory_bad_smbus
        if(memsz < 0x30)  {
        	/* I should really adjust all of this in C after I have resources
         	* to all of the pcie devices.
         	*/

        	/* Round up to 128M granularity */
        	memsz++;
        	memsz &= 0xfe;
        	memsz<<= 10;
		word = memsz;
        	pci_write_config16(ctrl->d0, 0xc4, word);
        } else {

        	/* FIXME will this work with 3.5G of ram? */
        	/* Put TOLM at 3G */
        	pci_write_config16(ctrl->d0, 0xc4, 0xc000);
        	/* Hard code a 1G remap window, right after the ram */
        	if(memsz< 0x40){
                	word = 0x40;  /* Ensure we are over 4G */
        	} else {
			word = memsz;
		}
        	pci_write_config16(ctrl->d0, 0xc6, word);
		word += 0x10;
        	pci_write_config16(ctrl->d0, 0xc8, word);

        }

        return dimm_mask;
}
                                                
static void sdram_set_spd_registers(const struct mem_controller *ctrl) {
	long dimm_mask;
#if DEBUG_RAM_CONFIG
	print_debug(spd_pre_init);
#endif
        //activate_spd_rom(ctrl);
        dimm_mask = spd_detect_dimms(ctrl);
	if (!(dimm_mask & ((1 << DIMM_SOCKETS) - 1))) {
                print_debug("No memory for this controller\n");
                return;
        }
	dimm_mask = spd_enable_clocks(ctrl, dimm_mask);
        if (dimm_mask < 0)
                goto hw_spd_err;
	//spd_verify_dimms(ctrl);
#if DEBUG_RAM_CONFIG
	print_debug(spd_pre_set);
#endif
	dimm_mask = spd_set_row_attributes(ctrl,dimm_mask);
        if (dimm_mask < 0)
                goto hw_spd_err;
	dimm_mask = spd_set_dram_controller_mode(ctrl,dimm_mask);
        if (dimm_mask < 0)
                goto hw_spd_err;	
	dimm_mask = spd_set_cas_latency(ctrl,dimm_mask);
        if (dimm_mask < 0)
                goto hw_spd_err;
	dimm_mask = spd_set_dram_timing(ctrl,dimm_mask);
        if (dimm_mask < 0)
                goto hw_spd_err;
#if DEBUG_RAM_CONFIG
	print_debug(spd_post_init);
#endif
	//moved from dram_post_init
	spd_set_ram_size(ctrl, dimm_mask);
	        return;
 hw_spd_err:
        /* Unrecoverable error reading SPD data */
        print_err("SPD error - reset\r\n");
        hard_reset();
        return;
}


#if 0
static void ram_postinit(const struct mem_controller *ctrl) {
#if DEBUG_RAM_CONFIG 
	dumpnorth();
#endif
	/* Include a test to verify that memory is more or less working o.k. 
  	 * This test is to catch programming errors and hardware that is out of
	 * spec, not a test to see if the memory dimms are working 100%
	 */
//#	CALL_LABEL(verify_ram)
	spd_set_ram_size(ctrl);
}
#define FIRST_NORMAL_REFERENCE() CALL_LABEL(ram_postinit)

#define SPECIAL_FINISHUP()   CALL_LABEL(dram_finish)

#endif

#define ecc_pre_init	"Initializing ECC state...\r\n"
#define ecc_post_init	"ECC state initialized.\r\n"
static void dram_finish(const struct mem_controller *ctrl)
{
	uint32_t dword;
	uint8_t byte;
	/* Test to see if ECC support is enabled */
	dword = pci_read_config32(ctrl->d0, 0x7c);
	dword >>=20;
	dword &=3;
	if(dword == 2)  {
		
#if DEBUG_RAM_CONFIG 	
		print_debug(ecc_pre_init);
#endif
		/* Initialize ECC bits , use ECC zero mode (new to 7501)*/
		pci_write_config8(ctrl->d0, 0x52, 0x06);
		pci_write_config8(ctrl->d0, 0x52, 0x07);
		do {
			byte = pci_read_config8(ctrl->d0, 0x52);

		} while ( (byte & 0x08 ) == 0);

		pci_write_config8(ctrl->d0, 0x52, byte & 0xfc);
#if DEBUG_RAM_CONFIG 	
		print_debug(ecc_post_init);	
#endif

		/* Clear the ECC error bits */
		pci_write_config8(ctrl->d0f1, 0x80, 0x03); /* dev 0, function 1, offset 80 */
		pci_write_config8(ctrl->d0f1, 0x82, 0x03); /* dev 0, function 1, offset 82 */

		pci_write_config32(ctrl->d0f1, 0x40, 1<<18); /* clear dev 0, function 1, offset 40; bit 18 by writing a 1 to it */
  	        pci_write_config32(ctrl->d0f1, 0x44, 1<<18); /* clear dev 0, function 1, offset 44; bit 18 by writing a 1 to it */

		pci_write_config8(ctrl->d0, 0x52, 0x0d);
	}
	
	dword = pci_read_config32(ctrl->d0, 0x7c); /* FCS_EN */
	dword |= (1<<17);
	pci_write_config32(ctrl->d0, 0x7c, dword);


#if DEBUG_RAM_CONFIG >= 2
	dumpnorth();
#endif

//	verify_ram();
}

#if ASM_CONSOLE_LOGLEVEL > BIOS_DEBUG
#define ram_enable_1    "Ram Enable 1\r\n"
#define ram_enable_2	"Ram Enable 2\r\n"
#define ram_enable_3 	"Ram Enable 3\r\n"
#define ram_enable_4 	"Ram Enable 4\r\n"
#define ram_enable_5 	"Ram Enable 5\r\n"
#define ram_enable_6 	"Ram Enable 6\r\n"
#define ram_enable_7 	"Ram Enable 7\r\n"
#define ram_enable_8 	"Ram Enable 8\r\n"
#define ram_enable_9 	"Ram Enable 9\r\n"
#define ram_enable_10 	"Ram Enable 10\r\n"
#define ram_enable_11 	"Ram Enable 11\r\n"
#endif

	/* Estimate that SLOW_DOWN_IO takes about 50&76us*/
	/* delay for 200us */

#if 1
static void do_delay(void)
{
	int i;
	for(i = 0; i < 16; i++) { SLOW_DOWN_IO }
}
#define DO_DELAY do_delay();
#else
#define DO_DELAY \
	udelay(200);
#endif		

#define EXTRA_DELAY DO_DELAY

static void sdram_enable(int controllers, const struct mem_controller *ctrl)
{
	int i;
	/* 1 & 2 Power up and start clocks */
#if DEBUG_RAM_CONFIG 
	print_debug(ram_enable_1);
	print_debug(ram_enable_2);
#endif

	/* A 200us delay is needed */

	DO_DELAY
	EXTRA_DELAY

	/* 3. Apply NOP */
#if DEBUG_RAM_CONFIG 
	print_debug(ram_enable_3);
#endif
	RAM_NOP(ctrl);
	EXTRA_DELAY

	/* 4 Precharge all */
#if DEBUG_RAM_CONFIG 
	print_debug(ram_enable_4);
#endif
	RAM_PRECHARGE(ctrl);
	EXTRA_DELAY
	
	/* wait until the all banks idle state... */
	/* 5. Issue EMRS to enable DLL */
#if DEBUG_RAM_CONFIG 
	print_debug(ram_enable_5);
#endif
	RAM_EMRS(ctrl);
	EXTRA_DELAY
	
	/* 6. Reset DLL */
#if DEBUG_RAM_CONFIG 
	print_debug(ram_enable_6);
#endif
	RAM_MRS(ctrl,1);
	EXTRA_DELAY

	/* Ensure a 200us delay between the DLL reset in step 6 and the final
	 * mode register set in step 9.
	 * Infineon needs this before any other command is sent to the ram.
	 */
	DO_DELAY
	EXTRA_DELAY
	
	/* 7 Precharge all */
#if DEBUG_RAM_CONFIG 
	print_debug(ram_enable_7);
#endif
	RAM_PRECHARGE(ctrl);
	EXTRA_DELAY
	
	/* 8 Now we need 2 AUTO REFRESH / CBR cycles to be performed */
#if DEBUG_RAM_CONFIG 
	print_debug(ram_enable_8);
#endif
	RAM_CBR(ctrl);
	EXTRA_DELAY
	RAM_CBR(ctrl);
	EXTRA_DELAY
	/* And for good luck 6 more CBRs */
	RAM_CBR(ctrl);
	EXTRA_DELAY
	RAM_CBR(ctrl);
	EXTRA_DELAY
	RAM_CBR(ctrl);
	EXTRA_DELAY
	RAM_CBR(ctrl);
	EXTRA_DELAY
	RAM_CBR(ctrl);
	EXTRA_DELAY
	RAM_CBR(ctrl);
	EXTRA_DELAY

	/* 9 mode register set */
#if DEBUG_RAM_CONFIG 
	print_debug(ram_enable_9);
#endif
	RAM_MRS(ctrl,0);
	EXTRA_DELAY
	
	/* 10 DDR Receive FIFO RE-Sync */
#if DEBUG_RAM_CONFIG 
	print_debug(ram_enable_10);
#endif
	RAM_RESET_DDR_PTR(ctrl);
	EXTRA_DELAY
	
	/* 11 normal operation */
#if DEBUG_RAM_CONFIG 
	print_debug(ram_enable_11);
#endif
	RAM_NORMAL(ctrl);


	// special from v1
        //FIRST_NORMAL_REFERENCE();
	//spd_set_ram_size(ctrl, 0x03);

        /* Finally enable refresh */
        ENABLE_REFRESH(ctrl);

	//SPECIAL_FINISHUP();
	dram_finish(ctrl);

}

