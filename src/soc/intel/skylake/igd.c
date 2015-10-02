/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2014 Google Inc.
 * Copyright (C) 2015 Intel Corporation.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc.
 */

#include <arch/acpi.h>
#include <arch/io.h>
#include <chip.h>
#include <console/console.h>
#include <delay.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <drivers/intel/gma/i915_reg.h>
#include <fsp/gop.h>
#include <soc/acpi.h>
#include <soc/cpu.h>
#include <soc/pm.h>
#include <soc/ramstage.h>
#include <soc/systemagent.h>
#include <stdlib.h>
#include <string.h>
#include <vendorcode/google/chromeos/chromeos.h>

u32 map_oprom_vendev(u32 vendev)
{
	return SA_IGD_OPROM_VENDEV;
}

static struct resource *gtt_res = NULL;

static unsigned long gtt_read(unsigned long reg)
{
	u32 val;
	val = read32((void *)(unsigned int)(gtt_res->base + reg));
	return val;
}

static void gtt_write(unsigned long reg, unsigned long data)
{
	write32((void *)(unsigned int)(gtt_res->base + reg), data);
}

static inline void gtt_rmw(u32 reg, u32 andmask, u32 ormask)
{
	u32 val = gtt_read(reg);
	val &= andmask;
	val |= ormask;
	gtt_write(reg, val);
}

static void igd_init(struct device *dev)
{
	if (IS_ENABLED(CONFIG_GOP_SUPPORT))
		return;

	/* IGD needs to be Bus Master */
	u32 reg32 = pci_read_config32(dev, PCI_COMMAND);
	reg32 |= PCI_COMMAND_MASTER | PCI_COMMAND_MEMORY | PCI_COMMAND_IO;
	pci_write_config32(dev, PCI_COMMAND, reg32);

	gtt_res = find_resource(dev, PCI_BASE_ADDRESS_0);
	if (!gtt_res || !gtt_res->base)
		return;

	/* Wait for any configured pre-graphics delay */
	if (!acpi_is_wakeup_s3()) {
#if IS_ENABLED(CONFIG_CHROMEOS)
		if (developer_mode_enabled() || recovery_mode_enabled() ||
		    vboot_wants_oprom())
			mdelay(CONFIG_PRE_GRAPHICS_DELAY);
#else
		mdelay(CONFIG_PRE_GRAPHICS_DELAY);
#endif
	}

	/* Initialize PCI device, load/execute BIOS Option ROM */
	pci_dev_init(dev);

#if IS_ENABLED(CONFIG_CHROMEOS)
	if (!gfx_get_init_done() && !acpi_is_wakeup_s3()) {
		/*
		 * Enable DDI-A if the Option ROM did not execute:
		 *
		 * bit 0: Display detected (RO)
		 * bit 4: DDI A supports 4 lanes and DDI E is not used
		 * bit 7: DDI buffer is idle
		 */
		gtt_write(DDI_BUF_CTL_A, DDI_BUF_IS_IDLE | DDI_A_4_LANES |
			  DDI_INIT_DISPLAY_DETECTED);
	}
#endif
}

/* Initialize IGD OpRegion, called from ACPI code */
static int init_igd_opregion(igd_opregion_t *opregion)
{
	const optionrom_vbt_t *vbt;
	uint32_t vbt_len;
	u16 reg16;

	memset(opregion, 0, sizeof(igd_opregion_t));

	/* Read VBT table from flash */
	vbt = fsp_get_vbt(&vbt_len);
	if (!vbt)
		die("vbt data not found");

	memcpy(&opregion->header.signature, IGD_OPREGION_SIGNATURE,
		sizeof(IGD_OPREGION_SIGNATURE));
	memcpy(opregion->header.vbios_version, vbt->coreblock_biosbuild, sizeof(u32));
	memcpy(opregion->vbt.gvd1, vbt, vbt->hdr_vbt_size <
		sizeof(opregion->vbt.gvd1) ? vbt->hdr_vbt_size :
		sizeof(opregion->vbt.gvd1));

	/* Size, in KB, of the entire OpRegion structure (including header)*/
	opregion->header.size = sizeof(igd_opregion_t) / KiB;
	opregion->header.version = IGD_OPREGION_VERSION;

	/* We just assume we're mobile for now */
	opregion->header.mailboxes = MAILBOXES_MOBILE;

	/* TODO Initialize Mailbox 1 */

	/* Initialize Mailbox 3 */
	opregion->mailbox3.bclp = IGD_BACKLIGHT_BRIGHTNESS;
	opregion->mailbox3.pfit = IGD_FIELD_VALID | IGD_PFIT_STRETCH;
	opregion->mailbox3.pcft = 0; /* should be (IMON << 1) & 0x3e */
	opregion->mailbox3.cblv = IGD_FIELD_VALID | IGD_INITIAL_BRIGHTNESS;
	opregion->mailbox3.bclm[0] = IGD_WORD_FIELD_VALID + 0x0000;
	opregion->mailbox3.bclm[1] = IGD_WORD_FIELD_VALID + 0x0a19;
	opregion->mailbox3.bclm[2] = IGD_WORD_FIELD_VALID + 0x1433;
	opregion->mailbox3.bclm[3] = IGD_WORD_FIELD_VALID + 0x1e4c;
	opregion->mailbox3.bclm[4] = IGD_WORD_FIELD_VALID + 0x2866;
	opregion->mailbox3.bclm[5] = IGD_WORD_FIELD_VALID + 0x327f;
	opregion->mailbox3.bclm[6] = IGD_WORD_FIELD_VALID + 0x3c99;
	opregion->mailbox3.bclm[7] = IGD_WORD_FIELD_VALID + 0x46b2;
	opregion->mailbox3.bclm[8] = IGD_WORD_FIELD_VALID + 0x50cc;
	opregion->mailbox3.bclm[9] = IGD_WORD_FIELD_VALID + 0x5ae5;
	opregion->mailbox3.bclm[10] = IGD_WORD_FIELD_VALID + 0x64ff;

	/* TODO This may need to happen in S3 resume */
	pci_write_config32(SA_DEV_IGD, ASLS, (u32)opregion);
	reg16 = pci_read_config16(SA_DEV_IGD, SWSCI);
	reg16 &= ~GSSCIE;
	reg16 |= SMISCISEL;
	pci_write_config16(SA_DEV_IGD, SWSCI, reg16);

	return 0;
}

static unsigned long write_acpi_igd_opregion(device_t device,
				unsigned long current, struct acpi_rsdp *rsdp)
{
	igd_opregion_t *opregion;

	/* If GOP is not used, exit here */
	if (!IS_ENABLED(CONFIG_GOP_SUPPORT))
		return current;

	/* If IGD is disabled, exit here */
	if (pci_read_config16(device, PCI_VENDOR_ID) == 0xFFFF)
		return current;

	printk(BIOS_DEBUG, "ACPI: * IGD OpRegion\n");
	opregion = (igd_opregion_t *)current;
	init_igd_opregion(opregion);
	current += sizeof(igd_opregion_t);
	ALIGN_CURRENT;

	printk(BIOS_DEBUG, "current = %lx\n", current);
	return current;
}

static struct device_operations igd_ops = {
	.read_resources		= &pci_dev_read_resources,
	.set_resources		= &pci_dev_set_resources,
	.enable_resources	= &pci_dev_enable_resources,
	.init			= &igd_init,
	.ops_pci		= &soc_pci_ops,
	.write_acpi_tables	= write_acpi_igd_opregion,
};

static const unsigned short pci_device_ids[] = {
	IGD_SKYLAKE_GT1_SULTM,
	IGD_SKYLAKE_GT2_SULXM,
	IGD_SKYLAKE_GT2_SULTM,
	0,
};

static const struct pci_driver igd_driver __pci_driver = {
	.ops	 = &igd_ops,
	.vendor	 = PCI_VENDOR_ID_INTEL,
	.devices = pci_device_ids,
};
