/* SPDX-License-Identifier: GPL-2.0-only */

#include <amdblocks/mca.h>
#include <amdblocks/reset.h>
#include <cpu/amd/msr.h>
#include <cpu/x86/lapic.h>
#include <cpu/x86/msr.h>
#include <console/console.h>
#include <types.h>

static const char *const mca_bank_name[] = {
	[0] = "Load-store unit",
	[1] = "Instruction fetch unit",
	[2] = "Combined unit",
	/* Bank 3 is reserved and not all corresponding MSRs are implemented in Family 15h.
	   Accessing non-existing MSRs causes a general protection fault. */
	[3] = NULL,
	[4] = "Northbridge",
	[5] = "Execution unit",
	[6] = "Floating point unit"
};

static bool mca_is_valid_bank(unsigned int bank)
{
	return (bank < ARRAY_SIZE(mca_bank_name) && mca_bank_name[bank] != NULL);
}

static const char *mca_get_bank_name(unsigned int bank)
{
	if (mca_is_valid_bank(bank))
		return mca_bank_name[bank];
	else
		return "";
}

static void mca_print_error(unsigned int bank)
{
	msr_t msr;

	printk(BIOS_WARNING, "#MC Error: core %u, bank %u %s\n", initial_lapicid(), bank,
		mca_get_bank_name(bank));

	msr = rdmsr(IA32_MC_STATUS(bank));
	printk(BIOS_WARNING, "   MC%u_STATUS =   %08x_%08x\n", bank, msr.hi, msr.lo);
	msr = rdmsr(IA32_MC_ADDR(bank));
	printk(BIOS_WARNING, "   MC%u_ADDR =     %08x_%08x\n", bank, msr.hi, msr.lo);
	msr = rdmsr(IA32_MC_MISC(bank));
	printk(BIOS_WARNING, "   MC%u_MISC =     %08x_%08x\n", bank, msr.hi, msr.lo);
	msr = rdmsr(IA32_MC_CTL(bank));
	printk(BIOS_WARNING, "   MC%u_CTL =      %08x_%08x\n", bank, msr.hi, msr.lo);
	msr = rdmsr(MC_CTL_MASK(bank));
	printk(BIOS_WARNING, "   MC%u_CTL_MASK = %08x_%08x\n", bank, msr.hi, msr.lo);
}

void mca_check_all_banks(void)
{
	struct mca_bank_status mci;
	const unsigned int num_banks = mca_get_bank_count();

	if (!is_warm_reset())
		return;

	for (unsigned int i = 0 ; i < num_banks ; i++) {
		if (!mca_is_valid_bank(i))
			continue;

		mci.bank = i;
		mci.sts = rdmsr(IA32_MC_STATUS(i));
		if (mci.sts.hi || mci.sts.lo) {
			mca_print_error(i);

			if (CONFIG(ACPI_BERT) && mca_valid(mci.sts))
				build_bert_mca_error(&mci);
		}
	}
}
