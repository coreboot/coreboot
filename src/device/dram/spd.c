/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <device/dram/spd.h>
#include <spd.h>
#include <stddef.h>

const char *spd_manufacturer_name(const uint16_t mod_id)
{
	switch (mod_id) {
	case 0x9b85:
		return "Crucial";
	case 0x4304:
		return "Ramaxel";
	case 0x4f01:
		return "Transcend";
	case 0x9801:
		return "Kingston";
	case 0x987f:
	case 0xad00:
		return "Hynix";
	case 0x9e02:
		return "Corsair";
	case 0xb004:
		return "OCZ";
	case 0xad80:
		return "Hynix/Hyundai";
	case 0x3486:
		return "Super Talent";
	case 0xcd04:
		return "GSkill";
	case 0xce80:
	case 0xce00:
		return "Samsung";
	case 0xfe02:
		return "Elpida";
	case 0x2c80:
	case 0x2c00:
		return "Micron";
	default:
		return NULL;
	}
}

static void convert_default_module_type_to_spd_info(struct spd_info *info)
{
	info->form_factor = MEMORY_FORMFACTOR_UNKNOWN;
	info->type_detail = MEMORY_TYPE_DETAIL_UNKNOWN;
}

static void convert_ddr2_module_type_to_spd_info(enum ddr2_module_type module_type,
						 struct spd_info *info)
{
	switch (module_type) {
	case DDR2_SPD_RDIMM:
	case DDR2_SPD_MINI_RDIMM:
		info->form_factor = MEMORY_FORMFACTOR_RIMM;
		info->type_detail = MEMORY_TYPE_DETAIL_REGISTERED;
		break;
	case DDR2_SPD_UDIMM:
	case DDR2_SPD_MINI_UDIMM:
		info->form_factor = MEMORY_FORMFACTOR_DIMM;
		info->type_detail = MEMORY_TYPE_DETAIL_UNBUFFERED;
		break;
	case DDR2_SPD_MICRO_DIMM:
		info->form_factor = MEMORY_FORMFACTOR_DIMM;
		info->type_detail = MEMORY_TYPE_DETAIL_UNKNOWN;
		break;
	case DDR2_SPD_SODIMM:
		info->form_factor = MEMORY_FORMFACTOR_SODIMM;
		info->type_detail = MEMORY_TYPE_DETAIL_UNKNOWN;
		break;
	default:
		convert_default_module_type_to_spd_info(info);
		break;
	}
}

static void convert_ddr3_module_type_to_spd_info(enum ddr3_module_type module_type,
						 struct spd_info *info)
{
	switch (module_type) {
	case DDR3_SPD_RDIMM:
	case DDR3_SPD_MINI_RDIMM:
		info->form_factor = MEMORY_FORMFACTOR_RIMM;
		info->type_detail = MEMORY_TYPE_DETAIL_REGISTERED;
		break;
	case DDR3_SPD_UDIMM:
	case DDR3_SPD_MINI_UDIMM:
		info->form_factor = MEMORY_FORMFACTOR_DIMM;
		info->type_detail = MEMORY_TYPE_DETAIL_UNBUFFERED;
		break;
	case DDR3_SPD_MICRO_DIMM:
		info->form_factor = MEMORY_FORMFACTOR_DIMM;
		info->type_detail = MEMORY_TYPE_DETAIL_UNKNOWN;
		break;
	case DDR3_SPD_SODIMM:
	case DDR3_SPD_72B_SO_UDIMM:
		info->form_factor = MEMORY_FORMFACTOR_SODIMM;
		info->type_detail = MEMORY_TYPE_DETAIL_UNKNOWN;
		break;
	default:
		convert_default_module_type_to_spd_info(info);
		break;
	}
}

static void convert_ddr4_module_type_to_spd_info(enum ddr4_module_type module_type,
						 struct spd_info *info)
{
	switch (module_type) {
	case DDR4_SPD_RDIMM:
	case DDR4_SPD_MINI_RDIMM:
		info->form_factor = MEMORY_FORMFACTOR_RIMM;
		info->type_detail = MEMORY_TYPE_DETAIL_REGISTERED;
		break;
	case DDR4_SPD_UDIMM:
	case DDR4_SPD_MINI_UDIMM:
		info->form_factor = MEMORY_FORMFACTOR_DIMM;
		info->type_detail = MEMORY_TYPE_DETAIL_UNBUFFERED;
		break;
	case DDR4_SPD_SODIMM:
	case DDR4_SPD_72B_SO_UDIMM:
		info->form_factor = MEMORY_FORMFACTOR_SODIMM;
		info->type_detail = MEMORY_TYPE_DETAIL_UNKNOWN;
		break;
	default:
		convert_default_module_type_to_spd_info(info);
		break;
	}
}

static void convert_ddr5_module_type_to_spd_info(enum ddr5_module_type module_type,
						 struct spd_info *info)
{
	switch (module_type) {
	case DDR5_SPD_RDIMM:
	case DDR5_SPD_MINI_RDIMM:
		info->form_factor = MEMORY_FORMFACTOR_RIMM;
		info->type_detail = MEMORY_TYPE_DETAIL_REGISTERED;
		break;
	case DDR5_SPD_UDIMM:
	case DDR5_SPD_MINI_UDIMM:
		info->form_factor = MEMORY_FORMFACTOR_DIMM;
		info->type_detail = MEMORY_TYPE_DETAIL_UNBUFFERED;
		break;
	case DDR5_SPD_SODIMM:
	case DDR5_SPD_72B_SO_UDIMM:
		info->form_factor = MEMORY_FORMFACTOR_SODIMM;
		info->type_detail = MEMORY_TYPE_DETAIL_UNKNOWN;
		break;
	case DDR5_SPD_2DPC:
		info->form_factor = MEMORY_FORMFACTOR_PROPRIETARY_CARD;
		info->type_detail = MEMORY_TYPE_DETAIL_UNKNOWN;
		break;
	default:
		convert_default_module_type_to_spd_info(info);
		break;
	}
}

static void convert_lpx_module_type_to_spd_info(enum lpx_module_type module_type,
						struct spd_info *info)
{
	switch (module_type) {
	case LPX_SPD_NONDIMM:
		info->form_factor = MEMORY_FORMFACTOR_ROC;
		info->type_detail = MEMORY_TYPE_DETAIL_UNKNOWN;
		break;
	default:
		convert_default_module_type_to_spd_info(info);
		break;
	}
}

void get_spd_info(smbios_memory_type memory_type, uint8_t module_type, struct spd_info *info)
{
	switch (memory_type) {
	case MEMORY_TYPE_DDR2:
		convert_ddr2_module_type_to_spd_info(module_type, info);
		break;
	case MEMORY_TYPE_DDR3:
		convert_ddr3_module_type_to_spd_info(module_type, info);
		break;
	case MEMORY_TYPE_DDR4:
		convert_ddr4_module_type_to_spd_info(module_type, info);
		break;
	case MEMORY_TYPE_DDR5:
		convert_ddr5_module_type_to_spd_info(module_type, info);
		break;
	case MEMORY_TYPE_LPDDR3:
	case MEMORY_TYPE_LPDDR4:
	case MEMORY_TYPE_LPDDR5:
		convert_lpx_module_type_to_spd_info(module_type, info);
		break;
	default:
		convert_default_module_type_to_spd_info(info);
		break;
	}
}

static uint8_t convert_default_form_factor_to_module_type(void)
{
	return SPD_UNDEFINED;
}

static uint8_t convert_ddrx_form_factor_to_module_type(smbios_memory_type memory_type,
						       smbios_memory_form_factor form_factor)
{
	uint8_t module_type;

	switch (form_factor) {
	case MEMORY_FORMFACTOR_DIMM:
		return DDR2_SPD_UDIMM;
	case MEMORY_FORMFACTOR_RIMM:
		return DDR2_SPD_RDIMM;
	case MEMORY_FORMFACTOR_SODIMM:
		module_type = (memory_type == MEMORY_TYPE_DDR2) ? DDR2_SPD_SODIMM :
									DDR3_SPD_SODIMM;
		return module_type;
	default:
		return convert_default_form_factor_to_module_type();
	}
}

static uint8_t convert_lpx_form_factor_to_module_type(smbios_memory_form_factor form_factor)
{
	switch (form_factor) {
	case MEMORY_FORMFACTOR_ROC:
		return LPX_SPD_NONDIMM;
	default:
		return convert_default_form_factor_to_module_type();
	}
}

uint8_t convert_form_factor_to_module_type(smbios_memory_type memory_type,
					   smbios_memory_form_factor form_factor)
{
	uint8_t module_type;

	switch (memory_type) {
	case MEMORY_TYPE_DDR2:
	case MEMORY_TYPE_DDR3:
	case MEMORY_TYPE_DDR4:
	case MEMORY_TYPE_DDR5:
		module_type = convert_ddrx_form_factor_to_module_type(memory_type, form_factor);
		break;
	case MEMORY_TYPE_LPDDR3:
	case MEMORY_TYPE_LPDDR4:
	case MEMORY_TYPE_LPDDR5:
		module_type = convert_lpx_form_factor_to_module_type(form_factor);
		break;
	default:
		module_type = convert_default_form_factor_to_module_type();
		break;
	}

	return module_type;
}
