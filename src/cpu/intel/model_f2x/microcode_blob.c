/* 512KB cache */
unsigned microcode_updates_f2x[] = {
	/* WARNING - Intel has a new data structure that has variable length
	 * microcode update lengths.  They are encoded in int 8 and 9.  A
	 * dummy header of nulls must terminate the list.
	 */

	/* Old microcode file not present in Intel's microcode.dat. */
#include "microcode_m02f2203.h"

	/* files from Intel's microcode.dat */
#include "microcode-1343-m04f252b.h"
#include "microcode-1346-m10f252c.h"
#include "microcode-1338-m02f292d.h"
#include "microcode-1340-m08f292f.h"
#include "microcode-1107-m10f2421.h"
#include "microcode-1339-m04f292e.h"
#include "microcode-1105-m08f2420.h"
#include "microcode-1336-m02f2610.h"
#include "microcode-1101-m02f2738.h"
#include "microcode-1100-m04f2737.h"
#include "microcode-1341-m01f2529.h"
#include "microcode-1102-m08f2739.h"
#include "microcode-1104-m04f241e.h"
#include "microcode-1342-m02f252a.h"
#include "microcode-1106-m02f241f.h"

	/*  Dummy terminator  */
        0x0, 0x0, 0x0, 0x0,
        0x0, 0x0, 0x0, 0x0,
        0x0, 0x0, 0x0, 0x0,
        0x0, 0x0, 0x0, 0x0,
};
