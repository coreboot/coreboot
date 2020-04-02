/* SPDX-License-Identifier: GPL-2.0-only */
/* This file is part of the coreboot project. */

#ifndef HUDSON_CHIP_H
#define HUDSON_CHIP_H

struct southbridge_amd_pi_hudson_config
{
	u32 ide0_enable : 1;
	u32 sata0_enable : 1;
	u32 boot_switch_sata_ide : 1;
	u32 hda_viddid;
	u8  gpp_configuration;
	u8  sd_mode;
};

#endif /* HUDSON_CHIP_H */
