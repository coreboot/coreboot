/* microcode.c:	Microcode update for PIII and later CPUS
 */

#include <stdint.h>
#include <console/console.h>
#include <cpu/cpu.h>
#include <cpu/x86/msr.h>
#include <cpu/intel/microcode.h>

struct microcode {
	uint32_t hdrver;
	uint32_t rev;
	uint32_t date;
	uint32_t sig;

	uint32_t cksum;
	uint32_t ldrver;
	uint32_t pf;

	uint32_t data_size;
	uint32_t total_size;

	uint32_t reserved[3];
	uint32_t bits[1012];
};


static inline uint32_t read_microcode_rev(void)
{
	/* Some Intel Cpus can be very finicky about the
	 * cpuid sequence used.  So this is implemented in
	 * assembly so that it works reliably.
	 */
	msr_t msr;
	__asm__ volatile (
		"wrmsr\n\t"
		"xorl %%eax, %%eax\n\t"
		"xorl %%edx, %%edx\n\t"
		"movl $0x8b, %%ecx\n\t"
		"wrmsr\n\t"
		"movl $0x01, %%eax\n\t"
		"cpuid\n\t"
		"movl $0x08b, %%ecx\n\t"
		"rdmsr \n\t"
		: /* outputs */
		"=a" (msr.lo), "=d" (msr.hi)
		: /* inputs */
		: /* trashed */
		 "ecx"
		);
	return msr.hi;
}

void intel_update_microcode(const void *microcode_updates)
{
	unsigned int eax;
	unsigned int pf, rev, sig;
	unsigned int x86_model, x86_family;
	const struct microcode *m;
	const char *c;
	msr_t msr;

	/* cpuid sets msr 0x8B iff a microcode update has been loaded. */
	msr.lo = 0;
	msr.hi = 0;
	wrmsr(0x8B, msr);
	eax = cpuid_eax(1);
	msr = rdmsr(0x8B);
	rev = msr.hi;
	x86_model = (eax >>4) & 0x0f;
	x86_family = (eax >>8) & 0x0f;
	sig = eax;

	pf = 0;
	if ((x86_model >= 5)||(x86_family>6)) {
		msr = rdmsr(0x17);
		pf = 1 << ((msr.hi >> 18) & 7);
	}
	print_debug("microcode_info: sig = 0x");
	print_debug_hex32(sig);
	print_debug(" pf=0x");
	print_debug_hex32(pf);
	print_debug(" rev = 0x");
	print_debug_hex32(rev);
	print_debug("\n");

	m = microcode_updates;
	for(c = microcode_updates; m->hdrver;  m = (const struct microcode *)c) {
		if ((m->sig == sig) && (m->pf & pf)) {
			unsigned int new_rev;
			msr.lo = (unsigned long)(&m->bits) & 0xffffffff;
			msr.hi = 0;
			wrmsr(0x79, msr);

			/* Read back the new microcode version */
			new_rev = read_microcode_rev();

			print_debug("microcode updated to revision: ");
			print_debug_hex32(new_rev);
			print_debug(" from revision ");
			print_debug_hex32(rev);
			print_debug("\n");
			break;
		}
		if (m->total_size) {
			c += m->total_size;
		} else {
			c += 2048;
		}
	}
}
