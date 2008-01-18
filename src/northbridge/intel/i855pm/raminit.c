/* This was originally for the e7500, modified for i855pm
 */

/* the 855pm uses only 
 * memory type (must be ddr)
 * number of row addresses, not counting bank addresses
 * number of column addresses
 * number of so-dimm banks
 * ecc, no ecc
 * refresh rate/type
 * number banks on each device
 * 
 * that's it. No other bytes are used. 
 * these are bytes
 * 2, 3, 4, 5, 11, 12 17
 */

/* converted to C 6/2004 yhlu */

#define DEBUG_RAM_CONFIG 2
#undef ASM_CONSOLE_LOGLEVEL
#define ASM_CONSOLE_LOGLEVEL 8
#define dumpnorth() dump_pci_device(PCI_DEV(0, 0, 1)) 

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
/*#define CAS_LATENCY  CAS_2_5*/
/*#define CAS_LATENCY  CAS_1_5*/

/* WOW! this could be bad! sets casl to 2 without checking! */
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
#if DEBUG_RAM_CONFIG >=2
        print_debug("P:");
        print_debug_hex8(value);
        print_debug("\r\n");
#endif
        /* %ecx - initial address to read from */
        /* Compute the offset */
        dword = value >> 16;
	/*        for(i=0;i<4;i++) {*/
        for(i=0;i<1;i++) {
                /* Set the ram command */
                byte = pci_read_config8(ctrl->d0, 0x70);
                byte &= 0x8f;
                byte |= (uint8_t)(value & 0xff);
#if DEBUG_RAM_CONFIG  
                print_debug("R:");
                print_debug_hex8(byte);
                print_debug("\r\n");
#endif

                pci_write_config8(ctrl->d0, 0x70, byte);

                /* Assert the command to the memory */
#if DEBUG_RAM_CONFIG  >= 2
                print_debug("R:");
                print_debug_hex32(dword);
                print_debug("\r\n");
#endif

		result = read32(dword);
		
#if DEBUG_RAM_CONFIG
		print_debug("Done\r\n");
#endif
                /* Go to the next base address */
                dword += 0x0200000;

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
	byte = pci_read_config8(ctrl->d0, 0x60); 
	/* Transform it into the form expected by SDRAM */
	dword = ram_cas_latency[(byte>>5) & 1];
#warning RAM_MRS -- using BROKEN hard-wired CAS 2.0. FIX ME SOON
/*
 */
	value  |= (dword<<(16+MD_SHIFT));
	
	value |= (MODE_NORM | BURST_TYPE | BURST_LENGTH) << (16+MD_SHIFT);

	do_ram_command(ctrl, value);
}

#define RAM_MRS(ctrl, dll_reset) ram_mrs( ctrl, (dll_reset << (8+MD_SHIFT+ 16)) | ((RAM_COMMAND_MRS <<4)& 0x70) )

static void RAM_NORMAL(const struct mem_controller *ctrl) {
	uint8_t byte;
	byte = pci_read_config8(ctrl->d0, 0x70);
	byte &=  0x8f;
	byte |= (RAM_COMMAND_NORMAL << 4);
	pci_write_config8(ctrl->d0, 0x70, byte);
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
	dword = pci_read_config32(ctrl->d0, 0x70);
	dword |= (1 << 29);
	pci_write_config32(ctrl->d0, 0x70, dword);
}

	/*
	 * Table:	constant_register_values
	 */
static const long register_values[] = {
	/* DRB - DRAM Row Boundary Registers
	 * 0x60 - 0x63
	 *     An array of 8 byte registers, which hold the ending
	 *     memory address assigned  to each pair of DIMMS, in 32MB 
	 *     granularity.   
	 */
	/* Conservatively say each row has 32MB of ram, we will fix this up later */
	0x40, 0x00000000, (0x01 << 0) | (0x02 << 8) | (0x03 << 16) | (0x04 << 24),
	/* DRA - DRAM Row Attribute Register 
	 * 0x70 Row 0,1
	 * 0x71 Row 2,3
	 * [2:0] Row Attributes for both rows
	 *       001 == 2KB
	 *       010 == 4KB
	 *       011 == 8KB
	 *       100 == 16KB
	 *       Others == Reserved
	 */
	/* leave it alone for now -- seems bad to set it at all 
	0x70, 0x00000000, 
		(((0<<3)|(0<<0))<< 0) | 
		(((0<<3)|(0<<0))<< 4) | 
		(((0<<3)|(0<<0))<< 8) | 
		(((0<<3)|(0<<0))<<12) | 
		(((0<<3)|(0<<0))<<16) | 
		(((0<<3)|(0<<0))<<20) | 
		(((0<<3)|(0<<0))<<24) | 
		(((0<<3)|(0<<0))<<28),
	*/
	/* DRT - DRAM Time Register
	 * 0x60
	 * [31:31] tWTR -- MBZ
	 * [30:30] tWR 0 is 2, 1 is 3 clocks
	 * [29:28] back to back write-read commands spacing
	 *         different rows
	 *         00 4, 01 3, 10 2, 11 reserved
	 *         
	 * [27:26] same or different
	 *         CL + .5x BL + TA(RD-WR) - DQSS
	 *         wow that's hard! 
	 *         00 7, 01 6, 10 5, 11 4
	 *         00 4, 01 3, 10 2, 11 reserved
	 *         
	 * [25:25] Back to Back Read-read spacing
	 *         .5xBL + TA(RD-RD)
	 *         0 4 , 1 3  
	 *         
	 * [24:15] Reserved
	 *
	 * [14:12] Refresh cycle time
	 * 000 14, 001 13, 010 12, 011 11, 100 10, 101 9, 110 8, 111 7
	 *
	 * [11:11] tRAS, max
	 *         0 120 us, 1 reserved
	 *         
	 * [10:09] Active to Precharge (tRAS)
	 *         00 == 8 clocks
	 *         01 == 7 clocks
	 *         10 == 6 clocks
	 *         11 == 5 clocks
	 * [08:07] Reserved
	 * [06:05] Cas Latency (tCL)
	 *         00 == 2.5 Clocks
	 *         01 == 2.0 Clocks (default)
	 *         10 == Reserved
	 *         11 == Reserved
	 * [04:04] Reserved
	 * [03:02] Ras# to Cas# Delay (tRCD)
	 *         00 == 4 DRAM Clocks
	 *         01 == 3 DRAM Clocks
	 *         10 == 2 DRAM Clocks
	 *         11 == reserved
	 * [01:00] DRAM RAS# to Precharge (tRP)
	 *         00 == 4 DRAM Clocks
	 *         01 == 3 DRAM Clocks
	 *         10 == 2 DRAM Clocks
	 *         11 == reserved
	 */

#define DRT_CAS_2_5 (0<<5)
#define DRT_CAS_2_0 (1<<5)   
#define DRT_CAS_MASK (3<<5)

#if CAS_LATENCY == CAS_2_5
#define DRT_CL DRT_CAS_2_5
#elif CAS_LATENCY == CAS_2_0
#define DRT_CL DRT_CAS_2_0
#endif

	/* bios is 0x2a004425 */
	/* default hardware is 18004425 */
	/* no setting for now */

	/* FDHC - Fixed DRAM Hole Control
	 * 0x97
	 * [7:7] Hole_Enable
	 *       0 == No memory Hole
	 *       1 == Memory Hole from 15MB to 16MB
	 * [6:0] Reserved
	 *
	 * PAM - Programmable Attribute Map
	 * 0x90 [3:0] Reserved
	 * 0x90 [5:4] 0xF0000 - 0xFFFFF
	 * 0x91 [1:0] 0xC0000 - 0xC3FFF
	 * 0x91 [5:4] 0xC4000 - 0xC7FFF
	 * 0x92 [1:0] 0xC8000 - 0xCBFFF
	 * 0x92 [5:4] 0xCC000 - 0xCFFFF
	 * 0x93 [1:0] 0xD0000 - 0xD3FFF
	 * 0x93 [5:4] 0xD4000 - 0xD7FFF
	 * 0x94 [1:0] 0xD8000 - 0xDBFFF
	 * 0x94 [5:4] 0xDC000 - 0xDFFFF
	 * 0x95 [1:0] 0xE0000 - 0xE3FFF
	 * 0x95 [5:4] 0xE4000 - 0xE7FFF
	 * 0x96 [1:0] 0xE8000 - 0xEBFFF
	 * 0x96 [5:4] 0xEC000 - 0xEFFFF
	 *       00 == DRAM Disabled (All Access go to memory mapped I/O space)
	 *       01 == Read Only (Reads to DRAM, Writes to memory mapped I/O space)
	 *       10 == Write Only (Writes to DRAM, Reads to memory mapped I/O space)
	 *       11 == Normal (All Access go to DRAM)
	 */
	/*	0x90, 0xcccccf7f, (0x00 << 0) | (0x30 << 8) | (0x33 << 16) | (0x33 << 24),*/
	/*0x94, 0xcccccccc, (0x33 << 0) | (0x33 << 8) | (0x33 << 16) | (0x33 << 24),*/


	/* FIXME why was I attempting to set a reserved bit? */
	/* 0x0100040f */

	/* DRC - DRAM Contoller Mode Register
	 * 0x7c
	 * [31:30] Rev #
	 * [29:29] Initialization Complete
	 *         0 == Not Complete
	 *         1 == Complete
	 * [28:27] Dynamic Power Down Enable (leave at 0 for now)
	 * [27:24] Reserved
	 * [23:23] Reduced Comamnd Drive Delay (leave at 0 for now)
	 * [22:22] Reduced Command Drive Enable (leave at 0 for now)
	 * [21:21] DRAM Data Integrity Mode
	 *         0 == Disabled, no ECC
	 *         1 == Error checking, with correction
	 * [20:20] Reserved
	 * [19:18] Reserved
	 *         Must equal 00
	 * [17:17] (Intel Undocumented)	should always be set to 
	 * [16:16] Disable SCK Tri-state in C3/S1-m 
	 *         0 == 2n Rule
	 *         1 == 1n rule
	 * [15:14] Reserved
	 * [13:13] Dynamic CS Disable
	 * [12:12] SM Interface Tristate enable
	 * [11:11] Reserved
	 * [10:08] Refresh mode select
	 *         000 == Refresh disabled
	 *         001 == Refresh interval 15.6 usec
	 *         010 == Refresh interval 7.8 usec
	 *         011 == Refresh interval 64 usec
	 *         111 == Reserved
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
	 * [03:01] Reserved
	 * [00:00] DRAM type --hardwired to 1 to indicate DDR
	 */
	0x70, 0xdf0f6c7f, 0,
	/* undocumnted shit */
	0x80, 0, 0xaf0031,

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

/*#define SLOW_DOWN_IO inb(0x80);*/
#define SLOW_DOWN_IO udelay(40);


static void ram_set_d0f0_regs(const struct mem_controller *ctrl) {
#if DEBUG_RAM_CONFIG
	//dumpnorth();
#endif
	int i;
	int max;
        max = sizeof(register_values)/sizeof(register_values[0]);
        for(i = 0; i < max; i += 3) {
                uint32_t reg;
#if DEBUG_RAM_CONFIG >=2
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
#if DEBUG_RAM_CONFIG
	dumpnorth();
#endif
}
static void sdram_set_registers(const struct mem_controller *ctrl){
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
	/* the hell with that! just use byte 31 -- rgm */
        value = spd_read_byte(device, 31); /* size * 4 MB */
	value = log2(value);
	/* this is in 4 MB chunks, or 32 MBits chunks. 
	 * log base 2 of 32 Mbits is log2 of (32*1024*1024) is 25
	 * so add 25 
	 */
	value += 25;
        sz.side1 = value;
        sz.side2 = 0;
#if DEBUG_RAM_CONFIG
	print_debug("returned size log 2 in bits is :");
	print_debug_hex32(value);
	print_debug("\r\n");
#endif
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
        uint16_t word=0x7777;
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
#if DEBUG_RAM_CONFIG
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

			/* Convert to the format needed for the DRA register */
			/* subtract 3 (there are 8 bytes)
			 * then subtract 11
			 * (since 12 bit size should map to a value of 1)
			 * so subtract 14 total
			 */
			sz.side1-=14;	

			/* Place in the %ebp the dra place holder */ /*i*/
			word &= ~(7<<i);
			word |= sz.side1<<(i<<3);
			
			/* Test to see if the second side is present */

	                if( sz.side2 !=0) {
        
	                        /* Test for a valid dimm width */
        	                if((sz.side2 <15) || (sz.side2>18) ) {
                	                print_err("unsupported page size\r\n");
                        	}

                        	/* Convert to the format needed for the DRA register */
                        	sz.side2-=14;

	                        /* Place in the %ebp the dra place holder */ /*i*/
				word &= ~(7<<i);
        	                word |= sz.side2<<((i<<3) + 4 );

			}
		}
	/* go to the next DIMM */
	}

	/* Write the new row attributes register */
	pci_write_config32(ctrl->d0, 0x50, word);

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
	 * 125us    -> 15.6us
	 */
	1, 0xff, 2, 1, 1, 1
};
#define MAX_SPD_REFRESH_RATE 5

static long spd_set_dram_controller_mode (const struct mem_controller *ctrl, long dimm_mask) {

        int i;  
        uint32_t dword;
        int value;
	uint32_t ecx;
	uint32_t edx;
	/* on this chipset we only do refresh "slow" or "fast" for now */
	/* we start out assuming "slow" (15.6 microseconds) */
	uint32_t refrate = 1; /* better than 7.8 */
        
        /* Read the inititial state */
        dword = pci_read_config32(ctrl->d0, 0x70);
	/* WTF? */
	/*dword |= 0x10000;*/
#if 0 /* DEBUG_RAM_CONFIG*/
	print_debug("spd_detect_dimms: 0x70.l is:");
	print_debug_hex32(dword);
	print_debug("\r\n");
#endif

#if 0
        /* Test if ECC cmos option is enabled */
        /* Clear the ecc enable */
1:
#endif


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
#if DEBUG_RAM_CONFIG
	print_debug("spd_detect_dimms:\r\n");
#endif
			/* Clear the ecc enable */
			dword &= ~(3 << 20);
#if 0 &&DEBUG_RAM_CONFIG
	print_debug("spd_detect_dimms: no ecc so set:");
	print_debug_hex32(dword);
	print_debug("\r\n");
#endif

		}
		value = spd_read_byte(ctrl->channel0[i], 12);  /* SDRAM refresh rate */
		if(value < 0 ) continue;
		value &= 0x7f;
		if(value > MAX_SPD_REFRESH_RATE) { print_err("unsupported refresh rate\r\n");}
/*		if(value == 0xff) { print_err("unsupported refresh rate\r\n");}*/
		
#if DEBUG_RAM_CONFIG
	print_debug("spd_detect_dimms: ref rate index:");
	print_debug_hex8(value);
	print_debug("\r\n");
#endif
	if (value == 2) /* have to go faster */
	  refrate = 2;
#if 0 &&DEBUG_RAM_CONFIG
	print_debug("spd_detect_dimms: dword is now w/refresh:");
	print_debug_hex32(dword);
	print_debug("\r\n");
#endif
		/* no applicability here but there are similar things
		 * we'll try later. 
		 */
#if 0
		value = spd_read_byte(ctrl->channel0[i], 33); /* Address and command hold time after clock */
		if(value < 0) continue;
		if(value >= 0xa0) { 		/* At 133Mhz this constant should be 0x75 */
			dword &= ~(1<<16);	/* Use two clock cyles instead of one */
		}
#endif
	
	/* go to the next DIMM */
	}

	/* set the refrate now */
	dword |= (refrate << 7);
	/* Now write the controller mode */
	pci_write_config32(ctrl->d0, 0x70, dword);
	
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
  Intel clears top bit here, should we?
  No the default is on and for normal timming it should be on.  Tom Z
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
 	 
/*#	.byte 0x05, 0x01, 0x06 */
/*#	.byte 0xb5, 0xb1, 0xb6 */
	0x0, 0x0bb1, 0x0662   /* RCVEN */ 
};
static const uint16_t cas_latency_80_4dimms[] = {
        0x0, 0x0bb1, 0x0882
};


static const uint8_t cas_latency_78[] = {
	DRT_CAS_2_0, DRT_CAS_2_5
};

static long spd_set_cas_latency(const struct mem_controller *ctrl, long dimm_mask) {
	/* Walk through all dimms and find the interesection of the
	 * supported cas latencies.
	 */
        int i;
        /* Initially allow cas latencies 2.5, 2.0
         * which the chipset supports.
         */
	uint32_t dword = (1<<3)| (1<<2);/* esi*/
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
	value = __builtin_bsf(dword);  /* bsrl = log2 how about bsfl?*/
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
/*	ecx<<=1;  // don't need shift left, because we already define that in u16 array*/
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

static const unsigned int bustimings[8] = {
  /* first four are for GM part */
  266, 200, 200, -1,
  /* and then the GME part */
  266, 200, 200, 333
};

static long spd_set_dram_timing(const struct mem_controller *ctrl, long dimm_mask) {
	/* Walk through all dimms and find the interesection of the
	 * supported dram timings.
	 */

        int i;
        uint32_t dword;
        int value;

	/* well, shit. Intel has not seen fit to document the observed
	 * setting for these bits. On my chipset it is 3 right now, and
	 * that's not in the table documented for this chip. 
	 * What a shame. We will assume 133 mhz I guess? not sure. 
	 * also they say in one place it is two bits and in another
	 * they say it is 3 bits! we'll assume two bits, that's
	 * the only one that makes sense. 
	 */
	uint32_t rambusfrequency;
	uint32_t ramindex;

	/* what kind of chip is it? */
	/* only bit that really matters is high order bit ... */
	/* here is a problem with the memory controller struct ...
	 * ram control is spread across d0/d1 on this part!
	 */
	ramindex = pci_read_config8(PCI_DEV(0,0,0), 0x44);
	ramindex >>= 5;

	/* compute the index into the table. Use the type of chip
	 * as the high order bit and the 0:0.3:c0 & 7 as the low 
	 * order four bits. 
	 */
	
	ramindex |= pci_read_config8(PCI_DEV(0,0,3), 0xc0) & 7;
	/* we now have an index into the clock rate table ... */
	
	rambusfrequency = bustimings[ramindex];

        /* Read the inititial state */
        dword = pci_read_config32(ctrl->d0, 0x60);
#if DEBUG_RAM_CONFIG >= 10
	print_debug("spd_detect_dimms: bus timing index: ");
	print_debug_hex32(ramindex);
	print_debug(" and speed ");
	print_debug_hex32(rambusfrequency);
	print_debug("\r\n");
#endif

	/* for now, since we are on deadline, set to "known good" and 
	 * fix later. 
	 */
	pci_write_config32(ctrl->d0, 0x60, 0x2a004425);
	return dimm_mask;

/*
  Intel clears top bit here, should we?
  No the default is on and for normal timming it should be on.  Tom Z
        andl    $0x7f, %esi
 */
        

/* HERE. WHat's the frequency kenneth?        */
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
	
	pci_write_config32(ctrl->d0, 0x60, dword);
	
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
        }       

        return dimm_mask;
}               

static uint32_t set_dimm_size(const struct mem_controller *ctrl, struct dimm_size sz, uint32_t memsz, unsigned index)
{
        int i;
        uint32_t base0, base1;
        uint32_t dch;
	uint8_t byte;

	/* I think size2 is always 0 ... */
        /* Double the size if we are using dual channel memory */
	if (sz.side1 != sz.side2) {
                sz.side2 = 0;
        } 


        /* Make certain side1 of the dimm is at least 32MB */
	/* This 28 is weird. 
	 * sz.size1 is log2 size in bits. 
	 * so what's 28? So think of it as this: 
	 * in log2 space: 10 + 10 + 8, i.e. 1024 * 1024 * 256 or
	 * 256 Mbits, or 32 Mbytes. 
	 */
	/* this is from the e7500 code and it's just wrong for small dimes (< 64 MB)
	 * However, realistically, this case will never happen! the dimms are all bigger ...
	 * so skip the conditional completely. 
	 *  if (sz.side1 >= (28)) { }
	 */
	memsz += (1 << (sz.side1 - (28)) ) ;

	 /* Write the size of side 1 of the dimm */
 #if DEBUG_RAM_CONFIG
	print_debug("Write size ");
	print_debug_hex8(memsz);
	print_debug(" to ");
	print_debug_hex8(0x40 + index);
	print_debug("\r\n");
#endif
	byte = memsz;
        pci_write_config8(ctrl->d0, 0x40+index, byte);

        /* now, fill in DRBs where no physical slot exists */
        
        for(i=index+1;i<4;i++) {
                pci_write_config8(ctrl->d0, 0x40+i, byte);
        }
        
        return memsz;

}

#define LAST_DRB_SLOT 0x43

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
#if 0 
        /* For now hardset everything at 128MB boundaries */
        /* %ebp has the ram size in multiples of 64MB */
/*        cmpl    $0, %ebp        /* test if there is no mem - smbus went bad */*/
/*        jz      no_memory_bad_smbus*/
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
#endif

        return dimm_mask;
}
                                                
static void sdram_set_spd_registers(const struct mem_controller *ctrl) {
	long dimm_mask;
#if DEBUG_RAM_CONFIG
	print_debug(spd_pre_init);
#endif
        /*activate_spd_rom(ctrl);*/
        dimm_mask = spd_detect_dimms(ctrl);
	if (!(dimm_mask & ((1 << DIMM_SOCKETS) - 1))) {
                print_debug("No memory for this controller\n");
                return;
        }
	dimm_mask = spd_enable_clocks(ctrl, dimm_mask);
        if (dimm_mask < 0)
                goto hw_spd_err;
	/*spd_verify_dimms(ctrl);*/
#if DEBUG_RAM_CONFIG
	print_debug(spd_pre_set);
#endif
	dimm_mask = spd_set_row_attributes(ctrl,dimm_mask);

        if (dimm_mask < 0)
                goto hw_spd_err;
	dimm_mask = spd_set_dram_controller_mode(ctrl,dimm_mask);

        if (dimm_mask < 0)
                goto hw_spd_err;	

	/* skip for now until we just get "known good" up
	dimm_mask = spd_set_cas_latency(ctrl,dimm_mask);
	*/

        if (dimm_mask < 0)
                goto hw_spd_err;
	dimm_mask = spd_set_dram_timing(ctrl,dimm_mask);
        if (dimm_mask < 0)
                goto hw_spd_err;

#if DEBUG_RAM_CONFIG
	print_debug(spd_post_init);
#endif
	/*moved from dram_post_init*/
	spd_set_ram_size(ctrl, dimm_mask);
	dump_pci_device(PCI_DEV(0,0,1));
	return;

 hw_spd_err:
        /* Unrecoverable error reading SPD data */
        print_err("SPD error - reset\r\n");
        hard_reset();
        return;
}


	/* I have finally seen ram bad enough to cause coreboot
	 * to die in mysterious ways, before booting up far
	 * enough to run a memory tester.  This code attempts
	 * to catch this blatantly bad ram, with a spot check.
	 * For most cases you should boot all of the way up 
	 * and run a memory tester.  
	 */
	/* Ensure I read/write each stick of bank of memory &&
	 * that I do more than 1000 bytes to avoid the northbridge cache.
	 * Only 64M of each side of each DIMM is currently mapped,
	 * so we can handle > 4GB of ram here.
	 */

static void ram_postinit(const struct mem_controller *ctrl) {
#if DEBUG_RAM_CONFIG 
	dumpnorth();
#endif
	/*spd_set_ram_size(ctrl);*/
}
#define FIRST_NORMAL_REFERENCE() CALL_LABEL(ram_postinit)

#define SPECIAL_FINISHUP()   CALL_LABEL(dram_finish)


#define ecc_pre_init	"Initializing ECC state...\r\n"
#define ecc_post_init	"ECC state initialized.\r\n"
static void dram_finish(const struct mem_controller *ctrl)
{
	uint32_t dword;
	uint8_t byte;
	/* Test to see if ECC support is enabled */
	dword = pci_read_config32(ctrl->d0, 0x70);
	dword >>=20;
	dword &=1;
	if(dword == 1)  {
		
#if DEBUG_RAM_CONFIG	
//		print_debug(ecc_pre_init);
#endif
#if DEBUG_RAM_CONFIG		
//		print_debug(ecc_post_init);	
#endif
#if 0
		/* Clear the ECC error bits */
	dword = pci_read_config32(ctrl->d0, 0x7c); /* FCS_EN */
	dword |= (1<<17);
	pci_write_config32(ctrl->d0, 0x7c, dword);
#endif
	  }

#if DEBUG_RAM_CONFIG 
//	dumpnorth();
#endif

/*	verify_ram(); */
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
void udelay(int usecs)
{
        int i;
        for(i = 0; i < usecs; i++)
                outb(i&0xff, 0x80);
}


#if 0
static void DO_DELAY(void){
	udelay(200);
}
#endif

#define DO_DELAY udelay(200);
/*
	for(i=0; i<16;i++)  { 	SLOW_DOWN_IO	}
 */
		

#define EXTRA_DELAY DO_DELAY

static void sdram_enable(int controllers, const struct mem_controller *ctrl)
{
	int i;
	uint32_t mchtst;
	/* 1 & 2 Power up and start clocks */
	/* let the games begin. More undocumented shit, so we'll just set it
	 * as intel sets it
	 */
	mchtst = pci_read_config32(ctrl->d0, 0x68);
#if DEBUG_RAM_CONFIG 
	print_debug(ram_enable_1);
	print_debug_hex32(mchtst);
	dumpnorth();
#endif	
	/*
	  mchtst = 0x10f10038;
	  pci_write_config32(ctrl->d0, 0x68, mchtst);
	  * can't find a ram power register ...
	*/

#if DEBUG_RAM_CONFIG 

	print_debug(ram_enable_2);
#endif

	/* A 200us delay is needed */

	DO_DELAY
	EXTRA_DELAY

	/* 3. Apply NOP */
#if DEBUG_RAM_CONFIG 
	  dump_pci_device(PCI_DEV(0, 0, 0)) ;
	print_debug(ram_enable_3);
#endif
	RAM_NOP(ctrl);
	EXTRA_DELAY
#undef DEBUG_RAM_CONFIG
#define DEBUG_RAM_CONFIG 0
	/* 4 Precharge all */
#if DEBUG_RAM_CONFIG 
	print_debug(ram_enable_4);
#endif
#undef DEBUG_RAM_CONFIG
#define DEBUG_RAM_CONFIG 0
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


	/* special from v1*/
        /*FIRST_NORMAL_REFERENCE();*/
	/*spd_set_ram_size(ctrl, 0x03);*/

        /* Finally enable refresh */
        ENABLE_REFRESH(ctrl);

	/*SPECIAL_FINISHUP();*/
	dram_finish(ctrl);
	{ char *c = (char *) 0;
	*c = 'a';
	print_debug("Test: ");
	print_debug_hex8(*c);
	print_debug("\r\n");
	}
	

}

