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

