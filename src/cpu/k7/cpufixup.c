// Needed so the AMD K7 runs correctly. 
#include <printk.h>
#include <cpu/p6/msr.h>
#include <cpu/cpufixup.h>


#SORRY but AMD doesn't want this released.
void
k7_cpufixup(unsigned long ram_kilobytes)
{
}
