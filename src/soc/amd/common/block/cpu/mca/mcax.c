/* SPDX-License-Identifier: GPL-2.0-only */

#include <amdblocks/msr_zen.h>
#include <cpu/x86/lapic.h>
#include <cpu/x86/msr.h>
#include <console/console.h>
#include <types.h>
#include "mca_common_defs.h"
#include "mcax.h"

union mca_bank_ipid {
	struct {
		u64 instance_id:32;    /* instance ID of this IP */
		u64 hardware_id:12;    /* hardware ID of the IP associated with MCA bank */
		u64 instance_id_hi:4;  /* High value of instance ID */
		u64 mca_type:16;       /* McaType of MCA bank with this IP */
	};
	u64 raw;
};

#define MCA_NBIO_ID                     (0x018)
#define  MCA_NBIO_EXT_ERR_PCIE_SIDEBAND       1
#define MCA_UMC_ID                      (0x096)

/* Returns the HardwareID from MSR MCAX_IPID. */
u16 mcax_bank_hardware_id(unsigned int bank)
{
	union mca_bank_ipid ipid;
	ipid.raw = rdmsr(MCAX_IPID_MSR(bank)).raw;
	return ipid.hardware_id;
}

/* Returns the InstanceID from MSR MCAX_IPID. */
u32 mcax_bank_instance_id(unsigned int bank)
{
	union mca_bank_ipid ipid;
	ipid.raw = rdmsr(MCAX_IPID_MSR(bank)).raw;
	/* Returns a package unique ID to identify the MCA source.
	 * Since it does not account for 'instance_id_hi', the ID might not
	 * be unique across all nodes and cannot be used as global identifier.
	 */
	return ipid.instance_id;
}

/* Return true when the bank is for the UMC */
bool mcax_bank_is_umc(unsigned int bank)
{
	return mcax_bank_hardware_id(bank) == MCA_UMC_ID;
}

/* The McaXEnable bit in the config registers of the available MCAX banks is already set by the
   FSP, so no need to set it here again. */

bool mca_skip_check(void)
{
	/* On Zen-based CPUs/APUs the MCA(X) status register have a defined state even in the
	   cold boot path, so no need to skip the check */
	return false;
}

bool mca_skip_error(unsigned int bank)
{
	msr_t status = rdmsr(MCAX_STATUS_MSR(bank));

	/*
	 * Skip NBIO PCIe_Sideband errors.
	 */
	if (mcax_bank_hardware_id(bank) == MCA_NBIO_ID &&
	    mca_err_extcode(status) == MCA_NBIO_EXT_ERR_PCIE_SIDEBAND)
		return true;
	return false;
}

/* Print the contents of the MCAX registers for a given bank */
void mca_print_error(unsigned int bank)
{
	msr_t msr, mca_status;

	printk(BIOS_WARNING, "#MC Error: core %u, bank %u %s\n", initial_lapicid(), bank,
		mca_get_bank_name(bank));

	msr = rdmsr(MCAX_CTL_MSR(bank));
	printk(BIOS_WARNING, "   MC%u_CTL =      %08x_%08x\n", bank, msr.hi, msr.lo);
	msr = mca_status = rdmsr(MCAX_STATUS_MSR(bank));
	printk(BIOS_WARNING, "   MC%u_STATUS =   %08x_%08x\n", bank, msr.hi, msr.lo);
	msr = rdmsr(MCAX_ADDR_MSR(bank));
	printk(BIOS_WARNING, "   MC%u_ADDR =     %08x_%08x\n", bank, msr.hi, msr.lo);
	msr = rdmsr(MCAX_MISC0_MSR(bank));
	printk(BIOS_WARNING, "   MC%u_MISC0 =    %08x_%08x\n", bank, msr.hi, msr.lo);
	msr = rdmsr(MCAX_CONFIG_MSR(bank));
	printk(BIOS_WARNING, "   MC%u_CONFIG =   %08x_%08x\n", bank, msr.hi, msr.lo);
	msr = rdmsr(MCAX_IPID_MSR(bank));
	printk(BIOS_WARNING, "   MC%u_IPID =     %08x_%08x\n", bank, msr.hi, msr.lo);
	msr = rdmsr(MCAX_SYND_MSR(bank));
	printk(BIOS_WARNING, "   MC%u_SYND =     %08x_%08x\n", bank, msr.hi, msr.lo);
	msr = rdmsr(MCAX_DESTAT_MSR(bank));
	printk(BIOS_WARNING, "   MC%u_DESTAT =   %08x_%08x\n", bank, msr.hi, msr.lo);
	msr = rdmsr(MCAX_DEADDR_MSR(bank));
	printk(BIOS_WARNING, "   MC%u_DEADDR =   %08x_%08x\n", bank, msr.hi, msr.lo);
	msr = rdmsr(MCAX_MISC1_MSR(bank));
	printk(BIOS_WARNING, "   MC%u_MISC1 =    %08x_%08x\n", bank, msr.hi, msr.lo);
	msr = rdmsr(MCAX_MISC2_MSR(bank));
	printk(BIOS_WARNING, "   MC%u_MISC2 =    %08x_%08x\n", bank, msr.hi, msr.lo);
	msr = rdmsr(MCAX_MISC3_MSR(bank));
	printk(BIOS_WARNING, "   MC%u_MISC3 =    %08x_%08x\n", bank, msr.hi, msr.lo);
	msr = rdmsr(MCAX_MISC4_MSR(bank));
	printk(BIOS_WARNING, "   MC%u_MISC4 =    %08x_%08x\n", bank, msr.hi, msr.lo);
	if (mca_syndv(mca_status)) {
		msr = rdmsr(MCAX_SYND1_MSR(bank));
		printk(BIOS_WARNING, "   MC%u_SYND1 =    %08x_%08x\n", bank, msr.hi, msr.lo);
		msr = rdmsr(MCAX_SYND2_MSR(bank));
		printk(BIOS_WARNING, "   MC%u_SYND2 =    %08x_%08x\n", bank, msr.hi, msr.lo);
	}
	msr = rdmsr(MCA_CTL_MASK_MSR(bank));
	printk(BIOS_WARNING, "   MC%u_CTL_MASK = %08x_%08x\n", bank, msr.hi, msr.lo);
}


/**
 * Fill in FRU text if from MSR SYND1 and SYND2 if supported.
 * @param bank     The MCA bank to check
 * @param fru_text The array to fill
 *
 * @return CB_SUCCESS on success
 */
enum cb_err amd_mca_fill_fru_from_synd(const int bank, char fru_text[17])
{
	msr_t msr = rdmsr(MCAX_CONFIG_MSR(bank));

	if (!(msr.lo & MCAX_CONFIG_MSR_FRU_TEXT))
		return CB_ERR_NOT_IMPLEMENTED;

	msr = rdmsr(MCAX_SYND1_MSR(bank));
	if (!msr.raw)
		return CB_ERR;

	strncpy(&fru_text[0], (char *)&msr.raw, sizeof(msr_t));
	msr = rdmsr(MCAX_SYND2_MSR(bank));
	strncpy(&fru_text[8], (char *)&msr.raw, sizeof(msr_t));
	fru_text[16] = 0;

	return CB_SUCCESS;
}
