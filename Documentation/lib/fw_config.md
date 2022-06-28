# Firmware Configuration Interface in coreboot

## Motivation

The firmware configuration interface in coreboot is designed to support a wide variety of
configuration options in that are dictated by the hardware at runtime.  This allows a single
BIOS image to be used across a wide variety of devices which may have key differences but are
otherwise similar enough to use the same coreboot build target.

The initial implementation is designed to take advantage of a bitmask returned by the Embedded
Controller on Google ChromeOS devices which allows the manufacturer to use the same firmware
image across multiple devices by selecting various options at runtime.  See the ChromiumOS
[Firmware Config][1] documentation for more information.

This firmware configuration interface differs from the CMOS option interface in that this
bitmask value is not intended as a user-configurable setting as the configuration values must
match the actual hardware.  In the case where a user was to swap their hardware this value
would need to be updated or overridden.

## Device Presence

One common example of why a firmware configuration interface is important is determining if a
device is present in the system.  With some bus topologies and hardware mechanisms it is
possible to probe and enumerate this at runtime:

- PCI is a self-discoverable bus and is very easy to handle.
- I2C devices can often be probed with a combination of bus and address.
- The use of GPIOs with external strap to ground or different voltages can be used to detect
presence of a device.

However there are several cases where this is insufficient:

- I2C peripherals that require different drivers but have the same bus address cannot be
uniquely identified at runtime.
- A mainboard may be designed with multiple daughter board combinations which contain devices
and configurations that cannot be detected.
- While presence detect GPIOs are a convenient way for a single device presence, they are
unable to distinguish between different devices so it can require a large number of GPIOs to
support relatively few options.

This presence detection can impact different stages of boot:

### ACPI

Devices that are not present should not provide an ACPI device indicating that they are
present or the operating system may not be able to handle it correctly.

The ACPI devices are largely driven by chips defined in the mainboard `devicetree.cb` and
the variant overridetree.cb.  This means it is important to be able to specify when a device
is present or not directly in `devicetree.cb` itself.  Otherwise each mainboard needs custom
code to parse the tree and disable unused devices.

### GPIO

GPIOs with multiple functions may need to be configured correctly depending on the attached
device.  Given the wide variety of GPIO configuration possibilities it is not feasible to
specify all combinations directly in `devicetree.cb` and it is best left to code provided by
the mainboard.

### FSP UPD

Enabling and disabling devices may require altering FSP UPD values that are provided to the
various stages of FSP.  These options are also not easy to specify multiple times for
different configurations in `devicetree.cb` and can be provided by the mainboard as code.

## Firmware Configuration Interface

The firmware configuration interface can be enabled by selecting `CONFIG_FW_CONFIG` and also
providing a source for the value by defining an additional Kconfig option defined below.

If the firmware configuration interface is disabled via Kconfig then all probe attempts will
return true.

## Firmware Configuration Value

The 64-bit value used as the firmware configuration bitmask is meant to be determined at runtime
but could also be defined at compile time if needed.

There are two supported sources for providing this information to coreboot.

### CBFS

The value can be provided with a 64-bit raw value in CBFS that is read by coreboot.  The value
can be set at build time but also adjusted in an existing image with `cbfstool`.

To enable this select the `CONFIG_FW_CONFIG_CBFS` option in the build configuration and add a
raw 64-bit value to CBFS with the name of the current prefix at `CONFIG_FW_PREFIX/fw_config`.

When `fw_config_probe_device()` or `fw_config_probe()` is called it will look for the specified
file in CBFS use the value it contains when matching fields and options.

### Embedded Controller

Google ChromeOS devices support an Embedded Controller interface for reading and writing the
firmware configuration value, along with other board-specific information.  It is possible for
coreboot to read this value at boot on systems that support this feature.

This option is selected by default for the mainboards that use it with
`CONFIG_FW_CONFIG_CHROME_EC_CBI` and it is not typically necessary to adjust the value.  It is
possible by enabling the CBFS source and coreboot will look in CBFS first for a valid value
before asking the embedded controller.

It is also possible to adjust the value in the embedded controller *(after disabling write
protection)* with the `ectool` command in a ChromeOS environment.

For more information on the firmware configuration field on ChromeOS devices see the Chromium
documentation for [Firmware Config][1] and [Board Info][2].

[1]: http://chromium.googlesource.com/chromiumos/docs/+/master/design_docs/firmware_config.md
[2]: http://chromium.googlesource.com/chromiumos/docs/+/master/design_docs/cros_board_info.md

## Firmware Configuration Table

The firmware configuration table itself is defined in the mainboard `devicetree.cb` with
special tokens for defining fields and options.

The table itself is enclosed in a `fw_config` token and terminated with `end` and it contains
a mix of field and option definitions.

Each field is defined by providing the field name and the start and end bit marking the exact
location in the bitmask.  Field names must be at least three characters long in order to
satisfy the sconfig parser requirements and they must be unique with non-overlapping masks.

	field <name> <start-bit> <end-bit> [option...] end

For single-bit fields only one number is needed:

	field <name> <bit> [option...] end

A field definition can also contain multiple sets of bit masks, which can be dis-contiguous.
They are treated as if they are contiguous when defining option values.  This allows for
extending fields even after the bits after its current masks are occupied.

	field <name> <start-bit0> <end-bit0> | <start-bit1> <end-bit1> | ...

For example, if more audio options need to be supported:

	field AUDIO 3 3
		option AUDIO_0  0
		option AUDIO_1  1
	end
	field OTHER 4 4
		...
	end

the following can be done:

	field AUDIO 3 3 | 5 5
		option AUDIO_FOO   0
		option AUDIO_BLAH  1
		option AUDIO_BAR   2
		option AUDIO_BAZ   3
	end
	field OTHER 4 4
		...
	end

In that case, the AUDIO masks are extended like so:

	#define FW_CONFIG_FIELD_AUDIO_MASK						0x28
	#define FW_CONFIG_FIELD_AUDIO_OPTION_AUDIO_FOO_VALUE	0x0
	#define FW_CONFIG_FIELD_AUDIO_OPTION_AUDIO_BLAH_VALUE	0x8
	#define FW_CONFIG_FIELD_AUDIO_OPTION_AUDIO_BAR_VALUE	0x20
	#define FW_CONFIG_FIELD_AUDIO_OPTION_AUDIO_BAz_VALUE	0x28

Each `field` definition starts a new block that can be composed of zero or more field options,
and it is terminated with `end`.

Inside the field block the options can be defined by providing the option name and the field
value that this option represents when the bit offsets are used to apply a mask and shift.
Option names must also be at least three characters for the sconfig parser.

	option <name> <value>

It is possible for there to be multiple `fw_config` blocks and for subsequent `field` blocks
to add additional `option` definitions to the existing field.  These subsequent definitions
should not provide the field bitmask as it has already been defined earlier in the file and
this is just matching an existing field by name.

	field <name> [option...] end

This allows a baseboard to define the major fields and options in `devicetree.cb` and a board
variant to add specific options to fields in or define new fields in the unused bitmask in
`overridetree.cb`.

It is not possible to redefine a field mask or override the value of an existing option this
way, only to add new options to a field or new fields to the table.

### Firmware Configuration Table Example

In this example a baseboard defines a simple boolean feature that is enabled or disabled
depending on the value of bit 0, and a field at bits 1-2 that indicates which daughter board
is attached.

The baseboard itself defines one daughter board and the variant adds two more possibilities.
This way each variant can support multiple possible daughter boards in addition to the one
that was defined by the baseboard.

#### devicetree.cb

    fw_config
        field FEATURE 0
            option DISABLED 0
            option ENABLED 1
        end
        field DAUGHTER_BOARD 1 2
            option NONE 0
            option REFERENCE_DB 1
        end
    end

#### overridetree.cb

    fw_config
        field DAUGHTER_BOARD
            option VARIANT_DB_ONE 2
            option VARIANT_DB_TWO 3
        end
    end

The result of this table defined in `devicetree.cb` is a list of constants that can be used
to check if fields match the firmware configuration options determined at runtime with a
simple check of the field mask and the option value.

#### static.h

```c
/* field: FEATURE */
#define FW_CONFIG_FIELD_FEATURE_NAME "FEATURE"
#define FW_CONFIG_FIELD_FEATURE_MASK 0x00000001
#define FW_CONFIG_FIELD_FEATURE_OPTION_DISABLED_NAME "DISABLED"
#define FW_CONFIG_FIELD_FEATURE_OPTION_DISABLED_VALUE 0x00000000
#define FW_CONFIG_FIELD_FEATURE_OPTION_ENABLED_NAME "ENABLED"
#define FW_CONFIG_FIELD_FEATURE_OPTION_ENABLED_VALUE 0x00000001

/* field: DAUGHTER_BOARD */
#define FW_CONFIG_FIELD_DAUGHTER_BOARD_NAME "DAUGHTER_BOARD"
#define FW_CONFIG_FIELD_DAUGHTER_BOARD_MASK 0x00000006
#define FW_CONFIG_FIELD_DAUGHTER_BOARD_OPTION_NONE_NAME "NONE"
#define FW_CONFIG_FIELD_DAUGHTER_BOARD_OPTION_NONE_VALUE 0x00000000
#define FW_CONFIG_FIELD_DAUGHTER_BOARD_OPTION_REFERENCE_DB_NAME "REFERENCE_DB"
#define FW_CONFIG_FIELD_DAUGHTER_BOARD_OPTION_REFERENCE_DB_VALUE 0x00000002
#define FW_CONFIG_FIELD_DAUGHTER_BOARD_OPTION_VARIANT_DB_ONE_NAME "VARIANT_DB_ONE"
#define FW_CONFIG_FIELD_DAUGHTER_BOARD_OPTION_VARIANT_DB_ONE_VALUE 0x00000004
#define FW_CONFIG_FIELD_DAUGHTER_BOARD_OPTION_VARIANT_DB_TWO_NAME "VARIANT_DB_TWO"
#define FW_CONFIG_FIELD_DAUGHTER_BOARD_OPTION_VARIANT_DB_TWO_VALUE 0x00000006
```

## Device Probing

One use of the firmware configuration interface in devicetree is to allow device probing to be
specified directly with the devices themselves.  A new `probe` token is introduced to allow a
device to be probed by field and option name.  Multiple `probe` entries may be present for
each device and any successful probe will consider the device to be present.

### Probing Example

Continuing with the previous example this device would be considered present if the field
`DAUGHTER_BOARD` was set to either `VARIANT_DB_ONE` or `VARIANT_DB_TWO`:

#### overridetree.cb

    chip drivers/generic/example
        device generic 0 on
            probe DAUGHTER_BOARD VARIANT_DB_ONE
            probe DAUGHTER_BOARD VARIANT_DB_TWO
        end
    end

If the field were set to any other option, including `NONE` and `REFERENCE_DB` and any
undefined value then the device would be disabled.

### Probe Overrides

When a device is declared with a probe in the baseboard `devicetree.cb` and the same device
is also present in the `overridetree.cb` then the probing information from the baseboard
is discarded and the override device must provide all necessary probing information.

In this example a device is listed in the baseboard with `DAUGHTER_BOARD` field probing for
`REFERENCE_DB` as a field option,  It is also defined as an override device with the field
probing for the `VARIANT_DB_ONE` option instead.

In this case only the probe listed in the override is checked and a field option of
`REFERENCE_DB` will not mark this device present.  If both options are desired then the
override device must list both.  This allows an override device to remove a probe entry that
was defined in the baseboard.

#### devicetree.cb

    chip drivers/generic/example
	    device generic 0 on
		    probe DAUGHTER_BOARD REFERENCE_DB
	    end
	end

#### overridetree.cb

    chip drivers/generic/example
	    device generic 0 on
		    probe DAUGHTER_BOARD VARIANT_DB_ONE
	    end
	end

### Automatic Device Probing

At boot time the firmware configuration interface will walk the device tree and apply any
probe entries that were defined in `devicetree.cb`.  This probing takes effect before the
`BS_DEV_ENUMERATE` step during the boot state machine in ramstage.

Devices that have a probe list but do do not find a match are disabled by setting
`dev->enabled = 0` but the chip `enable_dev()` and device `enable()` handlers will still
be executed to allow any device disable code to execute.

The result of this probe definition is to provide an array of structures describing each
field and option to check.

#### fw_config.h

```c
/**
 * struct fw_config - Firmware configuration field and option.
 * @field_name: Name of the field that this option belongs to.
 * @option_name: Name of the option within this field.
 * @mask: Bitmask of the field.
 * @value: Value of the option within the mask.
 */
struct fw_config {
	const char *field_name;
	const char *option_name;
	uint64_t mask;
	uint64_t value;
};
```

#### static.c

```c
STORAGE struct fw_config __devN_probe_list[] = {
	{
		.field_name = FW_CONFIG_FIELD_DAUGHTER_BOARD_NAME,
		.option_name = FW_CONFIG_FIELD_DAUGHTER_BOARD_OPTION_VARIANT_DB_ONE_NAME,
		.mask = FW_CONFIG_FIELD_DAUGHTER_BOARD_MASK,
		.value = FW_CONFIG_FIELD_DAUGHTER_BOARD_OPTION_VARIANT_DB_ONE_VALUE
	},
	{
		.field_name = FW_CONFIG_FIELD_DAUGHTER_BOARD_NAME,
		.option_name = FW_CONFIG_FIELD_DAUGHTER_BOARD_OPTION_VARIANT_DB_TWO_NAME,
		.mask = FW_CONFIG_FIELD_DAUGHTER_BOARD_MASK,
		.value = FW_CONFIG_FIELD_DAUGHTER_BOARD_OPTION_VARIANT_DB_TWO_VALUE
	},
	{ }
};
```

### Runtime Probing

The device driver probing allows for seamless integration with the mainboard but it is only
effective in ramstage and for specific devices declared in devicetree.cb.  There are other
situations where code may need to probe or check the value of a field in romstage or at other
points in ramstage.  For this reason it is also possible to use the firmware configuration
interface directly.

```c
/**
 * fw_config_probe() - Check if field and option matches.
 * @match: Structure containing field and option to probe.
 *
 * Return %true if match is found, %false if match is not found.
 */
bool fw_config_probe(const struct fw_config *match);
```

The argument provided to this function can be created from a macro for easy use:

	FW_CONFIG(field, option)

This example has a mainboard check if a feature is disabled and set an FSP UPD before memory
training.  This example expects that the default value of this `register` is set to `true` in
`devicetree.cb` and this code is disabling that feature before FSP is executed.

```c
#include <fw_config.h>

void mainboard_memory_init_params(FSPM_UPD *mupd)
{
	if (fw_config_probe_one(FW_CONFIG(FEATURE, DISABLED))
		mupd->ExampleFeature = false;
}
```
