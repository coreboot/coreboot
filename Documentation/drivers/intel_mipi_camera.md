# Intel MIPI camera ACPI (SSDT) generator

## Introduction

The Intel MIPI camera driver (`src/drivers/intel/mipi_camera`) generates SSDT
entries that describe MIPI CSI camera topology to the operating system. It
emits the binary data blocks and ACPI methods expected by Intel camera stacks
(SSDB, optional CLDB / PWDB, power resources, graph `_DSD`, and related
objects).

Typical consumers:

- ChromeOS / ChromeOS-derived kernels using Intel IPU camera support
- Windows INT347x camera drivers
- Mainline Linux (`cio2-bridge`, `ipu3`, sensor drivers such as `imx208`,
  plus optional INT3472 control-logic / regulator helpers)

Boards describe cameras in `devicetree.cb` with `chip drivers/intel/mipi_camera`
(and optionally `chip drivers/intel/mipi_camera/pmic`). The driver attaches
`acpi_fill_ssdt` ops at enable time and writes into the SSDT.

For general chip/driver wiring, see [Driver Devicetree Entries](dt_entries.md).

### Architecture (conceptual)

```text
                    +------------------+
                    |  CIO2 / IPU      |  port/_DSD (lanes, remote-endpoint)
                    |  (PCI + nested   |
                    |   mipi_camera)   |
                    +--------+---------+
                             ^
                             | media graph
                             v
  +------------------+  +----+------------------+  +------------------+
  | INT3472 PMIC/CL  |  | SENSOR (SSDB, graph   |  | VCM / NVM        |
  | CLDB + GPIO/I2C  |<-| _DSD, optional PRIC)  |  | (ChromeOS: own   |
  | _DSM; optional   |  | Win/Linux: VCM/NVM    |  |  devices;        |
  | CPMC/reset_once  |  | addresses in _CRS)    |  |  Win: packed)    |
  +------------------+  +-----------------------+  +------------------+
```

The INT3472 / PMIC block on the left is **optional**. Most JSL+ Chromebook and
RVP trees have no `mipi_camera/pmic` chip (sensor PRIC + PCH IMGCLKOUT only).
INT3472 appears mainly on older SKL poppy boards (discrete or TPS68470).

On Windows / mainline Linux for **SKL/KBL**, iGPU ACPI often also needs a
non-VGA child such as `SKC0` (via `drivers/gfx/generic`) so the IPU binds next
to the panel device. That is outside this driver. JSL+ platforms typically do
**not** need an `SKC0` child for camera; they may still select
`DRIVERS_GFX_GENERIC` for unrelated display ACPI.

## ACPI generation modes

`DRIVERS_INTEL_MIPI_CAMERA` selects one of two ACPI layouts:

```{eval-rst}
+----------------------------------+----------------------------------------+------------------------------------------------------------------+
| Kconfig                          | Typical use                            | Layout                                                           |
+==================================+========================================+==================================================================+
| ``MIPI_ACPI_TYPE_CHROMEOS``      | ChromeOS (default when ``CHROMEOS``)   | Separate ACPI devices for sensor, VCM, NVM, etc. IPU/CIO2 is     |
|                                  |                                        | modeled separately from the iGPU.                                |
+----------------------------------+----------------------------------------+------------------------------------------------------------------+
| ``MIPI_ACPI_TYPE_WINDOWS_LINUX`` | Windows and mainline Linux (default    | Sensor-centric SSDT: I2C ``INTEL_ACPI_CAMERA_SENSOR`` devices,   |
|                                  | otherwise)                             | and optional CIO2 port/``_DSD`` when a CIO2 chip is nested.      |
|                                  |                                        | Optional VCM/NVM I2C addresses are packed into the sensor        |
|                                  |                                        | ``_CRS``. Separate VCM/NVM chip nodes in the devicetree do       |
|                                  |                                        | **not** produce ACPI devices in this mode.                       |
+----------------------------------+----------------------------------------+------------------------------------------------------------------+
```
Both modes emit sensor graph `_DSD` (ports / endpoints / link-frequencies) and
an `SSDB` method. Graph `_DSD` serves mainline `cio2-bridge` / libcamera; SSDB
remains the primary blob for Intel / Windows INT347x.

### What Win/Linux SSDT generation includes

For `MIPI_ACPI_TYPE_WINDOWS_LINUX`, `camera_fill_ssdt` handles:

- `INTEL_ACPI_CAMERA_CIO2` nested under the IPU/CIO PCI device -- with
  `scope_into_parent`, Scope into the existing DSDT object (`Device (CIO2)`
  / `Device (IPU0)`); otherwise create `Device ()` under the PCI parent and
  emit port/`_DSD` (typical JSL+ Chromebooks with no DSDT stub)
- I2C `INTEL_ACPI_CAMERA_SENSOR` -- full sensor device (SSDB, graph, `_DSM`,
  optional PRIC)

It returns early for other types (VCM, NVM, IMGU, ...). Put companion addresses
on the sensor via `ssdb.vcm_type` / `vcm_address` and `ssdb.rom_type` /
`rom_address` when Windows needs them (extra I2C resources in `_CRS` only).
Do not expect a DT `NVM0` / `VCM0` chip to appear in the SSDT on Win/Linux
builds (ChromeOS multi-device mode still emits those devices). Graph
`lens-focus` references to a separate VCM device are ChromeOS-only.

### ChromeOS ASL vs Win/Linux DT (`VARIANT_HAS_CAMERA_ACPI`)

`VARIANT_HAS_CAMERA_ACPI` is a **mainboard** Kconfig (not under
`drivers/intel/mipi_camera`). It is orthogonal to the MIPI ACPI-type choice.

Some boards keep **static camera ASL for ChromeOS builds** and **MIPI
devicetree SSDT for Windows/Linux builds**. That is not a mixed ACPI output:
each firmware image uses one path or the other. The board tree may contain
both the ASL include (gated by `VARIANT_HAS_CAMERA_ACPI`) and DT camera chips
(used when that Kconfig is off / Win-Linux ACPI type is selected).

Typical arrangement:

- Always select `DRIVERS_INTEL_MIPI_CAMERA` (and `_HAS_PMIC` if needed) so
  symbols link on both builds.
- ChromeOS: `select VARIANT_HAS_CAMERA_ACPI if CHROMEOS` -- MIPI camera
  `enable_dev` leaves device ops unset so the SSDT generator does not
  duplicate the ASL.
- The PMIC chip attaches ops only for `MIPI_ACPI_TYPE_WINDOWS_LINUX`, so
  ChromeOS does not get a second control-logic device.
- Win/Linux builds omit the ASL include (including `ipu_*.asl`). Nest the
  port chip under `cio` or `ipu` when firmware must still emit port/`_DSD`.
  On SKL IPU3, leave CIO2 un-nested when a packed sensor VCM should come
  from SSDB via `ipu-bridge` (nautilus / soraka).
- Prefer a single DT path for both OSes when the topology can be expressed
  completely (as with atlas).

## Choosing a power model

Most current Chromebook camera trees (JSL / TGL / ADL / MTL / PTL and newer)
use **local sensor PRIC + PCH IMGCLKOUT** under `device ref ipu`, with
**no** INT3472 / TPS68470 PMIC chip.

```{eval-rst}
+--------------------------------------------------------------+--------------------------------------------------------------------+
| Topology                                                     | Typical setup                                                      |
+==============================================================+====================================================================+
| Modern IPU boards (dedede, brya, rex, fatcat, ...)           | Nest under ``device ref ipu``; sensor ``has_power_resource`` +     |
|                                                              | ``clk_panel`` IMGCLKOUT; separate VCM/NVM chips often share CAM    |
|                                                              | ``PRIC`` via ``pr0``                                               |
+--------------------------------------------------------------+--------------------------------------------------------------------+
| Discrete GPIOs; sensor needs ACPI ``_PR0`` (e.g. Sony IMX208 | Discrete INT3472 (``DSC0``) for Windows CLDB / GPIO ``_DSM``,      |
| on SKL)                                                      | **plus** sensor-local ``PRIC``                                     |
+--------------------------------------------------------------+--------------------------------------------------------------------+
| TPS68470 (or similar) regulators via INT3472                 | ``CLP0`` PMIC with ``reset_once``; sensors use ``_DEP`` on the     |
|                                                              | PMIC and usually **no** local PRIC                                 |
+--------------------------------------------------------------+--------------------------------------------------------------------+
| ChromeOS multi-device VCM sharing a PWREN rail               | Separate VCM chip with PWREN-only ``PRIC``; guarded GPIO           |
|                                                              | refcount with the sensor                                           |
+--------------------------------------------------------------+--------------------------------------------------------------------+
```
## Device types

`device_type` selects what the chip instance represents:

| `device_type` | Role |
|---|---|
| `INTEL_ACPI_CAMERA_CIO2` | CSI-2 receiver / bridge ports and endpoints |
| `INTEL_ACPI_CAMERA_IMGU` | Image processing unit (rarely used in current board trees; SoC-specific) |
| `INTEL_ACPI_CAMERA_SENSOR` | Image sensor (SSDB, optional local power resource) |
| `INTEL_ACPI_CAMERA_VCM` | Voice coil motor / lens (ChromeOS multi-device SSDT) |
| `INTEL_ACPI_CAMERA_NVM` | EEPROM / NVM (ChromeOS multi-device SSDT) |
| `INTEL_ACPI_CAMERA_PMIC` | Reserved; use the separate PMIC chip driver |

Each instance needs an appropriate device path:

- Sensors / VCM / NVM / TPS68470 PMIC: usually `device i2c <addr>` under an I2C
  controller
- Discrete INT3472 control logic: often `device generic <n>` (no I2C resource)
- CIO2 / IPU port data: usually `device generic <n>` nested under the CIO2 or
  IPU PCI device

## Kconfig summary

- `DRIVERS_INTEL_MIPI_CAMERA` -- enable the SSDT generator
- `MIPI_ACPI_TYPE_WINDOWS_LINUX` / `MIPI_ACPI_TYPE_CHROMEOS` -- ACPI layout
- `DRIVERS_INTEL_MIPI_CAMERA_HAS_PMIC` -- build the optional INT3472 /
  control-logic generator (`drivers/intel/mipi_camera/pmic`)
- `DRIVERS_INTEL_MIPI_SUPPORTS_PRE_PRODUCTION_SOC` -- hint for IPU firmware
  selection on pre-production silicon

Related board options (not in this driver's Kconfig):

- `VARIANT_HAS_CAMERA_ACPI` -- skip MIPI camera ops when static ASL owns ChromeOS
- `DRIVERS_GFX_GENERIC` -- used for panel / other iGPU ACPI; on SKL/KBL Win
  builds often paired with an `SKC0` child for IPU binding (not required for
  the common JSL+ camera pattern)

Mainboards that need Windows or mainline Linux discrete / TPS68470 control
logic should select `DRIVERS_INTEL_MIPI_CAMERA_HAS_PMIC` and add a PMIC chip
node. `cldb.platform` must be set explicitly or the PMIC device is not created.

## Required fields (validation)

Sensor chips (`INTEL_ACPI_CAMERA_SENSOR`):

- `ssdb.lanes_used` -- required (non-zero); enable logs an error if unset
- `ssdb.platform` -- required; enable logs an error if unset
- `rom_address` -- required when `ssdb.rom_type` is non-zero
- `vcm_address` -- required when `ssdb.vcm_type` is non-zero

PMIC / control-logic chips:

- `cldb.platform` -- required; the ACPI device is not created if unset
- Discrete type (`CL_TYPE_DISCRETE`) expects card SKU `SKU_CRD_D` (defaulted if
  unset; mismatched SKU fails creation)
- TPS68470 (`CL_TYPE_TPS68470`) requires an I2C device path

## Common configuration fields

Configured via `register` on `chip drivers/intel/mipi_camera` (see
`src/drivers/intel/mipi_camera/chip.h`).

### Identity and status

- `acpi_hid`, `acpi_uid`, `acpi_name` -- ACPI identity (`_HID` / `_UID` / name)
- `chip_name` / `sensor_name` -- human-readable strings (`_DDN` selection depends
  on ACPI type)
- `acpi_dep` -- absolute ACPI path for `_DEP` (for example
  `\\_SB.PCI0.I2C3.DSC0`). Relative names are not reliable across I2C scopes.
- `acpi_sta` -- if set, `_STA` returns this named object (for example
  `\\_SB.PCI0.CSTA`) instead of a fixed status. Some boards (e.g. poppy)
  publish `CSTA` from mainboard SSDT / CFR so cameras can be gated at runtime.

### Sensor SSDB and link data

- `ssdb.*` -- Sensor-Specific Data Block fields (platform, lanes, VCM/ROM types,
  card SKU, MCLK, and so on). Use the `ssdb.` prefix in `register` lines
  (for example `ssdb.lanes_used`, `ssdb.platform`).
- `ssdb.lanes_used` -- lane **count**; SSDB `ssdb.lane_config` is derived as
  `lanes_used - 1` when filling defaults (because `0` is a valid
  `lane_config` and cannot mean "unset")
- `link_freq[]` / `num_freq_entries` -- CSI link frequencies for graph `_DSD`
- `remote_name` -- sensor remote-endpoint target (defaults to the PCI CIO2/IPU
  ACPI path, or `\\_SB.PCI0.CIO2` when that device is absent)
- `vcm_address` / `rom_address` -- companion I2C addresses when
  `ssdb.vcm_type` / `ssdb.rom_type` are non-zero (packed into sensor `_CRS` on
  Win/Linux)
- `pld` -- physical location (`_PLD`)

### PWDB (optional)

`pwdb[]` / `num_pwdb_entries` can emit a `PWDB` method (power sequencing data
block) for Intel consumers that understand it. Few current board trees use it;
see `struct intel_pwdb` in `chip.h` / `ssdb.h` if you need it. Prefer local
`PRIC` sequences or INT3472 regulators for new ports unless you know the OS
stack requires PWDB.

### Local power resource (PRIC)

When `has_power_resource` is true, the driver emits a local `PRIC`
PowerResource and references it from `_PR0`.

- `gpio_panel` / `clk_panel` -- GPIO and IMGCLK resources indexed by power ops
- `on_seq` / `off_seq` -- ordered enable/disable steps via
  `SEQ_OPS_GPIO_*` / `SEQ_OPS_CLK_*`

Shared GPIOs used by multiple devices are reference-counted through guarded
operations so concurrent `_ON` / `_OFF` sequences stay coherent (for example a
VCM PWREN-only resource sharing a rail with the sensor).

Local PRIC is emitted whenever `has_power_resource` is set. It is **not**
suppressed merely because `acpi_dep` points at an INT3472 device. Boards may
intentionally use both:

- INT3472 / CLDB / GPIO `_DSM` for Windows (and some Linux clkdev paths)
- Sensor `_PR0` / PRIC for sensors whose Linux drivers do not consume INT3472
  regulators (for example Sony IMX208)

On ChromeOS ACPI type, sensor `_DEP` targeting a Win-only INT3472 is skipped
when the sensor also has a local PRIC, because the PMIC chip is not emitted in
that mode.

### Sensor GPIO `_DSM`

When a sensor ACPI device is generated, it always exposes the INT347x GPIO
`_DSM` UUID (both ACPI types). Current boards do **not** describe camera GPIOs
through that DSM: power/reset/clock lines live in the sensor `PRIC` sequence
(`gpio_panel` / `clk_panel`) and/or on an optional INT3472 control-logic
device. The sensor therefore reports a GPIO **count of zero**. Emitting the
UUID with an empty set still satisfies Windows bind; omitting the UUID fails
the DSM. INT347x queries function 1 (count) on this UUID and does not use a
function-0 "query supported functions" step (same pattern as the I2C-address
UUID DSM).

### CIO2 / IPU port graph

For `INTEL_ACPI_CAMERA_CIO2`:

- `cio2_num_ports`
- `cio2_lanes_used[]`
- `cio2_lane_endpoint[]` -- absolute paths to sensor devices
- `cio2_prt[]` -- port numbers
- `scope_into_parent` -- Scope into the parent PCI ACPI object instead of
  creating a new `Device ()` (set when DSDT already declares `CIO2` /
  `IPU0`, e.g. SKL/KBL `ipu.asl`)

Skylake/KBL DSDT declares `Device (CIO2)`. Nested CIO2 chips set
`scope_into_parent` and emit port/`_DSD` there. JSL+ boards typically have
no DSDT `IPU0`; leave `scope_into_parent` unset so the SSDT creates
`Device (IPU0)` under the PCI parent. Sensor remote-endpoints should name
that same parent (`IPU0` / `CIO2`).

Nesting a CIO2 chip under `device ref cio` or `device ref ipu` is how
firmware emits the CIO2/IPU side of the media graph after static `ipu_*.asl`
is dropped. Enabling `cio` / `ipu` alone does not emit port/`_DSD`.

On mainline Linux, `ipu-bridge` (formerly cio2-bridge) **skips** SSDB-based
graph and VCM synthesis when CIO2 already has a firmware graph endpoint. IPU3
VCM is the second `I2cSerialBus` on the **sensor** (packed `_CRS` +
`ssdb.vcm_type`). Boards with a VCM (nautilus, soraka) therefore leave
`device ref cio on end` so the bridge can instantiate the lens from SSDB.
Boards without a VCM that are not in the bridge HID table (atlas / IMX208)
still nest CIO2 so the CSI link exists as ACPI graph.

### VCM / NVM extras

- `pr0` -- external PowerResource path when the device does not own a local PRIC
- `low_power_probe` / `max_dstate_for_probe` -- probe D-state hints (`_DSC` /
  `i2c-allow-low-power-probe`)
- `nvm_*` / `vcm_compat` -- NVM geometry and compatible strings (ChromeOS
  multi-device devices)

## PMIC / control-logic chip

Path: `chip drivers/intel/mipi_camera/pmic` (`src/drivers/intel/mipi_camera/pmic`).

Emits INT3472-style control logic for Windows / mainline Linux:

- CLDB (Control Logic Data Block); `cldb.platform` is required
- `_DSM` for TPS68470 I2C addressing or discrete GPIO / IMGCLK control
- Optional PowerResource (`CPMC` when `reset_once` is set; otherwise the common
  ACPI power-resource helper). `reset_once` pulses reset only on the first
  `_ON` after boot, matching common ChromeOS TPS68470 ASL, and leaves reset
  alone across S3/S0ix.

### Path and CLDB type

| `cldb.pmic_type` | Typical DT path | Notes |
|---|---|---|
| `CL_TYPE_DISCRETE` | `device generic <n>` | GPIO panel via `gpio_panel[]`; SKU should be `SKU_CRD_D` |
| `CL_TYPE_TPS68470` | `device i2c <addr>` | I2C `_DSM`; often `has_power_resource` + `reset_once` |

Discrete GPIO panel entries use `MIPICAM_GPIO_TYPE_*` (power, clock, reset,
and related functions). For Linux INT3472, `gpio_active_value` on a reset GPIO
is the level that corresponds to **sensor out of reset** (not "assert reset").

### IMGCLK `_DSM` vs clock GPIO

For `CL_TYPE_DISCRETE`, the generator emits the IMGCLK `_DSM` UUID only when
**no** `MIPICAM_GPIO_TYPE_CLOCK` entry is present. IMGCLK control calls
`\\_SB.PCI0.MCON` / `MCOF` when those methods exist (typical on TGL+ camera
clock ASL). SKL/KBL boards that clock the sensor with a GPIO should list that
GPIO as type CLOCK and should not rely on IMGCLK.

The PMIC enable path is active for `MIPI_ACPI_TYPE_WINDOWS_LINUX`. On ChromeOS
ACPI type it does not attach ops, so boards that keep ChromeOS ASL and Win/Linux
DT in the same tree do not emit a second control-logic device under ChromeOS.

## Nesting under CIO2 / IPU (scan_bus)

Static `chip drivers/intel/mipi_camera` children only run `enable_dev` /
`acpi_fill_ssdt` if the parent PCI device scans its downstream bus.

- **JSL and newer (common case):** nest under `device ref ipu`.
  `SOC_INTEL_COMMON_BLOCK_IPU` provides `ipu_pci_ops` with `scan_generic_bus`
  and binds via PCI device ID. Leave `scope_into_parent` unset so the SSDT
  creates `Device (IPU0)` under that PCI parent.
- **Skylake/KBL:** nest under `device ref cio`. Chipset `device pci 14.3`
  uses `ops cio_ops` (`src/soc/intel/skylake/cio.c`) so nested camera chips
  are scanned without a PCI ID table. Set `scope_into_parent` so the chip
  Scopes into DSDT `Device (CIO2)`.

Boards that only enable CIO2/IPU as a bare PCI function (no children) are
unaffected; an empty generic bus scan is a no-op. Nest the CIO2/IPU chip when
firmware should emit port/`_DSD`. On SKL IPU3 Win/Linux, do **not** nest CIO2
if a packed sensor VCM must be left for `ipu-bridge` to synthesize (see
nautilus / soraka). JSL+ IPU boards still nest under `device ref ipu`.

## Example: IPU + local PRIC + IMGCLKOUT (common JSL+)

Most Google Chromebook camera trees after Skylake follow this pattern: nest
under `device ref ipu`, drive MCLK with PCH IMGCLKOUT, and power the sensor
with a local `PRIC`. There is usually **no** `mipi_camera/pmic` chip.

Illustrative fragment (abbreviated). Full trees include
`src/mainboard/google/dedede/variants/drawcia/overridetree.cb`,
`.../brya/variants/redrix/overridetree.cb`,
`.../rex/variants/rex0/overridetree.cb`, and
`.../fatcat/variants/fatcat/overridetree.cb`.

```text
device ref ipu on
	chip drivers/intel/mipi_camera
		register "acpi_name" = ""IPU0""
		register "device_type" = "INTEL_ACPI_CAMERA_CIO2"
		register "cio2_num_ports" = "1"
		register "cio2_lanes_used[0]" = "2"
		register "cio2_lane_endpoint[0]" = ""\\_SB.PCI0.I2C3.CAM0""
		register "cio2_prt[0]" = "0"
		device generic 0 on end
	end
end

device ref i2c3 on
	chip drivers/intel/mipi_camera
		register "acpi_name" = ""CAM0""
		register "device_type" = "INTEL_ACPI_CAMERA_SENSOR"
		register "ssdb.lanes_used" = "2"
		register "ssdb.platform" = "PLAT_JSL"   # or PLAT_ADL / MTL / PTL / ...
		register "has_power_resource" = "true"
		register "clk_panel.clks[0].clknum" = "IMGCLKOUT_3"
		register "clk_panel.clks[0].freq" = "FREQ_19_2_MHZ"
		# gpio_panel + on_seq / off_seq for reset / power / clock
		device i2c ... on end
	end
	chip drivers/intel/mipi_camera
		register "acpi_name" = ""NVM0""
		register "device_type" = "INTEL_ACPI_CAMERA_NVM"
		register "pr0" = ""\\_SB.PCI0.I2C3.CAM0.PRIC""
		device i2c ... on end
	end
end
```

Notes:

1. Select `SOC_INTEL_COMMON_BLOCK_IPU` (often via the SoC / mainboard Kconfig)
   so `ipu` gets `scan_generic_bus`.
2. Multi-camera boards add more CIO2 ports and sensors (often on other I2C
   controllers); `cio2_lane_endpoint[]` must match each sensor path.
3. ChromeOS multi-device mode emits separate VCM/NVM devices; Win/Linux packs
   companion addresses into the sensor `_CRS` instead.
4. `clk_panel.clks[].clknum` may be an `IMGCLKOUT_n` macro or a raw index
   (`0`, `1`, ...). Both forms appear in tree; they must match the SoC IMGCLKOUT
   numbering used by `MCON` / camera clock ASL.

## Example: discrete GPIO power + CIO2 (Atlas-style SKL)

Older SKL/KBL boards may still use `device ref cio` plus an INT3472 control-
logic device. Full tree:
`src/mainboard/google/poppy/variants/atlas/devicetree.cb`.

Also select `DRIVERS_GFX_GENERIC` and add an iGPU `SKC0` child for Windows IPU
binding (not shown).

```text
device ref cio on
	chip drivers/intel/mipi_camera
		register "device_type" = "INTEL_ACPI_CAMERA_CIO2"
		register "cio2_num_ports" = "1"
		register "cio2_lanes_used[0]" = "2"
		register "cio2_lane_endpoint[0]" = ""\\_SB.PCI0.I2C3.CAM0""
		register "cio2_prt[0]" = "0"
		device generic 0 on end
	end
end

device ref i2c3 on
	chip drivers/intel/mipi_camera/pmic
		register "acpi_name" = ""DSC0""
		register "cldb.pmic_type" = "CL_TYPE_DISCRETE"
		register "cldb.platform" = "PLAT_SKC"
		# power / clock / reset gpio_panel[...]
		device generic 0 on end
	end
	chip drivers/intel/mipi_camera
		register "acpi_hid" = ""INT3478""
		register "acpi_name" = ""CAM0""
		register "acpi_dep" = ""\\_SB.PCI0.I2C3.DSC0""
		register "device_type" = "INTEL_ACPI_CAMERA_SENSOR"
		register "ssdb.lanes_used" = "2"
		register "ssdb.platform" = "PLAT_SKC"
		register "has_power_resource" = "true"
		# on_seq / off_seq GPIO power sequence (local PRIC)
		device i2c 10 on end
	end
end
```

Notes for this pattern:

1. Nest the port chip under the CSI PCI device (`device ref cio` on SKL/KBL)
   so it is enabled and port/`_DSD` exists.
2. Point CIO2 endpoints and sensor remote-endpoints at consistent ACPI names.
3. Use DSC0 for Windows INT3472; keep CAM0 PRIC when the sensor needs ACPI `_PR0`.
4. Keep `acpi_dep` absolute.
5. Optional ChromeOS-only `NVM0` chip may sit beside CAM0; Win/Linux will not
   emit it -- put `rom_address` on the sensor if Windows needs the EEPROM.
6. `SKC0` under the iGPU is a SKL/KBL Win/Linux concern here; not part of the
   common JSL+ IPU pattern.

## Example: TPS68470 + reset_once (poppy nautilus / soraka)

SKL boards with a TPS68470 camera PMIC. Full trees:
`src/mainboard/google/poppy/variants/nautilus/devicetree.cb` and
`.../soraka/devicetree.cb`.

```text
device ref cio on end

device ref i2c2 on
	chip drivers/intel/mipi_camera/pmic
		register "acpi_name" = ""CLP0""
		register "cldb.pmic_type" = "CL_TYPE_TPS68470"
		register "cldb.platform" = "PLAT_SKC"
		register "has_power_resource" = "true"
		register "reset_once" = "true"
		register "use_gpio_for_status" = "true"
		register "enable_gpio" = "ACPI_GPIO_OUTPUT_ACTIVE_HIGH(...)"
		register "reset_gpio" = "ACPI_GPIO_OUTPUT_ACTIVE_LOW(...)"
		device i2c 4d on end
	end
	chip drivers/intel/mipi_camera
		register "acpi_name" = ""CAM0""
		register "acpi_dep" = ""\\_SB.PCI0.I2C2.CLP0""
		register "device_type" = "INTEL_ACPI_CAMERA_SENSOR"
		register "ssdb.lanes_used" = "4"
		register "ssdb.platform" = "PLAT_SKC"
		register "ssdb.vcm_type" = "..."
		register "vcm_address" = "0x0C"
		# usually no has_power_resource: regulators via INT3472
		device i2c 1a on end
	end
end
```

Leave CIO2 as a bare PCI function so firmware does not emit CIO2 port/`_DSD`.
Mainline `ipu-bridge` then builds the CSI graph and `lens-focus` from SSDB
and instantiates the VCM as I2C resource index 1 on CAM0. Nesting CIO2 here
makes the VCM disappear from the Linux media graph.

Soraka adds a front sensor on another I2C controller with the same bare `cio`
and packed companions on the rear CAM0. ChromeOS builds keep static ASL
(including CIO2 ports and a separate `VCM0`) via
`VARIANT_HAS_CAMERA_ACPI if CHROMEOS`; Win/Linux builds use this DT path.

## Example: VCM PWREN-only shared GPIO (Volteer-style)

When migrating from ASL, a VCM that only toggled a shared PWREN (not the full
sensor sequence) should not reuse the sensor's `PRIC`. Give the VCM its own
`has_power_resource` sequence on that GPIO; guarded refcounting keeps the rail
coherent with the sensor. Also restore `_DEP` on the sensor and
`low_power_probe` where the ASL had them.

See `src/mainboard/google/volteer/variants/*/overridetree.cb`.

## Debugging checklist

- Confirm cbmem shows the expected devices, for example
  `\\_SB.PCI0.IPU0: ... at PCI 05.0` (created device) or
  `\\_SB.PCI0.CIO2: ... (existing device)` (`scope_into_parent`), and
  `\\_SB.PCI0.I2C#.CAM#: ... at I2C 0x..`.
- Decompile SSDT/DSDT: sensor `_HID`/`SSDB`, optional IPU/CIO2 port/`_DSD`,
  optional INT3472 CLDB/`_DSM`, and `_PR0` / `PRIC` as designed.
- On Linux, verify the media graph links sensor -> CSI-2 -> CIO2/IPU. JSL+
  boards that omit the nested IPU chip have no firmware port/`_DSD`. On SKL
  IPU3, a nested CIO2 graph is required for HID-less sensors without a VCM
  (atlas) and must be **absent** when a packed VCM should come from SSDB
  (nautilus / soraka). A missing VCM with CIO2 port/`_DSD` present usually
  means the nest blocked `ipu-bridge` synthesis.
- Win/Linux builds must not reference a separate `VCM0` ACPI device in
  sensor `lens-focus`; VCM stays packed in the sensor `_CRS`.
- ChromeOS builds that still set `acpi_dep` on a Win-only INT3472: the driver
  skips that `_DEP` when the sensor has local PRIC; if you see probe-order
  issues, check whether you expected a dependency that was intentionally
  elided.
- Do not expect Win/Linux to emit both packed ROM in the sensor `_CRS` and a
  separate `NVM` ACPI device; only the sensor path is generated.
- Chip-ID / regulator failures often point at reset polarity (`gpio_active_value`
  / `active_value`), missing `_PR0`, or incorrect power sequence delays -- not
  at SSDB alone.
- Userspace stacks (libcamera IPA helpers, Bayer format negotiation) are
  outside this driver's scope once ACPI and V4L2 enumeration succeed.

## Reference board trees

### Common modern pattern (IPU + local PRIC + IMGCLKOUT)

| Board / variant | Notes |
|---|---|
| `mb/google/dedede/variants/drawcia` | JSL; `PLAT_JSL`; dual CIO2 ports; UFC sensor + NVM |
| `mb/google/dedede/variants/magolor` | JSL; fw_config alternate sensors |
| `mb/google/brya/variants/redrix` | ADL; UFC sensor + NVM; IMGCLKOUT |
| `mb/google/brya/variants/brya0` | ADL; world-facing + VCM + NVM + UFC |
| `mb/google/rex/variants/rex0` | MTL; dual ports across I2C; `PLAT_MTL` |
| `mb/google/fatcat/variants/fatcat` | PTL; dual sensors with VCM + NVM; `PLAT_PTL` |
| `mb/google/atria/variants/atria` | NVL-class; same IPU + PRIC pattern |
| `mb/google/volteer/variants/*` | TGL; multi-device; VCM PWREN-only / `_DEP` / low-power-probe |

Many other dedede / brya / nissa / rex variants follow the same IPU nesting and
local-PRIC model; the rows above are representative. Intel RVP trees
(`mb/intel/jasperlake_rvp`, `adlrvp`, `mtlrvp`, `ptlrvp`, ...) use the same
`device ref ipu` + local PRIC pattern.

### Skylake / Kaby Lake (CIO2 + optional PMIC)

| Board / variant | Notes |
|---|---|
| `mb/google/poppy/variants/atlas` | DT-only; discrete INT3472 + sensor PRIC + CIO2 nest + SKC0 (no VCM) |
| `mb/google/poppy/variants/nautilus` | ChromeOS ASL / Win-Linux DT; TPS68470 `reset_once`; packed VCM, bare `cio` |
| `mb/google/poppy/variants/soraka` | ChromeOS ASL / Win-Linux DT; dual camera; packed rear VCM, bare `cio` |

Only poppy currently selects `DRIVERS_INTEL_MIPI_CAMERA_HAS_PMIC` among Google
mainboards. Newer platforms do not use the PMIC chip generator.

## Source layout

| Path | Purpose |
|---|---|
| `src/drivers/intel/mipi_camera/camera.c` | Sensor / CIO2 / VCM / NVM SSDT generator |
| `src/drivers/intel/mipi_camera/chip.h` | Devicetree config for the main chip |
| `src/drivers/intel/mipi_camera/ssdb.h` | SSDB / PWDB structures and enums |
| `src/drivers/intel/mipi_camera/pmic/pmic.c` | INT3472 / CLDB SSDT generator |
| `src/drivers/intel/mipi_camera/pmic/chip.h` | PMIC / discrete control-logic config |
| `src/drivers/intel/mipi_camera/pmic/cldb.h` | CLDB structure and control-logic types |
| `src/soc/intel/skylake/cio.c` | Skylake CIO2 `scan_generic_bus` ops |
| `src/soc/intel/common/block/ipu/` | Newer SoC IPU PCI ops + IDs |
