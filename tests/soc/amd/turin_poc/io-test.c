/* SPDX-License-Identifier: GPL-2.0-only */

#include <amdblocks/data_fabric.h>
#include <amdblocks/ioapic.h>
#include <amdblocks/psp.h>
#include <amdblocks/root_complex.h>
#include <amdblocks/smn.h>
#include <device/device.h>
#include <device/pci.h>
#include <soc/fabric.h>
#include <soc/amd/common/block/psp/psp_def.h>
#include <string.h>
#include <tests/test.h>

DEVTREE_CONST struct device dev_root;
static struct device domain;
static struct bus downstream, upstream;
static struct resource ioapic_resource;
static unsigned int set_resources_calls;

#if !ENV_SMM
/* A legacy designated initializer leaves the new socket/BDF fields zero. */
static const struct domain_iohc_info test_iohc[9] = {
	[0] = { .fabric_id = 0x24, .misc_smn_base = SMN_IOHC_MISC_BASE_13D1 },
	[1] = {
		.fabric_id = 0x25, .misc_smn_base = SMN_IOHC_MISC_BASE_1D71,
		.smn_dev = PCI_DEV(0x160, 0, 0), .socket = 0,
	},
	[8] = {
		.fabric_id = 0x124, .misc_smn_base = SMN_IOHC_MISC_BASE_13D1,
		.smn_dev = PCI_DEV(0x180, 0, 0), .socket = 1,
	},
};

const struct domain_iohc_info *get_iohc_info(size_t *count)
{
	*count = ARRAY_SIZE(test_iohc);
	return test_iohc;
}

void amd_pci_domain_read_resources(struct device *dev) { fail(); }
void amd_pci_domain_init(struct device *dev) { fail(); }
void amd_pci_domain_fill_ssdt(const struct device *dev) { fail(); }
void turin_pci_domain_scan_bus(struct device *dev) { fail(); }

struct device *pcidev_on_root(uint8_t dev, uint8_t fn)
{
	fail_msg("No default-root PCI access expected in these resource operations");
	return NULL;
}

void pci_domain_set_resources(struct device *dev)
{
	assert_ptr_equal(dev, &domain);
	set_resources_calls++;
}

extern struct device_operations turin_pci_domain_ops;
#endif

int printk(int level, const char *fmt, ...)
{
	return 0;
}

void __noreturn pcidev_die(void)
{
	fail_msg("Unexpected PCI device lookup failure");
	__builtin_unreachable();
}

uint32_t pci_s_read_config32(pci_devfn_t dev, uint16_t reg)
{
	check_expected(dev);
	check_expected(reg);
	return mock_type(uint32_t);
}

void pci_s_write_config32(pci_devfn_t dev, uint16_t reg, uint32_t value)
{
	check_expected(dev);
	check_expected(reg);
	check_expected(value);
}

const struct resource *resource_range_idx(struct device *dev, unsigned long index,
					uint64_t base, uint64_t size, unsigned long flags)
{
	check_expected_ptr(dev);
	check_expected(index);
	check_expected(base);
	check_expected(size);
	assert_int_equal(flags, IORESOURCE_FIXED | IORESOURCE_MEM |
			 IORESOURCE_RESERVE | IORESOURCE_STORED);
	return NULL;
}

static void expect_pci_write(pci_devfn_t dev, uint16_t reg, uint32_t value)
{
	expect_value(pci_s_write_config32, dev, dev);
	expect_value(pci_s_write_config32, reg, reg);
	expect_value(pci_s_write_config32, value, value);
}

static void expect_smn_read(pci_devfn_t dev, uint32_t reg, uint32_t value)
{
	expect_pci_write(dev, 0xb8, reg);
	expect_value(pci_s_read_config32, dev, dev);
	expect_value(pci_s_read_config32, reg, 0xbc);
	will_return(pci_s_read_config32, value);
}

static void expect_smn_read64(pci_devfn_t dev, uint32_t reg, uint64_t value)
{
	expect_smn_read(dev, reg, value);
	expect_smn_read(dev, reg + 4, value >> 32);
}

static void expect_smn_write(pci_devfn_t dev, uint32_t reg, uint32_t value)
{
	expect_pci_write(dev, 0xb8, reg);
	expect_pci_write(dev, 0xbc, value);
}

static int setup(void **state)
{
	memset(&domain, 0, sizeof(domain));
	memset(&downstream, 0, sizeof(downstream));
	memset(&ioapic_resource, 0, sizeof(ioapic_resource));
	domain.path.type = DEVICE_PATH_DOMAIN;
	domain.downstream = &downstream;
	domain.upstream = &upstream;
	set_resources_calls = 0;
	return 0;
}

static void test_explicit_smn_transactions(void **state)
{
	const pci_devfn_t dev = PCI_DEV(0x180, 0, 0);
	const uint32_t reg = 0x13d102f0;
	const uint64_t value = 0x9876543212345601ull;
	expect_smn_read64(dev, reg, value);
	assert_int_equal(smn_read64_at(dev, reg), value);
	expect_smn_write(dev, reg, 0x12345600);
	smn_write32_at(dev, reg, 0x12345600);
}

#if !ENV_SMM
static void check_domain_resources(unsigned int domain_id)
{
	domain.path.domain.domain_id = domain_id;
	const struct domain_iohc_info *iohc = get_domain_iohc_info(&domain);
	assert_non_null(iohc);
	size_t count;
	const struct non_pci_mmio_reg *regs = get_iohc_non_pci_mmio_regs(&count);
	unsigned long idx = 7;
	for (size_t i = 0; i < count; i++) {
		/* Two enabled BARs, including one above 4 GiB; disabled BARs must vanish. */
		const uint64_t base = regs[i].iohc_misc_offset == 0x2d8 ? 0x180000000ull :
			(regs[i].res_idx == IOMMU_IOAPIC_IDX ? 0xfec10000ull : 0);
		expect_smn_read64(iohc->smn_dev, iohc->misc_smn_base + regs[i].iohc_misc_offset,
				  base ? base | IOHC_MMIO_EN : 0);
		if (!base)
			continue;
		expect_value(resource_range_idx, dev, &domain);
		expect_value(resource_range_idx, index,
			     regs[i].res_idx == NON_PCI_RES_IDX_AUTO ? 7 : regs[i].res_idx);
		expect_value(resource_range_idx, base, base);
		expect_value(resource_range_idx, size, regs[i].size);
	}
	read_non_pci_resources(&domain, &idx);
	assert_int_equal(idx, 8);
}

static void test_root_resource_endpoint(void **state)
{
	check_domain_resources(0);
	check_domain_resources(1);
	check_domain_resources(8);
	/* A hole must not turn into an SMN access through BDF 0/address 0. */
	domain.path.domain.domain_id = 2;
	unsigned long idx = 7;
	read_non_pci_resources(&domain, &idx);
	assert_int_equal(idx, 7);
}

static void test_ioapic_enable_uses_own_root(void **state)
{
	domain.path.domain.domain_id = 8;
	ioapic_resource.index = IOMMU_IOAPIC_IDX;
	ioapic_resource.base = 0xfec10000;
	domain.resource_list = &ioapic_resource;
	expect_smn_read(PCI_DEV(0x180, 0, 0), 0x13d102f0, 0xfec10000);
	expect_smn_write(PCI_DEV(0x180, 0, 0), 0x13d102f0, 0xfec10001);
	turin_pci_domain_ops.set_resources(&domain);
	assert_int_equal(set_resources_calls, 1);
	assert_string_equal(turin_pci_domain_ops.acpi_name(&domain), "S1B0");
	domain.path.domain.domain_id = 15;
	assert_string_equal(turin_pci_domain_ops.acpi_name(&domain), "S1B7");
	/* No mapping: neither downstream resource assignment nor a PCI write. */
	domain.path.domain.domain_id = 2;
	turin_pci_domain_ops.set_resources(&domain);
	assert_int_equal(set_resources_calls, 1);
}
#else
static void test_smm_table_and_default_smn(void **state)
{
	/* Same DF-to-NBIO mapping as ramstage; address each IOHC's own bank. */
	static const uint32_t expected_smn[] = {
		0x13d10000, 0x1d710000, 0x13e10000, 0x1d610000,
		0x1d410000, 0x13c10000, 0x1d510000, 0x13b10000,
	};
	size_t count;
	const struct domain_iohc_info *iohc = get_iohc_info(&count);
	assert_int_equal(count, 8); /* Even with CONFIG_MAX_SOCKET=2. */
	assert_int_equal(iohc[0].fabric_id, 0x24);
	assert_int_equal(iohc[7].fabric_id, 0x20);
	for (size_t i = 0; i < count; i++) {
		assert_int_equal(iohc[i].misc_smn_base, expected_smn[i]);
		assert_int_equal(iohc[i].smn_dev, PCI_DEV(0, 0, 0));
		assert_int_equal(iohc[i].socket, 0);
	}
	expect_smn_read(PCI_DEV(0, 0, 0), 0x13d102f0, 0x76543210);
	assert_int_equal(smn_read32(0x13d102f0), 0x76543210);
	expect_smn_write(PCI_DEV(0, 0, 0), 0x13d102f0, 0x12345678);
	smn_write32(0x13d102f0, 0x12345678);
}
#endif

static void test_psp_stays_on_master_socket(void **state)
{
	size_t count;
	const struct domain_iohc_info *iohc = get_iohc_info(&count);
	const uint64_t base = 0xfe900000;
	for (size_t i = 0; i < count; i++) {
		if (!iohc[i].misc_smn_base || iohc[i].socket != 0)
			continue;
		/* Multiple S0 aliases are valid. S1 must not replace the selected base. */
		const uint64_t value = base | IOHC_MMIO_EN | (i ? BIT(8) : 0);
		expect_smn_read64(iohc[i].smn_dev, iohc[i].misc_smn_base + 0x2e0, value);
		if (!i)
			expect_smn_write(iohc[i].smn_dev, iohc[i].misc_smn_base + 0x2e0,
					 value | BIT(8));
	}
	assert_int_equal(psp_get_base(), base);
	/* Cached access performs no further PCI transactions. */
	assert_int_equal(psp_get_base(), base);
}

int main(void)
{
	const struct CMUnitTest tests[] = {
		cmocka_unit_test_setup(test_explicit_smn_transactions, setup),
#if !ENV_SMM
		cmocka_unit_test_setup(test_root_resource_endpoint, setup),
		cmocka_unit_test_setup(test_ioapic_enable_uses_own_root, setup),
#else
		cmocka_unit_test_setup(test_smm_table_and_default_smn, setup),
#endif
		cmocka_unit_test_setup(test_psp_stays_on_master_socket, setup),
	};
	return cb_run_group_tests(tests, NULL, NULL);
}
