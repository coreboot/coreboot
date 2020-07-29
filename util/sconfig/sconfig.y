%{
/* sconfig, coreboot device tree compiler */
/* SPDX-License-Identifier: GPL-2.0-only */

#include "sconfig.h"

int yylex();
void yyerror(const char *s);

static struct bus *cur_parent;
static struct chip_instance *cur_chip_instance;
static struct fw_config_field *cur_field;

%}
%union {
	struct device *dev;
	struct chip_instance *chip_instance;
	char *string;
	int number;
}

%token CHIP DEVICE REGISTER ALIAS REFERENCE ASSOCIATION BOOL STATUS MANDATORY BUS RESOURCE END EQUALS HEX STRING PCI PNP I2C APIC CPU_CLUSTER CPU DOMAIN IRQ DRQ SLOT_DESC IO NUMBER SUBSYSTEMID INHERIT IOAPIC_IRQ IOAPIC PCIINT GENERIC SPI USB MMIO LPC ESPI FW_CONFIG_TABLE FW_CONFIG_FIELD FW_CONFIG_OPTION FW_CONFIG_PROBE
%%
devtree: { cur_parent = root_parent; } | devtree chip | devtree fw_config_table;

chipchildren: chipchildren device | chipchildren chip | chipchildren registers | chipchildren reference | /* empty */ ;

devicechildren: devicechildren device | devicechildren chip | devicechildren resource | devicechildren subsystemid | devicechildren ioapic_irq | devicechildren smbios_slot_desc | devicechildren registers | devicechildren fw_config_probe | /* empty */ ;

chip: CHIP STRING /* == path */ {
	$<chip_instance>$ = new_chip_instance($<string>2);
	chip_enqueue_tail(cur_chip_instance);
	cur_chip_instance = $<chip_instance>$;
}
	chipchildren END {
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

/* fw_config: firmware configuration table */
fw_config_table: FW_CONFIG_TABLE fw_config_table_children END { };

/* fw_config -> field */
fw_config_table_children: fw_config_table_children fw_config_field | /* empty */ ;

/* field -> option */
fw_config_field_children: fw_config_field_children fw_config_option | /* empty */ ;

/* field <start-bit> <end-bit> */
fw_config_field: FW_CONFIG_FIELD STRING NUMBER /* == start bit */ NUMBER /* == end bit */ {
	cur_field = new_fw_config_field($<string>2, strtoul($<string>3, NULL, 0), strtoul($<string>4, NULL, 0));
}
	fw_config_field_children END { };

/* field <bit> (for single-bit fields) */
fw_config_field: FW_CONFIG_FIELD STRING NUMBER /* == bit */ {
	cur_field = new_fw_config_field($<string>2, strtoul($<string>3, NULL, 0), strtoul($<string>3, NULL, 0));
}
	fw_config_field_children END { };

/* field (for adding options to an existing field) */
fw_config_field: FW_CONFIG_FIELD STRING {
	cur_field = get_fw_config_field($<string>2);
}
	fw_config_field_children END { };

/* option <value> */
fw_config_option: FW_CONFIG_OPTION STRING NUMBER /* == field value */
	{ add_fw_config_option(cur_field, $<string>2, strtoul($<string>3, NULL, 0)); };

/* probe <field> <option> */
fw_config_probe: FW_CONFIG_PROBE STRING /* == field */ STRING /* == option */
	{ add_fw_config_probe(cur_parent, $<string>2, $<string>3); }
%%
