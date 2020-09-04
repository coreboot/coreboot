/* SPDX-License-Identifier: GPL-2.0-only */

#include <console/console.h>
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
	SET32_BITFIELDS(getreg(base, MAS_SEC_0), PCIE_DOM, MAS_DOMAIN_1);
	SET32_BITFIELDS(getreg(base, MAS_DOM_1), SCP_SSPM_DOM, MAS_DOMAIN_2,
			CPU_EB_DOM, MAS_DOMAIN_2);
}

static void peri_master_init(uintptr_t base)
{
	/* Domain */
	SET32_BITFIELDS(getreg(base, MAS_DOM_0), SPM_DOM, MAS_DOMAIN_2);
}

static uintptr_t devapc_base[DEVAPC_AO_MAX] = {
	DEVAPC_INFRA_AO_BASE,
	DEVAPC_PERI_AO_BASE,
	DEVAPC_PERI2_AO_BASE,
	DEVAPC_PERI_PAR_AO_BASE,
	DEVAPC_FMEM_AO_BASE,
};

static void (*master_init[DEVAPC_AO_MAX])(uintptr_t) = {
	infra_master_init,
	peri_master_init,
};

void dapc_init(void)
{
	int i;
	uintptr_t devapc_ao_base;

	for (i = 0; i < ARRAY_SIZE(devapc_base); i++) {
		devapc_ao_base = devapc_base[i];

		/* Init dapc */
		write32(getreg(devapc_ao_base, AO_APC_CON), 0x0);
		write32(getreg(devapc_ao_base, AO_APC_CON), 0x1);

		/* Init master */
		if (master_init[i])
			master_init[i](devapc_ao_base);
	}
}
