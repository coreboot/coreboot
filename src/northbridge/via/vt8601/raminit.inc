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
/*
 * 11/26/02 - kevinh@ispiri.com - The existing comments implied that
 * this didn't work yet.  Therefore, I've updated it so that it works
 * correctly - at least on my VIA epia motherboard.  64MB DIMM in slot 0.
 */

#define loop200 $0x5000
#define loop100 $0x2500

raminit:
	intel_chip_post_macro(0x35)

/*; new code... pulled from via stuff.*/
/*  initialize registers */
	// memory clk enable. We are not using ECC
	CS_WRITE($0x78, $0x01)
	// dram control, see the book. 
	CS_WRITE($0x68, $0x42)
	// dram control, see the book. 
	CS_WRITE($0x6B, $0x0d)
	// 64/128 MB dram
	CS_WRITE($0x58, $0x80)
	// 64/128 MB dram
	CS_WRITE($0x59, $0x00)
	// bank 0 ends at 64 MB 
	CS_WRITE($0x5A, $0x08)
	// bank 1 ends at 64 MB
	CS_WRITE($0x5B, $0x08)
	// bank 2 ends at 64 MB
	CS_WRITE($0x5C, $0x08)
	// bank 2 ends at 64 MB
	CS_WRITE($0x5D, $0x08)
	// bank 2 ends at 64 MB
	CS_WRITE($0x5E, $0x08)
	// bank 2 ends at 64 MB
	CS_WRITE($0x5F, $0x08)

	// SDRAM in all banks
	CS_WRITE($0x60, $0x3F)
	// DRAM timing. I'm suspicious of this
	// This is for all banks, 64 is 0,1.  65 is 2,3. 66 is 4,5.
	// ras precharge 4T, RAS pulse 5T, CAS 2T
	// as per the note below, we change to cas 3 2000/8/31
	// cas2 is 0xd6, cas3 is 0xe6
	// we're also backing off write pulse width to 2T, so result is 0xee
	CS_WRITE($0x64, $0xe6)
	CS_WRITE($0x65, $0x95)
	CS_WRITE($0x66, $0x95)

	// dram frequency select. We set 66/66.
	// no 256 m, enable 4K pages for 64M dram. 
	CS_WRITE($0x69, $0xac)
	// refresh counter, disabled.
	CS_WRITE($0x6A, $0x00)
	// clkenable configuration. kevinh FIXME - add precharge
	CS_WRITE($0x6C, $0x00)
	// dram read latch delay of 1 ns, MD drive 8 mA,
	// high drive strength on MA[2:	13], we#, cas#, ras#
	// As per Cindy Lee, set to 0x37, not 0x57
	CS_WRITE($0x6D, $0x7f)

/* begin to initialize*/
	// I forget why we need this, but we do
	mov $0xa55a5aa5, %eax
	mov %eax, 0
	mov %eax, 0x4000000
	
/* set NOP*/
	CS_WRITE($0x6C, $0x01)


/* wait 200us*/
	// You need to do the memory reference. That causes the nop cycle. 
	mov 0x0, %eax
	mov 0x4000000, %eax
	DELAY(loop200)
	

/* set precharge */
	CS_WRITE($0x6C, $0x02)

/* dummy reads*/
	mov 0x0, %eax
	mov 0x4000000, %eax

/* set CBR*/
	CS_WRITE($0x6C, $0x04)
	
/* do 8 reads and wait 100us between each - from via*/
	mov 0x0, %eax
	mov 0x4000000, %eax
	DELAY(loop100)
	mov 0x0, %eax
	mov 0x4000000, %eax
	DELAY(loop100)
	mov 0x0, %eax
	mov 0x4000000, %eax
	DELAY(loop100)
	mov 0x0, %eax
	mov 0x4000000, %eax
	DELAY(loop100)
	mov 0x0, %eax
	mov 0x4000000, %eax
	DELAY(loop100)
	mov 0x0, %eax
	mov 0x4000000, %eax
	DELAY(loop100)
	mov 0x0, %eax
	mov 0x4000000, %eax
	DELAY(loop100)
	mov 0x0, %eax
	mov 0x4000000, %eax
	DELAY(loop100)

/* set MRS*/
	// 0x150 is cas2. We are now using 0x1d0, which is cas3
	CS_WRITE($0x6c, $0x03)
	movl $0x1d0, %ecx
	movl (%ecx), %eax
	movl $0x40001d0, %ecx
	movl (%ecx), %eax

/* set to normal mode */
	CS_WRITE($0x6C, $0x08)
	movl $0x55aa55aa, %eax
	mov %eax, 0x0
	mov 0x0, %eax

	// Set the refresh rate. 
	CS_WRITE($0x6A, $0x65)
	// enable multi-page open
	CS_WRITE($0x6B, $0x01)

	
/* From Mike Fan: 
	Hi all:
	If you are porting PM133, then you have to set DRAM Row Ending Address.
	You did not set Rx56 and Rx57 in intel_pm133ram.S.
	That register setting is like Rx5A~Rx5F. 
	Maybe could fix the mem wrapping issue.
	(from Ron Minnich)
		My manual says these are non-cacheable region registers.
	(Turns out the manual is wrong. However, this did not help.
	2000/8/31 8:49 am I am setting all dram to cas3, and if that fails,
		I'll be trying some of Cindy's other recommendations.
		DRAM is currently CAS2. Symptom is an explosion in free_all_bootmem_core,
		In the loop where it is freeing bootmem alloc pages from low mem.
	2000/8/31:	 10:57 No change, Linux still crashes. We'll try Cindy Lee's recommendation
		RE Register 0x6d, set it to 0x37. All our other settings conform to her
		other recommendations. We also need to see whether we should be setting
		Fixed MTRRs, but that seems unlikely.
	2000/8/31:	 5:56 PM. No significant change. We're going to try to use 1 cycle writes
		instead of 2. None of this feels like it is the real problem. Fixed MTRRs
		helped a tiny bit. We can get to schedule() before we crash, but only
		if we set a breakpoint after the first loop in free_all_bootmem_core
 */
        CS_WRITE($0x56, $0x10)
        CS_WRITE($0x57, $0x10)

	intel_chip_post_macro(0x36)
