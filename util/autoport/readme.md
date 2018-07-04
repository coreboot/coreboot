# Porting coreboot using autoport

## Supported platforms

### Chipset
For any Sandy Bridge or Ivy Bridge platform the generated result should
be bootable, possibly with minor fixes.

### EC
EC support is likely to work on Intel-based thinkpads. Other laptops are
likely to miss EC support.

## How to use

* Go into BIOS setup on the target machine and enable all devices.
This will allow autoport to detect as much as possible.
* Boot into target machine under GNU/Linux
* Make sure that the following components are installed:
  * GCC
  * golang
  * lspci
  * dmidecode
  * acpidump
* Grab coreboot tree
* Execute following commands starting from coreboot tree

		cd util/ectool
		make
		cd ../inteltool
		make
		cd ../superiotool
		make
		cd ../autoport
		go build
		sudo ./autoport --input_log=logs --make_logs --coreboot_dir=../..

	Note: in case you have problems getting gcc and golang to target machine
	you can just compile on another machine and transfer the binaries
	`autoport`, `inteltool` and `ectool`. You'll still need other prerequisites
	but you may place them in the same directory as autoport.

* Look for output unknown PCI devices. E.g.

		Unknown PCI device 8086:0085, assuming removable

If autoport says `assuming removable` then you're fine. If it doesn't
then you may want to add relevant PCIIDs to autoport. When rerunning
you can skip argument `--make_logs` to reuse the same logs

* At this point the new board is added to the tree but don't flash it
yet as it will brick your machine. Instead keep this new port and the logs
from `util/autoport/logs` somewhere safe.

* Disassemble your laptop and locate flash chip <http://flashrom.org/Technology>
is a great resource. The flash chip is usually in `SOIC-8` (2x4 pins) or `SOIC-16`
(2x8 chips). You'll probably have several candidates. Look up what's written on
them and look up what's this chip on the web.

* Once you know what's the chip is, get an external flasher and read it. Twice. Compare
the results and retry if they differ. Save the result somewhere safe, in preference
copy it to read-only storage as backup.

* Compile coreboot with console enabled (EHCI debug or serial if present are recommended)

* For recent Intel chipsets you need to avoid overwriting ME firmware. Recommended procedure is
(replace 8 with your flash size in MiB):

		cp backup.rom flash.rom
		dd if=coreboot/build/coreboot.rom skip=$[8-1] seek=$[8-1] bs=1M of=flash.rom

* Flash the result
* Boot and grab the log and fix the issues. See next section for useful info.
* grep your board for FIXME. autoport adds comments when it's unsure. Sometimes it's just
a minor check and sometimes it needs more involvment. See next section.
* Send new board to review.coreboot.org. I mean it, your effort is very appreciated.

## Manual fixes
### SPD
If you're able to use full memory with any combination of inserted modules than this is
most likely correct. In order to initialize the memory coreboot needs to know RAM timings.
For socketed RAM it's stored in a small EEPROM chip which can be accessed through SPD. Unfortunately
mapping between SPD addresses and RAM slots differs and cannot always be detected automatically.
Resulting SPD map is encoded in function `mainboard_get_spd` in `early_southbridge.c`.
autoport uses the most common map `0x50, 0x51, 0x52, 0x53` except for lenovos which are
known to use `0x50, 0x52, 0x51, 0x53`. To detect the correct memory map the easiest way is with
vendor BIOS to boot with just one module in channel 0 slot 0 and then see where does it show
up in SPD. Under Linux you can see present SPD addresses with following commands:

	phcoder@sid:~/coreboot/util/autoport$ sudo modprobe i2c-dev
	phcoder@sid:~/coreboot/util/autoport$ sudo i2cdetect -l
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
	phcoder@sid:~/coreboot/util/autoport$ sudo i2cdetect 9
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

Make sure to replace `9` with whatever bus is marked as SMBus. Here in an example
you see SPD at address `0x50`. Since we've booted with just the module in C0S0, so
the first entry in SPD map has to be `0x50`. Once you have SPD map your
`mainboard_get_spd` should look something like:

	void mainboard_get_spd(spd_raw_data *spd) {
		read_spd (&spd[0], 0x50);
		read_spd (&spd[1], 0x51);
		read_spd (&spd[2], 0x52);
		read_spd (&spd[3], 0x53);
	}

You can and should omit lines which correspond to
slots not present on your machine.

Note: slot labelling may be missing or unreliable. Use `inteltool` to see
which slots have modules in them.

This way works well if your RAM is socketed. For soldered RAM if you see
its SPD, you're in luck and can proceed the same way although you may have to
guess some entries due to RAM not being removable.

Most cases of soldered RAM don't have EEPROM chip. In this case you'd have to create
fake SPD. Look in `inteltool.log`. You'll see something like:

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

This is not completely exact represantation of RAM
capablities as it lists only the mode currently used
and lacks minor info like serial number. Using `xxd`
you can create binary represantation of this SPD:

	cat | xxd -r > spd.bin  <<EOF
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
	EOF

Then you can place this file into mainboard directory
and hook it up into build system by adding following
lines to `Makefile.inc` (creating a new file if needed)

	cbfs-files-y += spd.bin
	spd.bin-file := spd.bin
	spd.bin-type := raw

And then make coreboot actually use this SPD. Following
example shows a hybrid situation with one module
soldered and another is socketed:

	void mainboard_get_spd(spd_raw_data *spd)
	{
		void *spd_file;
		size_t spd_file_len = 0;
		/* C0S0 is a soldered RAM with no real SPD. Use stored SPD.  */
		spd_file = cbfs_boot_map_with_leak( "spd.bin", CBFS_TYPE_RAW,
						 &spd_file_len);
		if (spd_file && spd_file_len >= 128)
			memcpy(&spd[0], spd_file, 128);
		/* C0S0 is a DIMM slot.  */
		read_spd(&spd[1], 0x51);
	}

If several slots are soldered there are 3 ways of doing things:

* If all of them are the same use the same file. Don't forget to copy
it to all array elements.
* Use several files (recommended). Name them e.g. spd1, spd2,...
* Concatenate it into a single file and split into several
array elements on runtime. Not recommended

### `board_info.txt`

`board_info.txt` is a simple text file used to generate wiki page
listing supported boards. Some of the info can't be detected automatically.

As this is used only to present information to users then when it matches
your board and definitions it is correct.

* Which package is used for ROM and whether it's socketed, as well
as release year. For ROM package refer to <http://flashrom.org/Technology>
and compare it with ROM you found at the beginning of the port. Set
`ROM package`, `ROM socketed` and other variables mentioned in FIXME.
* Release year, just go to web and find that information.
* Category. It's difficult to make difference between desktop and similar
categories from inside the computer. Valid categories are:
	* `desktop`. Desktops and workstations.
	* `server`. Servers
	* `laptop`. Laptops.
	* `half`. Embedded / PC/104 / Half-size boards.
	* `mini`. Mini-ITX / Micro-ITX / Nano-ITX
	* `settop`. Set-top-boxes / Thin clients.
	* `eval`. Devel/Eval Boards
	* `sbc`. Single-Board computer.
	* `emulation`. Virtual machines and emulators. May require especial care
	as they often behave differently from real counterparts.
	* `misc`. Anything not fitting the categories above. You probably shouldn't use
	this.

### `USBDEBUG_HCD_INDEX`

Which controller the most easily accessible USB debug port is. On intel
1 is for `00:1d.0` and 2 is `00:1a.0` (yes, it's reversed). See
<https://www.coreboot.org/EHCI_Debug_Port> for more info.

If you're able to use EHCI debug port without setting HCD index manually
in config this is correct.

### `BOARD_ROMSIZE_KB_2048`

Default rom size should be detected automatically but sometimes isn't.
If yours weren't detected put correct rom size here to serve as sane
default when configuring coreboot.

If default ROM size when slecting the board is right one than this value
is correct.

### `DRAM_RESET_GATE_GPIO`

When machine is going to S3 in order not to reset the RAM modules, the
reset signal must be filtered out from reachin RAM. This is done by
powering down a special gate. Most manufacturers put this gate on
GPIO 60 but Lenovo is knowon to put it on GPIO 10. If you're able to
go to S3 and back than this value is correct.

## GNVS

`acpi_create_gnvs` sets values in GNVS which in turn is used by ACPI for
various power-related functions. Normally there is no need to modify it
but it makes sense to proofread it.

## `gfx.ndid` and `gfx.did`

Those describe which video outputs are declared in ACPI tables.
Normally there is no need to adjust but if you miss some nonstandard output
you can declare it there. Bit 31 is set to indicate presence of the output.
Byte 1 is the type and byte 0 is used for disambigution so that ID composed of
byte 1 and 0 is unique. Types are

* 1 = VGA
* 2 = TV
* 3 = DVI
* 4 = LCD

## `c*_acpower` and `c*_battery`

Which mwait states to match to which ACPI levels. Normally no need to modify unless
your device has very special power saving requirements.

## `install_intel_vga_int15_handler`

This is used to configure int15 hook used by vgabios. Parameters 2 and 3 usually
shouldn't be modified as vgabios is quite ok to figure panel fit and active
output by itself. Last number is the numerical ID of display type. If you
don't get any output with vgabios you should try different values for 4th
parameter. If it doesn't help try different values for first parameter as well

## CMOS options

Due to horrible state of CMOS support in coreboot tree, autoport doesn't support it and
this probably won't change until format in the tree improves. If you really care about
CMOS options:

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

If your dock has LPC wires or needs some special treatement you
need to fill `h8_mainboard_init_dock` and add support code to
DSDT. See the code for `x60`, `x200` or `x201`

## EC (generic laptop)

Almost any laptop has an embedded controller. In nutshell it's a small
low-powered computer specialised on managing power for laptop. Exact
functionality differs between macines but of interest to us is mainly:

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
