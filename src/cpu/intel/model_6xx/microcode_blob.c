static uint32_t microcode_updates[] = {
	/* WARNING - Intel has a new data structure that has variable length
	 * microcode update lengths.  They are encoded in int 8 and 9.  A
	 * dummy header of nulls must terminate the list.
	 */

#include "microcode-99-B_c6_612.h"
#include "microcode-43-B_c6_617.h"
#include "microcode-51-B_c6_616.h"
#include "microcode-153-d2_619.h"

#include "microcode-308-MU163336.h"
#include "microcode-309-MU163437.h"

#include "microcode-358-MU166d05.h"
#include "microcode-359-MU166d06.h"
#include "microcode-386-MU16600a.h"
#include "microcode-398-MU166503.h"
#include "microcode-399-MU166a0b.h"
#include "microcode-400-MU166a0c.h"
#include "microcode-401-MU166a0d.h"
#include "microcode-402-MU166d07.h"

#include "microcode-566-mu26a003.h"
#include "microcode-588-mu26a101.h"
#include "microcode-620-MU26a401.h"

	/*  Dummy terminator  */
        0x0, 0x0, 0x0, 0x0,
        0x0, 0x0, 0x0, 0x0,
        0x0, 0x0, 0x0, 0x0,
        0x0, 0x0, 0x0, 0x0,
};
