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
