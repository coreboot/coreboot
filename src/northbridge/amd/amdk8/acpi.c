/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2005 Advanced Micro Devices, Inc.
 * Copyright (C) 2010 Advanced Micro Devices, Inc.
 *
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

/*
 * Description: Add madt lapic creat dynamically and SRAT related by yhlu
*/

#include <console/console.h>
#include <string.h>
#include <arch/acpi.h>
#include <arch/acpigen.h>
#include <device/pci.h>
#include <cpu/x86/msr.h>
#include <cpu/amd/mtrr.h>
#include <cpu/amd/amdk8_sysconf.h>
#include "acpi.h"

//it seems some functions can be moved arch/x86/boot/acpi.c

unsigned long acpi_create_madt_lapic_nmis(unsigned long current, u16 flags, u8 lint)
{
	device_t cpu;
	int cpu_index = 0;

	for(cpu = all_devices; cpu; cpu = cpu->next) {
		if ((cpu->path.type != DEVICE_PATH_APIC) ||
		    (cpu->bus->dev->path.type != DEVICE_PATH_CPU_CLUSTER)) {
			continue;
		}
		if (!cpu->enabled) {
			continue;
		}
		current += acpi_create_madt_lapic_nmi((acpi_madt_lapic_nmi_t *)current, cpu_index, flags, lint);
		cpu_index++;
	}
	return current;
}

unsigned long acpi_create_srat_lapics(unsigned long current)
{
	device_t cpu;
	int cpu_index = 0;

	for(cpu = all_devices; cpu; cpu = cpu->next) {
		if ((cpu->path.type != DEVICE_PATH_APIC) ||
		    (cpu->bus->dev->path.type != DEVICE_PATH_CPU_CLUSTER)) {
			continue;
		}
		if (!cpu->enabled) {
			continue;
		}
		printk(BIOS_DEBUG, "SRAT: lapic cpu_index=%02x, node_id=%02x, apic_id=%02x\n", cpu_index, cpu->path.apic.node_id, cpu->path.apic.apic_id);
		current += acpi_create_srat_lapic((acpi_srat_lapic_t *)current, cpu->path.apic.node_id, cpu->path.apic.apic_id);
		cpu_index++;
	}
	return current;
}

static unsigned long resk(uint64_t value)
{
	unsigned long resultk;
	if (value < (1ULL << 42)) {
		resultk = value >> 10;
	} else {
		resultk = 0xffffffff;
	}
	return resultk;
}

struct acpi_srat_mem_state {
	unsigned long current;
};

static void set_srat_mem(void *gp, struct device *dev, struct resource *res)
{
	struct acpi_srat_mem_state *state = gp;
	unsigned long basek, sizek;
	basek = resk(res->base);
	sizek = resk(res->size);

	printk(BIOS_DEBUG, "set_srat_mem: dev %s, res->index=%04lx startk=%08lx, sizek=%08lx\n",
		     dev_path(dev), res->index, basek, sizek);
	/*
	 * 0-640K must be on node 0
	 * next range is from 1M---
	 * So will cut off before 1M in the mem range
	 */
	if((basek+sizek)<1024) return;

	if(basek<1024) {
		sizek -= 1024 - basek;
		basek = 1024;
	}

	// need to figure out NV
	state->current += acpi_create_srat_mem((acpi_srat_mem_t *)state->current, (res->index & 0xf), basek, sizek, 1);
}

unsigned long acpi_fill_srat(unsigned long current)
{
	struct acpi_srat_mem_state srat_mem_state;

	/* create all subtables for processors */
	current = acpi_create_srat_lapics(current);

	/* create all subteble for memory range */

	/* 0-640K must be on node 0 */
	current += acpi_create_srat_mem((acpi_srat_mem_t *)current, 0, 0, 640, 1);//enable

	srat_mem_state.current = current;
	search_global_resources(
		IORESOURCE_MEM | IORESOURCE_CACHEABLE, IORESOURCE_MEM | IORESOURCE_CACHEABLE,
		set_srat_mem, &srat_mem_state);

	current = srat_mem_state.current;
	return current;
}

unsigned long acpi_fill_slit(unsigned long current)
{
	/* need to find out the node num at first */
	/* fill the first 8 byte with that num */
	/* fill the next num*num byte with distance, local is 10, 1 hop mean 20, and 2 hop with 30.... */

	/* because We has assume that we know the topology of the HT connection, So we can have set if we know the node_num */
	static u8 hops_8[] = {   0, 1, 1, 2, 2, 3, 3, 4,
				      1, 0, 2, 1, 3, 2, 4, 3,
				      1, 2, 0, 1, 1, 2, 2, 3,
				      2, 1, 1, 0, 2, 1, 3, 2,
				      2, 3, 1, 2, 0, 1, 1, 2,
				      3, 2, 2, 1, 1, 0, 2, 1,
				      3, 4, 2, 3, 1, 2, 0, 1,
				      4, 4, 3, 2, 2, 1, 1, 0 };

//	u8 outer_node[8];

	u8 *p = (u8 *)current;
	int nodes = sysconf.nodes;
	int i,j;
	memset(p, 0, 8+nodes*nodes);
//	memset((u8 *)outer_node, 0, 8);
	*p = (u8) nodes;
	p += 8;

#if 0
	for(i=0;i<sysconf.hc_possible_num;i++) {
		if((sysconf.pci1234[i]&1) !=1 ) continue;
		outer_node[(sysconf.pci1234[i] >> 4) & 0xf] = 1; // mark the outer node
	}
#endif

	for(i=0;i<nodes;i++) {
		for(j=0;j<nodes; j++) {
			if(i==j) {
				p[i*nodes+j] = 10;
			} else {
#if 0
				int k;
				u8 latency_factor = 0;
				int k_start, k_end;
				if(i<j) {
					k_start = i;
					k_end = j;
				} else {
					k_start = j;
					k_end = i;
				}
				for(k=k_start;k<=k_end; k++) {
					if(outer_node[k]) {
						latency_factor = 1;
						break;
					}
				}
				p[i*nodes+j] = hops_8[i*nodes+j] * 2 + latency_factor + 10;
#else
				p[i*nodes+j] = hops_8[i*nodes+j] * 2 + 10;
#endif

			}
		}
	}

	current += 8+nodes*nodes;

	return current;
}

static int k8acpi_write_HT(void) {
	int len, lenp, i;

	len = acpigen_write_name("HCLK");
	lenp = acpigen_write_package(HC_POSSIBLE_NUM);

	for(i=0;i<sysconf.hc_possible_num;i++) {
		lenp += acpigen_write_dword(sysconf.pci1234[i]);
	}
	for(i=sysconf.hc_possible_num; i<HC_POSSIBLE_NUM; i++) { // in case we set array size to other than 8
		lenp += acpigen_write_dword(0x0);
	}

	acpigen_patch_len(lenp - 1);
	len += lenp;

	len += acpigen_write_name("HCDN");
	lenp = acpigen_write_package(HC_POSSIBLE_NUM);

	for(i=0;i<sysconf.hc_possible_num;i++) {
		lenp += acpigen_write_dword(sysconf.hcdn[i]);
	}
	for(i=sysconf.hc_possible_num; i<HC_POSSIBLE_NUM; i++) { // in case we set array size to other than 8
		lenp += acpigen_write_dword(0x20202020);
	}
	acpigen_patch_len(lenp - 1);
	len += lenp;

	return len;
}

static int k8acpi_write_pci_data(int dlen, const char *name, int offset) {
	device_t dev;
	uint32_t dword;
	int len, lenp, i;

	dev = dev_find_slot(0, PCI_DEVFN(0x18, 1));

	len = acpigen_write_name(name);
	lenp = acpigen_write_package(dlen);
	for(i=0; i<dlen; i++) {
		dword = pci_read_config32(dev, offset+i*4);
		lenp += acpigen_write_dword(dword);
	}
	// minus the opcode
	acpigen_patch_len(lenp - 1);
	return len + lenp;
}

int k8acpi_write_vars(void)
{
	int lens;
	msr_t msr;
	char pscope[] = "\\_SB.PCI0";

	lens = acpigen_write_scope(pscope);
	lens += k8acpi_write_pci_data(4, "BUSN", 0xe0);
	lens += k8acpi_write_pci_data(8, "PCIO", 0xc0);
	lens += k8acpi_write_pci_data(16, "MMIO", 0x80);
	lens += acpigen_write_name_byte("SBLK", sysconf.sblk);
	lens += acpigen_write_name_byte("CBST",
	    ((sysconf.pci1234[0] >> 12) & 0xff) ? 0xf : 0x0);
	lens += acpigen_write_name_dword("SBDN", sysconf.sbdn);
	msr = rdmsr(TOP_MEM);
	lens += acpigen_write_name_dword("TOM1", msr.lo);
	msr = rdmsr(TOP_MEM2);
	/*
	 * Since XP only implements parts of ACPI 2.0, we can't use a qword
	 * here.
	 * See http://www.acpi.info/presentations/S01USMOBS169_OS%2520new.ppt
	 * slide 22ff.
	 * Shift value right by 20 bit to make it fit into 32bit,
	 * giving us 1MB granularity and a limit of almost 4Exabyte of memory.
	 */
	lens += acpigen_write_name_dword("TOM2", (msr.hi << 12) | msr.lo >> 20);

	lens += k8acpi_write_HT();
	//minus opcode
	acpigen_patch_len(lens - 1);
	return lens;
}

void update_ssdtx(void *ssdtx, int i)
{
	u8 *PCI;
	u8 *HCIN;
	u8 *UID;

	PCI = ssdtx + 0x32;
	HCIN = ssdtx + 0x39;
	UID = ssdtx + 0x40;

	if (i < 7) {
		*PCI = (u8) ('4' + i - 1);
	} else {
		*PCI = (u8) ('A' + i - 1 - 6);
	}
	*HCIN = (u8) i;
	*UID = (u8) (i + 3);

	/* FIXME: need to update the GSI id in the ssdtx too */

}

