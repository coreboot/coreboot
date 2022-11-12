/* SPDX-License-Identifier: GPL-2.0-only */

#include <soc/smi.h>
#include <amdblocks/acpimmio.h>
#include <amdblocks/psp.h>
#include <amdblocks/smi.h>
#include <stdint.h>

void soc_fill_smm_trig_info(struct smm_trigger_info *trig)
{
	if (!trig)
		return;

	trig->address = (uintptr_t)acpimmio_smi + SMI_REG_SMITRIG0;
	trig->address_type = SMM_TRIGGER_MEM;
	trig->value_width = SMM_TRIGGER_DWORD;
	trig->value_and_mask = (uint32_t)~SMITRIG0_PSP;
	trig->value_or_mask = (uint32_t)SMITRIG0_PSP;
}

void soc_fill_smm_reg_info(struct smm_register_info *reg)
{
	if (!reg)
		return;

	reg->smi_enb.address = (uintptr_t)acpimmio_smi + SMI_REG_SMITRIG0;
	reg->smi_enb.address_type = SMM_TRIGGER_MEM;
	reg->smi_enb.value_width = SMM_TRIGGER_DWORD;
	reg->smi_enb.reg_bit_mask = SMITRG0_SMIENB;
	reg->smi_enb.expect_value = 0;

	reg->eos.address = (uintptr_t)acpimmio_smi + SMI_REG_SMITRIG0;
	reg->eos.address_type = SMM_TRIGGER_MEM;
	reg->eos.value_width = SMM_TRIGGER_DWORD;
	reg->eos.reg_bit_mask = SMITRG0_EOS;
	reg->eos.expect_value = SMITRG0_EOS;

	reg->psp_smi_en.address = (uintptr_t)acpimmio_smi + SMI_REG_CONTROL0;
	reg->psp_smi_en.address += sizeof(uint32_t) * SMITYPE_PSP / 16;
	reg->psp_smi_en.address_type = SMM_TRIGGER_MEM;
	reg->psp_smi_en.value_width = SMM_TRIGGER_DWORD;
	reg->psp_smi_en.reg_bit_mask = SMI_MODE_MASK << (2 * SMITYPE_PSP % 16);
	reg->psp_smi_en.expect_value = SMI_MODE_SMI << (2 * SMITYPE_PSP % 16);
}
