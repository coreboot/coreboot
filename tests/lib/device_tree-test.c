/* SPDX-License-Identifier: GPL-2.0-only */

#include <console/console.h>
#include <device_tree.h>
#include <helpers/file.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <tests/test.h>

static int setup_device_tree_test_group(void **state)
{
	/*
	 * fattest FDT I could find from Linux Kernel to test the worst cases
	 * https://git.kernel.org/pub/scm/linux/kernel/git/torvalds/linux.git/tree/arch/arm/boot/dts/nvidia/tegra30-ouya.dts?id=443b349019f2d946
	 */
	const char dtb_path[] = "lib/devicetree-test/tegra30-ouya.dtb";
	int file_size = test_get_file_size(dtb_path);
	assert_int_not_equal(file_size, -1);

	void *big_fat_dtb = test_malloc(file_size);
	assert_int_not_equal(big_fat_dtb, NULL);
	assert_int_equal(test_read_file(dtb_path, big_fat_dtb, file_size), file_size);
	*state = big_fat_dtb;

	return 0;
}

static int teardown_device_tree_test_group(void **state)
{
	test_free(*state);
	return 0;
}

static void test_fdt_find_node_by_path(void **state)
{
	uint32_t addrcp, sizecp;
	assert_int_equal(0, fdt_find_node_by_path(*state, "test", &addrcp, &sizecp));
	assert_int_equal(56, fdt_find_node_by_path(*state, "/", &addrcp, &sizecp));
	assert_int_equal(2, addrcp);
	assert_int_equal(1, sizecp);
	assert_int_equal(0, fdt_find_node_by_path(*state, "/test", &addrcp, &sizecp));
	assert_int_equal(0x181f4, fdt_find_node_by_path(*state, "/chosen", &addrcp, &sizecp));
	assert_int_equal(1, addrcp);
	assert_int_equal(1, sizecp);
	assert_int_equal(0x156d4, fdt_find_node_by_path(*state, "/cpus", &addrcp, &sizecp));
	assert_int_equal(1, addrcp);
	assert_int_equal(1, sizecp);
	assert_int_equal(0x1517c, fdt_find_node_by_path(*state, "/usb@7d004000/ethernet@2", &addrcp, &sizecp));
	assert_int_equal(1, addrcp);
	assert_int_equal(0, sizecp);
	assert_int_equal(0x1517c, fdt_find_node_by_path(*state, "/usb@7d004000/ethernet@2/", &addrcp, &sizecp));
	assert_int_equal(1, addrcp);
	assert_int_equal(0, sizecp);

	assert_int_equal(0xee08, fdt_find_node_by_path(*state, "/pinmux@70000868/pinmux/drive_groups",
						       &addrcp, &sizecp));
}

static void test_fdt_find_subnodes_by_prefix(void **state)
{
	uint32_t offset = fdt_find_node_by_path(*state, "/cpus", NULL, NULL);
	uint32_t results[3] = { 0 };
	uint32_t addrcp, sizecp;
	size_t count_results = fdt_find_subnodes_by_prefix(*state, offset, "cpu@",
							   &addrcp, &sizecp, results, 3);
	assert_int_equal(3, count_results);
	assert_int_equal(0x15700, results[0]);
	assert_int_equal(0x157a0, results[1]);
	assert_int_equal(0x15840, results[2]);

	results[1] = 0xDEADBEEF;
	results[2] = 0xDEADBEEF;
	count_results = fdt_find_subnodes_by_prefix(*state, offset, "cpu@",
							   &addrcp, &sizecp, results, 1);
	assert_int_equal(1, count_results);
	assert_int_equal(0x15700, results[0]);
	assert_int_equal(0xDEADBEEF, results[1]);
	assert_int_equal(0xDEADBEEF, results[2]);
}

static void test_fdt_find_node_by_alias(void **state)
{
	assert_int_equal(0xf298, fdt_find_node_by_alias(*state, "serial0", NULL, NULL));
	assert_int_equal(0, fdt_find_node_by_alias(*state, "mmc2", NULL, NULL));
}

static void test_fdt_find_prop_in_node(void **state)
{
	uintptr_t cnode_offset = fdt_find_node_by_path(*state, "/clock", NULL, NULL);
	uintptr_t mnode_offset = fdt_find_node_by_path(*state, "/memory@80000000", NULL, NULL);
	assert_int_equal(0x18400, cnode_offset);

	struct fdt_property fdt_prop;
	assert_int_equal(0x1840c, fdt_read_prop(*state, cnode_offset, "compatible", &fdt_prop));
	assert_string_equal("fixed-clock", (char *)fdt_prop.data);
	assert_int_equal(0x6094, fdt_read_prop(*state, mnode_offset, "reg", &fdt_prop));
	assert_int_equal(0x0, fdt_read_prop(*state, cnode_offset, "notfound", &fdt_prop));
}

static void test_fdt_read_reg_prop(void **state)
{
	uint32_t addrcp1, sizecp1, addrcp2, sizecp2;
	uint64_t node_offset1 = fdt_find_node_by_path(*state, "/memory@80000000", &addrcp1, &sizecp1);
	uint64_t node_offset2 = fdt_find_node_by_path(*state, "/reserved-memory/ramoops@bfdf0000", &addrcp2, &sizecp2);

	struct device_tree_region regions[3];
	regions[0].addr = 0xDEADBEEF;
	regions[0].size = 0xDEADBEEF;
	fdt_read_reg_prop(*state, node_offset1, addrcp1, sizecp1, regions, 0);
	assert_int_equal(0xDEADBEEF, regions[0].addr);
	assert_int_equal(0xDEADBEEF, regions[0].size);
	fdt_read_reg_prop(*state, node_offset2, addrcp2, sizecp2, regions, 1);
	assert_int_equal(0xbfdf0000, regions[0].addr);
	assert_int_equal(0x00010000, regions[0].size);
}

int main(void)
{
	const struct CMUnitTest tests[] = {
		cmocka_unit_test(test_fdt_find_node_by_path),
		cmocka_unit_test(test_fdt_find_subnodes_by_prefix),
		cmocka_unit_test(test_fdt_find_node_by_alias),
		cmocka_unit_test(test_fdt_find_prop_in_node),
		cmocka_unit_test(test_fdt_read_reg_prop),
	};

	return cb_run_group_tests(tests, setup_device_tree_test_group,
			teardown_device_tree_test_group);
}
