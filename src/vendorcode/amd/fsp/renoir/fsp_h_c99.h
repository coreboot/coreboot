/** @file
 *
 * C99 common FSP definitions from
 * Intel Firmware Support Package External Architecture Specification v2.0
 *
 * These definitions come in a format that is usable outside an EFI environment.
 **/
#ifndef FSP_H_C99_H
#define FSP_H_C99_H

#include <stdint.h>

enum {
	FSP_STATUS_RESET_REQUIRED_COLD		= 0x40000001,
	FSP_STATUS_RESET_REQUIRED_WARM		= 0x40000002,
	FSP_STATUS_RESET_REQUIRED_3		= 0x40000003,
	FSP_STATUS_RESET_REQUIRED_4		= 0x40000004,
	FSP_STATUS_RESET_REQUIRED_5		= 0x40000005,
	FSP_STATUS_RESET_REQUIRED_6		= 0x40000006,
	FSP_STATUS_RESET_REQUIRED_7		= 0x40000007,
	FSP_STATUS_RESET_REQUIRED_8		= 0x40000008,
};

typedef enum {
	EnumInitPhaseAfterPciEnumeration	= 0x20,
	EnumInitPhaseReadyToBoot		= 0x40,
	EnumInitPhaseEndOfFirmware		= 0xF0
} FSP_INIT_PHASE;

typedef struct __packed {
	uint64_t	Signature;
	uint8_t		Revision;
	uint8_t		Reserved[23];
} FSP_UPD_HEADER;

_Static_assert(sizeof(FSP_UPD_HEADER) == 32, "FSP_UPD_HEADER not packed");


#if CONFIG(PLATFORM_USES_FSP2_X86_32)
typedef struct __packed {
	uint8_t		Revision;
	uint8_t		Reserved[3];
	/* Note: This ought to be void*, but that won't allow calling this binary on x86_64. */
	uint32_t	NvsBufferPtr;
	/* Note: This ought to be void*, but that won't allow calling this binary on x86_64. */
	uint32_t	StackBase;
	uint32_t	StackSize;
	uint32_t	BootLoaderTolumSize;
	uint32_t	BootMode;
	uint8_t		Reserved1[8];
} FSPM_ARCH_UPD;

_Static_assert(sizeof(FSPM_ARCH_UPD) == 32, "FSPM_ARCH_UPD not packed");
#else
#error You need to implement this struct for x86_64 FSP
#endif

#endif /* FSP_H_C99_H */
