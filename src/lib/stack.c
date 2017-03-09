/*
This software and ancillary information (herein called SOFTWARE )
called LinuxBIOS          is made available under the terms described
here.  The SOFTWARE has been approved for release with associated
LA-CC Number 00-34   .  Unless otherwise indicated, this SOFTWARE has
been authored by an employee or employees of the University of
California, operator of the Los Alamos National Laboratory under
Contract No. W-7405-ENG-36 with the U.S. Department of Energy.  The
U.S. Government has rights to use, reproduce, and distribute this
SOFTWARE.  The public may copy, distribute, prepare derivative works
and publicly display this SOFTWARE without charge, provided that this
Notice and any statement of authorship are reproduced on all copies.
Neither the Government nor the University makes any warranty, express
or implied, or assumes any liability or responsibility for the use of
this SOFTWARE.  If SOFTWARE is modified to produce derivative works,
such modified SOFTWARE should be clearly marked, so as not to confuse
it with the version available from LANL.
 */
/* Copyright 2000, Ron Minnich, Advanced Computing Lab, LANL
 * rminnich@lanl.gov
 * Copyright (C) 2015 Timothy Pearson <tpearson@raptorengineeringinc.com>, Raptor Engineering
 */

#include <assert.h>
#include <lib.h>
#include <console/console.h>
#include <symbols.h>

int checkstack(void *top_of_stack, int core)
{
	/* Not all archs use CONFIG_STACK_SIZE, those who don't set it to 0. */
	size_t stack_size = CONFIG_STACK_SIZE ? CONFIG_STACK_SIZE : _stack_size;
	int i;
	u32 *stack = (u32 *) (top_of_stack - stack_size);

	if (stack[0] != 0xDEADBEEF){
		printk(BIOS_ERR, "Stack overrun on CPU%d (address %p overwritten). "
			"Increase stack from current %zu bytes\n",
			core, stack, stack_size);
		BUG();
		return -1;
	}

	for (i = 1; i < stack_size/sizeof(stack[0]); i++){
		if (stack[i] == 0xDEADBEEF)
			continue;
		printk(BIOS_SPEW, "CPU%d: stack: %p - %p, ",
			core, stack, &stack[stack_size/sizeof(stack[0])]);
		printk(BIOS_SPEW, "lowest used address %p, ", &stack[i]);
		printk(BIOS_SPEW, "stack used: %ld bytes\n",
			(unsigned long)&stack[stack_size / sizeof(stack[0])]
			- (unsigned long)&stack[i]);
		return 0;
	}

	return 0;

}
