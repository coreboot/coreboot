/* SPDX-License-Identifier: GPL-2.0-only */

#include <console/console.h>
#include <cpu/x86/msr.h>
#include <soc/smi.h>
#include <amdblocks/acpimmio_map.h>
#include <amdblocks/psp.h>

#define PSP_MAILBOX_OFFSET		0x10570
#define MSR_CU_CBBCFG			0xc00110a2

void *soc_get_mbox_address(void)
{
	uintptr_t psp_mmio;

	psp_mmio = rdmsr(MSR_CU_CBBCFG).lo;
	if (psp_mmio == 0xffffffff) {
		printk(BIOS_WARNING, "PSP: MSR_CU_CBBCFG uninitialized\n");
		return 0;
	}

	return (void *)(psp_mmio + PSP_MAILBOX_OFFSET);
}

void soc_fill_smm_trig_info(struct smm_trigger_info *trig)
{
	if (!trig)
		return;

	trig->address = 0xfed802a8;
	trig->address_type = SMM_TRIGGER_MEM;
	trig->value_width = SMM_TRIGGER_DWORD;
	trig->value_and_mask = 0xfdffffff;
	trig->value_or_mask = 0x02000000;
}

void soc_fill_smm_reg_info(struct smm_register_info *reg)
{
	if (!reg)
		return;

	reg->smi_enb.address = ACPIMMIO_SMI_BASE + SMI_REG_SMITRIG0;
	reg->smi_enb.address_type = SMM_TRIGGER_MEM;
	reg->smi_enb.value_width = SMM_TRIGGER_DWORD;
	reg->smi_enb.reg_bit_mask = SMITRG0_SMIENB;
	reg->smi_enb.expect_value = 0;

	reg->eos.address = ACPIMMIO_SMI_BASE + SMI_REG_SMITRIG0;
	reg->eos.address_type = SMM_TRIGGER_MEM;
	reg->eos.value_width = SMM_TRIGGER_DWORD;
	reg->eos.reg_bit_mask = SMITRG0_EOS;
	reg->eos.expect_value = SMITRG0_EOS;

	reg->psp_smi_en.address = ACPIMMIO_SMI_BASE + SMI_REG_CONTROL0;
	reg->psp_smi_en.address += sizeof(uint32_t) * SMITYPE_PSP / 16;
	reg->psp_smi_en.address_type = SMM_TRIGGER_MEM;
	reg->psp_smi_en.value_width = SMM_TRIGGER_DWORD;
	reg->psp_smi_en.reg_bit_mask = SMI_MODE_MASK << (2 * SMITYPE_PSP % 16);
	reg->psp_smi_en.expect_value = SMI_MODE_SMI << (2 * SMITYPE_PSP % 16);
}
