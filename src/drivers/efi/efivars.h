/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef _EDK2_OPTION_H_
#define _EDK2_OPTION_H_

#include <types.h>
#include <commonlib/region.h>

#include <vendorcode/intel/edk2/UDK2017/MdePkg/Include/Uefi/UefiBaseType.h>

/**
 * efi_fv_get_option
 * Use the provided EFI variable store inside the region device as variable store.
 * @rdev: the readable region to operate on
 * @guid: the vendor guid to look for
 * @name: the variable name to look for. NULL terminated.
 * @dest: memory buffer to place the result into
 * @size: on input the size of buffer pointed to by dest.
 *        on output the number of bytes written.
 */

enum cb_err efi_fv_get_option(struct region_device *rdev,
			      const EFI_GUID *guid,
			      const char *name,
			      void *dest,
			      uint32_t *size);

/**
 * efi_fv_set_option
 * Use the provided EFI variable store inside the region device as variable store.
 * If the variable exists with the same size and contents, nothing will be written
 * to the region device.
 * @rdev: the read/writable region to operate on
 * @guid: the vendor guid to write
 * @name: the variable name to write. NULL terminated.
 * @data: memory buffer where to read data from
 * @size: the size of buffer pointed to by data
 */
enum cb_err efi_fv_set_option(struct region_device *rdev,
			      const EFI_GUID *guid,
			      const char *name,
			      void *data,
			      uint32_t size);

enum cb_err efi_fv_print_options(struct region_device *rdev);

#endif /* _EDK2_OPTION_H_ */
