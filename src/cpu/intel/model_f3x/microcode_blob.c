unsigned microcode_updates_f3x[] = {
	/* WARNING - Intel has a new data structure that has variable length
	 * microcode update lengths.  They are encoded in int 8 and 9.  A
	 * dummy header of nulls must terminate the list.
	 */

#include "microcode-1290-m0df320a.h"
#include "microcode-1467-m0df330c.h"
#include "microcode-1468-m1df3417.h"

	/*  Dummy terminator  */
        0x0, 0x0, 0x0, 0x0,
        0x0, 0x0, 0x0, 0x0,
        0x0, 0x0, 0x0, 0x0,
        0x0, 0x0, 0x0, 0x0,
};
