#include <cpu/p6/mtrr.h>
#include "raminit.h"

/*
This software and ancillary information (herein called SOFTWARE )
called LinuxBIOS          is made available under the terms described
here.  The SOFTWARE has been approved for release with associated
LA-CC Number 00-34   .  Unless otherwise indicated, this SOFTWARE has
been authored by an employee or employees of the University of
California, operator of the Los Alamos National Laboratory under
Contract No. W-7405-ENG-36 with the U.S. Department of Energy.  The
U.S. Government has rights to use, reproduce, and distribute this
SOFTWARE.  The public may copy, distribute, prepare derivative works
and publicly display this SOFTWARE without charge, provided that this
Notice and any statement of authorship are reproduced on all copies.
Neither the Government nor the University makes any warranty, express 
or implied, or assumes any liability or responsibility for the use of
this SOFTWARE.  If SOFTWARE is modified to produce derivative works,
such modified SOFTWARE should be clearly marked, so as not to confuse
it with the version available from LANL.
 */
/* Copyright 2000, Ron Minnich, Advanced Computing Lab, LANL
 * rminnich@lanl.gov
 */
/*
 * 11/26/02 - kevinh@ispiri.com - The existing comments implied that
 * this didn't work yet.  Therefore, I've updated it so that it works
 * correctly - at least on my VIA epia motherboard.  64MB DIMM in slot 0.
 */

/* Added automatic detection of first equipped bank and its MA mapping type.
 * (Rest of configuration is done in C)
 * 5/19/03 by SONE Takeshi <ts1@tsn.or.jp>
 */
/* converted to C 9/2003 Ron Minnich */

/* Set to 1 if your DIMMs are PC133 Note that I'm assuming CPU's FSB
 * frequency is 133MHz. If your CPU runs at another bus speed, you
 * might need to change some of register values.
 */
#ifndef DIMM_PC133
#define DIMM_PC133 0
#endif

// Set to 1 if your DIMMs are CL=2
#ifndef DIMM_CL2
#define DIMM_CL2 0
#endif

void dimms_read(unsigned long x) {
  uint8_t c;
  unsigned long eax; 
  volatile unsigned long y;
  eax =  x;
  for(c = 0; c < 6; c++) {
    
    print_err("dimms_read: ");
    print_err_hex32(eax);
    print_err("\r\n");
    y = * (volatile unsigned long *) eax;
    eax += 0x10000000;
  }
}

void dimms_write(int x) {
  uint8_t c;
  unsigned long eax = x;
  for(c = 0; c < 6; c++) {
    print_err("dimms_write: ");
    print_err_hex32(eax);
    print_err("\r\n");
    *(volatile unsigned long *) eax = 0;
    eax += 0x10000000;
  }
}



#ifdef DEBUG_SETNORTHB
void setnorthb(device_t north, uint8_t reg, uint8_t val) {
  print_err("setnorth: reg ");
    print_err_hex8(reg);
  print_err(" to ");
  print_err_hex8(val);
  print_err("\r\n");
  pci_write_config8(north, reg, val);
}
#else
#define setnorthb pci_write_config8
#endif

void
dumpnorth(device_t north) {
  uint8_t r, c;
  for(r = 0; r < 256; r += 16) {
    print_err_hex8(r);
    print_err(":");
    for(c = 0; c < 16; c++) {
      print_err_hex8(pci_read_config8(north, r+c));
      print_err(" ");
    }
    print_err("\r\n");
  }
}
static void sdram_set_registers(const struct mem_controller *ctrl) {
  static const uint16_t raminit_ma_reg_table[] = {
    /* Values for MA type register to try */
    0x0000, 0x8088, 0xe0ee,
    0xffff // end mark
  };
  static const unsigned char ramregs[] = {0x5a, 0x5b, 0x5c, 0x5d, 0x5e, 0x5f, 
	  				0x56, 0x57};

  device_t north = (device_t) 0;
  uint8_t c, r;

  print_err("vt8601 init starting\n");
  north = pci_locate_device(PCI_ID(0x1106, 0x8601), 0);
  north = 0;
  print_err_hex32(north);
  print_err(" is the north\n");
  print_err_hex16(pci_read_config16(north, 0));
  print_err(" ");
  print_err_hex16(pci_read_config16(north, 2));
  print_err("\r\n");

  /* All we are doing now is setting initial known-good values that will
   * be revised later as we read SPD
   */	
  // memory clk enable. We are not using ECC
  pci_write_config8(north,0x78, 0x01);
  print_err_hex8(pci_read_config8(north, 0x78));
  // dram control, see the book. 
#if DIMM_PC133
  pci_write_config8(north,0x68, 0x52);
#else
  pci_write_config8(north,0x68, 0x42);
#endif
  // dram control, see the book. 
  pci_write_config8(north,0x6B, 0x0c);
  // Initial setting, 256MB in each bank, will be rewritten later.
  pci_write_config8(north,0x5A, 0x20);
  print_err_hex8(pci_read_config8(north, 0x5a));
  pci_write_config8(north,0x5B, 0x40);
  pci_write_config8(north,0x5C, 0x60);
  pci_write_config8(north,0x5D, 0x80);
  pci_write_config8(north,0x5E, 0xA0);
  pci_write_config8(north,0x5F, 0xC0);
  // It seems we have to take care of these 2 registers as if 
  // they are bank 6 and 7.
  pci_write_config8(north,0x56, 0xC0);
  pci_write_config8(north,0x57, 0xC0);

  // SDRAM in all banks
  pci_write_config8(north,0x60, 0x3F);
  // DRAM timing. I'm suspicious of this
  // This is for all banks, 64 is 0,1.  65 is 2,3. 66 is 4,5.
  // ras precharge 4T, RAS pulse 5T
  // cas2 is 0xd6, cas3 is 0xe6
  // we're also backing off write pulse width to 2T, so result is 0xee
#if DIMM_CL2
  pci_write_config8(north,0x64, 0xd4);
  pci_write_config8(north,0x65, 0xd4);
  pci_write_config8(north,0x66, 0xd4);
#else // CL=3
  pci_write_config8(north,0x64, 0xe4);
  pci_write_config8(north,0x65, 0xe4);
  pci_write_config8(north,0x66, 0xe4);
#endif

  // dram frequency select.
  // enable 4K pages for 64M dram. 
#if DIMM_PC133
  pci_write_config8(north,0x69, 0x3c);
#else
  pci_write_config8(north,0x69, 0xac);
#endif

  /* IMPORTANT -- disable refresh counter */
  // refresh counter, disabled.
  pci_write_config8(north,0x6A, 0x00);


  // clkenable configuration. kevinh FIXME - add precharge
  pci_write_config8(north,0x6C, 0x00);
  // dram read latch delay of 1 ns, MD drive 8 mA,
  // high drive strength on MA[2:	13], we#, cas#, ras#
  // As per Cindy Lee, set to 0x37, not 0x57
  pci_write_config8(north,0x6D, 0x7f);

  /* Initialize all banks at once */

}

/* slot is the dram slot. Base is the *8M base. */
static unsigned char 
do_module_size(unsigned char slot) { /*, unsigned char base) */
  static const unsigned char log2[256] = {[1] = 0, [2] = 1, [4] = 2, [8] = 3,
			     [16]=4, [32]=5, [64]=6, 
			     [128]=7};
  static const uint8_t ramregs[] = {0x5a, 0x5b, 0x5c, 0x5d, 0x5e, 0x5f, 
	  				0x56, 0x57};
  device_t north = 0;
  /* for all the DRAMS, see if they are there and get the size of each
   * module. This is just a very early first cut at sizing.
   */
  /* we may run out of registers ... */
  unsigned char width, banks, rows, cols, reg;
  unsigned char value = 0;
  unsigned char module = 0xa1 | (slot << 1);
    /* is the module there? if byte 2 is not 4, then we'll assume it 
     * is useless. 
     */
  if (smbus_read_byte(module, 2) != 4)
    goto done;

  //print_err_hex8(slot);
  //    print_err(" is SDRAM\n");
    width = smbus_read_byte(module, 6) | (smbus_read_byte(module,7)<<0);
    banks = smbus_read_byte(module, 17);
    /* we're going to assume symmetric banks. Sorry. */
    cols = smbus_read_byte(module, 4)  & 0xf;
    rows = smbus_read_byte(module, 3)  & 0xf;
    /* grand total. You have rows+cols addressing, * times of banks, times
     * width of data in bytes*/
    /* do this in terms of address bits. Then subtract 23 from it. 
     * That might do it.
     */
    value = cols + rows + log2[banks] + log2[width];
    value -= 23;
    /* now subtract 3 more bits as these are 8-bit bytes */
    value -= 3;
    //    print_err_hex8(value);
    //    print_err(" is the # bits for this bank\n");
    /* now put that size into the correct register */
    value = (1 << value);
 done:
    reg = ramregs[slot];

    //    print_err_hex8(value); print_err(" would go into ");
    //    print_err_hex8(ramregs[reg]); print_err("\n");
    //    pci_write_config8(north, ramregs[reg], value);
    return value;
}

static void sdram_set_spd_registers(const struct mem_controller *ctrl) {
  #define T133 7
  unsigned char Trp = 1, Tras = 1, casl = 2, val;
  unsigned char timing = 0xe4;
  /* read Trp */
  val = smbus_read_byte(0xa0, 27);
  if (val < 2*T133)
    Trp = 1;
  val = smbus_read_byte(0xa0, 30);
  if (val < 5*T133)
    Tras = 0;
  val = smbus_read_byte(0xa0, 18);
  if (val < 8)
    casl = 1;
  if (val < 4)
    casl = 0;

  val = (Trp << 7) | (Tras << 6) | (casl << 4) | 4;

  print_err_hex8(val); print_err(" is the computed timing\n");
  /* don't set it. Experience shows that this screwy chipset should just
   * be run with the most conservative timing.
   * pci_write_config8(0, 0x64, val);
   */
}

static void sdram_enable(int controllers, const struct mem_controller *ctrl) {
  unsigned char i;
  static const uint16_t raminit_ma_reg_table[] = {
    /* Values for MA type register to try */
    0x0000, 0x8088, 0xe0ee,
    0xffff // end mark
  };
  static const uint8_t ramregs[] = {0x5a, 0x5b, 0x5c, 0x5d, 0x5e, 0x5f, 
	  				0x56, 0x57};

  device_t north = 0;
  uint8_t c, r, base;
  /* begin to initialize*/
  // I forget why we need this, but we do
  dimms_write(0xa55a5aa5);

  /* set NOP*/
  pci_write_config8(north,0x6C, 0x01);
  print_err("NOP\r\n");
  /* wait 200us*/
  // You need to do the memory reference. That causes the nop cycle. 
  dimms_read(0);
  udelay(400);
  print_err("PRECHARGE\r\n");
  /* set precharge */
  pci_write_config8(north,0x6C, 0x02);
  print_err("DUMMY READS\r\n");
  /* dummy reads*/
  dimms_read(0);
  udelay(200);
  print_err("CBR\r\n");
  /* set CBR*/
  pci_write_config8(north,0x6C, 0x04);
	
  /* do 8 reads and wait >100us between each - from via*/
  dimms_read(0);
  udelay(200);
  dimms_read(0);
  udelay(200);
  dimms_read(0);
  udelay(200);
  dimms_read(0);
  udelay(200);
  dimms_read(0);
  udelay(200);
  dimms_read(0);
  udelay(200);
  dimms_read(0);
  udelay(200);
  dimms_read(0);
  udelay(200);
  print_err("MRS\r\n");
  /* set MRS*/
  pci_write_config8(north,0x6c, 0x03);
#if DIMM_CL2
  dimms_read(0x150);
#else // CL=3
  dimms_read(0x1d0);
#endif
  udelay(200);
  print_err("NORMAL\r\n");
  /* set to normal mode */
  pci_write_config8(north,0x6C, 0x08);

  dimms_write(0x55aa55aa);
  dimms_read(0);
  udelay(200);
  print_err("set ref. rate\r\n");
  // Set the refresh rate. 
#if DIMM_PC133
  pci_write_config8(north,0x6A, 0x86);
#else
  pci_write_config8(north,0x6A, 0x65);
#endif
  print_err("enable multi-page open\r\n");
  // enable multi-page open
  pci_write_config8(north,0x6B, 0x0d);

  /* Begin auto-detection
   * Find the first bank with DIMM equipped. */

  /* Maximum possible memory in bank 0, none in other banks.
   * Starting from bank 0, we fill 0 in these registers
   * until memory is found. */
  pci_write_config8(north,0x5A, 0xff);
  pci_write_config8(north,0x5B, 0xff);
  pci_write_config8(north,0x5C, 0xff);
  pci_write_config8(north,0x5D, 0xff);
  pci_write_config8(north,0x5E, 0xff);
  pci_write_config8(north,0x5F, 0xff);
  pci_write_config8(north,0x56, 0xff);
  pci_write_config8(north,0x57, 0xff);
  dumpnorth(north);
  print_err("MA\r\n");
  for(c = 0; c < 8; c++) {
    /* Write different values to 0 and 8, then read from 0.
     * If values of address 0 match, we have something there. */
    print_err("write to 0\r\n");
    *(volatile unsigned long *) 0 = 0x12345678;

    /* LEAVE THIS HERE. IT IS ESSENTIAL. OTHERWISE BUFFERING
     * WILL FOOL YOU!
     */
    print_err("write to 8\r\n");
    *(volatile unsigned long *) 8 = 0x87654321;

    if (*(volatile unsigned long *) 0 != 0x12345678) {
      print_err("no memory in this bank\r\n");
      /* No memory in this bank. Tell it to the bridge. */
      pci_write_config8(north,ramregs[c], 0);
    } 
    /* found something */
    {
      uint8_t best = 0;

      /* Detect MA mapping type of the bank. */

      for(r = 0; r < 3; r++) {
	volatile unsigned long esi = 0;
	volatile unsigned long eax = 0;
	pci_write_config8(north,0x58, raminit_ma_reg_table[r]);

	* (volatile unsigned long *) eax = 0;
	print_err(" done write to eax\r\n");
	// Write to addresses with only one address bit
	// on, from 0x80000000 to 0x00000008 (lower 3 bits
	// are ignored, assuming 64-bit bus).  Then what
	// is read at address 0 is the value written to
	// the lowest address where it gets
	// wrap-around. That address is either the size of
	// the bank, or a missing bit due to incorrect MA
	// mapping.
	eax = 0x80000000;
	while (eax !=  4) {
	  * (volatile unsigned long *) eax = eax;
	  //print_err_hex32(eax);
	  outb(eax&0xff, 0x80);
	  eax >>= 1;
	}
	print_err(" done read to eax\r\n");
	eax = * (unsigned long *)0;
	/* oh boy ... what is this. 
	   movl 0, %eax
	   cmpl %eax, %esi
	   jnc 3f
	*/
	print_err("eax and esi: ");
	print_err_hex32(eax); print_err(" ");
	print_err_hex32(esi); print_err("\r\n");

	if (eax > esi) { /* ??*/
		      
	  // This is the current best MA mapping.
	  // Save the address and its MA mapping value.
	  best = r;
	  esi = eax;
	}
      }
		  
      pci_write_config8(north,0x58, raminit_ma_reg_table[best]);
      print_err("enabled first bank of ram ... ma is ");
      print_err_hex8(pci_read_config8(north, 0x58));
      print_err("\r\n");
    }
  }
  base = 0;
  /* runs out of variable space. */
  /* this is unrolled and constants used as much as possible to help
   * us not run out of registers.
   * we'll run out of code space instead :-)
   */
  //  for(i = 0; i < 8; i++)
  base = do_module_size(0); /*, base);*/
  pci_write_config8(north, ramregs[0], base);
  base = do_module_size(1); /*, base);*/
  base += pci_read_config8(north, ramregs[0]);
  pci_write_config8(north, ramregs[1], base);
  /* runs out of code space. */
  for(i = 0; i < 8; i++){
    pci_write_config8(north, ramregs[i], base);
    /*
    pci_write_config8(north, ramregs[3], base);
    pci_write_config8(north, ramregs[4], base);
    pci_write_config8(north, ramregs[5], base);
    pci_write_config8(north, ramregs[6], base);
    pci_write_config8(north, ramregs[7], base);
    */
  }
  /*
  base = do_module_size(0xa0, base);
  base = do_module_size(0xa0, base);
  base = do_module_size(0xa0, base);
  base = do_module_size(0xa0, base);
  base = do_module_size(0xa0, base);
  base = do_module_size(0xa0, base);*/
  print_err("vt8601 done\n");
  dumpnorth(north);
  udelay(1000);
}
