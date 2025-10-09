/* amdtool - dump all registers on an AMD CPU + chipset based system */
/* SPDX-License-Identifier: GPL-2.0-only */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <inttypes.h>
#include <assert.h>
#include "acpimmio.h"
#include "amdtool.h"

#define ACPIMMIO_REGION_SIZE 0x100

const uint8_t *acpimmio_bar = NULL;
size_t acpimmio_size = 0;

static const io_register_t kunlun_acpi_mmio_regions[] = {
	{ 0x0000, ACPIMMIO_REGION_SIZE, "SMB PCI" },
	{ 0x0200, ACPIMMIO_REGION_SIZE, "SMI" },
	{ 0x0300, ACPIMMIO_REGION_SIZE, "PMIO" },
	{ 0x0400, ACPIMMIO_REGION_SIZE, "PMIO2" },
	{ 0x0500, ACPIMMIO_REGION_SIZE, "BIOS RAM" },
	{ 0x0600, ACPIMMIO_REGION_SIZE, "CMOS RAM" },
	{ 0x0700, ACPIMMIO_REGION_SIZE, "CMOS" },
	{ 0x0800, ACPIMMIO_REGION_SIZE, "ACPI" },
	{ 0x0900, ACPIMMIO_REGION_SIZE, "ASF" },
	{ 0x0a00, ACPIMMIO_REGION_SIZE, "SMB IO" },
	{ 0x0b00, ACPIMMIO_REGION_SIZE, "WDT" },
	{ 0x0c00, ACPIMMIO_REGION_SIZE, "HPET" },
	{ 0x0d00, ACPIMMIO_REGION_SIZE, "IOMUX" },
	{ 0x0e00, ACPIMMIO_REGION_SIZE, "MISC" },
	{ 0x1000, ACPIMMIO_REGION_SIZE, "Serial debug" },
	{ 0x1100, ACPIMMIO_REGION_SIZE, "Shadow timer" },
	{ 0x1200, ACPIMMIO_REGION_SIZE, "Remote GPIO+IOMUX" },
	{ 0x1300, ACPIMMIO_REGION_SIZE, "MISC2" },
	{ 0x1400, ACPIMMIO_REGION_SIZE, "DP-VGA" },
	{ 0x1500, ACPIMMIO_REGION_SIZE, "GPIO0" },
	{ 0x1600, ACPIMMIO_REGION_SIZE, "GPIO1" },
	{ 0x1700, ACPIMMIO_REGION_SIZE, "GPIO2" },
	{ 0x1800, ACPIMMIO_REGION_SIZE, "GPIO3" },
	{ 0x1900, ACPIMMIO_REGION_SIZE, "GPIO4" },
	{ 0x1d00, ACPIMMIO_REGION_SIZE, "AC DC TIMER" },
	{ 0x1e00, ACPIMMIO_REGION_SIZE, "AOAC" },
};

static uint8_t pmio_read8(uint8_t reg)
{
	outb(reg, 0xcd6);
	return inb(0xcd7);
}

static void print_region(const uint32_t offset)
{
	unsigned int i, j;

	printf("     0  1  2  3  4  5  6  7  8  9  a  b  c  d  e  f\n");

	for (i = 0; i < 0x100; i += 16) {
		printf("%02x: ", i);
		for (j = 0; j < 16; j++) {
			printf("%02"PRIx8, read8(acpimmio_bar + offset + i + j));
			if (j < 15)
				printf(" ");
		}
		printf("\n");
	}
}

static void print_acpi_mmio_regions(const io_register_t *acpi_mmio_regions, size_t size)
{
	size_t i;

	if (acpi_mmio_regions == NULL)
		return;

	for (i = 0; i < size; i++) {
		printf("\n========== %s (offset 0x%04x) ==========\n",
		       acpi_mmio_regions[i].name, acpi_mmio_regions[i].addr);
		print_region(acpi_mmio_regions[i].addr);
	}
}

static int init_acpimmio(struct pci_dev *sb)
{
	pciaddr_t acpimmio_phys;
	bool acpimmio_enabled = false;
	int smbus_rev = 0;

	if (acpimmio_bar)
		return 1;

	switch (sb->device_id) {
	case PCI_DEVICE_ID_AMD_FCH_LPC_2:
		smbus_rev = find_smbus_dev_rev(PCI_VENDOR_ID_AMD, PCI_DEVICE_ID_AMD_FCH_SMB_2);
		if (smbus_rev == -1)
			return 1;

		switch (smbus_rev) {
		case 0x71:
			acpimmio_phys = 0xfed80000;
			acpimmio_size = 0x2000;
			acpimmio_enabled = !!(pmio_read8(0x4) & 0x2);
			break;
		default:
			printf("Error: Dumping ACPI MMIO on this southbridge is not (yet) supported.\n");
			return 1;
		}

		break;
	default:
		perror("Error: Dumping ACPI MMIO on this southbridge is not (yet) supported.\n");
		return 1;
	}

	if (!acpimmio_enabled) {
		perror("ACPI MMIO not decoded by the soutbridge\n");
		return 1;
	}

	if (acpimmio_phys == 0 || acpimmio_size == 0) {
		perror("Error: Invalid ACPI MMIO address or size.\n");
		return 1;
	}

	printf("ACPI MMIO = 0x%08"PRIx64" (size 0x%"PRIx64") (MEM)\n\n", (uint64_t)acpimmio_phys, acpimmio_size);
	acpimmio_bar = map_physical(acpimmio_phys, acpimmio_size);
	if (!acpimmio_bar) {
		perror("Error mapping ACPI MMIO");
		return 1;
	}

	return 0;
}

int print_acpimmio(struct pci_dev *sb)
{
	int smbus_rev = 0;
	size_t acpimmio_regions_size = 0;
	const io_register_t *acpi_mmio_regions = NULL;

	printf("\n========== ACPI MMIO ==========\n\n");

	if (init_acpimmio(sb))
		return 1;

	switch (sb->device_id) {
	case PCI_DEVICE_ID_AMD_FCH_LPC_2:
		smbus_rev = find_smbus_dev_rev(PCI_VENDOR_ID_AMD, PCI_DEVICE_ID_AMD_FCH_SMB_2);
		if (smbus_rev == -1)
			return 1;

		switch (smbus_rev) {
		case 0x71:
			acpi_mmio_regions = kunlun_acpi_mmio_regions;
			acpimmio_regions_size = ARRAY_SIZE(kunlun_acpi_mmio_regions);
			break;
		default:
			printf("Error: Dumping ACPI MMIO on this southbridge is not (yet) supported.\n");
			return 1;
		}

		break;
	default:
		perror("Error: Dumping ACPI MMIO on this southbridge is not (yet) supported.\n");
		return 1;
	}

	print_acpi_mmio_regions(acpi_mmio_regions, acpimmio_regions_size);

	return 0;
}

const uint8_t *get_acpi_mmio_bar(struct pci_dev *sb)
{
	init_acpimmio(sb);

	return (const uint8_t *)acpimmio_bar;
}

void acpimmio_cleanup(void)
{
	if (acpimmio_bar)
		unmap_physical((void *)acpimmio_bar, acpimmio_size);
}
