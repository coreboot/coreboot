/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2011 Advanced Micro Devices, Inc.
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
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */

#ifndef _CIMX_SB900_CHIP_H_
#define _CIMX_SB900_CHIP_H_

/*
 * configuration set in mainboard/devicetree.cb
 *  boot_switch_sata_ide:
 *   0 -set SATA as primary, PATA(IDE) as secondary.
 *   1 -set PATA(IDE) as primary, SATA as secondary. if you want to boot from IDE,
 *  gpp_configuration - The configuration of General Purpose Port A/B/C/D
 *   0(GPP_CFGMODE_X4000) -PortA Lanes[3:0]
 *   2(GPP_CFGMODE_X2200) -PortA Lanes[1:0], PortB Lanes[3:2]
 *   3(GPP_CFGMODE_X2110) -PortA Lanes[1:0], PortB Lane2, PortC Lane3
 *   4(GPP_CFGMODE_X1111) -PortA Lanes0, PortB Lane1, PortC Lane2, PortD Lane3
 */
struct southbridge_amd_cimx_sb900_config
{
	u32 boot_switch_sata_ide : 1;
	u8  gpp_configuration;
};

#endif /* _CIMX_SB900_CHIP_H_ */
