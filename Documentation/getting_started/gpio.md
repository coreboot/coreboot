# Configuring a mainboard's GPIOs in coreboot

## Introduction

Every mainboard needs to appropriately configure its General Purpose Inputs /
Outputs (GPIOs). There are many facets of this issue, including which boot
stage a GPIO might need to be configured.

## Boot stages

Typically, coreboot does most of its non-memory related initialization work in
ramstage, when DRAM is available for use. Hence, the bulk of a mainboard's GPIOs
are configured in this stage. However, some boards might need a few GPIOs
configured before that; think of memory strapping pins which indicate what kind
of DRAM is installed. These pins might need to be read before initializing the
memory, so these GPIOs are then typically configured in bootblock or romstage.

## Configuration

Most mainboards will have a ``gpio.c`` file in their mainboard directory. This
file typically contains tables which describe the configuration of the GPIO
registers. Since these registers could be different on a per-SoC or per
SoC-family basis, you may need to consult the datasheet for your SoC to find out
how to appropriately set these registers. In addition, some mainboards are
based on a baseboard/variant model, where several variant mainboards may share a
lot of their circuitry and ICs and the commonality between the boards is
collected into a virtual ``baseboard.`` In that case, the GPIOs which are shared
between multiple boards are placed in the baseboard's ``gpio.c`` file, while the
ones that are board-specific go into each variant's ``gpio.c`` file.

## Intel SoCs

Many newer Intel SoCs share a common IP block for GPIOs, and that commonality
has been taken advantage of in coreboot, which has a large set of macros that
can be used to describe the configuration of each GPIO pad. This file lives in
``src/soc/intel/common/block/include/intelblocks/gpio_defs.h``.

### Older Intel SoCs

Baytrail and Braswell, for example, simply expect the mainboard to supply a
callback, `mainboard_get_gpios` which returns an array of `struct soc_gpio`
objects, defining the configuration of each pin.

### AMD SoCs

Some AMD SoCs use a list of `struct soc_amd_gpio` objects to define the
register values configuring each pin, similar to Intel.

### Register details

GPIO configuration registers typically control properties such as:
1. Input / Output
2. Pullups / Pulldowns
3. Termination
4. Tx / Rx Disable
5. Which reset signal to use
6. Native Function / IO
7. Interrupts
    * IRQ routing (e.g. on x86, APIC, SCI, SMI)
    * Edge or Level Triggered
    * Active High or Active Low
8. Debouncing

## Configuring GPIOs for pre-ramstage

coreboot provides for several SoC-specific and mainboard-specific callbacks at
specific points in time, such as bootblock-early, bootblock, romstage entry,
pre-silicon init, pre-RAM init, or post-RAM init. The GPIOs that are
configured in either bootblock or romstage, depending on when they are needed,
are denoted the "early" GPIOs. Some mainboard will use
``bootblock_mainboard_init()`` to configure their early GPIOs, and this is
probably a good place to start. Many mainboards will declare their GPIO
configuration as structs, i.e. (Intel),

```C
struct pad_config {
    /* offset of pad within community */
        int             pad;
    /* Pad config data corresponding to DW0, DW1,.... */
        uint32_t        pad_config[GPIO_NUM_PAD_CFG_REGS];
};
```

and will usually place these in an array, one for each pad to be configured.
Mainboards using Intel SoCs can use a library which combines common
configurations together into a set of macros, e.g.,

```C
    /* Native function configuration */
    #define PAD_CFG_NF(pad, pull, rst, func)
    /* General purpose output, no pullup/down. */
    #define PAD_CFG_GPO(pad, val, rst)
    /* General purpose output, with termination specified */
    #define PAD_CFG_TERM_GPO(pad, val, pull, rst)
    /* General purpose output, no pullup/down. */
    #define PAD_CFG_GPO_GPIO_DRIVER(pad, val, rst, pull)
    /* General purpose input */
    #define PAD_CFG_GPI(pad, pull, rst)
```
etc.

## Configuring GPIOs for ramstage and beyond...

In ramstage, most mainboards will configure the rest of their GPIOs for the
function they will be performing while the device is active. The goal is the
same as above in bootblock; another ``static const`` array is created, and the
rest of the GPIO registers are programmed.

In the baseboard/variant model described above, the baseboard will provide the
configuration for the GPIOs which are configured identically between variants,
and will provide a mechanism for a variant to override the baseboard's
configuration. This is usually done via two tables: the baseboard table and the
variant's override table.

This configuration is often hooked into the mainboard's `enable_dev` callback,
defined in its `struct chip_operations`.

## Unconnected and unused pads

In digital electronics, it is generally recommended to tie unconnected GPIOs to
a defined signal like VCC or GND by setting their direction to output, adding an
external pull resistor or configuring an internal pull resistor. This is done to
prevent floating of the pin state, which can cause various issues like EMI,
higher power usage due to continuously switching logic, etc.

On Intel PCHs from Sunrise Point onwards, termination of unconnected GPIOs is
explicitly not required, when the input buffer is disabled by setting the bit
`GPIORXDIS` which effectively disconnects the pad from the internal logic. All
pads defaulting to GPIO mode have this bit set. However, in the mainboard's
GPIO configuration the macro `PAD_NC(pad, NONE)` can be used to explicitly
configure a pad as unconnected.

In case there are no schematics available for a board and the vendor set a
pad to something like `GPIORXDIS=1`, `GPIOTXDIS=1` with an internal pull
resistor, an unconnected or otherwise unused pad can be assumed. In this case it
is recommended to keep the pull resistor, because the external circuit might
rely on it.

Unconnected pads defaulting to a native function (input and output) usually
don't need to be configured as GPIO with the `GPIORXDIS` bit set. For clarity
and documentation purpose the macro may be used as well for them.

Some pads configured as native input function explicitly require external
pull-ups when being unused, according to the PDGs:
- eDP_HPD
- SMBCLK/SMBDATA
- SML0CLK/SML0DATA/SML0ALERT
- SATAGP*

When the board was designed correctly, nothing needs to be done for them
explicitly, while using `PAD_NC(pad, NONE)` can act as documentation. If such a
pad is missing the external pull resistor due to bad board design, the pad
should be configured with `PAD_NC(pad, NONE)` anyway to disconnect it
internally.

## Potential issues (gotchas!)

There are a couple of configurations that you need to especially careful about,
as they can have a large impact on your mainboard.

The first is configuring a pin as an output, when it was designed to be an
input. There is a real risk in this case of short-circuiting a component which
could cause catastrophic failures, up to and including your mainboard!

### Intel SoCs

As per Intel Platform Controller Hub (PCH) EDS since Skylake, a GPIO PAD register
supports four different types of GPIO reset as:

```eval_rst
+------------------------+----------------+-------------+-------------+
|                        |                |         PAD Reset ?       |
+ PAD Reset Config       + Platform Reset +-------------+-------------+
|                        |                |     GPP     |     GPD     |
+========================+================+=============+=============+
| | 00 - Power Good      |  Warm Reset    |     N       |    N        |
| | (GPP: RSMRST,        +----------------+-------------+-------------+
| | GPD: DSW_PWROK)      |  Cold Reset    |     N       |    N        |
|                        |----------------+-------------+-------------+
|                        |  S3/S4/S5      |     N       |    N        |
|                        +----------------+-------------+-------------+
|                        |  Global Reset  |     N       |    N        |
|                        +----------------+-------------+-------------+
|                        |  Deep Sx       |     Y       |    N        |
|                        +----------------+-------------+-------------+
|                        |  G3            |     Y       |    Y        |
+------------------------+----------------+-------------+-------------+
| 01 - Deep              |  Warm Reset    |     Y       |    Y        |
|                        +----------------+-------------+-------------+
|                        |  Cold Reset    |     Y       |    Y        |
|                        +----------------+-------------+-------------+
|                        |  S3/S4/S5      |     N       |    N        |
|                        +----------------+-------------+-------------+
|                        |  Global Reset  |     Y       |    Y        |
|                        +----------------+-------------+-------------+
|                        |  Deep Sx       |     Y       |    Y        |
|                        +----------------+-------------+-------------+
|                        |  G3            |     Y       |    Y        |
+------------------------+----------------+-------------+-------------+
| 10 - Host Reset/PLTRST |  Warm Reset    |     Y       |    Y        |
|                        +----------------+-------------+-------------+
|                        |  Cold Reset    |     Y       |    Y        |
|                        +----------------+-------------+-------------+
|                        |  S3/S4/S5      |     Y       |    Y        |
|                        +----------------+-------------+-------------+
|                        |  Global Reset  |     Y       |    Y        |
|                        +----------------+-------------+-------------+
|                        |  Deep Sx       |     Y       |    Y        |
|                        +----------------+-------------+-------------+
|                        |  G3            |     Y       |    Y        |
+------------------------+----------------+-------------+-------------+
| | 11 - Resume Reset    |  Warm Reset    |     -       |    N        |
| | (GPP: Reserved,      +----------------+-------------+-------------+
| | GPD: RSMRST)         |  Cold Reset    |     -       |    N        |
|                        +----------------+-------------+-------------+
|                        |  S3/S4/S5      |     -       |    N        |
|                        +----------------+-------------+-------------+
|                        |  Global Reset  |     -       |    N        |
|                        +----------------+-------------+-------------+
|                        |  Deep Sx       |     -       |    Y        |
|                        +----------------+-------------+-------------+
|                        |  G3            |     -       |    Y        |
+------------------------+----------------+-------------+-------------+
```

Each GPIO Community has a Pad Configuration Lock register for a GPP allowing locking
specific register fields in the PAD configuration register.

The Pad Config Lock registers reset type is default hardcoded to **Power Good** and
it's **not** configurable by GPIO PAD DW0.PadRstCfg. Hence, it may appear that for a GPP,
the Pad Reset Config (DW0 Bit 31) is configured differently from `Power Good`.

This would create confusion where the Pad configuration is returned to its `default`
value but remains `locked`, this would prevent software to reprogram the GPP.
Additionally, this means software can't rely on GPIOs being reset by PLTRST# or Sx entry.

Hence, as per GPIO BIOS Writers Guide (BWG) it's recommended to change the Pad Reset
Configuration for lock GPP as `Power Good` so that pad configuration and lock bit are
always in sync and can be reset at the same time.

## Soft Straps

Soft straps, that can be configured by the vendor in the Intel Flash Image Tool
(FIT), can influence some pads' default mode. It is possible to select either a
native function or GPIO mode for some pads on non-server SoCs, while on server
SoCs most pads can be controlled. Thus, it is generally recommended to always
configure all pads and don't just rely on the defaults mentioned in the
datasheet(s) which might not reflect what the vendor configured.

## Pad-related known issues and workarounds

### LPC_CLKRUNB blocks S0ix states when board uses eSPI

When using eSPI, the pad implementing `LPC_CLKRUNB` must be set to GPIO mode.
Other pin settings i.e. Rx path enable/disable, Tx path enable/disable, pull up
enable/disable etc are ignored. Leaving this pin in native mode will keep the
LPC Controller awake and prevent S0ix entry. This issues is know at least on
Apollolake and Geminilake.
