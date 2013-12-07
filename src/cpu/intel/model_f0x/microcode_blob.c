/* 256KB cache */
unsigned microcode_updates[] = {
	#include "microcode-678-2f0708.h"
	#include "microcode-965-m01f0a13.h"
	#include "microcode-983-m02f0a15.h"
	#include "microcode-964-m01f0712.h"
	#include "microcode-966-m04f0a14.h"

	/*  Dummy terminator  */
        0x0, 0x0, 0x0, 0x0,
        0x0, 0x0, 0x0, 0x0,
        0x0, 0x0, 0x0, 0x0,
        0x0, 0x0, 0x0, 0x0,
};
