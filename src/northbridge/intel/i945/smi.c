/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2008-2009 coresystems GmbH
 * Copyright (C) 2013 Denis 'GNUtoo' Carikli
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; version 2 of
 * the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston,
 * MA 02110-1301 USA
 */
#include <types.h>
#include <arch/io.h>
#include <cpu/x86/smm.h>
#include "i945.h"

#if CONFIG_SMM_TSEG
static u32 tseg_base = 0;
u32 smi_get_tseg_base(void)
{
        if (!tseg_base)
                tseg_base = pci_read_config8(PCI_DEV(0, 0, 0), TOLUD) -
                        CONFIG_I915_UMA_SIZE - CONFIG_SMM_TSEG_SIZE;
        return tseg_base;
}
#endif

