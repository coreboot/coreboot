#ifndef lint
static char rcsid[] = "$Id$";
#endif

// just define these here. We may never need them anywhere else
#define PNP_COM1_DEVICE 0x1
#define PNP_COM2_DEVICE 0x2

#include <subr.h>
#include <cpu/p5/io.h>

void
enter_pnp(void)
{
}

void
exit_pnp(void)
{
}

#ifdef MUST_ENABLE_FLOPPY
void
enable_floppy(void)
{
}
#endif /* MUST_ENABLE_FLOPPY */

void
enable_com(int com)
{
}

#ifdef MUST_ENABLE_LPT
void
enable_lpt(void)
{
}
#endif /* MUST_ENABLE_LPT */

void
final_superio_fixup(void)
{
	enter_pnp();

#ifdef MUST_ENABLE_FLOPPY
	enable_floppy();
#endif /* MUST_ENABLE_LPT */

	enable_com(PNP_COM1_DEVICE);
	enable_com(PNP_COM2_DEVICE);

#ifdef MUST_ENABLE_LPT
	enable_lpt();
#endif /* MUST_ENABLE_LPT */

	exit_pnp();
}
