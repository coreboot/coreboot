/* SPDX-License-Identifier: GPL-2.0-or-later */

/* SMI utilities used in both SMM and normal mode */

#include <console/console.h>
#include <cpu/x86/smm.h>
#include <soc/southbridge.h>
#include <soc/smi.h>
#include <amdblocks/acpimmio.h>
#include <amdblocks/smi.h>

void configure_smi(uint8_t smi_num, uint8_t mode)
{
	uint8_t reg32_offset, bit_offset;
	uint32_t reg32;

	if (smi_num >= NUMBER_SMITYPES) {
		printk(BIOS_WARNING, "BUG: Invalid SMI: %u\n", smi_num);
		return;
	}

	/* 16 sources per register, 2 bits per source; registers are 4 bytes */
	reg32_offset = (smi_num / 16) * 4;
	bit_offset = (smi_num % 16) * 2;

	reg32 = smi_read32(SMI_REG_CONTROL0 + reg32_offset);
	reg32 &= ~(0x3 << (bit_offset));
	reg32 |= (mode & 0x3) << bit_offset;
	smi_write32(SMI_REG_CONTROL0 + reg32_offset, reg32);
}

/**
 * Configure generation of interrupts for given GEVENT pin
 *
 * @param gevent The GEVENT pin number. Valid values are 0 thru 23
 * @param mode The type of event this pin should generate. Note that only
 *	       SMI_MODE_SMI generates an SMI. SMI_MODE_DISABLE disables events.
 * @param level SMI__SCI_LVL_LOW or SMI_SCI_LVL_HIGH
 */
void configure_gevent_smi(uint8_t gevent, uint8_t mode, uint8_t level)
{
	uint32_t reg32;
	/* GEVENT pins range from [0:23] */
	if (gevent >= SMI_GEVENTS) {
		printk(BIOS_WARNING, "BUG: Invalid GEVENT: %u\n", gevent);
		return;
	}

	/* SMI0 source is GEVENT0 and so on */
	configure_smi(gevent, mode);

	/* And set the trigger level */
	reg32 = smi_read32(SMI_REG_SMITRIG0);
	reg32 &= ~(1 << gevent);
	reg32 |= (level & 0x1) << gevent;
	smi_write32(SMI_REG_SMITRIG0, reg32);
}

/** Set the EOS bit and enable SMI generation from southbridge */
void global_smi_enable(void)
{
	uint32_t reg = smi_read32(SMI_REG_SMITRIG0);
	reg &= ~SMITRG0_SMIENB;	/* Enable SMI generation */
	reg |= SMITRG0_EOS;	/* Set EOS bit */
	smi_write32(SMI_REG_SMITRIG0, reg);
}

void southbridge_smi_set_eos(void)
{
	uint32_t reg = smi_read32(SMI_REG_SMITRIG0);
	reg |= SMITRG0_EOS;
	smi_write32(SMI_REG_SMITRIG0, reg);
}

void soc_route_sci(uint8_t event)
{
	smi_write8(SMI_SCI_MAP(event), event);
}

/**
 * Configure generation of SCIs.
 */
void configure_scimap(const struct sci_source *sci)
{
	uint32_t reg32;

	/* GEVENT pins range */
	if (sci->scimap >= SCIMAPS) {
		printk(BIOS_WARNING, "BUG: Invalid SCIMAP: %u\n",
			sci->scimap);
		return;
	}

	/* GPEs range from [0:31] */
	if (sci->gpe >= SCI_GPES) {
		printk(BIOS_WARNING, "BUG: Invalid SCI GPE: %u\n", sci->gpe);
		return;
	}

	printk(BIOS_DEBUG, "SCIMAP %u maps to GPE %u (active %s, %s trigger)\n",
		sci->scimap, sci->gpe,
		(!!sci->direction) ? "high" : "low",
		(!!sci->level) ? "level" : "edge");

	/* Map Gevent to SCI GPE# */
	smi_write8(SMI_SCI_MAP(sci->scimap), sci->gpe);

	/* Set the trigger direction (high/low) */
	reg32 = smi_read32(SMI_SCI_TRIG);
	reg32 &= ~(1 << sci->gpe);
	reg32 |= !!sci->direction << sci->gpe;
	smi_write32(SMI_SCI_TRIG, reg32);

	/* Set the trigger level (edge/level) */
	reg32 = smi_read32(SMI_SCI_LEVEL);
	reg32 &= ~(1 << sci->gpe);
	reg32 |= !!sci->level << sci->gpe;
	smi_write32(SMI_SCI_LEVEL, reg32);
}

void gpe_configure_sci(const struct sci_source *scis, size_t num_gpes)
{
	size_t i;

	for (i = 0; i < num_gpes; i++)
		configure_scimap(scis + i);
}

/** Disable events from given GEVENT pin */
void disable_gevent_smi(uint8_t gevent)
{
	/* GEVENT pins range from [0:23] */
	if (gevent > 23) {
		printk(BIOS_WARNING, "BUG: Invalid GEVENT: %u\n", gevent);
		return;
	}

	/* SMI0 source is GEVENT0 and so on */
	configure_smi(gevent, SMI_MODE_DISABLE);
}

uint16_t pm_acpi_smi_cmd_port(void)
{
	return pm_read16(PM_ACPI_SMI_CMD);
}
