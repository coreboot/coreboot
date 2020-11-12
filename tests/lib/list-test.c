/* SPDX-License-Identifier: GPL-2.0-only */

#include <tests/test.h>
#include <stdlib.h>
#include <string.h>
#include <list.h>

struct test_container {
	int value;

	struct list_node list_node;
};

void test_list_insert_after(void **state)
{
	int i = 0;
	struct list_node root = { .prev = NULL, .next = NULL };
	struct test_container *c1 = (struct test_container *)malloc(sizeof(*c1));
	struct test_container *c2 = (struct test_container *)malloc(sizeof(*c2));
	struct test_container *c3 = (struct test_container *)malloc(sizeof(*c2));
	struct test_container *ptr;
	const int values[] = { 5, 10, 13 }; /* Random values */

	memset(c1, 0, sizeof(*c1));
	memset(c2, 0, sizeof(*c2));
	memset(c2, 0, sizeof(*c3));

	c1->value = values[0];
	c2->value = values[1];
	c3->value = values[2];

	list_insert_after(&c1->list_node, &root);
	list_insert_after(&c2->list_node, &c1->list_node);
	list_insert_after(&c3->list_node, &c2->list_node);

	list_for_each(ptr, root, list_node) {
		assert_int_equal(values[i], ptr->value);
		i++;
	}

	assert_int_equal(3, i);

	free(c3);
	free(c2);
	free(c1);
}

void test_list_insert_before(void **state)
{
	int i = 0;
	struct list_node root = { .prev = NULL, .next = NULL };
	struct test_container *c1 = (struct test_container *)malloc(sizeof(*c1));
	struct test_container *c2 = (struct test_container *)malloc(sizeof(*c2));
	struct test_container *c3 = (struct test_container *)malloc(sizeof(*c2));
	struct test_container *ptr;
	const int values[] = { 19, 71, 991  }; /* Random values */

	memset(c1, 0, sizeof(*c1));
	memset(c2, 0, sizeof(*c2));
	memset(c2, 0, sizeof(*c3));

	c1->value = values[0];
	c2->value = values[1];
	c3->value = values[2];

	list_insert_after(&c3->list_node, &root);
	list_insert_before(&c2->list_node, &c3->list_node);
	list_insert_before(&c1->list_node, &c2->list_node);


	list_for_each(ptr, root, list_node) {
		assert_int_equal(values[i], ptr->value);
		i++;
	}

	assert_int_equal(3, i);

	free(c3);
	free(c2);
	free(c1);
}

void test_list_remove(void **state)
{
	struct list_node root = { .prev = NULL, .next = NULL };
	struct test_container *c1 = (struct test_container *)malloc(sizeof(*c1));
	struct test_container *c2 = (struct test_container *)malloc(sizeof(*c2));
	struct test_container *ptr;
	int len;

	list_insert_after(&c1->list_node, &root);
	list_insert_after(&c2->list_node, &c1->list_node);

	len = 0;
	list_for_each(ptr, root, list_node) {
		len++;
	}
	assert_int_equal(2, len);

	list_remove(&c1->list_node);

	len = 0;
	list_for_each(ptr, root, list_node) {
		len++;
	}
	assert_int_equal(1, len);

	list_remove(&c2->list_node);
	len = 0;
	list_for_each(ptr, root, list_node) {
		len++;
	}
	assert_int_equal(0, len);

	free(c2);
	free(c1);
}

int main(void)
{
	const struct CMUnitTest tests[] = {
		cmocka_unit_test(test_list_insert_after),
		cmocka_unit_test(test_list_insert_before),
		cmocka_unit_test(test_list_remove),
	};


	return cmocka_run_group_tests(tests, NULL, NULL);
}
