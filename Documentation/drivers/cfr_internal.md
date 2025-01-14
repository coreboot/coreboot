# CFR - coreboot form representation - Internals

This documents the API internally used by coreboot to be used
by ramstage code.

Please read [CFR](cfr.md) first.

## Enabling CFR support

Users should select `DRIVERS_OPTION_CFR` in Kconfig to enable CFR
support. Mainboards should select `DRIVERS_OPTION_CFR_ENABLED` to
enable `DRIVERS_OPTION_CFR` by default.

## Using CFR

When CFR support is enabled there are two possibilites to generate
the records:

- mainboard specific code
- automatic collection

In both cases you have to add `C` structs in ramstage to describe the
option and group them together into a form. CFR objects should reside
on the heap as they can be modified to match the current boot flow.

### Updating CFR options

The CFR options should be updated before tables are written.
You can use a callback, using the `WITH_CALLBACK()` macro, to update
single or multiple options when the CFR table is written into the
coreboot table.

**Example:** Updates the option serial_number with the contents from the
EMI eeprom.

```
static void update_serial(const struct sm_object *obj, struct sm_object *new)
{
	new->sm_varchar.default_value = get_emi_eeprom_vpd()->serial_number;
}

static const struct sm_object serial_number = SM_DECLARE_VARCHAR({
	.flags		= CFR_OPTFLAG_READONLY | CFR_OPTFLAG_VOLATILE,
	.opt_name	= "serial_number",
	.ui_name	= "Serial Number",
}, WITH_CALLBACK(update_serial));
```

### Dependencies in CFR options

The CFR options can have a dependency that must be evaluated at runtime by
the OS/payload that parses the CFR record and displays the UI.
By using the `WITH_DEP()` macro you can specify another numeric option that
is checked to hide the current option. The `WITH_DEP_VALUES()` macro allows
specifying one or more values that cause the dependent option to be displayed.

**Example:** Declares a dependency from `sata_disable_port0` to `sata_enable`.
The option `sata_disable_port0` will be hidden as long as "sata_enable" is 0.
When the user changes "sata_enable" to 1 or it was 1 then the option
`sata_disable_port0` should be visible.

```
static struct sm_object sata_enable = SM_DECLARE_BOOL({
	.flags		= CFR_OPTFLAG_RUNTIME,
	.opt_name	= "sata_enable",
	.ui_name	= "Enable SATA controller",
	.ui_helptext	= NULL,
	.default_value	= true,
});

static struct sm_object sata_disable_port0 = SM_DECLARE_BOOL({
	.flags		= CFR_OPTFLAG_RUNTIME,
	.opt_name	= "sata_disable_port0",
	.ui_name	= "Disable SATA port #0",
	.ui_helptext	= NULL,
	.default_value	= false,
}, WITH_DEP(&sata_enable));
```

**Example:** Declares a dependency from `com1_termination` to `com1_mode`.
The option `com1_termination` will only be shown if `com1_mode` is set to RS-485.

```
#define COM_MODE_DISABLED 3
#define COM_MODE_RS232    0
#define COM_MODE_RS485    1

static struct sm_object com1_mode = SM_DECLARE_ENUM({
	.flags		= CFR_OPTFLAG_RUNTIME,
	.opt_name	= "com1_mode",
	.ui_name	= "COM1 Mode",
	.ui_helptext	= NULL,
	.default_value	= 1,
	.values		= (const struct sm_enum_value[]) {
				{ "Disabled", COM_MODE_DISABLED },
				{ "RS-232",   COM_MODE_RS232    },
				{ "RS-485",   COM_MODE_RS485    },
				SM_ENUM_VALUE_END },
});

static struct sm_object com1_termination = SM_DECLARE_BOOL({
	.flags		= CFR_OPTFLAG_RUNTIME,
	.opt_name	= "com1_termination",
	.ui_name	= "Enable COM1 termination resistors",
	.ui_helptext	= NULL,
	.default_value	= false,
}, WITH_DEP_VALUES(&com1_mode, COM_MODE_RS485));

```

### Providing mainboard custom options

A mainboard that uses CFR can provide a list of custom options
be overwriting the weak `void mb_cfr_setup_menu(struct lb_cfr *cfr_root);`
function in ramstage.

### Automatic CFR collection

CFR forms that have the `__cfr_form` attribute are automatically collected
and inserted into the coreboot table.

## Example

The following CFR form `southbridge` will be automatically added to the
coreboot table and it will have a single option called `Enable NMI` that
allows the variable `nmi` to be changed to *0* or *1*.

**Example:**
```C
static struct sm_object nmi = SM_DECLARE_BOOL({
	.flags		= CFR_OPTFLAG_RUNTIME,
	.opt_name	= "nmi",
	.ui_name	= "Enable NMI",
	.ui_helptext	= NULL,
	.default_value	= false,
});

static const  __cfr_form struct sm_obj_form southbridge = {
	.flags		= 0,
	.ui_name	= "Southbridge",
	.obj_list	= (const struct sm_object *[]) {
		&nmi,
		NULL
	},
};
```