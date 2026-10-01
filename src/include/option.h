/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef _OPTION_H_
#define _OPTION_H_

#include <assert.h>
#include <types.h>

/* Constraints describe stored values. The caller's fallback is used unchanged. */
struct option_constraints {
	enum { OPTION_NUMBER, OPTION_VALUES } type;
	union {
		struct { unsigned int min, max, step; } number;
		struct { const unsigned int *values; size_t count; } enumeration;
	};
};

/* Reject negative and oversized arguments before converting to unsigned int. */
static inline struct option_constraints option_range(uint64_t low, uint64_t high, uint64_t step)
{
	bool valid = low <= UINT_MAX && high <= UINT_MAX && step <= UINT_MAX;

	assert(valid);
	if (!valid)
		return (struct option_constraints) { .type = OPTION_NUMBER };

	return (struct option_constraints) {
		.type = OPTION_NUMBER, .number = { low, high, step }
	};
}

#define OPTION_RANGE_STEP(low, high, increment) option_range(low, high, increment)
#define OPTION_RANGE(low, high) OPTION_RANGE_STEP(low, high, 1)
#define OPTION_BOOL OPTION_RANGE(0, 1)
#define OPTION_ENUM_VALUES(array, length) ((struct option_constraints) { \
	.type = OPTION_VALUES, .enumeration = { (array), (length) } })
#define OPTION_ENUM(...) OPTION_ENUM_VALUES(((const unsigned int[]) { __VA_ARGS__ }), \
	sizeof((const unsigned int[]) { __VA_ARGS__ }) / sizeof(unsigned int))

static inline bool option_value_valid(unsigned int value, struct option_constraints rule)
{
	switch (rule.type) {
	case OPTION_NUMBER:
		return rule.number.step && value >= rule.number.min &&
			value <= rule.number.max &&
			(value - rule.number.min) % rule.number.step == 0;
	case OPTION_VALUES:
		for (size_t i = 0; i < rule.enumeration.count; i++)
			if (value == rule.enumeration.values[i])
				return true;
		return false;
	}
	return false;
}

void sanitize_cmos(void);

/* The CBFS file option backend cannot be used in SMM due to vboot
 * dependencies, which are not added to SMM.
 * The UEFI variable store option backend cannot be used in verstage because:
 * - Verstage runs before SMM is available (needed for SMMSTORE access)
 * - EDK2 headers required for UEFI variables are x86-specific and not available
 *   in ARM verstage (PSP verstage).
 * Postcar does not need runtime options, and pulling in the UEFI variable store
 * backend there also pulls in SPI flash probing/programming code. */
#if CONFIG(OPTION_BACKEND_NONE) || \
	(CONFIG(USE_CBFS_FILE_OPTION_BACKEND) && ENV_SMM) || \
	(CONFIG(USE_UEFI_VARIABLE_STORE) && (ENV_SEPARATE_VERSTAGE || ENV_POSTCAR))

static inline unsigned int get_uint_option(const char *name, const unsigned int fallback)
{
	return fallback;
}

static inline enum cb_err set_uint_option(const char *name, unsigned int value)
{
	return CB_CMOS_OTABLE_DISABLED;
}

#else /* !OPTION_BACKEND_NONE */

unsigned int get_uint_option(const char *name, const unsigned int fallback);
enum cb_err set_uint_option(const char *name, unsigned int value);

#endif /* OPTION_BACKEND_NONE? */

static inline unsigned int get_uint_option_checked(const char *name, unsigned int fallback,
						   struct option_constraints rule)
{
	if (rule.type == OPTION_NUMBER) {
		assert(rule.number.min <= rule.number.max && rule.number.step);
		if (rule.number.min > rule.number.max || !rule.number.step)
			return fallback;
	}

	unsigned int value = get_uint_option(name, fallback);

	if (value == fallback)
		return fallback;

	return option_value_valid(value, rule) ? value : fallback;
}

#endif /* _OPTION_H_ */
