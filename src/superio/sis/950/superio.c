#ifndef lint
static char rcsid[] = "$Id$";
#endif

// just define these here. We may never need them anywhere else
#define PNP_COM1_DEVICE 0x1
#define PNP_COM2_DEVICE 0x2

#include <subr.h>
#include <pci.h>
#include <cpu/p5/io.h>

static void
enter_pnp(struct superio *s)
{
	// unlock it XXX make this a subr at some point 
	outb(0x87, s->port);
	outb(0x01, s->port);
	outb(0x55, s->port);
	outb(0x55, s->port);
}

static void
exit_pnp(struct superio *s)
{
	/* all done. */
	// select configure control
	outb(2, s->port);
	outb(2, s->port+1);
}


static void
enable_floppy(struct superio *s)
{
	/* now set the LDN to floppy LDN */
	outb(0x7, s->port);	/* pick reg. 7 */
	outb(0x0, s->port+1);	/* LDN 0 to reg. 7 */

	/* now select register 0x30, and set bit 1 in that register */
	outb(0x30, s->port);
	outb(0x1, s->port+1);
}


static void
enable_com(struct superio *s, int com)
{
	unsigned char b;
	/* now set the LDN to com LDN */
	outb(0x7, s->port);	/* pick reg. 7 */
	outb(com, s->port+1);	/* LDN 1,2 to reg. 7 */

	/* now select register 0x30, and set bit 1 in that register */
	outb(0x30, s->port);
	outb(0x1, s->port+1);

	/* set CLKIN frequence to 24 MHZ */
	outb(0x24, s->port);
	b = inb(s->port+1) | 0xfd;
	outb(0x24, s->port);
	outb(b, s->port+1);
}


static void
enable_lpt(struct superio *s)
{
	/* now set the LDN to floppy LDN */
	outb(0x7, s->port);	/* pick reg. 7 */
	outb(0x3, s->port+1);	/* LDN 3 to reg. 7 */

	/* now select register 0x30, and set bit 1 in that register */
	outb(0x30, s->port);
	outb(0x1, s->port+1);
}

static void
finishup(struct superio *s)
{
  enter_pnp(s);

  // don't fool with IDE just yet ...
  if (s->floppy)
    enable_floppy(s);
  
  if (s->com1.enable)
    enable_com(s, PNP_COM1_DEVICE);
  if (s->com2.enable)
    enable_com(s, PNP_COM2_DEVICE);

  if (s->lpt)
    enable_lpt(s);
  
  exit_pnp(s);
}

struct superio_control superio_sis_950_control = {
  (void *)0, (void *)0, finishup, 0x2e, "SiS 950"
};

#if 0
void
final_superio_fixup(void)
{
  superio_sis_950.finishup((struct superio *) 0);
}
#endif


