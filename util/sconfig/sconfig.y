%{
/*
 * sconfig, coreboot device tree compiler
 *
 * Copyright (C) 2010 coresystems GmbH
 *                 written by Patrick Georgi <patrick.georgi@coresystems.de>
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

#include "sconfig.h"

static struct device *cur_parent, *cur_bus;

%}
%union {
	struct device *device;
	char *string;
	int number;
}

%token CHIP DEVICE REGISTER BOOL BUS RESOURCE END EQUALS HEX STRING PCI PNP I2C APIC CPU_CLUSTER DOMAIN IRQ DRQ IO NUMBER SUBSYSTEMID INHERIT IOAPIC_IRQ IOAPIC PCIINT
%%
devtree: { cur_parent = cur_bus = head; } chip { postprocess_devtree(); } ;

chipchildren: chipchildren device | chipchildren chip | chipchildren registers | /* empty */ ;

devicechildren: devicechildren device | devicechildren chip | devicechildren resource | devicechildren subsystemid | devicechildren ioapic_irq | /* empty */ ;

chip: CHIP STRING /* == path */ {
	$<device>$ = new_chip(cur_parent, cur_bus, $<string>2);
	cur_parent = $<device>$;
}
	chipchildren END {
	cur_parent = $<device>3->parent;
	fold_in($<device>3);
	add_header($<device>3);
};

device: DEVICE BUS NUMBER /* == devnum */ BOOL {
	$<device>$ = new_device(cur_parent, cur_bus, $<number>2, $<string>3, $<number>4);
	cur_parent = $<device>$;
	cur_bus = $<device>$;
}
	devicechildren END {
	cur_parent = $<device>5->parent;
	cur_bus = $<device>5->bus;
	fold_in($<device>5);
	alias_siblings($<device>5->children);
};

resource: RESOURCE NUMBER /* == resnum */ EQUALS NUMBER /* == resval */
	{ add_resource(cur_parent, $<number>1, strtol($<string>2, NULL, 0), strtol($<string>4, NULL, 0)); } ;

registers: REGISTER STRING /* == regname */ EQUALS STRING /* == regval */
	{ add_register(cur_parent, $<string>2, $<string>4); } ;

subsystemid: SUBSYSTEMID NUMBER NUMBER
	{ add_pci_subsystem_ids(cur_parent, strtol($<string>2, NULL, 16), strtol($<string>3, NULL, 16), 0); };

subsystemid: SUBSYSTEMID NUMBER NUMBER INHERIT
	{ add_pci_subsystem_ids(cur_parent, strtol($<string>2, NULL, 16), strtol($<string>3, NULL, 16), 1); };

ioapic_irq: IOAPIC_IRQ NUMBER PCIINT NUMBER
	{ add_ioapic_info(cur_parent, strtol($<string>2, NULL, 16), $<string>3, strtol($<string>4, NULL, 16)); };
%%
