/* SPDX-License-Identifier: GPL-2.0-only */

#include <console/console.h>
#include <soc/pmic.h>
#include <soc/qcom_spmi.h>
#include <types.h>

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

/*
 * is_pon_on_ac - Checks if the system was powered on by an AC/cable insertion event.
 *
 * This function reads and parses the PMIC Power-On (PON) history log to determine
 * the specific cause of the system power-up. It specifically looks for the
 * PON_CBLPWR_RSN trigger, which indicates that external power (AC/charging cable)
 * was the reason for the boot sequence initiation.
 *
 * @return true if AC/Cable Power was the PON reason, false otherwise.
 */
bool is_pon_on_ac(void)
{
	uint16_t data, data2;
	uint32_t i;
	int8_t current_index = -1;
	uint8_t *pon_hist_curr_addr;
	uint8_t pon_hist_raw[PON_EVENT_TOTAL_LOG_AREA_SIZE] = {0};

	printk(BIOS_INFO, "PON: Show power on reason -\n");
	if (pm_pon_read_pon_hist(pon_hist_raw))
		return false;

	pon_hist_curr_addr = pon_hist_raw;

	for (i = 0; i < PON_EVENT_LOG_AREA_SIZE - 2; i += 4) {
		if (current_index >= PON_EVENT_PARSE_LIMIT) {
			pon_hist_curr_addr += 4;
			continue;
		}
		data = ((uint16_t)(*(pon_hist_curr_addr + 1)) << 8) | *pon_hist_curr_addr;

		if (BEGIN_PON  == *(pon_hist_curr_addr + 2)) {
			pm_pon_get_pon_event(BEGIN_PON, pon_hist_curr_addr, (PON_EVENT_LOG_AREA_SIZE - 2) - (uint8_t)(pon_hist_curr_addr - pon_hist_raw), &data2);
			current_index += 1;
		} else if (PM_PON_EVENT_PON_TRIGGER == *(pon_hist_curr_addr + 2)) {
			switch (data) { /* SID<<12|PID<<4|IRQ */
			case PON_CBLPWR_RSN:
				printk(BIOS_INFO, " PON Reason : cblpwr\n");
				return true;
			default:
				printk(BIOS_INFO, " PON Reason : %d\n", data);
				return false;
			}
		} else if (PM_PON_EVENT_RESET_TYPE == *(pon_hist_curr_addr + 2)) {
			printk(BIOS_INFO, " Reset Reason : %d\n", data & 0xFF);
			return false;
		} else if (PM_PON_EVENT_FUNDAMENTAL_RESET == *(pon_hist_curr_addr + 2)) {
			printk(BIOS_INFO, " POFF Reason : %d\n", data & PON_RAW_XVDD_RB_MASK);
			return false;
		}
		pon_hist_curr_addr += 4;
	}
	printk(BIOS_INFO, " Unable to detect PON reason.\n");
	return false;
}
