/*
 * Bootstrap code for the STPC Consumer
 * Copyright (c) 1999 by Net Insight AB. All Rights Reserved.
 *
 * $Id$
 *
 */
/* oh, barf. This won't work if all you use is .o's.  -- RGM */

/*
 *	Written by Johan Rydberg, based on work by Daniel Kahlin.
 */
/*
 *	We use ELF as output format. So that we can
 *	debug the code in some form. 
 */
OUTPUT_FORMAT("elf32-i386", "elf32-i386", "elf32-i386")
OUTPUT_ARCH(i386)

/*
 *	Memory map:
 *
 *	0x00000	(4*4096 bytes)	: stack
 *	0x04000 (4096 bytes)	: private data 
 *	0x05000 		: data space
 *	0x90000			: kernel stack
 *	0xf0000 (64 Kbyte)	: EPROM
 */
MEMORY
{
    ram (rwx) : ORIGIN = 0x00000000, LENGTH = 128M	/* 128 MB memory is
							 * max for STPC */
    rom (rx)  : ORIGIN = 0x000f0000, LENGTH = 128K	/* 128 K EPROM */
}

_PDATABASE	= 0x04000;
_RAMBASE	= 0x05000;
_KERNSTK	= 0x90000;
/* should be parameterized but is not, yuck! */
/*
_ROMBASE	= 0xe0000;
 */
_ROMBASE	= 0xf0000;

/*
 *	Entry point is not really nececary, since the mkrom(8)
 *	tool creates a entry point that jumps to $0xc000:0x0000.
 */
/* baloney, but ... RGM*/
ENTRY(_start)

SECTIONS {
	/*
	 * First we place the code and read only data (typically const declared).
	 * This get placed in rom.
	 */
	.text _ROMBASE : {
		_text = .;
		*(.text);
		*(.rodata);
		_etext = .;
	}

	_pdata = .;

/*
	.pdata _PDATABASE : AT ( LOADADDR(.text) + SIZEOF(.text) +  
					SIZEOF(.rodata)) {
 */
	.pdata _PDATABASE : AT ( _etext ) {
		*(.pdata);
	}

	_epdata = LOADADDR(.pdata) + SIZEOF(.pdata);

	/*
	 * After the code we place initialized data (typically initialized
	 * global variables). This gets copied into ram by startup code.
	 * __data_start and __data_end shows where in ram this should be placed,
	 * whereas __data_loadstart and __data_loadend shows where in rom to
	 * copy from.
	 */
	.data _RAMBASE : AT ( LOADADDR(.pdata) + SIZEOF(.pdata) ) {
		_data = .;
		*(.data)
		*(.sdata)
		*(.sdata2)
		*(.got)
		_edata = .;
	}
    
	_ldata	= LOADADDR(.data);
	_eldata	= LOADADDR(.data) + SIZEOF(.data);
 	
	/*
	 * bss does not contain data, it is just a space that should be zero
	 * initialized on startup. (typically uninitialized global variables)
	 * crt0.S fills between __bss_start and __bss_end with zeroes.
	 */
	.bss ( ADDR(.data) + SIZEOF(.data) ) : {
		_bss = .;
		*(.bss)
		*(.sbss)
		*(COMMON)
		_ebss = .;
		_heap = .;
	}
}

/*
 *	This provides the start and end address for the whole image 
 */
_image	= LOADADDR(.text);
_eimage	= LOADADDR(.data) + SIZEOF(.data);

/* EOF */
