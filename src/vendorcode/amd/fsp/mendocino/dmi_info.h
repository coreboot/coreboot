 /*****************************************************************************
 *
 * Copyright (c) 2008 - 2017, Advanced Micro Devices, Inc.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of Advanced Micro Devices, Inc. nor the names of
 *       its contributors may be used to endorse or promote products derived
 *       from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL ADVANCED MICRO DEVICES, INC. BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 ***************************************************************************/

/**
 * This code was copied from src/vendorcode/amd/pi/00670F00/AGESA.h
 */

#define AMD_FSP_DMI_HOB_GUID  {0x4118FC0E, 0x353D, 0x4726, { 0x97, 0xC0, 0x53, 0xCD, 0x92, 0xB6, 0x49, 0x25}}

// Our ACPI HOB max payload, accounting for the size of the HOB header as well as the information structure
#define HOB_MAX_SIZE                0xFFF8
#define HOB_GUID_EXTENSION_SIZE     (HOB_MAX_SIZE - sizeof (EFI_HOB_GUID_TYPE))

#define MAX_SOCKETS_SUPPORTED     2   ///< Max number of sockets in system
#define MAX_CHANNELS_PER_SOCKET   8   ///< Max Channels per sockets
#define MAX_DIMMS_PER_CHANNEL     4   ///< Max DIMMs on a memory channel (independent of platform)

/// DMI Type 16 offset 04h - Location
typedef enum {
  OtherLocation = 0x01,                                 ///< Assign 01 to Other
  UnknownLocation,                                      ///< Assign 02 to Unknown
  SystemboardOrMotherboard,                             ///< Assign 03 to systemboard or motherboard
  IsaAddonCard,                                         ///< Assign 04 to ISA add-on card
  EisaAddonCard,                                        ///< Assign 05 to EISA add-on card
  PciAddonCard,                                         ///< Assign 06 to PCI add-on card
  McaAddonCard,                                         ///< Assign 07 to MCA add-on card
  PcmciaAddonCard,                                      ///< Assign 08 to PCMCIA add-on card
  ProprietaryAddonCard,                                 ///< Assign 09 to proprietary add-on card
  NuBus,                                                ///< Assign 0A to NuBus
  Pc98C20AddonCard,                                     ///< Assign 0A0 to PC-98/C20 add-on card
  Pc98C24AddonCard,                                     ///< Assign 0A1 to PC-98/C24 add-on card
  Pc98EAddoncard,                                       ///< Assign 0A2 to PC-98/E add-on card
  Pc98LocalBusAddonCard                                 ///< Assign 0A3 to PC-98/Local bus add-on card
} DMI_T16_LOCATION;

/// DMI Type 16 offset 05h - Memory Error Correction
typedef enum {
  OtherUse = 0x01,                                      ///< Assign 01 to Other
  UnknownUse,                                           ///< Assign 02 to Unknown
  SystemMemory,                                         ///< Assign 03 to system memory
  VideoMemory,                                          ///< Assign 04 to video memory
  FlashMemory,                                          ///< Assign 05 to flash memory
  NonvolatileRam,                                       ///< Assign 06 to non-volatile RAM
  CacheMemory                                           ///< Assign 07 to cache memory
} DMI_T16_USE;

/// DMI Type 16 offset 07h - Maximum Capacity
typedef enum {
  Dmi16OtherErrCorrection = 0x01,                       ///< Assign 01 to Other
  Dmi16UnknownErrCorrection,                            ///< Assign 02 to Unknown
  Dmi16NoneErrCorrection,                               ///< Assign 03 to None
  Dmi16Parity,                                          ///< Assign 04 to parity
  Dmi16SingleBitEcc,                                    ///< Assign 05 to Single-bit ECC
  Dmi16MultiBitEcc,                                     ///< Assign 06 to Multi-bit ECC
  Dmi16Crc                                              ///< Assign 07 to CRC
} DMI_T16_ERROR_CORRECTION;

/// DMI Type 16 - Physical Memory Array
typedef struct {
  OUT DMI_T16_LOCATION          Location;               ///< The physical location of the Memory Array,
                                                        ///< whether on the system board or an add-in board.
  OUT DMI_T16_USE               Use;                    ///< Identifies the function for which the array
                                                        ///< is used.
  OUT DMI_T16_ERROR_CORRECTION  MemoryErrorCorrection;  ///< The primary hardware error correction or
                                                        ///< detection method supported by this memory array.
  OUT UINT16                    NumberOfMemoryDevices;  ///< The number of slots or sockets available
                                                        ///< for memory devices in this array.
} TYPE16_DMI_INFO;

/// DMI Type 17 offset 0Eh - Form Factor
typedef enum {
  OtherFormFactor = 0x01,                               ///< Assign 01 to Other
  UnknowFormFactor,                                     ///< Assign 02 to Unknown
  SimmFormFactor,                                       ///< Assign 03 to SIMM
  SipFormFactor,                                        ///< Assign 04 to SIP
  ChipFormFactor,                                       ///< Assign 05 to Chip
  DipFormFactor,                                        ///< Assign 06 to DIP
  ZipFormFactor,                                        ///< Assign 07 to ZIP
  ProprietaryCardFormFactor,                            ///< Assign 08 to Proprietary Card
  DimmFormFactorFormFactor,                             ///< Assign 09 to DIMM
  TsopFormFactor,                                       ///< Assign 10 to TSOP
  RowOfChipsFormFactor,                                 ///< Assign 11 to Row of chips
  RimmFormFactor,                                       ///< Assign 12 to RIMM
  SodimmFormFactor,                                     ///< Assign 13 to SODIMM
  SrimmFormFactor,                                      ///< Assign 14 to SRIMM
  FbDimmFormFactor                                      ///< Assign 15 to FB-DIMM
} DMI_T17_FORM_FACTOR;

/// DMI Type 17 offset 12h - Memory Type
typedef enum {
  OtherMemType = 0x01,                                  ///< Assign 01 to Other
  UnknownMemType,                                       ///< Assign 02 to Unknown
  DramMemType,                                          ///< Assign 03 to DRAM
  EdramMemType,                                         ///< Assign 04 to EDRAM
  VramMemType,                                          ///< Assign 05 to VRAM
  SramMemType,                                          ///< Assign 06 to SRAM
  RamMemType,                                           ///< Assign 07 to RAM
  RomMemType,                                           ///< Assign 08 to ROM
  FlashMemType,                                         ///< Assign 09 to Flash
  EepromMemType,                                        ///< Assign 10 to EEPROM
  FepromMemType,                                        ///< Assign 11 to FEPROM
  EpromMemType,                                         ///< Assign 12 to EPROM
  CdramMemType,                                         ///< Assign 13 to CDRAM
  ThreeDramMemType,                                     ///< Assign 14 to 3DRAM
  SdramMemType,                                         ///< Assign 15 to SDRAM
  SgramMemType,                                         ///< Assign 16 to SGRAM
  RdramMemType,                                         ///< Assign 17 to RDRAM
  DdrMemType,                                           ///< Assign 18 to DDR
  Ddr2MemType,                                          ///< Assign 19 to DDR2
  Ddr2FbdimmMemType,                                    ///< Assign 20 to DDR2 FB-DIMM
  Ddr3MemType = 0x18,                                   ///< Assign 24 to DDR3
  Fbd2MemType,                                          ///< Assign 25 to FBD2
  Ddr4MemType,                                          ///< Assign 26 to DDR4
  LpDdrMemType,                                         ///< Assign 27 to LPDDR
  LpDdr2MemType,                                        ///< Assign 28 to LPDDR2
  LpDdr3MemType,                                        ///< Assign 29 to LPDDR3
  LpDdr4MemType,                                        ///< Assign 30 to LPDDR4
  Ddr5MemType = 0x22,                                   ///< Assign 34 to DDR5
  LpDdr5MemType,                                        ///< Assign 35 to LPDDR5
  LpDdr5xMemType,                                       ///< Assign 36 to LPDDR5X
} DMI_T17_MEMORY_TYPE;

/// DMI Type 17 offset 13h - Type Detail
typedef struct {
  OUT UINT16                    Reserved1:1;            ///< Reserved
  OUT UINT16                    Other:1;                ///< Other
  OUT UINT16                    Unknown:1;              ///< Unknown
  OUT UINT16                    FastPaged:1;            ///< Fast-Paged
  OUT UINT16                    StaticColumn:1;         ///< Static column
  OUT UINT16                    PseudoStatic:1;         ///< Pseudo-static
  OUT UINT16                    Rambus:1;               ///< RAMBUS
  OUT UINT16                    Synchronous:1;          ///< Synchronous
  OUT UINT16                    Cmos:1;                 ///< CMOS
  OUT UINT16                    Edo:1;                  ///< EDO
  OUT UINT16                    WindowDram:1;           ///< Window DRAM
  OUT UINT16                    CacheDram:1;            ///< Cache Dram
  OUT UINT16                    NonVolatile:1;          ///< Non-volatile
  OUT UINT16                    Registered:1;           ///< Registered (Buffered)
  OUT UINT16                    Unbuffered:1;           ///< Unbuffered (Unregistered)
  OUT UINT16                    LRDIMM:1;               ///< LRDIMM
} DMI_T17_TYPE_DETAIL;

/// DMI Type 17 offset 28h - Memory Technology
typedef enum {
  OtherType = 0x01,                                     ///< Assign 01 to Other
  UnknownType = 0x02,                                   ///< Assign 02 to Unknown
  DramType = 0x03,                                      ///< Assign 03 to DRAM
  NvDimmNType = 0x04,                                   ///< Assign 04 to NVDIMM-N
  NvDimmFType = 0x05,                                   ///< Assign 05 to NVDIMM-F
  NvDimmPType = 0x06,                                   ///< Assign 06 to NVDIMM-P
  IntelPersistentMemoryType = 0x07,                     ///< Assign 07 to Intel persistent memory
} DMI_T17_MEMORY_TECHNOLOGY;

/// DMI Type 17 offset 29h - Memory Operating Mode Capability
typedef struct {
  OUT UINT16                    Reserved1:1;            ///< Reserved, set to 0
  OUT UINT16                    Other:1;                ///< Other
  OUT UINT16                    Unknown:1;              ///< Unknown
  OUT UINT16                    VolatileMemory:1;       ///< Volatile memory
  OUT UINT16                    ByteAccessiblePersistentMemory:1;    ///< Byte-accessible persistent memory
  OUT UINT16                    BlockAccessiblePersistentMemory:1;   ///< Block-accessible persistent memory
  OUT UINT16                    Reserved2:10;           ///< Reserved, set to 0
} DMI_T17_MEMORY_OPERATING_MODE_CAPABILITY;

typedef union {
  DMI_T17_MEMORY_OPERATING_MODE_CAPABILITY  AsBitmap;
  UINT16                                    AsUint16;
} DMI_T17_MEMORY_OPERATING_MODE_CAPABILITY_VAR;

/// DMI Type 17 - Memory Device
typedef struct {
  OUT UINT16                    Handle;                 ///< The temporary handle, or instance number, associated with the structure
  OUT UINT16                    TotalWidth;             ///< Total Width, in bits, of this memory device, including any check or error-correction bits.
  OUT UINT16                    DataWidth;              ///< Data Width, in bits, of this memory device.
  OUT UINT16                    MemorySize;             ///< The size of the memory device.
  OUT DMI_T17_FORM_FACTOR       FormFactor;             ///< The implementation form factor for this memory device.
  OUT UINT8                     DeviceSet;              ///< Identifies when the Memory Device is one of a set of
                                                        ///< Memory Devices that must be populated with all devices of
                                                        ///< the same type and size, and the set to which this device belongs.
  OUT CHAR8                     DeviceLocator[8];       ///< The string number of the string that identifies the physically labeled socket or board position where the memory device is located.
  OUT CHAR8                     BankLocator[13];        ///< The string number of the string that identifies the physically labeled bank where the memory device is located.
  OUT DMI_T17_MEMORY_TYPE       MemoryType;             ///< The type of memory used in this device.
  OUT DMI_T17_TYPE_DETAIL       TypeDetail;             ///< Additional detail on the memory device type
  OUT UINT16                    Speed;                  ///< Identifies the speed of the device, in megahertz (MHz).
  OUT UINT64                    ManufacturerIdCode;     ///< Manufacturer ID code.
  OUT CHAR8                     SerialNumber[9];        ///< Serial Number.
  OUT CHAR8                     PartNumber[21];         ///< Part Number.
  OUT UINT8                     Attributes;             ///< Bits 7-4: Reserved, Bits 3-0: rank.
  OUT UINT32                    ExtSize;                ///< Extended Size.
  OUT UINT16                    ConfigSpeed;            ///< Configured memory clock speed
  OUT UINT16                    MinimumVoltage;         ///< Minimum operating voltage for this device, in millivolts
  OUT UINT16                    MaximumVoltage;         ///< Maximum operating voltage for this device, in millivolts
  OUT UINT16                    ConfiguredVoltage;      ///< Configured voltage for this device, in millivolts
  // SMBIOS 3.2
  OUT UINT8                     MemoryTechnology;       ///< Memory technology type for this memory device
  OUT DMI_T17_MEMORY_OPERATING_MODE_CAPABILITY_VAR   MemoryOperatingModeCapability; ///< The operating modes supported by this memory device
  OUT CHAR8                     FirmwareVersion[10];    ///< String number for the firmware version of this memory device
  OUT UINT16                    ModuleManufacturerId;   ///< The two-byte module manufacturer ID found in the SPD of this memory device; LSB first.
  OUT UINT16                    ModuleProductId;        ///< The two-byte module product ID found in the SPD of this memory device; LSB first
  OUT UINT16                    MemorySubsystemControllerManufacturerId; //< The two-byte memory subsystem controller manufacturer ID found in the SPD of this memory device; LSB first
  OUT UINT16                    MemorySubsystemControllerProductId; //< The two-byte memory subsystem controller product ID found in the SPD of this memory device; LSB first
  OUT UINT64                    NonvolatileSize;        ///< Size of the Non-volatile portion of the memory device in Bytes, if any.
  OUT UINT64                    VolatileSize;           ///< Size of the Volatile portion of the memory device in Bytes, if any.
  OUT UINT64                    CacheSize;              ///< Size of the Cache portion of the memory device in Bytes, if any.
  OUT UINT64                    LogicalSize;            ///< Size of the Logical memory device in Bytes.
  // SMBIOS 3.3
  OUT UINT32                    ExtendedSpeed;          ///< Extended Speed
  OUT UINT32                    ExtendedConfiguredMemorySpeed; ///< Extended Configured memory speed
} __packed TYPE17_DMI_INFO;

/// Collection of pointers to the DMI records
typedef struct {
  OUT TYPE16_DMI_INFO           T16;                          ///< Type 16 struc
  OUT TYPE17_DMI_INFO           T17[MAX_SOCKETS_SUPPORTED][MAX_CHANNELS_PER_SOCKET][MAX_DIMMS_PER_CHANNEL]; ///< Type 17 struc
} DMI_INFO;
