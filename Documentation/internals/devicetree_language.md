# Devicetree Language

Devicetrees use what's called a Domain Specific Language, so you need to
follow the rules of the language when you're writing a devicetree.
Fortunately, it's a relatively simple language, and because its use is
so limited, it can frequently be learned just by following existing
examples.


## Comments

Comments in the devicetree are all single line style. They use the '#'
character to start the comment, and go to the end of the line.


### Comment Style

*   Put a space between the pound sign and the comment.
    *   \# Good
    *   \#Bad
*   Sentence fragments are fine, but don't end them with a period.


### When to use comments

There are standard locations where comments are typically used and
expected.


#### Starting a group of settings

Similar to commenting a function with the intent of the function, it can
be useful to describe at the top of a block of settings with what's
being configured.

Below is a good example.

```text
# VR Settings Configuration for 4 Domains
# +----------------+-----------+-----------+-------------+----------+
# | Domain/Setting |    SA     |    IA     | GT Unsliced |    GT    |
# +----------------+-----------+-----------+-------------+----------+
# | Psi1Threshold  | 20A       | 20A       | 20A         | 20A      |
# | Psi2Threshold  | 4A        | 5A        | 5A          | 5A       |
# | Psi3Threshold  | 1A        | 1A        | 1A          | 1A       |
# | Psi3Enable     | 1         | 1         | 1           | 1        |
# | Psi4Enable     | 1         | 1         | 1           | 1        |
# | ImonSlope      | 0         | 0         | 0           | 0        |
# | ImonOffset     | 0         | 0         | 0           | 0        |
# | IccMax         | 7A        | 34A       | 35A         | 35A      |
# | VrVoltageLimit | 1.52V     | 1.52V     | 1.52V       | 1.52V    |
# | AC LoadLine    | 15 mOhm   | 5.7 mOhm  | 5.2 mOhm    | 5.2 mOhm |
# | DC LoadLine    | 14.3 mOhm | 4.83 mOhm | 4.2 mOhm    | 4.2 mOhm |
# +----------------+-----------+-----------+-------------+----------+
```


#### End of keyword blocks

If there are multiple end statements in a row, and it's not instantly
obvious which end statement closes which keyword block, It's useful to
add a comment to say what's being ended.

Below, we have three end statements in a row and the start of these
blocks are probably close to the top of the file. Identifying what the
'end' statement is closing can be determined by the spacing, but it's
still useful to note it with a comment.

```text
        end # agesa northbridge
    end # domain
end # northbridge/amd/agesa/family14/root_complex
```


#### Identifying the reason for a setting

Registers which don't have a name that makes it obvious what is being
set, or don't have a macro that identifies what it's being set to should
be commented.

The register setting and macro below both describe what the setting is,
but don't describe what they're being used for. Adding the comment of
"Camera", gives the reader the extra information to know what's on that
USB port, and understand that you (generally) don't need overcurrent
protection for an internal camera.

```text
register "usb2_ports[7]" = "USB2_PORT_MID(OC_SKIP)" # Camera
```

This below register is well named and tells you that there's going to be
a delay of 12ms, but without the comment, you don't know why.

```text
register "stop_delay_ms" = "12" # NIC needs time to quiesce
```


#### Devices without aliases

It's been common in the past to comment device identifiers with what the
device is, but this may be less needed now with aliases, which allow
devices to be named in a meaningful way.

For older platforms which don't have aliases set in the chipset.cb file,
or for non chipset devices, it's still useful to add a comment after the
device is set.


### When not to use comments

#### Magic numbers

This overlaps somewhat with the next section, but if you find yourself
having to put in a comment to explain what's being set, evaluate whether
it can be expressed as a macro instead.

```
register "PmConfigSlpS3MinAssert" = "2"    # 50ms
```

Instead of using the magic number 2, use a macro, just as you'd be
expected to do in your code. It's not obvious without the comment what
2 means, so use a macro instead.

If there's some reason that a macro cannot be used, then sure, add a
comment, but this should be very unusual.

#### Redundant comments

With good naming, comments may not be needed.

Example of redundant comments:

```text
register "ipc1" = "0x00000000" # IPC1
```

```text
register "link_freq[0]" = "360 * MHz" # 360 MHz
```

#### Devices with aliases

For older chipset or SoC devices, or for non chipset devices that don't
have a chipset.cb file, it's still desirable to use comments on the
'device' line, but if the device has an alias, it should be used.


## Registers

### Register Names

Any registers which encode a value that has any sort of unit should
include the unit in the name instead of in a comment.

The name "tFAW" in the register below is taken directly from the name in
the spec, but adding the units makes it a lot easier to understand.

```text
register "tFAW" = "40000"                   # picoseconds
```

### Use Macros for encoded values

If the value associated with the register encodes the unit, instead of
entering the value directly, add a macro for the value.

```text
register "PchPmSlpS3MinAssert" = "3" # 50ms
```

This would be easier to understand if it had a macro, not just a magic
number, as seen below.

```text
register "PchPmSlpS3MinAssert" = "ASSERT_50_MILLISEC"
```

## Devicetree Keywords

alias, as, chip, cpu, cpu_cluster, device, domain, drq, end, field,
fw_config, generic, gpio, hidden, i2c, inherit, io, ioapic, ioapic_irq,
lapic, mandatory, mmio, off, on, option, pci, pnp, probe, ref, register,
smbios_dev_info, smbios_slot_desc, spi, subsystemid, usb, use


## Keyword Types

The keywords are divided into a number of different categories. A few
keywords fall into more than one of these categories.


### Alias Types

The alias type keywords are used to give human readable names to
devices, then later update that name for a specific usage.

* alias, as, use


### Containers

These keywords act to group information into logical units of different
sorts.

* chip, device, domain, field, fw_config, end


### Device/Bus Types

These keywords are used to identify the type of device being configured
and select the appropriate structure for configuration.

* cpu, cpu_cluster, domain, generic, gpio, i2c, ioapic, lapic, mmio,
  pci, pnp, ref, spi, usb


### Enumeration Status Modifiers

The status modifiers determine how a device should be treated during
enumeration.

* on, off, hidden, mandatory


### Firmware configuration

* field, fw_config, option, probe, ref


### Resource allocation

* drq, io, irq


### SMBIOS

* smbios_dev_info, smbios_slot_desc


### Subsystem

* inherit, subsystemid


### Register

* register


### MPTABLE

* ioapic_irq


## Keyword Definitions

### alias

* Keyword type: Alias
* Introduced in June of 2020, coreboot version 4.12
* Usage: `device <device type> <device address> **alias** <ALIAS ID> <status modifier> ... end`

Sometimes, the driver of one device needs to know about another device
that can be anywhere in the device hierarchy. Current applications boil
down to EEPROMs that store information that is consumed by some code
(e.g. MAC address).

The idea is to give device nodes in the `devicetree.cb` an alias that
can later be used to link it to a device driver's `config` structure.
The driver has to declare a field of type `struct device *`, e.g.

Override devices can add an alias if it does not exist, but cannot
change the alias for a device that already exists.

Alias names are checked for conflicts both in the base tree and in the
override tree.

References are resolved after the tree is parsed so aliases and
references do not need to be in a specific order in the tree.

Example:
```text
device i2c 0x50 alias my_eeprom on end
```

static.c:

```C
struct some_chip_driver_config {
        DEVTREE_CONST struct device *needed_eeprom;
};
```

### as

* Keyword type: Alias
* Introduced in June of 2020, coreboot version 4.12
* Usage: `use <Alias ID> as <Alias ID 2>`

This keyword is used along with the 'use' keyword to give a more
specific name to a generic alias.

The author of the devicetree is free to choose any alias name that is
unique in the devicetree. Later, when configuring the driver the alias
can be used to link the device with the field of a driver's config as in
the following example.


Example:

```text
chip some/chip/driver
        use my_eeprom as needed_eeprom
end
```


### chip

* Keyword type: Component type
* Introduced in initial sconfig implementation, pre coreboot 4.0
* Usage: `**chip** <path to chip.h file> ... end`

The chip keyword defines a section for a device inside a hierarchy of
devices.

Following the chip keyword is a path to an optional chip.h file. If
this file exists, it should contain a structure of all register settings
available for this device.

The section started with "chip" is closed with the "end" keyword.

Note that register commands are chip based, not device based.

Example:
```text
chip some/chip/driver # comment as desired
    # A chip section must have at least one device inside it
    ...
end # some/chip/driver
```


### cpu

* Keyword type: Device type
* Introduced in October of 2014, coreboot version 4.0
* Usage: `device **cpu** <Identifier> [alias <alias ID>] <status modifier> ... end`

The cpu device type was introduced In order to enumerate CPU devices
that are non-x86 (read: no lapic).

This device type is not used very much, as most non-x86 platforms just
handle all the CPU configuration at the cpu_cluster level.

Example:

```text
device cpu 0 on end
```


### cpu_cluster

* Keyword type: Device type
* Introduced in initial sconfig implementation, pre coreboot 4.0 as apic_cluster
* Updated in May of 2010 - renamed from apic_cluster to lapic_cluster
* Updated in February of 2013 - Renamed from from lapic_cluster to cpu_cluster
* Usage: `device cpu_cluster <Identifier> [alias <alias ID>] <status modifier> …. end`

The CPU Cluster is typically one of the two top-level devices in a
devicetree, with the other being the PCI Domain. Like the domain
keyword, the cpu_cluster acts as a bridge device to contain any CPU and
lapic devices.

On x86 type devices, the CPU Cluster contains CPU chip entries for each
socket which then contain any lapics on the system.

On ARM or other non-x86 devices, the CPU Cluster can either contain
multiple CPUs or it can just be marked 'on', leaving all CPU
configuration to be handled at the CPU Cluster level.

Example:

```text
device cpu_cluster 0 on
    device lapic 0 on end
end
```

### device

* Keyword type: Component type
* Introduced in: Initial sconfig implementation, pre coreboot 4.0
* Usage: `device <device type> <Identifier> [alias <alias ID>]  <status modifier>…. end`

A chip defines a collection of one or more devices, and as such, the
device is the fundamental building block of devicetree.

The device block tells coreboot that a device is present on the
mainboard, gives some information on how to find, access, and work with
the device based on the device type.

Examples:

```text
device cpu_cluster 0 on
    device lapic 0 on end
end
```

### domain

* Keyword type: Device type
* Introduced in: Initial sconfig implementation, pre coreboot 4.0
* Updated in: February of 2013: Renamed pci_domain to domain
* Usage: `device domain <identifier> [alias <alias ID>]  <status modifier> ... end`

The domain type device acts as a container for one or more chips,
defining a logical bus segment.

As of 2022-04-29, the domain keyword is still used exclusively to define
PCI bus segments.

Example:

```text
device domain 0 on
    device generic 0 alias dptf_policy on end
        device pci 1f.0 on                      # 8086 229c - LPC bridge
            device pnp 2e.209 off end           # GPIO 4
    end
end
```


### drq

* Keyword type: Resource type
* Introduced in: Initial sconfig implementation, pre coreboot 4.0
* Usage: `drq 0x<register #> = <drq line>`

Drq is used to configure a legacy DMA Request line register for a device
on a legacy bus - ISA/LPC.

The drq configuration is only allowed inside a pnp block.

Example:

```text
device pnp 2e.1 on              # Parallel port
    io 0x60 = 0x378
    irq 0x70 = 7
    drq 0x74 = 3
end
```

### end

* Keyword type: Component type
* Introduced in: Initial sconfig implementation, pre coreboot 4.0
* Usage: `<device | chip | domain | field | fw_config> ... end`

End is used to close a chip, device, field, or fw_config block.

Example:

```text
chip northbridge/intel/x4x
    device domain 0 on
        chip southbridge/intel/i82801gx
            device pci 1f.0 on                 # ISA bridge
                chip superio/winbond/w83627dhg
                    device pnp 2e.1 on         # LPT
                        io 0x60 = 0x378
                        irq 0x70 = 7
                        drq 0x74 = 3
                    end # LPT
                end # superio/winbond/w83627dhg
            end # ISA Bridge
        end # Southbridge
    end # domain 0
end # Northbridge
```

### field

* Keyword type: Firmware config
* Introduced in: May of 2020, coreboot version 4.12
* Usage: `field <Identifier string> <low bit> <high bit> [ | <low bit> <high bit> …] ... end`

Fields define the bits used in the Firmware config runtime options.
Note that the bits in a field can be discontiguous.

All field definitions must be inside a fw_config block.

Example:

```text
field AUDIO 8 10 | 29 29
    option NONE 0
    option MAX98357_ALC5682I_I2S 1
    option MAX98373_ALC5682I_I2S 2
    option MAX98373_ALC5682_SNDW 3
    option MAX98373_ALC5682I_I2S_UP4 4
    option MAX98360_ALC5682I_I2S 5
    option RT1011_ALC5682I_I2S 6
    option AUDIO_FOO 7
    option AUDIO_BAR 8
    option AUDIO_QUUX 9
    option AUDIO_BLAH1 10
    option AUDIO_BLAH2 15
end
```

static_fw_config.h:

```C
 FW_CONFIG_FIELD_AUDIO_MASK 0x20000700
 FW_CONFIG_FIELD_AUDIO_OPTION_NONE_VALUE 0x0
 FW_CONFIG_FIELD_AUDIO_OPTION_MAX98357_ALC5682I_I2S_VALUE 0x100
 FW_CONFIG_FIELD_AUDIO_OPTION_MAX98373_ALC5682I_I2S_VALUE 0x200
 FW_CONFIG_FIELD_AUDIO_OPTION_MAX98373_ALC5682_SNDW_VALUE 0x300
 FW_CONFIG_FIELD_AUDIO_OPTION_MAX98373_ALC5682I_I2S_UP4_VALUE 0x400
 FW_CONFIG_FIELD_AUDIO_OPTION_MAX98360_ALC5682I_I2S_VALUE 0x500
 FW_CONFIG_FIELD_AUDIO_OPTION_RT1011_ALC5682I_I2S_VALUE 0x600
 FW_CONFIG_FIELD_AUDIO_OPTION_AUDIO_FOO_VALUE 0x700
 FW_CONFIG_FIELD_AUDIO_OPTION_AUDIO_BAR_VALUE 0x20000000
 FW_CONFIG_FIELD_AUDIO_OPTION_AUDIO_QUUX_VALUE 0x20000100
 FW_CONFIG_FIELD_AUDIO_OPTION_AUDIO_BLAH1_VALUE 0x20000200
 FW_CONFIG_FIELD_AUDIO_OPTION_AUDIO_BLAH2_VALUE 0x20000700
```

### fw_config

* Keyword type: Firmware config
* Introduced in: May of 2020, coreboot version 4.12
* Usage: fw_config

Fw_config is the top level token for defining firmware config fields.
All field and option definitions must be inside the fw_config block.
Note that the table can be defined before any chips are configured.

Example:
```text
fw_config
    field USB_DB 0 1
        option USB_DB_A1_PS8811_C1_PS8818 0
        option USB_DB_A1_ANX7491_C1_ANX7451 1
    end
    field FORM_FACTOR 2
        option FORM_FACTOR_CLAMSHELL 0
        option FORM_FACTOR_CONVERTIBLE 1
    end
end
```


### generic

* Keyword type: Device type
* Introduced in: May of 2016, coreboot version 4.4
* Usage: `device generic <identifier> [alias <alias ID>] <status modifier> ... end`

This allows the devicetree to describe a device that does not have a
specific bus, but may need to be described in tables for the operating
system. For instance some chips may have various GPIO connections that
need to be described but do not fall under any other device.

Example:
```text
device pci 0c.0 on                        # CNVi
    chip drivers/wifi/generic
        register "wake" = "GPE0A_CNVI_PME_STS"
        device generic 0 on end
    end
end
```


### gpio

* Keyword type: Device type
* Introduced in: December 2020, coreboot version 4.13
* Usage: `device **gpio** <identifier> [alias <alias ID>] <status modifier> ... end`

The general idea behind this is that every chip can have gpios that
shall be accessible in a very generic way by any driver through the
devicetree.

The chip that implements the chip-specific gpio operations has to assign
them to the generic device operations struct, which then gets assigned
to the gpio device during device probing. See CB:48583 for how this gets
done for the SoCs using intel/blocks/gpio.

The gpio device then can be added to the devicetree with an alias name
like in the following example:

```text
chip soc/whateverlake
    device gpio 0 alias soc_gpio on end
    ...
end
```

Any driver that requires access to this gpio device needs to have a
device pointer (or multiple) and an option for specifying the gpio to be
used in its chip config like this:

```C
struct drivers_ipmi_config {
    ...
    DEVTREE_CONST struct device *gpio_dev;
    u16 post_complete_gpio;
    ...
};
```

The device `soc_gpio` can then be linked to the chip driver's `gpio_dev`
above by using the syntax `use ... as ...`, which was introduced in
commit 8e1ea52:

```text
chip drivers/ipmi
    use soc_gpio as gpio_dev
    register "bmc_jumper_gpio" = "GPP_D22"
    ...
end
```

The IPMI driver can then use the generic gpio operations without any
knowlege of the chip's specifics:

```C
unsigned int gpio_val;
const struct gpio_operations *gpio_ops;
gpio_ops = dev_get_gpio_ops(conf->gpio_dev);
gpio_val = gpio_ops->get(conf->bmc_jumper_gpio);
```

For a full example have a look at CB:48096 and CB:48095.


### hidden

* Keyword type: Enumeration Status Modifier
* Introduced in: September 2018, coreboot version 4.8
* Updated in: February of 2020, coreboot version 4.11
* Usage: `device <device type> <identifier> [alias <alias ID>] hidden ... end`

The "Hidden" keyword was added to support devices sharing the same
driver to update their ACPI status with different values. If a device
is marked as Hidden, it omits the "SHOW_IN_UI" field, which prevents the
device from showing up in the OSPM user interface. This was added to
prevent ChromeOS specific devices from showing up in Windows, for
example. It does not look like this keyword was ever actually used in
this capacity, but the code for this is still present.

In 2020, the "Hidden" keyword's use was expanded to fix the problem of
PCI devices which have been hidden by disabling access to the config
space. Because the Device/Vendor IDs read back as 0xffffffff from the
hidden device, it appears that there is no PCI device located at that
Bus/Device/Function. This causes coreboot to remove the device at the
end of PCI enumeration. To fix this, if a device uses 'hidden' instead
of 'on', then it will be assumed during PCI enumeration that the device
indeed does exist, and it will not be removed. This allows coreboot to
manually assign resources to the device even though it doesn't appear to
exist.

Example:
```text
device pci 1f.2 alias pmc hidden end
```

static.c:

```C
.enabled = 1,
.hidden = 1,
.mandatory = 0,
```


### i2c

* Keyword type: Device type
* Introduced in: Initial sconfig implementation, pre coreboot 4.0
* Usage: `device I2c <identifier> [alias <alias ID>] <status modifier> ... end`

The identifier is the device's address encoded as hexadecimal number.

Example:

```text
device i2c 2c on end
chip drivers/i2c/generic
    register "hid" = ""ELAN0000""
    register "desc" = ""ELAN Touchpad""
    register "irq_gpio" = "ACPI_GPIO_IRQ_LEVEL_LOW(GPIO_9)"
    register "wake" = "GEVENT_22"
    register "probed" = "1"
    device i2c 15 on end
end
```

### inherit

* Keyword type: subsystem
* Introduced in: March of 2011, coreboot version 4.0
* Usage: `subsystemid beee c001 inherit`

Used to modify the subsystemid keyword, inherit makes the ID apply to
all subdevices and functions.

Example:
```text
device pci 00.0 on
    subsystemid 0xdead 0xbeef inherit
end
```


### io

* Keyword type: Resource
* Introduced in: Initial sconfig implementation, pre coreboot 4.0
* Usage: `io 0x<address> = 0x<value>`

The io command is used to set an legacy device's io register to a value.

Example:

```text
device pnp 4e.06 on                # Keyboard
    io 0x60            = 0x0060
    io 0x62            = 0x0064
    irq 0x70        = 1
end
```

### ioapic

* Keyword type: Device type

* Usage: `device ioapic <identifier> [alias <alias ID>] <status modifier> ... end`


### ioapic_irq

* Keyword type: Mpinit type
* Introduced in: June of 2012, coreboot version 4.0
* Usage: `ioapic_irq <APICID> <INTA|INTB|INTC|INTD> <INTPIN>`

This keyword is used to support autogeneration of the MPTABLE from
devicetree.cb. This is done by a write_smp_table() declared weak in
mpspec.c. If the mainboard doesn't provide its own function, this
generic implementation is called.

The ioapic_irq directive can be used in pci and pci_domain devices. If
there's no directive, the autogen code traverses the tree back to the
pci_domain and stops at the first device which sets such a directive,
and uses that information to generate the entry according to PCI IRQ
routing rules.

```text
```


### irq

* Keyword type: Resource type
* Introduced in initial sconfig implementation, pre coreboot 4.0
* Usage: `irq 0x<address> = 0x<value>`

Irq is used to configure a legacy interrupt Request line register for a
device on a legacy bus - ISA/LPC/eSPI.

The irq configuration is only allowed inside a pnp block.

Example:

```text
irq 0xc5 = 0x1f
```

```text
chip superio/fintek/f71808a
    device pnp 4e.4 on    # Hardware monitor
        io 0x60 = 0x295
        irq 0x70 = 0
    end
end
```

### lapic

* Keyword type: Device type
* Introduced in initial sconfig implementation, pre coreboot 4.0
* Updated in May of 2010 - Renamed apic to lapic
* Usage: `device lapic <identifier> [alias <alias ID>] <status modifier> ... end`

Defines an lapic device on an x86 mainboard.

Example:

```text
device cpu_cluster 0 on          # (L)APIC cluster
    chip cpu/intel/slot_1        # CPU socket 0
        device lapic 0 on end    # Local APIC of CPU 0
    end
    chip cpu/intel/slot_1        # CPU socket 1
        device lapic 1 on end    # Local APIC of CPU 1
    end
end
```

### mandatory

* Keyword type: Enumeration Status Modifier
* Introduced in: February of 2020, coreboot version 4.11
* Usage: `device <device type> <identifier> [alias <alias ID>] mandatory ... end`

The Mandatory keyword allows for minimal PCI scanning which speeds up the
boot process. Only devices marked "mandatory" get enabled and scanned
during PCI enumeration when the MINIMAL_PCI_SCANNING Kconfig option is
enabled.

If MINIMAL_PCI_SCANNING is not enabled, this means the same as 'on'.

static.c:

```C
.enabled = 1,
.hidden = 0,
.mandatory = 1,
```

example:

```text
device pci 1f.0 mandatory  # LPC
    chip drivers/pc80/tpm
        device pnp 0c31.0 on end
    end # tpm
end # LPC
```


### mmio

* Keyword type: Device type
* Introduced in January of 2018, coreboot version 4.7
* Usage: `device mmio 0x<address> [alias <alias ID>] <status modifier> ... end`

The mmio keyword allows fixed memory-mapped IO addressed devices to be
assigned to given values.

AMD platforms perform a significant amount of configuration through these
MMIO addresses, including I2C bus configuration.

example:

```text
device mmio 0xfedc9000 alias uart_0 off end
device mmio 0xfedca000 alias uart_1 off end
device mmio 0xfedc2000 on                      # I2C 0

    chip drivers/generic/adau7002
        device generic 0.0 on end
    end

    chip drivers/i2c/da7219
        register "irq_gpio" = "ACPI_GPIO_IRQ_EDGE_LOW(GPIO_14)"
    end

    chip drivers/generic/max98357a
        register "hid" = ""MX98357A""
        register "sdmode_delay" = "5"
        device generic 0.1 on end
    end

end # I2C 0
```


### off

* Keyword type: Enumeration Status Modifier
* Introduced in: Initial sconfig implementation, pre coreboot 4.0
* Usage: `device <device type> <identifier> [alias <alias ID>] off ... end`

Indicates that the device should be disabled if possible and should not
be enumerated or configured. To disable a device, the code to disable it
must be present in the chipset/SoC code, though it isn't always possible
to actually disable the device.

Note that If a device (especially PCI devices) is marked as 'off', but
does not get disabled, the OS or a driver may discover the device during
its enumeration sequence and assign it resources anyway.

static.c:

```C
.enabled = 0
.hidden = 0,
.mandatory = 0,
```

example:

```text
device usb 2.6 off end
device pnp 2e.0 off end # FDC
device pci 15.2 off end
```

### on

* Keyword type: Enumeration Status Modifier
* Introduced in: Initial sconfig implementation, pre coreboot 4.0
* Usage: `device <device type> <identifier> [alias <alias ID>] on ... end`

Marks a device as present and enabled. This indicates that the device
should be enumerated and configured.

static.c:

```C
.enabled = 1
.hidden = 0,
.mandatory = 0,
```

Example:

```text
device lapic 0 on end
device pci 00.0 on  end # Host Bridge
device pnp 0c31.0 on end
```

### option

* Keyword type: Firmware config
* Usage: `option <name> <value>`

Options define the values which may be used in a firmware config field.
The option keyword may only be used inside a field, which may only be
used inside an fw_config block.

Example:

```text
fw_config
    field OLED_SCREEN 28
        option OLED_NOT_PRESENT 0
        option OLED_PRESENT 1
    end
end
```

### pci

* Keyword type: Device type
* Introduced in: Initial sconfig implementation, pre coreboot 4.0
* Usage: `device pci <dev.func> [alias <alias ID>]  <status modifier> …. end`

The pci device type defines a PCI or PCIe device on the PCI logical bus.

Resources for all PCI devices are assigned automatically, or must be
assigned in code if they're non-standard.

Only a single segment group is supported per domain, but there can be multiple
domains to support the case of multiple segment groups, each with a bus 0.
Because the bus is not specified, It's assumed that all pci devices that are
not behind a pci bridge device are on bus 0. If there are additional pci busses
in a chip, they can be added behind their bridge device.

Examples:

```text
device pci 1a.0 on end # USB2 EHCI #2
device pci 1b.0 on # High Definition Audio
    subsystemid 0x1a86 0x4352
end
```

Example - ISA bus under PCI to ISA bridge:

```text
device pci 4.0 on                        # ISA bridge
    chip superio/winbond/w83977tf        # Super I/O
        device pnp 3f0.2 on              # COM1
            io 0x60 = 0x3f8
            irq 0x70 = 4
        end
    end
end
```

Example - PCI Bus under a PCI to PCI bridge:

```text
device pci 08.0 on end                    # Dummy Host Bridge, do not disable
device pci 08.1 alias gpp_bridge_a off    # Internal GPP Bridge 0 to Bus A
    device pci 0.0 alias gfx off end            # Internal GPU (GFX)
    device pci 0.1 alias gfx_hda off end        # Display HDA (GFXAZ)
end
```


### pnp

* Keyword type: Device type
* Introduced in: Initial sconfig implementation, pre coreboot 4.0
* Usage: `device pnp <Identifier> [alias <alias ID>]  <status modifier>…. end`

pnp is used to define an IO addressed device within a chip on a legacy
bus - ISA, LPC, or eSPI. The identifier is the IO address used to
access the chip, followed by the identifier for the device within the
chip.

The io, irq, and drq resource identifiers are allowed only inside a pnp
device.

Example:

```text
chip superio/ite/it8783ef
    device pnp 2e.0 off end    # Floppy
    device pnp 2e.1 on    # COM 1
        io 0x60 = 0x3f8
        irq 0x70 = 4
    end
end
```

### probe

* Keyword type: Firmware config
* Introduced in May of 2020, coreboot version 4.12
* Usage: `probe <Field Name> <Option Name>`

The fields and options can be used to probe for a device and have that
device be disabled if it is not found at boot time.

Example:

```text
fw_config
    field FEATURE 0 0
        option DISABLE 0
        option ENABLE 1
    end
end

chip drivers/generic/feature
    device generic 0 on
        probe FEATURE ENABLE
    end
end
```


### ref

* Keyword type: Device type (alias)
* Introduced in July of 2020, coreboot version 4.12
* Usage: `device REF <Identifier> <status modifier>…. end`

This allows the chipset to assign alias names to devices as well as set
default register values. This works for both the baseboard devicetree.cb
as well as variant overridetree.cb.

Example:

chipset.cb:

```text
device pci 15.0 alias i2c0 off end
```

devicetree.cb:

```text
device ref i2c0 on end
```

### register

* Keyword type: Register
* Introduced in: Initial sconfig implementation, pre coreboot 4.0
* Usage: `register "name" = "value"`

The register keyword was initially intended to supply just that -
information for device register values. Over the years, its use has been
expanded to much more than this, becoming a general configuration
mechanism.

All values set by the register keyword MUST be defined in the chip.h
file. The register keyword only works at the chip level, not at the
device level.

Any register defined in the chip.h file that is not set in the
devicetree file gets set to zero.

To double quote a value, use two sets of quotes: `register "name" = ""string""`

Examples:

```text
register "desc" = ""USB2 Type-A Rear Lower""
register "pcie_power_limits" = "{ { 10, 0 }, { 0, 0 }, { 0, 0 },
                                  { 0, 0 }, { 10, 0 }, { 0, 0 } }"
register "SataPortsEnable[0]" = "1"
register "generic.irq"              = "ACPI_IRQ_LEVEL_LOW(GPP_B3_IRQ)"
register "common_soc_config" = "{
                                    // Touchpad I2C bus
                                    .i2c[0] = {
                                        .speed = I2C_SPEED_FAST,
                                        .rise_time_ns = 80,
                                        .fall_time_ns = 110,
                                    },
                                }"
```


### smbios_dev_info

* Keyword type: smbios
* Introduced in: May of 2019, coreboot version 4.9
* Usage: `smbios_dev_info <Identifier>`

Specify the instance ID and RefDes (Reference Designation) of onboard
devices.

The `SMBIOS_TYPE41_PROVIDED_BY_DEVTREE` Kconfig option enables using
this syntax to control the generated Type 41 entries (Onboard Devices
Extended Information). When this option is enabled, Type 41 entries are
only autogenerated for devices with a defined instance ID. This avoids
having to keep track of which instance IDs have been used for every
device class.

Using `smbios_dev_info` when `SMBIOS_TYPE41_PROVIDED_BY_DEVTREE` is not
enabled will result in a build-time error, as the syntax is meaningless
in this case. This is done with preprocessor guards around the Type 41
members in `struct device` and the code which uses the guarded members.
Although the preprocessor usage isn't particularly elegant, adjusting the
devicetree syntax and/or grammar depending on a Kconfig option is
probably even worse.

Example:

```text
device pci 1c.0 on        # PCIe Port #1
    device pci 00.0 on
        smbios_dev_info 6
    end
end
device pci 1c.1 on        # PCIe Port #2
    device pci 00.0 on
        smbios_dev_info 42 "PCIe-PCI Time Machine"
    end
end
```

### smbios_slot_desc

* Keyword type: smbios
* Introduced in: May of 2019, coreboot version 4.9
* Usage: `smbios_slot_desc** <type> <length> [designation] [data width]`

Add the new field 'smbios_slot_desc', which takes 2 to 4 arguments.
The field is only valid for PCI devices and only compiled if SMBIOS
table generation is enabled.

smbios_slot_desc arguments - described in src/include/smbios.h:

1.  slot type: enum misc_slot_type
2.  slot length: enum misc_slot_length
3.  slot designation (optional): text string
4.  slot data width (optional): enum slot_data_bus_bandwidth

Example:

```text
device pci 1c.1 on
    smbios_slot_desc "SlotTypePciExpressGen3X16" "SlotLengthOther" "SLOT2" "SlotDataBusWidth8X"
end # PCIe Port #2 Integrated Wireless LAN
```

### spi

* Keyword type: Device type
* Introduced in: February of 2017, coreboot version 4.5
* Usage: device **SPI** <Identifier> [alias <alias ID>]  <status modifier>…. end

The SPI device takes only one parameter, the chip-select for the device
on the SPI bus.

Example:

```text
device spi 0 alias spi_tpm on end
```


### subsystemid

* Keyword type: Subsystem
* Introduced in: March of 2011, coreboot version 4.0
* Usage: `subsystemid <vendor> <device> [inherit]`

Sets a PCI subsystem ID.

If the user wants to have this ID inherited to all subdevices/functions,
they can add the keyword 'inherit' after the ID.

If the user doesn't want to inherit the subsystem value for a single
device, they can specify 'subsystemid 0 0' on this particular device.

Example:

```text
device pci 00.0 on
    subsystemid 0xdead 0xbeef
end
```

or

```text
device pci 00.0 on
    subsystemid 0xdead 0xbeef inherit
end
```

### usb

* Keyword type: Device type
* Introduced in: May of 2018, coreboot version 4.7
* Usage: `device usb <Identifier> [alias <alias ID>]  <status modifier> ... end`

This supports describing USB ports in the devicetree. It allows a USB
port location to be described in the tree with configuration
information, and ACPI code to be generated that provides this
information to the OS.

The usb symbol works with the scan_usb_bus() operation to scan bridges
for devices so a tree of ports and hubs can be created.

The device address is computed with a 'port type' and a 'port id' which
is flexible for the SOC to handle depending on their specific USB setup
This also allows USB2 and USB3 ports to be described separately.

For example a board may have devices on two ports, one with a USB2
device and one with a USB3 device, both of which are connected to an
xHCI controller with a root hub:

```text
             xHCI
               |
            RootHub
            |     |
        USB2[0]  USB3[2]
```

This is described by the following example:

```text
device pci 14.0 on
    chip drivers/usb/acpi
        register "name" = ""Root Hub""
        device usb 0.0 on

            chip drivers/usb/acpi
                register "name" = ""USB 2.0 Port 0""
                device usb 2.0 on end
            end

            chip drivers/usb/acpi
                register "name" = ""USB 3.0 Port 2""
                device usb 3.2 on end
            end

        end # device usb 0.0
    end # chip drivers/usb/acpi
end # device pci 14.0
```

### use

* Keyword type: Alias
* Introduced in September of 2020, coreboot version 4.12
* Usage: `use <Alias ID> as <Alias ID2>`

The author of the devicetree is free to choose any alias name that is
unique in the devicetree. Later, when configuring the driver for a
board, the alias can be used to link the device with the field of a
driver's config, giving it a specific name.

Example:

```text
chip some/chip/driver
    use my_eeprom as needed_eeprom
end
```
