#include <subr.h>
#include <cpu/p5/io.h>

/*
 * This file is for setting up the SMC Super IO chip.  
 *
 * This file contains some hard coded mappings for IRQs which may
 * work for most boards but is really board specific.  A
 * configuration mechanism is needed.
 *
 * I have only implemented the UART features that I needed at
 * the time plus enableing EPP mode to get interrupts that can
 * be shared.  Other features for floppies and parallel ports
 * can be added by others as needed.
 *
 *  -Tyson Sawyer  tyson@rwii.com
 *
 * Sharing interrupts between two SMC chips doesn't work for me. -tds
 */

static int smc_configuration_state(int addr, int state) {

  if ((addr!=0x370) && (addr!=0x3f0)) return(-1);

  if (state) {
    outb(0x55, addr);
    return(0);
  }
  else {
    outb(0xAA, addr);
    return(0);
  }
 
  return(-1);
}

static int smc_write(int addr, unsigned char data, unsigned char index) {

  if ((addr!=0x370) && (addr!=0x3f0)) return(-1);

  outb(index, addr);
  outb(data, addr+1);
  return(0);
}

static int smc_read(int addr, unsigned char index, unsigned char *data) {

  if ((addr!=0x370) && (addr!=0x3f0)) return(-1);

  outb(index, addr);
  *data = inb(addr+1);
  return(0);
}

int smc_uart_setup(int addr,
		   int addr1, int irq1,
		   int addr2, int irq2) {

  int rv;
  unsigned char int1, int2;
  unsigned char data;

  if ((addr!=0x370) && (addr!=0x3f0)) return(-1);

  /*
   * Warning:
   * Board specifc mapping of IRQs here.
   * A configuration mechanism is needed.
   */

  switch (irq1) {
  case  3: int1 = 1; break;
  case  4: int1 = 2; break;
  case  5: int1 = 3; break;
  case  6: int1 = 4; break;
  case  7: int1 = 5; break;
  case 10: int1 = 6; break;
  case 11: int1 = 8; break;
  default: int1 = 0;
  }

  switch (irq2) {
  case  3: int2 = 1; break;
  case  4: int2 = 2; break;
  case  5: int2 = 3; break;
  case  6: int2 = 4; break;
  case  7: int2 = 5; break;
  case 10: int2 = 6; break;
  case 11: int2 = 8; break;
  default: int2 = 0;
  }

  if (int1 == int2) {
    int2 = 0x0f;
  }

  rv = smc_configuration_state(addr, 1); if (rv) return(rv);
  rv = smc_write(addr, (addr1>>2) & 0xfe, 0x24); if (rv) return(rv);
  rv = smc_write(addr, (addr2>>2) & 0xfe, 0x25); if (rv) return(rv);
  rv = smc_write(addr, (int1<<4) | int2, 0x28); if (rv) return(rv);

  /* Enable INTB output */

  if ((int1==2) || (int2==2)) {
    rv = smc_read(addr, 0x03, &data); if (rv) return(rv);
    rv = smc_write(addr, data | 0x84, 0x03);  if (rv) return(rv);
  }

  rv = smc_configuration_state(addr, 0); return(rv);
}

int smc_pp_setup(int addr, int pp_addr, int mode) {
  int rv;
  unsigned char data;

  rv = smc_configuration_state(addr, 1); if (rv) return(rv);

  rv = smc_read(addr, 0x04, &data); if (rv) return(rv);
  data = (data & (~0x03)) | (mode & 0x03);
  rv = smc_write(addr, data, 0x04);  if (rv) return(rv);

  rv = smc_read(addr, 0x01, &data); if (rv) return(rv);
  data = data & (~0x08);
  rv = smc_write(addr, data, 0x01);  if (rv) return(rv);

  rv = smc_write(addr, (pp_addr>>2) & 0xff, 0x23);  if (rv) return(rv);

  rv = smc_configuration_state(addr, 0); return(rv);
}

int smc_validbit(int addr, int valid) {
  int rv;
  unsigned char data;

  if ((addr!=0x370) && (addr!=0x3f0)) return(-1);

  rv = smc_configuration_state(addr, 1); if (rv) return(rv);
  rv = smc_read(addr, 0x00, &data); if (rv) return(rv);

  if (valid) {
    data = data | 0x80;
  }
  else {
    data = data & (~0x80);
  }

  rv = smc_write(addr, data, 0x00);  if (rv) return(rv);
  rv = smc_configuration_state(addr, 0); return(rv);
}

/*
 * ============== Linux bios standard superio functions ========
 *
 * These functions suck.
 */

void
enter_pnp()
{
	// unlock it XXX make this a subr at some point 
    // outb(0x87, PNPADDR);
    // outb(0x87, PNPADDR);
}

void
exit_pnp()
{
	/* all done. */
	// select configure control
    // outb(0xaa, PNPADDR);
}

#ifdef MUST_ENABLE_FLOPPY

void enable_floppy()
{
	/* now set the LDN to floppy LDN */
// 	outb(0x7, PNPADDR);	/* pick reg. 7 */
// 	outb(0x0, PNPDATA);	/* LDN 0 to reg. 7 */

	/* now select register 0x30, and set bit 1 in that register */
// 	outb(0x30, PNPADDR);
// 	outb(0x1, PNPDATA);
}
#endif /* MUST_ENABLE_FLOPPY */

void
enable_com(int com)
{
  //   unsigned char b;
    /* now set the LDN to com LDN */
    // outb(0x7, PNPADDR);	/* pick reg. 7 */
    // outb(com, PNPDATA);	/* LDN 0 to reg. 7 */

    /* now select register 0x30, and set bit 1 in that register */
    // outb(0x30, PNPADDR);
    // outb(0x1, PNPDATA);

}

void
final_superio_fixup()
{

    enter_pnp();
#ifdef MUST_ENABLE_FLOPPY
    enable_floppy();
#endif
    enable_com(PNP_COM1_DEVICE);
    enable_com(PNP_COM2_DEVICE);

    exit_pnp();
}
