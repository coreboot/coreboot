// Skeleton code for Tyson. 
#define PNP_COM1_DEVICE 0x2
#define PNP_COM2_DEVICE 0x3

#include <subr.h>
#include <cpu/p5/io.h>

// funny how all these chips are "pnp compatible", and they're all different. 
#define PNPADDR 0x3f0
#define PNPDATA 0x3f1
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
