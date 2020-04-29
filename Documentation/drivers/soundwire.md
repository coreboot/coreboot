# SoundWire Implementation in coreboot

## Introduction

SoundWire is an audio interface specification from the MIPI Alliance.

- Low complexity
- Low power
- Low latency
- Two pins (clock and data)
- Multi-drop capable
- Multiple audio streams
- Embedded control/command channel

The main *SoundWire Specification* is at version 1.2 and can be downloaded from
<https://mipi.org> but it is unfortunately only available to MIPI Alliance members.

There is a separate *SoundWire Discovery and Configuration (DisCo) Specification* which
is at version 1.0 and is available for non-members after providing name and email at
<https://resources.mipi.org/disco_soundwire>.

The coreboot implementation is based on the SoundWire DisCo Specification which defines
object hierarchy and properties for providing topology and configuration information to
OS kernel drivers via ACPI or DeviceTree.

SoundWire itself is architecture independent and the coreboot basic definition is also not
specific to any to any SoC.  The examples in this document use ACPI to generate properties,
but the same structures and properties would be needed in a DeviceTree implementation.

## Bus

The SoundWire bus commonly consists of two pins:

* Clock: A common clock signal distributed from the master to all of the slaves.
* Data: A shared data signal that can be driven by any of the devices, and has a defined
value when no device is driving it.

While most designs have one data lane it is possible for a multi-lane device to have up
to 8 data lanes and thus would have more than two pins.

A SoundWire bus consists of one master device, up to 11 slave devices, and an optional
monitor interface for debug.

SoundWire is an enumerable bus, but not a discoverable one.  That means it is required
for firmware to provide details about the connected devices to the OS.

### Controller

A SoundWire controller contains one or more master devices.  The handling of multiple
masters is left up to the implementation, they may share a clock or be operated
independently or entirely in tandem.  The master devices connected to a controller are
also referred to as links.

In coreboot the controller device is provided by the SoC or an add-in PCI card.

### Master

A SoundWire master (or link) device is responsible for clock and data handling, bus
management, and bit slot allocation.

In coreboot the definition of the master device is left up to the controller and the
mainboard should only need to know the controller's SoundWire topology (number of masters)
to configure `devicetree.cb`.

It may however be expected to provide some additional SoC-specific configuration data to
the controller, such as an input clock rate or a list of available masters that cannot
be determined at run time.

### Slave

SoundWire slave devices are connected to a master and respond to the two-wire control
information on the SoundWire bus.  There can be up to 11 slave devices on a bus and they
are capable of interrupting and waking the host.

Slave devices may also have master links which can be connected to other slave devices.
It is also possible for a multi-lane slave device to have multiple data lanes connected
to different combinations of master and slave devices.

In coreboot the slave device is defined by a codec driver which should be found in the
source tree at `src/drivers/soundwire`.

The mainboard provides:

* Master link that this slave device is connected to.
* Unique ID that this codec responds to on the SoundWire bus.
* Multi-lane mapping. (optional)

The codec driver provides:

* Slave device properties.
* Audio Mode properties including bus frequencies and sampling rates.
* Data Port 1-14 properties such as word lengths, interrupt support, channels.
* Data Port 0 and Bulk Register Access properties. (optional)

### Monitor

A SoundWire monitor device is defined that allows for test equipment to snoop the bus and
take over and issue commands.  The monitor interface is not defined for coreboot.

### Example SoundWire Bus

```
+---------------+                                       +---------------+
|               |                       Clock Signal    |               |
|    Master     |-------+-------------------------------|    Slave      |
|   Interface   |       |               Data Signal     |  Interface 1  |
|               |-------|-------+-----------------------|               |
+---------------+       |       |                       +---------------+
                        |       |
                        |       |
                        |       |
                     +--+-------+--+
                     |             |
                     |   Slave     |
                     | Interface 2 |
                     |             |
                     +-------------+
```

## coreboot

The coreboot implementation of SoundWire integrates with the device model and takes
advantage of the hierarchical nature of `devicetree.cb` to populate the topology.

The architecture-independent SoundWire tables are defined at

    src/include/device/soundwire.h

Support for new devices comes in three forms:

1. New controller and master drivers.  The first implementation in coreboot is for an Intel
SoC but the SoundWire specification is in wide use on various ARM SoCs.

    Controller drivers can be implemented in `src/soc` or `src/drivers` and should
    strive to re-use code as much as possible between different SoC generations from the
    same vendor.

2. New codec drivers.  These should be implemented for each codec that is added which
supports SoundWire.  The properties vary between codecs and careful study of the data sheet
is necessary to ensure proper operation.

    Codec drivers should be implemented in `src/drivers/soundwire` as separate chip drivers.
    As every codec is different there may not be opportunities of code re-use except between
    similar codecs from the same vendor.

3. New mainboards with SoundWire support.  The mainboard will combine controllers and codecs
to form a topology that is described in `devicetree.cb`.  Some devices may need to provide
board-specific configuration information, and multi-lane devices will need to provide the
master/slave lane map.

## ACPI Implementation

The implementation for x86 devices relies on ACPI for providing device properties to the OS
kernel drivers.

The ACPI implementation can be found at

    src/acpi/soundwire.c

And used by including

    #include <acpi/acpi_soundwire.h>

### Controller

The controller driver should populate a `struct soundwire_controller`:

```c
/**
 * struct soundwire_controller - SoundWire controller properties.
 * @master_count: Number of masters present on this device.
 * @master_list: One entry for each master device.
 */
struct soundwire_controller {
	unsigned int master_list_count;
	struct soundwire_link master_list[SOUNDWIRE_MAX_DEV];
};
```

Once the detail of the master links are specified in the `master_list` variable, the controller
properties for the ACPI object can be generated:

```c
struct acpi_dp *dsd = acpi_dp_new_table("_DSD");
soundwire_gen_controller(dsd, &soc_controller, NULL);
acpi_dp_write(dsd);
```

If the controller needs to generate custom properties for links it can provide a callback
function to `soundwire_gen_controller()` instead of passing NULL:

```c
static void controller_link_prop_cb(struct acpi_dp *dsd, unsigned int id,
                                    struct soundwire_controller *controller)
{
	acpi_dp_add_integer(dsd, "custom-link-property", 1);
}
```

### Codec

The codec driver should populate a *struct soundwire_codec* with necessary properties:

```c
/**
 * struct soundwire_codec - Contains all configuration for a SoundWire codec slave device.
 * @slave: Properties for slave device.
 * @audio_mode: Properties for Audio Mode for Data Ports 1-14.
 * @dpn: Properties for Data Ports 1-14.
 * @multilane: Properties for slave multilane device. (optional)
 * @dp0_bra_mode: Properties for Bulk Register Access mode for Data Port 0. (optional)
 * @dp0: Properties for Data Port 0 for Bulk Register Access. (optional)
 */
struct soundwire_codec {
	struct soundwire_slave *slave;
	struct soundwire_audio_mode *audio_mode[SOUNDWIRE_MAX_DEV];
	struct soundwire_dpn_entry dpn[SOUNDWIRE_MAX_DPN - SOUNDWIRE_MIN_DPN];
	struct soundwire_multilane *multilane;
	struct soundwire_bra_mode *dp0_bra_mode[SOUNDWIRE_MAX_DEV];
	struct soundwire_dp0 *dp0;
};
```

Many of these properties are optional, and depending on the codec will not be supported.

#### Slave Device Properties

These properties provide information about the codec device and what features it supports:

* Wake capability
* Clock stop behavior
* Clock and channel state machine behavior
* Features like register pages, broadcast read, bank delay, and high performance PHY

#### Multi-lane Slave Device Properties

Most slave devices have a single data pin and a single lane, but it is possible for up to
7 other lanes to be supported on a device.  These lanes can be connected to other master
links or to other slave devices.

If a codec supports this feature it must indicate that by providing an entry for
`struct soundwire_multilane` in the chip configuration.

```c
/**
 * struct drivers_soundwire_example_config - Example codec configuration.
 * @multilane: Multi-lane slave configuration.
 */
struct drivers_soundwire_example_config {
	struct soundwire_multilane multilane;
};
```

The mainboard is required to provide the lane map in `devicetree.cb` for any codec that has
multiple lanes connected.  This includes the definition up to 7 entries that indicate which
lane number on the slave devices (array index starting at 1) maps to which other device:

```
chip drivers/soundwire/multilane_codec
	register "multilane.lane_mapping" = "{
		{
			# Slave Lane 1 maps to Master Lane 2
			.lane = 1,
			.direction = MASTER_LANE,
			.connection.master_lane = 2
		},
		{
			# Slave Lane 3 maps to Slave Link B
			.lane = 3,
			.direction = SLAVE_LINK,
			.connection.slave_link = 1
		}
	}"
	device generic 0.0 on end
end
```

#### Data Port 0 Properties

SoundWire Data Port 0 (DP0) is a special port used for control and status operation relating
to the whole device interface, and as a special data port for bulk read/write operations.

The properties for data port 0 are different from that of data ports 1-14 and are about the
control channel behavior and the overall bulk register mode.

Data port 0 is not required to be supported by the slave device.

#### Bulk Register Access Mode Properties

Bulk Register Access (BRA) is an optional mechanism for transporting higher bandwidth of
register operations than the typical command mechanism.  The BRA protocol is a particular
format of the data on the (optional) data port 0 connection between the master and slave.

The BRA protocol may have alignment or timing requirements that are directly related to the
bus frequencies.  As a result there may be several configurations listed, for symmetry with
the audio modes paired with data ports 1-14.

#### Data Port 1-14 Properties

Data ports 1-14 are typically dedicated to streaming audio payloads, and each data port can
have from 1 to 8 channels.  There are different levels of data ports, with some registers
being required and supported on all data ports and some optional registers only being used
on some data ports.

Data ports can have both a sink and a source component, and the codec may support one or
both of these on each port.

Similar to data port 0 the properties defined here describe the capabilities and supported
features of each data port, and they may be configured separately.  For example the Maxim
MAX98373 codec supports a 32bit source data port for speaker output, and a 16bit sink data
port for speaker sense data.

#### Audio Mode Properties

Each data port may be tied to one or more audio modes.  The audio mode describes the actual
audio capabilities of the codec, including supported frequencies and sample rates.  These
modes can be shared by multiple data ports and do not need to be duplicated.

For example:

```
static struct soundwire_audio_mode audio_mode = {
	.bus_frequency_max = 24 * MHz,
	.bus_frequency_min = 24 * KHz,
	.max_sampling_frequency = 192 * KHz,
	.min_sampling_frequency = 8 * KHz,
};
static struct soundwire_dpn codec_dp1 = {
    [...]
	.port_audio_mode_count = 1,
	.port_audio_mode_list = {0}
};
static struct soundwire_dpn codec_dp3 = {
    [...]
	.port_audio_mode_count = 1,
	.port_audio_mode_list = {0}
};
```

### Generating Codec Properties

Once the properties are known it can generate the ACPI code with:

```c
struct acpi_dp *dsd = acpi_dp_new_table("_DSD");
soundwire_gen_codec(dsd, &soundwire_codec, NULL);
acpi_dp_write(dsd);
```

If the codec needs to generate custom properties for links it can provide a callback
function to `soundwire_gen_codec()` instead of passing NULL:

```c
static void codec_dp_prop_cb(struct acpi_dp *dsd, unsigned int id,
                             struct soundwire_codec *codec)
{
	acpi_dp_add_integer(dsd, "custom-dp-property", 1);
}
```

#### Codec Address

SoundWire slave devices use a SoundWire defined ACPI _ADR that requires a 64-bit integer
and uses the master link ID and slave device unique ID to form a unique address for the
device on this controller.

SoundWire addresses must be distinguishable from all other slave devices on the same master
link, so multiple instances of the same manufacturer and part on the same master link will
need different unique IDs.  The value is typically determined by strapping pins on the codec
chip and can be decoded for this table with the codec datasheet and board schematics.

```c
/**
 * struct soundwire_address - SoundWire ACPI Device Address Encoding.
 * @version: SoundWire specification version from &enum soundwire_version.
 * @link_id: Zero-based SoundWire Link Number.
 * @unique_id: Unique ID for multiple devices.
 * @manufacturer_id: Manufacturer ID from include/device/mipi_ids.h.
 * @part_id: Vendor defined part ID.
 * @class: MIPI class encoding in &enum mipi_class.
 */
struct soundwire_address {
	enum soundwire_version version;
	uint8_t link_id;
	uint8_t unique_id;
	uint16_t manufacturer_id;
	uint16_t part_id;
	enum mipi_class class;
};
```

This ACPI address can be generated by calling the provided acpigen function:

    acpigen_write_ADR_soundwire_device(const struct soundwire_address *sdw);

### Mainboard

The mainboard needs to select appropriate drivers in `Kconfig` and define the topology in
`devicetree.cb` with the controllers and codecs that exist on the board.

The topology uses the **generic** device to describe SoundWire:

```c
struct generic_path {
	unsigned int id;       /* SoundWire Master Link ID */
	unsigned int subid;    /* SoundWire Slave Unique ID */
};
```

This allows devices to be specified in `devicetree.cb` with the necessary information to
generate ACPI address and device properties.

```
chip drivers/intel/soundwire
	# SoundWire Controller 0
	device generic 0 on
		chip drivers/soundwire/codec1
			# SoundWire Link 0 ID 0
			device generic 0.0 on end
		end
		chip drivers/soundwire/codec2
			# SoundWire Link 1 ID 2
			device generic 1.2 on end
		end
	end
end
```

## Volteer Example

This is an example of an Intel Tiger Lake reference board using SoundWire Link 0 for the
headphone codec connection, and Link 1 for connecting two speaker amps for stereo speakers.

The mainboard can be found at

    src/mainboard/google/volteer

```
  +------------------+         +-------------------+
  |                  |         | Headphone Codec   |
  | Intel Tiger Lake |    +--->| Realtek ALC5682   |
  |    SoundWire     |    |    |       ID 1        |
  |    Controller    |    |    +-------------------+
  |                  |    |
  |           Link 0 +----+    +-------------------+
  |                  |         | Left Speaker Amp  |
  |           Link 1 +----+--->| Maxim MAX98373    |
  |                  |    |    |       ID 3        |
  |           Link 2 |    |    +-------------------+
  |                  |    |
  |           Link 3 |    |    +-------------------+
  |                  |    |    | Right Speaker Amp |
  +------------------+    +--->| Maxim MAX98373    |
                               |       ID 7        |
                               +-------------------+
```

This implementation requires a controller driver for the Intel Tigerlake SoC and a codec
driver for the Realtek and Maxim chips.  If those drivers did not already exist they would
need to be added and reviewed separately before adding the support to the mainboard.

The volteer example requires some `Kconfig` options to be selected:

```
config BOARD_GOOGLE_BASEBOARD_VOLTEER
	select DRIVERS_INTEL_SOUNDWIRE
	select DRIVERS_SOUNDWIRE_ALC5682
	select DRIVERS_SOUNDWIRE_MAX98373
```

And the following `devicetree.cb` entries to define this topology:

```
device pci 1f.3 on
	chip drivers/intel/soundwire
		# SoundWire Controller 0
		device generic 0 on
			chip drivers/soundwire/alc5682
				# SoundWire Link 0 ID 1
				register "desc" = ""Headphone Jack""
				device generic 0.1 on end
			end
			chip drivers/soundwire/max98373
				# SoundWire Link 0 ID 1
				register "desc" = ""Left Speaker Amp""
				device generic 1.3 on end
			end
			chip drivers/soundwire/max98373
				# SoundWire Link 1 ID 7
				register "desc" = ""Right Speaker Amp""
				device generic 1.7 on end
			end
		end
	end
end
```
