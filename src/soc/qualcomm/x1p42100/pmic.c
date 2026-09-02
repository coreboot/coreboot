/* SPDX-License-Identifier: GPL-2.0-only */

#include <console/console.h>
#include <soc/pmic.h>
#include <soc/qcom_spmi.h>
#include <types.h>

#define WARM_RESET	1
#define PON_VALUE_INVALID	0xFFFF

/*
 * pm_pon_get_pon_event - Search for a specific event type in the PON log.
 * @event: The event type to search for.
 * @pon_hist_log: Pointer to the PON history log buffer.
 * @pon_hist_raw_size: Total size of the raw log buffer.
 * @return_data: Pointer to store the associated event data.
 *
 * This helper function iterates through the PON log buffer and searches for an
 * entry matching the specified event type (e.g., PM_PON_EVENT_PON_TRIGGER).
 * If found, it extracts the associated 16-bit data field (which usually contains
 * the power-on reason or status) and returns it via return_data.
 *
 * @return 0 on success, -1 on error.
 */
static int pm_pon_get_pon_event(uint8_t event, uint8_t *pon_hist_log, uint32_t pon_hist_raw_size, uint16_t *return_data)
{
	bool event_found = false;
	uint8_t data0;
	uint8_t data1;
	uint32_t i;

	if (!pon_hist_log || !return_data)
		return -1;

	for (i = 0; i < pon_hist_raw_size; i += 4) {
		if (event == *(pon_hist_log + 2)) {
			data0 = *pon_hist_log;
			data1 = *(pon_hist_log + 1);
			event_found = true;
			break;
		}
		pon_hist_log += 4;
	}

	if (event_found == false)
		*return_data = 0;
	else
		*return_data = ((uint16_t) data1 << 8) | data0;

	return 0;
}

/*
 * pm_pon_read_pon_hist - Read, reorder, and prepare the PMIC Power-On history log.
 * @pon_hist_raw: Buffer to store the resulting PON history log. The buffer size
 * must be at least PON_EVENT_TOTAL_LOG_AREA_SIZE bytes.
 *
 * This function handles the low-level logic to read the PON event data from
 * the PMIC's SDAM registers (which function as a circular buffer) and reorders
 * the data in the output buffer (pon_hist_raw) so that the most recent event
 * entry is placed at the beginning (index 0).
 *
 * @return 0 on success, -1 on failure (e.g., if pon_hist_raw is NULL or SPMI read fails).
 */
int pm_pon_read_pon_hist(uint8_t *pon_hist_raw)
{
	int status;
	uint8_t sdam_count;
	uint8_t enqueue_sdam_num;
	uint32_t i;
	uint32_t read_size;
	uint32_t push_ptr;
	uint8_t enqueue_addr;
	uint8_t temp;

	if (!pon_hist_raw)
		return -1;

	enqueue_addr = spmi_read8(SPMI_ADDR(PMIC_SLAVE_ID, PM_PON_ENQUEUE_ADDR));
	sdam_count = spmi_read8(SPMI_ADDR(PMIC_SLAVE_ID, PM_PON_SDAM_COUNT_ADDR));
	enqueue_sdam_num = spmi_read8(SPMI_ADDR(PMIC_SLAVE_ID, PM_PON_ENQUEUE_SDAM_NUM));

	/* if sdam_count == 1, each SDAM contains half of the total size
	 * otherwise, the SDAM stores the whole size
	 */
	status = spmi_read_bytes(SPMI_ADDR(PMIC_SLAVE_ID, PM_PON_LOGGING_AREA_START), pon_hist_raw, PON_EVENT_LOG_AREA_SIZE);

	if (sdam_count != 0) {
		/* 0: Only used 1 SDAM
		 * 1: Used 2 SDAM
		 * Currently only extend to 2 continuous SDAM.
		 */
		status |= spmi_read_bytes(SPMI_ADDR(PMIC_SLAVE_ID, (PM_PON_LOGGING_AREA_START + 0x100)), pon_hist_raw + PON_EVENT_LOG_AREA_SIZE, PON_EVENT_LOG_AREA_SIZE);
	}

	push_ptr = enqueue_addr;
	push_ptr = PM_PON_PUSH_PTR_INDEX(push_ptr) + (enqueue_sdam_num * PON_EVENT_LOG_AREA_SIZE);

	if (status != 0 || push_ptr >= PON_EVENT_TOTAL_LOG_AREA_SIZE)
		return -1;

	read_size = push_ptr/2;

	/* Reverse the Buffer to start from latest event */
	for (i = 0; i < read_size; i++) {
		temp = pon_hist_raw[push_ptr - i - 1];
		pon_hist_raw[push_ptr - i - 1] = pon_hist_raw[i];
		pon_hist_raw[i] = temp;
	}

	read_size = (PON_EVENT_TOTAL_LOG_AREA_SIZE - push_ptr) / 2;

	for (i = 0; i < read_size; i++) {
		temp = pon_hist_raw[PON_EVENT_TOTAL_LOG_AREA_SIZE - i - 1];
		if (push_ptr + i < PON_EVENT_TOTAL_LOG_AREA_SIZE) {
			pon_hist_raw[PON_EVENT_TOTAL_LOG_AREA_SIZE - i - 1] =  pon_hist_raw[push_ptr + i];
			pon_hist_raw[push_ptr + i] = temp;
		}
	}

	return status;
}

struct pon_history_status {
	uint16_t pon_reason;
	uint16_t reset_type;
	uint16_t poff_reason;
};

static bool parse_pon_history(struct pon_history_status *status)
{
	uint16_t data, data2;
	uint32_t i;
	int8_t current_index = -1;
	uint8_t *pon_hist_curr_addr;
	uint8_t pon_hist_raw[PON_EVENT_TOTAL_LOG_AREA_SIZE] = {0};

	/* Initialize to invalid sentinel so we know which events were actually logged */
	status->pon_reason = PON_VALUE_INVALID;
	status->reset_type = PON_VALUE_INVALID;
	status->poff_reason = PON_VALUE_INVALID;

	printk(BIOS_INFO, "PON: Show power on reason -\n");
	if (pm_pon_read_pon_hist(pon_hist_raw)) {
		printk(BIOS_ERR, "PON: Failed to read PON history\n");
		return false;
	}
	pon_hist_curr_addr = pon_hist_raw;
	for (i = 0; i < PON_EVENT_LOG_AREA_SIZE - 2; i += 4) {
		if (current_index >= PON_EVENT_PARSE_LIMIT) {
			pon_hist_curr_addr += 4;
			continue;
		}
		data = ((uint16_t)(*(pon_hist_curr_addr + 1)) << 8) | *pon_hist_curr_addr;
		uint8_t event_type = *(pon_hist_curr_addr + 2);
		if (event_type == BEGIN_PON) {
			pm_pon_get_pon_event(BEGIN_PON, pon_hist_curr_addr,
					     (PON_EVENT_LOG_AREA_SIZE - 2) -
					     (uint8_t)(pon_hist_curr_addr - pon_hist_raw),
					     &data2);
			current_index += 1;
		} else if (event_type == PM_PON_EVENT_PON_TRIGGER) {
			status->pon_reason = data;
			if (data == PON_CBLPWR_RSN)
				printk(BIOS_INFO, " PON Reason : cblpwr\n");
			else
				printk(BIOS_INFO, " PON Reason : %d\n", data);
		} else if (event_type == PM_PON_EVENT_RESET_TYPE) {
			status->reset_type = data & 0xFF;
			printk(BIOS_INFO, " Reset Reason : %d\n", status->reset_type);
		} else if (event_type == PM_PON_EVENT_FUNDAMENTAL_RESET) {
			status->poff_reason = data & PON_RAW_XVDD_RB_MASK;
			printk(BIOS_INFO, " POFF Reason : %d\n", status->poff_reason);
		}
		pon_hist_curr_addr += 4;
	}

	if (status->pon_reason == PON_VALUE_INVALID &&
	    status->reset_type == PON_VALUE_INVALID)
		printk(BIOS_INFO, " Unable to detect PON reason / reset type.\n");

	return true;
}
/* Cached status to avoid repeatedly reading the PMIC buffer over SPMI/I2C */
static struct pon_history_status cached_pon;
static bool pon_cached;

static const struct pon_history_status *get_pon_status(void)
{
	if (!pon_cached) {
		parse_pon_history(&cached_pon);
		pon_cached = true;
	}

	return &cached_pon;
}
/*
 * get_pon_reason - Retrieve the PMIC Power-On Reason (Trigger)
 *
 * Return: PON reason (e.g. PON_CBLPWR_RSN) on success, or negative error.
 */
static int get_pon_reason(void)
{
	const struct pon_history_status *status = get_pon_status();

	if (!status)
		return -1;

	return (int)status->pon_reason;
}

/*
 * get_reset_type - Retrieve the PMIC Reset Type
 *
 * Return: Reset type (0-255) on success, or negative error if not present.
 */
static int get_reset_type(void)
{
	const struct pon_history_status *status = get_pon_status();

	if (!status)
		return -1;

	return (int)status->reset_type;
}

/*
 * is_pon_on_ac - Check if system powered on via AC charger insertion
 *
 * This function reads and parses the PMIC Power-On (PON) history log to determine
 * the specific cause of the system power-up. It specifically looks for the
 * PON_CBLPWR_RSN trigger, which indicates that external power (AC/charging cable)
 * was the reason for the boot sequence initiation.
 *
 * Return: true if boot trigger was cable power, false otherwise.
 */
bool is_pon_on_ac(void)
{
	return get_pon_reason() == PON_CBLPWR_RSN;
}

/*
 * is_reset_type_warm - Check if the system rebooted via warm reset.
 *
 * Inspects the PMIC PON history log to check whether the boot sequence was
 * initiated by a warm reset event (WARM_RESET).
 *
 * Return: true if reset type is WARM_RESET, false otherwise (e.g. cold boot
 *         or error reading PMIC history).
 */
bool is_reset_type_warm(void)
{
	return get_reset_type() == WARM_RESET;
}
