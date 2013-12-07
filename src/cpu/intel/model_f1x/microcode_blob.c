/* 256KB cache */
unsigned microcode_updates_f1x[] = {
	/* WARNING - Intel has a new data structure that has variable length
	 * microcode update lengths.  They are encoded in int 8 and 9.  A
	 * dummy header of nulls must terminate the list.
	 */
	#include "microcode-1068-m01f122d.h"
	#include "microcode-1069-m04f122e.h"
	#include "microcode-1070-m02f122f.h"
	#include "microcode-1072-m04f1305.h"

	/*  Dummy terminator  */
        0x0, 0x0, 0x0, 0x0,
        0x0, 0x0, 0x0, 0x0,
        0x0, 0x0, 0x0, 0x0,
        0x0, 0x0, 0x0, 0x0,
};
