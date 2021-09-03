%{
/* sconfig, coreboot device tree compiler */
/* SPDX-License-Identifier: GPL-2.0-only */

#include <stdint.h>
#include "sconfig.h"

int yylex();
void yyerror(const char *s);

static struct bus *cur_parent;
static struct chip_instance *cur_chip_instance;
static struct fw_config_field *cur_field;
static struct fw_config_field_bits *cur_bits;

%}
%union {
	struct device *dev;
	struct chip_instance *chip_instance;
	char *string;
	uint64_t number;
}

%token CHIP DEVICE REGISTER ALIAS REFERENCE ASSOCIATION BOOL STATUS MANDATORY BUS RESOURCE END EQUALS HEX STRING PCI PNP I2C APIC CPU_CLUSTER CPU DOMAIN IRQ DRQ SLOT_DESC SMBIOS_DEV_INFO IO NUMBER SUBSYSTEMID INHERIT IOAPIC_IRQ IOAPIC PCIINT GENERIC SPI USB MMIO GPIO FW_CONFIG_TABLE FW_CONFIG_FIELD FW_CONFIG_OPTION FW_CONFIG_PROBE PIPE
%%
devtree: { cur_parent = root_parent; } | devtree chip | devtree fw_config_table;

/* Ensure at least one `device` below each `chip`. */
chipchild_nondev: chip | registers | reference;
chipchild: device | chipchild_nondev;
chipchildren: chipchildren chipchild | /* empty */ ;
chipchildren_dev: device chipchildren | chipchild_nondev chipchildren_dev;

devicechildren: devicechildren device | devicechildren chip | devicechildren resource | devicechildren subsystemid | devicechildren ioapic_irq | devicechildren smbios_slot_desc | devicechildren smbios_dev_info | devicechildren registers | devicechildren fw_config_probe | /* empty */ ;

chip: CHIP STRING /* == path */ {
	$<chip_instance>$ = new_chip_instance($<string>2);
	chip_enqueue_tail(cur_chip_instance);
	cur_chip_instance = $<chip_instance>$;
}
	chipchildren_dev END {
	cur_chip_instance = chip_dequeue_tail();
};

device: DEVICE BUS NUMBER /* == devnum */ alias status {
	$<dev>$ = new_device_raw(cur_parent, cur_chip_instance, $<number>2, $<string>3, $<string>4, $<number>5);
	cur_parent = $<dev>$->last_bus;
}
	devicechildren END {
	cur_parent = $<dev>6->parent;
};

device: DEVICE REFERENCE STRING status {
	$<dev>$ = new_device_reference(cur_parent, cur_chip_instance, $<string>3, $<number>4);
	cur_parent = $<dev>$->last_bus;
}
	devicechildren END {
	cur_parent = $<dev>5->parent;
};

alias: /* empty */ {
	$<string>$ = NULL;
} | ALIAS STRING {
	$<string>$ = $<string>2;
};

status: BOOL | STATUS ;

resource: RESOURCE NUMBER /* == resnum */ EQUALS NUMBER /* == resval */
	{ add_resource(cur_parent, $<number>1, strtol($<string>2, NULL, 0), strtol($<string>4, NULL, 0)); } ;

reference: REFERENCE STRING /* == alias */ ASSOCIATION STRING /* == field in chip config */
	{ add_reference(cur_chip_instance, $<string>4, $<string>2); } ;

registers: REGISTER STRING /* == regname */ EQUALS STRING /* == regval */
	{ add_register(cur_chip_instance, $<string>2, $<string>4); } ;

subsystemid: SUBSYSTEMID NUMBER NUMBER
	{ add_pci_subsystem_ids(cur_parent, strtol($<string>2, NULL, 16), strtol($<string>3, NULL, 16), 0); };

subsystemid: SUBSYSTEMID NUMBER NUMBER INHERIT
	{ add_pci_subsystem_ids(cur_parent, strtol($<string>2, NULL, 16), strtol($<string>3, NULL, 16), 1); };

ioapic_irq: IOAPIC_IRQ NUMBER PCIINT NUMBER
	{ add_ioapic_info(cur_parent, strtol($<string>2, NULL, 16), $<string>3, strtol($<string>4, NULL, 16)); };

smbios_slot_desc: SLOT_DESC STRING STRING STRING STRING
	{ add_slot_desc(cur_parent, $<string>2, $<string>3, $<string>4, $<string>5); };

smbios_slot_desc: SLOT_DESC STRING STRING STRING
	{ add_slot_desc(cur_parent, $<string>2, $<string>3, $<string>4, NULL); };

smbios_slot_desc: SLOT_DESC STRING STRING
	{ add_slot_desc(cur_parent, $<string>2, $<string>3, NULL, NULL); };

smbios_dev_info: SMBIOS_DEV_INFO NUMBER STRING
	{ add_smbios_dev_info(cur_parent, strtol($<string>2, NULL, 0), $<string>3); };

smbios_dev_info: SMBIOS_DEV_INFO NUMBER
	{ add_smbios_dev_info(cur_parent, strtol($<string>2, NULL, 0), NULL); };

/* fw_config: firmware configuration table */
fw_config_table: FW_CONFIG_TABLE fw_config_table_children END { };

/* fw_config -> field */
fw_config_table_children: fw_config_table_children fw_config_field | /* empty */ ;

/* field -> option */
fw_config_field_children: fw_config_field_children fw_config_option | /* empty */ ;

/* <start-bit> <end-bit> */
fw_config_field_bits: NUMBER /* == start bit */ NUMBER /* == end bit */
{
	append_fw_config_bits(&cur_bits, strtoul($<string>1, NULL, 0), strtoul($<string>2, NULL, 0));
};

/* field <start-bit> <end-bit>(| <start-bit> <end-bit>)* */
fw_config_field_bits_repeating: PIPE fw_config_field_bits fw_config_field_bits_repeating | /* empty */ ;

fw_config_field: FW_CONFIG_FIELD STRING fw_config_field_bits fw_config_field_bits_repeating
	{ cur_field = new_fw_config_field($<string>2, cur_bits); }
	fw_config_field_children END { cur_bits = NULL; };

/* field <bit> (for single-bit fields) */
fw_config_field: FW_CONFIG_FIELD STRING NUMBER /* == bit */ {
	cur_bits = NULL;
	append_fw_config_bits(&cur_bits, strtoul($<string>3, NULL, 0), strtoul($<string>3, NULL, 0));
	cur_field = new_fw_config_field($<string>2, cur_bits);
}
	fw_config_field_children END { cur_bits = NULL; };

/* field (for adding options to an existing field) */
fw_config_field: FW_CONFIG_FIELD STRING {
	cur_field = get_fw_config_field($<string>2);
}
	fw_config_field_children END { cur_bits = NULL; };

/* option <value> */
fw_config_option: FW_CONFIG_OPTION STRING NUMBER /* == field value */
	{ add_fw_config_option(cur_field, $<string>2, strtoull($<string>3, NULL, 0)); };

/* probe <field> <option> */
fw_config_probe: FW_CONFIG_PROBE STRING /* == field */ STRING /* == option */
	{ add_fw_config_probe(cur_parent, $<string>2, $<string>3); }
%%
