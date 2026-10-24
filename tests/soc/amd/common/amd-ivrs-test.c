/* SPDX-License-Identifier: GPL-2.0-only */

#include <acpi/acpi.h>
#include <acpi/acpi_ivrs.h>
#include <amdblocks/acpi.h>
#include <amdblocks/ioapic.h>
#include <amdblocks/iommu.h>
#include <arch/ioapic.h>
#include <device/device.h>
#include <device/pci_def.h>
#include <device/pci_ops.h>
#include <soc/pci_devs.h>
#include <tests/test.h>

#define MAX_DOMAINS 8
#define IOMMU_CAP_OFFSET 0x40
#define FCH_VECTOR_COUNT 24

struct test_domain {
	struct device dev;
	struct bus bus;
	struct device nb;
	struct device iommu;
	struct resource ioapic;
	bool has_nb;
	bool has_iommu;
	bool has_ioapic;
	u64 iommu_base;
	u64 control;
	u64 features;
	u64 features2;
	u64 counters;
	u32 capability;
	u8 vectors;
	const char *extra_hid;
	const char *extra_uid;
	u16 extra_dev_id;
	unsigned int extension_calls;
	unsigned int pci_reads;
	unsigned int mmio_reads32;
	unsigned int mmio_reads64;
	unsigned int resource_probes;
};

struct block_summary {
	acpi_ivrs_ivhd_t *header;
	unsigned int hpet;
	unsigned int fch;
	unsigned int ioapic;
	unsigned int emmc;
	ivrs_ivhd_f0_entry_t *extra;
};

static struct test_domain domains[MAX_DOMAINS];
static size_t domain_count;
static u8 table_buffer[4096] __aligned(16);
static acpi_rsdp_t rsdp;
static acpi_xsdt_t xsdt;
static unsigned long irq_override_start;
static unsigned long table_write_limit;
const unsigned int asl_revision = 0;

int printk(int level, const char *fmt, ...)
{
	return 0;
}

void __noreturn pcidev_die(void)
{
	fail_msg("Unexpected null PCI device");
	__builtin_unreachable();
}

static struct test_domain *domain_for_device(const struct device *dev)
{
	for (size_t i = 0; i < domain_count; i++)
		if (dev == &domains[i].dev)
			return &domains[i];
	fail_msg("Unexpected domain device");
	return NULL;
}

struct device *dev_find_path(struct device *previous, enum device_path_type type)
{
	assert_int_equal(type, DEVICE_PATH_DOMAIN);
	if (!previous)
		return domain_count ? &domains[0].dev : NULL;
	for (size_t i = 0; i < domain_count; i++)
		if (previous == &domains[i].dev)
			return i + 1 < domain_count ? &domains[i + 1].dev : NULL;
	fail_msg("Unexpected domain iterator");
	return NULL;
}

struct device *pcidev_path_behind(const struct bus *bus, unsigned int devfn)
{
	assert_non_null(bus);
	struct test_domain *domain = domain_for_device(bus->dev);
	assert_true(domain->dev.enabled);
	assert_ptr_equal(bus, &domain->bus);
	if (devfn == PCI_DEVFN(0, 0))
		return domain->has_nb ? &domain->nb : NULL;
	assert_int_equal(devfn, PCI_DEVFN(0, 2));
	return domain->has_iommu ? &domain->iommu : NULL;
}

unsigned int dev_get_domain_id(const struct device *dev)
{
	assert_int_equal(dev->path.type, DEVICE_PATH_DOMAIN);
	return dev->path.domain.domain_id;
}

struct resource *probe_resource(const struct device *dev, unsigned int index)
{
	struct test_domain *domain = domain_for_device(dev);
	assert_true(dev->enabled);
	assert_int_equal(index, IOMMU_IOAPIC_IDX);
	domain->resource_probes++;
	return domain->has_ioapic ? &domain->ioapic : NULL;
}

static struct test_domain *domain_for_bdf(pci_devfn_t bdf)
{
	for (size_t i = 0; i < domain_count; i++) {
		struct test_domain *domain = &domains[i];
		if (bdf != PCI_DEV(domain->bus.segment_group * 256 +
				   domain->bus.secondary, 0, 2))
			continue;
		assert_true(domain->dev.enabled);
		assert_non_null(domain->dev.downstream);
		assert_true(domain->has_nb && domain->nb.enabled);
		assert_true(domain->has_iommu && domain->iommu.enabled);
		return domain;
	}
	fail_msg("Unexpected PCI access at BDF 0x%x", bdf);
	return NULL;
}

u16 pci_s_find_capability(pci_devfn_t dev, u16 cap)
{
	domain_for_bdf(dev);
	assert_int_equal(cap, IOMMU_CAP_ID);
	return IOMMU_CAP_OFFSET;
}

u32 pci_s_read_config32(pci_devfn_t dev, u16 reg)
{
	struct test_domain *domain = domain_for_bdf(dev);
	domain->pci_reads++;
	switch (reg) {
	case IOMMU_CAP_BASE_LO:
		return (u32)domain->iommu_base | IOMMU_ENABLE;
	case IOMMU_CAP_BASE_HI:
		return domain->iommu_base >> 32;
	case IOMMU_CAP_OFFSET:
		return domain->capability;
	case IOMMU_CAP_OFFSET + 0xc:
		return 0;
	case IOMMU_CAP_OFFSET + 0x10:
		return 0;
	default:
		fail_msg("Unexpected IOMMU configuration register 0x%x", reg);
		return 0;
	}
}

static struct test_domain *domain_for_mmio(uintptr_t address)
{
	for (size_t i = 0; i < domain_count; i++) {
		struct test_domain *domain = &domains[i];
		if (address >= domain->iommu_base &&
		    address < domain->iommu_base + 0x10000) {
			assert_true(domain->dev.enabled);
			assert_true(domain->has_nb && domain->nb.enabled);
			assert_true(domain->has_iommu && domain->iommu.enabled);
			return domain;
		}
	}
	fail_msg("Unexpected IOMMU MMIO address 0x%llx", (unsigned long long)address);
	return NULL;
}

u32 acpi_test_read32(const volatile void *addr)
{
	uintptr_t address = (uintptr_t)addr;
	struct test_domain *domain = domain_for_mmio(address);
	domain->mmio_reads32++;
	switch (address - domain->iommu_base) {
	case 0x30:
		return domain->features;
	case 0x34:
		return domain->features >> 32;
	case 0x1a0:
		return domain->features2;
	case 0x1a4:
		return domain->features2 >> 32;
	default:
		fail_msg("Unexpected 32-bit IOMMU MMIO register");
		return 0;
	}
}

u64 acpi_test_read64(const volatile void *addr)
{
	uintptr_t address = (uintptr_t)addr;
	struct test_domain *domain = domain_for_mmio(address);
	domain->mmio_reads64++;
	switch (address - domain->iommu_base) {
	case 0x18:
		return domain->control;
	case 0x30:
		return domain->features;
	case 0x4000:
		return domain->counters;
	default:
		fail_msg("Unexpected 64-bit IOMMU MMIO register");
		return 0;
	}
}

u8 get_ioapic_id(uintptr_t base)
{
	if (base == IO_APIC_ADDR)
		return FCH_IOAPIC_ID;
	for (size_t i = 0; i < domain_count; i++)
		if (base == domains[i].ioapic.base) {
			assert_true(domains[i].dev.enabled && domains[i].has_ioapic);
			return i + 1;
		}
	fail_msg("Unexpected IOAPIC read");
	return 0;
}

unsigned int ioapic_get_max_vectors(uintptr_t base)
{
	if (base == IO_APIC_ADDR)
		return FCH_VECTOR_COUNT;
	for (size_t i = 0; i < domain_count; i++)
		if (base == domains[i].ioapic.base) {
			assert_true(domains[i].dev.enabled && domains[i].has_ioapic);
			return domains[i].vectors;
		}
	fail_msg("Unexpected IOAPIC vector-count read");
	return 0;
}

bool check_acpi_tables_write_limit(unsigned long current, size_t size)
{
	assert_true(current >= (uintptr_t)table_buffer);
	assert_true(current <= (uintptr_t)table_buffer + sizeof(table_buffer));
	return current <= table_write_limit && size <= table_write_limit - current;
}

unsigned long mainboard_write_madt_irq_overrides(unsigned long current)
{
	irq_override_start = current;
	return current + acpi_create_madt_irqoverride((void *)current, 0, 0, 2, 0);
}

static struct test_domain *add_domain(unsigned int id, u16 segment, u8 bus)
{
	assert_true(domain_count < MAX_DOMAINS);
	const size_t index = domain_count++;
	struct test_domain *domain = &domains[index];
	domain->dev.path.type = DEVICE_PATH_DOMAIN;
	domain->dev.path.domain.domain_id = id;
	domain->dev.enabled = 1;
	domain->dev.downstream = &domain->bus;
	domain->bus.dev = &domain->dev;
	domain->bus.segment_group = segment;
	domain->bus.secondary = bus;
	domain->bus.subordinate = bus + 0x0f;
	domain->bus.max_subordinate = bus + 0x0f;
	domain->nb.path.type = DEVICE_PATH_PCI;
	domain->nb.path.pci.devfn = PCI_DEVFN(0, 0);
	domain->nb.upstream = &domain->bus;
	domain->nb.enabled = 1;
	domain->iommu.path.type = DEVICE_PATH_PCI;
	domain->iommu.path.pci.devfn = PCI_DEVFN(0, 2);
	domain->iommu.upstream = &domain->bus;
	domain->iommu.enabled = 1;
	domain->ioapic.index = IOMMU_IOAPIC_IDX;
	domain->ioapic.base = 0xfec20000 + index * 0x1000;
	domain->has_nb = true;
	domain->has_iommu = true;
	domain->has_ioapic = true;
	domain->iommu_base = 0xfea00000 + index * 0x10000;
	domain->capability = EFR_FEATURE_SUP;
	domain->vectors = 32;
	return domain;
}

static int setup(void **state)
{
	memset(domains, 0, sizeof(domains));
	memset(table_buffer, 0, sizeof(table_buffer));
	memset(&rsdp, 0, sizeof(rsdp));
	memset(&xsdt, 0, sizeof(xsdt));
	domain_count = 0;
	irq_override_start = 0;
	table_write_limit = (uintptr_t)table_buffer + sizeof(table_buffer);
	rsdp.xsdt_address = (uintptr_t)&xsdt;
	return 0;
}

static acpi_ivrs_t *build_ivrs(void)
{
	acpi_ivrs_t *ivrs = (void *)table_buffer;
	unsigned long end = acpi_add_ivrs_table((uintptr_t)table_buffer, &rsdp);
	assert_int_equal(end, (uintptr_t)table_buffer + ivrs->header.length);
	assert_int_equal(xsdt.entry[0], (uintptr_t)ivrs);
	assert_memory_equal(ivrs->header.signature, "IVRS", 4);
	u8 sum = 0;
	for (size_t i = 0; i < ivrs->header.length; i++)
		sum += table_buffer[i];
	assert_int_equal(sum, 0);
	return ivrs;
}

/* Decode the variable-length records emitted by the real IVRS writer. */
static size_t summarize_ivrs(acpi_ivrs_t *ivrs, struct block_summary *blocks)
{
	u8 *cursor = (u8 *)&ivrs->ivhd;
	u8 *table_end = (u8 *)ivrs + ivrs->header.length;
	size_t count = 0;
	while (cursor < table_end) {
		assert_true(count < MAX_DOMAINS * 3);
		struct block_summary *summary = &blocks[count++];
		memset(summary, 0, sizeof(*summary));
		summary->header = (void *)cursor;
		acpi_ivrs_ivhd_t *header = summary->header;
		size_t header_size;
		switch (header->type) {
		case IVHD_BLOCK_TYPE_LEGACY__FIXED:
			header_size = sizeof(*header);
			break;
		case IVHD_BLOCK_TYPE_FULL__FIXED:
			header_size = sizeof(acpi_ivrs_ivhd11_t);
			break;
		case IVHD_BLOCK_TYPE_FULL__ACPI_HID:
			header_size = sizeof(acpi_ivrs_ivhd40_t);
			break;
		default:
			fail_msg("Unexpected IVHD type 0x%x", header->type);
			return count;
		}
		assert_true(header->length >= header_size);
		u8 *block_end = cursor + header->length;
		assert_true(block_end <= table_end);
		/* The first entry is a four-byte-aligned device range. */
		u8 *entry = (void *)ALIGN_UP((uintptr_t)cursor + header_size, 4);
		while (entry < block_end) {
			size_t entry_size;
			switch (*entry) {
			case IVHD_DEV_4_BYTE_START_RANGE:
			case IVHD_DEV_4_BYTE_END_RANGE:
			case IVHD_DEV_4_BYTE_SELECT:
				entry_size = sizeof(ivrs_ivhd_generic_t);
				break;
			case IVHD_DEV_8_BYTE_EXT_SPECIAL_DEV: {
				ivrs_ivhd_special_t *special = (void *)entry;
				entry_size = sizeof(*special);
				assert_true(entry + entry_size <= block_end);
				assert_int_equal(special->reserved, 0);
				if (special->variety == IVHD_SPECIAL_DEV_HPET) {
					assert_int_equal(special->source_dev_id, SMBUS_DEVFN);
					summary->hpet++;
				} else {
					assert_int_equal(special->variety,
							 IVHD_SPECIAL_DEV_IOAPIC);
					if (special->source_dev_id == SMBUS_DEVFN)
						summary->fch++;
					else
						summary->ioapic++;
				}
				break;
			}
			case IVHD_DEV_VARIABLE: {
				ivrs_ivhd_f0_entry_t *f0 = (void *)entry;
				assert_true(entry + sizeof(*f0) <= block_end);
				entry_size = sizeof(*f0) + f0->uuid_length;
				if (!memcmp(f0->hardware_id, "AMDI0040", 8)) {
					assert_int_equal(f0->uuid_format, IVHD_UID_NOT_PRESENT);
					assert_int_equal(f0->uuid_length, 0);
					summary->emmc++;
				} else {
					assert_null(summary->extra);
					summary->extra = f0;
				}
				break;
			}
			default:
				fail_msg("Unexpected IVHD device entry 0x%x", *entry);
				return count;
			}
			assert_true(entry + entry_size <= block_end);
			entry += entry_size;
		}
		assert_ptr_equal(entry, block_end);
		cursor = block_end;
	}
	assert_ptr_equal(cursor, table_end);
	return count;
}

static void test_ivrs_segment_and_bus_scope(void **state)
{
	add_domain(0, 0, 0);
	add_domain(8, 1, 0);
	add_domain(1, 0, 0x80);
	struct block_summary blocks[MAX_DOMAINS * 3];
	assert_int_equal(summarize_ivrs(build_ivrs(), blocks), 9);
	const u8 types[] = {0x10, 0x11, 0x40};
	for (size_t i = 0; i < 9; i++) {
		const size_t index = i / 3;
		assert_int_equal(blocks[i].header->type, types[i % 3]);
		assert_int_equal(blocks[i].header->pci_segment_group,
				 domains[index].bus.segment_group);
		assert_int_equal(blocks[i].header->device_id,
				 (domains[index].bus.secondary << 8) | 2);
		assert_int_equal(blocks[i].ioapic, 1);
		assert_int_equal(blocks[i].hpet, index == 0 ? 1 : 0);
		assert_int_equal(blocks[i].fch, index == 0 ? 1 : 0);
		assert_int_equal(blocks[i].emmc, i == 2 ? 1 : 0);
	}
}

static struct device *peer;

struct device *soc_acpi_ivhd_domain(struct device *owner, unsigned int index)
{
	if (!index)
		return owner;
	return index == 1 && owner == &domains[0].dev ? peer : NULL;
}

static void test_peer_ranges(void **state)
{
	struct block_summary blocks[MAX_DOMAINS * 3];
	add_domain(0, 0, 0);
	struct test_domain *second = add_domain(1, 0, 0x40);
	second->has_iommu = false;
	peer = &second->dev;
	assert_int_equal(summarize_ivrs(build_ivrs(), blocks), 3);
	for (size_t i = 0; i < 3; i++) {
		assert_int_equal(blocks[i].ioapic, 2);
		assert_int_equal(blocks[i].hpet, 1);
		assert_int_equal(blocks[i].fch, 1);
	}
	peer = NULL;
}

static void test_reject_cross_segment_peer(void **state)
{
	struct block_summary blocks[MAX_DOMAINS * 3];
	add_domain(0, 0, 0);
	struct test_domain *second = add_domain(1, 1, 0x40);
	second->has_iommu = false;
	peer = &second->dev;
	assert_int_equal(summarize_ivrs(build_ivrs(), blocks), 3);
	for (size_t i = 0; i < 3; i++)
		assert_int_equal(blocks[i].ioapic, 1);
	peer = NULL;
}

static void test_reject_overlapping_peer(void **state)
{
	struct block_summary blocks[MAX_DOMAINS * 3];
	struct test_domain *owner = add_domain(0, 0, 0);
	struct test_domain *second = add_domain(1, 0, 0x40);
	second->has_iommu = false;
	peer = &second->dev;
	owner->bus.max_subordinate = 0x40;
	assert_int_equal(summarize_ivrs(build_ivrs(), blocks), 3);
	for (size_t i = 0; i < 3; i++)
		assert_int_equal(blocks[i].ioapic, 1);
	peer = &owner->dev;
	assert_int_equal(summarize_ivrs(build_ivrs(), blocks), 3);
	for (size_t i = 0; i < 3; i++)
		assert_int_equal(blocks[i].ioapic, 1);
	peer = NULL;
}

int main(void)
{
	const struct CMUnitTest tests[] = {
		cmocka_unit_test_setup(test_ivrs_segment_and_bus_scope, setup),
		cmocka_unit_test_setup(test_peer_ranges, setup),
		cmocka_unit_test_setup(test_reject_overlapping_peer, setup),
		cmocka_unit_test_setup(test_reject_cross_segment_peer, setup),
	};
	return cb_run_group_tests(tests, NULL, NULL);
}
