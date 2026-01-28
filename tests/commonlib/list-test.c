/* SPDX-License-Identifier: GPL-2.0-only */

#include <commonlib/list.h>
#include <stdlib.h>
#include <string.h>
#include <tests/test.h>
#include <types.h>

struct test_container {
	int value;

	struct list_node list_node;
};

static void test_list_empty(void **state)
{
	struct list_node head = {};
	struct test_container *node;

	list_for_each(node, head, list_node) {
		assert_true(false);
	}

	assert_true(list_is_empty(&head));
	assert_null(list_first(&head));
	assert_null(list_last(&head));
	assert_int_equal(0, list_length(&head));
}

static void test_list_one_node(void **state)
{
	struct list_node head = {};
	struct test_container c = { .value = 1 };

	list_insert_after(&c.list_node, &head);

	assert_false(list_is_empty(&head));
	assert_null(list_next(&c.list_node, &head));
	assert_null(list_prev(&c.list_node, &head));
	assert_ptr_equal(&c.list_node, list_first(&head));
	assert_ptr_equal(&c.list_node, list_last(&head));
	assert_int_equal(1, list_length(&head));
}

static void test_list_insert_after(void **state)
{
	int i = 0;
	struct list_node head = { .prev = NULL, .next = NULL };
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

	list_insert_after(&c1->list_node, &head);
	list_insert_after(&c2->list_node, &c1->list_node);
	list_insert_after(&c3->list_node, &c2->list_node);

	list_for_each(ptr, head, list_node) {
		assert_int_equal(values[i], ptr->value);
		i++;
	}

	assert_int_equal(3, i);
	assert_int_equal(3, list_length(&head));
	assert_false(list_is_empty(&head));

	assert_ptr_equal(&c3->list_node, list_next(&c2->list_node, &head));
	assert_ptr_equal(&c1->list_node, list_prev(&c2->list_node, &head));
	assert_ptr_equal(&c1->list_node, list_first(&head));
	assert_ptr_equal(&c3->list_node, list_last(&head));

	free(c3);
	free(c2);
	free(c1);
}

static void test_list_insert_before(void **state)
{
	int i = 0;
	struct list_node head = { .prev = NULL, .next = NULL };
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

	list_insert_after(&c3->list_node, &head);
	list_insert_before(&c2->list_node, &c3->list_node);
	list_insert_before(&c1->list_node, &c2->list_node);


	list_for_each(ptr, head, list_node) {
		assert_int_equal(values[i], ptr->value);
		i++;
	}

	assert_int_equal(3, i);
	assert_int_equal(3, list_length(&head));
	assert_false(list_is_empty(&head));

	free(c3);
	free(c2);
	free(c1);
}

static void test_list_remove(void **state)
{
	struct list_node head = { .prev = NULL, .next = NULL };
	struct test_container *c1 = (struct test_container *)malloc(sizeof(*c1));
	struct test_container *c2 = (struct test_container *)malloc(sizeof(*c2));

	list_insert_after(&c1->list_node, &head);
	list_insert_after(&c2->list_node, &c1->list_node);

	assert_int_equal(2, list_length(&head));

	list_remove(&c1->list_node);
	assert_int_equal(1, list_length(&head));

	list_remove(&c2->list_node);
	assert_int_equal(0, list_length(&head));

	free(c2);
	free(c1);
}

static void test_list_append(void **state)
{
	size_t idx;
	struct test_container *node;
	struct list_node head = {};
	struct test_container nodes[] = {
		{1}, {2}, {3}
	};

	for (idx = 0; idx < ARRAY_SIZE(nodes); ++idx)
		list_append(&nodes[idx].list_node, &head);

	idx = 0;
	list_for_each(node, head, list_node) {
		assert_ptr_equal(node, &nodes[idx]);
		idx++;
	}

	assert_int_equal(3, idx);
	assert_int_equal(3, list_length(&head));
}

int main(void)
{
	const struct CMUnitTest tests[] = {
		cmocka_unit_test(test_list_empty),
		cmocka_unit_test(test_list_one_node),
		cmocka_unit_test(test_list_insert_after),
		cmocka_unit_test(test_list_insert_before),
		cmocka_unit_test(test_list_remove),
		cmocka_unit_test(test_list_append),
	};

	return cb_run_group_tests(tests, NULL, NULL);
}
