/* 
 * Copyright (C) 2000 Ron Minnich, Advanced Computing Lab, LANL 
 * Copyright (C) 2007 Stefan Reinauer, coresystems GmbH
 */

#ifndef X86_MACROS_H
#define X86_MACROS_H 1


#define port80_post(value)	\
	movb	$value, %al;	\
	outb	%al, $0x80

#endif
