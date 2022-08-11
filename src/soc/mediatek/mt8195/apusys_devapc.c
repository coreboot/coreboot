/* SPDX-License-Identifier: GPL-2.0-only OR MIT */

#include <console/console.h>
#include <soc/apusys_devapc.h>
#include <soc/devapc.h>
#include <soc/devapc_common.h>

static const enum domain_id domain_map[] = {
	DOMAIN_0, DOMAIN_1, DOMAIN_2,  DOMAIN_3,  DOMAIN_4,  DOMAIN_5,	DOMAIN_6,  DOMAIN_7,
	DOMAIN_8, DOMAIN_9, DOMAIN_10, DOMAIN_11, DOMAIN_12, DOMAIN_13, DOMAIN_14, DOMAIN_15,
};

#define DAPC_APU_AO_SYS0_ATTR(...)                                                             \
	{                                                                                      \
		{                                                                              \
			DAPC_PERM_ATTR_16(__VA_ARGS__)                                         \
		}                                                                              \
	}
#define DAPC_APU_NOC_AO_SYS0_ATTR(...)                                                         \
	{                                                                                      \
		{                                                                              \
			DAPC_PERM_ATTR_16(__VA_ARGS__)                                         \
		}                                                                              \
	}

/* NOC DAPC */
static const struct apc_apu_dom_16 apusys_noc_dapc[] = {
	/* 0 */
	DAPC_APU_NOC_AO_SYS0_ATTR("slv00-0", NO_PROTECTION, FORBIDDEN15),
	DAPC_APU_NOC_AO_SYS0_ATTR("slv00-1", NO_PROTECTION, FORBIDDEN15),
	DAPC_APU_NOC_AO_SYS0_ATTR("slv00-2", NO_PROTECTION, FORBIDDEN15),
	DAPC_APU_NOC_AO_SYS0_ATTR("slv01-0", NO_PROTECTION, FORBIDDEN15),
	DAPC_APU_NOC_AO_SYS0_ATTR("slv01-1", NO_PROTECTION, FORBIDDEN15),
	DAPC_APU_NOC_AO_SYS0_ATTR("slv01-2", NO_PROTECTION, FORBIDDEN15),
	DAPC_APU_NOC_AO_SYS0_ATTR("slv03-0", NO_PROTECTION, FORBIDDEN15),
	DAPC_APU_NOC_AO_SYS0_ATTR("slv03-1", NO_PROTECTION, FORBIDDEN15),
	DAPC_APU_NOC_AO_SYS0_ATTR("slv03-2", NO_PROTECTION, FORBIDDEN15),
	DAPC_APU_NOC_AO_SYS0_ATTR("slv03-3", NO_PROTECTION, FORBIDDEN15),

	/* 10 */
	DAPC_APU_NOC_AO_SYS0_ATTR("slv03-4", NO_PROTECTION, FORBIDDEN15),
	DAPC_APU_NOC_AO_SYS0_ATTR("slv04_05_06_07-0", NO_PROTECTION, FORBIDDEN15),
	DAPC_APU_NOC_AO_SYS0_ATTR("slv04_05_06_07-1", NO_PROTECTION, FORBIDDEN15),
	DAPC_APU_NOC_AO_SYS0_ATTR("slv04_05_06_07-2", NO_PROTECTION, FORBIDDEN15),
	DAPC_APU_NOC_AO_SYS0_ATTR("slv04_05_06_07-3", NO_PROTECTION, FORBIDDEN15),
};
_Static_assert(ARRAY_SIZE(apusys_noc_dapc) == APUSYS_NOC_DAPC_AO_SLAVE_NUM,
	       "Wrong size on apusys_noc_dapc");

/* AO DAPC */
static const struct apc_apu_dom_16 apusys_ao_apc[] = {
	/* 0 */
	DAPC_APU_AO_SYS0_ATTR("apusys_ao-0", NO_PROTECTION, FORBIDDEN15),
	DAPC_APU_AO_SYS0_ATTR("apusys_ao-1", NO_PROTECTION, FORBIDDEN15),
	DAPC_APU_AO_SYS0_ATTR("apusys_ao-2", SEC_RW_ONLY, FORBIDDEN15),
	DAPC_APU_AO_SYS0_ATTR("apusys_ao-3", NO_PROTECTION, FORBIDDEN15),
	DAPC_APU_AO_SYS0_ATTR("apusys_ao-4", NO_PROTECTION, FORBIDDEN15),
	DAPC_APU_AO_SYS0_ATTR("apusys_ao-5", NO_PROTECTION, FORBIDDEN15),
	DAPC_APU_AO_SYS0_ATTR("apusys_ao-6", NO_PROTECTION, FORBIDDEN15),
	DAPC_APU_AO_SYS0_ATTR("apusys_ao-8", SEC_RW_ONLY, FORBIDDEN15),
	DAPC_APU_AO_SYS0_ATTR("apusys_ao-9", SEC_RW_ONLY, FORBIDDEN15),
	DAPC_APU_AO_SYS0_ATTR("md32_apb_s-0", NO_PROTECTION, FORBIDDEN15),

	/* 10 */
	DAPC_APU_AO_SYS0_ATTR("md32_apb_s-1", NO_PROTECTION, FORBIDDEN15),
	DAPC_APU_AO_SYS0_ATTR("md32_apb_s-2", NO_PROTECTION, FORBIDDEN15),
	DAPC_APU_AO_SYS0_ATTR("md32_debug_apb", NO_PROTECTION, FORBIDDEN15),
	DAPC_APU_AO_SYS0_ATTR("apu_con2_config", NO_PROTECTION, FORBIDDEN15),
	DAPC_APU_AO_SYS0_ATTR("apu_con1_config", NO_PROTECTION, FORBIDDEN15),
	DAPC_APU_AO_SYS0_ATTR("apu_sctrl_reviscer", SEC_RW_ONLY, FORBIDDEN15),
	DAPC_APU_AO_SYS0_ATTR("apu_sema_stimer", NO_PROTECTION, FORBIDDEN15),
	DAPC_APU_AO_SYS0_ATTR("apu_emi_config", NO_PROTECTION, FORBIDDEN15),
	DAPC_APU_AO_SYS0_ATTR("apu_edma0", NO_PROTECTION, FORBIDDEN15),
	DAPC_APU_AO_SYS0_ATTR("apu_edma1", NO_PROTECTION, FORBIDDEN15),

	/* 20 */
	DAPC_APU_AO_SYS0_ATTR("apu_cpe_sensor", NO_PROTECTION, FORBIDDEN15),
	DAPC_APU_AO_SYS0_ATTR("apu_cpe_coef", NO_PROTECTION, FORBIDDEN15),
	DAPC_APU_AO_SYS0_ATTR("apu_cpe_ctrl", NO_PROTECTION, FORBIDDEN15),
	DAPC_APU_AO_SYS0_ATTR("apu_sensor_wrp_dla_0", NO_PROTECTION, FORBIDDEN15),
	DAPC_APU_AO_SYS0_ATTR("apu_sensor_wrp_dla_1", NO_PROTECTION, FORBIDDEN15),
	DAPC_APU_AO_SYS0_ATTR("apu_dapc_ao", NO_PROTECTION, FORBIDDEN15),
	DAPC_APU_AO_SYS0_ATTR("apu_dapc", NO_PROTECTION, FORBIDDEN15),
	DAPC_APU_AO_SYS0_ATTR("infra_bcrm", NO_PROTECTION, FORBIDDEN15),
	DAPC_APU_AO_SYS0_ATTR("infra_ao_bcrm", NO_PROTECTION, FORBIDDEN15),
	DAPC_APU_AO_SYS0_ATTR("noc_dapc", NO_PROTECTION, FORBIDDEN15),

	/* 30 */
	DAPC_APU_AO_SYS0_ATTR("apu_noc_bcrm", NO_PROTECTION, FORBIDDEN15),
	DAPC_APU_AO_SYS0_ATTR("apu_noc_config_0", NO_PROTECTION, FORBIDDEN, FORBIDDEN,
			      NO_PROTECTION, FORBIDDEN12),
	DAPC_APU_AO_SYS0_ATTR("apu_noc_config_1", NO_PROTECTION, FORBIDDEN, FORBIDDEN,
			      NO_PROTECTION, FORBIDDEN12),
	DAPC_APU_AO_SYS0_ATTR("apu_noc_config_2", NO_PROTECTION, FORBIDDEN, FORBIDDEN,
			      NO_PROTECTION, FORBIDDEN12),
	DAPC_APU_AO_SYS0_ATTR("vpu_core0_config-0", NO_PROTECTION, FORBIDDEN, FORBIDDEN3,
			      NO_PROTECTION, FORBIDDEN3, FORBIDDEN7),
	DAPC_APU_AO_SYS0_ATTR("vpu_core0_config-1", NO_PROTECTION, FORBIDDEN, FORBIDDEN3,
			      NO_PROTECTION, FORBIDDEN3, FORBIDDEN7),
	DAPC_APU_AO_SYS0_ATTR("vpu_core1_config-0", NO_PROTECTION, FORBIDDEN, FORBIDDEN3,
			      NO_PROTECTION, FORBIDDEN3, FORBIDDEN7),
	DAPC_APU_AO_SYS0_ATTR("vpu_core1_config-1", NO_PROTECTION, FORBIDDEN, FORBIDDEN3,
			      NO_PROTECTION, FORBIDDEN3, FORBIDDEN7),
	DAPC_APU_AO_SYS0_ATTR("mdla0_apb-0", NO_PROTECTION, FORBIDDEN15),
	DAPC_APU_AO_SYS0_ATTR("mdla0_apb-1", NO_PROTECTION, FORBIDDEN15),

	/* 40 */
	DAPC_APU_AO_SYS0_ATTR("mdla0_apb-2", NO_PROTECTION, FORBIDDEN15),
	DAPC_APU_AO_SYS0_ATTR("mdla0_apb-3", NO_PROTECTION, FORBIDDEN15),
	DAPC_APU_AO_SYS0_ATTR("mdla1_apb-0", NO_PROTECTION, FORBIDDEN15),
	DAPC_APU_AO_SYS0_ATTR("mdla1_apb-1", NO_PROTECTION, FORBIDDEN15),
	DAPC_APU_AO_SYS0_ATTR("mdla1_apb-2", NO_PROTECTION, FORBIDDEN15),
	DAPC_APU_AO_SYS0_ATTR("mdla1_apb-3", NO_PROTECTION, FORBIDDEN15),
	DAPC_APU_AO_SYS0_ATTR("apu_iommu0_r0", NO_PROTECTION, FORBIDDEN15),
	DAPC_APU_AO_SYS0_ATTR("apu_iommu0_r1", SEC_RW_ONLY, FORBIDDEN15),
	DAPC_APU_AO_SYS0_ATTR("apu_iommu0_r2", SEC_RW_ONLY, FORBIDDEN15),
	DAPC_APU_AO_SYS0_ATTR("apu_iommu0_r3", SEC_RW_ONLY, FORBIDDEN15),

	/* 50 */
	DAPC_APU_AO_SYS0_ATTR("apu_iommu0_r4", SEC_RW_ONLY, FORBIDDEN15),
	DAPC_APU_AO_SYS0_ATTR("apu_iommu1_r0", NO_PROTECTION, FORBIDDEN15),
	DAPC_APU_AO_SYS0_ATTR("apu_iommu1_r1", SEC_RW_ONLY, FORBIDDEN15),
	DAPC_APU_AO_SYS0_ATTR("apu_iommu1_r2", SEC_RW_ONLY, FORBIDDEN15),
	DAPC_APU_AO_SYS0_ATTR("apu_iommu1_r3", SEC_RW_ONLY, FORBIDDEN15),
	DAPC_APU_AO_SYS0_ATTR("apu_iommu1_r4", SEC_RW_ONLY, FORBIDDEN15),
	DAPC_APU_AO_SYS0_ATTR("apu_rsi2_config", NO_PROTECTION, FORBIDDEN15),
	DAPC_APU_AO_SYS0_ATTR("apu_s0_ssc_config", NO_PROTECTION, FORBIDDEN15),
	DAPC_APU_AO_SYS0_ATTR("apu_n0_ssc_config", NO_PROTECTION, FORBIDDEN15),
	DAPC_APU_AO_SYS0_ATTR("apu_acp_ssc_config", NO_PROTECTION, FORBIDDEN15),

	/* 60 */
	DAPC_APU_AO_SYS0_ATTR("apu_s1_ssc_config", NO_PROTECTION, FORBIDDEN15),
	DAPC_APU_AO_SYS0_ATTR("apu_n1_ssc_config", NO_PROTECTION, FORBIDDEN15),
	DAPC_APU_AO_SYS0_ATTR("apu_ao_dbgapb-0", NO_PROTECTION, FORBIDDEN15),
	DAPC_APU_AO_SYS0_ATTR("apu_ao_dbgapb-1", NO_PROTECTION, FORBIDDEN15),
	DAPC_APU_AO_SYS0_ATTR("apu_ao_dbgapb-2", NO_PROTECTION, FORBIDDEN15),
	DAPC_APU_AO_SYS0_ATTR("apu_ao_dbgapb-3", NO_PROTECTION, FORBIDDEN15),
	DAPC_APU_AO_SYS0_ATTR("apu_ao_dbgapb-4", NO_PROTECTION, FORBIDDEN15),
	DAPC_APU_AO_SYS0_ATTR("apu_ao_dbgapb-5", NO_PROTECTION, FORBIDDEN15),
	DAPC_APU_AO_SYS0_ATTR("vpu_core0_debug_apb", NO_PROTECTION, FORBIDDEN15),
	DAPC_APU_AO_SYS0_ATTR("vpu_core1_debug_apb", NO_PROTECTION, FORBIDDEN15),

	/* 70 */
	DAPC_APU_AO_SYS0_ATTR("apb_infra_dbg_ctl", NO_PROTECTION, FORBIDDEN15),
};
_Static_assert(ARRAY_SIZE(apusys_ao_apc) == APUSYS_APC_SYS0_AO_SLAVE_NUM,
	       "Wrong size on apusys_ao_apc");

static int set_slave_noc_dapc(u32 slave, enum domain_id domain_id, enum devapc_perm_type perm)
{
	u32 apc_register_index;
	u32 apc_set_index;
	u32 *base;

	if (perm >= PERM_NUM || perm < 0) {
		printk(BIOS_ERR, "[NOC_DAPC] permission type:%#x is not supported!\n", perm);
		return APUSYS_APC_ERR_PERMISSION_NOT_SUPPORTED;
	}

	if (slave >= APUSYS_NOC_DAPC_AO_SLAVE_NUM || domain_id >= APUSYS_NOC_DAPC_AO_DOM_NUM) {
		printk(BIOS_ERR, "[NOC_DAPC] %s: %s, %s:%#x, %s:%#x\n", __func__,
		       "out of boundary", "slave", slave, "domain_id", domain_id);
		return APUSYS_APC_ERR_OUT_OF_BOUNDARY;
	}

	apc_register_index = slave / APUSYS_NOC_DAPC_AO_SLAVE_NUM_IN_1_DOM;
	apc_set_index = slave % APUSYS_NOC_DAPC_AO_SLAVE_NUM_IN_1_DOM;
	base = (void *)((size_t)APUSYS_NOC_DAPC_AO_BASE + domain_id * 0x40 +
			apc_register_index * 4);
	clrsetbits32(base, 0x3U << (apc_set_index * 2), perm << (apc_set_index * 2));

	return APUSYS_APC_OK;
}

static void dump_apusys_noc_dapc(void)
{
	u32 reg_num;
	size_t d, i;

	reg_num = DIV_ROUND_UP(ARRAY_SIZE(apusys_noc_dapc),
			       APUSYS_NOC_DAPC_AO_SLAVE_NUM_IN_1_DOM);
	for (d = 0U; d < APUSYS_NOC_DAPC_AO_DOM_NUM; d++) {
		for (i = 0U; i < reg_num; i++)
			printk(BIOS_DEBUG, "[NOCDAPC] D%ld_APC_%ld: %#x\n", d, i,
			       read32((void *)(APUSYS_NOC_DAPC_AO_BASE + d * 0x40 + i * 4)));
	}
	printk(BIOS_DEBUG, "[NOCDAPC] APC_CON: %#x\n", read32(APUSYS_NOC_DAPC_CON));
}

static int set_slave_apc(u32 slave, enum domain_id domain_id, enum devapc_perm_type perm)
{
	u32 apc_register_index;
	u32 apc_set_index;
	u32 *base;

	if (perm >= PERM_NUM || perm < 0) {
		printk(BIOS_ERR, "[APUAPC] perm type:%#x is not supported!\n", perm);
		return APUSYS_APC_ERR_PERMISSION_NOT_SUPPORTED;
	}

	if (slave >= APUSYS_APC_SYS0_AO_SLAVE_NUM || domain_id >= APUSYS_APC_SYS0_AO_DOM_NUM) {
		printk(BIOS_ERR, "[APUAPC] %s: %s, %s:%#x, %s:%#x\n", __func__,
		       "out of boundary", "slave", slave, "domain_id", domain_id);
		return APUSYS_APC_ERR_OUT_OF_BOUNDARY;
	}

	apc_register_index = slave / APUSYS_APC_SYS0_AO_SLAVE_NUM_IN_1_DOM;
	apc_set_index = slave % APUSYS_APC_SYS0_AO_SLAVE_NUM_IN_1_DOM;
	base = (void *)((size_t)APUSYS_APC_AO_BASE + domain_id * 0x40 + apc_register_index * 4);

	clrsetbits32(base, 0x3U << (apc_set_index * 2), perm << (apc_set_index * 2));

	return APUSYS_APC_OK;
}

static void dump_apusys_ao_apc(void)
{
	u32 reg_num;
	size_t d, i;

	reg_num =
		DIV_ROUND_UP(ARRAY_SIZE(apusys_ao_apc), APUSYS_APC_SYS0_AO_SLAVE_NUM_IN_1_DOM);
	for (d = 0U; d < APUSYS_APC_SYS0_AO_DOM_NUM; d++) {
		for (i = 0U; i < reg_num; i++)
			printk(BIOS_DEBUG, "[APUAPC] D%ld_APC_%ld: %#x\n", d, i,
			       read32((void *)(APUSYS_APC_AO_BASE + d * 0x40 + i * 4)));
	}
	printk(BIOS_DEBUG, "[APUAPC] APC_CON: %#x\n", read32(APUSYS_APC_CON));
}

static int set_apusys_noc_dapc(void)
{
	int ret = APUSYS_APC_OK;
	size_t i, j;

	for (i = 0; i < ARRAY_SIZE(apusys_noc_dapc); i++) {
		for (j = 0; j < ARRAY_SIZE(apusys_noc_dapc[i].d_permission); j++) {
			ret = set_slave_noc_dapc(i, j, apusys_noc_dapc[i].d_permission[j]);
			if (ret)
				printk(BIOS_ERR, "[APUAPC] fail (%ld, %ld)(%d)!\n", i, j, ret);
		}
	}

	return ret;
}

static int32_t set_apusys_ao_apc(void)
{
	int ret = APUSYS_APC_OK;
	size_t i, j;

	for (i = 0; i < ARRAY_SIZE(apusys_ao_apc); i++) {
		for (j = 0; j < ARRAY_SIZE(apusys_ao_apc[j].d_permission); j++) {
			ret = set_slave_apc(i, domain_map[j], apusys_ao_apc[i].d_permission[j]);
			if (ret)
				printk(BIOS_ERR, "[APUAPC] fail (%ld, %ld)(%d)!\n", i, j, ret);
		}
	}

	return ret;
}

void start_apusys_devapc(void)
{
	int ret = APUSYS_APC_OK;

	/* Check violation status */
	printk(BIOS_DEBUG, "[APUAPC] vio %d\n", read32(APUSYS_APC_CON) & 0x80000000);

	/* Initial Permission */
	ret = set_apusys_ao_apc();
	printk(BIOS_DEBUG, "[APUAPC] %s - %s!\n", "set_apusys_ao_apc",
	       ret ? "FAILED" : "SUCCESS");

	/* Lock */
	write32(APUSYS_SYS0_APC_LOCK_0, APU_SCTRL_REVISER | DEVAPC_AO_WRAPPER);

	/* Initial NoC Permission */
	ret = set_apusys_noc_dapc();
	printk(BIOS_DEBUG, "[APUAPC] %s - %s!\n", "set_apusys_noc_dapc",
	       ret ? "FAILED" : "SUCCESS");

	dump_apusys_ao_apc();
	dump_apusys_noc_dapc();

	printk(BIOS_DEBUG, "[APUAPC] %s done\n", __func__);
}
