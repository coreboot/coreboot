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

#define HT_INIT_CONTROL		0x6C
#define HTIC_ColdR_Detect  (1<<4)
#define HTIC_BIOSR_Detect  (1<<5)
#define HTIC_INIT_Detect   (1<<6)

#define NODE_HT(x) PCI_DEV(0,24+x,0)
#define NODE_MP(x) PCI_DEV(0,24+x,1)
#define NODE_MC(x) PCI_DEV(0,24+x,3)

void set_bios_reset(void);
void distinguish_cpu_resets(unsigned int nodeid);
unsigned int get_sblk(void);
unsigned int get_sbbusn(unsigned sblk);

#endif /* AMDK8_H */
