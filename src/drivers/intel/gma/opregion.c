/* SPDX-License-Identifier: GPL-2.0-only */

#include <acpi/acpi.h>
#include <types.h>
#include <string.h>
#include <cbfs.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <device/pci_ops.h>
#include <console/console.h>
#include <cbmem.h>
#include "intel_bios.h"
#include "opregion.h"

__weak
const char *mainboard_vbt_filename(void)
{
	return "vbt.bin";
}

static char vbt_data[CONFIG_VBT_DATA_SIZE_KB * KiB];
static size_t vbt_data_sz;

void *locate_vbt(size_t *vbt_size)
{
	uint32_t vbtsig = 0;

	if (vbt_data_sz != 0) {
		if (vbt_size)
			*vbt_size = vbt_data_sz;
		return (void *)vbt_data;
	}

	const char *filename = mainboard_vbt_filename();

	size_t file_size = cbfs_load(filename, vbt_data, sizeof(vbt_data));

	if (file_size == 0)
		return NULL;

	if (vbt_size)
		*vbt_size = file_size;

	memcpy(&vbtsig, vbt_data, sizeof(vbtsig));
	if (vbtsig != VBT_SIGNATURE) {
		printk(BIOS_ERR, "Missing/invalid signature in VBT data file!\n");
		return NULL;
	}

	printk(BIOS_INFO, "Found a VBT of %zu bytes after decompression\n",
		file_size);
	vbt_data_sz = file_size;

	return (void *)vbt_data;
}

/* Write ASLS PCI register and prepare SWSCI register. */
static void intel_gma_opregion_register(uintptr_t opregion)
{
	struct device *igd;
	u16 reg16;
	u16 sci_reg;

	igd = pcidev_on_root(0x2, 0);
	if (!igd || !igd->enabled)
		return;

	/*
	 * Intel BIOS Specification
	 * Chapter 5.3.7 "Initialize Hardware State"
	 */
	pci_write_config32(igd, ASLS, opregion);

	/*
	 * Atom-based platforms use a combined SMI/SCI register,
	 * whereas non-Atom platforms use a separate SCI register.
	 */
	if (CONFIG(INTEL_GMA_SWSMISCI))
		sci_reg = SWSMISCI;
	else
		sci_reg = SWSCI;

	/*
	 * Intel's Windows driver relies on this:
	 * Intel BIOS Specification
	 * Chapter 5.4 "ASL Software SCI Handler"
	 */
	reg16 = pci_read_config16(igd, sci_reg);
	reg16 &= ~GSSCIE;
	reg16 |= SMISCISEL;
	pci_write_config16(igd, sci_reg, reg16);
}

/* Restore ASLS register on S3 resume and prepare SWSCI. */
static enum cb_err intel_gma_restore_opregion(void)
{
	const igd_opregion_t *const opregion = cbmem_find(CBMEM_ID_IGD_OPREGION);
	if (!opregion) {
		printk(BIOS_ERR, "GMA: Failed to find IGD OpRegion.\n");
		return CB_ERR;
	}
	/* Write ASLS PCI register and prepare SWSCI register. */
	intel_gma_opregion_register((uintptr_t)opregion);
	return CB_SUCCESS;
}

static enum cb_err vbt_validate(struct region_device *rdev)
{
	uint32_t sig;

	if (rdev_readat(rdev, &sig, 0, sizeof(sig)) != sizeof(sig))
		return CB_ERR;

	if (sig != VBT_SIGNATURE)
		return CB_ERR;

	return CB_SUCCESS;
}

static enum cb_err locate_vbt_vbios(const u8 *vbios, struct region_device *rdev)
{
	const optionrom_header_t *oprom;
	const optionrom_pcir_t *pcir;
	struct region_device rd;
	enum cb_err ret;
	u8 opromsize;
	size_t offset;

	// FIXME: caller should supply a region_device instead of vbios pointer
	if (rdev_chain_mem(&rd, vbios, sizeof(*oprom)))
		return CB_ERR;

	if (rdev_readat(&rd, &opromsize, offsetof(optionrom_header_t, size),
	    sizeof(opromsize)) != sizeof(opromsize) || !opromsize)
		return CB_ERR;

	if (rdev_chain_mem(&rd, vbios, opromsize * 512))
		return CB_ERR;

	oprom = rdev_mmap(&rd, 0, sizeof(*oprom));
	if (!oprom)
		return CB_ERR;

	if (!oprom->pcir_offset || !oprom->vbt_offset) {
		rdev_munmap(&rd, (void *)oprom);
		return CB_ERR;
	}

	pcir = rdev_mmap(&rd, oprom->pcir_offset, sizeof(*pcir));
	if (pcir == NULL) {
		rdev_munmap(&rd, (void *)oprom);
		return CB_ERR;
	}

	printk(BIOS_DEBUG, "GMA: %s: %x %x %x %x %x\n", __func__,
		oprom->signature, pcir->vendor, pcir->classcode[0],
		pcir->classcode[1], pcir->classcode[2]);

	/* Make sure we got an Intel VGA option rom */
	if ((oprom->signature != OPROM_SIGNATURE) ||
	    (pcir->vendor != PCI_VID_INTEL) ||
	    (pcir->signature != 0x52494350) ||
	    (pcir->classcode[0] != 0x00) ||
	    (pcir->classcode[1] != 0x00) ||
	    (pcir->classcode[2] != 0x03)) {
		rdev_munmap(&rd, (void *)oprom);
		rdev_munmap(&rd, (void *)pcir);
		return CB_ERR;
	}

	rdev_munmap(&rd, (void *)pcir);

	/* Search for $VBT as some VBIOS are broken... */
	offset = oprom->vbt_offset;
	do {
		ret = rdev_chain(rdev, &rd, offset,
				 (opromsize * 512) - offset);
		offset++;
	} while (ret == CB_SUCCESS && vbt_validate(rdev) != CB_SUCCESS);

	offset--;

	if (ret == CB_SUCCESS && offset != oprom->vbt_offset)
		printk(BIOS_WARNING, "GMA: Buggy VBIOS found\n");
	else if (ret != CB_SUCCESS)
		printk(BIOS_ERR, "GMA: Broken VBIOS found\n");

	rdev_munmap(&rd, (void *)oprom);
	return ret;
}

static enum cb_err locate_vbt_cbfs(struct region_device *rdev)
{
	size_t vbt_data_size;
	void *vbt = locate_vbt(&vbt_data_size);

	if (vbt == NULL)
		return CB_ERR;

	if (rdev_chain_mem(rdev, vbt, vbt_data_size))
		return CB_ERR;

	printk(BIOS_INFO, "GMA: Found VBT in CBFS\n");

	return CB_SUCCESS;
}

static enum cb_err locate_vbt_vbios_cbfs(struct region_device *rdev)
{
	const u8 *oprom =
		(const u8 *)pci_rom_probe(pcidev_on_root(0x2, 0));
	if (oprom == NULL)
		return CB_ERR;

	printk(BIOS_INFO, "GMA: Found VBIOS in CBFS\n");

	return locate_vbt_vbios(oprom, rdev);
}

/*
 * Try to locate VBT in possible locations and return if found.
 * VBT can be possibly in one of 3 regions:
 *  1. Stitched directly into CBFS region as VBT
 *  2. Part of pci8086 option ROM within CBFS
 *  3. part of VBIOS at location 0xC0000.
 */
static enum cb_err find_vbt_location(struct region_device *rdev)
{
	/* Search for vbt.bin in CBFS. */
	if (locate_vbt_cbfs(rdev) == CB_SUCCESS &&
	    vbt_validate(rdev) == CB_SUCCESS) {
		printk(BIOS_INFO, "GMA: Found valid VBT in CBFS\n");
		return CB_SUCCESS;
	}
	/* Search for pci8086,XXXX.rom in CBFS. */
	else if (locate_vbt_vbios_cbfs(rdev) == CB_SUCCESS &&
		 vbt_validate(rdev) == CB_SUCCESS) {
		printk(BIOS_INFO, "GMA: Found valid VBT in VBIOS\n");
		return CB_SUCCESS;
	}
	/*
	 * Try to locate Intel VBIOS at 0xc0000. It might have been placed by
	 * Native Graphics Init as fake Option ROM or when coreboot did run the
	 * VBIOS on legacy platforms.
	 * TODO: Place generated fake VBT in CBMEM and get rid of this.
	 */
	else if (locate_vbt_vbios((u8 *)0xc0000, rdev) == CB_SUCCESS &&
		 vbt_validate(rdev) == CB_SUCCESS) {
		printk(BIOS_INFO, "GMA: Found valid VBT in legacy area\n");
		return CB_SUCCESS;
	}

	printk(BIOS_ERR, "GMA: VBT couldn't be found\n");
	return CB_ERR;
}

/* Function to get the IGD Opregion version */
static struct opregion_version opregion_get_version(void)
{
	if (CONFIG(INTEL_GMA_OPREGION_2_1))
		return (struct opregion_version) { .major = 2, .minor = 1 };

	return (struct opregion_version) { .major = 2, .minor = 0 };
}

/*
 * Function to determine if we need to use extended VBT region to pass
 * VBT pointer. If VBT size > 6 KiB then we need to use extended VBT
 * region.
 */
static inline bool is_ext_vbt_required(igd_opregion_t *opregion, optionrom_vbt_t *vbt)
{
	return (vbt->hdr_vbt_size > sizeof(opregion->vbt.gvd1));
}

/* Function to determine if the VBT uses a relative address */
static inline bool uses_relative_vbt_addr(opregion_header_t *header)
{
	if (header->opver.major > 2)
		return true;

	return header->opver.major >= 2 && header->opver.minor >= 1;
}

/*
 * Copy extended VBT at the end of opregion and fill rvda and rvds
 * values correctly for the opregion.
 */
static void opregion_add_ext_vbt(igd_opregion_t *opregion, uint8_t *ext_vbt,
				optionrom_vbt_t *vbt)
{

	opregion_header_t *header = &opregion->header;
	/* Copy VBT into extended VBT region (at offset 8 KiB) */
	memcpy(ext_vbt, vbt, vbt->hdr_vbt_size);

	/* Fill RVDA value with relative address of the opregion buffer in case of
	IGD Opregion version 2.1+ and physical address otherwise */

	if (uses_relative_vbt_addr(header))
		opregion->mailbox3.rvda = sizeof(*opregion);
	else
		opregion->mailbox3.rvda = (uintptr_t)ext_vbt;

	opregion->mailbox3.rvds = vbt->hdr_vbt_size;
}

/* Initialize IGD OpRegion, called from ACPI code and OS drivers */
enum cb_err intel_gma_init_igd_opregion(void)
{
	igd_opregion_t *opregion;
	struct region_device rdev;
	optionrom_vbt_t *vbt = NULL;
	size_t opregion_size = sizeof(igd_opregion_t);

	if (acpi_is_wakeup_s3())
		return intel_gma_restore_opregion();

	if (find_vbt_location(&rdev) != CB_SUCCESS)
		return CB_ERR;

	vbt = rdev_mmap_full(&rdev);
	if (!vbt) {
		printk(BIOS_ERR, "GMA: Error mapping VBT\n");
		return CB_ERR;
	}

	if (vbt->hdr_vbt_size > region_device_sz(&rdev)) {
		printk(BIOS_ERR, "GMA: Error mapped only a partial VBT\n");
		rdev_munmap(&rdev, vbt);
		return CB_ERR;
	}

	if (is_ext_vbt_required(opregion, vbt))
		opregion_size += vbt->hdr_vbt_size;

	opregion = cbmem_add(CBMEM_ID_IGD_OPREGION, opregion_size);
	if (!opregion) {
		printk(BIOS_ERR, "GMA: Failed to add IGD OpRegion to CBMEM.\n");
		return CB_ERR;
	}

	memset(opregion, 0, opregion_size);

	memcpy(&opregion->header.signature, IGD_OPREGION_SIGNATURE,
		sizeof(opregion->header.signature));
	memcpy(opregion->header.vbios_version, vbt->coreblock_biosbuild,
					ARRAY_SIZE(vbt->coreblock_biosbuild));

	/* Get the opregion version information */
	opregion->header.opver = opregion_get_version();

	/* Extended VBT support */
	if (is_ext_vbt_required(opregion, vbt)) {
		/* Place extended VBT just after opregion */
		uint8_t *ext_vbt = (uint8_t *)opregion + sizeof(*opregion);
		opregion_add_ext_vbt(opregion, ext_vbt, vbt);
	} else {
		/* Raw VBT size which can fit in gvd1 */
		memcpy(opregion->vbt.gvd1, vbt, vbt->hdr_vbt_size);
	}

	rdev_munmap(&rdev, vbt);

	/* 8kb */
	opregion->header.size = sizeof(igd_opregion_t) / 1024;

	// FIXME We just assume we're mobile for now
	opregion->header.mailboxes = MAILBOXES_MOBILE;

	// TODO Initialize Mailbox 1
	opregion->mailbox1.clid = 1;

	// TODO Initialize Mailbox 3
	opregion->mailbox3.bclp = IGD_BACKLIGHT_BRIGHTNESS;
	opregion->mailbox3.pfit = IGD_FIELD_VALID | IGD_PFIT_STRETCH;
	opregion->mailbox3.pcft = 0; // should be (IMON << 1) & 0x3e
	opregion->mailbox3.cblv = IGD_FIELD_VALID | IGD_INITIAL_BRIGHTNESS;
	opregion->mailbox3.bclm[0] = IGD_WORD_FIELD_VALID + 0x0000;
	opregion->mailbox3.bclm[1] = IGD_WORD_FIELD_VALID + 0x0a19;
	opregion->mailbox3.bclm[2] = IGD_WORD_FIELD_VALID + 0x1433;
	opregion->mailbox3.bclm[3] = IGD_WORD_FIELD_VALID + 0x1e4c;
	opregion->mailbox3.bclm[4] = IGD_WORD_FIELD_VALID + 0x2866;
	opregion->mailbox3.bclm[5] = IGD_WORD_FIELD_VALID + 0x327f;
	opregion->mailbox3.bclm[6] = IGD_WORD_FIELD_VALID + 0x3c99;
	opregion->mailbox3.bclm[7] = IGD_WORD_FIELD_VALID + 0x46b2;
	opregion->mailbox3.bclm[8] = IGD_WORD_FIELD_VALID + 0x50cc;
	opregion->mailbox3.bclm[9] = IGD_WORD_FIELD_VALID + 0x5ae5;
	opregion->mailbox3.bclm[10] = IGD_WORD_FIELD_VALID + 0x64ff;

	/* Write ASLS PCI register and prepare SWSCI register. */
	intel_gma_opregion_register((uintptr_t)opregion);

	return CB_SUCCESS;
}
