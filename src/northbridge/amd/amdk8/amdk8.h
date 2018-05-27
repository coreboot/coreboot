/*
 * This file is part of the coreboot project.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#ifndef AMDK8_H

#define AMDK8_H

#if IS_ENABLED(CONFIG_K8_REV_F_SUPPORT)

#define K8_REV_F_SUPPORT_F0_F1_WORKAROUND 0

	#include "f.h"
#else
	#include "pre_f.h"
#endif

#define HTIC_ColdR_Detect  (1<<4)
#define HTIC_BIOSR_Detect  (1<<5)
#define HTIC_INIT_Detect   (1<<6)

#define NODE_HT(x) PCI_DEV(0,24+x,0)
#define NODE_MP(x) PCI_DEV(0,24+x,1)
#define NODE_MC(x) PCI_DEV(0,24+x,3)


#ifdef __PRE_RAM__
void showallroutes(int level, pci_devfn_t dev);
void setup_resource_map_offset(const unsigned int *register_values, int max, unsigned offset_pci_dev, unsigned offset_io_base);
void fill_mem_ctrl(int controllers, struct mem_controller *ctrl_a, const uint16_t *spd_addr);
int optimize_link_coherent_ht(void);
unsigned int get_nodes(void);
#if IS_ENABLED(CONFIG_RAMINIT_SYSINFO)
void setup_coherent_ht_domain(void);
#else
int setup_coherent_ht_domain(void);
#endif
#endif

void set_bios_reset(void);
void distinguish_cpu_resets(unsigned int nodeid);
unsigned int get_sblk(void);
unsigned int get_sbbusn(unsigned sblk);

void cpus_ready_for_init(void);

#endif /* AMDK8_H */
