/*
	This program is free software; you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation; either version 2 of the License, or
	(at your option) any later version.
	
	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.
	
	You should have received a copy of the GNU General Public License
	along with this program; if not, write to the Free Software
	Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA

*/

#ifndef DEVICE_PATH_H
#define DEVICE_PATH_H

enum device_path_type {
	DEVICE_PATH_NONE = 0,
	DEVICE_PATH_ROOT,
	DEVICE_PATH_PCI_DOMAIN,
	DEVICE_PATH_PCI_BUS, 
	DEVICE_PATH_PCI,
	DEVICE_PATH_PNP,
	DEVICE_PATH_I2C,
	DEVICE_PATH_APIC,
	DEVICE_PATH_APIC_CLUSTER,
	DEVICE_PATH_CPU,
	DEVICE_PATH_CPU_BUS,
	DEVICE_PATH_LPC,
};

struct pci_domain_path
{
	unsigned domain;
};

struct pci_bus_path
{
	unsigned bus;
};

struct pci_path
{
	unsigned devfn;
};

struct pnp_path
{
	unsigned port;
	unsigned device;
};

struct i2c_path
{
	unsigned device;
};

struct apic_path
{
	unsigned apic_id;
	unsigned node_id;
	unsigned core_id;
};

struct apic_cluster_path
{
	unsigned cluster;
};

struct cpu_path
{
	unsigned id;
};

struct cpu_bus_path
{
	unsigned id;
};

struct lpc_path
{
	unsigned iobase;
};


struct device_path {
	enum device_path_type type;
	union {
		struct pci_path		pci;
		struct pnp_path		pnp;
		struct i2c_path		i2c;
		struct apic_path	apic;
		struct pci_domain_path  pci_domain;
		struct pci_bus_path	pci_bus;
		struct apic_cluster_path apic_cluster;
		struct cpu_path		cpu;
		struct cpu_bus_path	cpu_bus;
		struct lpc_path		lpc;
	} u;
};


#define DEVICE_PATH_MAX 30
#define BUS_PATH_MAX (DEVICE_PATH_MAX+10)

extern int path_eq(struct device_path *path1, struct device_path *path2);

#endif /* DEVICE_PATH_H */
