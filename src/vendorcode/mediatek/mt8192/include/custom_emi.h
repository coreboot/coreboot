/* SPDX-License-Identifier: BSD-3-Clause */

#ifndef __CUSTOM_EMI__
#define __CUSTOM_EMI__

#define __ETT__ 0

#include "dramc_pi_api.h"
#include "emi.h"

#define MT29VZZZBD9DQKPR

#ifdef MT29VZZZBD9DQKPR
EMI_SETTINGS default_emi_setting =
//MT29VZZZBD9DQKPR
{
	0x1,		/* sub_version */
	0x0206,		/* TYPE */
	9,		/* EMMC ID/FW ID checking length */
	0,		/* FW length */
	{0x13,0x01,0x4E,0x53,0x30,0x4A,0x39,0x4D,0x39,0x0,0x0,0x0,0x0,0x0,0x0,0x0},		/* NAND_EMMC_ID */
	{0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0},		/* FW_ID */
	0x3530154,		/* EMI_CONA_VAL */
	0x66660033,		/* EMI_CONH_VAL */
	.DRAMC_ACTIME_UNION = {
	0x00000000,		/* U 00 */
	0x00000000,		/* U 01 */
	0x00000000,		/* U 02 */
	0x00000000,		/* U 03 */
	0x00000000,		/* U 04 */
	0x00000000,		/* U 05 */
	0x00000000,		/* U 06 */
	0x00000000,		/* U 07 */
	},
	{0xC0000000,0xC0000000,0,0},		/* DRAM RANK SIZE */
	0x421000,		/* EMI_CONF_VAL */
	0x466005D,		/* CHN0_EMI_CONA_VAL */
	0x466005D,		/* CHN1_EMI_CONA_VAL */
	CBT_R0_R1_NORMAL,		/* dram_cbt_mode_extern */
	{0,0,0,0,0,0},		/* reserved 6 */
	0x000000FF,		/* LPDDR4X_MODE_REG5 */
	0,		/* PIN_MUX_TYPE for tablet */
};
#endif

EMI_SETTINGS emi_settings[] =
{
	//H9HKNNNFBMMVAR - 4GB (2+2)
	{
		0x1,		/* sub_version */
		0x0006,		/* TYPE */
		0,		/* EMMC ID/FW ID checking length */
		0,		/* FW length */
		{0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0},		/* NAND_EMMC_ID */
		{0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0},		/* FW_ID */
		0xF053F154,		/* EMI_CONA_VAL */
		0x44440003,		/* EMI_CONH_VAL */
		.DRAMC_ACTIME_UNION = {
		0x00000000,		/* U 00 */
		0x00000000,		/* U 01 */
		0x00000000,		/* U 02 */
		0x00000000,		/* U 03 */
		0x00000000,		/* U 04 */
		0x00000000,		/* U 05 */
		0x00000000,		/* U 06 */
		0x00000000,		/* U 07 */
		},
		{0x80000000,0x80000000,0,0},		/* DRAM RANK SIZE */
		0x421000,		/* EMI_CONF_VAL */
		0x444F051,		/* CHN0_EMI_CONA_VAL */
		0x444F051,		/* CHN1_EMI_CONA_VAL */
		CBT_R0_R1_NORMAL,		/* dram_cbt_mode_extern */
		{0,0,0,0,0,0},		/* reserved 6 */
		0x00000006,		/* LPDDR4X_MODE_REG5 */
		0,		/* PIN_MUX_TYPE for tablet */
	} ,
	//MT29VZZZBD9DQKPR - 6GB (3+3)
	{
		0x1,		/* sub_version */
		0x0206,		/* TYPE */
		9,		/* EMMC ID/FW ID checking length */
		0,		/* FW length */
		{0x13,0x01,0x4E,0x53,0x30,0x4A,0x39,0x4D,0x39,0x0,0x0,0x0,0x0,0x0,0x0,0x0},		/* NAND_EMMC_ID */
		{0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0},		/* FW_ID */
		0x3530154,		/* EMI_CONA_VAL */
		0x66660033,		/* EMI_CONH_VAL */
		.DRAMC_ACTIME_UNION = {
		0x00000000,		/* U 00 */
		0x00000000,		/* U 01 */
		0x00000000,		/* U 02 */
		0x00000000,		/* U 03 */
		0x00000000,		/* U 04 */
		0x00000000,		/* U 05 */
		0x00000000,		/* U 06 */
		0x00000000,		/* U 07 */
		},
		{0xC0000000,0xC0000000,0,0},		/* DRAM RANK SIZE */
		0x421000,		/* EMI_CONF_VAL */
		0x466005D,		/* CHN0_EMI_CONA_VAL */
		0x466005D,		/* CHN1_EMI_CONA_VAL */
		CBT_R0_R1_NORMAL,		/* dram_cbt_mode_extern */
		{0,0,0,0,0,0},		/* reserved 6 */
		0x000000FF,		/* LPDDR4X_MODE_REG5 */
		0,		/* PIN_MUX_TYPE for tablet */
	} ,
	//H9HQ16AFAMMDAR / H9HCNNNFAMMLXR-NEE / K4UCE3Q4AA-MGCR - 8GB (4+4) Byte Mode
	{
		0x1,		/* sub_version */
		0x0306,		/* TYPE */
		14,		/* EMMC ID/FW ID checking length */
		0,		/* FW length */
		{0x48,0x39,0x48,0x51,0x31,0x36,0x41,0x46,0x41,0x4D,0x4D,0x44,0x41,0x52,0x0,0x0},		/* NAND_EMMC_ID */
		{0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0},		/* FW_ID */
		0x3530154,		/* EMI_CONA_VAL */
		0x88880033,		/* EMI_CONH_VAL */
		.DRAMC_ACTIME_UNION = {
		0x00000000,		/* U 00 */
		0x00000000,		/* U 01 */
		0x00000000,		/* U 02 */
		0x00000000,		/* U 03 */
		0x00000000,		/* U 04 */
		0x00000000,		/* U 05 */
		0x00000000,		/* U 06 */
		0x00000000,		/* U 07 */
		},
		{0x100000000,0x100000000,0,0},		/* DRAM RANK SIZE */
		0x421000,		/* EMI_CONF_VAL */
		0x488005D,		/* CHN0_EMI_CONA_VAL */
		0x488005D,		/* CHN1_EMI_CONA_VAL */
		CBT_R0_R1_BYTE,		/* dram_cbt_mode_extern */
		{0,0,0,0,0,0},		/* reserved 6 */
		0x00000006,		/* LPDDR4X_MODE_REG5 */
		0,		/* PIN_MUX_TYPE for tablet */
	},
	//MT29VZZZAD8GQFSL-046 - 4GB -Normal mode (4+0)
	{
		0x1,		/* sub_version */
		0x0006,		/* TYPE */
		0,		/* EMMC ID/FW ID checking length */
		0,		/* FW length */
		{0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0},		/* NAND_EMMC_ID */
		{0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0},		/* FW_ID */
		0x3500154,		/* EMI_CONA_VAL */
		0x88880033,		/* EMI_CONH_VAL */
		.DRAMC_ACTIME_UNION = {
		0x00000000,		/* U 00 */
		0x00000000,		/* U 01 */
		0x00000000,		/* U 02 */
		0x00000000,		/* U 03 */
		0x00000000,		/* U 04 */
		0x00000000,		/* U 05 */
		0x00000000,		/* U 06 */
		0x00000000,		/* U 07 */
		},
		{0x100000000,0,0,0},		/* DRAM RANK SIZE */
		0x421000,		/* EMI_CONF_VAL */
		0x488005C,		/* CHN0_EMI_CONA_VAL */
		0x488005C,		/* CHN1_EMI_CONA_VAL */
		CBT_R0_R1_NORMAL,		/* dram_cbt_mode_extern */
		{0,0,0,0,0,0},		/* reserved 6 */
		0x00000006,		/* LPDDR4X_MODE_REG5 */
		0,		/* PIN_MUX_TYPE for tablet */
	},
	//KM2V8001CM_B707 - 6GB -byte mode (2+4)
	{
		0x1,		/* sub_version */
		0x0306,		/* TYPE */
		14,		/* EMMC ID/FW ID checking length */
		0,		/* FW length */
		{0x4b,0x4d,0x32,0x56,0x38,0x30,0x30,0x31,0x43,0x4d,0x2d,0x42,0x37,0x30,0x0,0x0},		/* NAND_EMMC_ID */
		{0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0},		/* FW_ID */
		0x32533154,		/* EMI_CONA_VAL */
		0x84840023,		/* EMI_CONH_VAL */
		.DRAMC_ACTIME_UNION = {
		0x00000000,		/* U 00 */
		0x00000000,		/* U 01 */
		0x00000000,		/* U 02 */
		0x00000000,		/* U 03 */
		0x00000000,		/* U 04 */
		0x00000000,		/* U 05 */
		0x00000000,		/* U 06 */
		0x00000000,		/* U 07 */
		},
		{0x80000000,0x100000000,0,0},		/* DRAM RANK SIZE */
		0x421000,		/* EMI_CONF_VAL */
		0x4843059,		/* CHN0_EMI_CONA_VAL */
		0x4843059,		/* CHN1_EMI_CONA_VAL */
		CBT_R0_NORMAL_R1_BYTE,		/* dram_cbt_mode_extern */
		{0,0,0,0,0,0},		/* reserved 6 */
		0x00000001,		/* LPDDR4X_MODE_REG5 */
		0,		/* PIN_MUX_TYPE for tablet */
	} ,
	//MT53E2G32D4 - 8GB (4+4) Normal Mode
	{
		0x1,		/* sub_version */
		0x0306,		/* TYPE */
		14,		/* EMMC ID/FW ID checking length */
		0,		/* FW length */
		{0x48,0x39,0x48,0x51,0x31,0x36,0x41,0x46,0x41,0x4D,0x4D,0x44,0x41,0x52,0x0,0x0},		/* NAND_EMMC_ID */
		{0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0},		/* FW_ID */
		0x3530154,		/* EMI_CONA_VAL */
		0x88880033,		/* EMI_CONH_VAL */
		.DRAMC_ACTIME_UNION = {
		0x00000000,		/* U 00 */
		0x00000000,		/* U 01 */
		0x00000000,		/* U 02 */
		0x00000000,		/* U 03 */
		0x00000000,		/* U 04 */
		0x00000000,		/* U 05 */
		0x00000000,		/* U 06 */
		0x00000000,		/* U 07 */
		},
		{0x100000000,0x100000000,0,0},		/* DRAM RANK SIZE */
		0x421000,		/* EMI_CONF_VAL */
		0x488005D,		/* CHN0_EMI_CONA_VAL */
		0x488005D,		/* CHN1_EMI_CONA_VAL */
		CBT_R0_R1_NORMAL,	/* dram_cbt_mode_extern */
		{0,0,0,0,0,0},		/* reserved 6 */
		0x00000006,		/* LPDDR4X_MODE_REG5 */
		0,		/* PIN_MUX_TYPE for tablet */
	},
};

#define num_of_emi_records		(sizeof(emi_settings) / sizeof(emi_settings[0]))

#endif /* __CUSTOM_EMI__ */

