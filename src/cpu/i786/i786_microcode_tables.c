/* i786_microcode_tables.c:	Microcode update for i786 CPUS
 *
 */

unsigned int microcode_updates [] = {
	/*
	  Copyright  Intel Corporation, 1995, 96, 97, 98, 99, 2000. 

	  These microcode updates are distributed for the sole purpose of 
	  installation in the BIOS or Operating System of computer systems
	  which include an Intel P6 family microprocessor sold or distributed
	  to or by you.  You are authorized to copy and install this material
	  on such systems.  You are not authorized to use this material for
	  any other purpose.  
	*/
#include "microcode_m02f2410.c"	/* microcode for CPUID f24, PI 02, Patch 0x10 */
#include "microcode_m02f2734.c" /* microcode for CPUID f27, PI 02, Patch 0x34 */
#include "microcode_m02f2203.c" /* microcode for CPUID f22, PI 02, Patch 0x03 */
};


unsigned int microcode_updates_size = sizeof(microcode_updates);


