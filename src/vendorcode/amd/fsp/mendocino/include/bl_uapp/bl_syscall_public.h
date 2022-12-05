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
 ***************************************************************************/

#ifndef _BL_SYSCALL_PUBLIC_H_
#define _BL_SYSCALL_PUBLIC_H_

#include <stdint.h>

#define SVC_EXIT		0x00
#define SVC_ENTER		0x02
#define SVC_VERSTAGE_CMD	0x3A

enum verstage_cmd_id {
	CMD_SHA	= 1,
	CMD_MODEXP,
	CMD_DEBUG_PRINT,
	CMD_DEBUG_PRINT_EX,
	CMD_UPDATE_PSP_BIOS_DIR,
	CMD_GET_SPI_INFO,
	CMD_MAP_SPIROM_DEVICE,
	CMD_UNMAP_SPIROM_DEVICE,
	CMD_READ_TIMER_VAL,
	CMD_DELAY_IN_MICRO_SECONDS,
	CMD_RESET_SYSTEM,
	CMD_GET_BOOT_MODE,
	CMD_COPY_DATA_FROM_UAPP,
	CMD_MAP_FCH_IO_DEVICE,
	CMD_UNMAP_FCH_IO_DEVICE,
	CMD_CCP_DMA,
	CMD_SET_PLATFORM_BOOT_MODE,
	CMD_SET_FW_HASH_TABLE,
	CMD_GET_PREV_BOOT_STATUS,
};

struct mod_exp_params {
	char		*pExponent;	// Exponent address
	unsigned int	ExpSize;	// Exponent size in bytes
	char		*pModulus;	// Modulus address
	unsigned int	ModulusSize;	// Modulus size in bytes
	char		*pMessage;	// Message address, same size as ModulusSize
	char		*pOutput;	// Output address; Must be big enough to hold the
					// data of ModulusSize
};

enum psp_boot_mode {
	PSP_BOOT_MODE_S0 = 0x0,
	PSP_BOOT_MODE_S0i3_RESUME = 0x1,
	PSP_BOOT_MODE_S3_RESUME = 0x2,
	PSP_BOOT_MODE_S4 = 0x3,
	PSP_BOOT_MODE_S5_COLD = 0x4,
	PSP_BOOT_MODE_S5_WARM = 0x5,
};

enum reset_type
{
	RESET_TYPE_COLD    = 0,
	RESET_TYPE_WARM    = 1,
	RESET_TYPE_MAX     = 2,
};

enum fch_io_device {
	FCH_IO_DEVICE_SPI,
	FCH_IO_DEVICE_I2C,
	FCH_IO_DEVICE_GPIO,
	FCH_IO_DEVICE_ESPI,
	FCH_IO_DEVICE_IOMUX,
	FCH_IO_DEVICE_MISC,
	FCH_IO_DEVICE_AOAC,
	FCH_IO_DEVICE_IOPORT,
	FCH_IO_DEVICE_END,
};

enum fch_i2c_controller_id {
	FCH_I2C_CONTROLLER_ID_0 = 0,
	FCH_I2C_CONTROLLER_ID_1 = 1,
	FCH_I2C_CONTROLLER_ID_2 = 2,
	FCH_I2C_CONTROLLER_ID_3 = 3,
	FCH_I2C_CONTROLLER_ID_MAX,
};

struct spirom_info {
	void *SpiBiosSysHubBase;
	void *SpiBiosSmnBase;
	uint32_t SpiBiosSize;
};

enum psp_timer_type {
	PSP_TIMER_TYPE_CHRONO     = 0,
	PSP_TIMER_TYPE_SECURE_RTC = 1,
	PSP_TIMER_TYPE_MAX        = 2,
};

/* SHA types same as ccp SHA type in crypto.h */
enum sha_type {
	SHA_TYPE_256,
	SHA_TYPE_384
};

/* All SHA operation supported */
enum sha_operation_mode {
	SHA_GENERIC
};

/* SHA Supported Data Structures */
struct sha_generic_data {
	enum sha_type	SHAType;
	uint8_t		*Data;
	uint32_t	DataLen;
	uint32_t	DataMemType;
	uint8_t		*Digest;
	uint32_t	DigestLen;
	uint8_t		*IntermediateDigest;
	uint32_t	IntermediateMsgLen;
	uint32_t	Init;
	uint32_t	Eom;
};

/*
 * This is state that PSP manages internally.
 * We only report BOOT_MODE_DEVELOPER or BOOT_MODE_NORMAL in verstage.
 */
enum chrome_platform_boot_mode
{
	NON_CHROME_BOOK_BOOT_MODE                = 0x0,
	CHROME_BOOK_BOOT_MODE_UNSIGNED_VERSTAGE  = 0x1,
	CHROME_BOOK_BOOT_MODE_NORMAL             = 0x2,
	CHROME_BOOK_BOOT_MODE_DEVELOPER          = 0x3,
	CHROME_BOOK_BOOT_MODE_TYPE_MAX_LIMIT     = 0x4, // used for boundary check
};

struct psp_fw_entry_hash_256 {
	uint16_t fw_type;
	uint16_t sub_type;
	uint8_t  sha[32];
} __packed;

struct psp_fw_entry_hash_384 {
	uint16_t fw_type;
	uint16_t sub_type;
	uint8_t  sha[48];
} __packed;

struct psp_fw_hash_table {
	uint16_t version;			// Version of psp_fw_hash_table, Start with 0.
	uint16_t no_of_entries_256;
	uint16_t no_of_entries_384;
	struct psp_fw_entry_hash_256 *fw_hash_256;
	struct psp_fw_entry_hash_384 *fw_hash_384;
} __packed;

/*
 * Exit to the main Boot Loader. This does not return back to user application.
 *
 * Parameters:
 *     status  -   either Ok or error code defined by AGESA
 */
void svc_exit(uint32_t status);

/* Print debug message into serial console.
 *
 * Parameters:
 *     string     -   null-terminated string
 */
void svc_debug_print(const char *string);

/* Print 4 DWORD values in hex to serial console
 *
 * Parameters:
 *     dword0...dword3 - 32-bit DWORD to print
 */
void svc_debug_print_ex(uint32_t dword0,
		uint32_t dword1, uint32_t dword2, uint32_t dword3);

/* Description     - Returns the current boot mode from the enum psp_boot_mode found in
 *                   bl_public.h.
 *
 * Inputs          - boot_mode - Output parameter passed in R0
 *
 * Outputs         - The boot mode in boot_mode.
 *                   See Return Values.
 *
 * Return Values   - BL_OK
 *                   BL_ERR_NULL_PTR
 *                   Other BL_ERRORs lofted up from called functions
 */
uint32_t svc_get_boot_mode(uint32_t *boot_mode);

/* Add delay in micro seconds
 *
 * Parameters:
 *     delay       - required delay value in microseconds
 *
 * Return value: NONE
 */
void svc_delay_in_usec(uint32_t delay);

/* Get the SPI-ROM information
 *
 * Parameters:
 *     spi_rom_iInfo  - SPI-ROM information
 *
 * Return value: BL_OK or error code
 */
uint32_t svc_get_spi_rom_info(struct spirom_info *spi_rom_info);

/* Map the FCH IO device register space (SPI/I2C/GPIO/eSPI/etc...)
 *
 * Parameters:
 *     io_device         - ID for respective FCH IO controller register space to be mapped
 *     arg1              - Based on IODevice ID, interpretation of this argument changes.
 *     arg2              - Based on IODevice ID, interpretation of this argument changes.
 *     io_device_axi_addr    - AXI address for respective FCH IO device register space
 *
 * Return value: BL_OK or error code
 */
uint32_t svc_map_fch_dev(enum fch_io_device io_device,
		uint32_t arg1, uint32_t arg2, void **io_device_axi_addr);

/* Unmap the FCH IO device register space mapped earlier using Svc_MapFchIODevice()
 *
 * Parameters:
 *     io_device        - ID for respective FCH IO controller register space to be unmapped
 *     io_device_addr   - AXI address for respective FCH IO device register space
 *
 * Return value: BL_OK or error code
 */
uint32_t svc_unmap_fch_dev(enum fch_io_device io_device,
		void *io_device_axi_addr);

/* Map the SPIROM FLASH device address space
 *
 * Parameters:
 *     SpiRomAddr     - Address in SPIROM tobe mapped (SMN based)
 *     size           - Size to be mapped
 *     pSpiRomAddrAxi - Mapped address in AXI space
 *
 * Return value: BL_OK or error code
 */
uint32_t svc_map_spi_rom(void *spi_rom_addr,
		uint32_t size, void **spi_rom_axi_addr);

/* Unmap the SPIROM FLASH device address space mapped earlier using Svc_MapSpiRomDevice()
 *
 * Parameters:
 *     pSpiRomAddrAxi - Address in AXI address space previously mapped
 *
 * Return value: BL_OK or error code
 */
uint32_t svc_unmap_spi_rom(void *spi_rom_addr);

/* Updates the offset at which PSP or BIOS Directory can be found in the
 * SPI flash
 *
 * Parameters:
 *     psp_dir_offset - [in/out] Offset at which PSP Directory can be
 *                      found in the SPI Flash. Same pointer is used
 *                      to return the offset in case of GET operation
 *     bios_dir_offset - [in/out] Offset at which BIOS Directory can be
 *                       found in the SPI Flash. Same pointer is used
 *                       to return the offset in case of GET operation
 *
 * Return value: BL_OK or error code
 */
uint32_t svc_update_psp_bios_dir(uint32_t *psp_dir_offset,
		uint32_t *bios_dir_offset);

/* Copies the data that is shared by verstage to the PSP BL owned memory
 *
 * Parameters:
 *     address - Address in UAPP controlled/owned memory
 *     size    - Total size of memory to copy (max 16Kbytes)
 */
uint32_t svc_save_uapp_data(void *address, uint32_t size);

/*
 *    Read timer raw (currently CHRONO and RTC) value
 *
 *    Parameters:
 *                type            - [in] Type of timer UAPP would like to read from
 *                                  (currently CHRONO and RTC)
 *                counter_value   - [out] return the raw counter value read from
 *                                  RTC or CHRONO_LO/HI counter register
 -----------------------------------------------------------------------------*/
uint32_t svc_read_timer_val(enum psp_timer_type type, uint64_t *counter_value);

/*
 *    Reset the system
 *
 *   Parameters:
 *      reset_type -   Cold or Warm reset
 */
uint32_t svc_reset_system(enum reset_type reset_type);

/*
 *    Write postcode to Port-80
 *
 *    Parameters:
 *                postcode -   Postcode value to be written on port-80h
 */
uint32_t svc_write_postcode(uint32_t postcode);

/*
 * Generic SHA call for SHA, SHA_OTP, SHA_HMAC
 */
uint32_t svc_crypto_sha(struct sha_generic_data *sha_op, enum sha_operation_mode sha_mode);

/*
 * Calculate ModEx
 *
 * Parameters:
 *       mod_exp_param - ModExp parameters
 *
 *   Return value: BL_OK or error code
 */
uint32_t svc_modexp(struct mod_exp_params *mod_exp_param);

/*
 * Copies the data from source to destination using ccp
 *
 * Parameters:
 *   Source Address - SPI ROM offset
 *   Destination Address - Address in Verstage memory
 *   Size    - Total size to copy
 *
 * Return value: BL_OK or error code
 */
uint32_t svc_ccp_dma(uint32_t spi_rom_offset, void *dest, uint32_t size);

/*
 * Get the Platform boot mode from verstage. Normal or developer
 *
 * Parameters:
 *       - boot mode
 -----------------------------------------------------------------------------*/
uint32_t svc_set_platform_boot_mode(enum chrome_platform_boot_mode boot_mode);

/*
 * Set the PSP FW hash table.
 *
 * Parameters:
 *       - hash_table - Table of hash for each PSP binary signed against SoC chain of trust
 *
 * Return value: BL_OK or error code
 */
uint32_t svc_set_fw_hash_table(struct psp_fw_hash_table *hash_table);

/* Get the previous boot status.
 *
 * Parameters:
 * 	- boot_status - Address where the boot status is read into
 *
 * Return value: BL_OK or error code
 */
uint32_t svc_get_prev_boot_status(uint32_t *boot_status);

/* C entry point for the Bootloader Userspace Application */
void Main(void);

#endif /* _BL_SYSCALL__PUBLIC_H_ */
