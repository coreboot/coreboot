/* SPDX-License-Identifier: GPL-2.0-only */

#include <acpi/acpi.h>
#include <amdblocks/ioapic.h>
#include <device/device.h>
#include <tests/test.h>

static struct device domains[3];
struct device dev_root;
static struct resource ioapic = { .base = 0xfec00000 };
static unsigned int probes, emitted;

struct resource *probe_resource(const struct device *dev, unsigned int index)
{
	assert_true(dev->enabled);
	assert_int_equal(index, IOMMU_IOAPIC_IDX);
	probes++;
	return dev == &domains[2] ? NULL : &ioapic;
}

int acpi_create_madt_ioapic_from_hw(acpi_madt_ioapic_t *entry, u32 address)
{
	assert_int_equal(address, ioapic.base);
	emitted++;
	return sizeof(*entry);
}

static void test_disabled_domain_not_probed(void **state)
{
	uint8_t table[128];

	all_devices = domains;
	for (size_t i = 0; i < ARRAY_SIZE(domains); i++) {
		domains[i].path.type = DEVICE_PATH_DOMAIN;
		domains[i].next = i < 2 ? &domains[i + 1] : NULL;
	}
	domains[0].enabled = false;
	domains[1].enabled = true;
	domains[2].enabled = true;
	probes = emitted = 0;
	assert_int_equal(acpi_fill_madt((uintptr_t)table),
			 (uintptr_t)table + sizeof(acpi_madt_ioapic_t));
	assert_int_equal(probes, 2);
	assert_int_equal(emitted, 1);
}

int main(void)
{
	const struct CMUnitTest tests[] = {
		cmocka_unit_test(test_disabled_domain_not_probed),
	};
	return cb_run_group_tests(tests, NULL, NULL);
}
