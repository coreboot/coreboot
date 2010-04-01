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
 */


#ifndef ROM_INTEL_H
#define	ROM_INTEL_H

/*
 * Bootstrap code for the Intel
 *
 */

#define RET_LABEL(label)	\
	jmp label##_done

#define CALL_LABEL(label)	\
	jmp label		;\
label##_done:

#define CALLSP(func) \
	lea	0f, %esp	; \
	jmp func		; \
0:

#define RETSP \
	jmp *%esp


#define	post_code(value)	\
	movb	$value, %al;	\
	outb	%al, $0x80

#endif /* ROM_INTEL_H */
