/* SPDX-License-Identifier: GPL-2.0-only */

#include <arch/io.h>
#include "cpld.h"

/* CPLD definitions */
#define CPLD_PCB_VERSION_PORT 0x283
#define CPLD_PCB_VERSION_MASK 0xF0
#define CPLD_PCB_VERSION_BIT 4

#define CPLD_RESET_PORT 0x287
#define CPLD_CMD_RESET_DSI_BRIDGE_ACTIVE 0x20
#define CPLD_CMD_RESET_DSI_BRIDGE_INACTIVE 0x00

/* Reset DSI bridge */
void cpld_reset_bridge(void)
{
	outb(CPLD_CMD_RESET_DSI_BRIDGE_ACTIVE, CPLD_RESET_PORT);
	outb(CPLD_CMD_RESET_DSI_BRIDGE_INACTIVE, CPLD_RESET_PORT);
}

/* Read PCB version */
unsigned int cpld_read_pcb_version(void)
{
	return ((inb(CPLD_PCB_VERSION_PORT) & CPLD_PCB_VERSION_MASK) >> CPLD_PCB_VERSION_BIT);
}
