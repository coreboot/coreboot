/* SPDX-License-Identifier: GPL-2.0-only */

#include <stdint.h>
#include <arch/io.h>
#include <cpu/x86/smm.h>
#include <superio/smsc/sch5545/sch5545.h>

int mainboard_smi_apmc(u8 data)
{
	u8 val;
	switch (data) {
	case APM_CNT_ACPI_ENABLE:
		/* Enable wake on PS2 */
		val = inb(SCH5545_RUNTIME_REG_BASE + SCH5545_RR_PME_EN1);
		val |= (SCH5545_KBD_PME_EN | SCH5545_MOUSE_PME_EN);
		outb(val, SCH5545_RUNTIME_REG_BASE + SCH5545_RR_PME_EN1);
		/* Clear pending and enable PMEs */
		outb(SCH5545_GLOBAL_PME_STS, SCH5545_RUNTIME_REG_BASE + SCH5545_RR_PME_STS);
		outb(SCH5545_GLOBAL_PME_EN, SCH5545_RUNTIME_REG_BASE + SCH5545_RR_PME_EN);
		break;
	case APM_CNT_ACPI_DISABLE:
		/* Disable wake on PS2 */
		val = inb(SCH5545_RUNTIME_REG_BASE + SCH5545_RR_PME_EN1);
		val &= ~(SCH5545_KBD_PME_EN | SCH5545_MOUSE_PME_EN);
		outb(val, SCH5545_RUNTIME_REG_BASE + SCH5545_RR_PME_EN1);
		/* Clear pending and disable PMEs */
		outb(SCH5545_GLOBAL_PME_STS, SCH5545_RUNTIME_REG_BASE + SCH5545_RR_PME_STS);
		outb(0, SCH5545_RUNTIME_REG_BASE + SCH5545_RR_PME_EN);
		break;
	default:
		break;
	}
	return 0;
}
