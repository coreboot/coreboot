/* SPDX-License-Identifier: GPL-2.0-only */

#include <bootstate.h>
#include <cbmem.h>
#include <commonlib/bsd/stdlib.h>
#include <commonlib/list.h>
#include <console/console.h>
#include <self_test.h>

static char *cbmem_st_buffer;
static struct self_test_log *cbmem_st_log;
static u32 cbmem_st_log_index;

struct selftest_entry {
	struct self_test_t test;
	struct st_status result;
	struct list_node list_node;
};

struct st_exec_state_t {
	boot_state_t state;
	boot_state_sequence_t when;
	struct list_node selftest_list;
	struct boot_state_callback callback;
};

#define ST_BS_STATE_MAX	(BS_PAYLOAD_BOOT + 1)
#define ST_BS_SEQ_MAX	(BS_ON_EXIT + 1)

#define ST_POPULATE_MAP(a) [a] = #a
static const char * const st_state_to_name[] = {
	ST_POPULATE_MAP(BS_PRE_DEVICE),
	ST_POPULATE_MAP(BS_DEV_INIT_CHIPS),
	ST_POPULATE_MAP(BS_DEV_ENUMERATE),
	ST_POPULATE_MAP(BS_DEV_RESOURCES),
	ST_POPULATE_MAP(BS_DEV_ENABLE),
	ST_POPULATE_MAP(BS_DEV_INIT),
	ST_POPULATE_MAP(BS_POST_DEVICE),
	ST_POPULATE_MAP(BS_OS_RESUME_CHECK),
	ST_POPULATE_MAP(BS_OS_RESUME),
	ST_POPULATE_MAP(BS_WRITE_TABLES),
	ST_POPULATE_MAP(BS_PAYLOAD_LOAD),
	ST_POPULATE_MAP(BS_PAYLOAD_BOOT)
};
static const char * const st_seq_to_name[] = {
	ST_POPULATE_MAP(BS_ON_ENTRY),
	ST_POPULATE_MAP(BS_ON_EXIT)
};

static struct st_exec_state_t st_exec_states[ST_BS_STATE_MAX * ST_BS_SEQ_MAX];

static void st_log(uint32_t id, struct st_status result)
{
	size_t max_elements = CONFIG_SELF_TEST_OUTPUT_BUFFER_SIZE / sizeof(struct self_test_log);

	if (cbmem_st_log_index < max_elements) {
		struct self_test_log *entry = &cbmem_st_log[cbmem_st_log_index++];
		entry->id = id;
		entry->result = result;
	}
}

static void add_selftest(const struct self_test_t *test)
{
	if (!test || !test->pre_check)
		return;

	if (test->state >= ST_BS_STATE_MAX || test->when >= ST_BS_SEQ_MAX) {
		st_debug("Error: Invalid self test bounds (state: %d, when: %d)\n",
			 test->state, test->when);
		return;
	}

	struct selftest_entry *e = xmalloc(sizeof(*e));
	if (!e) {
		st_debug("Error: Failed to allocate memory for self test entry\n");
		return;
	}

	/* Initialize the entry */
	e->test = *test;
	e->result.status = ST_SKIPPED;
	e->result.error_code = 0;

	/* Append to the appropriate execution list */
	u32 list_idx = (test->state * ST_BS_SEQ_MAX) + test->when;
	list_append(&e->list_node, &st_exec_states[list_idx].selftest_list);

	st_debug("Added self test 0x%08x\n", e->test.id);
}

void register_selftest(const struct self_test_t *tests)
{
	if (tests == NULL)
		return;
	while (tests->id != ST_INVALID_ID) {
		add_selftest(tests);
		tests++;
	}
}

static const char *st_get_name(uint32_t id)
{
	for (int i = 0; i < ARRAY_SIZE(st_ids); i++) {
		if (st_ids[i].id == id)
			return st_ids[i].name;
	}
	return NULL;
}

static void run_selftest(void *data)
{
	size_t idx;
	struct selftest_entry *node;
	if (cbmem_st_buffer == NULL) {
		st_debug("No self test buffer in cbmem\n");
		return;
	}
	struct st_exec_state_t *st_exec_state = (struct st_exec_state_t *)data;

	if (list_is_empty(&st_exec_state->selftest_list))
		return;

	st_info("Executing self test(s) at %s/%s\n",
		st_state_to_name[st_exec_state->state], st_seq_to_name[st_exec_state->when]);
	st_debug("START\n");
	idx = 0;
	list_for_each(node, st_exec_state->selftest_list, list_node) {
		st_debug("[%zu] Running 0x%x\n", idx + 1, node->test.id);
		node->result = node->test.exec();
		st_info("Selftest: %s (0x%x), Result: %s, Error Code: 0x%x\n",
			st_get_name(node->test.id), node->test.id,
			st_status_str[node->result.status], node->result.error_code);
		st_log(node->test.id, node->result);
		idx++;
	}
	st_debug("END\n");
	st_info("Executed %zu self test(s)\n", idx);
	st_info("self test buffer state: %zu/%u\n",
		(cbmem_st_log_index * sizeof(struct self_test_log)),
		CONFIG_SELF_TEST_OUTPUT_BUFFER_SIZE);
}

static void init_selftest(void *unused)
{
	st_debug("Starting self test init\n");
	st_info("self test buffer size is %d\n", CONFIG_SELF_TEST_OUTPUT_BUFFER_SIZE);
	cbmem_st_buffer = cbmem_add(CBMEM_ID_SELF_TEST, CONFIG_SELF_TEST_OUTPUT_BUFFER_SIZE);
	if (cbmem_st_buffer == NULL) {
		st_debug("failed to add self test buffer in cbmem\n");
		return;
	}

	memset(cbmem_st_buffer, 0, CONFIG_SELF_TEST_OUTPUT_BUFFER_SIZE);
	cbmem_st_log = (struct self_test_log *)cbmem_st_buffer;

	for (int i = 0; i < ARRAY_SIZE(st_exec_states); i++) {
		st_exec_states[i].callback.callback = run_selftest;
		st_exec_states[i].callback.arg = &st_exec_states[i];
		st_exec_states[i].state = i / ST_BS_SEQ_MAX;
		st_exec_states[i].when = i % ST_BS_SEQ_MAX;
		if (st_exec_states[i].when == BS_ON_ENTRY)
			boot_state_sched_on_entry(&st_exec_states[i].callback, st_exec_states[i].state);
		else if (st_exec_states[i].when == BS_ON_EXIT)
			boot_state_sched_on_exit(&st_exec_states[i].callback, st_exec_states[i].state);
	}

	st_debug("Exiting self test init\n");
}

/*
 * Individual self-tests can be called in any boot state, so we initialize
 * the infrastructure in the earlist boot state BS_PRE_DEVICE.
 */
BOOT_STATE_INIT_ENTRY(BS_PRE_DEVICE, BS_ON_ENTRY, init_selftest, NULL);
