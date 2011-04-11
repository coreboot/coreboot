#include "smi.h"
Scope (\_GPE)
{
	Method(_L18, 0, NotSerialized)
	{
		/* Read EC register to clear wake status */
		Store(\_SB.PCI0.LPCB.EC.WAKE, Local0)
	}

       /* SLICE_ON_3M GPE (Dock status) */
       Method(_L1D, 0, NotSerialized)
       {
               if (GP13) {
		       Or(GIV1, 0x20, GIV1)
		       Notify(\_SB.DOCK, 3)
               } else {
		       And(GIV1, 0xdf, GIV1)
		       Notify(\_SB.DOCK, 0)
               }
	}
}
