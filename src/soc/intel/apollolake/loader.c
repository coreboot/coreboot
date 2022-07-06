/* SPDX-License-Identifier: GPL-2.0-only */

#include <assert.h>
#include <console/console.h>
#include <cpu/x86/cache.h>
#include <device/pci_ids.h>
#include <device/pci_ops.h>
#include <soc/cpu.h>
#include <soc/iomap.h>
#include <soc/loader.h>
#include <soc/pci_devs.h>
#include <string.h>
#include <symbols.h>

static void flush_cache(uintptr_t start, uintptr_t size)
{
	uintptr_t	end;
	uintptr_t	addr;

	end = start + (ALIGN_UP(size, 4096));
	for (addr = start; addr < end; addr += 64)
		clflush((void *)addr);
}

/*
 * This function is intentionally comment heavy, as the documentation for
 * APL and GLK is almost non-existant.
 */

bool load_ibb(uint32_t ibb_dest, uint32_t ibb_size)
{
	uint32_t	host_to_cse		= (ibb_size << 4) | 0xc0000000;
	uint32_t	cse_to_host;
	uint32_t	state;
	uint8_t		ring_index;

	uint32_t	chunk_num;
	uint32_t	chunk_index		= 0;
	uint32_t	chunk_size;
	uint32_t	number_of_chunks;
	uint32_t	ibb_size_left;

	uint8_t		*dst;
	uint8_t		*src;
	uint32_t	size;

	/* Check if the CSE exists */
	if ((pci_read_config32(PCH_DEV_CSE, 0) !=
		((PCI_DID_INTEL_APL_CSE0 << 16) | PCI_VID_INTEL)) &&
	    (pci_read_config32(PCH_DEV_CSE, 0) !=
		((PCI_DID_INTEL_GLK_CSE0 << 16) | PCI_VID_INTEL)))
		return -1;

	/* Request data from CSE */
	pci_write_config32(PCH_DEV_CSE, HOST2CSE, host_to_cse);

	/* Wait for response */
	do {
		cse_to_host	= pci_read_config32(PCH_DEV_CSE, CSE2HOST);
		chunk_num	= (cse_to_host >> 28) & 3;
	} while (chunk_num == 0);

	/*
	 * As the size of the SRAM isn't divisible by 3, we split
	 * the data into 4 equally sized chunks of 0x8000.
	 */
	if (chunk_num == 3)
		chunk_num = 4;

	/* Retrieve related information */
	number_of_chunks	= chunk_num;
	/* Get the size of the IBB remaining bytes */
	ibb_size_left		= (cse_to_host & 0x0fffc000) >> 4;
	ibb_size		= ibb_size_left;
	chunk_size		= SHARED_SRAM_SIZE / number_of_chunks;

	/* Loading IBBM */
	while (ibb_size_left > 0) {

		/* Check whether there is ring buffer ready to copy */
		state = (cse_to_host ^ host_to_cse) & 0x0f;
		while (state == 0) {
			cse_to_host	= pci_read_config32(PCH_DEV_CSE, CSE2HOST);
			state		= (cse_to_host ^ host_to_cse) & 0x0f;
		}

		/* Calculate ring index */
		ring_index	= (uint8_t)(chunk_index % number_of_chunks);
		if ((state & (1 << ring_index)) != 0) {
			/* Calculate the source and destination address in ring buffer */
			src	= (uint8_t *)(uintptr_t)
					(SHARED_SRAM_BASE + chunk_size * ring_index);
			dst	= (uint8_t *)(uintptr_t)
					(ibb_dest + chunk_size * chunk_index);

			if (ibb_size_left < chunk_size)
				size	= ibb_size_left;
			else
				size	= chunk_size;

			/*
			 * The RBP can handle a maximum of 0x9be2 for each copy.
			 * Whilst the above code should account for this, copying
			 * more will break serial output so we die here so the
			 * issue is known.
			 */
			if (size > CSE_RBP_LIMIT)
				die("CSE RBP capabilities exceeded!\n");

			/* Move data from SRAM into temporary memory */
			if ((uint32_t)src >= SHARED_SRAM_BASE)
				flush_cache((uint32_t)(uint32_t)src, size);

			memcpy(dst, src, size);
			assert(!memcmp(dst, src, size));
			ibb_size_left -= size;

			/* Send ACK to CSE */
			host_to_cse ^= 1 << ring_index;
			pci_write_config32(PCH_DEV_CSE, HOST2CSE, host_to_cse);

			chunk_index++;
		}
	}

	/* Check that there are no remaining chunks */
	do {
		cse_to_host = pci_read_config32(PCH_DEV_CSE, CSE2HOST);
	} while ((cse_to_host & (BIT(10))) == 0);

	if ((cse_to_host & ((BIT(9)))) != 0)
		printk(BIOS_DEBUG, "CSE RBP: IBBM loaded to CAR!\n");

	/* Clear MCA in BANK4 caused by the Clflush */
	clear_mca_bank4();

	return 0;
}
