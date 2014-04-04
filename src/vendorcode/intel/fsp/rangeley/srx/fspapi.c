/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2007-2008 coresystems GmbH
 * Copyright (C) 2013, Intel Corporation.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */

// __PRE_RAM__ means: use "unsigned" for device, not a struct.

#include <stdint.h>
#include <string.h>
#include <arch/io.h>
#include <arch/romcc_io.h>
#include <device/pci_def.h>
#include <device/pnp_def.h>
#include <cpu/x86/lapic.h>
#include <cpu/x86/stack.h>
#include <pc80/mc146818rtc.h>
#include <console/console.h>
#include <spi.h>
#include <spi_flash.h>
#include <cbmem.h>
#include <ip_checksum.h>

#include "fsptypes.h"
#include "fspfv.h"
#include "fspffs.h"
#include "fsphob.h"
#include "fspvpd.h"
#include "fspapi.h"
#include "fspplatform.h"
#include "fspinfoheader.h"
#include "fspbootmode.h"
#include "northbridge/intel/rangeley/northbridge.h"

void find_fsp_header(void);
void early_init(FSP_INFO_HEADER *fsp_info);
void FspNotifyPhase(uint32_t Phase);
void UpdateFspUpdConfigs (
  UPD_DATA_REGION        *FspUpdRgn
);

//
// Spd Data table for Memory Down
//
#ifdef __PRE_RAM__
#if CONFIG_MEMORY_DOWN_ENABLE
/*
  * MEM_DOWN_DIMM_CONFIG
  *   MemoryDownDimmPopulation - 0: Empty 1: Populated.
  *   MemoryDownDimmSpdData - structure MEM_DOWN_DIMM_SPD_DATA for spd data.
 */
  /* Apacer 2GB PC3-12800 DDR3-1600MHz ECC Unbuffered CL11 240-Pin DIMM Single Rank Memory Module Mfr P/N 78.A1GG3.AF20C  */
static const MEM_DOWN_DIMM_CONFIG OemMemoryDownDimmConfig = {
0x01, // MemoryDownDimmPopulation
{  // MemoryDownDimmSpdData
  0xB,    //Byte   2  Dram Device Type
  0x2,    //Byte   3  Module type (3:0)
  0x3,    //Byte   4  Density and Banks
  0x19,   //Byte   5  Addressing
  0x2,    //Byte   6  Nominal Voltage, VDD
  0x1,    //Byte   7  Module Organization
  0xB,    //Byte   8  Memory Bus Width
  0x1,    //Byte  10  Medium Timebase (MTB) Dividend
  0x8,    //Byte  11  Medium Timebase (MTB) Divisor
  0xA,    //Byte  12  Minimum cycle time (tCKmin)
  0xFC,   //Byte  14  CAS latency supported, low byte
  0,      //Byte  15  CAS latency supported, high byte
  0x69,   //Byte  16  Minimum CAS latency time (tAA)
  0x78,   //Byte  17  Minimum write recovery time (tWR)
  0x69,   //Byte  18  Minimum RAS to CAS time (tRCD)
  0x30,   //Byte  19  Minimum RA to RA time (tRRD)
  0x69,   //Byte  20  Minimum precharge time (tRP)
  0x11,   //Byte  21  Upper nibbles for tRAS (7:4), tRC (3:0)
  0x18,   //Byte  22  Minimum active to precharge (tRAS)
  0x81,   //Byte  23  Minimum active to active/refresh (tRC)
  0,      //Byte  24  Minimum refresh recovery (tRFC), low byte
  0x5,    //Byte  25  Minimum refresh recovery (tRFC), high byte
  0x3C,   //Byte  26  Minimum internal wr to rd cmd (tWTR)
  0x3C,   //Byte  27  Minimum internal rd to pc cmd (tRTP)
  0,      //Byte  28  Upper Nibble for tFAW
  0xF0,   //Byte  29  Minimum Four Activate Window Delay Time (tFAWmin), Least Significant Byte
  0x80,   //Byte  32
  0,      //Byte  33
  0,      //Byte  34
  0,      //Byte  35
  0,      //Byte  41
  0x3,    //Byte  62
  0,      //Byte  63
  0x1,    //Byte 117
  0x7A,   //Byte 118
  0,      //Byte 119
  0x13,   //Byte 120
  0x15,   //Byte 121
  0x2,    //Byte 122
  0x1,    //Byte 123
  0x31,   //Byte 124
  0x52,   //Byte 125
  0,      //Byte 148
  0,      //Byte 149
}
};

  /* Dummy structure  */
static const MEM_DOWN_DIMM_CONFIG DummyMemoryDownDimmConfig = {
0x00, // MemoryDownDimmPopulation
{  // MemoryDownDimmSpdData
  0,    //Byte   2  Dram Device Type
  0,    //Byte   3  Module type (3:0)
  0,    //Byte   4  Density and Banks
  0,   //Byte   5  Addressing
  0,    //Byte   6  Nominal Voltage, VDD
  0,    //Byte   7  Module Organization
  0,    //Byte   8  Memory Bus Width
  0,    //Byte  10  Medium Timebase (MTB) Dividend
  0,    //Byte  11  Medium Timebase (MTB) Divisor
  0,    //Byte  12  Minimum cycle time (tCKmin)
  0,   //Byte  14  CAS latency supported, low byte
  0,      //Byte  15  CAS latency supported, high byte
  0,   //Byte  16  Minimum CAS latency time (tAA)
  0,   //Byte  17  Minimum write recovery time (tWR)
  0,   //Byte  18  Minimum RAS to CAS time (tRCD)
  0,   //Byte  19  Minimum RA to RA time (tRRD)
  0,   //Byte  20  Minimum precharge time (tRP)
  0,   //Byte  21  Upper nibbles for tRAS (7:4), tRC (3:0)
  0,   //Byte  22  Minimum active to precharge (tRAS)
  0,   //Byte  23  Minimum active to active/refresh (tRC)
  0,      //Byte  24  Minimum refresh recovery (tRFC), low byte
  0,    //Byte  25  Minimum refresh recovery (tRFC), high byte
  0,   //Byte  26  Minimum internal wr to rd cmd (tWTR)
  0,   //Byte  27  Minimum internal rd to pc cmd (tRTP)
  0,      //Byte  28  Upper Nibble for tFAW
  0,   //Byte  29  Minimum Four Activate Window Delay Time (tFAWmin), Least Significant Byte
  0,   //Byte  32
  0,      //Byte  33
  0,      //Byte  34
  0,      //Byte  35
  0,      //Byte  41
  0,    //Byte  62
  0,      //Byte  63
  0,    //Byte 117
  0,   //Byte 118
  0,    //Byte 119
  0,   //Byte 120
  0,   //Byte 121
  0,    //Byte 122
  0,    //Byte 123
  0,   //Byte 124
  0,   //Byte 125
  0,      //Byte 148
  0,      //Byte 149
}
};
#endif
#endif

//
// FSP_INFO_HEADER GUID
//
#define FSP_INFO_HEADER_GUID \
  { \
  0x912740BE, 0x2284, 0x4734, {0xB9, 0x71, 0x84, 0xB0, 0x27, 0x35, 0x3F, 0x0C} \
  }

//
// FSP_NON_VOLATILE_STORAGE_HOB_GUID
//
#define FSP_NON_VOLATILE_STORAGE_HOB_GUID \
  { \
  0x721acf02, 0x4d77, 0x4c2a, { 0xb3, 0xdc, 0x27, 0xb, 0x7b, 0xa9, 0xe4, 0xb0} \
  }

static EFI_GUID  gFspInfoHeaderGuid = FSP_INFO_HEADER_GUID;
#ifdef __PRE_RAM__
static EFI_GUID  gEfiMemoryConfigDataGuid = FSP_NON_VOLATILE_STORAGE_HOB_GUID;
#endif

/*
  * Compares two GUIDs.
  * Input:
  *   Guid1       a 128 bit GUID.
  *   Guid2       a 128 bit GUID.
  * Return:
  *   TRUE      Guid1 and Guid2 are identical.
  *   FALSE     Guid1 and Guid2 are not identical.
 */
#define CompareGuid(Guid1, Guid2) (	\
	(Guid1.Data1 == Guid2.Data1) &&	\
	(Guid1.Data2 == Guid2.Data2) &&	\
	(Guid1.Data3 == Guid2.Data3) &&	\
	(Guid1.Data4[0] == Guid2.Data4[0]) &&	\
	(Guid1.Data4[1] == Guid2.Data4[1]) &&	\
	(Guid1.Data4[2] == Guid2.Data4[2]) &&	\
	(Guid1.Data4[3] == Guid2.Data4[3]) &&	\
	(Guid1.Data4[4] == Guid2.Data4[4]) &&	\
	(Guid1.Data4[5] == Guid2.Data4[5]) &&	\
	(Guid1.Data4[6] == Guid2.Data4[6]) &&	\
	(Guid1.Data4[7] == Guid2.Data4[7])	\
  )

/*
  * Compares two GUIDs.
  * Input:
  *   Guid1       A pointer to a 128 bit GUID.
  *   Guid2       A pointer to a 128 bit GUID.
  * Return:
  *   TRUE      Guid1 and Guid2 are identical.
  *   FALSE     Guid1 and Guid2 are not identical.
 */
#define CompareGuidPtr(Guid1, Guid2) (	\
        (((UINT32 *) Guid1)[0] == ((UINT32 *) Guid2)[0]) && \
        (((UINT32 *) Guid1)[1] == ((UINT32 *) Guid2)[1]) && \
        (((UINT32 *) Guid1)[2] == ((UINT32 *) Guid2)[2]) && \
        (((UINT32 *) Guid1)[3] == ((UINT32 *) Guid2)[3]) \
  )

/*
 * Platform configuration pointer/structure here.
 */

#ifdef __PRE_RAM__
static void ContinuationFunc (EFI_STATUS Status, void *HobListPtr)
{
  /* Should add the necessary code here! */

  EFI_HOB_GUID_TYPE            *GuidHob;

  /*
   * DO NOT know why the following code always fail to build.
   *
   * GuidHob = GetNextGuidHob (&gEfiMemoryConfigDataGuid, HobListPtr);
   *
   * The error message as below,
   *
   *   .data is non-zero size in romstage which is not allowed -- global variable?
   *
   * OR,
   *
   *   .bss is non-zero size in romstage which is not allowed -- global variable?
   *
   * It's replaced with the following code.
   */

  /* Get the Memory Configure Data HOB */
  EFI_PEI_HOB_POINTERS      mGuidHob;

  mGuidHob.Raw = (UINT8 *) HobListPtr;
  while ((mGuidHob.Raw = GetNextHob (EFI_HOB_TYPE_GUID_EXTENSION, mGuidHob.Raw)) != NULL) {
    if (CompareGuid(gEfiMemoryConfigDataGuid, mGuidHob.Guid->Name)) {
      break;
    }
    mGuidHob.Raw = GET_NEXT_HOB (mGuidHob);
  }
  GuidHob = (EFI_HOB_GUID_TYPE *) mGuidHob.Raw;

  if (GuidHob == NULL) {

    /* Avoid store Hob to CBMEM if NVRAM Hob not exist */
    printk(BIOS_DEBUG, "FSP NVRAM Hob NOT found.\n");

  } else {

  /* Get Guid Hob size and Guid Hob Data pointer */
  void             *GuidHobData;
  uint32_t         GuidHobSize;
  GuidHobData = (VOID *)(*(UINT8 **)&(mGuidHob.Raw) + sizeof (EFI_HOB_GUID_TYPE));
  GuidHobSize = (uint32_t) ((((EFI_HOB_GUID_TYPE *)(mGuidHob.Raw))->Header.HobLength) - sizeof(EFI_HOB_GUID_TYPE));

  /* DEBUG INFO */
  printk(BIOS_DEBUG, "FSP NVRAM Hob at 0x%x (size = 0x%x).\n", (uint32_t)GuidHobData, (uint32_t)GuidHobSize);

#if CONFIG_EARLY_CBMEM_INIT
  struct mrc_data_container *mrcdata;
  int output_len = ALIGN(GuidHobSize, 16);

  /* Save the MRC S3/Fastboot/ADR restore data to cbmem */
  cbmem_initialize();
  mrcdata = cbmem_add
      (CBMEM_ID_MRCDATA,
      output_len + sizeof(struct mrc_data_container));

  printk(BIOS_DEBUG, "Relocate MRC DATA from %p to %p (%u bytes)\n",
      (void *)GuidHobData, mrcdata, output_len);

  mrcdata->mrc_signature = MRC_DATA_SIGNATURE;
  mrcdata->mrc_data_size = output_len;
  mrcdata->reserved = 0;
  memcpy(mrcdata->mrc_data, (void *)GuidHobData,
      GuidHobSize);

  /* Zero the unused space in aligned buffer. */
  if (output_len > GuidHobSize)
      memset(mrcdata->mrc_data + GuidHobSize, 0,
         output_len - GuidHobSize);

  mrcdata->mrc_checksum = compute_ip_checksum(mrcdata->mrc_data,
      mrcdata->mrc_data_size);
#endif
  }

  /* Rangeley early init */
  rangeley_early_initialization();

  /* Notify FSP for PostPciEnumeration */
  FspNotifyPhase (EnumInitPhaseAfterPciEnumeration);

  /* Return to coreboot boot process. */
  __asm__ __volatile__ (
      "jmp fsp_init_done\n\t"
    );
}

/** C Based Basic Initialization
 *
 * Platform configuration with Temp Stack starts here.
 *
 */
void early_init (FSP_INFO_HEADER *fsp_info)
{
  FSP_FSP_INIT              FspInitApi;
  FSP_INIT_PARAMS           FspInitParams;
  FSP_INIT_RT_BUFFER        FspRtBuffer;
  VPD_DATA_REGION     *FspVpdRgn;
  UPD_DATA_REGION     FspUpdRgn;
  struct mrc_data_container *mrc_cache;

  memset((void*)&FspRtBuffer, 0, sizeof(FSP_INIT_RT_BUFFER));
  FspRtBuffer.Common.StackTop = (uint32_t *) ROMSTAGE_STACK;
  FspRtBuffer.Common.UpdDataRgnPtr = (UPD_DATA_REGION *)&FspUpdRgn;

  /* find_current_mrc_cache */
  if (((mrc_cache = find_current_mrc_cache()) == NULL) || (mrc_cache->mrc_data_size == -1UL)) {
    printk(BIOS_DEBUG, "MRC cache not present.\n");
    FspInitParams.NvsBufferPtr = (void *) NULL;
    FspRtBuffer.Common.BootMode = BOOT_WITH_FULL_CONFIGURATION;
  } else {
    printk(BIOS_DEBUG, "MRC cache present at %x.\n", (uint32_t)mrc_cache);
    FspInitParams.NvsBufferPtr = (void *) (EFI_HOB_GUID_TYPE *) mrc_cache->mrc_data;
    FspRtBuffer.Common.BootMode = BOOT_WITH_FULL_CONFIGURATION;
  }

  /* Get VPD region start */
  FspVpdRgn = (VPD_DATA_REGION *)(fsp_info->ImageBase + fsp_info->CfgRegionOffset);

  /* Verifify the VPD data region is valid */
  ASSERT((FspVpdRgn->PcdImageRevision == VPD_IMAGE_REV) && (FspVpdRgn->PcdVpdRegionSign == VPD_IMAGE_ID));

  /* Copy default data from Flash */
  memcpy ((void*)&FspUpdRgn, (void *)(fsp_info->ImageBase + FspVpdRgn->PcdUpdRegionOffset), sizeof(UPD_DATA_REGION));

  /* Verifify the UPD data region is valid */
  ASSERT(FspUpdRgn.PcdRegionTerminator == 0x55AA);

  /* Override any UPD setting if required */
  UpdateFspUpdConfigs (&FspUpdRgn);

#if CONFIG_MEMORY_DOWN_ENABLE
 /*
  * Setup platform specific structure
  */

  /* Channel0/Dimm0, DIMM populated */
  FspRtBuffer.Platform.MemDownDimmConfig[0][0] = (MEM_DOWN_DIMM_CONFIG *)&OemMemoryDownDimmConfig;
  /* Channel0/Dimm1, not populated */
  FspRtBuffer.Platform.MemDownDimmConfig[0][1] = (MEM_DOWN_DIMM_CONFIG *)&DummyMemoryDownDimmConfig;
  /* Channel1/Dimm0, DIMM populated */
  FspRtBuffer.Platform.MemDownDimmConfig[1][0] = (MEM_DOWN_DIMM_CONFIG *)&OemMemoryDownDimmConfig;
  /* Channel1/Dimm1, not populated */
  FspRtBuffer.Platform.MemDownDimmConfig[1][1] = (MEM_DOWN_DIMM_CONFIG *)&DummyMemoryDownDimmConfig;
#endif

  FspInitParams.RtBufferPtr  = (FSP_INIT_RT_BUFFER *)&FspRtBuffer;
  FspInitParams.ContinuationFunc = (CONTINUATION_PROC)ContinuationFunc;
  FspInitApi = (FSP_FSP_INIT)(fsp_info->ImageBase + fsp_info->FspInitEntry);
  FspInitApi(&FspInitParams);

  /* Should never return. Control will continue from ContinuationFunc */
  while (1);
}
#endif

/** C Based Basic Initialization
 *
 * Find FSP header
 *
 */
void __attribute__((optimize("O0"))) find_fsp_header ()
{
  volatile register UINT8 *ptr asm ("eax");

#ifdef __PRE_RAM__
  __asm__ __volatile__ (
      ".global find_fsp_entry\n\t"
      "find_fsp_entry:\n\t"
    );
#endif

  //
  // Start at the FSP / FV Header base
  //
  ptr = (UINT8 *)CONFIG_FSP_BIN_BASE;

  //
  // Validate FV signature _FVH
  //
  if (((EFI_FIRMWARE_VOLUME_HEADER *)ptr)-> Signature != EFI_FVH_SIGNATURE) {
    ptr = 0;
    goto NotFound;
  }

  //
  // Add the Ext Header size to the Ext Header base to go to the end of FV
  // header
  //
  ptr += ((EFI_FIRMWARE_VOLUME_HEADER *)ptr)->ExtHeaderOffset;
  ptr += ((EFI_FIRMWARE_VOLUME_EXT_HEADER *)ptr)->ExtHeaderSize;

  //
  // Align the end of FV header address to 8 bytes
  //
  ptr  = (UINT8 *)(((uint32_t)ptr + 7) & 0xFFFFFFF8);

  //
  // Now ptr is pointing to thr FFS Header. Verify if the GUID
  // matches the FSP_INFO_HEADER GUID
  //
  if (!CompareGuid(((EFI_GUID) (((EFI_FFS_FILE_HEADER *)ptr)->Name)), ((EFI_GUID) gFspInfoHeaderGuid))) {
      ptr = 0;
      goto NotFound;
  }

  //
  // Add the FFS Header size to the base to find the Raw section Header
  //
  ptr += sizeof(EFI_FFS_FILE_HEADER);
  if (((EFI_RAW_SECTION *)ptr)->Type != EFI_SECTION_RAW) {
      ptr = 0;
      goto NotFound;
  }

  //
  // Add the Raw Header size to the base to find the FSP INFO Header
  //
  ptr += sizeof(EFI_RAW_SECTION);

NotFound:
  __asm__ __volatile__ ("ret");
}

/** C Based Basic Initialization
 *
 * Notify Phase for different stage.
 *
 */
void FspNotifyPhase (uint32_t Phase)
{
    FSP_NOTFY_PHASE        NotifyPhaseProc;
    NOTIFY_PHASE_PARAMS    NotifyPhaseParams;
    FSP_INFO_HEADER *fsp_info_header;
    EFI_STATUS             Status;

  __asm__ __volatile__ (
        "call  find_fsp_header      \n\t" /* Call FSP Notify API */
        "movl %%eax, %0\n\t"
	: "=m"(fsp_info_header) ::
	);

    /* call FSP PEI to Notify PostPciEnumeration */
    NotifyPhaseProc   = (FSP_NOTFY_PHASE)(fsp_info_header->ImageBase + fsp_info_header->NotifyPhaseEntry);
    NotifyPhaseParams.Phase = Phase;
    Status = NotifyPhaseProc (&NotifyPhaseParams);
    if (Status != 0) {
	printk(BIOS_DEBUG, "FSP API NotifyPhase failed for phase %d!\n", Phase);
    }
}

/*
 * Board Specific routine before handling over to boot loader.
 */
#if !defined(__PRE_RAM__)
void mainboard_cbmem_post_handling(void)
{

  /* Notify FSP for ReadyToBoot */
  FspNotifyPhase (EnumInitPhaseReadyToBoot);
}
#endif

/**
  This function overrides the default configurations in the UPD data region.

  @param   UpdDataRgn   A pointer to the UPD_DATA_REGION data strcture.

  @return  None
**/
VOID
UpdateFspUpdConfigs (
  UPD_DATA_REGION        *FspUpdRgn
)
{
  /* Override any UPD setting if required */

  //
  // Uncomment the line below to disable LAN device
  //
  //FspUpdRgn->PcdEnableLan = 0;

}
