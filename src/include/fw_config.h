/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef __FW_CONFIG__
#define __FW_CONFIG__

#include <device/device.h>
#include <static.h>  /* Provides fw_config definitions from devicetree.cb */
#include <stdbool.h>
#include <stdint.h>

#define UNDEFINED_FW_CONFIG	~((uint64_t)0)

/**
 * struct fw_config - Firmware configuration field and option.
 * @field_name: Name of the field that this option belongs to.
 * @option_name: Name of the option within this field.
 * @mask: Bitmask of the field.
 * @value: Value of the option within the mask.
 */
struct fw_config {
	const char *field_name;
	const char *option_name;
	uint64_t mask;
	uint64_t value;
};

struct fw_config_field {
	const char *field_name;
	uint64_t mask;
};

/* Return the unshifted option value for a given field. */
#define FW_CONFIG_VALUE(__field, __option) \
	(((uint64_t)FW_CONFIG_FIELD_##__field##_OPTION_##__option##_VALUE) >> \
	 __builtin_ctzll(FW_CONFIG_FIELD_##__field##_MASK))

/* Generate a pointer to a compound literal of the fw_config structure. */
#define FW_CONFIG(__field, __option)	(&(const struct fw_config) {		\
	.field_name = FW_CONFIG_FIELD_##__field##_NAME,				\
	.option_name = FW_CONFIG_FIELD_##__field##_OPTION_##__option##_NAME,	\
	.mask = FW_CONFIG_FIELD_##__field##_MASK,				\
	.value = FW_CONFIG_FIELD_##__field##_OPTION_##__option##_VALUE		\
})

#define FW_CONFIG_FIELD(__field) (&(const struct fw_config_field) {		\
	.field_name = FW_CONFIG_FIELD_##__field##_NAME,				\
	.mask = FW_CONFIG_FIELD_##__field##_MASK,				\
})

/**
 * fw_config_mainboard_override() - Allow mainboard to override fw_config fields.
 *
 * Implementations should call fw_config_override_field() to override specific
 * fw_config fields at runtime. If fields are overridden when the underlying
 * FW_CONFIG source is unprovisioned (UNDEFINED_FW_CONFIG),
 * fw_config_is_provisioned() will still return %false, while fw_config_get(),
 * fw_config_probe(), and fw_config_get_field() will evaluate the overridden
 * fields.
 */
void fw_config_mainboard_override(void);

/**
 * fw_config_probe_mainboard_override() - Mainboard hook to override specific probes
 * @match: Structure containing field and option to probe
 * @result: Output parameter - set to the probe result if this probe was handled
 *
 * Return: %true if this probe was handled, %false otherwise
 *
 * Mainboards can override this function to handle specific fw_config probes
 * (e.g., based on CFR/CMOS options). If a probe is handled, return %true and
 * set *result to the desired probe result. If not handled, return %false and
 * the standard fw_config logic will be used.
 */
bool fw_config_probe_mainboard_override(const struct fw_config *match, bool *result);

#if CONFIG(FW_CONFIG)

/**
 * fw_config_get() - Provide firmware configuration value.
 *
 * Return 64-bit firmware configuration value determined for the system,
 * including any runtime modifications from fw_config_mainboard_override().
 */
uint64_t fw_config_get(void);

/**
 * fw_config_get_field() - Provide firmware configuration field value.
 * @field: Structure containing field name and mask
 *
 * Return 64bit firmware configuration value determined for the system.
 * Will return UNDEFINED_FW_CONFIG if undefined, caller should treat
 * as error value for the case.
 */
uint64_t fw_config_get_field(const struct fw_config_field *field);

/**
 * fw_config_override_field() - Override a field in the cached fw_config value.
 * @field: The field to override.
 * @value: The new value for the field.
 *
 * Must only be called from fw_config_mainboard_override(). Updates the field
 * in the cached fw_config value and records field->mask in the override mask.
 */
void fw_config_override_field(const struct fw_config_field *field,
			      uint64_t value);

/**
 * fw_config_get_override_mask() - Provide mask of overridden fw_config bits.
 *
 * Return 64-bit bitmask of fw_config fields that were overridden at runtime
 * by fw_config_mainboard_override().
 */
uint64_t fw_config_get_override_mask(void);

/**
 * fw_config_probe() - Check if field and option matches.
 * @match: Structure containing field and option to probe.
 *
 * Return %true if match is found, %false if match is not found.
 */
bool fw_config_probe(const struct fw_config *match);

/**
 * fw_config_for_each_found() - Call a callback for each fw_config field found
 * @cb: The callback function
 * @arg: A context argument that is passed to the callback
 */
void fw_config_for_each_found(void (*cb)(const struct fw_config *config, void *arg), void *arg);

/**
 * fw_config_get_found() - Return a pointer to the fw_config struct for a given field.
 * @field_mask: A field mask from static.h, e.g., FW_CONFIG_FIELD_FEATURE_MASK
 *
 * Return pointer to cached `struct fw_config` if successfully probed, otherwise NULL.
*/
const struct fw_config *fw_config_get_found(uint64_t field_mask);

/**
 * fw_config_probe_dev() - Check if any of the probe conditions are true for given device.
 * @dev: Device for which probe conditions are checked
 * @matching_probe: If any probe condition match, then the matching probe condition is returned
 * to the caller.
 * Return %true if device has no probing conditions or if a matching probe condition is
 * encountered, %false otherwise.
 */
bool fw_config_probe_dev(const struct device *dev, const struct fw_config **matching_probe);

#else

static inline uint64_t fw_config_get(void)
{
	return UNDEFINED_FW_CONFIG;
}

static inline void fw_config_override_field(const struct fw_config_field *field,
					    uint64_t value)
{
}

static inline uint64_t fw_config_get_override_mask(void)
{
	return 0;
}

static inline bool fw_config_probe(const struct fw_config *match)
{
	/* Always return true when probing with disabled fw_config. */
	return true;
}

static inline bool fw_config_probe_dev(const struct device *dev,
				       const struct fw_config **matching_probe)
{
	/* Always return true when probing with disabled fw_config. */
	if (matching_probe)
		*matching_probe = NULL;
	return true;
}

static inline uint64_t fw_config_get_field(const struct fw_config_field *field)
{
	/* Always return UNDEFINED_FW_CONFIG when get with disabled fw_config. */
	return UNDEFINED_FW_CONFIG;
}

#endif /* CONFIG(FW_CONFIG) */

/**
 * fw_config_is_provisioned() - Determine if FW_CONFIG has been provisioned.
 *
 * Return %true if the underlying FW_CONFIG source (CBI, CBFS, or VPD) has been
 * provisioned, %false otherwise (ignoring any runtime field overrides from
 * fw_config_mainboard_override()).
 */
static inline bool fw_config_is_provisioned(void)
{
	return (fw_config_get() & ~fw_config_get_override_mask()) !=
	       (UNDEFINED_FW_CONFIG & ~fw_config_get_override_mask());
}

#endif /* __FW_CONFIG__ */
