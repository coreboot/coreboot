/*****************************************************************************
 *
 * Copyright (c) 2020, Advanced Micro Devices, Inc.
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
 *******************************************************************************/

#ifndef BL_ERRORCODES_PUBLIC_H
#define BL_ERRORCODES_PUBLIC_H

/* Bootloader Return Codes, Error only (0x00 through 0x9F) */
#define BL_OK					0x00	// General - Success
#define BL_ERR_GENERIC				0x01	// Generic Error Code
#define BL_ERR_MEMORY				0x02	// Generic Memory Error
#define BL_ERR_BUFFER_OVERFLOW			0x03	// Buffer Overflow
#define BL_ERR_INVALID_PARAMETER		0x04	// Invalid Parameter(s)
#define BL_ERR_DATA_ALIGNMENT			0x06	// Data Alignment Error
#define BL_ERR_NULL_PTR				0x07	// Null Pointer Error
#define BL_ERR_INVALID_ADDRESS			0x0A	// Invalid Address
#define BL_ERR_OUT_OF_RESOURCES			0x0B	// Out of Resource Error
#define BL_ERR_DATA_ABORT			0x0D	// Data Abort exception
#define BL_ERR_PREFETCH_ABORT			0x0E	// Prefetch Abort exception
#define BL_ERR_GET_FW_HEADER			0x13	// Failure in retrieving firmware
							// header
#define BL_ERR_KEY_SIZE				0x14	// Key size not supported
#define BL_ERR_ENTRY_NOT_FOUND			0x15	// Entry not found at requested
							// location
#define BL_ERR_UNSUPPORTED_PLATFORM		0x16	// Error when feature is not enabled
							// on a given platform.
#define BL_ERR_FWVALIDATION			0x18	// Generic FW Validation error
#define BL_ERR_CCP_RSA				0x19	// RSA operation fail - bootloader
#define BL_ERR_CCP_PASSTHR			0x1A	// CCP Passthrough operation failed
#define BL_ERR_CCP_AES				0x1B	// AES operation failed
#define BL_ERR_SHA				0x1E	// SHA256/SHA384 operation failed
#define BL_ERR_ZLIB				0x1F	// ZLib Decompression operation fail
#define BL_ERR_DIR_ENTRY_NOT_FOUND		0x22	// PSP directory entry not found
#define BL_ERR_SYSHUBMAP_FAILED			0x3A	// Unable to map a SYSHUB address to
							// AXI space
#define BL_ERR_UAPP_PSP_HEADER_NOT_MATCH	0x7A	// PSP level directory from OEM user-
							// app does not match expected value.
#define BL_ERR_UAPP_BIOS_HEADER_NOT_MATCH	0x7B	// BIOS level directory from OEM
							// user-app not match expected value.
#define BL_ERR_UAPP_PSP_DIR_OFFSET_NOT_SET	0x7C	// PSP Directory offset is not set
							// by OEM user-app.
#define BL_ERR_UAPP_BIOS_DIR_OFFSET_NOT_SET	0x7D	// BIOS Directory offset is not set
							// by OEM user-app.
#define BL_ERR_POSTCODE_MAX_VALUE		0x9F	// The maximum allowable error post

/* Bootloader Return Codes, Success only (0xA0 through 0xFF) */
#define BL_SUCCESS_USERMODE_OEM_APP		0xF7	// Updated only PSPFW Status when OEM
							// PSP BL user app returns success.
#define BL_SUCCESS_PSP_BIOS_DIRECTORY_UPDATE	0xF8	// PSP and BIOS directories are loaded
							// into SRAM from the offset provided
							// by OEM user app.

#define BL_SUCCESS_LAST_CODE			0xFF	// Bootloader sequence finished

#endif /* BL_ERRORCODES_PUBLIC_H */
