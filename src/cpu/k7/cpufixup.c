// Needed so the AMD K7 runs correctly. 
#include <printk.h>
#include <cpu/p6/msr.h>
#include <cpu/cpufixup.h>


#ifndef WE_KNOW_K7_IS_BROKEN
#SORRY but AMD doesnt want this released.
#endif
void
k7_cpufixup(unsigned long ram_kilobytes)
{
	printk_err("WARNING: you are running the busted version of linuxbios\n");
	printk_err("for K7. It will really run slow\n");
}
