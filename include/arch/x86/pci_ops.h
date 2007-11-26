/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
 */

#ifndef ARCH_X86_PCI_OPS_H
#define ARCH_X86_PCI_OPS_H

extern const struct pci_bus_operations pci_cf8_conf1;
extern const struct pci_bus_operations pci_cf8_conf2;

#if defined(CONFIG_MMCONF_SUPPORT) && (CONFIG_MMCONF_SUPPORT==1)
extern const struct pci_bus_operations pci_ops_mmconf;
#endif

void pci_set_method(struct device * dev);

#endif /* ARCH_X86_PCI_OPS_H */
