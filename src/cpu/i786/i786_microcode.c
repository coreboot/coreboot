/* microcode.c:	Microcode update for i786 CPUS
 *
 */

#include <pciconf.h>
#include <subr.h>
#include <cpu/p6/msr.h>
#include <printk.h>
#include <cpu/p5/cpuid.h>
#include <cpu/cpufixup.h>

extern unsigned int microcode_updates [];
extern unsigned int microcode_updates_size;

struct microcode {
	unsigned int hdrver;
	unsigned int rev;
	unsigned int date;
	unsigned int sig;
	
	unsigned int cksum;
	unsigned int ldrver;
	unsigned int pf;

	unsigned int reserved[5];
	unsigned int bits[500];
};


void display_cpuid_update_microcode(void)
{
	unsigned int eax, ebx, ecx, edx;
	unsigned int pf, rev, sig, val[2];
	unsigned int x86_model, x86_family, i;
	struct microcode *m;
	
	/* cpuid sets msr 0x8B iff a microcode update has been loaded. */
	wrmsr(0x8B, 0, 0);
	cpuid(1, &eax, &ebx, &ecx, &edx);
	rdmsr(0x8B, val[0], rev);
	x86_model = (eax >>4) & 0x0f;
	x86_family = (eax >>8) & 0x0f;
	sig = eax;

	pf = 0;
	if ((x86_model >= 5)||(x86_family>6)) {
		rdmsr(0x17, val[0], val[1]);
		pf = 1 << ((val[1] >> 18) & 7);
	}
	printk_info("microcode_info: sig = 0x%08x pf=0x%08x rev = 0x%08x\n",
	       sig, pf, rev);
   
	m = (void *)&microcode_updates;
	for(i = 0; i < microcode_updates_size/sizeof(struct microcode); i++) {
		if ((m[i].sig == sig) && (m[i].pf == pf)) {
			wrmsr(0x79, (unsigned int)&m[i].bits, 0);
			__asm__ __volatile__ ("cpuid" : : : "ax", "bx", "cx", "dx");
			rdmsr(0x8B, val[0], val[1]);
			printk_info("microcode updated from revision %d to %d\n",
			       rev, val[1]);
		}
	}
}

