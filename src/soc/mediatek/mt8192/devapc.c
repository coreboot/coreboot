/* SPDX-License-Identifier: GPL-2.0-only */

#include <soc/devapc.h>

static void *getreg(uintptr_t base, unsigned int offset)
{
	return (void *)(base + offset);
}

static void infra_master_init(uintptr_t base)
{
	/* Sidband */
	SET32_BITFIELDS(getreg(base, MAS_SEC_0), SCP_SSPM_SEC, 1, CPU_EB_SEC, 1);

	/* Domain */
	SET32_BITFIELDS(getreg(base, MAS_DOM_0), PCIE_DOM, MAS_DOMAIN_1);
	SET32_BITFIELDS(getreg(base, MAS_DOM_1), SCP_SSPM_DOM, MAS_DOMAIN_2,
			CPU_EB_DOM, MAS_DOMAIN_2);

	/*
	 * Domain Remap: TINYSYS to non-EMI (3-bit to 4-bit)
	 *    1. SCP from 3 to 3
	 *    2. others from XXX to 15
	 */
	SET32_BITFIELDS(getreg(base, DOM_REMAP_0_0),
			FOUR_BIT_DOM_REMAP_0, MAS_DOMAIN_15,
			FOUR_BIT_DOM_REMAP_1, MAS_DOMAIN_15,
			FOUR_BIT_DOM_REMAP_2, MAS_DOMAIN_15,
			FOUR_BIT_DOM_REMAP_3, MAS_DOMAIN_3,
			FOUR_BIT_DOM_REMAP_4, MAS_DOMAIN_15,
			FOUR_BIT_DOM_REMAP_5, MAS_DOMAIN_15,
			FOUR_BIT_DOM_REMAP_6, MAS_DOMAIN_15,
			FOUR_BIT_DOM_REMAP_7, MAS_DOMAIN_15);

	/*
	 * Domain Remap: MMSYS slave domain remap (4-bit to 2-bit)
	 *    1. From domain 0 ~ 3 to domain 0 ~ 3
	 *    2. others from XXX to domain 0
	 */
	SET32_BITFIELDS(getreg(base, DOM_REMAP_0_0),
			TWO_BIT_DOM_REMAP_0, MAS_DOMAIN_0,
			TWO_BIT_DOM_REMAP_1, MAS_DOMAIN_1,
			TWO_BIT_DOM_REMAP_2, MAS_DOMAIN_2,
			TWO_BIT_DOM_REMAP_3, MAS_DOMAIN_3);

}

static void peri_master_init(uintptr_t base)
{
	/* Domain */
	SET32_BITFIELDS(getreg(base, MAS_DOM_0), SPM_DOM, MAS_DOMAIN_2);
}

static void fmem_master_init(uintptr_t base)
{
	/* Domain Remap: TINYSYS to EMI (3-bit to 4-bit)
	 *    1. SCP from 3 to 3
	 *    2. others from XXX to 15
	 */
	SET32_BITFIELDS(getreg(base, DOM_REMAP_0_0),
			FOUR_BIT_DOM_REMAP_0, MAS_DOMAIN_15,
			FOUR_BIT_DOM_REMAP_1, MAS_DOMAIN_15,
			FOUR_BIT_DOM_REMAP_2, MAS_DOMAIN_15,
			FOUR_BIT_DOM_REMAP_3, MAS_DOMAIN_3,
			FOUR_BIT_DOM_REMAP_4, MAS_DOMAIN_15,
			FOUR_BIT_DOM_REMAP_5, MAS_DOMAIN_15,
			FOUR_BIT_DOM_REMAP_6, MAS_DOMAIN_15,
			FOUR_BIT_DOM_REMAP_7, MAS_DOMAIN_15);
}

struct devapc_init {
	uintptr_t base;
	void (*init)(uintptr_t base);
} devapc_init[DEVAPC_AO_MAX] = {
	{ DEVAPC_INFRA_AO_BASE, infra_master_init },
	{ DEVAPC_PERI_AO_BASE, peri_master_init },
	{ DEVAPC_PERI2_AO_BASE, NULL },
	{ DEVAPC_PERI_PAR_AO_BASE, NULL },
	{ DEVAPC_FMEM_AO_BASE, fmem_master_init },
};

void dapc_init(void)
{
	int i;
	uintptr_t devapc_ao_base;
	void (*init_func)(uintptr_t base);

	for (i = 0; i < ARRAY_SIZE(devapc_init); i++) {
		devapc_ao_base = devapc_init[i].base;
		init_func = devapc_init[i].init;

		/* Init dapc */
		write32(getreg(devapc_ao_base, AO_APC_CON), 0x0);
		write32(getreg(devapc_ao_base, AO_APC_CON), 0x1);

		/* Init master */
		if (init_func)
			init_func(devapc_ao_base);
	}
}
