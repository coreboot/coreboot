// just define these here. We may never need them anywhere else
#define PNP_COM1_DEVICE 0x1
#define PNP_COM2_DEVICE 0x2

#include <subr.h>
#include <cpu/p5/io.h>

void
enter_pnp()
{
	// unlock it XXX make this a subr at some point 
	outb(0x87, 0x2e);
	outb(0x01, 0x2e);
	outb(0x55, 0x2e);
	outb(0x55, 0x2e);
}

void
exit_pnp()
{
	/* all done. */
	// select configure control
	outb(2, 0x2e);
	outb(2, 0x2f);
}

#ifdef MUST_ENABLE_FLOPPY

void enable_floppy()
{
	/* now set the LDN to floppy LDN */
	outb(0x7, 0x2e);	/* pick reg. 7 */
	outb(0x0, 0x2f);	/* LDN 0 to reg. 7 */

	/* now select register 0x30, and set bit 1 in that register */
	outb(0x30, 0x2e);
	outb(0x1, 0x2f);
}
#endif /* MUST_ENABLE_FLOPPY */

void
enable_com(int com)
{
    unsigned char b;
    /* now set the LDN to com LDN */
    outb(0x7, 0x2e);	/* pick reg. 7 */
    outb(com, 0x2f);	/* LDN 0 to reg. 7 */

    /* now select register 0x30, and set bit 1 in that register */
    outb(0x30, 0x2e);
    outb(0x1, 0x2f);

    /* set CLKIN frequence to 24 MHZ */
    outb(0x24, 0x2e);
    b = inb(0x2f) | 0xfd;
    outb(0x24, 0x2e);
    outb(b, 0x2f);
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
