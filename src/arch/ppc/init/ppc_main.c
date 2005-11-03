/* 
 * Copyright (C) 2003 by Greg Watson, Los Alamos National Laboratory
 * gwatson@lanl.gov
 */

#include <board.h>
#include <sdram.h>

extern unsigned _iseg[];
extern unsigned _liseg[];
extern unsigned _eliseg[];

void (*payload)(void) = (void (*)(void))_iseg;

/*
 * At this point we're running out of flash with our
 * stack in cache ram. We need to do the following:
 *
 * - turn on real memory
 * - relocate our payload into real memory
 * - start hardwaremain() which does remainder of setup
 */

extern void flush_dcache(void);

void ppc_main(void)
{
	unsigned *from;
	unsigned *to;

	/*
 	 * very early board initialization
	 */
	board_init();

	/*
	 * turn on memory
	 */
	memory_init();

	/*
	 * final initialization before jumping to payload
	 */
	board_init2();

	/*
	 * Flush cache now that memory is enabled.
	 */
	flush_dcache();

	/*
	 * Relocate payload (text & data) if necessary
	 */
	if (_liseg != _iseg) {	
		from = _liseg;
		to = _iseg;
		while (from < _eliseg)
			*to++ = *from++;
	}

	payload();

	/* NOT REACHED */
}
