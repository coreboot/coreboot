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
	void                        *NvsBufferPtr;
	///
	/// Pointer to the temporary stack base address to be
	/// consumed inside FspMemoryInit() API.
	///
	void                        *StackBase;
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

#endif /* _FSP2_0_UPD_H_ */
