/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <amdblocks/acpimmio.h>
#include <amdblocks/smi.h>
#include <amdblocks/smm.h>
#include <cpu/x86/smm.h>
#include <soc/smi.h>

static void process_smi_sources(uint32_t reg)
{
	const uint32_t status = smi_read32(reg);
	int bit_zero = 32 / sizeof(uint32_t) * (reg - SMI_REG_SMISTS0);
	void (*source_handler)(void);
	int i;

	for (i = 0 ; i < 32 ; i++) {
		if (status & (1 << i)) {
			source_handler = get_smi_source_handler(i + bit_zero);
			if (source_handler)
				source_handler();
		}
	}

	if (reg == SMI_REG_SMISTS0)
		if (status & GEVENT_MASK)
			/* Gevent[23:0] are assumed to be mainboard-specific */
			mainboard_smi_gpi(status & GEVENT_MASK);

	/* Clear all events in this register */
	smi_write32(reg, status);
}

void southbridge_smi_handler(void)
{
	const uint16_t smi_src = smi_read16(SMI_REG_POINTER);

	if (smi_src & SMI_STATUS_SRC_SCI)
		/* Clear events to prevent re-entering SMI if event isn't handled */
		clear_smi_sci_status();
	if (smi_src & SMI_STATUS_SRC_0)
		process_smi_sources(SMI_REG_SMISTS0);
	if (smi_src & SMI_STATUS_SRC_1)
		process_smi_sources(SMI_REG_SMISTS1);
	if (smi_src & SMI_STATUS_SRC_2)
		process_smi_sources(SMI_REG_SMISTS2);
	if (smi_src & SMI_STATUS_SRC_3)
		process_smi_sources(SMI_REG_SMISTS3);
	if (smi_src & SMI_STATUS_SRC_4)
		process_smi_sources(SMI_REG_SMISTS4);
}
