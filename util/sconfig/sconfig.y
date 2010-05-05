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

struct device *cur_parent, *cur_bus;

%}
%union {
	struct device *device;
	char *string;
	int number;
}
%token CHIP DEVICE REGISTER BOOL BUS RESOURCE END EQUALS HEX STRING PCI PNP I2C APIC APIC_CLUSTER PCI_DOMAIN IRQ DRQ IO NUMBER
%%
devtree: devchip { postprocess_devtree(); } ;

devchip: chip | device ;

devices: devices devchip | devices registers | ;

devicesorresources: devicesorresources devchip | devicesorresources resource | ;

chip: CHIP STRING /* == path */ {
	$<device>$ = new_chip($<string>2);
	cur_parent = $<device>$;
}
	devices END {
	cur_parent = $<device>3->parent;
	fold_in($<device>3);
	add_header($<device>3);
};

device: DEVICE BUS NUMBER /* == devnum */ BOOL {
	$<device>$ = new_device($<number>2, $<string>3, $<number>4);
	cur_parent = $<device>$;
	cur_bus = $<device>$;
}
	devicesorresources END {
	cur_parent = $<device>5->parent;
	cur_bus = $<device>5->bus;
	fold_in($<device>5);
	alias_siblings($<device>5->children);
};

resource: RESOURCE NUMBER /* == resnum */ EQUALS NUMBER /* == resval */
	{ add_resource($<number>1, strtol($<string>2, NULL, 0), strtol($<string>4, NULL, 0)); } ;

registers: REGISTER STRING /* == regname */ EQUALS STRING /* == regval */
	{ add_register($<string>2, $<string>4); } ;

%%
