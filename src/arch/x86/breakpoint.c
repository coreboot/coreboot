/* SPDX-License-Identifier: GPL-2.0-only */
#include <arch/registers.h>
#include <arch/breakpoint.h>
#include <console/console.h>
#include <types.h>

#define DEBUG_REGISTER_COUNT 4

/* Each enable field is 2 bits and starts at bit 0 */
#define DEBUG_CTRL_ENABLE_SHIFT(index) (2 * (index))
#define DEBUG_CTRL_ENABLE_MASK(index) (0x3 << DEBUG_CTRL_ENABLE_SHIFT(index))
#define DEBUG_CTRL_ENABLE(index, enable) ((enable) << DEBUG_CTRL_ENABLE_SHIFT(index))

/* Each breakpoint has a length and type, each is two bits and start at bit 16 */
#define DEBUG_CTRL_LT_SHIFT(index) (4 * (index) + 16)
#define DEBUG_CTRL_LT_MASK(index) (0xf << DEBUG_CTRL_LT_SHIFT(index))
#define DEBUG_CTRL_LT(index, len, type) ((((len) << 2 | (type))) << DEBUG_CTRL_LT_SHIFT(index))

/* Each field is one bit, starting at bit 0 */
#define DEBUG_STATUS_BP_HIT_MASK(index) (1 << (index))
#define DEBUG_STATUS_GET_BP_HIT(index, value) \
	(((value) & DEBUG_STATUS_BP_HIT_MASK(index)) >> (index))

/* Breakpoint lengths values */
#define DEBUG_CTRL_LEN_1 0x0
#define DEBUG_CTRL_LEN_2 0x1
#define DEBUG_CTRL_LEN_8 0x2
#define DEBUG_CTRL_LEN_4 0x3

/* Breakpoint enable values */
#define DEBUG_CTRL_ENABLE_LOCAL 0x1
#define DEBUG_CTRL_ENABLE_GLOBAL 0x2

/* eflags/rflags bit to continue execution after hitting an instruction breakpoint */
#define FLAGS_RESUME (1 << 16)

struct breakpoint {
	bool allocated;
	enum breakpoint_type type;
	breakpoint_handler handler;
};

static struct breakpoint breakpoints[DEBUG_REGISTER_COUNT];

static inline bool debug_write_addr_reg(int index, uintptr_t value)
{
	switch (index) {
	case 0:
		asm("mov %0, %%dr0" ::"r"(value));
		break;

	case 1:
		asm("mov %0, %%dr1" ::"r"(value));
		break;

	case 2:
		asm("mov %0, %%dr2" ::"r"(value));
		break;

	case 3:
		asm("mov %0, %%dr3" ::"r"(value));
		break;

	default:
		return false;
	}

	return true;
}

static inline uintptr_t debug_read_status(void)
{
	uintptr_t ret = 0;

	asm("mov %%dr6, %0" : "=r"(ret));
	return ret;
}

static inline void debug_write_status(uintptr_t value)
{
	asm("mov %0, %%dr6" ::"r"(value));
}

static inline uintptr_t debug_read_control(void)
{
	uintptr_t ret = 0;

	asm("mov %%dr7, %0" : "=r"(ret));
	return ret;
}

static inline void debug_write_control(uintptr_t value)
{
	asm("mov %0, %%dr7" ::"r"(value));
}

static enum breakpoint_result allocate_breakpoint(struct breakpoint_handle *out_handle,
						  enum breakpoint_type type)
{
	for (int i = 0; i < DEBUG_REGISTER_COUNT; i++) {
		if (breakpoints[i].allocated)
			continue;

		breakpoints[i].allocated = true;
		breakpoints[i].handler = NULL;
		breakpoints[i].type = type;
		out_handle->bp = i;
		return BREAKPOINT_RES_OK;
	}

	return BREAKPOINT_RES_NONE_AVAILABLE;
}

static enum breakpoint_result validate_handle(struct breakpoint_handle handle)
{
	int bp = handle.bp;

	if (bp < 0 || bp >= DEBUG_REGISTER_COUNT || !breakpoints[bp].allocated)
		return BREAKPOINT_RES_INVALID_HANDLE;

	return BREAKPOINT_RES_OK;
}

enum breakpoint_result breakpoint_create_instruction(struct breakpoint_handle *out_handle,
						     void *virt_addr)
{
	enum breakpoint_result res =
		allocate_breakpoint(out_handle, BREAKPOINT_TYPE_INSTRUCTION);

	if (res != BREAKPOINT_RES_OK)
		return res;

	int bp = out_handle->bp;
	if (!debug_write_addr_reg(bp, (uintptr_t)virt_addr))
		return BREAKPOINT_RES_INVALID_HANDLE;

	uintptr_t control = debug_read_control();
	control &= ~DEBUG_CTRL_LT_MASK(bp);
	control |= DEBUG_CTRL_LT(bp, DEBUG_CTRL_LEN_1, BREAKPOINT_TYPE_INSTRUCTION);
	debug_write_control(control);
	return BREAKPOINT_RES_OK;
}

enum breakpoint_result breakpoint_create_data(struct breakpoint_handle *out_handle,
					      void *virt_addr, size_t len, bool write_only)
{
	uintptr_t len_value = 0;

	switch (len) {
	case 1:
		len_value = DEBUG_CTRL_LEN_1;
		break;

	case 2:
		len_value = DEBUG_CTRL_LEN_2;
		break;

	case 4:
		len_value = DEBUG_CTRL_LEN_4;
		break;

	case 8:
		/* Only supported on 64-bit CPUs */
		if (!ENV_X86_64)
			return BREAKPOINT_RES_INVALID_LENGTH;
		len_value = DEBUG_CTRL_LEN_8;
		break;

	default:
		return BREAKPOINT_RES_INVALID_LENGTH;
	}

	enum breakpoint_type type =
		write_only ? BREAKPOINT_TYPE_DATA_WRITE : BREAKPOINT_TYPE_DATA_RW;
	enum breakpoint_result res = allocate_breakpoint(out_handle, type);
	if (res != BREAKPOINT_RES_OK)
		return res;

	int bp = out_handle->bp;
	if (!debug_write_addr_reg(bp, (uintptr_t)virt_addr))
		return BREAKPOINT_RES_INVALID_HANDLE;

	uintptr_t control = debug_read_control();
	control &= ~DEBUG_CTRL_LT_MASK(bp);
	control |= DEBUG_CTRL_LT(bp, len_value, type);
	debug_write_control(control);
	return BREAKPOINT_RES_OK;
}

enum breakpoint_result breakpoint_remove(struct breakpoint_handle handle)
{
	enum breakpoint_result res = validate_handle(handle);

	if (res != BREAKPOINT_RES_OK)
		return res;
	breakpoint_enable(handle, false);

	int bp = handle.bp;
	breakpoints[bp].allocated = false;
	return BREAKPOINT_RES_OK;
}

enum breakpoint_result breakpoint_enable(struct breakpoint_handle handle, bool enabled)
{
	enum breakpoint_result res = validate_handle(handle);

	if (res != BREAKPOINT_RES_OK)
		return res;

	uintptr_t control = debug_read_control();
	int bp = handle.bp;
	control &= ~DEBUG_CTRL_ENABLE_MASK(bp);
	if (enabled)
		control |= DEBUG_CTRL_ENABLE(bp, DEBUG_CTRL_ENABLE_GLOBAL);
	debug_write_control(control);
	return BREAKPOINT_RES_OK;
}

enum breakpoint_result breakpoint_get_type(struct breakpoint_handle handle,
					   enum breakpoint_type *type)
{
	enum breakpoint_result res = validate_handle(handle);

	if (res != BREAKPOINT_RES_OK)
		return res;

	*type = breakpoints[handle.bp].type;
	return BREAKPOINT_RES_OK;
}

enum breakpoint_result breakpoint_set_handler(struct breakpoint_handle handle,
					      breakpoint_handler handler)
{
	enum breakpoint_result res = validate_handle(handle);

	if (res != BREAKPOINT_RES_OK)
		return res;

	breakpoints[handle.bp].handler = handler;
	return BREAKPOINT_RES_OK;
}

static enum breakpoint_result is_breakpoint_hit(struct breakpoint_handle handle, bool *out_hit)
{
	enum breakpoint_result res = validate_handle(handle);

	if (res != BREAKPOINT_RES_OK)
		return res;

	uintptr_t status = debug_read_status();
	*out_hit = DEBUG_STATUS_GET_BP_HIT(handle.bp, status);

	return BREAKPOINT_RES_OK;
}

int breakpoint_dispatch_handler(struct eregs *info)
{
	bool instr_bp_hit = 0;

	for (int i = 0; i < DEBUG_REGISTER_COUNT; i++) {
		struct breakpoint_handle handle = { i };
		bool hit = false;
		enum breakpoint_type type;

		if (is_breakpoint_hit(handle, &hit) != BREAKPOINT_RES_OK || !hit)
			continue;

		if (breakpoint_get_type(handle, &type) != BREAKPOINT_RES_OK)
			continue;

		instr_bp_hit |= type == BREAKPOINT_TYPE_INSTRUCTION;

		/* Call the breakpoint handler. */
		if (breakpoints[handle.bp].handler) {
			int ret = breakpoints[handle.bp].handler(handle, info);
			/* A non-zero return value indicates a fatal error. */
			if (ret)
				return ret;
		}
	}

	/* Clear hit breakpoints. */
	uintptr_t status = debug_read_status();
	for (int i = 0; i < DEBUG_REGISTER_COUNT; i++) {
		status &= ~DEBUG_STATUS_BP_HIT_MASK(i);
	}
	debug_write_status(status);

	if (instr_bp_hit) {
		/* Set the resume flag so the same breakpoint won't be hit immediately. */
#if ENV_X86_64
		info->rflags |= FLAGS_RESUME;
#else
		info->eflags |= FLAGS_RESUME;
#endif
	}

	return 0;
}
