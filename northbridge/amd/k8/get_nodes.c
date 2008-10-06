/*
 * coherent hypertransport initialization for AMD64 
 * This file is part of the coreboot project.
 *
 * written by Stefan Reinauer <stepan@openbios.org>
 * (c) 2003-2004 by SuSE Linux AG
 *
 * (c) 2004 Tyan Computer
 *  2004.12 yhlu added support to create routing table dynamically.
 *          it also support 8 ways too. (8 ways ladder or 8 ways crossbar)
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
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA, 02110-1301 USA
 */
#include <mainboard.h>
#include <console.h>
#include <mtrr.h>
#include <macros.h>
#include <spd.h>
#include <cpu.h>
#include <msr.h>
#include <amd/k8/k8.h>
#include <amd/k8/sysconf.h>
#include <device/pci.h>
#include <device/hypertransport_def.h>
#include <mc146818rtc.h>
#include <lib.h>

/** 
 * get_nodes
 * see page 46 of the BKDG Publication # 26094       Revision: 3.30 Issue Date: February 2006
 * @returns an int containing the number of nodes.
 * The format of the register is 32 bits, and the node count is in bits 4-6
 */
unsigned int get_nodes(void)
{
        return ((pci_conf1_read_config32(PCI_BDF(0, 0x18, 0), NODEID)>>4) & 7) + 1;
}

