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
 * modified by RGM for the new superio setup. 
 * we need to finish this up but we have to get other parts of the 
 * dell done first. 
 */

static int 
smc_configuration_state(struct superio *s, int state) 
{
	unsigned short addr;
	addr = s->port;
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

static int smc_write(struct superio *s, unsigned char data, 
	unsigned char index) 
{
	unsigned short addr;
	addr = s->port;
	outb(index, addr);
	outb(data, addr+1);
	return(0);
}

static int smc_read(struct superio *s, 
	unsigned char index, unsigned char *data) 
{
	unsigned short addr;
	addr = s->port;

	outb(index, addr);
	*data = inb(addr+1);
	return(0);
}

static int smc_uart_setup(struct superio *s)
{
	unsigned short addr = s->port;
	unsigned short addr1 = s->com1.base;
	unsigned short addr2 = s->com2.base;
	int rv;
	unsigned char int1, int2;
	unsigned char data;


	/*
	 * Warning:
	 * Board specifc mapping of IRQs here.
	 * A configuration mechanism is needed.
	 */
	
	switch (s->com1.irq) {
	case  3: int1 = 1; break;
	case  4: int1 = 2; break;
	case  5: int1 = 3; break;
	case  6: int1 = 4; break;
	case  7: int1 = 5; break;
	case 10: int1 = 6; break;
	case 11: int1 = 8; break;
	default: int1 = 0;
	}

	switch (s->com2.irq) {
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

	rv = smc_write(addr, (addr1>>2) & 0xfe, 0x24); if (rv) return(rv);
	rv = smc_write(addr, (addr2>>2) & 0xfe, 0x25); if (rv) return(rv);
	rv = smc_write(addr, (int1<<4) | int2, 0x28); if (rv) return(rv);

	/* Enable INTB output */

	if ((int1==2) || (int2==2)) {
		rv = smc_read(addr, 0x03, &data); if (rv) return(rv);
		rv = smc_write(addr, data | 0x84, 0x03);  if (rv) return(rv);
	}

}

static int smc_pp_setup(struct superio *s)
{
	unsigned short addr = s->port;
	unsigned short pp_addr = s->lpt1.base;
	unsigned char mode = s->lpt1.mode;
	int rv;
	unsigned char data;


	rv = smc_read(addr, 0x04, &data); if (rv) return(rv);
	data = (data & (~0x03)) | (mode & 0x03);
	rv = smc_write(addr, data, 0x04);  if (rv) return(rv);

	rv = smc_read(addr, 0x01, &data); if (rv) return(rv);
	data = data & (~0x08);
	rv = smc_write(addr, data, 0x01);  if (rv) return(rv);

	rv = smc_write(addr, (pp_addr>>2) & 0xff, 0x23);  if (rv) return(rv);

}

static int smc_validbit(struct superio *s, int valid) 
{
	unsigned short addr = s->port;
	int rv;
	unsigned char data;

	rv = smc_read(addr, 0x00, &data); if (rv) return(rv);

	if (valid) {
		data = data | 0x80;
	}
	else {
		data = data & (~0x80);
	}

	rv = smc_write(addr, data, 0x00);  if (rv) return(rv);
}

static void
finishup(struct superio *s)
{
  	int rv = 0;

	rv = smc_configuration_state(addr, 1); if (rv) return(rv);
	// this really needs to be broken into two com setups, but for onw
	// we leave it.
	if (s->com1.enable)
		rv |= smc_uart_setup(s);
	if (s->lpt1.enable)
		rv |= smc_pp_setup(s);
  
	rv |= smc_configuration_state(addr, 0); return(rv);
	if (rv)
		printk("For %s rv in finishup is %d\n", s->name, rv);
}


struct superio_control superio_smc_fdc37b807_control = {
	(void *)0, (void *)0, finishup, 0x370, "SMC fdc37b807"
};



