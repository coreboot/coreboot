/* SPDX-License-Identifier: GPL-2.0-only */

#include <stdint.h>
#include <string.h>
#include <baseboard/variant.h>
#include <drivers/vpd/vpd.h>
#include <fsp/soc_binding.h>
#include <smbios.h>

#define K4E6E304EB_MEM_ID	0x5

#define MEM_SINGLE_CHAN0	0x0
#define MEM_SINGLE_CHAN3	0x3
#define MEM_SINGLE_CHAN4	0x4
#define MEM_SINGLE_CHAN7	0x7
#define MEM_SINGLE_CHANB	0xb
#define MEM_SINGLE_CHANC	0xc

#define VPD_KEY_CUSTOMIZATION_ID    "customization_id"
#define VPD_CUSTOMIZATION_LEN        32

void variant_memory_init_params(FSPM_UPD *mupd, const int spd_index)
{
	FSP_M_CONFIG *mem_cfg;
	mem_cfg = &mupd->FspmConfig;

	/* DQ byte map */
	const u8 dq_map[2][12] = {
		  { 0x0F, 0xF0, 0x00, 0xF0, 0x0F, 0xF0,
		    0x0F, 0x00, 0xFF, 0x00, 0xFF, 0x00 },
		  { 0x0F, 0xF0, 0x00, 0xF0, 0x0F, 0xF0,
		    0x0F, 0x00, 0xFF, 0x00, 0xFF, 0x00 } };
	/* DQS CPU<>DRAM map */
	const u8 dqs_map[2][8] = {
		{ 0, 1, 3, 2, 6, 5, 4, 7 },
		{ 2, 3, 0, 1, 6, 7, 4, 5 } };

	/* Rcomp resistor */
	const u16 RcompResistor[3] = { 200, 81, 162 };

	/* Rcomp target */
	const u16 RcompTarget[5] = { 100, 40, 40, 23, 40 };

	/*Strengthen the Rcomp Target Ctrl for 8GB K4E6E304EB -EGCF*/
	const u16 StrengthendRcompTarget[5] = { 100, 40, 40, 21, 40 };

	/* Default Rcomp Target assignment */
	const u16 *targeted_rcomp = RcompTarget;

	/* Override Rcomp Target assignment for specific SKU(s) */
	if (spd_index == K4E6E304EB_MEM_ID)
		targeted_rcomp = StrengthendRcompTarget;

	memcpy(mem_cfg->DqByteMapCh0, dq_map[0], sizeof(mem_cfg->DqByteMapCh0));
	memcpy(mem_cfg->DqByteMapCh1, dq_map[1], sizeof(mem_cfg->DqByteMapCh1));
	memcpy(mem_cfg->DqsMapCpu2DramCh0, dqs_map[0], sizeof(mem_cfg->DqsMapCpu2DramCh0));
	memcpy(mem_cfg->DqsMapCpu2DramCh1, dqs_map[1], sizeof(mem_cfg->DqsMapCpu2DramCh1));
	memcpy(mem_cfg->RcompResistor, RcompResistor, sizeof(mem_cfg->RcompResistor));
	memcpy(mem_cfg->RcompTarget, targeted_rcomp, sizeof(mem_cfg->RcompTarget));
}

int is_dual_channel(const int spd_index)
{
	return (spd_index != MEM_SINGLE_CHAN0
		&& spd_index != MEM_SINGLE_CHAN3
		&& spd_index != MEM_SINGLE_CHAN4
		&& spd_index != MEM_SINGLE_CHAN7
		&& spd_index != MEM_SINGLE_CHANB
		&& spd_index != MEM_SINGLE_CHANC);
}

/* SKU ID enumeration */
#define SKU_LARS	"sku0"
#define SKU_LILI	"sku1"

const char *smbios_system_sku(void)
{
	if (!CONFIG(VPD))
		return SKU_LARS;

	static char customization_id[VPD_CUSTOMIZATION_LEN];
	if (!vpd_gets(VPD_KEY_CUSTOMIZATION_ID, customization_id,
				VPD_CUSTOMIZATION_LEN, VPD_RO))
		return SKU_LARS;

	if (strstr(customization_id, "LILI"))
		return SKU_LILI;

	return SKU_LARS;
}
