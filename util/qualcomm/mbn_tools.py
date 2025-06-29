#!/usr/bin/env python3
#===============================================================================
#
# MBN TOOLS
#
# GENERAL DESCRIPTION
#    Contains all MBN Utilities for image generation
#
# SPDX-License-Identifier: BSD-3-Clause

#-------------------------------------------------------------------------------
#                      EDIT HISTORY FOR FILE
#
#  This section contains comments describing changes made to the module.
#  Notice that changes are listed in reverse chronological order.
#
# when       who     what, where, why
# --------   ---     ---------------------------------------------------------
# 05/21/18   rissha  Added support for extended MBNV6 and Add support for hashing elf segments with SHA384
# 03/22/18   thiru   Added support for extended MBNV5.
# 06/06/13   yliong  CR 497042: Signed and encrypted image is corrupted. MRC features.
# 03/18/13   dhaval  Add support for hashing elf segments with SHA256 and
#                    sync up to mpss, adsp mbn-tools
# 01/14/13   kedara  Remove dependency on .builds, cust<bid>.h, targ<bid>.h files
# 08/30/12   kedara  Add virtual block suppport
# 02/24/12   dh      Add ssd side effect file names
# 07/08/11   aus     Added support for image_id in SBL image header as required by PBL
#                    Sahara mode
# 10/20/11   dxiang  Clean up
#===============================================================================

import stat
import csv
import itertools
import struct
import os
import shutil
import hashlib

#----------------------------------------------------------------------------
# GLOBAL VARIABLES BEGIN
#----------------------------------------------------------------------------
PAD_BYTE_1                  = 255           # Padding byte 1s
PAD_BYTE_0                  = 0             # Padding byte 0s
SHA256_SIGNATURE_SIZE       = 256           # Support SHA256
MAX_NUM_ROOT_CERTS          = 4             # Maximum number of OEM root certificates
MI_BOOT_SBL_HDR_SIZE        = 80            # sizeof(sbl_header)
BOOT_HEADER_LENGTH          = 20            # Boot Header Number of Elements
SBL_HEADER_LENGTH           = 20            # SBL Header Number of Elements
MAX_PHDR_COUNT              = 100           # Maximum allowable program headers
CERT_CHAIN_ONEROOT_MAXSIZE  = 6*1024        # Default Cert Chain Max Size for one root
VIRTUAL_BLOCK_SIZE          = 131072        # Virtual block size for MCs insertion in SBL1 if ENABLE_VIRTUAL_BLK ON
MAGIC_COOKIE_LENGTH         = 12            # Length of magic Cookie inserted per VIRTUAL_BLOCK_SIZE
MIN_IMAGE_SIZE_WITH_PAD     = 256*1024      # Minimum image size for sbl1 Nand based OTA feature

SBL_AARCH64                 = 0xF           # Indicate that SBL is a Aarch64 image
SBL_AARCH32                 = 0x0           # Indicate that SBL is a Aarch32 image

# Magic numbers filled in for boot headers
FLASH_CODE_WORD                       = 0x844BDCD1
UNIFIED_BOOT_COOKIE_MAGIC_NUMBER      = 0x33836685
MAGIC_NUM                             = 0x73D71034
AUTODETECT_PAGE_SIZE_MAGIC_NUM        = 0x7D0B435A
AUTODETECT_PAGE_SIZE_MAGIC_NUM64      = 0x7D0B5436
AUTODETECT_PAGE_SIZE_MAGIC_NUM128     = 0x7D0B6577
SBL_VIRTUAL_BLOCK_MAGIC_NUM           = 0xD48B54C6

# ELF Definitions
ELF_HDR_COMMON_SIZE       = 24
ELF32_HDR_SIZE            = 52
ELF32_PHDR_SIZE           = 32
ELF64_HDR_SIZE            = 64
ELF64_PHDR_SIZE           = 56
ELFINFO_MAG0_INDEX        = 0
ELFINFO_MAG1_INDEX        = 1
ELFINFO_MAG2_INDEX        = 2
ELFINFO_MAG3_INDEX        = 3
ELFINFO_MAG0              = 127     # 0x7F
ELFINFO_MAG1              = 69      # E
ELFINFO_MAG2              = 76      # L
ELFINFO_MAG3              = 70      # F
ELFINFO_CLASS_INDEX       = 4
ELFINFO_CLASS_32          = 1
ELFINFO_CLASS_64          = 2
ELFINFO_VERSION_INDEX     = 6
ELFINFO_VERSION_CURRENT   = 1
ELF_BLOCK_ALIGN           = 0x1000
ALIGNVALUE_1MB             = 0x100000
ALIGNVALUE_4MB            = 0x400000
ELFINFO_DATA2LSB          = b'\x01'
ELFINFO_EXEC_ETYPE        = b'\x02\x00'
ELFINFO_ARM_MACHINETYPE   = b'\x28\x00'
ELFINFO_VERSION_EV_CURRENT = b'\x01\x00\x00\x00'
ELFINFO_SHOFF             = 0x00
ELFINFO_PHNUM             = b'\x01\x00'
ELFINFO_RESERVED          = 0x00

# ELF Program Header Types
NULL_TYPE                 = 0x0
LOAD_TYPE                 = 0x1
DYNAMIC_TYPE              = 0x2
INTERP_TYPE               = 0x3
NOTE_TYPE                 = 0x4
SHLIB_TYPE                = 0x5
PHDR_TYPE                 = 0x6
TLS_TYPE                  = 0x7

"""
The eight bits between 20 and 27 in the p_flags field in ELF program headers
is not used by the standard ELF format. We use this byte to hold OS and processor
specific fields as recommended by ARM.

The bits in this byte are defined as follows:

                   Pool Indx    Segment type     Access type    Page/non page
  bits in p_flags /-----27-----/----26-24-------/---- 23-21----/------20-------/

After parsing segment description strings in the SCL file, the appropriate segment
flag values are chosen from the follow definitions. The mask defined below is then
used to update the existing p_flags field in the program headers with the updated
values.
"""
# Mask for bits 20-27 to parse program header p_flags
MI_PBT_FLAGS_MASK = 0x0FF00000

# Helper defines to help parse ELF program headers
MI_PBT_FLAG_SEGMENT_TYPE_MASK         = 0x07000000
MI_PBT_FLAG_SEGMENT_TYPE_SHIFT        = 0x18
MI_PBT_FLAG_PAGE_MODE_MASK            = 0x00100000
MI_PBT_FLAG_PAGE_MODE_SHIFT           = 0x14
MI_PBT_FLAG_ACCESS_TYPE_MASK          = 0x00E00000
MI_PBT_FLAG_ACCESS_TYPE_SHIFT         = 0x15
MI_PBT_FLAG_POOL_INDEX_MASK           = 0x08000000
MI_PBT_FLAG_POOL_INDEX_SHIFT          = 0x1B

# Segment Type
MI_PBT_L4_SEGMENT                     = 0x0
MI_PBT_AMSS_SEGMENT                   = 0x1
MI_PBT_HASH_SEGMENT                   = 0x2
MI_PBT_BOOT_SEGMENT                   = 0x3
MI_PBT_L4BSP_SEGMENT                  = 0x4
MI_PBT_SWAPPED_SEGMENT                = 0x5
MI_PBT_XBL_SEC_SEGMENT                = 0x5
MI_PBT_SWAP_POOL_SEGMENT              = 0x6
MI_PBT_PHDR_SEGMENT                   = 0x7

# Page/Non-Page Type
MI_PBT_NON_PAGED_SEGMENT              = 0x0
MI_PBT_PAGED_SEGMENT                  = 0x1

# Access Type
MI_PBT_RW_SEGMENT                     = 0x0
MI_PBT_RO_SEGMENT                     = 0x1
MI_PBT_ZI_SEGMENT                     = 0x2
MI_PBT_NOTUSED_SEGMENT                = 0x3
MI_PBT_SHARED_SEGMENT                 = 0x4
MI_PBT_RWE_SEGMENT                    = 0x7

# ELF Segment Flag Definitions
MI_PBT_ELF_AMSS_NON_PAGED_RO_SEGMENT  = 0x01200000
MI_PBT_ELF_AMSS_PAGED_RO_SEGMENT  = 0x01300000
MI_PBT_ELF_SWAP_POOL_NON_PAGED_ZI_SEGMENT_INDEX0 = 0x06400000
MI_PBT_ELF_SWAPPED_PAGED_RO_SEGMENT_INDEX0 = 0x05300000
MI_PBT_ELF_SWAP_POOL_NON_PAGED_ZI_SEGMENT_INDEX1 = 0x0E400000
MI_PBT_ELF_SWAPPED_PAGED_RO_SEGMENT_INDEX1 = 0x0D300000
MI_PBT_ELF_AMSS_NON_PAGED_ZI_SEGMENT = 0x01400000
MI_PBT_ELF_AMSS_PAGED_ZI_SEGMENT = 0x01500000
MI_PBT_ELF_AMSS_NON_PAGED_RW_SEGMENT = 0x01000000
MI_PBT_ELF_AMSS_PAGED_RW_SEGMENT = 0x01100000
MI_PBT_ELF_AMSS_NON_PAGED_NOTUSED_SEGMENT = 0x01600000
MI_PBT_ELF_AMSS_PAGED_NOTUSED_SEGMENT = 0x01700000
MI_PBT_ELF_AMSS_NON_PAGED_SHARED_SEGMENT = 0x01800000
MI_PBT_ELF_AMSS_PAGED_SHARED_SEGMENT = 0x01900000
MI_PBT_ELF_HASH_SEGMENT = 0x02200000
MI_PBT_ELF_BOOT_SEGMENT = 0x03200000
MI_PBT_ELF_PHDR_SEGMENT = 0x07000000
MI_PBT_ELF_NON_PAGED_L4BSP_SEGMENT = 0x04000000
MI_PBT_ELF_PAGED_L4BSP_SEGMENT = 0x04100000
MI_PBT_ELF_AMSS_RELOCATABLE_IMAGE = 0x8000000

# New definitions for EOS demap paging requirement
# Bit 20 (0b) Bit 24-26(000): Non Paged = 0x0000_0000
# Bit 20 (1b) Bit 24-26(000): Locked Paged = 0x0010_0000
# Bit 20 (1b) Bit 24-26(001): Unlocked Paged = 0x0110_0000
# Bit 20 (0b) Bit 24-26(011): non secure = 0x0310_0000
MI_PBT_ELF_RESIDENT_SEGMENT = 0x00000000
MI_PBT_ELF_PAGED_LOCKED_SEGMENT = 0x00100000
MI_PBT_ELF_PAGED_UNLOCKED_SEGMENT = 0x01100000
MI_PBT_ELF_UNSECURE_SEGMENT = 0x03100000
#----------------------------------------------------------------------------
# GLOBAL VARIABLES END
#----------------------------------------------------------------------------

#----------------------------------------------------------------------------
# CLASS DEFINITIONS BEGIN
#----------------------------------------------------------------------------
#----------------------------------------------------------------------------
# OS Type ID Class
#----------------------------------------------------------------------------
class OSType:
    BMP_BOOT_OS        = 0
    WM_BOOT_OS         = 1
    ANDROID_BOOT_OS    = 2
    CHROME_BOOT_OS     = 3
    SYMBIAN_BOOT_OS    = 4
    LINUX_BOOT_OS      = 5

#----------------------------------------------------------------------------
# Image Type ID Class - These values must be kept consistent with mibib.h
#----------------------------------------------------------------------------
class ImageType:
   NONE_IMG           = 0
   OEM_SBL_IMG        = 1
   AMSS_IMG           = 2
   QCSBL_IMG          = 3
   HASH_IMG           = 4
   APPSBL_IMG         = 5
   APPS_IMG           = 6
   HOSTDL_IMG         = 7
   DSP1_IMG           = 8
   FSBL_IMG           = 9
   DBL_IMG            = 10
   OSBL_IMG           = 11
   DSP2_IMG           = 12
   EHOSTDL_IMG        = 13
   NANDPRG_IMG        = 14
   NORPRG_IMG         = 15
   RAMFS1_IMG         = 16
   RAMFS2_IMG         = 17
   ADSP_Q5_IMG        = 18
   APPS_KERNEL_IMG    = 19
   BACKUP_RAMFS_IMG   = 20
   SBL1_IMG           = 21
   SBL2_IMG           = 22
   RPM_IMG            = 23
   SBL3_IMG           = 24
   TZ_IMG             = 25
   PSI_IMG            = 32

#----------------------------------------------------------------------------
# Global Image Type Table
# Format of the look-up table:
# KEY -  IMAGE_TYPE string as passed into mbn_builder.py
# VALUE - [Specific ImageType ID enum, Template key string, MBN Type]
#----------------------------------------------------------------------------
image_id_table = {
   'appsbl': [ImageType.APPSBL_IMG, 'APPSBL_IMG', 'bin'],
   'dbl': [ImageType.DBL_IMG, 'DBL_IMG', 'bin'],
   'osbl': [ImageType.OSBL_IMG, 'OSBL_IMG', 'bin'],
   'amss': [ImageType.AMSS_IMG, 'AMSS_IMG', 'elf'],
   'amss_mbn': [ImageType.HASH_IMG, 'HASH_IMG', 'elf'],
   'apps': [ImageType.APPS_IMG, 'APPS_IMG', 'bin'],
   'hostdl': [ImageType.HOSTDL_IMG, 'HOSTDL_IMG', 'bin'],
   'ehostdl': [ImageType.EHOSTDL_IMG, 'EHOSTDL_IMG', 'bin'],
   'emmcbld': [ImageType.EHOSTDL_IMG, 'EMMCBLD_IMG', 'bin'],
   'qdsp6fw': [ImageType.DSP1_IMG, 'DSP1_IMG', 'elf'],
   'qdsp6sw': [ImageType.DSP2_IMG, 'DSP2_IMG', 'elf'],
   'qdsp5': [ImageType.ADSP_Q5_IMG, 'ADSP_Q5_IMG', 'bin'],
   'tz': [ImageType.TZ_IMG, 'TZ_IMG', 'elf'],
   'tz_rumi': [ImageType.TZ_IMG, 'TZ_IMG', 'elf'],
   'tz_virtio': [ImageType.TZ_IMG, 'TZ_IMG', 'elf'],
   'tzbsp_no_xpu': [ImageType.TZ_IMG, 'TZ_IMG', 'elf'],
   'tzbsp_with_test': [ImageType.TZ_IMG, 'TZ_IMG', 'elf'],
   'rpm': [ImageType.RPM_IMG, 'RPM_IMG', 'elf'],
   'sbl1': [ImageType.SBL1_IMG, 'SBL1_IMG', 'bin'],
   'sbl2': [ImageType.SBL2_IMG, 'SBL2_IMG', 'bin'],
   'sbl3': [ImageType.SBL3_IMG, 'SBL3_IMG', 'bin'],
   'efs1': [ImageType.RAMFS1_IMG, 'RAMFS1_IMG', 'bin'],
   'efs2': [ImageType.RAMFS2_IMG, 'RAMFS2_IMG', 'bin'],
   'pmic': [ImageType.PSI_IMG, 'PSI_IMG', 'elf'],
   # DO NOT add any additional image information
}

#----------------------------------------------------------------------------
# Header Class Notes:
# In order to properly read and write the header structures as binary data,
# the Python Struct library is used to align and package up the header objects
# All Struct objects are initialized by a special string with the following
# notation. These structure objects are then used to decode binary data in order
# to fill out the appropriate class in Python, or they are used to package up
# the Python class so that we may write the binary data out.
#----------------------------------------------------------------------------
"""
      Format | C Type         | Python Type | Standard Size
      -----------------------------------------------------
    1) 'X's  | char *         | string      | 'X' bytes
    2) H     | unsigned short | integer     | 2 bytes
    3) I     | unsigned int   | integer     | 4 bytes

"""

#----------------------------------------------------------------------------
# ELF Header Class
#----------------------------------------------------------------------------
class Elf_Ehdr_common:
   # Structure object to align and package the ELF Header
   s = struct.Struct('16sHHI')

   def __init__(self, data):
      unpacked_data       = (Elf_Ehdr_common.s).unpack(data)
      self.unpacked_data  = unpacked_data
      self.e_ident        = unpacked_data[0]
      self.e_type         = unpacked_data[1]
      self.e_machine      = unpacked_data[2]
      self.e_version      = unpacked_data[3]

   def printValues(self):
      print("ATTRIBUTE / VALUE")
      for attr, value in self.__dict__.items():
         print(attr, value)



#----------------------------------------------------------------------------
# ELF Header Class
#----------------------------------------------------------------------------
class Elf32_Ehdr:
   # Structure object to align and package the ELF Header
   s = struct.Struct('16sHHIIIIIHHHHHH')

   def __init__(self, data):
      unpacked_data       = (Elf32_Ehdr.s).unpack(data)
      self.unpacked_data  = unpacked_data
      self.e_ident        = unpacked_data[0]
      self.e_type         = unpacked_data[1]
      self.e_machine      = unpacked_data[2]
      self.e_version      = unpacked_data[3]
      self.e_entry        = unpacked_data[4]
      self.e_phoff        = unpacked_data[5]
      self.e_shoff        = unpacked_data[6]
      self.e_flags        = unpacked_data[7]
      self.e_ehsize       = unpacked_data[8]
      self.e_phentsize    = unpacked_data[9]
      self.e_phnum        = unpacked_data[10]
      self.e_shentsize    = unpacked_data[11]
      self.e_shnum        = unpacked_data[12]
      self.e_shstrndx     = unpacked_data[13]

   def printValues(self):
      print("ATTRIBUTE / VALUE")
      for attr, value in self.__dict__.items():
         print(attr, value)

   def getPackedData(self):
      if type(self.e_ident) == str:
        packvalue = bytes(self.e_ident, 'utf-8')
      else:
        packvalue = self.e_ident
      values = [packvalue,
                self.e_type,
                self.e_machine,
                self.e_version,
                self.e_entry,
                self.e_phoff,
                self.e_shoff,
                self.e_flags,
                self.e_ehsize,
                self.e_phentsize,
                self.e_phnum,
                self.e_shentsize,
                self.e_shnum,
                self.e_shstrndx
               ]

      return (Elf32_Ehdr.s).pack(*values)

#----------------------------------------------------------------------------
# ELF Program Header Class
#----------------------------------------------------------------------------
class Elf32_Phdr:

   # Structure object to align and package the ELF Program Header
   s = struct.Struct('I' * 8)

   def __init__(self, data):
      unpacked_data       = (Elf32_Phdr.s).unpack(data)
      self.unpacked_data  = unpacked_data
      self.p_type         = unpacked_data[0]
      self.p_offset       = unpacked_data[1]
      self.p_vaddr        = unpacked_data[2]
      self.p_paddr        = unpacked_data[3]
      self.p_filesz       = unpacked_data[4]
      self.p_memsz        = unpacked_data[5]
      self.p_flags        = unpacked_data[6]
      self.p_align        = unpacked_data[7]

   def printValues(self):
      print("ATTRIBUTE / VALUE")
      for attr, value in self.__dict__.items():
         print(attr, value)

   def getPackedData(self):
      values = [self.p_type,
                self.p_offset,
                self.p_vaddr,
                self.p_paddr,
                self.p_filesz,
                self.p_memsz,
                self.p_flags,
                self.p_align
               ]

      return (Elf32_Phdr.s).pack(*values)

#----------------------------------------------------------------------------
# ELF Header Class
#----------------------------------------------------------------------------
class Elf64_Ehdr:
   # Structure object to align and package the ELF Header
   s = struct.Struct('16sHHIQQQIHHHHHH')

   def __init__(self, data):
      unpacked_data       = (Elf64_Ehdr.s).unpack(data)
      self.unpacked_data  = unpacked_data
      self.e_ident        = unpacked_data[0]
      self.e_type         = unpacked_data[1]
      self.e_machine      = unpacked_data[2]
      self.e_version      = unpacked_data[3]
      self.e_entry        = unpacked_data[4]
      self.e_phoff        = unpacked_data[5]
      self.e_shoff        = unpacked_data[6]
      self.e_flags        = unpacked_data[7]
      self.e_ehsize       = unpacked_data[8]
      self.e_phentsize    = unpacked_data[9]
      self.e_phnum        = unpacked_data[10]
      self.e_shentsize    = unpacked_data[11]
      self.e_shnum        = unpacked_data[12]
      self.e_shstrndx     = unpacked_data[13]

   def printValues(self):
      print("ATTRIBUTE / VALUE")
      for attr, value in self.__dict__.items():
         print(attr, value)

   def getPackedData(self):
      if type(self.e_ident) == str:
        packvalue = bytes(self.e_ident, 'utf-8')
      else:
        packvalue = self.e_ident
      values = [packvalue,
                self.e_type,
                self.e_machine,
                self.e_version,
                self.e_entry,
                self.e_phoff,
                self.e_shoff,
                self.e_flags,
                self.e_ehsize,
                self.e_phentsize,
                self.e_phnum,
                self.e_shentsize,
                self.e_shnum,
                self.e_shstrndx
               ]

      return (Elf64_Ehdr.s).pack(*values)

#----------------------------------------------------------------------------
# ELF Program Header Class
#----------------------------------------------------------------------------
class Elf64_Phdr:

   # Structure object to align and package the ELF Program Header
   s = struct.Struct('IIQQQQQQ')

   def __init__(self, data):
      unpacked_data       = (Elf64_Phdr.s).unpack(data)
      self.unpacked_data  = unpacked_data
      self.p_type         = unpacked_data[0]
      self.p_flags        = unpacked_data[1]
      self.p_offset       = unpacked_data[2]
      self.p_vaddr        = unpacked_data[3]
      self.p_paddr        = unpacked_data[4]
      self.p_filesz       = unpacked_data[5]
      self.p_memsz        = unpacked_data[6]
      self.p_align        = unpacked_data[7]

   def printValues(self):
      print("ATTRIBUTE / VALUE")
      for attr, value in self.__dict__.items():
         print(attr, value)

   def getPackedData(self):
      values = [self.p_type,
                self.p_flags,
                self.p_offset,
                self.p_vaddr,
                self.p_paddr,
                self.p_filesz,
                self.p_memsz,
                self.p_align
               ]

      return (Elf64_Phdr.s).pack(*values)


#----------------------------------------------------------------------------
# ELF Segment Information Class
#----------------------------------------------------------------------------
class SegmentInfo:
   def __init__(self):
      self.flag  = 0
   def printValues(self):
      print('Flag: ' + str(self.flag))

#----------------------------------------------------------------------------
# Regular Boot Header Class
#----------------------------------------------------------------------------
class Boot_Hdr:
   def __init__(self, init_val):
      self.image_id = ImageType.NONE_IMG
      self.flash_parti_ver = 3
      self.image_src = init_val
      self.image_dest_ptr = init_val
      self.image_size = init_val
      self.code_size = init_val
      self.sig_ptr = init_val
      self.sig_size = init_val
      self.cert_chain_ptr = init_val
      self.cert_chain_size = init_val
      self.magic_number1 = init_val
      self.version = init_val
      self.OS_type = init_val
      self.boot_apps_parti_entry = init_val
      self.boot_apps_size_entry = init_val
      self.boot_apps_ram_loc = init_val
      self.reserved_ptr = init_val
      self.reserved_1 = init_val
      self.reserved_2 = init_val
      self.reserved_3 = init_val
      self.reserved = 0
      self.common_metadata_size = init_val

      # Common Metadata members. These are needed for an MBNv7.
      # Common Metadata is placed just after the header, hence, clubbed with
      # this header.
      self.common_metadata_major_version = 0
      self.common_metadata_minor_version = 0
      self.software_id = init_val
      self.secondary_software_id = 0
      self.hash_table_algorithm = 3
      self.measurement_register_target = 0

      # OEM Metadata 2.0 Members added for MBNv7.
      self.major_version = 2
      self.minor_version = 0
      self.anti_rollback_version = 0
      self.mrc_index = 0
      self.soc_hw_ver_0 = 0
      self.soc_hw_ver_1 = 0
      self.soc_hw_ver_2 = 0
      self.soc_hw_ver_3 = 0
      self.soc_hw_ver_4 = 0
      self.soc_hw_ver_5 = 0
      self.soc_hw_ver_6 = 0
      self.soc_hw_ver_7 = 0
      self.soc_hw_ver_8 = 0
      self.soc_hw_ver_9 = 0
      self.soc_hw_ver_10 = 0
      self.soc_hw_ver_11 = 0
      self.soc_feature_id = 0
      self.jtag_id = 0
      self.serial_number_0 = 0
      self.serial_number_1 = 0
      self.serial_number_2 = 0
      self.serial_number_3 = 0
      self.serial_number_4 = 0
      self.serial_number_5 = 0
      self.serial_number_6 = 0
      self.serial_number_7 = 0
      self.oem_id = 0
      self.oem_product_id = 0
      self.soc_lifecycle_state = 0
      self.oem_lifecycle_state = 0
      self.oem_root_certificate_hash_algorithm = 0
      self.oem_root_certificate_hash = b'\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00'
      self.flags = 1398101

   def getLength(self):
      return BOOT_HEADER_LENGTH

   def writePackedData(self, target, write_full_hdr):
      if self.flash_parti_ver == 7:
        values = [self.reserved,
                  self.flash_parti_ver,
                  self.common_metadata_size,
                  self.metadata_size_qti,
                  self.metadata_size,
                  self.code_size,
                  self.image_src, # QTI signature size per MBNv6/v5/v3 structure below.
                  self.image_dest_ptr, # QTI certificate chain size per MBNv6/v5/v3 structure below.
                  self.sig_size,
                  self.cert_chain_size,
                  # Common Metadata members.
                  self.common_metadata_major_version,
                  self.common_metadata_minor_version,
                  self.software_id,
                  self.secondary_software_id,
                  self.hash_table_algorithm,
                  self.measurement_register_target,
                  # OEM Metadata 2.0 members.
                  self.major_version,
                  self.minor_version,
                  self.anti_rollback_version,
                  self.mrc_index,
                  self.soc_hw_ver_0,
                  self.soc_hw_ver_1,
                  self.soc_hw_ver_2,
                  self.soc_hw_ver_3,
                  self.soc_hw_ver_4,
                  self.soc_hw_ver_5,
                  self.soc_hw_ver_6,
                  self.soc_hw_ver_7,
                  self.soc_hw_ver_8,
                  self.soc_hw_ver_9,
                  self.soc_hw_ver_10,
                  self.soc_hw_ver_11,
                  self.soc_feature_id,
                  self.jtag_id,
                  self.serial_number_0,
                  self.serial_number_1,
                  self.serial_number_2,
                  self.serial_number_3,
                  self.serial_number_4,
                  self.serial_number_5,
                  self.serial_number_6,
                  self.serial_number_7,
                  self.oem_id,
                  self.oem_product_id,
                  self.soc_lifecycle_state,
                  self.oem_lifecycle_state,
                  self.oem_root_certificate_hash_algorithm,
                  self.oem_root_certificate_hash,
                  self.flags]
      else:
        values = [self.image_id,
                  self.flash_parti_ver,
                  self.image_src,
                  self.image_dest_ptr,
                  self.image_size,
                  self.code_size ,
                  self.sig_ptr,
                  self.sig_size,
                  self.cert_chain_ptr,
                  self.cert_chain_size,
                  self.magic_number1,
                  self.version,
                  self.OS_type,
                  self.boot_apps_parti_entry,
                  self.boot_apps_size_entry,
                  self.boot_apps_ram_loc,
                  self.reserved_ptr,
                  self.reserved_1,
                  self.reserved_2,
                  self.reserved_3 ]

        if self.flash_parti_ver == 6:
            values.insert(10, self.metadata_size_qti)
            values.insert(11, self.metadata_size)

        if self.image_dest_ptr >= 0x100000000:
            values[3] = 0xFFFFFFFF

        if self.cert_chain_ptr >= 0x100000000:
            values[6] = 0xFFFFFFFF

        if self.sig_ptr >= 0x100000000:
            values[8] = 0xFFFFFFFF

      # Write 10 entries(40B) or 20 entries(80B) of boot header
      if write_full_hdr is False:
         if self.flash_parti_ver == 6:
            s = struct.Struct('I'* 12)
            values = values[:12]
         elif self.flash_parti_ver == 7:
            # MBNv7 Header is 40 bytes. Common Metadata is 24 bytes,
            # OEM Metadata is 224 bytes and of the format: "I*18 + Q*8 + I*5+ 64s + I".
            s = struct.Struct('I'*34 + 'Q'*8 + 'I'*5 + '64sI')
         else:
            s = struct.Struct('I'* 10)
            values = values[:10]
      else:
         s = struct.Struct('I' * self.getLength())

      packed_data = s.pack(*values)

      fp = OPEN(target,'wb')
      fp.write(packed_data)
      fp.close()

      return s.size

#----------------------------------------------------------------------------
# SBL Boot Header Class
#----------------------------------------------------------------------------
class Sbl_Hdr:
   def __init__(self, init_val):
      self.codeword = init_val
      self.magic = init_val
      self.image_id = init_val
      self.reserved_1 = init_val
      self.reserved_2 = init_val
      self.image_src = init_val
      self.image_dest_ptr = init_val
      self.image_size = init_val
      self.code_size = init_val
      self.sig_ptr = init_val
      self.sig_size = init_val
      self.cert_chain_ptr = init_val
      self.cert_chain_size = init_val
      self.oem_root_cert_sel = init_val
      self.oem_num_root_certs = init_val
      self.booting_image_config = init_val
      self.reserved_6 = init_val
      self.reserved_7 = init_val
      self.reserved_8 = init_val
      self.reserved_9 = init_val

   def getLength(self):
      return SBL_HEADER_LENGTH

   def writePackedData(self, target):
      values = [self.codeword,
                self.magic,
                self.image_id,
                self.reserved_1,
                self.reserved_2,
                self.image_src,
                self.image_dest_ptr,
                self.image_size,
                self.code_size,
                self.sig_ptr,
                self.sig_size,
                self.cert_chain_ptr,
                self.cert_chain_size,
                self.oem_root_cert_sel,
                self.oem_num_root_certs,
                self.booting_image_config,
                self.reserved_6,
                self.reserved_7,
                self.reserved_8,
                self.reserved_9 ]

      s = struct.Struct('I' * self.getLength())
      packed_data = s.pack(*values)

      fp = OPEN(target,'wb')
      fp.write(packed_data)
      fp.close()

      return s.size

#----------------------------------------------------------------------------
# CLASS DEFINITIONS END
#----------------------------------------------------------------------------

#------------------------------------------------------------------------------
# Hooks for Scons
#------------------------------------------------------------------------------
def exists(env):
   return env.Detect('mbn_tools')

def generate(env):

   #----------------------------------------------------------------------------
   # Generate Global Dictionary
   #----------------------------------------------------------------------------
   generate_global_dict(env)

   #----------------------------------------------------------------------------
   # Assign Build Configurable Values
   #----------------------------------------------------------------------------
   init_build_vars(env)

   #----------------------------------------------------------------------------
   # Add Methods to Environment
   #----------------------------------------------------------------------------
   env.AddMethod(filter_dictionary, "FilterDictionary")
   env.AddMethod(image_auth, "ImageAuth")
   env.AddMethod(image_header, "ImageHeader")
   env.AddMethod(pboot_gen_elf, "PBootGenElf")
   env.AddMethod(pboot_add_hash, "PBootAddHash")
   env.AddMethod(modify_elf_flags, "ModifyElfFlags")
   env.AddMethod(generate_code_hash, "GenerateCodeHash")
   env.AddMethod(insert_SBL1_magicCookie, "InsertSBLMagicCookie")
   env.AddMethod(modify_relocatable_flags, "ModifyRelocatableFlags")

   #----------------------------------------------------------------------------
   # Load Encryption Tools and Methods if required
   #----------------------------------------------------------------------------
   if 'USES_ENCRYPT_MBN' in env:
      # Add Encryption Tools to environment
      env.Tool('pil_encrypt', toolpath = ['${BUILD_ROOT}/core/securemsm/ssd/tools/pil_encrypt'])
      env.AddMethod(get_ssd_se_fname, "GetSSDSideEffectFileName")
      env.AddMethod(encrypt_elf_segments, "EncryptElfSegments")
      env.AddMethod(generate_meta_data, "GenerateMetaData")
      env.AddMethod(encrypt_mbn, "EncryptMBN")
   return None

#----------------------------------------------------------------------------
# BOOT TOOLS BEGIN
#----------------------------------------------------------------------------

#----------------------------------------------------------------------------
# generate_meta_data
#----------------------------------------------------------------------------
def generate_meta_data(env, meta_out_file_name, add_magic_num = False):

   '''
   Make call to SSD API to return buffer filled with XML header information.
   The XML header which we write contains information regarding the algorithms
   being used along with specific key values which are to be used for encrpytion.
   '''
   xml_header = env.SSDGetMetaData(add_magic_num)

   # Initialize
   xml_target_file = open(meta_out_file_name,'wb')
   xml_header_size = len(xml_header)

   # Write XML buffer into target file
   xml_target_file.write(xml_header)

   # Pad if necessary to the maximum size
   if xml_header_size <= XML_HEADER_MAXSIZE:
      bytes_to_pad = XML_HEADER_MAXSIZE - xml_header_size
      pad_file(xml_target_file, bytes_to_pad, PAD_BYTE_1)
      xml_target_file.close()
   else:
      xml_target_file.close()
      raise RuntimeError("XML Size too large: " + str(xml_header_size))

#----------------------------------------------------------------------------
# encrypt_mbn
#----------------------------------------------------------------------------
def encrypt_mbn(env, mbn_in_file_name, mbn_out_file_name):
    # Open Files
    mbn_in_fp = OPEN(mbn_in_file_name, "rb")
    mbn_out_fp = OPEN(mbn_out_file_name, "wb+")

    # encrypt the input file content and write to output file
    mbn_file_size = os.path.getsize(mbn_in_file_name)
    file_buff = mbn_in_fp.read(mbn_file_size)
    encrypted_buf = env.SSDEncryptSegment(0, file_buff, mbn_file_size)
    mbn_out_fp.write(encrypted_buf)

    # Close Files
    mbn_in_fp.close()
    mbn_out_fp.close()

    # Clean up encryption files
    env.SSDDeInit()

#----------------------------------------------------------------------------
# get_ssd_se_fname
#----------------------------------------------------------------------------
def get_ssd_se_fname(env):
   return env.SSDGetSideEffectFileName()

#----------------------------------------------------------------------------
# encrypt_elf_segments
#----------------------------------------------------------------------------
def encrypt_elf_segments(env, elf_in_file_name,
                              elf_out_file_name):

   # Open Files
   elf_in_fp = OPEN(elf_in_file_name, "rb")
   elf_out_fp = OPEN(elf_out_file_name, "wb+")

   # Initialize
   [elf_header, phdr_table] = preprocess_elf_file(elf_in_file_name)
   encrypted_seg_counter = 0

   # Copy input file to output file
   shutil.copyfileobj(elf_in_fp, elf_out_fp, os.path.getsize(elf_in_file_name))

   # Begin ELF segment encryption
   for i in range(elf_header.e_phnum):
      curr_phdr = phdr_table[i]

      # Only encrypt segments of LOAD_TYPE. Do not encrypt the hash segment.
      if curr_phdr.p_type == LOAD_TYPE and \
         MI_PBT_SEGMENT_TYPE_VALUE(curr_phdr.p_flags) != MI_PBT_HASH_SEGMENT:

         # Read full segment into buffer
         elf_in_fp.seek(curr_phdr.p_offset)
         data_len = curr_phdr.p_filesz
         file_buff = elf_in_fp.read(data_len)

         # Call encryption routine on buffer
         encrypted_buf = env.SSDEncryptSegment(encrypted_seg_counter, file_buff, data_len)
         encrypted_seg_counter += 1

         # Write encrypted segment into output file in same location
         elf_out_fp.seek(curr_phdr.p_offset)
         elf_out_fp.write(encrypted_buf)

   # Close Files
   elf_in_fp.close()
   elf_out_fp.close()

   # Clean up encryption files
   env.SSDDeInit()

#----------------------------------------------------------------------------
# Converts integer to bytes. If length after conversion
# is smaller than given length of byte string, returned value is right-filled
# with 0x00 bytes. Use Little-endian byte order.
#----------------------------------------------------------------------------
def convert_int_to_byte_string(n, l):
    return b''.join([chr((n >> ((l - i - 1) * 8)) % 256) for i in xrange(l)][::-1])

#----------------------------------------------------------------------------
# Create default elf header
#----------------------------------------------------------------------------
def create_elf_header( output_file_name,
                       image_dest,
                       image_size,
                       is_elf_64_bit = False):

   if (output_file_name is None):
      raise RuntimeError("Requires a ELF header file")

   # Create a elf header and program header
   # Write the headers to the output file
   elf_fp = file(output_file_name, "wb")

   if (is_elf_64_bit is True):
      # ELf header
      elf_fp.write(ELFINFO_MAG0)
      elf_fp.write(ELFINFO_MAG1)
      elf_fp.write(ELFINFO_MAG2)
      elf_fp.write(ELFINFO_MAG3)
      elf_fp.write(ELFINFO_CLASS_64)
      elf_fp.write(ELFINFO_DATA2LSB)
      elf_fp.write(ELFINFO_VERSION_CURRENT)
      elf_fp.write(''.rjust(9, chr(ELFINFO_RESERVED)))
      elf_fp.write(ELFINFO_EXEC_ETYPE)
      elf_fp.write(ELFINFO_ARM_MACHINETYPE)
      elf_fp.write(ELFINFO_VERSION_EV_CURRENT)
      elf_fp.write(convert_int_to_byte_string(image_dest, 8))
      elf_fp.write(convert_int_to_byte_string(ELF64_HDR_SIZE, 8))
      elf_fp.write(convert_int_to_byte_string(ELFINFO_SHOFF, 8))
      elf_fp.write(''.rjust(4, chr(ELFINFO_RESERVED)))
      elf_fp.write(convert_int_to_byte_string(ELF64_HDR_SIZE, 2))
      elf_fp.write(convert_int_to_byte_string(ELF64_PHDR_SIZE, 2))
      elf_fp.write(ELFINFO_PHNUM)
      elf_fp.write(''.rjust(6, chr(ELFINFO_RESERVED)))

      # Program Header
      elf_fp.write(convert_int_to_byte_string(LOAD_TYPE, 4))
      elf_fp.write(convert_int_to_byte_string(MI_PBT_RWE_SEGMENT, 4))
      elf_fp.write(convert_int_to_byte_string(ELF64_HDR_SIZE+ELF64_PHDR_SIZE, 8))
      elf_fp.write(convert_int_to_byte_string(image_dest, 8))
      elf_fp.write(convert_int_to_byte_string(image_dest, 8))
      elf_fp.write(convert_int_to_byte_string(image_size, 8))
      elf_fp.write(convert_int_to_byte_string(image_size, 8))
      elf_fp.write(convert_int_to_byte_string(ELF_BLOCK_ALIGN, 8))
   else:
      # ELf header
      elf_fp.write(ELFINFO_MAG0)
      elf_fp.write(ELFINFO_MAG1)
      elf_fp.write(ELFINFO_MAG2)
      elf_fp.write(ELFINFO_MAG3)
      elf_fp.write(ELFINFO_CLASS_32)
      elf_fp.write(ELFINFO_DATA2LSB)
      elf_fp.write(ELFINFO_VERSION_CURRENT)
      elf_fp.write(''.rjust(9, chr(ELFINFO_RESERVED)))
      elf_fp.write(ELFINFO_EXEC_ETYPE)
      elf_fp.write(ELFINFO_ARM_MACHINETYPE)
      elf_fp.write(ELFINFO_VERSION_EV_CURRENT)
      elf_fp.write(convert_int_to_byte_string(image_dest, 4))
      elf_fp.write(convert_int_to_byte_string(ELF32_HDR_SIZE, 4))
      elf_fp.write(convert_int_to_byte_string(ELFINFO_SHOFF, 4))
      elf_fp.write(''.rjust(4, chr(ELFINFO_RESERVED)))
      elf_fp.write(convert_int_to_byte_string(ELF32_HDR_SIZE, 2))
      elf_fp.write(convert_int_to_byte_string(ELF32_PHDR_SIZE, 2))
      elf_fp.write(ELFINFO_PHNUM)
      elf_fp.write(''.rjust(6, chr(ELFINFO_RESERVED)))

      # Program Header
      elf_fp.write(convert_int_to_byte_string(LOAD_TYPE, 4))
      elf_fp.write(convert_int_to_byte_string(ELF32_HDR_SIZE+ELF32_PHDR_SIZE, 4))
      elf_fp.write(convert_int_to_byte_string(image_dest, 4))
      elf_fp.write(convert_int_to_byte_string(image_dest, 4))
      elf_fp.write(convert_int_to_byte_string(image_size, 4))
      elf_fp.write(convert_int_to_byte_string(image_size, 4))
      elf_fp.write(convert_int_to_byte_string(MI_PBT_RWE_SEGMENT, 4))
      elf_fp.write(convert_int_to_byte_string(ELF_BLOCK_ALIGN, 4))

   elf_fp.close()
   return 0

#----------------------------------------------------------------------------
# image_header
#----------------------------------------------------------------------------
def image_header(env, gen_dict,
                      code_file_name,
                      output_file_name,
                      secure_type,
                      header_format = 'reg',
                      requires_preamble = False,
                      preamble_file_name = None,
                      elf_file_name = None,
                      write_full_hdr = False,
                      in_code_size = None,
                      cert_chain_size_in = CERT_CHAIN_ONEROOT_MAXSIZE,
                      num_of_pages = None,
                      header_version = None):

   # Preliminary checks
   if (requires_preamble is True) and (preamble_file_name is None):
      raise RuntimeError("Image Header requires a preamble file")

   if (gen_dict['IMAGE_KEY_MBN_TYPE'] == 'elf') and (elf_file_name is None):
      raise RuntimeError("ELF Image Headers require an elf file")

   if (in_code_size is None) and (os.path.exists(code_file_name) is False):
      raise RuntimeError("Code size unavailable, and input file does not exist")

   # Initialize
   if in_code_size is not None:
      code_size = in_code_size
   else:
      code_size = os.path.getsize(code_file_name)

   image_dest = 0
   image_source = 0

   # If secure build, set signature and cert chain sizes
   if secure_type == 'secure':
      signature_size = SHA256_SIGNATURE_SIZE
      cert_chain_size = cert_chain_size_in
      image_size = code_size + cert_chain_size + signature_size
      if (image_size % 4) != 0:
         image_size += (4 - (image_size % 4))
   else:
      signature_size = 0
      cert_chain_size = 0
      image_size = code_size

   if header_version:
      assert header_version in [3, 5, 6, 7], 'Not a valid MBN header version'

   # For ELF or hashed images, image destination will be determined from an ELF input file
   if gen_dict['IMAGE_KEY_MBN_TYPE'] == 'elf':
      image_dest = get_hash_address(elf_file_name) + (header_size(header_version))
   elif gen_dict['IMAGE_KEY_MBN_TYPE'] == 'bin':
      image_dest = gen_dict['IMAGE_KEY_IMAGE_DEST']
      image_source = gen_dict['IMAGE_KEY_IMAGE_SOURCE']

   # Build the header based on format specified
   if header_format == 'sbl':
      boot_sbl_header = Sbl_Hdr(init_val = int('0xFFFFFFFF',16))
      boot_sbl_header.codeword = FLASH_CODE_WORD
      boot_sbl_header.magic = MAGIC_NUM
      boot_sbl_header.image_id = gen_dict['IMAGE_KEY_IMAGE_ID']
      boot_sbl_header.image_src = MI_BOOT_SBL_HDR_SIZE
      boot_sbl_header.image_dest_ptr = image_dest
      boot_sbl_header.image_size = image_size
      boot_sbl_header.code_size = code_size
      boot_sbl_header.sig_ptr = image_dest + code_size
      boot_sbl_header.sig_size = signature_size
      boot_sbl_header.cert_chain_ptr = image_dest + code_size + signature_size
      boot_sbl_header.cert_chain_size = cert_chain_size
      boot_sbl_header.oem_root_cert_sel = gen_dict['IMAGE_KEY_OEM_ROOT_CERT_SEL']
      boot_sbl_header.oem_num_root_certs = gen_dict['IMAGE_KEY_OEM_NUM_ROOT_CERTS']
      if 'USES_SBL_FOR_AARCH64' in env:
         boot_sbl_header.booting_image_config = SBL_AARCH64
      elif 'USES_SBL_FOR_AARCH632' in env:
         boot_sbl_header.booting_image_config = SBL_AARCH32

      # If preamble is required, output the preamble file and update the boot_sbl_header
      if requires_preamble is True:
         boot_sbl_header = image_preamble(gen_dict, preamble_file_name, boot_sbl_header, num_of_pages)

      # Package up the header and write to output file
      boot_sbl_header.writePackedData(target = output_file_name)

   elif header_format == 'reg':
      boot_header = Boot_Hdr(init_val = int('0x0',16))
      boot_header.image_id = gen_dict['IMAGE_KEY_IMAGE_ID']
      boot_header.image_src = image_source
      boot_header.image_dest_ptr = image_dest
      boot_header.image_size = image_size
      boot_header.code_size = code_size
      boot_header.sig_ptr = image_dest + code_size
      boot_header.sig_size = signature_size
      boot_header.cert_chain_ptr = image_dest + code_size + signature_size
      boot_header.cert_chain_size = cert_chain_size
      boot_header.flash_parti_ver = header_version  # version

      if header_version >= 5:
         boot_header.image_src = 0                            # sig_size_qc
         boot_header.image_dest_ptr = 0                       # cert_chain_size_qc

      if header_version >= 6:
         boot_header.metadata_size_qti = 0                    # qti_metadata size
         boot_header.metadata_size = 0                        # oem_metadata size

      if header_version == 7:
         boot_header.software_id = 0x7B                       # Hardcoded to BOOTBLOCK SW ID
         boot_header.common_metadata_size = 24
         boot_header.metadata_size = 224


      # If preamble is required, output the preamble file and update the boot_header
      if requires_preamble is True:
         boot_header = image_preamble(gen_dict, preamble_file_name, boot_header, num_of_pages)

      # Package up the header and write to output file
      boot_header.writePackedData(target = output_file_name, write_full_hdr = write_full_hdr)

   else:
      raise RuntimeError("Header format not supported: " + str(header_format))
   return 0


#----------------------------------------------------------------------------
# pboot_gen_elf
#----------------------------------------------------------------------------
def pboot_gen_elf(env, elf_in_file_name,
                       hash_out_file_name,
                       elf_out_file_name,
                       secure_type = 'non_secure',
                       hash_seg_max_size = None,
                       last_phys_addr = None,
                       append_xml_hdr = False,
                       is_sha256_algo = True,
                       cert_chain_size_in = CERT_CHAIN_ONEROOT_MAXSIZE,
                       header_version = None):
   sha_algo = 'SHA1'
   if is_sha256_algo:
       sha_algo = 'SHA256'

   if header_version >= 6:
       sha_algo = 'SHA384'
   image_header_size = header_size(header_version)

   if (sha_algo == 'SHA384'):
      mi_prog_boot_digest_size = 48
   elif sha_algo == 'SHA256':
      mi_prog_boot_digest_size = 32
   else:
      mi_prog_boot_digest_size = 20

   # Open Files
   elf_in_fp = OPEN(elf_in_file_name, "rb")
   hash_out_fp = OPEN(hash_out_file_name, "wb+")

   if elf_out_file_name is not None:
      elf_out_fp = OPEN(elf_out_file_name, "wb+")

   # Initialize
   [elf_header, phdr_table] = preprocess_elf_file(elf_in_file_name)
   num_phdrs = elf_header.e_phnum
   phdr_total_size = num_phdrs * elf_header.e_phentsize
   phdr_size = elf_header.e_phentsize
   hashtable_size = 0
   hashtable_shift = 0

   if elf_header.e_ident[ELFINFO_CLASS_INDEX] == ELFINFO_CLASS_64:
      new_phdr = Elf64_Phdr(b'\0' * ELF64_PHDR_SIZE)
      elf_header_size = ELF64_HDR_SIZE
      is_elf64 = True
   else:
      new_phdr = Elf32_Phdr(b'\0' * ELF32_PHDR_SIZE)
      elf_header_size = ELF32_HDR_SIZE
      is_elf64 = False

   hash = b'\0' * mi_prog_boot_digest_size
   phdr_start = 0
   bytes_to_pad = 0
   hash_seg_end = 0

   # Process program headers if an output elf is specified
   if elf_out_file_name is not None:
      # Assert limit on number of program headers in input ELF
      if num_phdrs > MAX_PHDR_COUNT:
         raise RuntimeError("Input ELF has exceeded maximum number of program headers")

      # Create new program header for the ELF Header + Program Headers
      new_phdr.p_type = NULL_TYPE
      new_phdr.p_flags = MI_PBT_ELF_PHDR_SEGMENT

      # If hash table program header is not found, make sure to include it
      elf_header.e_phnum += 2

      # Create an empty hash entry for PHDR_TYPE
      hash_out_fp.write(b'\0' * mi_prog_boot_digest_size)
      hashtable_size += mi_prog_boot_digest_size

      # Create an empty hash entry for the hash segment itself
      hash_out_fp.write(b'\0' * mi_prog_boot_digest_size)
      hashtable_size += mi_prog_boot_digest_size

   # Begin hash table generation
   for i in range(num_phdrs):
      curr_phdr = phdr_table[i]

      if (MI_PBT_PAGE_MODE_VALUE(curr_phdr.p_flags) == MI_PBT_PAGED_SEGMENT):
         seg_offset = curr_phdr.p_offset
         seg_size = curr_phdr.p_filesz
         hash_size = 0

         # Check if the vaddr is page aligned
         off = curr_phdr.p_vaddr & (ELF_BLOCK_ALIGN - 1)
         if int(off) != 0:
            seg_size -= (ELF_BLOCK_ALIGN - off)
            seg_offset += (ELF_BLOCK_ALIGN - off)

         # Seg_size should be page aligned
         if (seg_size & (ELF_BLOCK_ALIGN - 1)) > 0:
            raise RuntimeError("seg_size: " + hex(seg_size) + " is not ELF page aligned!")

         off = seg_offset + seg_size

         while seg_offset < off:

            if seg_offset < ELF_BLOCK_ALIGN:
               hash_size = seg_offset
            else:
               hash_size = ELF_BLOCK_ALIGN

            elf_in_fp.seek(seg_offset)
            fbuf = elf_in_fp.read(hash_size)

            if MI_PBT_CHECK_FLAG_TYPE(curr_phdr.p_flags) is True:
               hash = generate_hash(fbuf, sha_algo)
            else:
               hash = b'\0' * mi_prog_boot_digest_size

            # Write hash to file
            hash_out_fp.write(hash)

            hashtable_size += mi_prog_boot_digest_size
            seg_offset += ELF_BLOCK_ALIGN

      # Copy the hash entry for all that are PAGED segments and those that are not the PHDR type. This is for
      # backward tool compatibility where some images are generated using older exe tools.
      elif((MI_PBT_PAGE_MODE_VALUE(curr_phdr.p_flags) == MI_PBT_NON_PAGED_SEGMENT) and (curr_phdr.p_type is not PHDR_TYPE)):
         # Read full hash entry into buffer
         elf_in_fp.seek(curr_phdr.p_offset)
         data_len = curr_phdr.p_filesz
         file_buff = elf_in_fp.read(data_len)

         if (MI_PBT_CHECK_FLAG_TYPE(curr_phdr.p_flags) is True) and (data_len > 0):
            hash = generate_hash(file_buff, sha_algo)
         else:
            hash = b'\0' *  mi_prog_boot_digest_size

         # Write hash to file
         hash_out_fp.write(hash)

         hashtable_size += mi_prog_boot_digest_size
   # End hash table generation

   # Generate the rest of the ELF output file if specified
   if elf_out_file_name is not None:

     # Preempt hash table size if necessary
     if secure_type == 'secure':
        hashtable_size += (SHA256_SIGNATURE_SIZE + cert_chain_size_in)

     if append_xml_hdr is True:
        hashtable_size += XML_HEADER_MAXSIZE

     # Initialize the hash table program header
     [hash_Phdr, pad_hash_segment, hash_tbl_end_addr, hash_tbl_offset] = \
            initialize_hash_phdr(elf_in_file_name, hashtable_size, image_header_size, ELF_BLOCK_ALIGN, is_elf64)

     # Check if hash segment max size parameter was passed
     if (hash_seg_max_size is not None):
         # Error checking for hash segment size validity
        if hashtable_size > hash_seg_max_size:
           raise RuntimeError("Hash table exceeds maximum hash segment size: " + hex(hash_seg_max_size))
        if (hash_seg_max_size & (ELF_BLOCK_ALIGN-1)) != 0:
           raise RuntimeError("Hash segment size passed is not ELF Block Aligned: " + hex(hash_seg_max_size))

     # Check if hash physical address parameter was passed
     if last_phys_addr is not None:
        hash_Phdr.p_vaddr = last_phys_addr
        hash_Phdr.p_paddr = last_phys_addr

     # Check if hash segment max size was passed
     if hash_seg_max_size is not None:
        hash_Phdr.p_memsz = hash_seg_max_size

     # Determine the end of the hash segment, make sure it's block aligned
     bytes_to_pad = ELF_BLOCK_ALIGN - pad_hash_segment
     hash_seg_end = hash_tbl_end_addr + bytes_to_pad

     # Check if a shifting is required to accommodate for the hash segment.
     # Get the minimum offset by going through the program headers.
     # Note that the program headers in the input file do not contain
     # the dummy program header for ELF + Program header, and the
     # program header for the hashtable.
     min_offset = phdr_table[0].p_offset
     for i in range(num_phdrs):
        curr_phdr = phdr_table[i]
        if curr_phdr.p_offset < min_offset:
            min_offset = curr_phdr.p_offset

     if min_offset < hash_seg_end:
        hashtable_shift = hash_seg_end - min_offset

     # Move program headers to after ELF header
     phdr_start = elf_header_size

     # We copy over no section headers so assign these values to 0 in ELF Header
     elf_header.e_shnum = 0
     elf_header.e_shstrndx = 0
     elf_header.e_shoff = 0

     # Output remaining ELF segments
     for i in range(num_phdrs):

         # Increment the file offset before writing to the destination file
         curr_phdr = phdr_table[i]

         # We do not copy over program headers of PHDR type, decrement the program
         # header count and continue the loop
         if curr_phdr.p_type is PHDR_TYPE:
            elf_header.e_phnum -= 1
            continue

         src_offset = curr_phdr.p_offset

         # Copy the ELF segment
         file_copy_offset(elf_in_fp, src_offset, elf_out_fp, curr_phdr.p_offset + hashtable_shift, curr_phdr.p_filesz)

     # Output remaining program headers and ELF segments
     elf_header.e_phoff = phdr_start

     # Output new program headers which we have generated
     elf_out_fp.seek(phdr_start)
     new_phdr.p_filesz = elf_header_size + (elf_header.e_phnum * phdr_size)
     elf_out_fp.write(new_phdr.getPackedData())
     elf_out_fp.write(hash_Phdr.getPackedData())
     phdr_start += (2 * phdr_size)

     # Increment the file offset before writing to the destination file
     for i in range(num_phdrs):
         curr_phdr = phdr_table[i]

         if curr_phdr.p_type is PHDR_TYPE:
            continue

         curr_phdr.p_offset += hashtable_shift

         # Copy the program header
         elf_out_fp.seek(phdr_start)
         elf_out_fp.write(curr_phdr.getPackedData())

         # Update phdr_start
         phdr_start += phdr_size

     # Finally, copy the new ELF header to the destination file
     elf_out_fp.seek(0)
     elf_out_fp.write(elf_header.getPackedData())

     # Recalculate hash of ELF + program headers and output to hash output file
     elf_out_fp.seek(0)
     # Read the elf header
     elfhdr_buff = elf_out_fp.read(elf_header_size)
     # Seek to the program header offset listed in elf header.
     elf_out_fp.seek(elf_header.e_phoff)
     # Read the program header and compute hash
     proghdr_buff = elf_out_fp.read(elf_header.e_phnum * phdr_size)

     hash = generate_hash(elfhdr_buff + proghdr_buff, sha_algo)

     # Write hash to file as first hash table entry
     hash_out_fp.seek(0)
     hash_out_fp.write(hash)

   # Close files
   elf_in_fp.close()
   hash_out_fp.close()

   if elf_out_file_name is not None:
      elf_out_fp.close()

   return 0


#----------------------------------------------------------------------------
# pboot_add_hash
#----------------------------------------------------------------------------
def pboot_add_hash(env, elf_in_file_name,
                        hash_tbl_file_name,
                        elf_out_file_name):

   # Open files
   elf_in_fp = OPEN(elf_in_file_name, "rb")
   hash_tbl_fp = OPEN(hash_tbl_file_name, "rb")
   elf_out_fp = OPEN(elf_out_file_name, "wb+")

   # Initialize
   [elf_header, phdr_table] = preprocess_elf_file(elf_in_file_name)

   hash_size = os.path.getsize(hash_tbl_file_name)
   hash_segment_found = False

   # Attempt to find the location of the hash program header
   for i in range(elf_header.e_phnum):
      curr_phdr = phdr_table[i]
      if curr_phdr.p_flags == MI_PBT_ELF_HASH_SEGMENT:
         hash_segment_found = True
         break

   if hash_segment_found is True:
      # Copy input file to output file
      shutil.copyfileobj(elf_in_fp, elf_out_fp, os.path.getsize(elf_in_file_name))

      # Update ELF to insert hash table at corresponding file offset
      hash_hdr_offset = curr_phdr.p_offset
      file_copy_offset(hash_tbl_fp, 0, elf_out_fp, hash_hdr_offset, hash_size)

   else:
      raise RuntimeError("Hash segment program header not found in file " + elf_in_file_name)

   # Close files
   elf_in_fp.close()
   hash_tbl_fp.close()
   elf_out_fp.close()

   return 0

#----------------------------------------------------------------------------
# image_auth
#----------------------------------------------------------------------------
def image_auth(env, *args):

   if len(args) < 7 or len(args) > 8:
      raise RuntimeError("Usage Invalid")

   # Initialize File Names
   binary_in            = args[0]
   signature            = args[1]
   attestation_cert     = args[2]
   attestation_ca_cert  = args[3]
   root_cert            = args[4]
   cert_chain_out       = args[5]
   signed_image_out     = args[6]
   if len(args) == 8:
     cert_size_max_in   = args[7]
   else:
     cert_size_max_in   = CERT_CHAIN_ONEROOT_MAXSIZE

   # Creating list of certificates to support creation of certificate chains
   # of lenth 1, 2, or 3 certificates
   cert_list = []
   num_certs = 0
   if (os.path.exists(attestation_cert)):
      cert_list.append(attestation_cert)
      num_certs = num_certs + 1
   if (os.path.exists(attestation_ca_cert)):
      cert_list.append(attestation_ca_cert)
      num_certs = num_certs + 1
   if (os.path.exists(root_cert)):
      cert_list.append(root_cert)
      num_certs = num_certs + 1

   if (num_certs == 0):
      raise RuntimeError("Missing file(s) required for signing.\n")

   # Create the Certificate Chain
   concat_files (cert_chain_out, cert_list)

   # Pad to ensure Certificate Chain Size is CERT_CHAIN_MAX_SIZE
   cert_size = os.path.getsize(cert_chain_out)

   if cert_size <= cert_size_max_in:
      bytes_to_pad = cert_size_max_in - cert_size
      cert_fp = OPEN(cert_chain_out,'ab')
      pad_file(cert_fp, bytes_to_pad, PAD_BYTE_1)
      cert_fp.close()
   else:
      raise RuntimeError("Certificate Size too large: " + str(cert_size))

   # Create the Final Signed Image File
   concat_files (signed_image_out, [binary_in, signature, cert_chain_out])

   return 0

#----------------------------------------------------------------------------
# modify_relocatable_flags
#----------------------------------------------------------------------------
def modify_relocatable_flags(env, output_elf ):

   # Offset into program header where the p_flags field is stored
   phdr_align_flag_offset = 28
   phdr_reloc_flag_offset = 24

   # Initialize
   [elf_header, phdr_table] = preprocess_elf_file(output_elf)

   if elf_header.e_ident[ELFINFO_CLASS_INDEX] == ELFINFO_CLASS_64:
      curr_phdr = Elf64_Phdr('\0' * ELF64_PHDR_SIZE)
      elf_header_size = ELF64_HDR_SIZE
      is_elf64 = True
   else:
      curr_phdr = Elf32_Phdr('\0' * ELF32_PHDR_SIZE)
      elf_header_size = ELF32_HDR_SIZE
      is_elf64 = False

   # Open files
   elf_in_fp = OPEN(output_elf, "r+")

   # Go to the start of the p_flag entry in the first program header
   file_offset_align_flag = elf_header.e_phoff + phdr_align_flag_offset

   # Change the align field in the program header in the ELF file
   elf_in_fp.seek(file_offset_align_flag)
   curr_phdr = phdr_table[0]

   #default alignment value is 1MB unless otherwise specified
   if 'USES_RELOC_ALIGN_VALUE_4MB' in env:
       alignment_value = ALIGNVALUE_4MB
   else:
       alignment_value = ALIGNVALUE_1MB



   #create new alignment value
   new_align = (curr_phdr.p_align & 0) | alignment_value

   # Create structure to package new flag field
   s = struct.Struct('I')
   new_flag_bytes = s.pack(new_align)

   # Write the new flag value and incr ement offset
   elf_in_fp.write(new_flag_bytes)

   # Go to the start of the p_flag entry in the first program header
   file_offset_reloc_flag = elf_header.e_phoff + phdr_reloc_flag_offset

   # Change each program header flag in the ELF file with relocatable flag
   for i in range(elf_header.e_phnum):
      # Seek to correct location and create new p_flag value
      elf_in_fp.seek(file_offset_reloc_flag)
      curr_phdr = phdr_table[i]
      new_flag = (curr_phdr.p_flags & ~MI_PBT_FLAGS_MASK) | (MI_PBT_ELF_AMSS_RELOCATABLE_IMAGE)

      # Create structure to package new flag field
      s = struct.Struct('I')
      new_flag_bytes = s.pack(new_flag)

      # Write the new flag value and increment offset
      elf_in_fp.write(new_flag_bytes)
      file_offset_reloc_flag += elf_header.e_phentsize

   # Close files
   elf_in_fp.close()


   return 0


#----------------------------------------------------------------------------
# modify_elf_flags
#----------------------------------------------------------------------------
def modify_elf_flags(env, elf_in_file_name,
                          scl_file_name):

   # Initialize
   [elf_header, phdr_table] = preprocess_elf_file(elf_in_file_name)
   segment_list = readSCL(scl_file_name, env['GLOBAL_DICT'])

   if elf_header.e_ident[ELFINFO_CLASS_INDEX] == ELFINFO_CLASS_64:
     curr_phdr = Elf64_Phdr('\0' * ELF64_PHDR_SIZE)
     # Offset into program header where the p_flags field is stored
     phdr_flag_off = 4
   else:
     curr_phdr = Elf32_Phdr('\0' * ELF32_PHDR_SIZE)
     # Offset into program header where the p_flags field is stored
     phdr_flag_off = 24

   # Open files
   elf_in_fp = OPEN(elf_in_file_name, "r+")

   # Check for corresponding number of segments
   if len(segment_list) is not elf_header.e_phnum:
      raise RuntimeError('SCL file and ELF file have different number of segments!')

   # Go to the start of the p_flag entry in the first program header
   file_offset = elf_header.e_phoff + phdr_flag_off

   # Change each program header flag in the ELF file based off the SCL file
   for i in range(elf_header.e_phnum):
      # Seek to correct location and create new p_flag value
      elf_in_fp.seek(file_offset)
      curr_phdr = phdr_table[i]
      new_flag = (curr_phdr.p_flags & ~MI_PBT_FLAGS_MASK) | (segment_list[i].flag)

      # Create structure to package new flag field
      s = struct.Struct('I')
      new_flag_bytes = s.pack(new_flag)

      # Write the new flag value and increment offset
      elf_in_fp.write(new_flag_bytes)
      file_offset += elf_header.e_phentsize

   # Close files
   elf_in_fp.close()

   return 0

#----------------------------------------------------------------------------
# generate_code_hash
#----------------------------------------------------------------------------
def generate_code_hash(env, elf_in_file_name):

   # Initialize
   [elf_header, phdr_table] = preprocess_elf_file(elf_in_file_name)

   if elf_header.e_ident[ELFINFO_CLASS_INDEX] == ELFINFO_CLASS_64:
     curr_phdr = Elf64_Phdr('\0' * ELF64_PHDR_SIZE)
     # Offset into program header where the p_flags field is stored
     phdr_flag_off = 4
   else:
     curr_phdr = Elf32_Phdr('\0' * ELF32_PHDR_SIZE)
     # Offset into program header where the p_flags field is stored
     phdr_flag_off = 24

   # Open files
   elf_in_fp = OPEN(elf_in_file_name, "rb+")

   # Go to the start of the p_flag entry in the first program header
   file_offset = elf_header.e_phoff + phdr_flag_off

   # XXX Get these from env?
   DP_CODE_ALIGN = 0x100
   DP_PAGE_SIZE  = 4096
   DP_HASH_SIZE  = 32  # SHA-256
   DP_HASH_MAGIC = 0xC0DEDEC0
   PH_PERM_RW   = 0x06
   PH_PERM_RX   = 0x05
   PH_PERM_RO   = 0x04
   PH_PERM_MASK = 0x07

   page_size = DP_PAGE_SIZE
   hash_size = DP_HASH_SIZE

   # First identify the hash segment. It is the first RW section.
   # Its Align should be 8, and its size a multiple of DP_HASH_SIZE;

   hash_seg_idx = -1
   for i in range(elf_header.e_phnum):
      curr_phdr = phdr_table[i]

      if (curr_phdr.p_align == 8 and
          (curr_phdr.p_flags & PH_PERM_MASK) == PH_PERM_RW and
          curr_phdr.p_filesz != 0 and (curr_phdr.p_filesz % DP_HASH_SIZE) == 0):
         hash_seg_idx = i
         # Validate the contents of the hash segment. It should be
         # filled with DP_HASH_MAGIC
         elf_in_fp.seek(curr_phdr.p_offset)
         hash_data = "";
         while (len(hash_data) < curr_phdr.p_filesz):
            hash_data = hash_data + elf_in_fp.read(curr_phdr.p_filesz - len(hash_data))

         hash_data = struct.unpack("I" * (curr_phdr.p_filesz / 4), hash_data)

         for v in hash_data[:]:
            if (v != DP_HASH_MAGIC):
               hash_seg_idx = -1
               break;

         if (hash_seg_idx != -1):
            break

   if (hash_seg_idx == -1):
      # return if there is no hash segment.
      return 0

   hash_phdr = phdr_table[hash_seg_idx]

   # Now find the code segment for the hashes. Look for matching number of pages
   code_seg_idx = -1
   code_seg_pages = hash_phdr.p_filesz / DP_HASH_SIZE

   for i in range(elf_header.e_phnum):
      curr_phdr = phdr_table[i]
      curr_pages = (curr_phdr.p_filesz + DP_PAGE_SIZE - 1) / DP_PAGE_SIZE

      if (curr_phdr.p_align == DP_CODE_ALIGN and
          (curr_phdr.p_flags & PH_PERM_MASK) == PH_PERM_RX and
          curr_pages == code_seg_pages):
         if (code_seg_idx != -1):
            raise RuntimeError('Multiple code segments match for: ' + code_seg_pages + ' pages')
         code_seg_idx = i

   if (code_seg_idx == -1):
      raise RuntimeError('No matching code segment found')

   code_phdr = phdr_table[code_seg_idx]

   # Now hash the pages in the code segment
   hashes = []
   elf_in_fp.seek(code_phdr.p_offset)
   bytes_left = code_phdr.p_filesz;
   while (bytes_left > 0):
      bytes_in_page = min(bytes_left, DP_PAGE_SIZE)
      page = "";
      while (len(page) < bytes_in_page):
         page = page + elf_in_fp.read(bytes_in_page - len(page))
      if (len(page) < DP_PAGE_SIZE):
         page = page + (struct.pack('b', 0) * (DP_PAGE_SIZE - len(page)))
      hashes = hashes + [generate_hash(page, 'SHA256')]
      bytes_left -= bytes_in_page

   # And write them to the hash segment
   elf_in_fp.seek(hash_phdr.p_offset)

   for h in hashes[:]:
      elf_in_fp.write(h)

   # Close files
   elf_in_fp.close()

   return 0

#----------------------------------------------------------------------------
# BOOT TOOLS END
#----------------------------------------------------------------------------

#----------------------------------------------------------------------------
# HELPER FUNCTIONS BEGIN
#----------------------------------------------------------------------------

#----------------------------------------------------------------------------
# Create a list to hold all segment information from an input SCL file
#----------------------------------------------------------------------------
def readSCL(filename, global_dict):

  scl_fp = OPEN(filename,'r')

  # Initialize
  file_data = scl_fp.readlines()
  num_lines = len(file_data)
  current_line = ''
  previous_line = ''
  strip_chars = '(){}[]'
  i = 0
  bracket_counter = 0
  seg_list = []

  # Parse through all lines
  while i < num_lines:

     # Save the last line read
     previous_line = current_line
     current_line = file_data[i]

     # Look for the symbol '{' for the line to read.
     # Use bracket counter to skip nested '{ }'
     if ('{' in current_line):
        if bracket_counter == 0:
           # Create a new SegmentInfo class and set up tokens
           new_scl_entry = SegmentInfo()
           previous_line = previous_line.strip()
           tokens = previous_line.split(' ')

           # Check that at least two tokens were parsed
           # Token 1: Segment Name
           # Token 2: Start Address -- not used in MBN tools
           if len(tokens) < 2:
              raise RuntimeError('SCL Segment Syntax malformed: ' + previous_line)

           # Get the segment flags corresponding to the segment name description
           new_scl_entry.flag = getSegmentFlag(tokens[0].strip(strip_chars))
           seg_list.append(new_scl_entry)

        bracket_counter += 1
     elif '}' in current_line:
        bracket_counter -= 1

     i+=1

  scl_fp.close()
  return seg_list

#----------------------------------------------------------------------------
# Given a string parsed from a SCL file, returns the ELF segment flags
#----------------------------------------------------------------------------
def getSegmentFlag(seg_info):

   ret_val = None

   # Define string values for various types of segments
   RO = "RO"
   RW = "RW"
   ZI = "ZI"
   PAGEABLE = "PAGED"
   NOTPAGEABLE = "NOTPAGED"
   SWAPABLE = "SWAPPED"
   SWAP_POOL = "SWAP_POOL"
   RESERVED = "RESERVED"
   HASHTBL = "HASH"
   SHARED = "SHARED"
   NOTUSED = "NOTUSED"
   BOOT_SEGMENT = "BOOT_SEGMENT"
   CODE = "CODE"
   L4BSP = "L4BSP"
   POOL_INDEX_0 = "INDEX_0"
   POOL_INDEX_1 = "INDEX_1"

   # New definitions for EOS demand paging
   NONPAGE = "NONPAGE"
   PAGEUNLOCKED = "PAGEUNLOCKED"
   PAGELOCKED = "PAGELOCKED"
   UNSECURE = "UNSECURE"

   if seg_info is None or len(seg_info) == 0:
      raise RuntimeError('Invalid segment information passed: ' + seg_info)

   # Conditional checks and assignments of the corresponding segment flag values
   if NOTPAGEABLE in seg_info:
      if RO in seg_info:
         ret_val = MI_PBT_ELF_AMSS_NON_PAGED_RO_SEGMENT
      elif CODE in seg_info:
         ret_val = MI_PBT_ELF_AMSS_NON_PAGED_RO_SEGMENT
      elif ZI in seg_info:
         if SWAP_POOL in seg_info:
            if POOL_INDEX_0 in seg_info:
               ret_val = MI_PBT_ELF_SWAP_POOL_NON_PAGED_ZI_SEGMENT_INDEX0
            else:
               ret_val = MI_PBT_ELF_SWAP_POOL_NON_PAGED_ZI_SEGMENT_INDEX1
         else:
            ret_val = MI_PBT_ELF_AMSS_NON_PAGED_ZI_SEGMENT

      elif NOTUSED in seg_info:
         ret_val = MI_PBT_ELF_AMSS_NON_PAGED_NOTUSED_SEGMENT

      elif SHARED in seg_info:
         ret_val = MI_PBT_ELF_AMSS_NON_PAGED_SHARED_SEGMENT
      elif HASHTBL in seg_info:
         ret_val = MI_PBT_ELF_HASH_SEGMENT
      elif BOOT_SEGMENT in seg_info:
         ret_val = MI_PBT_ELF_BOOT_SEGMENT
      elif L4BSP in seg_info:
         ret_val = MI_PBT_ELF_NON_PAGED_L4BSP_SEGMENT
      else:
         ret_val = MI_PBT_ELF_AMSS_NON_PAGED_RW_SEGMENT

   elif PAGEABLE in seg_info:
      if RO in seg_info or CODE in seg_info:
         if SWAPABLE in seg_info:
            if POOL_INDEX_0 in seg_info:
               ret_val = MI_PBT_ELF_SWAPPED_PAGED_RO_SEGMENT_INDEX0
            else:
               ret_val = MI_PBT_ELF_SWAPPED_PAGED_RO_SEGMENT_INDEX1
         else:
            ret_val = MI_PBT_ELF_AMSS_PAGED_RO_SEGMENT
      elif ZI in seg_info:
         ret_val = MI_PBT_ELF_AMSS_PAGED_ZI_SEGMENT

      elif NOTUSED in seg_info:
         ret_val = MI_PBT_ELF_AMSS_PAGED_NOTUSED_SEGMENT
      elif SHARED in seg_info:
         ret_val = MI_PBT_ELF_AMSS_PAGED_SHARED_SEGMENT
      elif L4BSP in seg_info:
         ret_val = MI_PBT_ELF_PAGED_L4BSP_SEGMENT
      else:
         ret_val = MI_PBT_ELF_AMSS_PAGED_RW_SEGMENT

   elif PAGELOCKED in seg_info:
      ret_val = MI_PBT_ELF_PAGED_LOCKED_SEGMENT
   elif PAGEUNLOCKED in seg_info:
      ret_val = MI_PBT_ELF_PAGED_UNLOCKED_SEGMENT
   elif NONPAGE in seg_info:
      ret_val = MI_PBT_ELF_RESIDENT_SEGMENT
   elif UNSECURE in seg_info:
      ret_val = MI_PBT_ELF_UNSECURE_SEGMENT

   else:
      raise RuntimeError('The segment name is wrongly defined in the SCL file: ' + seg_info)

   return ret_val

#----------------------------------------------------------------------------
# Pad a file with specific number of bytes
# Note: Assumes the fp is seeked to the correct location of padding
#----------------------------------------------------------------------------
def pad_file(fp, num_bytes, value):

   if num_bytes < 0:
      raise RuntimeError("Number of bytes to pad must be greater than zero")

   while num_bytes > 0:
      fp.write('%c' % value)
      num_bytes -= 1

   return

#----------------------------------------------------------------------------
# Concatenates the files listed in 'sources' in order and writes to 'target'
#----------------------------------------------------------------------------
def concat_files (target, sources):
   if type(sources) is not list:
      sources = [sources]

   target_file = OPEN(target,'wb')

   for fname in sources:
      file = OPEN(fname,'rb')
      while True:
         bin_data = file.read(65536)
         if not bin_data:
            break
         target_file.write(bin_data)
      file.close()
   target_file.close()

#----------------------------------------------------------------------------
# Parse build configurable values and assign to global variables for tools
#----------------------------------------------------------------------------
def init_build_vars(env):

   # Maximum size of Certificate Chain used in Secure Boot
   global CERT_CHAIN_ONEROOT_MAXSIZE
   CERT_CHAIN_ONEROOT_MAXSIZE = get_dict_value(env['GLOBAL_DICT'], 'CERT_CHAIN_MAXSIZE', (6*1024))

   # Maximum size of the XML Header used in encrypted ELF images
   global XML_HEADER_MAXSIZE
   XML_HEADER_MAXSIZE = get_dict_value(env['GLOBAL_DICT'], 'XML_HEADER_MAXSIZE', (2*1024))

#----------------------------------------------------------------------------
# Generates the global dictionary and add to the environment
#----------------------------------------------------------------------------
def generate_global_dict(env):

   # Get file names for 'cust' and 'targ' auto-generated files inside 'build/ms'
   cust_h = env.subst('CUST${BUILD_ID}.H').lower()
   targ_h = env.subst('TARG${BUILD_ID}.H').lower()
   cust_file_name = str(env.FindFile(cust_h, "${INC_ROOT}/build/ms"))
   targ_file_name = str(env.FindFile(targ_h, "${INC_ROOT}/build/ms"))

   # Check that files are present
   if (os.path.exists(cust_file_name) is True) and \
      (os.path.exists(targ_file_name) is True):

      # Populate the dictionary from the auto-generated files
      global_dict = populate_dictionary(targ_file_name, cust_file_name)
   else:
      global_dict = {}

   # Add the dictionary to the environment
   env.Replace(GLOBAL_DICT = global_dict)

#----------------------------------------------------------------------------
# Populate the dictionary from a list of input files
#----------------------------------------------------------------------------
def populate_dictionary(*args):

   if len(args) < 1:
      raise RuntimeError("At least 1 file must be specified as an input")

   global_dict = {}
   Fields = ["Define", "Key", "Value"]

   # For each input file
   for i in range(len(args)):

      template_file_path = args[i]
      instream = OPEN(template_file_path, 'r')
      # Tokenize each line with a white space
      values = csv.DictReader(instream, Fields, delimiter=" ")

      for values in itertools.izip(values):
         new_entry = values[0]
         # Verify the parsed tokens
         if (new_entry['Define'] == '#define') and \
            (new_entry['Key'] != None) and \
            (new_entry['Value'] != None):

            new_key   = new_entry['Key'].strip()
            new_value = new_entry['Value'].strip()

            # If value pair is empty string, assume feature definition is true
            if new_value == '':
               new_value = 'yes'

            # Check for and handle text replacements as we parse
            if global_dict is not None and len(global_dict.keys()) > 0:
               for key in global_dict:
                  new_value = new_value.replace(key, str(global_dict.get(key)))

            # Attempt to evaluate value
            try:
               new_value = eval(new_value)
            # Catch exceptions and do not evaluate
            except:
               pass

            # Add to global dictionary
            global_dict[new_key] = new_value
      instream.close()

   return global_dict

#----------------------------------------------------------------------------
# Filter out a generic dictionary from the global dictionary
#----------------------------------------------------------------------------
def filter_dictionary(env, global_dict, **kwargs):

   # Check for Image Type
   # If IMAGE_TYPE parameter is not provided, raise error
   if not kwargs.has_key('IMAGE_TYPE'):
      raise RuntimeError("IMAGE_TYPE must be defined to use FilterDictionary.")
   else:
      image_type = kwargs.get('IMAGE_TYPE')
      if type(image_type) is not str:
         raise RuntimeError("IMAGE_TYPE must be of string type.")

   # Check for Flash Type
   # If FLASH_TYPE parameter is not provided, default to 'nand'
   if not kwargs.has_key('FLASH_TYPE'):
      flash_type = 'nand'
   else:
      flash_type = kwargs.get('FLASH_TYPE')
      if type(flash_type) is not str:
         raise RuntimeError("FLASH_TYPE must be of string type. ")

   # Check for MBN Type
   # If MBN_TYPE parameter is not provided, default to 'elf'
   if not kwargs.has_key('MBN_TYPE'):
      mbn_type = 'elf'
   else:
      mbn_type = kwargs.get('MBN_TYPE')
      if mbn_type != 'elf' and mbn_type != 'bin':
         raise RuntimeError("MBN_TYPE currently not supported: " + mbn_type)

   # Check for Image ID
   # If IMAGE_ID parameter is not provided, default to ID 0
   if not kwargs.has_key('IMAGE_ID'):
      image_id = ImageType.NONE_IMG
   else:
      image_id = kwargs.get('IMAGE_ID')
      if type(image_id) is not int:
         raise RuntimeError("IMAGE_ID must be of integer type.")

   # Initialize
   gen_dict = {}
   image_dest = 0
   image_source = 0

   # Check for image_type
   if image_type not in image_id_table:
      id = image_id
      id_match_str = image_type.upper() + "_IMG"
      id_mbn_type = mbn_type
   else:
      id = image_id_table[image_type][0]
      id_match_str = image_id_table[image_type][1]
      id_mbn_type = image_id_table[image_type][2]

   # Handle MBN Type and assign image destination address
   if id_mbn_type == 'elf':
      pass
   elif id_mbn_type == 'bin':
      template_key_match = 'IMAGE_KEY_' + id_match_str + "_DEST_ADDR"
      if template_key_match in global_dict:
         image_dest = global_dict[template_key_match]
      else:
         raise RuntimeError("Builds file does not have IMAGE_KEY pair for: " + image_type)
   else:
      raise RuntimeError("MBN_TYPE currently not supported: " + mbn_type)

   # Assign generic dictionary key/value pairs
   gen_dict['IMAGE_KEY_IMAGE_ID'] = id
   gen_dict['IMAGE_KEY_IMAGE_DEST'] = image_dest
   gen_dict['IMAGE_KEY_IMAGE_SOURCE'] = image_source
   gen_dict['IMAGE_KEY_FLASH_TYPE'] = flash_type
   gen_dict['IMAGE_KEY_MBN_TYPE'] = id_mbn_type
   gen_dict['IMAGE_KEY_ID_MATCH_STR'] = id_match_str
   gen_dict['IMAGE_KEY_FLASH_AUTO_DETECT_MAX_PAGE'] = \
      get_dict_value(global_dict,'FLASH_AUTO_DETECT_MAX_PAGE', 8192)
   gen_dict['IMAGE_KEY_FLASH_AUTO_DETECT_MIN_PAGE'] = \
      get_dict_value(global_dict,'FLASH_AUTO_DETECT_MIN_PAGE', 2048)
   gen_dict['IMAGE_KEY_MAX_SIZE_OF_VERIFY_BUFFER'] = \
      get_dict_value(global_dict,'MAX_SIZE_OF_VERIFY_BUFFER', 8192)
   gen_dict['IMAGE_KEY_BOOT_SMALL_PREAMBLE'] = \
      get_dict_value(global_dict,'BOOT_SMALL_PREAMBLE', 1)

   # Get OEM root certificate select and number
   oem_root_cert_sel = get_dict_value(global_dict,'OEM_ROOT_CERT_SEL', 1)
   oem_num_root_certs = get_dict_value(global_dict,'OEM_NUM_ROOT_CERTS', 1)

   # Error checking for OEM configurable values
   if oem_root_cert_sel in range(1, MAX_NUM_ROOT_CERTS + 1) and \
      oem_num_root_certs in range(1, MAX_NUM_ROOT_CERTS + 1) and \
      oem_root_cert_sel <= oem_num_root_certs:

      gen_dict['IMAGE_KEY_OEM_ROOT_CERT_SEL'] = oem_root_cert_sel
      gen_dict['IMAGE_KEY_OEM_NUM_ROOT_CERTS'] = oem_num_root_certs

   else:
      raise RuntimeError("Invalid OEM root certificate configuration values")

   # Assign additional dictionary key/values pair as needed by tools.

   return gen_dict


#----------------------------------------------------------------------------
# Get index value from dictionary if exists, otherwise return default
#----------------------------------------------------------------------------
def get_dict_value(dict, key_string, default):

   key = 'IMAGE_KEY_' + key_string

   if key in dict:
      return dict[key]
   else:
      return default

#----------------------------------------------------------------------------
# Preprocess an ELF file and return the ELF Header Object and an
# array of ELF Program Header Objects
#----------------------------------------------------------------------------
def preprocess_elf_file(elf_file_name):

   # Initialize
   elf_fp = OPEN(elf_file_name, 'rb')
   elf_header = Elf_Ehdr_common(elf_fp.read(ELF_HDR_COMMON_SIZE))

   if verify_elf_header(elf_header) is False:
      raise RuntimeError("ELF file failed verification: " + elf_file_name)

   elf_fp.seek(0)

   if elf_header.e_ident[ELFINFO_CLASS_INDEX] == ELFINFO_CLASS_64:
      elf_header = Elf64_Ehdr(elf_fp.read(ELF64_HDR_SIZE))
   else:
      elf_header = Elf32_Ehdr(elf_fp.read(ELF32_HDR_SIZE))

   phdr_table = []

   # Verify ELF header information
   if verify_elf_header(elf_header) is False:
      raise RuntimeError("ELF file failed verification: " + elf_file_name)

   # Get program header size
   phdr_size = elf_header.e_phentsize

   # Find the program header offset
   file_offset = elf_header.e_phoff
   elf_fp.seek(file_offset)

   # Read in the program headers
   for i in range(elf_header.e_phnum):
      if elf_header.e_ident[ELFINFO_CLASS_INDEX] == ELFINFO_CLASS_64:
         phdr_table.append(Elf64_Phdr(elf_fp.read(phdr_size)))
      else:
         phdr_table.append(Elf32_Phdr(elf_fp.read(phdr_size)))

   elf_fp.close()
   return [elf_header, phdr_table]

#----------------------------------------------------------------------------
# Get the hash table address from an input ELF file
#----------------------------------------------------------------------------
def get_hash_address(elf_file_name):

   [elf_header, phdr_table] = preprocess_elf_file(elf_file_name)

   last_paddr = 0
   last_paddr_segment = 0

   # Find the segment with the largest physical address.
   # Hash segment's physical address will be immediately after this segment.
   for i in range(elf_header.e_phnum):
      curr_phdr = phdr_table[i]
      if curr_phdr.p_paddr > last_paddr:
         # Skip the demand paging segment as it would be outside the physical RAM location
         if MI_PBT_SEGMENT_TYPE_VALUE(curr_phdr.p_flags) != MI_PBT_XBL_SEC_SEGMENT:
            last_paddr = curr_phdr.p_paddr;
            last_paddr_segment = i;

   max_phdr = phdr_table[last_paddr_segment]

   ret_val  = (((max_phdr.p_paddr + max_phdr.p_memsz - 1) & \
              ~(ELF_BLOCK_ALIGN-1)) + ELF_BLOCK_ALIGN)

   return ret_val

#----------------------------------------------------------------------------
# Verify ELF header contents from an input ELF file
#----------------------------------------------------------------------------
def verify_elf_header(elf_header):
   if (elf_header.e_ident[ELFINFO_MAG0_INDEX] != ELFINFO_MAG0):
      print("MAG0[{:d}]\n".format((elf_header.e_ident[ELFINFO_MAG0_INDEX])))
      return False
   if (elf_header.e_ident[ELFINFO_MAG1_INDEX] != ELFINFO_MAG1):
      print("MAG1[{:d}]\n".format((elf_header.e_ident[ELFINFO_MAG1_INDEX])))
      return False
   if (elf_header.e_ident[ELFINFO_MAG2_INDEX] != ELFINFO_MAG2):
      print("MAG2[{:d}]\n".format((elf_header.e_ident[ELFINFO_MAG2_INDEX])))
      return False
   if (elf_header.e_ident[ELFINFO_MAG3_INDEX] != ELFINFO_MAG3):
      print("MAG3[{:d}]\n".format((elf_header.e_ident[ELFINFO_MAG3_INDEX])))
      return False
   if ((elf_header.e_ident[ELFINFO_CLASS_INDEX] != ELFINFO_CLASS_64) and \
       (elf_header.e_ident[ELFINFO_CLASS_INDEX] != ELFINFO_CLASS_32)):
      print("ELFINFO_CLASS_INDEX[{:d}]\n".format((elf_header.e_ident[ELFINFO_CLASS_INDEX])))
      return False
   if (elf_header.e_ident[ELFINFO_VERSION_INDEX] != ELFINFO_VERSION_CURRENT):
      print("ELFINFO_VERSION_INDEX[{:d}]\n".format((elf_header.e_ident[ELFINFO_VERSION_INDEX])))
      return False
   return True

#----------------------------------------------------------------------------
# Perform file copy given offsets and the number of bytes to copy
#----------------------------------------------------------------------------
def file_copy_offset(in_fp, in_off, out_fp, out_off, num_bytes):
   in_fp.seek(in_off)
   read_in = in_fp.read(num_bytes)
   out_fp.seek(out_off)
   out_fp.write(read_in)

   return num_bytes

#----------------------------------------------------------------------------
# Return header_size based on version
#----------------------------------------------------------------------------
def header_size(header_version):
    if header_version == 6:
        return 48
    elif header_version == 7:
        return 288
    else:
        return 40

#----------------------------------------------------------------------------
# sha1/sha256 hash routine wrapper
#----------------------------------------------------------------------------
def generate_hash(in_buf, sha_algo):
   # Initialize a SHA1 object from the Python hash library
   if sha_algo == 'SHA384':
      m = hashlib.sha384()
   elif sha_algo == 'SHA256':
      m = hashlib.sha256()
   else:
      m = hashlib.sha1()

   # Set the input buffer and return the output digest
   m.update(in_buf)
   return m.digest()

#----------------------------------------------------------------------------
# Initialize the hash program header.
#----------------------------------------------------------------------------
def initialize_hash_phdr(elf_in_file_name, hash_tbl_size, hdr_size, hdr_offset, is_elf64):
   # Set hash header offset to page size boundary. Hash table will be
   # located at first segment of elf image.
   hash_hdr_size =  hdr_size
   hash_hdr_offset = hdr_offset
   hash_tbl_offset = hash_hdr_offset + hash_hdr_size
   hash_tbl_end_addr = hash_tbl_offset + hash_tbl_size;
   pad_hash_segment = (hash_tbl_end_addr) & (ELF_BLOCK_ALIGN-1)

   # Update the hash table program header
   if is_elf64 is True:
      hash_Phdr = Elf64_Phdr(b'\0'*ELF64_PHDR_SIZE)
   else:
      hash_Phdr = Elf32_Phdr(b'\0'*ELF32_PHDR_SIZE)
   hash_Phdr.p_flags = MI_PBT_ELF_HASH_SEGMENT
   hash_Phdr.p_align = ELF_BLOCK_ALIGN
   hash_Phdr.p_offset = hash_hdr_offset
   hash_Phdr.p_memsz = hash_hdr_size + hash_tbl_size + (ELF_BLOCK_ALIGN - pad_hash_segment)
   hash_Phdr.p_filesz = hash_hdr_size + hash_tbl_size
   hash_Phdr.p_type = NULL_TYPE
   hash_Phdr.p_vaddr = get_hash_address(elf_in_file_name)
   hash_Phdr.p_paddr = hash_Phdr.p_vaddr

   return [hash_Phdr, pad_hash_segment, hash_tbl_end_addr, hash_tbl_offset]

#----------------------------------------------------------------------------
# image_preamble
#----------------------------------------------------------------------------
def image_preamble(gen_dict, preamble_file_name, boot_sbl_header, num_of_pages=None):
   # Generate the preamble file
   preamble_fp = OPEN(preamble_file_name, 'wb')

   # Initialize
   max_size_verify = gen_dict['IMAGE_KEY_MAX_SIZE_OF_VERIFY_BUFFER']
   flash_max_page = gen_dict['IMAGE_KEY_FLASH_AUTO_DETECT_MAX_PAGE']
   flash_min_page = gen_dict['IMAGE_KEY_FLASH_AUTO_DETECT_MIN_PAGE']
   autodetectpage = [int('0xFFFFFFFF',16)] * max_size_verify

   # The first three entries in the preamble must include the following values
   autodetectpage[0] = FLASH_CODE_WORD
   autodetectpage[1] = MAGIC_NUM
   if (num_of_pages == 64):
     autodetectpage[2] = AUTODETECT_PAGE_SIZE_MAGIC_NUM64
   elif (num_of_pages == 128):
     autodetectpage[2] = AUTODETECT_PAGE_SIZE_MAGIC_NUM128
   else:
     autodetectpage[2] = AUTODETECT_PAGE_SIZE_MAGIC_NUM

   # Package the list into binary data to be written to the preamble
   s = struct.Struct('I' * max_size_verify)
   packed_data = s.pack(*autodetectpage)

   # Output preamble pages based on maximum/minimum page size support
   for i in range(flash_max_page/flash_min_page):
      preamble_fp.write(packed_data[:flash_min_page])

   # Determine appropriate amount of padding for the preamble and
   # update the boot_sbl_header accordingly
   if gen_dict['IMAGE_KEY_BOOT_SMALL_PREAMBLE'] == 1:
      boot_sbl_header.image_src += (flash_max_page + flash_min_page)
      amount_to_write = flash_min_page
   else:
      boot_sbl_header.image_src += flash_max_page * 2
      amount_to_write = flash_max_page

   pad_file(preamble_fp, amount_to_write, PAD_BYTE_1)
   preamble_fp.close()

   return boot_sbl_header

#----------------------------------------------------------------------------
# Helper functions to parse ELF program headers
#----------------------------------------------------------------------------
def MI_PBT_SEGMENT_TYPE_VALUE(x):
    return ( ((x) & MI_PBT_FLAG_SEGMENT_TYPE_MASK) >> MI_PBT_FLAG_SEGMENT_TYPE_SHIFT )

def MI_PBT_PAGE_MODE_VALUE(x):
    return ( ((x) & MI_PBT_FLAG_PAGE_MODE_MASK) >> MI_PBT_FLAG_PAGE_MODE_SHIFT )

def MI_PBT_ACCESS_TYPE_VALUE(x):
    return ( ((x) & MI_PBT_FLAG_ACCESS_TYPE_MASK) >> MI_PBT_FLAG_ACCESS_TYPE_SHIFT )

def MI_PBT_CHECK_FLAG_TYPE(x):
    return (MI_PBT_SEGMENT_TYPE_VALUE(x) != MI_PBT_HASH_SEGMENT) and \
           (MI_PBT_ACCESS_TYPE_VALUE(x) != MI_PBT_NOTUSED_SEGMENT) and \
           (MI_PBT_ACCESS_TYPE_VALUE(x) != MI_PBT_SHARED_SEGMENT)


#----------------------------------------------------------------------------
# Helper functions to open a file and return a valid file object
#----------------------------------------------------------------------------
def OPEN(file_name, mode):
    try:
       fp = open(file_name, mode)
    except IOError:
       raise RuntimeError("The file could not be opened: " + file_name)

    # File open has succeeded with the given mode, return the file object
    return fp

#----------------------------------------------------------------------------
# Helper functions to insert MCs in SBL1(Badger) if ENABLE_VIRTUAL_BLK is ON
#----------------------------------------------------------------------------
def insert_SBL1_magicCookie (env, target):
    file = open(target, "rb")
    #read the file contents
    filedata = file.read()
    length = len(filedata)
    file.close()

    if (length <= VIRTUAL_BLOCK_SIZE):
        return None
    else:
        #remove the previous file
        os.remove(target)
        #generate new file for appending target data + required MCs
        file = open(target, "ab")

        while length > VIRTUAL_BLOCK_SIZE:
            filedata_till_128kb = filedata[0:VIRTUAL_BLOCK_SIZE]
            filedata_after_128kb = filedata[VIRTUAL_BLOCK_SIZE:length]

            a = str(hex(FLASH_CODE_WORD))
            mc1 = chr(int(a[8:10],16)) + chr(int(a[6:8],16)) + chr(int(a[4:6],16)) + chr(int(a[2:4],16))

            b = str(hex(MAGIC_NUM))
            mc2 = chr(int(b[8:10],16)) + chr(int(b[6:8],16)) + chr(int(b[4:6],16)) + chr(int(b[2:4],16))

            c = str(hex(SBL_VIRTUAL_BLOCK_MAGIC_NUM))
            mc3 = chr(int(c[8:10],16)) + chr(int(c[6:8],16)) + chr(int(c[4:6],16)) + chr(int(c[2:4],16))

            MC_inserted_data = filedata_till_128kb + mc1 + mc2 + mc3
            file.write(MC_inserted_data)

            filedata = filedata_after_128kb
            length = len(filedata)

        #copy the leftover data (<128KB) in output file
        if length > 0:
            file.write(filedata)

        #close the final output file
        file.close()
    # MC_insertion code end

#----------------------------------------------------------------------------
# Helper functions to remove MCs in SBL1(Badger)
#----------------------------------------------------------------------------
def remove_SBL1_magicCookie (env, target, dest):
    file = open(target, "rb")
    #read the file contents
    filedata = file.read()
    length = len(filedata)
    file.close()

    #generate new file for appending target data + required MCs
    file = open(dest, "ab")

    while length > VIRTUAL_BLOCK_SIZE:
        filedata_till_128kb = filedata[0:VIRTUAL_BLOCK_SIZE]
        # skipped 12 byte of Virtual Block Magic Cookie Header
        filedata_after_128kb = filedata[VIRTUAL_BLOCK_SIZE+MAGIC_COOKIE_LENGTH:length]

        file.write(filedata_till_128kb)

        filedata = filedata_after_128kb
        length = len(filedata)

    #copy the leftover data (<128KB) in output file
    if length > 0:
        file.write(filedata)

    #close the final output file
    file.close()

    # MC_removal code end

#----------------------------------------------------------------------------
# Helper functions to pad SBL1 image
# min_size defaults to 256k
# If page_size or num_of_pages is set to 0, the variable is unset
#----------------------------------------------------------------------------
def pad_SBL1_image (env, target, min_size_with_pad=MIN_IMAGE_SIZE_WITH_PAD, page_size=0, num_of_pages=0):
    file = open(target, "rb")
    #read the file contents
    filedata = file.read()
    length = len(filedata)
    file.close()

    multiple = 1
    alignment = page_size * num_of_pages

    if (length > alignment and alignment > 0):
      import math
      multiple = math.ceil(length/float(alignment))

    final_image_size = max(min_size_with_pad, multiple * alignment)

    if length < final_image_size:
      sbl1_fp = open(target, 'ab')
      pad_file (sbl1_fp, (final_image_size-length), PAD_BYTE_0)
      sbl1_fp.close()

   # SBL1 pad code end
#----------------------------------------------------------------------------
# HELPER FUNCTIONS END
#----------------------------------------------------------------------------
