/*
 */

#include <stdint.h>

#define PACKED __attribute__((packed))

#define ALIGN(x,a)              __ALIGN_MASK(x,(typeof(x))(a)-1UL)
#define __ALIGN_MASK(x,mask)    (((x)+(mask))&~(mask))

typedef long intn_t;
typedef unsigned long uintn_t;

#include <EfiTypes.h>
#include <EfiFirmwareVolumeHeader.h>
#include <PiFirmwareFile.h>
#include <PeiHob.h>
#include <MemoryAllocationHob.h>

#define EFI_DXE_FILE_GUID \
  { 0xb1644c1a, 0xc16a, 0x4c5b, { 0x88, 0xde, 0xea, 0xfb, 0xa9, 0x7e, 0x74, 0xd8 } }

