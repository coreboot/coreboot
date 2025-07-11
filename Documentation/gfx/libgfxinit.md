libgfxinit - Native Graphics Initialization
===========================================

Introduction and Current State in coreboot
------------------------------------------

*libgfxinit* is a library of full-featured graphics initialization
(aka. modesetting) drivers. It's implemented in SPARK (a subset of
Ada with formal verification features). While not restricted to in
any way, it currently only supports Intel's integrated graphics
controllers (GMA).

Currently, it supports the Intel Core i3/i5/i7 processor line, HDMI
and DP on the Apollo Lake processors and everything but SDVO on G45
and GM45 chipsets. At the time of writing, G45, GM45, everything
from Arrandale to Coffee Lake, and Apollo Lake are verified to work
within *coreboot*.

GMA: Framebuffer Configuration
------------------------------

*coreboot* supports two different framebuffer setups. The default
enables the legacy VGA plane in textmode. Due to legacy hardware
constraints, only the first found display is enabled in this mode.
(cf. `src/drivers/intel/gma/text_fb/gma-gfx_init.adb`).

The second option sets up a high-resolution framebuffer with the
native resolution of the display if only one is detected, or the
smallest of all resolutions (per dimension) if multiple displays
are detected. This option is selected by
`CONFIG_FRAMEBUFFER_KEEP_VESA_MODE`.
(cf. `src/drivers/intel/gma/hires_fb/gma-gfx_init.adb`).

In any case, a smaller framebuffer is up-scaled to each display's
native resolution while keeping aspect ratio.

GMA: Hook-up in Chipset Initialization
--------------------------------------

Both configurations described above implement a procedure
`GMA.gfxinit()`:

    procedure gfxinit (lightup_ok : out int);

This procedure is exported as the C function `gma_gfxinit()` as
follows:

    void gma_gfxinit(int *lightup_ok);

* `lightup_ok`: returns whether the initialization succeeded `1` or
                failed `0`. Currently, only the case that no display
                could be found counts as failure. A failure at a
                later stage (e.g. failure to train a DP) is not
                propagated.

GMA: Per Board Configuration
----------------------------

In order to set up the display panel, see the
[display panel-specific documentation](/gfx/display-panel.md).

There are a few Kconfig symbols to consider. To indicate that a
board can initialize graphics through *libgfxinit*:

    select MAINBOARD_HAS_LIBGFXINIT

Internal ports share some hardware blocks (e.g. backlight, panel
power sequencer). Therefore, each system with an integrated panel
should set `GFX_GMA_PANEL_1_PORT` to the respective port, e.g.:

    config GFX_GMA_PANEL_1_PORT
            default "DP3"

For the most common cases, LVDS and eDP, exists a shorthand, one
can select either:

    select GFX_GMA_PANEL_1_ON_EDP	# the default, or
    select GFX_GMA_PANEL_1_ON_LVDS

Some newer chips feature a second block of panel control logic.
For this, `GFX_GMA_PANEL_2_PORT` can be set.

Boards with a DVI-I connector share the DDC (I2C) pins for both
analog and digital displays. In this case, *libgfxinit* needs to
know through which interface the EDID can be queried:

    select GFX_GMA_ANALOG_I2C_HDMI_B	# or
    select GFX_GMA_ANALOG_I2C_HDMI_C	# or
    select GFX_GMA_ANALOG_I2C_HDMI_D

*libgfxinit* needs to know which ports are implemented on a board
and should be probed for displays. There are two mechanisms to
constrain the list of ports to probe, 1. port presence straps on
the mainboard, and 2. a list of ports provided by *coreboot* (see
below).

Presence straps are configured via the state of certains pins of
the chipset at reset time. They are documented in the chipset's
datasheets. By default, *libgfxinit* honors these straps for
safety. However, some boards don't implement the straps correctly.
If ports are not strapped as implemented by error, one can select
an option to ignore the straps:

    select GFX_GMA_IGNORE_PRESENCE_STRAPS

In the opposite case, that ports are strapped as implemented,
but are actually unconnected, one has to make sure that the
list of ports in *coreboot* omits them.

The mapping between the physical ports and these entries depends on
the hardware implementation and can be recovered by testing or
studying the output of `intelvbttool` or `intel_vbt_decode`.
Each board has to implement the package `GMA.Mainboard` with a list:

    ports : HW.GFX.GMA.Display_Probing.Port_List;

or a function returning such a list:

    function ports return HW.GFX.GMA.Display_Probing.Port_List;

You can select from the following Ports:

    type Port_Type is
      (Disabled,	-- optionally terminates the list
       LVDS,
       eDP,
       DP1,
       DP2,
       DP3,
       HDMI1,		-- also DVI-D, or HDMI over DP++
       HDMI2,
       HDMI3,
       Analog);		-- legacy VGA port, or analog part of DVI-I

Each `DPx` and `HDMIx` pair share pins. If they are exposed as DP
ports, they are usually DP++ (aka. dual-mode DP) ports that can
also output HDMI signals through passive adapters. In this case,
both DPx and HDMIx should be listed.

A good example is the mainboard Kontron/KTQM77, it features two
DP++ ports (DP2/HDMI2, DP3/HDMI3), one DVI-I port (HDMI1/Analog),
eDP and LVDS. It defines `ports` as follows:

    ports : constant Port_List :=
      (DP2,
       DP3,
       HDMI1,
       HDMI2,
       HDMI3,
       Analog,
       LVDS,
       eDP,
       others => Disabled);

The `GMA.gfxinit()` procedure probes for display EDIDs in the
given order until all available pipes are taken. That's 1 pipe
in VGA textmode, 2 pipes in high-resolution mode until Sandy
Bridge, 3 pipes from Ivy Bridge on.
