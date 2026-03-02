# Acer TravelMate P633 series
Available in 2 variants P633-M and P633-V laptops come in a total of
8 different CPU models, only the P633-M variant hosting a i3-2370M CPU
with the following case and motherboard labels has been tested.

```
+-------------------------------+
| TravelMate P633 series        |
+-------------------------------+
| MODEL NO. MS2362              |
+-------------------------------+
| TravelMate P633-M-32374G32ikk |
+-------------------------------+
| BAD30-HC MB                   |
+-------------------------------+
| 11313-1M                      |
+-------------------------------+
| 48.4VT01.01M                  |
+-------------------------------+
```

## Required proprietary blobs
- Intel Flash Descriptor (FD)
- Intel Management Engine (ME)
- Intel Gigabit Ethernet (GBE) *P633-V variant*
- CPU microcode

## Build config
From here on it is assumed that you already have a working coreboot
build environment setup with at least the first 3 steps finished which
are documented at [Tutorial, part 1: Starting from scratch].

```bash
# cleanup build environment and select Acer P633_M or P633_V board
make distclean
touch .config
./util/scripts/config --enable VENDOR_ACER
./util/scripts/config --enable BOARD_ACER_TRAVELMATE_P633_M # or P633_V

# enable this for CMOS setup by nvramtool
./util/scripts/config --enable USE_OPTION_TABLE

# expand CBFS_SIZE to contain EDK2 payload
./util/scripts/config --set-val CBFS_SIZE "0x300000"

# save current build config
make olddefconfig
```

## Flashing coreboot
On OEM firmware the FD is read-only and the ME is locked but BIOS region
is writable without restrictions. Although internal flashing is possible
it is good practice to at least backup the FD, BIOS and GBE region and
in situ externally backup each separate 4MiB and 8MiB ROM before flashing
since the later is the only option to have a backup containing the ME!

### Internal programming
Backup the readable OEM firmware regions and notice variant difference!

```bash
flashrom -p internal -r p633_m-fd-bios.rom --ifd -i fd -i bios
flashrom -p internal -r p633_v-fd-bios-gbe.rom --ifd -i fd -i bios -i gbe
```
The obtained FD (and GBE) region should be added to the coreboot build
process, first extract them in your coreboot build folder and take
notice of the variant differences.

```bash
# extract readable regions from ROM
ifdtool -x p633_$model.rom

# add FD to coreboot build
./util/scripts/config --enable HAVE_IFD_BIN
./util/scripts/config --set-str IFD_BIN_PATH "flashregion_0_flashdescriptor.bin"

# add GBE to coreboot build (P633_V variant only)
#./util/scripts/config --enable HAVE_GBE_BIN
#./util/scripts/config --set-str GBE_BIN_PATH "flashregion_3_gbe.bin"

make olddefconfig
make
```

Mind that your current `build/coreboot.rom` comes without a ME region
which should never be flashed as a whole image or it bricks the laptop!
Unless you made in situ backups before see next external programming or
if you want to risk a brick you may flash coreboot using this command:
`flashrom -p internal -w coreboot-nome.rom --ifd -i bios --noverify-all`
Don't reboot or reset after flashing, but remove ac-adapter and or
battery!

### External programming
With only removing the bottom "Base Door" and breaking away the plastic
divider between the HDD bay and the 2 Mini PCIe slots, 2 SPI chips are
exposed without having to take apart the whole laptop. Since the 2 SPI
chips are very tightly spaced together, a SOIC-8 SPI pomona probe
(not clip) with a external ch341a_spi (3.3V) programmer can be used to
in situ backup both ROM's. No ac-adapter or battery should be connected
only a USB powered programmer! Having a external programmer opens a lot
of recovery and easy internal flashing options see example commands:

```bash
# backup the 4MiB OEM ROM
flashrom -p ch341a_spi -r 4m.rom -c "MX25L3206E/MX25L3208E"
# backup the 8MiB OEM ROM
flashrom -p ch341a_spi -r 8m.rom -c "MX25L6406E/MX25L6408E"
# repeat backups 2 times with different filenames and compare hashes!

# combine both OEM ROM's
cat 4m.rom 8m.rom > 12m.rom
# unlock FD and ME region on OEM ROM for easy internal flashing/recovery
ifdtool -u 12m.rom
mv 12m.rom.new oem_fd_me_unlocked.rom

# split FD & ME unlocked combined OEM ROM for external flashing/recovery
dd if=oem_fd_me_unlocked.rom of=u4m.rom bs=1M count=4
dd if=oem_fd_me_unlocked.rom of=u8m.rom bs=1M skip=4
# flash unlocked OEM 4MiB ROM
flashrom -p ch341a_spi -w u4m.rom -c "MX25L3206E/MX25L3208E"
# flash unlocked OEM 8MiB ROM
flashrom -p ch341a_spi -w u8m.rom -c "MX25L6406E/MX25L6408E"

# build a complete coreboot ROM including FD / (GBE) / ME
# extract all regions from combined OEM ROM
ifdtool -x 12m.rom
# add ME to coreboot build
./util/scripts/config --enable HAVE_ME_BIN
./util/scripts/config --set-str ME_BIN_PATH "flashregion_2_intel_me.bin"
make olddefconfig
make

# split coreboot.rom first before externally flashing!
dd if=build/coreboot.rom of=cb4m.rom bs=1M count=4
dd if=build/coreboot.rom of=cb8m.rom bs=1M skip=4
# flash unlocked OEM 4MiB ROM
flashrom -p ch341a_spi -w cb4m.rom -c "MX25L3206E/MX25L3208E"
# backup the 8MiB ROM
flashrom -p ch341a_spi -w cb8m.rom -c "MX25L6406E/MX25L6408E"
```

## Status

### Tested
- coreboot 26.06-645-gc6c871909a12 as base
- EDK2 (MrChromebox/2605)
- iPXE (EDK2 only: built-in LAN & ExpressCard/34 Realtek Gb LAN)
- SeaBIOS 1.17.0
- CMOS (debug_level / hyper_threading / power_on_after_fail)
- CFR (debug_level / hyper_threading / sata_mode / power_on_after_fail)
- libgfxinit textmode (SeaBIOS) / framebuffer (EDK2)
- HDMI / LVDS / VGA working during POST, BOOT and OS
- Display (built-in screen, dimming and can be put to sleep)
- i3-2370M
- RAM 2x 4GB DDR3-1333 - SK Hynix HMT351S6CFR8C-H9 (2012-W08)
- KDE NEON 6.6 (Kernel 6.17)
- Windows 11 (Installer only detects: PS/2 KB & TP & SATA 2.5inch bay)
- PS/2 Keyboard (some function keys) & Trackpad
- Audio outputs: HDMI / Speakers / Headphones (jack detect)
- Audio inputs: built-in Microphone / Microphone (jack detect)
- SD card reader built-in (Realtek RTS5209 PCIe)
- Gigabit Ethernet built-in (Broadcom BCM57761 PCIe)
- Wake on LAN
- ExpressCard/34 (TI FW800 / Realtek Gb LAN / Fresco Logic USB 3.0)
- Mini PCIe WLAN1 (Intel Corporation Centrino Advanced-N 6235)
- Mini PCIe WWLAN1 (Atheros AR5BCB112)
- SATA 3 ports (e-SATA with hotplug & m-SATA & 2.5inch bay header)
- USB 2.0 all ports (Bluetooth / Fingerprint Sensor / Webcam)
- USB 3.0 all ports
- LEDs BATTERY (charge) & HDD & POWER (suspend color change) & WIFI
- pcspkr
- WifiButton (both fn-keyboard shortcut and mechanical slider)
- PowerButton (PowerOn/PowerOff/Wake)
- power_on_after_fail = Disable / Keep (without battery)
- Shutdown/Reboot/Sleep
- ME disabling using CMOS
- flashrom -p internal #read write erase verify 12MiB
- TPM

### Not Tested
- dock connector
- power_on_after_fail = Enable
- USBDEBUG PORT
- VBIOS

### Not Working
- ACPI ac-adapter, battery and lid devices are not detected
- Display brightness only dimmable by slider in OS (0% won't turn off)
- Display brightness keyboard shorcuts won't work, but OS slider does
- ExpressCard/34 only fails with e-SATA cards (Silicon Image SiI3132)
- ME device is not listed (although not disabled in FD or neutering)
- keyboard special function key's (P / mail / backup / mute-microphone)
- TPM command `tpm_version` standard output preceded by strange symbols

### EC/SuperIO
Although no support is added yet, the device behaves mostly the same as
on OEM firmware. It single beeps when powering on, wakes or when reset
and if an ac-power adapter is connected and or removed. The fan is also
spinning based on CPU usage and or temperature and turns silent or off
when idle!

Running from battery below a certain threshold impacts performance and
pressing the power-button or choose shutdown screen on KDE NEON will not
show that shutdown option screen as if there were hidden keyboard or
mouse presses stealing focus back to the desktop. Shutting down from cli
`sudo systemctl poweroff`, or running from an ac-adapter or a
charged battery resolves this issue.

Pressing yet unsupported FN-keyboard or special functions key's create
short stall's and or minor freezes (couple of seconds) where mouse and
and or keyboard input won't update. Also power consumption boosts to ~40
Watt during these minor freezes.

If compiled without EC_ACPI, no acpi nor `ec.asl` nor `mainboard.c` the
machine behaves the same except the beeps on restart, reboot and
ac-adapter disconnects will stop functioning.

## Variant P633-V
The service manual states P633-V series use QM77 vs HM77 chipset with
onboard Intel 82579LM (vPRO capabilities) vs Broadcom BCM57761 LAN as
only differences. Based on that info and logs from [linux-hardware.org]
a untested P633-V variant has been added. It is good practice however
to at least verify the `gpio.c` which can be obtained from OEM firmware
using `util/autoport` before trying to flash this variant!

## Energy Usage
Comparison of idle power consumption between OEM and coreboot firmware
using 1 SSD, 2 DIMM's, LAN, built-in Display (full ON or OFF) and Wi-Fi
switch set to disabled (Wi-Fi itself consumes another ~ 1,3 Watt):

```
+----------+------------+-------------+
| Firmware | Display ON | Display OFF |
+----------+------------+-------------+
| OEM 1.10 | 11,6 Watt  | 7,1 Watt    |
+----------+------------+-------------+
| coreboot | 12,4 Watt  | 7,9 Watt    |
+----------+------------+-------------+
```

## Technology

```
+------------------+------------------------------------------------------------------+
| Northbridge      | Intel Sandy/Ivy Bridge System Agent (`nb/intel/sandybridge`)     |
+------------------+------------------------------------------------------------------+
| Southbridge      | Intel HM77 / QM77 Panther Point PCH (`sb/intel/bd82x6x`)         |
+------------------+------------------------------------------------------------------+
| CPU socket       | rPGA 988B / Socket G2 (removable) (`cpu/intel/model_206ax`)      |
+------------------+------------------------------------------------------------------+
| RAM              | 2 x DDR3-1333/DDR3-1600 MT's SO-DIMM slots                       |
+------------------+------------------------------------------------------------------+
| EC / SuperIO     | Nuvoton NPCE885PA0DX                                             |
+------------------+------------------------------------------------------------------+
| Audio            | Conexant CX20588                                                 |
+------------------+------------------------------------------------------------------+
| Gigabit Ethernet | Broadcom Inc. BCM57761 / Intel Corporation 82579LM               |
+------------------+------------------------------------------------------------------+
| TPM 1.2          | Nuvoton NPCT420RA0WX                                             |
+------------------+------------------------------------------------------------------+
```

[Tutorial, part 1: Starting from scratch]: https://doc.coreboot.org/tutorial/part1.html
[linux-hardware.org]: https://linux-hardware.org/?probe=fd426b6c71
