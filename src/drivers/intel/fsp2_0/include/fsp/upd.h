/* SPDX-License-Identifier: GPL-2.0-or-later */

#ifndef _FSP2_0_UPD_H_
#define _FSP2_0_UPD_H_

#include <stdint.h>

struct FSP_UPD_HEADER {
	///
	/// UPD Region Signature. This signature will be
	/// "XXXXXX_T" for FSP-T
	/// "XXXXXX_M" for FSP-M
	/// "XXXXXX_S" for FSP-S
	/// Where XXXXXX is an unique signature
	///
	uint64_t                      Signature;
	///
	/// Revision of the Data structure. For FSP v2.0 value is 1.
	///
	uint8_t                       Revision;
	uint8_t                       Reserved[23];
} __packed;

#if CONFIG(PLATFORM_USES_FSP2_X86_32)
struct FSPM_ARCH_UPD {
	///
	/// Revision of the structure. For FSP v2.0 value is 1.
	///
	uint8_t                       Revision;
	uint8_t                       Reserved[3];
	///
	/// Pointer to the non-volatile storage (NVS) data buffer.
	/// If it is NULL it indicates the NVS data is not available.
	///
	uint32_t                      NvsBufferPtr;
	///
	/// Pointer to the temporary stack base address to be
	/// consumed inside FspMemoryInit() API.
	///
	uint32_t                      StackBase;
	///
	/// Temporary stack size to be consumed inside
	/// FspMemoryInit() API.
	///
	uint32_t                      StackSize;
	///
	/// Size of memory to be reserved by FSP below "top
	/// of low usable memory" for bootloader usage.
	///
	uint32_t                      BootLoaderTolumSize;
	///
	/// Current boot mode.
	///
	uint32_t                      BootMode;
	uint8_t                       Reserved1[8];
} __packed;
#else
#error You need to implement this struct for x86_64 FSP
#endif

#endif
struct FSPS_ARCH_UPD {
	///
	/// Revision of the structure. For FSP v2.2 value is 1.
	///
	uint8_t                       Revision;
	uint8_t                       Reserved[3];
	///
	/// Length of the structure in bytes. The current value for this field is 32
	///
	uint32_t                      Length;
	uint8_t                       Reserved1[4];
	///
	/// To enable multi-phase silicon initialization the bootloader must set non-zero value
	///
	uint8_t                       EnableMultiPhaseSiliconInit;
	uint8_t                       Reserved2[19];
} __packed;

#endif /* _FSP2_0_UPD_H_ */
