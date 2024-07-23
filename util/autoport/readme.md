# Porting coreboot using autoport

## Supported platforms

### Chipset
For any Sandy Bridge, Ivy Bridge, or Haswell platform the generated result
should be bootable, possibly with minor fixes.

### EC / SuperIO
EC support is likely to work on Intel-based thinkpads. Other laptops are
likely to miss EC support. SuperIO support on desktops is more likely to
work out of the box than any EC.

## How to use autoport

Enable as many devices as possible in the firmware setup of your system.
This is useful to detect as many devices as possible and make the port
more complete, as disabled devices cannot be detected.

Boot into target machine under any Linux-based distribution and install
the following tools on it:
* `gcc`
* `golang`
* `lspci`
* `dmidecode`
* `acpidump` (part of `acpica` on some distros)

Clone the coreboot tree and `cd` into it. For more detailed steps, refer
to Rookie Guide, Lesson 1. Afterwards, run these commands:

		cd util/ectool
		make
		cd ../inteltool
		make
		cd ../superiotool
		make
		cd ../autoport
		go build
		sudo ./autoport --input_log=logs --make_logs --coreboot_dir=../..

	Note: in case you have problems getting gcc and golang on the target
	machine, you can compile the utilities on another computer and copy
	the binaries to the target machine. You will still need the other
	listed programs on the target machine, but you may place them in the
	same directory as autoport.

Check for unknown detected PCI devices, e.g.:

		Unknown PCI device 8086:0085, assuming removable

If autoport says `assuming removable`, you are fine. If it doesn't,
you may want to add the relevant PCI IDs to autoport. Run `lspci -nn`
and check which device this is using the PCI ID. Devices which are not
part of the chipset, such as GPUs or network cards, can be considered
removable, whereas devices inside the CPU or the PCH such as integrated
GPUs and bus controllers (SATA, USB, LPC, SMBus...) are non-removable.

Your board has now been added to the tree. However, do not flash it
in its current state. It can brick your machine. Instead, keep this
new port and the logs from `util/autoport/logs` somewhere safe. The
following steps will back up your current firmware, which is always
recommended, since coreboot may not boot on the first try.

Disassemble your computer and find the flash chip(s). Since there could be
more than one, this guide will refer to "flash chips" as one or more chips.
Refer to <https://flashrom.org/Technology> as a reference. The flash chip is
usually in a `SOIC-8` (2x4 pins, 200mil) or `SOIC-16` (2x8 pins) package. As
it can be seen on flashrom's wiki, the former package is like any other 8-pin
chip on the mainboard, but it is slightly larger. The latter package is much
easier to locate. Always make sure it is a flash chip by looking up what its
model, printed on it, refers to.

There may be a smaller flash chip for the EC on some laptops, and other chips
such as network cards may use similar flash chips. These should be left as-is.
If in doubt, ask!

Once located, use an external flasher to read the flash chips with `flashrom -r`.
Verify with `flashrom -v` several times that reading is consistent. If it is not,
troubleshoot your flashing setup. Save the results somewhere safe, preferably on
media that cannot be easily overwritten and on several devices. You may need this
later. The write process erases the flash chips first, and erased data on a flash
chip is lost for a very long time, usually forever!

Compile coreboot for your ported mainboard with some console enabled. The most
common ones are EHCI debug, serial port and SPI flash console as a last resort.
If your system is a laptop and has a dedicated video card, you may need to add
a video BIOS (VBIOS) to coreboot to be able to see any video output. Desktop
video cards, as well as some MXM video cards, have this VBIOS on a flash chip
on the card's PCB, so this step is not necessary for them.

Flash coreboot on the machine. On recent Intel chipsets, the flash space is split
in several regions. Only the one known as "BIOS region" should be flashed. If
there is only one flash chip present, this is best done by adding the `--ifd`
and `-i bios` parameters flashrom has (from v1.0 onwards) to specify what flash
descriptor region it should operate on. If the ME (Management Engine) region is
not readable, which is the case on most systems, use the `--noverify-all`
parameter as well.

For systems with two flash chips, this is not so easy. It is probably better to
ask in coreboot or flashrom communication channels, such as via IRC or on the
mailing lists.

Once flashed, try to boot. Anything is possible. If a log is generated, save it
and use it to address any issues. See the next section for useful information.
Find all the sections marked with `FIXME` and correct them.

Send your work to review.coreboot.org. I mean it, your effort is very appreciated.
Refer to Rookie Guide, Lesson 2 for instructions on how to submit a patch.

## Manual fixes
### SPD
In order to initialize the RAM (memory), coreboot needs to know its timings, which vary between
modules. Socketed RAM has a small EEPROM chip, which is accessible via SMBus and contains the
timing data. This data is usually known as SPD. Unfortunately, the SMBus addresses may not
correlate with the RAM slots and cannot always be detected automatically. The address map is
usually in the devicetree, `register "spd_addresses"`. For mainboards with memory-down (where
the RAM chips are soldered directly to the mainboard), there is no EEPROM to get SPD data from,
so function `mb_get_spd_map` in `early_init.c` has to populate the SPD data from a file in CBFS.

By default, autoport uses the most common map `0x50, 0x51, 0x52, 0x53` on everything except for
Lenovo systems, which are known to use `0x50, 0x52, 0x51, 0x53`. To detect the correct memory
map, the easiest way is to boot on the vendor firmware with just one module in channel 0, slot
0, and check the SMBus address the EEPROM has. Under Linux, you can use these commands to see
which devices appear on SMBus:

	$ sudo modprobe i2c-dev
	$ sudo modprobe i2c-i801
	$ sudo i2cdetect -l
	i2c-0	i2c		i915 gmbus ssc				I2C adapter
	i2c-1	i2c		i915 gmbus vga				I2C adapter
	i2c-2	i2c		i915 gmbus panel			I2C adapter
	i2c-3	i2c		i915 gmbus dpc				I2C adapter
	i2c-4	i2c		i915 gmbus dpb				I2C adapter
	i2c-5	i2c		i915 gmbus dpd				I2C adapter
	i2c-6	i2c		DPDDC-B					I2C adapter
	i2c-7	i2c		DPDDC-C					I2C adapter
	i2c-8	i2c		DPDDC-D					I2C adapter
	i2c-9	smbus		SMBus I801 adapter at 0400		SMBus adapter

	$ sudo i2cdetect 9
	WARNING! This program can confuse your I2C bus, cause data loss and worse!
	I will probe file /dev/i2c-9.
	I will probe address range 0x03-0x77.
	Continue? [Y/n] y
	     0  1  2  3  4  5  6  7  8  9  a  b  c  d  e  f
	00:          -- -- -- -- -- 08 -- -- -- -- -- -- --
	10: -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
	20: -- -- -- -- 24 -- -- -- -- -- -- -- -- -- -- --
	30: 30 31 -- -- -- -- -- -- -- -- -- -- -- -- -- --
	40: -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
	50: 50 -- -- -- 54 55 56 57 -- -- -- -- 5c 5d 5e 5f
	60: -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
	70: -- -- -- -- -- -- -- --

Note: if some devices appear as `UU`, it means a kernel module is loaded for this device, like
`at24` or `ee1004`. You can use the `decode-dimms` command to get more information about SPDs.

Make sure to replace the `9` on the last command with the bus number for SMBus on
your system. Here, there is a module at address `0x50`. Since only one module was
installed on the first slot of the first channel, we know the first position of
the SPD array must be `0x50`. After testing all the slots, your `spd_addresses`
should look similar to this:

	register "spd_addresses" = "{0x50,    0, 0x52,    0}" # 2-slot mainboard / laptop
	register "spd_addresses" = "{0x53, 0x52, 0x51, 0x50}" # 4-slot BTX mainboard

Note: slot labelling may be missing or unreliable. Use `inteltool` to see which slots have
modules in them.

This procedure is ideal, if your RAM is socketed. If you have soldered RAM,
remove any socketed memory modules and check if any EEPROM appears on SMBus.
If this is the case, you can proceed as if the RAM was socketed. However,
you may have to guess some entries if there multiple EEPROMs appear.

Most of the time, soldered RAM does not have an EEPROM. Instead, the SPD data is
inside the main flash chip where the firmware is. If this is the case, you need
to generate the SPD data to use with coreboot. Look at `inteltool.log`. There
should be something like this:

	/* SPD matching current mode:  */
	/* CH0S0  */
	00: 92 11 0b 03 04 00 00 09 03 52 01 08 0a 00 80 00
	10: 6e 78 6e 32 6e 11 18 81 20 08 3c 3c 00 f0 00 00
	20: 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
	30: 00 00 00 00 00 00 00 00 00 00 00 00 00 00 65 00
	40: 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
	50: 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
	60: 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
	70: 00 00 00 00 00 00 00 00 00 00 00 00 00 00 6d 17
	80: 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
	90: 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
	a0: 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
	b0: 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
	c0: 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
	d0: 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
	e0: 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
	f0: 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
	/* CH1S0  */
	00: 92 11 0b 03 04 00 00 09 03 52 01 08 0a 00 80 00
	10: 6e 78 6e 32 6e 11 18 81 20 08 3c 3c 00 f0 00 00
	20: 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
	30: 00 00 00 00 00 00 00 00 00 00 00 00 00 00 65 00
	40: 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
	50: 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
	60: 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
	70: 00 00 00 00 00 00 00 00 00 00 00 00 00 00 6d 17
	80: 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
	90: 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
	a0: 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
	b0: 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
	c0: 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
	d0: 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
	e0: 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
	f0: 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00

This is not a full-fledged SPD dump, as it only lists the currently-used speed configuration,
and lacks info such as a serial number, vendor and model. To create a SPD hex file, one has to
trim the offset numbers from the leftmost column:

	$ cat | cut -d ' ' -f 2- > data.spd.hex
	00: 92 11 0b 03 04 00 00 09 03 52 01 08 0a 00 80 00
	10: 6e 78 6e 32 6e 11 18 81 20 08 3c 3c 00 f0 00 00
	20: 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
	30: 00 00 00 00 00 00 00 00 00 00 00 00 00 00 65 00
	40: 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
	50: 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
	60: 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
	70: 00 00 00 00 00 00 00 00 00 00 00 00 00 00 6d 17
	80: 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
	90: 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
	a0: 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
	b0: 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
	c0: 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
	d0: 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
	e0: 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
	f0: 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
	EOF (press Ctrl + D)

Then, move the generated file into your mainboard's directory and hook it up to the build
system. It is recommended to check what other mainboards with soldered memory do. The main
switch to use SPD files is `select HAVE_SPD_IN_CBFS` in Kconfig.

Now we need coreboot to use this SPD file. The following example shows a hybrid configuration,
in which one module is soldered down and the other one is socketed:

	void mb_get_spd_map(struct spd_info *spdi)
	{
		spdi->spd_index = 0;
		/* C0S0 is soldered RAM, use stored SPD */
		spdi->addresses[0] = SPD_MEMORY_DOWN;
		/* C1S0 is a physical slot, use SPD address on SMBus */
		spdi->addresses[2] = 0x52;
	}

If several slots are soldered, the system only accounts for a single set of SPD data. So all
slots would need to use the same SPD data, if possible. If not possible, the API needs to be
adapted accordingly, which is significantly more involved.

If memory initialization is not working, in particular write training (timB)
on DIMM's second rank fails, try enabling rank 1 mirroring, which can't be
detected by inteltool. It is described by SPD field "Address Mapping from Edge
Connector to DRAM", byte `63` (`0x3f`). Bit 0 describes Rank 1 Mapping,
0 = standard, 1 = mirrored; set it to 1. Bits 1-7 are reserved.

### `board_info.txt`

`board_info.txt` is a text file used in the board status page to list all
the supported boards and their specifications. Most of the information
cannot be detected by autoport. Common entries are:

* `ROM package`, `ROM protocol` and `ROM socketed`:
  These refer to the flash chips you found earlier. You can visit
  <https://flashrom.org/Technology> for more information.

* `Release year`: Use the power of Internet to find that information.
* `Category`: This describes the type of mainboard you have.
  Valid categories are:
  * `desktop`. Desktops and workstations.
  * `server`. Servers.
  * `laptop`. Laptops, notebooks and netbooks.
  * `half`. Embedded / PC/104 / Half-size boards.
  * `mini`. Mini-ITX / Micro-ITX / Nano-ITX
  * `settop`. Set-top-boxes / Thin clients.
  * `eval`. Development / Evaluation Boards.
  * `sbc`. Single-Board computer.
  * `emulation`: Virtual machines and emulators. May require especial care
                 as they often behave differently from real counterparts.
  * `misc`. Anything not fitting the categories above. Not recommended.

* `Flashrom support`: This means whether the internal programmer is usable.
  If flashing coreboot internally works, this should be set to `y`. Else,
  feel free to investigate why it is not working.

### `USBDEBUG_HCD_INDEX`

Which controller the most easily accessible USB debug port is. On Intel,
1 is for `00:1d.0` and 2 is for `00:1a.0` (yes, it's reversed). Refer to
<https://www.coreboot.org/EHCI_Debug_Port> for more info.

If you are able to use EHCI debug without setting the HCD index manually,
this is correct.

### `BOARD_ROMSIZE_KB_2048`

This parameter refers to the total size of the flash chips coreboot will be in.
This value must be correct for S3 resume to work properly. This parameter also
defines the size of the generated coreboot image, but that is not a major issue
since tools like `dd` can be used to cut fragments of a coreboot image to flash
on smaller chips.

This should be detected automatically, but it may not be detected properly in
some cases. If it was not detected, put the correct total size here to serve
as a sane default when configuring coreboot.

### `DRAM_RESET_GATE_GPIO`

When the computer is suspended to RAM (ACPI S3), the RAM reset signal must not
reach the RAM modules. Otherwise, the computer will not resume and any opened
programs will be lost. This is done by powering down a MOSFET, which disconnects
the reset signal from the RAM modules. Most manufacturers put this gate on GPIO
60 but Lenovo is known to put it on GPIO 10. If suspending and resuming works,
this value is correct. This can also be determined from the board's schematics.

## GNVS

`mainboard_fill_gnvs` sets values in GNVS, which then ACPI makes use of for
various power-related functions. Normally, there is no need to modify it
on laptops (desktops have no "lid"!) but it makes sense to proofread it.

## `gfx.ndid` and `gfx.did`

Those describe which video outputs are declared in ACPI tables.
Normally, there is no need to have these values, but if you miss some
non-standard video output, you can declare it there. Bit 31 is set to
indicate the presence of the output. Byte 1 is the type and byte 0 is
used for disambigution so that ID composed of byte 1 and 0 is unique.

Types are:
* 1 = VGA
* 2 = TV
* 3 = DVI
* 4 = LCD

## `c*_acpower` and `c*_battery`

Which mwait states to match to which ACPI levels. Normally, there is no
need to modify anything unless your device has very special power saving
requirements.

## `install_intel_vga_int15_handler`

This is used with the Intel VGA BIOS, which is not the default option.
It is more error-prone than open-source graphics initialization, so do
not bother with this until your mainboard boots. This is a function
which takes four parameters:
1.  Which type of LCD panel is connected.
2.  Panel fit.
3.  Boot display.
4.  Display type.

Refer to `src/drivers/intel/gma/int15.h` to see which values can be used.
For desktops, there is no LCD panel directly connected to the Intel GPU,
so the first parameter should be `GMA_INT15_ACTIVE_LFP_NONE`. On other
mainboards, it depends.

## CMOS options

Due to the poor state of CMOS support in coreboot, autoport does not
support it and this probably won't change until the format in the tree
improves. If you really care about CMOS options:

* Create files `cmos.layout` and `cmos.default`
* Enable `HAVE_OPTION_TABLE` and `HAVE_CMOS_DEFAULT` in `Kconfig`

## EC (lenovo)

You need to set `has_keyboard_backlight` (backlit keyboard like X230),
`has_power_management_beeps` (optional beeps when e.g. plugging the cord
in) and `has_uwb` (third MiniPCIe slot) in accordance to functions available
on your machine

In rare cases autoport is unable to detect GPE. You can detect it from
dmesg or ACPI tables. Look for line in dmesg like

	ACPI: EC: GPE = 0x11, I/O: command/status = 0x66, data = 0x62

This means that GPE is `0x11` in ACPI notation. This is the correct
value for `THINKPAD_EC_GPE`. To get the correct value for `GPE_EC_SCI`
you need to substract `0x10`, so value for it is `1`.

The pin used to wake the machine from EC is guessed. If your machine doesn't
wake on lid open and pressing of Fn, change `GPE_EC_WAKE`.

Keep `GPE_EC_WAKE` and `GPE_EC_SCI` in sync with `gpi*_routing`.
`gpi*_routing` matching `GPE_EC_WAKE` or `GPE_EC_SCI` is set to `2`
and all others are absent.

If your dock has LPC wires or needs some special treatement you may
need to add codes to initialize the dock and support code to
DSDT. See the `init_dock()` for `x60`, `x200` or `x201`.

## EC (generic laptop)

Almost any laptop has an embedded controller. In a nutshell, it's a
small, low-powered computer designed to be used on laptops. Exact
functionality differs between machines. Its main functions include:

* Control of power and rfkill to different component
* Keyboard (PS/2) interface implementation
* Battery, AC, LID and thermal information exporting
* Hotkey support

autoport automatically attempts to restore the dumped config but it
may or may not work and may even lead to a hang or powerdown. If your
machine stops at `Replaying EC dump ...` try commenting EC replay out

autoport tries to detect if machine has PS/2 interface and if so calls
`pc_keyboard_init` and exports relevant ACPI objects. If detection fails
you may have to add them yourself

ACPI methods `_PTS` (prepare to sleep) and `_WAK` (wake) are executed
when transitioning to sleep or wake state respectively. You may need to
add power-related calls there to either shutdown some components or to
add a workaround to stop giving OS thermal info until next refresh.

For exporting the battery/AC/LID/hotkey/thermal info you need to write
`acpi/ec.asl`. For an easy example look into `apple/macbook21` or
`packardbell/ms2290`. For information about needed methods consult
relevant ACPI specs. Tracing which EC events can be done using
[dynamic debug](https://wiki.ubuntu.com/Kernel/Reference/ACPITricksAndTips)

EC GPE needs to be routed to SCI in order for OS in order to receive
EC events like "hotkey X pressed" or "AC plugged". autoport attempts
to detect GPE but in rare cases may fail. You can detect it from
dmesg or ACPI tables. Look for line in dmesg like

	ACPI: EC: GPE = 0x11, I/O: command/status = 0x66, data = 0x62

This means that GPE is `0x11` in ACPI notation. This is the correct
value for `_GPE`.

Keep GPE in sync with `gpi*_routing`.
`gpi*_routing` matching `GPE - 0x10` is set to `2`
and all others are absent. If EC has separate wake pin
then this GPE needs to be routed as well
