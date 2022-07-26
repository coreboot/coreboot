# AMD Platform Security Processor (PSP) Firmware Integration Guide

The following content defines the structures of PSP tables and describes the
firmware images integrated into a functioning system.  Further details of
each Platform Security Processor (PSP) firmware blob or PSP feature are
beyond the scope of this document, and may be found in AMD NDA publications.

The current name for the security technology is "AMD Secure Processor".
To be consistent with the latest documentation, and because of familiarity
with the older name, this document continues with "Platform Security Processor"
and "PSP".

## Platform Security Processor (PSP) Overview

The Platform Security Processor (PSP) is an on-die, isolated security processor
that runs independently from the main x86 cores of the platform.
Security-sensitive components run on the PSP without being affected by the
commodity or untrusted software running on the x86 cores. The PSP executes
its own firmware and shares the SPI flash storage that is used by the
system BIOS.

## Embedded Firmware Structure

The PSP identifies its important tables by first locating the Embedded Firmware
Structure.  It reads specific addresses in the SPI flash, from top to bottom,
attempting to identify the signature.  The locations (for clarity, the x86
physical addresses) checked are:
*   0xfffa0000
*   0xfff20000
*   0xffe20000
*   0xffc20000
*   0xff820000
*   0xff020000

Most coreboot implementations provide flexibility to position the structure in
any of the eligible locations.  Below are typical definitions within the
structure (for all families combined).  Individual features supported vary by
family and model.

```eval_rst
+--------------+---------------+------------------+----------------------------+
|  Field Name  |  Offset (Hex) |  Size (In Bytes) |    Description/Purpose     |
+==============+===============+==================+============================+
|  Signature   | 0x00          | 4                | 0x55aa55aa                 |
+--------------+---------------+------------------+----------------------------+
|  IMC FW      | 0x04          | 4                | Integrated Micro           |
|              |               |                  | Controller: unsupported    |
|              |               |                  | but functional in some     |
|              |               |                  | systems                    |
+--------------+---------------+------------------+----------------------------+
| GbE FW       | 0x08          | 4                | Gigabit Ethernet           |
+--------------+---------------+------------------+----------------------------+
| xHCI FW      | 0x0c          | 4                | xHCI firmware              |
+--------------+---------------+------------------+----------------------------+
| PSP Dir Tbl  | 0x10          | 4                | Pointer to PSP Directory   |
|              |               |                  | Table (early devices)      |
+--------------+---------------+------------------+----------------------------+
| PSP Dir Tbl  | 0x14          | 4                | Pointer to PSP Directory   |
|              |               |                  | Table (later devices and   |
|              |               |                  | is combo capable)          |
+--------------+---------------+------------------+----------------------------+
| BIOS Dir Tbl | 0x18          | 4                | Pointer to BIOS Directory  |
|              |               |                  | Table for models n*        |
+--------------+---------------+------------------+----------------------------+
| BIOS Dir Tbl | 0x1c          | 4                | Pointer to BIOS Directory  |
|              |               |                  | Table for models nn        |
+--------------+---------------+------------------+----------------------------+
| BIOS Dir Tbl | 0x20          | 4                | Pointer to BIOS Directory  |
|              |               |                  | Table for models nnn       |
+--------------+---------------+------------------+----------------------------+
| …            |               |                  | ...                        |
+--------------+---------------+------------------+----------------------------+
```

* The Embedded Firmware Structure may support pointers to multiple generations
  of devices, e.g. Family 17h Models 00h-0Fh, Family 17h Models 10h-1Fh, etc.
  Details are specific to the implementation.

## PSP Directory Table

The PSP Directory Table allows the PSP to find and load various images.  A
second level table may be generated to allow updates without the risk of
corrupting the primary table.  Certain models support a combo type table,
allowing secondary tables to be referenced by device ID.  No coreboot
implementations currently use combo tables.

### PSP Directory Table Header
```eval_rst
+--------------+---------------+------------------+----------------------------+
|  Field Name  |  Offset (Hex) |  Size (In Bytes) |    Description/Purpose     |
+==============+===============+==================+============================+
| PSP Cookie   | 0x00          | 4                | PSP cookie "$PSP" to       |
|              |               |                  | recognize the header.      |
|              |               |                  | Cookie “$PL2” for level 2  |
+--------------+---------------+------------------+----------------------------+
| Checksum     | 0x04          | 4                | 32-bit CRC value of header |
|              |               |                  | below this field and       |
|              |               |                  | including all entries      |
+--------------+---------------+------------------+----------------------------+
| Total Entries| 0x08          | 4                | Number of PSP Directory    |
|              |               |                  | entries in the table       |
+--------------+---------------+------------------+----------------------------+
| Reserved     | 0x0C          | 4                | Reserved - Set to zero     |
+--------------+---------------+------------------+----------------------------+
```

### PSP Directory Table Entries
```eval_rst
+--------------+---------------+------------------+----------------------------+
|  Field Name  |  Offset (Hex) |  Size (In Bits)  |    Description/Purpose     |
+==============+===============+==================+============================+
| Type         | 0x00          | 8                | Entry type (see below)     |
+--------------+---------------+------------------+----------------------------+
| Sub Program  | 0x01          | 8                | Specifies sub program      |
+--------------+---------------+------------------+----------------------------+
| Reserved     | 0x02          | 16               | Reserved - set to 0        |
+--------------+---------------+------------------+----------------------------+
| Size         | 0x04          | 32               | Size of PSP entry in bytes |
+--------------+---------------+------------------+----------------------------+
| Location /   | 0x08          | 62               | Location: Physical Address |
| Value        |               |                  | of SPIROM location where   |
|              |               |                  | corresponding PSP entry    |
|              |               |                  | located.                   |
|              |               |                  |                            |
|              |               |                  | Value: 62-bit value for the|
|              |               |                  | PSP Entry                  |
+--------------+---------------+------------------+----------------------------+
| Address Mode | 0x0F[7:6]     | 2                | 00: x86 Physical address   |
|              |               |                  | 01: offset from start of   |
|              |               |                  |     BIOS (flash offset)    |
|              |               |                  | 02: offset from start of   |
|              |               |                  |     directory header       |
|              |               |                  | 03: offset from start of   |
|              |               |                  |     partition              |
+--------------+---------------+------------------+----------------------------+

```
### PSP Directory Table Types

**0x00**: AMD public key
*   Public key used by on-chip bootcode to verify the signature of PSP boot
    loader firmware.

**0x01**: PSP boot loader firmware
*   Second stage boot loader firmware to be loaded by on-chip bootcode.

**0x02**: PSP SecureOS firmware
*   Off-chip PSP boot loader will be overwritten in SRAM by the Secure/Trusted
    OS during initial boot up.
*   PSP SecureOS performs:
    *   Initialization of OS internal structures and instantiates the fTPM as a
        trusted application
    *   Sets up CPU/BIOS-PSP interface registers
    *   Enters steady state idling and waiting for commands
    *   In steady state, on notification, prepares for S3 state
    *   Verify and loading GFX Firmware

**0x03**: PSP recovery boot loader firmware
*   Recovery PSP boot loader image, loaded by on-chip bootcode in case of
    failure in loading PSP boot loader.

**0x08**: SMU off-chip firmware

**0x12**: SMU off-chip firmware section 2
*   Power Management firmware, responsible for system power/clock management.

**0x09**: Secure Debug unlock public key
*   Public key token used during Secure Debug unlock process to verify message
    payload from AMD server.

**0x0b**: Soft fuse chain
*   Refer to documentation for definitions.  (See External References below.)

**0x0c**: PSP trustlet binaries
*   Optional file to enable fTPM.

**0x13**: PSP Secure Debug unlock debug image
*   Secure Debug unlock firmware image, used to unlock the device.

**0x21**: Wrapped iKEK
*   Intermediate Key Encryption Key, used to decrypt encrypted firmware images.
    This is mandatory in order to support encrypted firmware.

**0x22**: PSP Token Unlock data
*   Used to support time-bound Secure Debug unlock during boot.  This entry may
    be omitted if the Token Unlock debug feature is not required.

**0x24**: Security policy binary
*   A security policy is applied to restrict the untrusted access to security
    sensitive regions.

**0x25**: MP2 firmware
*   The MP2 of the SMU, also known as the Sensor Fusion Integration is used to
    aggregate the data from various sensors such as accelerometer, gyrometer,
    ambient light sensor, orientation sensor, etc.  This is off-chip firmware
    for Sensor Fusion Processor (SFP) subsystem of the SMU.

**0x28**: System driver
*   Driver executing on top of SecureOS.

**0x30 - 0x37**: PSP AGESA binaries
*   AGESA Boot Loaders (ABLs) are a set of binary images executed by the PSP.
    They are responsible for initializing APU silicon components (including but
    not limited to APU memory interface) on S5, S4 and S3, prior to releasing
    the main cores from reset.

**0x3a**: Whitelist
*   Optional image containing a signed whitelist of one or more serial numbers.

**0x40**: Pointer to secondary table
*   Pointer to PSP Directory Table level 2.

**0x52**: PSP boot loader usermode OEM application
*   Supported only in certain SKUs.

### Firmware Version of Binaries

Every firmware binary contains 256 bytes of a PSP Header, which includes
the firmware version. The version is made up of the four bytes located at
offset 0x60 in the binary image.

For example, in the PSP BootLoader:

    0000000: 0000 0000 0000 0000 0000 0000 0000 0000  ................
    0000010: 2450 5331 c0e1 0000 0100 0000 0000 0000  $PS1............
    0000020: 5c0a ddb8 b279 4846 e154 aa4c ed7d 414d  \....yHF.T.L.}AM
    0000030: 0100 0000 0000 0000 60bb a67e 1a43 4c6b  ........`..~.CLk
    0000040: 9807 bc8d fdb4 1f40 0000 0000 0000 0000  .......@........
    0000050: 0000 0000 0000 0000 0000 0000 0000 0000  ................
    0000060: 7401 0800 ffff ffff 0001 0000 c0e3 0000  t...............
    0000070: 0000 0000 0000 0000 0000 0000 0100 0000  ................
    0000080: 4766 9186 9d5f e909 492d 491d d9ee 8e6c  Gf..._..I-I....l
    0000090: 0000 0000 0000 0000 0000 0000 0000 0000  ................
    00000a0: 0000 0000 0000 0000 0000 0000 0000 0000  ................
    00000b0: 0000 0000 0000 0000 0000 0000 0000 0000  ................
    00000c0: 0000 0000 0000 0000 0000 0000 0000 0000  ................
    00000d0: 0000 0000 0000 0000 0000 0000 0000 0000  ................
    00000e0: 0000 0000 0000 0000 0000 0000 0000 0000  ................
    00000f0: 0000 0000 0000 0000 0000 0000 0000 0000  ................

The PSP BootLoader version is 00.08.01.74.

Note that only Firmware binary images have versions.  Key tokens are not
versioned, as there will not be multiple keys.  Keys are unique to processor
family.

### BIOS Directory Table Entry Types

All x86 accessible components (both executable and data blobs) are found via
the BIOS Directory Table.  A second level table may be generated to allow for
updates without the risk of corrupting the primary table.

The BIOS Directory table structure is slightly different from the PSP Directory:
*   Multiple instances of firmware components are allowed for one specific type
*   The type field is further structured to reflect attributes of BIOS
    components such as "Region Type", "Reset Image", "Copy Image", "Read Only",
    allowing design flexibility
*   The "Destination Address" field is added for specific entries that are
    expected to be copied from boot media to specific memory location

### BIOS Directory Table Header

```eval_rst
+--------------+---------------+------------------+----------------------------+
|  Field Name  |  Offset (Hex) |  Size (In Bytes) |    Description/Purpose     |
+==============+===============+==================+============================+
| BIOS Cookie  | 0x00          | 4                | BIOS cookie "$BHD" to      |
|              |               |                  | recognize the header.      |
|              |               |                  | Cookie “$BL2” for level 2  |
+--------------+---------------+------------------+----------------------------+
| Checksum     | 0x04          | 4                | 32 bit CRC value of header |
|              |               |                  | below this field and       |
|              |               |                  | including all entries      |
+--------------+---------------+------------------+----------------------------+
| Total Entries| 0x08          | 4                | Number of BIOS Directory   |
|              |               |                  | entries in the table       |
+--------------+---------------+------------------+----------------------------+
| Reserved     | 0x0C          | 4                | Reserved - Set to zero     |
+--------------+---------------+------------------+----------------------------+
```

### BIOS Directory Table Entries

```eval_rst
+--------------+---------------+------------------+----------------------------+
|  Field Name  |  Offset (Hex) |  Size (In Bits)  |    Description/Purpose     |
+==============+===============+==================+============================+
| Type         | 0x00          | 8                | Entry type (see below)     |
+--------------+---------------+------------------+----------------------------+
| Region Type  | 0x01          | 8                | Setup the memory region's  |
|              |               |                  | security attribute for the |
|              |               |                  | BIOS entry                 |
+--------------+---------------+------------------+----------------------------+
| Reset Image  | 0x02[0]       | 1                | Boolean value to define the|
|              |               |                  | BIOS entry is a reset      |
|              |               |                  | binary image               |
+--------------+---------------+------------------+----------------------------+
| Copy Image   | 0x02[1]       | 1                | Define the binary image of |
|              |               |                  | the BIOS entry is for      |
|              |               |                  | copying over to the memory |
|              |               |                  | region                     |
+--------------+---------------+------------------+----------------------------+
| Read Only    | 0x02[2]       | 1                | Setup the memory region for|
|              |               |                  | the BIOS entry to read only|
+--------------+---------------+------------------+----------------------------+
| Compressed   | 0x02[3]       | 1                | Compressed using zlib      |
|              |               |                  |                            |
+--------------+---------------+------------------+----------------------------+
| Instance     | 0x02[7:4]     | 4                | Specify the Instance of an |
|              |               |                  | entry                      |
+--------------+---------------+------------------+----------------------------+
| SubProgram   | 0x03[2:0]     | 3                | Specify the SubProgram     |
+--------------+---------------+------------------+----------------------------+
| RomId        | 0x03[4:3]     | 2                | Which SPI device the       |
|              |               |                  | content is placed in       |
+--------------+---------------+------------------+----------------------------+
| Writeable    | 0x03[5]       | 1                | Region is writable or read |
|              |               |                  | only                       |
+--------------+---------------+------------------+----------------------------+
| Reserved     | 0x03[7:6]     | 2                | Reserved - Set to zero     |
+--------------+---------------+------------------+----------------------------+
| Size         | 0x04          | 32               | Memory Region Size         |
+--------------+---------------+------------------+----------------------------+
| Source       | 0x08          | 62               | Physical Address of SPIROM |
| Address      |               |                  | location where the data for|
|              |               |                  | the corresponding entry is |
|              |               |                  | located                    |
+--------------+---------------+------------------+----------------------------+
| Entry Address| 0x0F[7:6]     | 2                | Same as Entry Address Mode |
| Mode         |               |                  | in PSP directory table     |
|              |               |                  | entry fields               |
+--------------+---------------+------------------+----------------------------+
| Destination  | 0x10          | 64               | Destination Address of     |
| Address      |               |                  | memory location where the  |
|              |               |                  | data for the corresponding |
|              |               |                  | BIOS Entry is copied       |
+--------------+---------------+------------------+----------------------------+
```

### BIOS Directory Table Entry Types

**0x60**: APCB data
*   Source field points to the AGESA PSP Customization Block (APCB) data.

**0x68**: Backup copy of APCB data
*   Source field points to the backup copy of the AGESA PSP Customization Block
    (APCB) data.

**0x61**: APOB data
*   Location field points to the AGESA PSP Output Block (APOB) data.

**0x62**: BIOS reset image
*   Source field points to BIOS binary image in flash.  Destination points to
    DRAM.

**0x63**: APOB data NV
*   Source field points to the AGESA PSP Output Block (APOB) data NV copy.
    This data is written by coreboot and replayed by PSP ABLs during S3 resume
    and in certain S5 boots.

**0x64**: PMU firmware (instruction)
*   Source field points to the instruction portion of Phy Microcontroller Unit
    firmware.

**0x65**: PMU firmware (data)
*   Source field points to the data portion of Phy Microcontroller Unit
    firmware.

**0x66**: x86 microcode patch
*   Source field points to the microcode patch.

**0x6a**: MP2 FW config file
*   Source field points to the MP2 FW configuration file.

**0x70**: Pointer to secondary table
*   Pointer to BIOS Directory Table level 2.

## Tools

### amdcompress

`cbfstool/amdcompress` is a helper for creating the BIOS Reset Image (BIOS
Directory Table type 0x62).  This is the code the PSP uncompresses into DRAM
at the location where the x86 begins execution when released from reset.
Typical usage is for amdcompress to convert an ELF file’s program section
into a zlib compressed image.

### amdfwtool

All images requiring PSP functionality rely on the amdfwtool utility.
amdfwtool takes image names as command-line arguments, as well as the size of
the flash device, and intended location of the Embedded Firmware Structure.
Its output is a monolithic image with correctly positioned headers, pointers,
structures, and the firmware images added.  The file, typically named
`amdfw.rom`, may then be added directly into the coreboot image.

## External Reference

*   NDA document #55758: *AMD Platform Security Processor BIOS Architecture
    Design Guide for AMD Family 17h Processors*
*   NDA document #54267 *AMD Platform Security Processor BIOS Architecture
    Design Guide*: For all devices earlier than Family 17h
