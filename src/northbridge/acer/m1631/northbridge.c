#include <pci.h>


// FIX ME!
unsigned long sizeram()
{
	return 64*1024*1024;
}


#ifdef HAVE_FRAMEBUFFER

void intel_framebuffer_on()
{
}
#endif

final_northbridge_fixup()
{
    printk("SET THAT BIT!\n");
    /* set bit 4 of north bridge register d4 to 1 */
}
