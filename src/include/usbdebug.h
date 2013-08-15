/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2007 AMD
 * Written by Yinghai Lu <yinghai.lu@amd.com> for AMD.
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

#ifndef USBDEBUG_H
#define USBDEBUG_H

#define EHCI_BAR_INDEX		0x10
#define PCI_EHCI_CLASSCODE 	0x0c0320	/* USB2.0 with EHCI controller */

typedef u32 pci_devfn_t;
pci_devfn_t pci_ehci_dbg_dev(unsigned hcd_idx);
unsigned long pci_ehci_base_regs(pci_devfn_t dev);
void pci_ehci_dbg_set_port(pci_devfn_t dev, unsigned int port);
void pci_ehci_dbg_enable(pci_devfn_t dev, unsigned long base);

#ifndef __PRE_RAM__
#if !CONFIG_USBDEBUG
#define pci_ehci_read_resources pci_dev_read_resources
#else
/* Relocation of EHCI Debug Port BAR
 *
 * PCI EHCI controller with Debug Port capability shall replace
 * pci_dev_read_resources() with pci_ehci_read_resources() in its
 * declaration of device_operations for .read_resources.
 * This installs a hook to reconfigure usbdebug when resource allocator
 * assigns a new BAR for the device.
 */
struct device;
void pci_ehci_read_resources(struct device *dev);
#endif
#endif

struct dbgp_pipe;

int usbdebug_init(void);

struct dbgp_pipe *dbgp_console_output(void);
struct dbgp_pipe *dbgp_console_input(void);
int dbgp_ep_is_active(struct dbgp_pipe *pipe);
int dbgp_bulk_write_x(struct dbgp_pipe *pipe, const char *bytes, int size);
int dbgp_bulk_read_x(struct dbgp_pipe *pipe, void *data, int size);
void usbdebug_tx_byte(struct dbgp_pipe *pipe, unsigned char data);
void usbdebug_tx_flush(struct dbgp_pipe *pipe);
#endif
