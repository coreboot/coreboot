/*
 */

#include <stdint.h>

#define PACKED __attribute__((packed))

#define ALIGN(x,a)              __ALIGN_MASK(x,(typeof(x))(a)-1UL)
#define __ALIGN_MASK(x,mask)    (((x)+(mask))&~(mask))

typedef long intn_t;
typedef unsigned long uintn_t;

typedef uint64_t UINT64;
typedef uint32_t UINT32;
typedef uint16_t UINT16;
typedef uint8_t UINT8;

#include <EfiTypes.h>
#include <EfiFirmwareVolumeHeader.h>
#include <PiFirmwareFile.h>
#include <PeiHob.h>

