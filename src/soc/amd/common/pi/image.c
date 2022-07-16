/* SPDX-License-Identifier: GPL-2.0-only */

#include <agesa_headers.h>
#include <amdblocks/image.h>
#include <types.h>

/* Check if the image has the desired module. */
static bool validate_image(void *module_chain, const char module_signature[8])
{
	AMD_MODULE_HEADER *mod_ptr = (AMD_MODULE_HEADER *)module_chain;
	uint64_t signature = *(uint64_t *)module_signature;
	char *checking_str;

	while ((mod_ptr != NULL) &&
	  (MODULE_SIGNATURE == *(uint32_t *)&mod_ptr->ModuleHeaderSignature)) {
		checking_str = (char *)&mod_ptr->ModuleIdentifier;
		if (signature == *(uint64_t *)checking_str)
			return true;
		mod_ptr = (AMD_MODULE_HEADER *)mod_ptr->NextBlock;
	}
	return false;
}

/*
 * Find an image that has the desired module. The image is aligned within
 * a given range.
 */
void *amd_find_image(const void *start_address, const void *end_address,
			uint32_t alignment, const char name[8])
{
	uint8_t *current_ptr = (uint8_t *)start_address;
	uint8_t *start = (uint8_t *)start_address;
	uint8_t *end = (uint8_t *)end_address;
	AMD_IMAGE_HEADER *image_ptr;

	while ((current_ptr >= start) && (current_ptr < end)) {
		if (IMAGE_SIGNATURE == *((uint32_t *)current_ptr)) {
			image_ptr = (AMD_IMAGE_HEADER *)current_ptr;

			/* Check if the image has the desired module */
			if (validate_image((void *)image_ptr->ModuleInfoOffset,
					   name))
				return current_ptr;
		}
		current_ptr += alignment;
	}
	return NULL;
}
