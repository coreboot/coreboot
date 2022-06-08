# vboot - Verified Boot Support

Google's verified boot support consists of:

*   A root of trust
*   Special firmware layout
*   Firmware verification
*   Firmware measurements
*   A firmware update mechanism
*   Specific build flags
*   Signing the coreboot image

Google's vboot verifies the firmware and places measurements within the TPM.

- [List of supported Devices](list_vboot.md)

***

## Root of Trust

When using vboot, the root-of-trust is basically the read-only portion of the
SPI flash. The following items factor into the trust equation:

* The GCC compiler must reliably translate the code into machine code
  without inserting any additional code (virus, backdoor, etc.)
* The CPU must reliably execute the reset sequence and instructions as
  documented by the CPU manufacturer.
* The SPI flash must provide only the code programmed into it to the CPU
  without providing any alternative reset vector or code sequence.
* The SPI flash must honor the write-protect input and protect the specified
  portion of the SPI flash from all erase and write accesses.

The firmware is typically protected using the write-protect pin on the SPI
flash part and setting some of the write-protect bits in the status register
during manufacturing. The protected area is platform specific and for x86
platforms is typically 1/4th of the SPI flash part size.
Because this portion of the SPI flash is hardware write protected, it is not
possible to update this portion of the SPI flash in the field, without altering
the system to eliminate the ground connection to the SPI flash write-protect pin.
Without hardware modifications, this portion of the SPI flash maintains the
manufactured state during the system's lifetime.

***

## Firmware Layout

Several sections are added to the firmware layout to support vboot:

* Read-only section
* Google Binary Blob (GBB) area
* Read/write section A
* Read/write section B

The following sections describe the various portions of the flash layout.

### Read-Only Section

The read-only section contains a coreboot file system (CBFS) that contains all
of the boot firmware necessary to perform recovery for the system. This firmware
is typically protected using the write-protect pin on the SPI flash part and
setting some of the write-protect bits in the status register during
manufacturing.
The protected area is typically 1/4th of the SPI flash part size and must cover
the entire read-only section which consists of:

* Vital Product Data (VPD) area
* Firmware ID area
* Google Binary Blob (GBB) area
* coreboot file system containing read-only recovery firmware

### Google Binary Blob (GBB) Area

The GBB area is part of the read-only section. This area contains a 4096 or 8192
bit public root RSA key that is used to verify the *VBLOCK* area to obtain the
firmware signing key.

### Recovery Firmware

The recovery firmware is contained within a coreboot file system and consists of:

* reset vector
* bootblock
* verstage
* romstage
* postcar
* ramstage
* payload
* flash map file
* config file
* processor specific files:
  * Microcode
  * fspm.bin
  * fsps.bin

The recovery firmware is written during manufacturing and typically contains
code to write the storage device (eMMC device or hard disk). The recovery image
is usually contained on a socketed device such as a USB flash drive or an
SD card. Depending upon the payload firmware doing the recovery, it may be
possible for the user to interact with the system to specify the recovery
image path. Part of the recovery is also to write the A and B areas of the SPI
flash device to boot the system.

### Read/Write Section

The read/write sections contain an area which contains the firmware signing
key and signature and an area containing a coreboot file system with a subset
of the firmware. The firmware files in *FW_MAIN_A* and *FW_MAIN_B* are:

* romstage
* postcar
* ramstage
* payload
* config file
* processor specific files:
  * Microcode
  * fspm.bin
  * fsps.bin

The firmware subset enables most issues to be fixed in the field with firmware
updates. The firmware files handle memory and most of silicon initialization.
These files also produce the tables which get passed to the operating system.

***

## Firmware Updates

The read/write sections exist in one of three states:

* Invalid
* Ready to boot
* Successfully booted


Firmware updates are handled by the operating system by writing any read/write
section that is not in the "successfully booted" state. Upon the next reboot,
vboot determines the section to boot. If it finds one in the "ready to boot"
state then it attempts to boot using that section. If the boot fails then
vboot marks the section as invalid and attempts to fall back to a read/write
section in the "successfully booted" state. If vboot is not able to find a
section in the "successfully booted" state then vboot enters recovery mode.

Only the operating system is able to transition a section from the
"ready to boot" state to the "successfully booted" state.
The transition is typically done after the operating system has been running
for a while indicating that successful boot was possible and the operating
system is stable.

Note that as long as the SPI write protection is in place then the system
is always recoverable. If the flash update fails then the system will continue
to boot using the previous read/write area. The same is true if coreboot passes
control to the payload or the operating system and then the boot fails. In the
worst case, the SPI flash gets totally corrupted in which case vboot fails the
signature checks and enters recovery mode. There are no times where the SPI
flash is exposed and the reset vector or part of the recovery firmware gets
corrupted.

***

## Build Flags

The following *Kconfig* values need to be selected to enable vboot:

* COLLECT_TIMESTAMPS
* VBOOT

The starting stage needs to be specified by selecting either
VBOOT_STARTS_IN_BOOTBLOCK or VBOOT_STARTS_IN_ROMSTAGE.

If vboot starts in bootblock then vboot may be built as a separate stage by
selecting `VBOOT_SEPARATE_VERSTAGE`. Additionally, if static RAM is too small
to fit both verstage and romstage then selecting `VBOOT_RETURN_FROM_VERSTAGE`
enables bootblock to reuse the RAM occupied by verstage for romstage.

Non-volatile flash is needed for vboot operation. This flash area may be in
CMOS, the EC, or in a read/write area of the SPI flash device.
Select one of the following:

* `VBOOT_VBNV_CMOS`
* `VBOOT_VBNV_FLASH`

More non-volatile storage features may be found in `security/vboot/Kconfig`.

A TPM is also required for vboot operation.
TPMs are available in `drivers/i2c/tpm` and `drivers/pc80/tpm`.

In addition to adding the coreboot files into the read-only region,
enabling vboot causes the build script to add the read/write files into
coreboot file systems in *FW_MAIN_A* and *FW_MAIN_B*.

**RO_REGION_ONLY**

The files added to this list will only be placed in the read-only region and
not into the read/write coreboot file systems in *FW_MAIN_A* and *FW_MAIN_B*.

**VBOOT_ENABLE_CBFS_FALLBACK**

Normally coreboot will use the active read/write coreboot file system for all
of it's file access when vboot is active and is not in recovery mode.

When the `VBOOT_ENABLE_CBFS_FALLBACK` option is enabled the cbfs file system will
first try to locate a file in the active read/write file system. If the file
doesn't exist here the file system will try to locate the file in the read-only
file system.

This option can be used to prevent duplication of static data. Files can be
removed from the read/write partitions by adding them to the `RO_REGION_ONLY`
config. If a file needs to be changed in a later stage simply remove it from
this list.

***

## Signing the coreboot Image

The following command script is an example of how to sign the coreboot image
file. This script is used on the Intel Galileo board and creates the *GBB* area
and inserts it into the coreboot image. It also updates the *VBLOCK* areas with
the firmware signing key and the signature for the *FW_MAIN* firmware.
More details are available in `3rdparty/vboot/README`.

```bash
#!/bin/sh
#
#  The necessary tools were built and installed using the following commands:
#
#        pushd 3rdparty/vboot
#        make
#        sudo make install
#        popd
#
#  The keys were made using the following command
#
#        3rdparty/vboot/scripts/keygeneration/create_new_keys.sh  \
#                --output $PWD/keys
#
#
#  The "magic" numbers below are derived from the GBB section in
#  src/mainboard/intel/galileo/vboot.fmd.
#
#  GBB Header Size:     0x80
#  GBB Offset:      0x611000, 4KiB block number: 1553 (0x611)
#  GBB Length:       0x7f000, 4KiB blocks:        127  (0x7f)
#  COREBOOT Offset: 0x690000, 4KiB block number: 1680 (0x690)
#  COREBOOT Length: 0x170000, 4KiB blocks:        368 (0x170)
#
#  0x7f000 (GBB Length) = 0x80 + 0x100 + 0x1000 + 0x7ce80 + 0x1000
#
#  Create the GBB area blob
#  Parameters: hwid_size,rootkey_size,bmpfv_size,recoverykey_size
#
gbb_utility -c 0x100,0x1000,0x7ce80,0x1000 gbb.blob

#
#  Copy from the start of the flash to the GBB region into the signed flash
#  image.
#
#  1553 * 4096 = 0x611 * 0x1000 = 0x611000, size of area before GBB
#
dd  conv=fdatasync  ibs=4096  obs=4096  count=1553  \
if=build/coreboot.rom  of=build/coreboot.signed.rom

#
#  Append the empty GBB area to the coreboot.rom image.
#
#  1553 * 4096 = 0x611 * 0x1000 = 0x611000, offset to GBB
#
dd  conv=fdatasync  obs=4096  obs=4096  seek=1553  if=gbb.blob  \
of=build/coreboot.signed.rom

#
#  Append the rest of the read-only region into the signed flash image.
#
#  1680 * 4096 = 0x690 * 0x1000 = 0x690000, offset to COREBOOT area
#   368 * 4096 = 0x170 * 0x1000 = 0x170000, length of COREBOOT area
#
dd  conv=fdatasync  ibs=4096  obs=4096  skip=1680  seek=1680  count=368  \
if=build/coreboot.rom  of=build/coreboot.signed.rom

#
#  Insert the HWID and public root and recovery RSA keys into the GBB area.
#
gbb_utility                          \
--set --hwid='Galileo'            \
-r $PWD/keys/recovery_key.vbpubk  \
-k $PWD/keys/root_key.vbpubk      \
build/coreboot.signed.rom

#
#  Sign the read/write firmware areas with the private signing key and update
#  the VBLOCK_A and VBLOCK_B regions.
#
3rdparty/vboot/scripts/image_signing/sign_firmware.sh  \
build/coreboot.signed.rom                           \
$PWD/keys                                           \
       build/coreboot.signed.rom
```

***

## Boot Flow

The reset vector exist in the read-only area and points to the bootblock
entry point. The only copy of the bootblock exists in the read-only area
of the SPI flash. Verstage may be part of the bootblock or a separate stage.
If separate then the bootblock loads verstage from the read-only area and
transfers control to it.

Upon first boot, verstage attempts to verify the read/write section A.
It gets the public root key from the GBB area and uses that to verify the
*VBLOCK* area in read-write section A. If the *VBLOCK* area is valid then it
extracts the firmware signing key (1024-8192 bits) and uses that to verify
the *FW_MAIN_A* area of read/write section A. If the verification is successful
then verstage instructs coreboot to use the coreboot file system in read/write
section A for the contents of the remaining boot firmware (romstage, postcar,
ramstage and the payload).

If verification fails for the read/write area and the other read/write area is
not valid vboot falls back to the read-only area to boot into system recovery.

***

## Chromebook Special Features

Google's Chromebooks have some special features:

* Developer mode
* Write-protect screw

### Developer Mode

Developer mode allows the user to use coreboot to boot another operating system.
This may be a another (beta) version of Chrome OS, or another flavor of
GNU/Linux. Use of developer mode does not void the system warranty. Upon entry
into developer mode, all locally saved data on the system is lost.
This prevents someone from entering developer mode to subvert the system
security to access files on the local system or cloud.

### Write Protect Screw

Chromebooks have a write-protect screw which provides the ground to the
write-protect pin of the SPI flash.
Google specifically did this to allow the manufacturing line and advanced
developers to re-write the entire SPI flash part. Once the screw is removed,
any firmware may be placed on the device.
However, accessing this screw requires opening the case and voids the
system warranty!
