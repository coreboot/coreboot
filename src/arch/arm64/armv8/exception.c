/* SPDX-License-Identifier: BSD-3-Clause */

#include <types.h>
#include <arch/barrier.h>
#include <arch/exception.h>
#include <arch/transition.h>
#include <console/console.h>
#include <console/uart.h>
#include <arch/lib_helpers.h>

uint8_t exception_stack[2*KiB] __attribute__((aligned(16)));

static const char *exception_names[NUM_EXC_VIDS] = {
	[EXC_VID_CUR_SP_EL0_SYNC] = "_sync_sp_el0",
	[EXC_VID_CUR_SP_EL0_IRQ] = "_irq_sp_el0",
	[EXC_VID_CUR_SP_EL0_FIRQ] = "_fiq_sp_el0",
	[EXC_VID_CUR_SP_EL0_SERR] = "_serror_sp_el0",
	[EXC_VID_CUR_SP_ELX_SYNC] = "_sync_sp_el3",
	[EXC_VID_CUR_SP_ELX_IRQ] = "_irq_sp_el3",
	[EXC_VID_CUR_SP_ELX_FIQ] = "_fiq_sp_el3",
	[EXC_VID_CUR_SP_ELX_SERR] = "_serror_sp_el3",
	[EXC_VID_LOW64_SYNC] = "_sync_elx_64",
	[EXC_VID_LOW64_IRQ] = "_irq_elx_64",
	[EXC_VID_LOW64_FIQ] = "_fiq_elx_64",
	[EXC_VID_LOW64_SERR] = "_serror_elx_64",
	[EXC_VID_LOW32_SYNC] = "_sync_elx_32",
	[EXC_VID_LOW32_IRQ] = "_irq_elx_32",
	[EXC_VID_LOW32_FIQ] = "_fiq_elx_32",
	[EXC_VID_LOW32_SERR] = "_serror_elx_32"
};

static void dump_stack(uintptr_t addr, size_t bytes)
{
	int i, j;
	const int words_per_line = 8;
	uint64_t *ptr = (void *)ALIGN_DOWN(addr, words_per_line * sizeof(*ptr));

	printk(BIOS_DEBUG, "Dumping stack:\n");
	for (i = bytes / sizeof(*ptr); i >= 0; i -= words_per_line) {
		printk(BIOS_DEBUG, "%p: ", ptr + i);
		for (j = i; j < i + words_per_line; j++)
			printk(BIOS_DEBUG, "%016llx ", *(ptr + j));
		printk(BIOS_DEBUG, "\n");
	}
}

static void print_regs(struct exc_state *exc_state)
{
	int i;
	struct elx_state *elx = &exc_state->elx;
	struct regs *regs = &exc_state->regs;

	printk(BIOS_DEBUG, "ELR = 0x%016llx         ESR = 0x%08llx\n",
	       elx->elr, raw_read_esr_el3());
	printk(BIOS_DEBUG, "FAR = 0x%016llx        SPSR = 0x%08llx\n",
	       raw_read_far_el3(), raw_read_spsr_el3());
	for (i = 0; i < 30; i += 2) {
		printk(BIOS_DEBUG,
		       "X%02d = 0x%016llx         X%02d = 0x%016llx\n",
		       i, regs->x[i], i + 1, regs->x[i + 1]);
	}
	printk(BIOS_DEBUG, "X30 = 0x%016llx          SP = 0x%016llx\n",
	       regs->x[30], regs->sp);
}

static struct exception_handler *handlers[NUM_EXC_VIDS];

enum cb_err exception_handler_register(uint64_t vid, struct exception_handler *h)
{
	if (vid >= NUM_EXC_VIDS)
		return CB_ERR;

	/* Just place at head of queue. */
	h->next = handlers[vid];
	store_release(&handlers[vid], h);

	return CB_SUCCESS;
}

enum cb_err exception_handler_unregister(uint64_t vid, struct exception_handler *h)
{
	struct exception_handler *cur;
	struct exception_handler **prev;

	if (vid >= NUM_EXC_VIDS)
		return CB_ERR;

	prev = &handlers[vid];

	for (cur = handlers[vid]; cur != NULL; cur = cur->next) {
		if (cur != h)
			continue;
		/* Update previous pointer. */
		store_release(prev, cur->next);
		return CB_SUCCESS;
	}

	/* Not found */
	return CB_ERR;
}

static void print_exception_info(struct exc_state *state, uint64_t idx)
{
	/* Poor man's sign of life in case printk() is shot. */
	__uart_tx_byte('\r');
	__uart_tx_byte('\n');
	__uart_tx_byte('!');

	printk(BIOS_DEBUG, "\nexception %s\n",
	       idx < NUM_EXC_VIDS ? exception_names[idx] : "_unknown");

	print_regs(state);
	/* Few words below SP in case we need state from a returned function. */
	dump_stack(state->regs.sp - 32, 512);
}

static void print_exception_and_die(struct exc_state *state, uint64_t idx)
{
	print_exception_info(state, idx);
	die("exception death");
}

static int handle_exception(struct exc_state *state, uint64_t idx)
{
	int ret = EXC_RET_ABORT;

	struct exception_handler *h;

	for (h = handlers[idx]; h != NULL; h = h->next) {
		int hret;

		hret = h->handler(state, idx);

		if (hret > ret)
			ret = hret;
	}

	return ret;
}

void exc_dispatch(struct exc_state *state, uint64_t idx)
{
	int ret;

	if (idx >= NUM_EXC_VIDS) {
		printk(BIOS_DEBUG, "Bad exception index %x.\n", (int)idx);
		print_exception_and_die(state, idx);
	}

	ret = handle_exception(state, idx);

	if (ret == EXC_RET_ABORT)
		print_exception_and_die(state, idx);

	if (ret == EXC_RET_IGNORED || ret == EXC_RET_HANDLED_DUMP_STATE)
		print_exception_info(state, idx);

	exc_exit(&state->regs);
}

static int test_exception_handler(struct exc_state *state, uint64_t vector_id)
{
	/* Update instruction pointer to next instrution. */
	state->elx.elr += sizeof(uint32_t);
	raw_write_elr_el3(state->elx.elr);
	return EXC_RET_HANDLED;
}

static uint64_t test_exception(void)
{
	struct exception_handler sync_elx;
	struct exception_handler sync_el0;
	unsigned long long *a = (void *)0xffffffff00000000ULL;

	sync_elx.handler = &test_exception_handler;
	sync_el0.handler = &test_exception_handler;

	exception_handler_register(EXC_VID_CUR_SP_ELX_SYNC, &sync_elx);
	exception_handler_register(EXC_VID_CUR_SP_EL0_SYNC, &sync_el0);

	force_read(*a);

	exception_handler_unregister(EXC_VID_CUR_SP_ELX_SYNC, &sync_elx);
	exception_handler_unregister(EXC_VID_CUR_SP_EL0_SYNC, &sync_el0);

	return 0;
}

void exception_init(void)
{
	/* Load the exception table and initialize SP_EL3. */
	exception_init_asm(exception_stack + ARRAY_SIZE(exception_stack));

	printk(BIOS_DEBUG, "ARM64: Exception handlers installed.\n");

	/* Only spend time testing on debug builds that are trying to detect more errors. */
	if (CONFIG(FATAL_ASSERTS)) {
		printk(BIOS_DEBUG, "ARM64: Testing exception\n");
		test_exception();
		printk(BIOS_DEBUG, "ARM64: Done test exception\n");
	}
}
