/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <console/console.h>
#include <device/pci_ops.h>
#include <southbridge/intel/lynxpoint/pch.h>
#include <types.h>

void pch_dmi_setup_physical_layer(void)
{
	/** FIXME: We need to make sure the SA supports Gen2 as well **/
	if ((RCBA32(0x21a4) & 0x0f) == 0x02) {
		/* Set Gen 2 Common Clock N_FTS */
		RCBA32_AND_OR(0x2340, ~0x00ff0000, 0x3a << 16);

		/* Set Target Link Speed to DMI Gen2 */
		RCBA8_AND_OR(DLCTL2, ~0x07, 0x02);
	}
}

#define VC_ACTIVE	(1U << 31)

#define VCNEGPND	(1 << 1)

void pch_dmi_tc_vc_mapping(const u32 vc0, const u32 vc1, const u32 vcp, const u32 vcm)
{
	printk(BIOS_DEBUG, "Programming PCH DMI VC/TC mappings...\n");

	RCBA32_AND_OR(CIR0050, ~(0xf << 20), 2 << 20);
	if (vcp & VC_ACTIVE)
		RCBA32_OR(CIR0050, 1 << 19 | 1 << 17);

	RCBA32(CIR0050);	/* Ensure posted write hits */

	/* Use the same virtual channel mapping on both ends of the DMI link */
	RCBA32(V0CTL) = vc0;
	RCBA32(V1CTL) = vc1;
	RCBA32(V1CTL);		/* Ensure posted write hits */
	RCBA32(VPCTL) = vcp;
	RCBA32(VPCTL);		/* Ensure posted write hits */
	RCBA32(VMCTL) = vcm;

	/* Lock the registers */
	RCBA32_OR(CIR0050, 1U << 31);
	RCBA32(CIR0050);	/* Ensure posted write hits */

	printk(BIOS_DEBUG, "Waiting for PCH DMI VC negotiation... ");
	do {} while (RCBA16(V0STS) & VCNEGPND);
	do {} while (RCBA16(V1STS) & VCNEGPND);
	do {} while (RCBA16(VPSTS) & VCNEGPND);
	do {} while (RCBA16(VMSTS) & VCNEGPND);
	printk(BIOS_DEBUG, "done!\n");
}
