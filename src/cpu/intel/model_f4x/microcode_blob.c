unsigned microcode_updates_f4x[] = {
	/* WARNING - Intel has a new data structure that has variable length
	 * microcode update lengths.  They are encoded in int 8 and 9.  A
	 * dummy header of nulls must terminate the list.
	 */

	#include "microcode-1735-m01f480c.h"
	#include "microcode-1460-m9df4305.h"
	#include "microcode-2492-m02f480e.h"
	#include "microcode-1470-m9df4703.h"
	#include "microcode-1521-m5ff4807.h"
	#include "microcode-1466-m02f4116.h"
	#include "microcode-1469-m9df4406.h"
	#include "microcode-1471-mbdf4117.h"
	#include "microcode-1637-m5cf4a04.h"
	#include "microcode-1462-mbdf4903.h"
	#include "microcode-1498-m5df4a02.h"

	/*  Dummy terminator  */
        0x0, 0x0, 0x0, 0x0,
        0x0, 0x0, 0x0, 0x0,
        0x0, 0x0, 0x0, 0x0,
        0x0, 0x0, 0x0, 0x0,
};
